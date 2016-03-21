# -*- Makefile -*-

OBJS += cros3_ext.o

REWRITE_DIR=$(call TREE_UCESB_OR_HOME_SRC_DIR,landdaq_/vidsel/cros3)
$(eval $(call CHECK_DIRECTORY_EXIST,$(REWRITE_DIR)))

SRC_DIRS += $(UCESB_BASE_DIR)/dets/pdc

CXXFLAGS += -I$(REWRITE_DIR)
