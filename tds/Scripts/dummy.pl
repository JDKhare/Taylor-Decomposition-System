#!/usr/bin/perl -w
#===============================================================================
#
#         FILE:  dummy.pl
#
#        USAGE:  ./dummy.pl  
#
#  DESCRIPTION:  
#
#       AUTHOR:  Daniel Gomez, 
#      CREATED:  03/31/2009 01:37:22 PM
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;

my ($filename);
my $what = $0;
my $HELP = <<EOF
  Usage: 
  	$what filename

Description:
	$what, echoes the filename passed. This is a dummy script just to test what files
	will be modfied when running traverse*.pl
  
EOF
;

#   If no arguments, print an error message
if( $#ARGV < 0 ) {
    print $HELP;
    exit 1;
} else {
	$filename = $ARGV[0];
}

print ",touching $filename";
