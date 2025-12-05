#!/usr/bin/perl -w 
#===============================================================================
#
#         FILE:  dos2unix.pl
#
#        USAGE:  ./dos2unix.pl  
#
#  DESCRIPTION:  
#   Perl equivalent of the popular dos2unix utility:
#   Convert DOS line endings to Unix line endings:
#   works in bulk, safely updates files in place.
#
#       AUTHOR:  Daniel Gomez (taken from: http://www.obviously.com/tech_tips/dos2unix.html)
#      VERSION:  1.0
#      CREATED:  03/30/2009 11:52:00 PM
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;


my ($filename);
my $what = $0;
my $HELP = <<EOF
  Usage: 
  	$what [-h|--help] filename

  Description:
	$what, Replace DOS line endings with Unix line endings
  
EOF
;
#   If no arguments, print an error message
if( $#ARGV < 0 ) {
    print $HELP;
    exit 1;
} else {
	$filename = $ARGV[0];
}
main();

sub main {
	if( -e "$filename.new" ) {
		printf "Skipping $filename.new - it already exists\n";
	}
	elsif(!( -f $filename && -r $filename && -w $filename  )) {
		printf "Skipping $filename - not a regular writable file\n";
	}
	else {
		rename("$filename","$filename.new");
		open INPUT, "$filename.new";
		open OUTPUT, ">$filename";

		while( <INPUT> ) {
			s/\r\n$/\n/;     # convert CR LF to LF
			print OUTPUT $_;
		}

		close INPUT;
		close OUTPUT;
		unlink("$filename.new");
	}
	print ",converted to unix ending";
}

