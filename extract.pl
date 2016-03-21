#!/usr/bin/perl -W

# This scripts copies the part of the input which is between lines containing
# BEGIN_${NAME} and END_${NAME} to the output

my $inside = 0;

while ( my $line = <STDIN> )
{
    # Begin copy?
    if ($line =~ /BEGIN_${ARGV[0]}/) { $inside = 1; }

    # Do copy
    if ($inside) { print $line; }

    # End copy?
    if ($line =~ /END_${ARGV[0]}/) { $inside = 0; }
}


###
#
# The remainder of this script could work as extract.sh, and use awk
# instead of needing perl.  Look in makefile_unpacker.mk at EXTRACT=

##!/bin/sh
#
## replacement for extract.pl, that only need awk
#
#awk "BEGIN{skip=1} ; \
# /BEGIN_$1/ { skip = 0; } ; \
# skip == 1 { next } ; \
# { print \$0 } ; \
# /END_$1/ { skip = 1 }"
