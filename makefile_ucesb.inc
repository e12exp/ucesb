# -*- Makefile -*-

UCESB=$(UCESB_BASE_DIR)/ucesb/ucesb

-include $(UCESB_BASE_DIR)/ucesb/ucesb.base.dep

$(UCESB): 
	$(MAKE) -C $(UCESB_BASE_DIR)/ucesb ucesb

