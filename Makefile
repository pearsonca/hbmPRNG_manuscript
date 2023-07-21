
default: setup simulate analyze visualize

# defines "setup" target
#  - get relevant code / tools
#  - setup filesystem folder structure
#  - also defines path / configuration variables
include makefiles/setup.makefile

# defines "simulate" target
# run & time:
#  - K et al approach
#  - this approach
#  - unmatched approach
#  - seed-matched-only approach
include makefiles/simulate.makefile

# defines "analyze" target
#  - consolidate simulation results
#  - compute matched comparisons
include makefiles/analyze.makefile

# defines "visualize"
# produces the figures
include makefiles/visualize.makefile
