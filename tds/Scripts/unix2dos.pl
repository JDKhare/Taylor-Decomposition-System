#!/usr/bin/perl -w 
#===============================================================================
#
#         FILE:  unix2dos.pl
#
#        USAGE:  ./unix2dos.pl  
#
#  DESCRIPTION:  
#   Perl equivalent of the popular dos2unix utility:
#   Convert Unix line endings to DOS line endings:
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
	$what, Replace Unix line endings with DOS line endings
  
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
			s/\n$/\r\n/;     # convert LF to CR LF
			print OUTPUT $_;
		}

		close INPUT;
		close OUTPUT;
		unlink("$filename.new");
	}
	print ",converted to dos ending";
}

