# -*- Makefile -*-

USE_HLD_INPUT=1

OBJS += hades_user.o

# For the hades_rich (common)
RICH_DIR = $(UCESB_BASE_DIR)/dets/rich
$(eval $(call CHECK_DIRECTORY_EXIST,$(RICH_DIR)))

ifneq (clean,$(MAKECMDGOALS))

include $(RICH_DIR)/makefile_rich_ext.inc

CXXFLAGS += -DCONTROL_INCLUDE \
	-I$(RICH_DIR)
CXXFLAGS += -I$(HOME)/landdaq_/vidsel/siderem/unfinished

UCESB_CPP_FLAGS += -I$(RICH_DIR) -I.
PSDC_CPP_FLAGS  += -I$(RICH_DIR)

endif
