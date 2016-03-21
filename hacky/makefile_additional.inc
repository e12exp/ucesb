# -*- Makefile -*-

USE_LMD_INPUT=1

# If you use an external unpacker (and then copy e.g. the
# external_data.{hh,cc} template files to this directory,
# you need to also get them into the list of sources =>
# enable external_hacky.o below.

OBJS += hacky_user.o # external_hacky_format.o

# The file containing user structure elements that are ntuple-dumpable,
# bia psdc-parsing

USER_STRUCT_FILE = hacky_user_struct.hh

CXXFLAGS += -DCONTROL_INCLUDE
