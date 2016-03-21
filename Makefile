
# In case of problems compiling the parsers, ask them to be generated
# with bison instead of yacc (trouble on redhat / bsds) (uncomment
# below)
#YACC=bison  # needed on SLC
#export YACC #           SLC
#LEX=flex    # needed on cygwin
#export LEX  #           cygwin

UCESB_BASE_DIR=$(shell pwd)
export UCESB_BASE_DIR

#CXX=g++-3.2
export CXX

#########################################################

UNPACKERS=land xtst rpc2006 is446 is430_05 is445_08 esn labbet1 mwpclab \
	gamma_k8 hacky empty sid_genf madrid ebye i123 s107 tacquila \
	fa192mar09 is507 sampler ridf #tagtest

UNPACKERS_EXT=hades siderem01 cros3

all: $(UNPACKERS)
# These need landdaq / landdaq_
all_ext: $(UNPACKERS_EXT)

#########################################################
# Submakefiles that the programs depend on

include $(UCESB_BASE_DIR)/makefile_acc_def.inc
include $(UCESB_BASE_DIR)/makefile_ucesb.inc
include $(UCESB_BASE_DIR)/makefile_psdc.inc
include $(UCESB_BASE_DIR)/makefile_empty_file.inc
include $(UCESB_BASE_DIR)/makefile_tdas_conv.inc
include $(UCESB_BASE_DIR)/makefile_ext_file_writer.inc

DEPENDENCIES=$(UCESB) $(PSDC) $(EMPTY_FILE) $(EXT_WRITERS)

include $(UCESB_BASE_DIR)/makefile_hasrawapi.inc
ifneq (,$(HAS_RAWAPI))
include $(UCESB_BASE_DIR)/makefile_rfiocmd.inc
DEPENDENCIES+=$(RFCAT)
endif

#########################################################

.PHONY: empty_real
empty_real: $(DEPENDENCIES)
	@$(MAKE) -C empty -f ../makefile_unpacker.inc UNPACKER=empty 

#########################################################

empty/empty: empty_real

EMPTY_EMPTY_FILE=--lmd --random-trig --events=10

