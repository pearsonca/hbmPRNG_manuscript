
# if you add a local.makefile to e.g. override the root output dir,
# that file will be included; otherwise, this quietly continues
-include local.makefile

# these variables define the filestructure for recording results
# n.b.: these are relative to the overall project root, not *this* directory
REFDIR ?= output
RUNDIR := $(REFDIR)/simres
OUTDIR := $(REFDIR)/digest
FIGDIR := $(REFDIR)/figure

setup: cfepi/package $(RUNDIR) $(OUTDIR) $(FIGDIR)
	@echo "Setup task:"
	@echo "Ensured 'cfepi' available."
	@echo "Ensured folder structure created:"
	@echo "  - $(RUNDIR): where simulation runs data will be collected"
	@echo "  - $(OUTDIR): where run data will be consolidated"
	@echo "  - $(FIGDIR): where figure data will be stored"

cfepi/package: .FORCE
	@echo "If necessary, initializing/updating submodule 'cfepi' ..."
	git submodule update --init --recursive

$(RUNDIR) $(OUTDIR) $(FIGDIR):
	@echo "Creating $@ directory ..."
	@mkdir -p $@

# convenience definitions

# targets that depend on this will always be attempted
.FORCE:

# this is a lazily defined operation to run `Rscript` on all dependencies ($^),
# optionally arguments to the call invocation, and the target ($@)
# use as `$(call R)` (no optional arguments) or `$(call R extra args)`
# n.b.: the extra-args-version, commas are NOT permitted
R = $(strip Rscript $^ $(1) $@)
