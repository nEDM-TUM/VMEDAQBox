include ../buildTools/config.mk

SHLIB := ../lib/pycdtware.$(SHREXT)

include ../buildTools/BasicMakefile

INCLUDEFLAGS += $(PYTHONINCLUDEFLAGS) -I../Cascade
LIBFLAGS += -lboost_python -lboost_system -lboost_thread $(PYTHONLIBFLAGS) 
