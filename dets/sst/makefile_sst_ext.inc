# -*- Makefile -*-

OBJS += siderem_ext.o sst_ext.o

DSPCODE_DIR=$(call TREE_UCESB_OR_HOME_SRC_DIR,landdaq_/vidsel/siderem)
$(eval $(call CHECK_DIRECTORY_EXIST,$(DSPCODE_DIR)))

SRC_DIRS += $(UCESB_BASE_DIR)/dets/sst

CXXFLAGS += -I$(DSPCODE_DIR)

SST_DET_ADD_PED = $(SST_DIR)/sst_get_add_ped.pl

gen/sst_ped_add.hh: sidped_daq.txt $(SST_DET_ADD_PED)
	@echo " GET_PED $@"
	@$(SST_DET_ADD_PED) < $< > $@
