# -*- Makefile -*-

# Should really perhaps be USE_EBYE_INPUT_16=1, but since the code for
# 32-bit data was kept, let's use this otherwise unused test unpacker
# to check that it compiles and works.

USE_EBYE_INPUT_32=1

CXXFLAGS += -D'RAW_EVENT_USER_FUNCTION=user_function'

OBJS += is507_user.o

