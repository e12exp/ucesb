# This makefile is needed when you make a copy of this directory
# (hacky/) and use it outside the unpacker/ structure (where the
# Makefile is within the unpacker/ directory itself.

#########################################################

# In case of problems compiling the parsers, ask them to be generated
# with bison instead of yacc (trouble on redhat / bsds) (uncomment
# below)

#YACC=bison
#export YACC

#########################################################

# If you want to always enforce the inclusion of the ntuple-generation
# and the watcher (daqscope) facility, you may use the following
# declarations.  (otherwise, they can also be done on the command line,
# prior to invoking make)

#USE_CERNLIB=1
#USE_CURSES=1

#export USE_CERNLIB
#export USE_CURSES

#########################################################

# We still need access to the unpacker/ directory, since the ucesb and
# psdc programs are there, and since the common source also is there

TREE_OR_HOME_SRC_DIR = $(firstword $(wildcard $(addsuffix /$(1),$(shell pwd)/.. $(HOME))))

UCESB_BASE_DIR=$(call TREE_OR_HOME_SRC_DIR,unpacker)
export UCESB_BASE_DIR

#########################################################

#CXX=g++-3.2
export CXX

#########################################################

# Our target, rename as needed

all: hacky

#########################################################
# Submakefiles that the programs depend on

include $(UCESB_BASE_DIR)/makefile_ucesb.inc
include $(UCESB_BASE_DIR)/makefile_psdc.inc

DEPENDENCIES=$(UCESB) $(PSDC)

#########################################################

.PHONY: hacky
hacky: $(DEPENDENCIES)
	@$(MAKE) -f $(UCESB_BASE_DIR)/makefile_unpacker.inc UNPACKER=$@ 

#########################################################

clean:
	$(MAKE) -f $(UCESB_BASE_DIR)/makefile_unpacker.inc UNPACKER=hacky clean

all-clean: clean
	rm -rf gen

