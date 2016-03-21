
MOD_NAME = hbook
MOD_DIR  = util/hbook
MOD_DEP  = 
MOD_SRCS = hbook.cc
MOD_CSRCS =

ifeq (,$(findstring -DNO_NEED_DATABASE,$(NEED_CXX_FLAGS)))
USING_EXT_WRITER = YES
endif

ifdef USING_EXT_WRITER
NTUPLE_STAGING = 1
endif

ifdef NTUPLE_STAGING
MOD_SRCS += staged_ntuple.cc staging_ntuple.cc writing_ntuple.cc 
ifdef USING_EXT_WRITER
MOD_SRCS += external_writer.cc
endif
endif

#########################################################

EXTWRITE = util/hbook/make_external_struct_sender.pl

EXTWRITE_GENDIR := $(BUILD_DIR)/auto_gen

$(EXTWRITE_GENDIR)/extwrite_%.hh: $(EXTWRITE) $(EXTWRITE_SOURCES)
	@echo "EXTWRSTR $@" # $(ASMGENFLAGS)
	@$(EXTWRITE) --struct=extwrite_$* < $(EXTWRITE_SOURCES) > $@ || rm $@

# This rule fixes up problems with auto-gen dependent paths
# in dependecy files.  Automatic dependencies are a pain.
auto_gen/extwrite_%.hh: $(EXTWRITE_GENDIR)/extwrite_%.hh
	@echo " SPECIAL $@"

#########################################################

include module.mk
