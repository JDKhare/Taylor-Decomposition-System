#!/usr/bin/perl 
##  @file     RegressionTest.pl
#   @brief    Traverses all folders in RegressionTest recursively, executing test cases scripts *.scr
#   @details  Usage:
#              @verbatim  RegressionTest.pl [-h|--help] @endverbatim
#   @note     All the properties and options are case insensitive.
#   @author   Daniel Gomez-Prado
#   @version  1.0
#   @date     09/09/2008 03:14:27 PM PDT

use strict;
use warnings;

use Shell qw(cat);
use File::Basename;
use Cwd qw(abs_path);

# hold the name of the current script file
my $what = $0;
my $root_dir = qx(pwd);	
# on windows, it requires having the GNUWin32 environment
# in order to be able to run the command "pwd"
if($root_dir=~m{^/cygdrive/([A-Za-z])/(.*)}){
	$root_dir = "$1:/$2";
}
my $regression_dir = "RegressionTest";
my $to_test = "ALL";

my $HELP = <<EOF
  Usage: 
	  $what [-h|--help] [directory]

  Description:
	  $what, traverses all folders in RegressionTest recursively, searching for test cases scripts *.scr
	  executes one by one.

	  $what FileDir, traverses only the directory "FileDir" looking for the scripts *.scr. This is usefull
	  if we have a set of test common to one feature that we are currently developping.

EOF
;

my $os = $^O;
my $path_separator;
my $tds_bin;
my $path_patch;
if ( $os=~/MSWin32/ ) {
	$path_separator = "\\";
	$tds_bin = "tds32.exe";
	$path_patch = "Release\\Win32\\";
#	$relative_path = ".\\..\\..\\";
} else {
	$path_separator = "/";
	$tds_bin = "tds";
	$path_patch = "";
#	$relative_path = "./../../";
}
my $relative_path = ".${path_separator}..${path_separator}..${path_separator}";

# Start executing the script or show the help
if( @ARGV > 0 ){
	while (@ARGV > 0){
		my $opt = $ARGV[0];
		if ($opt=~m/^--help$/ || $opt=~m/^-h$/) {
			print $HELP;
			exit 1;
		} else {
			$to_test = $opt;
			$to_test =~s/$regression_dir\///g;
			shift;
			print "Only Regression Test $to_test will be run\n";
		}
	};
}
main();

sub runRegression {
	my $path2tds;
	($path2tds) = (@_);
	my @test_cases = glob "*.scr";
	my $current_test;
	foreach $current_test (@test_cases) {
		if ($current_test!~m{^__.*}){
			print "--> Running test: $current_test \n";
			system("${path2tds}${path_patch}${tds_bin} -f $current_test");
		}
	}
}

sub main {
	print "Starting the Regression Test\n";
	print "----------------------------\n\n";
	chdir $regression_dir;
	my @test_folders;
	if ($to_test=~m{ALL}) {
		@test_folders = glob "*";
	} else {
		@test_folders = ($to_test);  
	}
	my $current_suit;
	foreach $current_suit (@test_folders) {
		if( (-d $current_suit) && ($current_suit!~m{^__.*}) ) {
			chdir $current_suit;
			print "************************************\n";
			print "*** Regression Suit: $current_suit\n";
			runRegression($relative_path);
			my @sub_test_folders = glob "*";
			my $sub_suit;
			foreach $sub_suit (@sub_test_folders) {
				if( (-d $sub_suit) && ($sub_suit!~m{^__.*}) ) {
					chdir $sub_suit;
					print "****** Sub Reg Suit: $sub_suit\n";
					runRegression($relative_path."..".$path_separator);
					chdir "../";
				}
			}
			chdir "../";
		}
	}
	print "--------------------\n";
	print "Reggression Test End\n";
}


