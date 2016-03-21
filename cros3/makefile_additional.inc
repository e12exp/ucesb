# -*- Makefile -*-

USE_LMD_INPUT=1

OBJS += cros3_user.o 

USER_STRUCT_FILE = cros3_user_struct.hh

# For the cros3_ext (common)
PDC_DIR = $(UCESB_BASE_DIR)/dets/pdc
$(eval $(call CHECK_DIRECTORY_EXIST,$(PDC_DIR)))

ifneq (clean,$(MAKECMDGOALS))

include $(PDC_DIR)/makefile_pdc_ext.inc

CXXFLAGS += -DCONTROL_INCLUDE  -DUSE_LOCAL_ERROR_HANDLING \
	-I$(PDC_DIR)
UCESB_CPP_FLAGS += -I$(PDC_DIR) -I.
PSDC_CPP_FLAGS  += -I$(PDC_DIR)

endif
