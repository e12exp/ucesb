# -*- Makefile -*-

# Makefile include for generating listings of dependencies
#
# $(BASENAME) is the name of the program,
#
# we then do $(BASENAME).dep on request

CPP_DEFINES_FREEBSD = $(shell cpp -dM /dev/null | grep __FreeBSD__)
CPP_DEFINES_NETBSD = $(shell cpp -dM /dev/null | grep __NetBSD__)
CPP_DEFINES_OPENBSD = $(shell cpp -dM /dev/null | grep __OpenBSD__)
CPP_DEFINES_SVR4 = $(shell cpp -dM /dev/null | grep __svr4__)
CPP_DEFINES_CYGWIN = $(shell cpp -dM /dev/null | grep __CYGWIN__)
CPP_DEFINES_APPLE = $(shell cpp -dM /dev/null | grep __APPLE__)

GCC_IS_3_3 = $(shell echo "__GNUC__ : __GNUC_MINOR__" | $(CXX) -E - | grep "3 : 3")
GCC_IS_4_3 = $(shell echo "__GNUC__ : __GNUC_MINOR__" | $(CXX) -E - | grep "4 : 3")
GCC_IS_4_4 = $(shell echo "__GNUC__ : __GNUC_MINOR__" | $(CXX) -E - | grep "4 : 4")

ifneq (,$(CPP_DEFINES_FREEBSD)$(CPP_DEFINES_NETBSD)$(CPP_DEFINES_SVR4))
SPACE_TO_NEWLINE = sed -e 'y, ,\n,'
else
SPACE_TO_NEWLINE = awk '{ gsub(/ /, "\n") ; print }'
endif

#$(warning BASENAME $(BASENAME))
#$(BASENAME).dep: $(AUTO_DEPS)
#	@echo " FULLDEP $@"
#	@echo "$@ $(MAKECMDGOALS): \\" > $@
#	@cat $(AUTO_DEPS) | sed -e 's,^.*:,,g' |\
#	  $(SPACE_TO_NEWLINE) |\
#	  sed -e 's,^,  ,g' |\
#	  sed -e 's,$$,\\,g' |\
#	  sed -e 's,\\\\$$,\\,g' | sort -u >> $@
#	@echo "  parser.y\\" >> $@
#	@echo "  lexer.lex\\" >> $@

