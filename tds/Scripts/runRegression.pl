#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  runRegression.pl
#
#        USAGE:  ./runRegression.pl  
#
#  DESCRIPTION:  runs the regression test on a given ted script
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  Daniel Gomez-Prado 
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  03/30/2009 10:12:47 AM
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;

use Shell qw(cat);
use File::Basename;
use Cwd qw(abs_path);

my $os = $^O;
my $what = $0;
my $root_dir = qx(pwd);	
my $current_test = "";
my $path_to_tds = "";
my $tds = "";
if($os=~m{linux}){
	$tds = "./tds";
} else {
	$tds = "tds.exe";
	if($root_dir=~m{^/cygdrive/([A-Za-z])/(.*)}){
		$root_dir = "$1:/$2";
	}
}

my $HELP = <<EOF
  Usage: 
  	$what [-h|--help] script.ted path_to_tds

  Description:
	$what, runs a script.ted into $tds. This script should be called
	by traverseRegression.pl

  Example:
  	./Scripts/traverseRegression.pl -d Reorder -f "*.scr" runRegression.pl
	  
EOF
;

# Start executing the script or show the help
if(@ARGV == 2 ){
  $current_test = $ARGV[0];
  $path_to_tds = $ARGV[1];
} elsif(@ARGV == 1 ){
  	print $HELP;
	exit 1;
} else {
    print "requires a tds-script filename, and the path to tds executable\n";
	print $HELP;
    exit 1;
}

main();

sub main {
	#print "Executing ../../$tds -f $current_test  -> ";
	if( !(-e "${path_to_tds}$tds") ){
		print " Warning: executable \"$tds\" was not found in path\n";
		exit 3;
	}
   	system("${path_to_tds}$tds -f $current_test");
   	if ($?==0) { 
		print ",Normal\n";
    } else {
    	print ",FAILED,,,ABORTED\n";
    }   
}

