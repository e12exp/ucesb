# -*- Makefile -*-

USE_PAX_INPUT=1

CXXFLAGS += -DCONTROL_INCLUDE -D'USER_EXTERNAL_UNPACK_STRUCT_FILE="esn_external.hh"'

OBJS += esn_user.o esn_fcns.o \
	esn_external.o esn_external_mwpc.o esn_external_vdc.o

CALIB_STRUCT_FILE = esn_calib_struct.hh

#USE_VESNA5=1

#########################################################
#
# Including VESNA5
#

ifdef USE_VESNA5
OBJS += vesna5/vesna5.o \
	vesna5/v5_dump.o \
	vesna5/v5_match.o \
	vesna5/v5_q_track.o \
	vesna5/v5_q_track2.o \
	vesna5/v5_cal_tdc.o \
#	vesna5/v5_calib.o
endif

CXXFLAGS += -Ivesna5

clean::
	rm -f vesna5/*.o
	rm -f vesna5/*.d

#########################################################

