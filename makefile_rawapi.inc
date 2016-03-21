# -*- Makefile -*-

# Provides RAWAPI_DIR
include $(UCESB_BASE_DIR)/makefile_hasrawapi.inc

RAWAPI_CFLAGS=-D Linux -D SYS_UNIX

CXXFLAGS += -I$(RAWAPI_DIR)/inc

RAWAPI_USE_LIB=1

ifeq ($(RAWAPI_USE_LIB),1)
###################################################
# When using libraries

DEFAULT_SYSTEM_64:=$(shell cpp -dM /dev/null | grep __amd64__)

ifneq (,$(DEFAULT_SYSTEM_64))
CXXLIBS += -L $(RAWAPI_DIR)/Linux64 -lrawapiclin64 -lrawservn64
else
CXXLIBS += -L $(RAWAPI_DIR)/Linux -lrawapiclin -lrawservn
endif

###################################################
else # !RAWAPI_USE_LIB

RAWAPI_SRCS=rawCliProcn.c rawProcUn.c rawProcn.c \
	rawapin.c rconnect.c

RAWAPI_OBJS=$(RAWAPI_SRCS:%.c=rawapi_%.o)

###################################################
# When compiling from sources

OBJS += $(RAWAPI_OBJS)

###################################################

rawapi_%.o: $(RAWAPI_DIR)/src/%.c rawapi_%.d
	@echo "   CXX   $@"
	@$(CXX) -I. \
	  $(RAWAPI_CFLAGS) \
	  $(call FILTER_WARNINGS,$@,$(CXXFLAGS)) $(OPTFLAGS) -o $@ -c $<

rawapi_%.d: $(RAWAPI_DIR)/src/%.c
	@echo "  DEPS   $@"
	@$(CXX) -I. \
	  $(RAWAPI_CFLAGS) \
	    $(call FILTER_WARNINGS,$@,$(CXXFLAGS)) -MM -MG $< | \
	  sed -e 's,\($(*F)\)\.o[ :]*,rawapi_$*.o $@ : ,g' > $@

###################################################
endif # RAWAPI_USE_LIB
