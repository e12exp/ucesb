# -*- Makefile -*-

ifndef NO_RAWAPI
#RAWAPI_DIR=/u/johansso/staging_adsm/v60c_patched
# v80 has compile issues for source
# v70 has permission issues for the include files
RAWAPI_DIR=/GSI/staging/adsm/v80
HAS_RAWAPI = $(shell test -e $(RAWAPI_DIR)/inc/rawapin.h && echo exist)
endif
