# -*- Makefile -*-

USE_LMD_INPUT=1

CXXFLAGS += -D'CAL_EVENT_USER_FUNCTION=user_function'

OBJS += rpc2006_user.o

