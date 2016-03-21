#!/usr/bin/perl

# usage: cpp -P ana_par/mwpc_jan00.par | ./mwpc_par.pl 

# adress  slot    #MWPC   #plane */ #card

while (my $line = <>)
{
    if ($line =~ /(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/)
    {
	$address = $1;
	$slot    = $2;
	$mwpc    = $3;
	$plane   = $4;
	$card    = $5;	

	printf "$address  $slot  $mwpc  $plane  $card\n";
    }  


}


