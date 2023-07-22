
#include <random>    // mt19937_64, uniform_real_distribution
#include <vector>    // vector
#include <set>       // set
#include <cassert>   // assert
#include <algorithm> // shuffle

#include "Parameters.h"

enum SIRSTATE {
  SUSCEPTIBLE,
  INFECTED,
  REMOVED,
  TREATED
};

enum EVENT {
  VACCINATE, // vaccination event
  SHUFFLE,   // assign initial population identity
  TRANSMIT,  // transmission event
  TREAT,     // treatment event
  RECOVER    // recovery event
};

using namespace std;

// hash-based matching => reseed
void hbm (
  mt19937_64 &prng, const unsigned int &salt,
  const EVENT &event, const unsigned int &time,
  const unsigned int &to, const unsigned int &from
) {
  seed_seq sseq{ salt, static_cast<unsigned int>(event), time, to, from };
  prng.seed(sseq);
}

// convenience for single-id HBM
void hbm (
  mt19937_64 &prng, const unsigned int &salt, const EVENT &event
) {
  hbm(prng, salt, event, 0, 0, 0);
}

// convenience for event-only HBM
void hbm (
  mt19937_64 &prng, const unsigned int &salt, const EVENT &event,
  const unsigned int &time, const unsigned int &to
) {
  hbm(prng, salt, event, time, to, 0);
}

// yields a sample of incidence time series
vector<vector<unsigned int>> sampler (
    mt19937_64 &prng,                   // the PRNG - if NOT matching, already seeded
    const unsigned int n,               // the number of samples
    const unsigned int maxtime,         // the simulation duration
    const unsigned int population,      // the population size
    const Parameters &parameters,       // the simulation parameters
    const vector<unsigned int> prngref, // the PRNG reference; empty if not matching
    const bool & HBM // if true, use HBM - prngref => salt; if false AND prngref, seed matching
) {

    auto result = vector<vector<unsigned int>>(n, vector<unsigned int>(maxtime, 0));
    if (prngref.size() == 0) { // if not matching: assume seed set externally
        prngref.resize(n, 0); // set all the seeds to 0 => they will be ignored
        assert(!HBM);
    }

    // sampling loop
    for (unsigned int i = 0; i < n; i++) {
        auto s = prngref[i];
        // if matching - set the prng state, whether HBM or not
        if (s != 0) { prng.seed(s); } 
        // invoke the stepper; if HBM, pass the salt otherwise 0
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

    auto incidence = vector<unsigned int>(maxtime, 0);

    auto pop = vector<SIRSTATE>(population, SIRSTATE::SUSCEPTIBLE);
    uniform_real_distribution<> u01();

    // TODO check if K et al allow initial vax to prevent infection

    // if vaccinating, vaccinate
    // n.b. this is always the first n draws - no need to HBM, since prng already seed matched
    if (parameters.vaccination_p > 0) {
        for (auto i = 0; i < population; i++) {
            if (u01(prng) < parameters.vaccination_p) {
                pop[i] = SIRSTATE::REMOVED;
            }
        }
    }

    for (auto i = 0; i < parameters.i0; i++) {
        if (pop[i] == SIRSTATE::SUSCEPTIBLE) pop[i] = SIRSTATE::INFECTED;
    }
 
    // TODO: check if K et al shuffle i0
    // however, if HBM, we need to reseed to ensure matched shuffle of i0 between vax vs not
    if (salt != 0) { hbm(prng, salt, SHUFFLE); }
    shuffle(pop.begin(), pop.end(), prng);

    for (auto tm = 0; tm < maxtime; tm ++) {
        incidence[tm] = step(prng, tm, pop, parameters, salt);
    }

    return incidence;

};

// yields the incidence at a given time
unsigned int step (
  mt19937_64 &prng,                // the PRNG
  const unsigned int &time, // current time step
  vector<SIRSTATE> &state,  // current population state; side effected
  const Parameters &parameters, // simulation parameters
  const unsigned int &salt   // if HBM, the salt
) {
    return salt != 0 ? 
        step_HBM(prng, time, state, parameters, salt) :
        step_NON(prng, time, state, parameters);
};

// yields the incidence at a given time
unsigned int step_HBM (
  mt19937_64 &prng,                // the PRNG
  const unsigned int &time, // current time step
  vector<SIRSTATE> &state,  // current population state; side effected
  const Parameters &parameters, // simulation parameters
  const unsigned int &salt   // if HBM, the salt
) {

    uniform_real_distribution<> u01();
    set<unsigned int> infectees;

    hbm(prng, salt, TRANSMIT, time);
    auto psalt = prng();

    for (auto i = 0; i < state.size(); i++) if (not infectees.count(i)) {
        if (state[i] == SIRSTATE::INFECTED || state[i] == SIRSTATE::TREATED) {
            if (u01(prng) < (state[i] == SIRSTATE::INFECTED ? parameters.recovery_p : parameters.treatrec_p)) {
                state[i] = SIRSTATE::REMOVED;
            }
            for (auto j = 0; j < state.size(); j++) if (not infectees.count(j)) {
                if (i != j && state[j] == SIRSTATE::SUSCEPTIBLE) {
                    hbm(prng, psalt, j);
                    if (u01(prng) < parameters.transmission_p) {
                        infectees.insert(j);
                    }
                }
            }
        }
    }

    if (parameters.treatment_p > 0) {
        hbm(prng, salt, TREAT, time);
        psalt = prng();
        for (auto i : infectees) {
            hbm(prng, psalt, i);
            state[i] = (u01(prng) < parameters.treatment_p) ? SIRSTATE::TREATED ? SIRSTATE::INFECTED;
        }
    } else {
        for (auto i : infectees) {
            state[i] = SIRSTATE::INFECTED;
        }
    }

    return infectees.size();
};

// yields the incidence at a given time
unsigned int step_NON (
  mt19937_64 &prng,                // the PRNG
  const unsigned int &time, // current time step
  vector<SIRSTATE> &state,  // current population state; side effected
  const Parameters &parameters
) {
    auto result = 0;
    uniform_real_distribution<> u01();
    set<unsigned int> infectees;

    for (auto i = 0; i < state.size(); i++) if (not infectees.count(i)) {
        if (state[i] == SIRSTATE::INFECTED || state[i] == SIRSTATE::TREATED) {
            if (u01(prng) < (state[i] == SIRSTATE::INFECTED ? parameters.recovery_p : parameters.treatrec_p)) {
                state[i] = SIRSTATE::REMOVED;
            }
            for (auto j = 0; j < state.size(); j++) if (not infectees.count(j)) {
                if (i != j && state[j] == SIRSTATE::SUSCEPTIBLE) {
                    if (u01(prng) < parameters.transmission_p) {
                        infectees.insert(j);
                    }
                }
            }
        }
    }

    if (parameters.treatment_p > 0) {
        for (auto i : infectees) {
            if (u01(prng) < parameters.treatment_p) {
                state[i] = SIRSTATE::TREATED;
            } else {
                state[i] = SIRSTATE::INFECTED;
            }
        }
    } else {
        for (auto i : infectees) {
            state[i] = SIRSTATE::INFECTED;
        }
    }

    return infectees.size();
};