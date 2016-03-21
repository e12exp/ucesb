# -*- Makefile -*-

EXT_HBOOK_WRITER=$(UCESB_BASE_DIR)/hbook/hbook_writer
EXT_ROOT_WRITER=$(UCESB_BASE_DIR)/hbook/root_writer
EXT_STRUCT_WRITER=$(UCESB_BASE_DIR)/hbook/struct_writer

EXT_WRITERS=

#########################################################

# We only depend on (and compile) for cernlib, if we can find/set CERN_ROOT

ifndef NO_CERNLIB
ifndef CERN_ROOT
# Try to localise cernlib on our own
CERNLIB_CERN_ROOT=$(shell which cernlib | sed -e "s,/bin/cernlib,," | grep -v cernlib)
ifneq (,$(CERNLIB_CERN_ROOT))
ifneq (,$(shell (test -e $(CERNLIB_CERN_ROOT)/include/cfortran/packlib.h || test -e $(CERNLIB_CERN_ROOT)/include/packlib.h) && echo exists))
CERN_ROOT=$(CERNLIB_CERN_ROOT)
endif
endif
endif

ifdef CERN_ROOT
EXT_WRITERS += $(EXT_HBOOK_WRITER)
-include $(UCESB_BASE_DIR)/hbook/hbook_writer.base.dep
endif
endif

#########################################################

FILE_ROOT_CONFIG:=$(shell which root-config 2> /dev/null)
ifeq (,$(FILE_ROOT_CONFIG))
FILE_ROOT_CONFIG=$(shell which $(ROOTSYS)/bin/root-config 2> /dev/null)
endif

ifneq (,$(FILE_ROOT_CONFIG))
EXT_WRITERS += $(EXT_ROOT_WRITER)
-include $(UCESB_BASE_DIR)/hbook/root_writer.base.dep
endif

#########################################################

EXT_WRITERS += $(EXT_STRUCT_WRITER)
-include $(UCESB_BASE_DIR)/hbook/struct_writer.base.dep

#########################################################

$(EXT_HBOOK_WRITER):
	@$(MAKE) -C $(UCESB_BASE_DIR)/hbook hbook_writer

$(EXT_ROOT_WRITER):
	@$(MAKE) -C $(UCESB_BASE_DIR)/hbook root_writer

$(EXT_STRUCT_WRITER):
	@$(MAKE) -C $(UCESB_BASE_DIR)/hbook struct_writer





