# Overview

This repository provides the analysis associated with TODO-PREPRINT-DOI. Briefly, this work addresses matching stochastic simulations for counterfactual comparison. Using computational benchmarking and resulting estimate resolution, we compare (1) no matching, (2) seed matching only, (3) Kaminsky et al's proposed approach, and (4) an approach based on hash-encoding. Our demonstration model concerns a Susceptible-Infectious-Removed transmission system, with distinct interventions to reduce transmission probability, reduce infectious duration, and reduce the susceptible population.

# Quick Start

Ensure R, make, and C/C++ toolchain are available. Warning: this will install R packages when not available at sufficient version.

```
make setup      # will take care of e.g. syncing subrepository to K et al approach, building all sources
make simulate   # long running; writes approximately XYZ Gb
make analyze    # collate all the results from simulate
make illustrate # generate the analysis figures
```

# Organization

TODO, file structure, naming conventions

# Setup

TODO, sub-part explanation of the steps in the `setup` target

# Simulate

TODO, sub-part explanation of the steps in the `simulate` target

# Analyze

TODO, sub-part explanation of the steps in the `analyze` target

# Illustrate

TODO, sub-part explanation of the steps in the `analyze` target.

# License

See [License](LICENSE).

# Reporting Errors

If you have problems reproducing this analysis, please raise an issue against the repository. We will not consider formal changes to *this analysis* - it will remain what was done. We will consider implementing / incorporating software engineering changes (e.g. to address some platform-related issue reproducing the analysis); any such updates will be tagged as a post-publication version.

We will also consider *new work* based on / extending / in response to / etc this analysis, but that work would be done in a new venue.

See our Contributing (TODO) file for details.
