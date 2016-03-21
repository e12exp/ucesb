# -*- Makefile -*-

USE_LMD_INPUT=1

CXXFLAGS += -DCONTROL_INCLUDE

OBJS += is430_05_user.o \
	is430_aug05_repack.o \
	scaler_timer.o


