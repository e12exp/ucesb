#!/bin/bash
cd $(dirname $0)

CXXFLAGS="-O0 -g" USE_INPUTFILTER=1 NO_CERNLIB=1 NO_YACC_WERROR=1 make empty -j8

