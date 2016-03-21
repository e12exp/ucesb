# -*- Makefile -*-

TDAS_CONV=$(UCESB_BASE_DIR)/file_input/tdas_conv

$(TDAS_CONV): gen/acc_auto_def/byteorder_include.h \
	gen/acc_auto_def/byteswap_include.h

$(TDAS_CONV): $(TDAS_CONV).cc
	@echo "  BUILD  $@"
	@$(CXX) -O3 -g -D__LAND02_CODE__ \
	  -I$(UCESB_BASE_DIR)/file_input \
	  -I$(UCESB_BASE_DIR)/eventloop \
	  -I$(UCESB_BASE_DIR)/threading \
	  -I$(UCESB_BASE_DIR)/common \
	  -I$(UCESB_BASE_DIR)/lu_common \
	  -I$(UCESB_BASE_DIR)/acc_def \
	  -I$(UCESB_BASE_DIR) \
	  -o $@ $< 

.PHONY: tdas_conv
tdas_conv: $(TDAS_CONV)
