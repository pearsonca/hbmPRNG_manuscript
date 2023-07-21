
#include <random>
#include <vector>

// use seed_seq for hashing?

enum SIRTYPE {
  SUSCEPTIBLE,
  INFECTED,
  REMOVED,
  TREATED
};

enum EVENT {
  SHUFFLE,  // assign initial population identity
  TRANSMIT, // transmission event
  TREAT,    // treatment event
  RECOVER   // recovery event
};

struct Parameters {
    double transmission_p; // transmission probability per pair per time step
    double recovery_p;     // recovery probability per individual per time step
    double treatment_p;    // treatment probability per infection
    double treatrec_p;     // recovery probability per treated individual per time step
    double vaccination_p;  // vaccination probability per individual at outset
    unsigned int i0 = 5;
};

using namespace std;

// TODO IMPLEMENT
void hbm (
  mt19937_64 &prng, const unsigned int &salt,
  const EVENT &event, const unsigned int &time,
  const unsigned int &to, const unsigned int &from
) {
    prng({ salt, event, time, to, from });
}

void hbm (
  mt19937_64 &prng, const unsigned int &salt,
  const EVENT &event
) {
    hbm(prng, salt, event, 0, 0, 0);
}

// yields a sample of incidence time series
vector<vector<unsigned int>> sampler (
    mt19937_64 &prng,                   // the PRNG
    const unsigned int n,               // the number of samples
    const unsigned int maxtime,         // the simulation duration
    const unsigned int population,      // the population size
    const Parameters &parameters,       // the simulation parameters
    const vector<unsigned int> prngref, // the PRNG reference; if empty, unmatched
    const bool & HBM // if true, use HBM - prngref => salt; if false AND prngref, seed matching
) {

    auto result = vector<vector<unsigned int>>(n, vector<unsigned int>(maxtime, 0));
    if (prngref.size() == 0) { // if not matching: assume seed set externally
        prngref.resize(n, 0);
        assert(!HBM);
    }

    for (unsigned int i = 0; i < n; i++) {
        auto s = prngref[i];
        if (s != 0) { // if matching
            prng(s);
        } 
        result[i] = stepper(prng, maxtime, population, parameters, (HBM ? s : 0));
    }

    return result;

};

// yields a time series of incidence
vector<unsigned int> stepper (
    mt19937_64 &prng,                 // the PRNG
    const unsigned int maxtime,       // the simulation duration
    const unsigned int population,    // the population size
    const Parameters &parameters, // the simulation parameters
    const unsigned int salt           // if != 0, using HBM
) {

    auto result = vector<unsigned int>(maxtime, 0);

    auto pop = vector<SIRTYPE>(population, SIRTYPE::SUSCEPTIBLE);
    uniform_real_distribution<> u01();

    // TODO check if K et al allow initial vax to prevent infection

    // if vaccinating, vaccinate
    // n.b. this is always the first n draws - no need to HBM, since prng already seed matched
    if (parameters.vaccination_p > 0) {
        for (auto i = 0; i < population; i++) {
            if (u01(prng()) < parameters.vaccination_p) {
                pop[i] = SIRTYPE::REMOVED;
            }
        }
    }

    for (auto i = 0; i < parameters.i0; i++) {
        if (pop[i] == SIRTYPE::SUSCEPTIBLE) pop[i] = SIRTYPE::INFECTED;
    }
 
    // TODO: check if K et al shuffle i0
    // however, if HBM, we need to reseed to ensure matched shuffle of i0 between vax vs not
    if (salt != 0) { hbm(prng, salt, SHUFFLE); }
    shuffle(pop.begin(), pop.end(), prng);

    for (auto tm = 0; tm < maxtime; tm ++) {
        result[tm] = step(prng, tm, pop, parameters, salt);
    }

    return result;

};

// yields the incidence at a given time
unsigned int step (
  mt19937_64 &prng,                // the PRNG
  const unsigned int &time, // current time step
  vector<SIRTYPE> &state,  // current population state; side effected
  const Parameters &parameters, // simulation parameters
  const option<unsigned int> salt   // if HBM, the salt
) {
    auto result = 0;

    for (auto i = 0; i < state.size(); i++) {
        if (state[i] == SIRTYPE::INFECTED) {
            if (salt != 0) { hbm(prng, salt, TRANSMIT, time, i, 0); }
            for (auto j = 0; j < state.size(); j++) {
                if (i != j && state[j] == SIRTYPE::SUSCEPTIBLE) {
                    if (prng() < parameters.transmission_p) {
                        state[j] = SIRTYPE::INFECTED;
                        result++;
                    }
                }
            }
        }
    }

    return result;
};
