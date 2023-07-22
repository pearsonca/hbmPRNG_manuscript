
#ifndef PARAMETERS_H

struct Parameters {
  double transmission_p; // transmission probability per pair per time step
  double recovery_p;     // recovery probability per individual per time step
  double treatment_p;    // treatment probability per infection
  double treatrec_p;     // recovery probability per treated individual per time step
  double vaccination_p;  // vaccination probability per individual at outset
  unsigned int i0 = 5;   // initial number of infections
};

// TODO also declare all the scenarios here?

#endif  // PARAMETERS_H