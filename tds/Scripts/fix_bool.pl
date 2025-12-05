#!/usr/bin/perl -w
#taken from samba source/scripts

open(INFILE, "$ARGV[0]") || die $@;
open(OUTFILE, ">$ARGV[0].new") || die $@;

while (<INFILE>) {
	$_ =~ s/True/true/;
	$_ =~ s/False/false/;
	print OUTFILE "$_";
}

close(INFILE);
close(OUTFILE);

rename("$ARGV[0].new", "$ARGV[0]") || die @_;

print ",conforming bool values to \"true\" or \"false\"";
exit(0);

