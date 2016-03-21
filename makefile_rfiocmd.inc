# -*- Makefile -*-

export RAWAPI_DIR

RFIOCMD_DIR=$(UCESB_BASE_DIR)/rfiocmd

RFCAT=$(RFIOCMD_DIR)/rfcat

-include $(RFCAT).base.dep

$(RFCAT): 
	$(MAKE) -C $(RFIOCMD_DIR) rfcat 

CXXFLAGS += -DRFCAT_PREFIX="\"$(UCESB_BASE_DIR)/rfiocmd/\""
