#!/usr/bin/perl

use strict;
use warnings;

my @logval;
my $index = 0;

my $det = -1;

my @valid;

for (my $i = 0; $i < 1024; $i++) { $logval[$i] = "0"; $valid[$i] = 0; }

foreach my $argv (@ARGV)
{
    if ($argv =~ /det(\d+)/) { $det = $1; }
    elsif ($argv =~ /x(\d+)-(\d+)/) {
	for (my $i = $1*8; $i < ($2+1)*8; $i++) { $valid[$i] = 1; }
    }
    elsif ($argv =~ /y(\d+)-(\d+)/) {
	for (my $i = $1*8; $i < ($2+1)*8; $i++) { $valid[$i+640] = 1; }
    }
    else { die "Bad option: $argv"; }
}

while (my $line = <STDIN>)
{
    chomp $line;

    $line =~ s/^ +//;
    $line =~ s/ +$//;

    if (!($line =~ /\#/))
    {
	$logval[$index++] = $line;
    }
}

for (my $i = 0; $i < 1024; $i++)
{
    if ($logval[$i] ne "0")
    {
	my $logval = $logval[$i];
	my $val = 1.0/exp($logval);
	print sprintf ("%sCALIB_PARAM( SST[%d][%4d].E , OFFSET_SLOPE , 0.0 , %6.2f ); /* %4d : %s%3d : %6.2f */\n",
		       $valid[$i] ? "   " : "// ",$det,$i,
		       $val,$i,$i<640?"x":"y",($i < 640 ? $i : $i - 640) / 8,$logval);
	
    }
}


# CALIB_PARAM( SCI[0][0].T ,SLOPE_OFFSET,  2.4 , 6.7 );