ext_h101.h: empty/empty $(EXT_STRUCT_WRITER)
	@echo "  EMPTY  $@"
	@empty/empty /dev/null --ntuple=UNPACK,STRUCT_HH,$@ \
	  > $@.out 2> $@.err || \
	  ( echo "Failure while running: '$< /dev/null --ntuple=UNPACK,STRUCT_HH,$@':" ; \
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr: ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err

# $(EXT_STRUCT_WRITER) make sure hbook/ext_data_client.o is built
ext_reader_h101: hbook/example/ext_data_reader.c $(EXT_STRUCT_WRITER) ext_h101.h
	@echo "  BUILD  $@"
	@$(CC) -W -Wall -Wconversion -g -O3 -o $@ -I. -Ihbook \
	  hbook/example/ext_data_reader.c hbook/ext_data_client.o

ext_reader_h101_items_info: hbook/example/ext_data_reader.c $(EXT_STRUCT_WRITER) ext_h101.h
	@echo "  BUILD  $@"
	@$(CC) -W -Wall -Wconversion -g -O3 -o $@ -I. -Ihbook \
	  -DUSE_ITEMS_INFO=1 \
	  hbook/example/ext_data_reader.c hbook/ext_data_client.o

ext_reader_h101_stderr: hbook/example/ext_data_reader_stderr.c $(EXT_STRUCT_WRITER) ext_h101.h
	@echo "  BUILD  $@"
	@$(CC) -W -Wall -Wconversion -g -O3 -o $@ -I. -Ihbook \
	  hbook/example/ext_data_reader_stderr.c hbook/ext_data_client.o

ext_writer_h101: hbook/example/ext_data_writer.c $(EXT_STRUCT_WRITER) ext_h101.h
	@echo "  BUILD  $@"
	@$(CC) -W -Wall -Wconversion -g -O3 -o $@ -I. -Ihbook \
	  hbook/example/ext_data_writer.c hbook/ext_data_client.o

ext_reader_h%.runstamp: ext_reader_h% $(EMPTY_FILE)
	@echo "  TEST   $@"
	@$(EMPTY_FILE) $(EMPTY_EMPTY_FILE) 2> $@.err3 | \
	  empty/empty --file=- --ntuple=UNPACK,STRUCT,- 2> $@.err2 | \
	  ./$< - > $@.out 2> $@.err || echo "* FAIL * ..."
	@diff -u hbook/example/$<.good $@.out || \
	  ( echo "--- Failure while running: " ; \
	    echo "$(EMPTY_FILE) $(EMPTY_EMPTY_FILE) | empty/empty --file=- --ntuple=UNPACK,STRUCT,- | ./$< -" ;\
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr (empty_file): ---"; cat $@.err3 ; \
	    echo "--- stderr (empty): ---"; cat $@.err2 ; \
	    echo "--- stderr ($@): ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err $@.err2 $@.err3
	@touch $@

ext_writer_h%.runstamp: ext_writer_h% $(EMPTY_FILE)
	@echo "  TEST   $@"
	@./$< 10 2> $@.err2 | \
	  empty/empty --in-tuple=UNPACK,STRUCT,- > $@.out 2> $@.err || \
	  echo "* FAIL * ..."
	@diff -u hbook/example/$<.good $@.out || true || \
	  ( echo "--- Failure while running: " ; \
	    echo "./$< 10 | empty/empty --in-tuple=UNPACK,STRUCT,-" ; \
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr ($@): ---"; cat $@.err2 ; \
	    echo "--- stderr (empty): ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err $@.err2
	@touch $@

#########################################################

.PHONY: empty
empty: empty_real
ifndef USE_MERGING # disabled for the time being (to be fixed...)
empty: ext_reader_h101.runstamp ext_reader_h101_items_info.runstamp \
	ext_reader_h101_stderr.runstamp \
	ext_writer_h101.runstamp
endif

#########################################################

.PHONY: xtst_real
xtst_real: $(DEPENDENCIES)
	@$(MAKE) -C xtst -f ../makefile_unpacker.inc UNPACKER=xtst

#########################################################

xtst/xtst: xtst_real

XTST_EMPTY_FILE=--lmd --random-trig --caen-v775=2 --caen-v1290=2 --events=1000
XTST_REGRESS=UNPACK,EVENTNO,TRIGGER,regress,ID=xtst_regress
XTST_REGRESS_MORE=UNPACK,EVENTNO,TRIGGER,regress,regressextra,ID=xtst_regress
XTST_REGRESS_LESS=UNPACK,EVENTNO,regress,ID=xtst_regress

# We depend on empty (full ext_reader stuff, as it is simpler)
ext_xtst_regress.h: xtst/xtst $(EXT_STRUCT_WRITER) ext_reader_h101.runstamp
	@echo "  XTST   $@"
	@xtst/xtst /dev/null \
	  --ntuple=$(XTST_REGRESS),STRUCT_HH,$@ \
	  > $@.out 2> $@.err || \
	  ( echo "Failure while running: '$< /dev/null --ntuple=$(XTST_REGRESS),STRUCT_HH,$@':" ; \
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr: ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err

# $(EXT_STRUCT_WRITER) make sure hbook/ext_data_client.o is built
ext_reader_xtst_regress: hbook/example/ext_data_reader_xtst_regress.c $(EXT_STRUCT_WRITER) ext_xtst_regress.h hbook/example/test_caen_v775_data.h hbook/example/test_caen_v1290_data.h
	@echo "  BUILD  $@"
	@$(CC) -W -Wall -Wconversion -g -O3 -o $@ -I. -Ihbook \
	  hbook/example/ext_data_reader_xtst_regress.c hbook/ext_data_client.o

ext_reader_xtst_regress.runstamp: ext_reader_xtst_regress $(XTST_FILE)
	@echo "  TEST   $@"
	@$(EMPTY_FILE) $(XTST_EMPTY_FILE) 2> $@.err3 | \
	  xtst/xtst --file=- \
	    --ntuple=$(XTST_REGRESS),STRUCT,- 2> $@.err2 | \
	  ./$< - > $@.out 2> $@.err || echo "fail..."
	@diff -u hbook/example/$<.good $@.out || \
	  ( echo "Failure while running: xtst_file | xtst | $@:" ; \
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr (xtst_file): ---"; cat $@.err3 ; \
	    echo "--- stderr (xtst): ---"; cat $@.err2 ; \
	    echo "--- stderr ($@): ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err $@.err2 $@.err3
	@touch $@

ext_reader_xtst_regress_more.runstamp: ext_reader_xtst_regress $(XTST_FILE)
	@echo "  TEST   $@"
	@$(EMPTY_FILE) $(XTST_EMPTY_FILE) 2> $@.err3 | \
	  xtst/xtst --file=- \
	    --ntuple=$(XTST_REGRESS_MORE),STRUCT,- 2> $@.err2 | \
	  ./$< - > $@.out 2> $@.err || echo "fail..."
	@diff -u hbook/example/$<_more.good $@.out || \
	  ( echo "Failure while running: xtst_file | xtst | $@:" ; \
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr (xtst_file): ---"; cat $@.err3 ; \
	    echo "--- stderr (xtst): ---"; cat $@.err2 ; \
	    echo "--- stderr ($@): ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err $@.err2 $@.err3
	@touch $@

ext_reader_xtst_regress_less.runstamp: ext_reader_xtst_regress $(XTST_FILE)
	@echo "  TEST   $@"
	@$(EMPTY_FILE) $(XTST_EMPTY_FILE) 2> $@.err3 | \
	  xtst/xtst --file=- \
	    --ntuple=$(XTST_REGRESS_LESS),STRUCT,- 2> $@.err2 | \
	  ./$< - > $@.out 2> $@.err || echo "fail..."
	@diff -u hbook/example/$<_less.good $@.out || \
	  ( echo "Failure while running: xtst_file | xtst | $@:" ; \
	    echo "--- stdout: ---" ; cat $@.out ; \
	    echo "--- stderr (xtst_file): ---"; cat $@.err3 ; \
	    echo "--- stderr (xtst): ---"; cat $@.err2 ; \
	    echo "--- stderr ($@): ---"; cat $@.err ; \
	    echo "---------------" ; false)
	@rm $@.out $@.err $@.err2 $@.err3
	@touch $@

#########################################################

.PHONY: xtst
xtst: xtst_real
ifndef USE_MERGING # disabled for the time being (to be fixed...)
xtst: ext_reader_xtst_regress.runstamp ext_reader_xtst_regress_more.runstamp \
	ext_reader_xtst_regress_less.runstamp
endif

#########################################################

.PHONY: hacky
hacky: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: sid_genf
sid_genf: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: sampler
sampler: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: madrid
madrid: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: is507
is507: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: tagtest
tagtest: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: labbet1
labbet1: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: mwpclab
mwpclab: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: gamma_k8
gamma_k8: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: cros3
cros3: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: esn
esn: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: is430_05
is430_05: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: is445_08
is445_08: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: is446
is446: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: siderem01
siderem01: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: rpc2006
rpc2006: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@ 

#########################################################

.PHONY: land
land: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: hades
hades: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: ebye
ebye: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: i123
i123: $(DEPENDENCIES)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: s107
s107: $(DEPENDENCIES) $(TDAS_CONV)
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: tacquila
tacquila: $(DEPENDENCIES) 
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: fa192mar09
fa192mar09: $(DEPENDENCIES) 
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

.PHONY: ridf
ridf: $(DEPENDENCIES) 
	@$(MAKE) -C $@ -f ../makefile_unpacker.inc UNPACKER=$@

#########################################################

clean: clean-dir-ucesb clean-dir-psdc clean-dir-rfiocmd clean-dir-hbook \
	$(UNPACKERS:%=clean-unp-%) $(UNPACKERS_EXT:%=clean-unp-%)
	rm -rf gen/acc_auto_def
	rm -f xtst/xtst.spec.d xtst/*.o xtst/*.d xtst/*.dep
	rm -f file_input/empty_file file_input/tdas_conv
	rm -f ext_reader_h101 ext_reader_h101_stderr ext_writer_h101
	@if (echo $(MAKEFLAGS) | grep -v -q j); then \
		echo "Hint: if cleaning is slow, do 'make clean -j 10'" ; \
	fi

clean-dir-%:
	$(MAKE) -C $* clean

clean-unp-%:
	$(MAKE) -C $* -f ../makefile_unpacker.inc UNPACKER=$* clean

all-clean: clean
	rm -rf land/gen
	rm -rf xtst/gen
