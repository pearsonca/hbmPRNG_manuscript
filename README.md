# Overview

This repository provides the analysis associated with TODO-PREPRINT-DOI. Briefly, this work addresses matching stochastic simulations for counterfactual comparison. Using computational benchmarking and resulting estimate resolution, we compare (1) no matching, (2) seed matching only, (3) Kaminsky et al's proposed approach, and (4) an approach based on hash-encoding. Our demonstration model concerns a Susceptible-Infectious-Removed transmission system, with distinct interventions to reduce transmission probability, reduce infectious duration, and reduce the susceptible population.

# Quick Start

Ensure R, make, git, and C/C++ toolchain are available. Warning: this will install R packages when not available at sufficient version.

```
make setup      # will take care of e.g. syncing subrepository to K et al approach, building all sources
make simulate   # long running; writes approximately XYZ Gb
make analyze    # collate all the results from simulate
make illustrate # generate the analysis figures
```

# Organization

This repository is organized with a [`make`](https://www.gnu.org/software/make/manual/html_node/index.html) workflow. The single entry point to overall workflow is the `Makefile` at the root directory. Sub-step workflow `make` files are in `makefiles/`.

The source for the previously proposed approach by [Kaminsky et al](https://doi.org/10.1098/rstb.2018.0279) is contained via git submodule in `cfepi/`.

The simulation source for this reiteration of the model is contained in `src/`, as is the hashing function used XYZ, a specialization of the XYZ code included in the [SMHasher3](https://gitlab.com/fwojcik/smhasher3) hash-function library and testing suite.

Results parsing and figure generation are accomplished with R, using [`data.table`](https://rdatatable.gitlab.io/data.table/) and [`ggplot2`](https://ggplot2.tidyverse.org/). All R scripts are in the `R/` directory, subdivided by `R/extract`, `R/transform`, and `R/visualize`.

Intermediate and final outputs are collected into an output folder (by default, `output/` in the root project directory - adjustable with a `local.makefile` or command line argument; see `makefiles/setup.makefile` comments), subdivided into compiled code (`build/`), simulation results (`simres/`), extracted-and-transformed results (`digest/`), and visualizations (`figure/`). Simulation results are generally plaintext, comma-separated value files. The ET'd files are generally R serialization binary files. Figures are portable network graphics (png) format by default (adjustable with a `local.makefile` or command line argument; see `makefiles/visualize.makefile` comments).

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
