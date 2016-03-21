# -*- Makefile -*-

USE_LMD_INPUT=1

USER_STRUCT_FILE = is446_user_struct.hh
INCLUDE_DIRS = $(UCESB_BASE_DIR)/dets/dsssd/

CXXFLAGS += -DCONTROL_INCLUDE

OBJS += is446_user.o \
	is446_repack.o

