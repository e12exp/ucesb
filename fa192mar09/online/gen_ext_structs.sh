#!/bin/sh

../fa192mar09 /dev/null --ntuple=RAW,STRUCT_HH,fa192_onl.h

g++ -o dump_graph -I . -I ../../hbook dump_graph.cc ../../hbook/ext_data_client.o

# For SSSD, LU

../fa192mar09 /dev/null --ntuple=RAW,SSSD,LU,TRIGGER,EVENTNO,STRUCT_HH,fa192_SSSD_LU_onl.h

# Run from unpacker/:

# fa192mar09/fa192mar09 --trans=localhost --ntuple=RAW,SSSD,LU,TRIGGER,EVENTNO,SERVER,PORT=56001,dummy

# root -x mon_setup_compile.C+ -x mon_Si.C+

# For sampling

../fa192mar09 /dev/null --ntuple=RAW,sadcp,sadcpp,TRIGGER,EVENTNO,STRUCT_HH,fa192_sadcp_onl.h

# fa192mar09/fa192mar09 --trans=localhost --ntuple=RAW,sadcp,sadcpp,TRIGGER,EVENTNO,SERVER,PORT=56002,dummy

