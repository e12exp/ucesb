# -*- Makefile -*-

PSDC=$(UCESB_BASE_DIR)/psdc/psdc

-include $(UCESB_BASE_DIR)/psdc/psdc.base.dep

$(PSDC): 
	$(MAKE) -C $(UCESB_BASE_DIR)/psdc psdc

