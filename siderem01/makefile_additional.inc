# -*- Makefile -*-

USE_LMD_INPUT=1

USER_STRUCT_FILE = siderem01_user_struct.hh

OBJS += siderem01_user.o siderem01_user_ap.o sst_peak.o

# For the siderem_ext (common)
SST_DIR = $(UCESB_BASE_DIR)/dets/sst
$(eval $(call CHECK_DIRECTORY_EXIST,$(SST_DIR)))

ifneq (clean,$(MAKECMDGOALS))

include $(SST_DIR)/makefile_sst_ext.inc

INCLUDE_DIRS += $(SST_DIR)

# For the auto-pedestal-adjustment from ccdaqlib

CCDAQLIB_DIR=$(call TREE_UCESB_OR_HOME_SRC_DIR,landdaq_/ccdaqlib)
$(eval $(call CHECK_DIRECTORY_EXIST,$(CCDAQLIB_DIR),))

# For the new format from vidsel/siderem

SIDFILTER_DIR=$(call TREE_UCESB_OR_HOME_SRC_DIR,landdaq_/vidsel/siderem)
$(eval $(call CHECK_DIRECTORY_EXIST,$(SIDFILTER_DIR),))

$(warning: SIDFILTER_DIR=$(SIDFILTER_DIR))

CXXFLAGS += -DCONTROL_INCLUDE  -DUSE_LOCAL_ERROR_HANDLING \
	-I$(CCDAQLIB_DIR) \
	-I$(SIDFILTER_DIR) \
	-I$(SST_DIR)
UCESB_CPP_FLAGS += -I$(SST_DIR) -I.
PSDC_CPP_FLAGS  += -I$(SST_DIR)

endif
