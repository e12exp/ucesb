# -*- Makefile -*-

#########################################################

MAKE_ACC_AUTO_DEF=$(UCESB_BASE_DIR)/lu_common/make_acc_auto_def.sh

gen/acc_auto_def/%.h: $(UCESB_BASE_DIR)/acc_def/%.h $(MAKE_ACC_AUTO_DEF)
	@echo " AUTODEF $@"
	@mkdir -p $(dir $@)
	@$(MAKE_ACC_AUTO_DEF) \
	  --input $< --compiler $(CXX) \
	  --compileargs $(filter-out -Wno-non-template-friend,$(CXXFLAGS)) > $@.tmp
	@mv $@.tmp $@

#########################################################
