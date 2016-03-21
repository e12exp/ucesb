# -*- Makefile -*-

UNAME_S:=$(shell uname -s)

ifneq ($(UNAME_S),Darwin)
  LIB_LRT:=-lrt
else
  LIB_LRT:=
endif

EMPTY_FILE=$(UCESB_BASE_DIR)/file_input/empty_file

$(EMPTY_FILE): gen/acc_auto_def/byteorder_include.h \
	gen/acc_auto_def/byteswap_include.h \
	$(UCESB_BASE_DIR)/hbook/example/test_caen_v775_data.h \
	$(UCESB_BASE_DIR)/hbook/example/test_caen_v1290_data.h

$(EMPTY_FILE): $(EMPTY_FILE).cc
	@echo "  BUILD  $@"
	@$(CXX) -O3 -g \
	  -I$(UCESB_BASE_DIR)/file_input \
	  -I$(UCESB_BASE_DIR)/eventloop \
	  -I$(UCESB_BASE_DIR)/acc_def \
	  -I$(UCESB_BASE_DIR) \
	  -o $@ $< $(LIB_LRT)
