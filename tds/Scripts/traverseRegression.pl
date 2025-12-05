#!/usr/bin/perl 
##  @file     traverseRegression.pl
#   @brief    Traverses all folders in traverseRegression recursively, executing test cases scripts *.scr
#   @details  Usage:
#              @verbatim  traverseRegression.pl [-h|--help] @endverbatim
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
my $HELP = <<EOF
  Usage: 
	$what [-h|--help] [-d directory] [-f fileType] scriptFile

  Description:
	$what, traverses folders in RegressionTest recursively, 
	searching for certain file types to execute a script on
	them, one by one. Files and Folders starting with "__"
	are always skipt.

  Options:
	-d directory {default is *}
		Traverses only the folder "directory" looking for
		files with extension "filetype". This is usefull
		if we have a set of test common to one feature 
		that we are currently developping.
	-f fileType {default is *.scr}
		Executes in all files of type "fileType" the passed
		script
	scriptFile 
		The script file name to run in each test. The output
		of its execution is send to the screen and to a file
		named "outReg_scriptFile.csv". The default script is
		runRegression.pl. 

  Scripts' List:
	dummy.pl         	prints out the name of the tests to run
	runRegression.pl 	runs the test cases

  Example:  
	$what -d Reorder -f "*.scr" runRegression.pl
	$what runRegression.pl

EOF
;

my $regression_dir = "RegressionTest";
my %flag_option = ( 'directory'=>'*', 'fileType'=>'*.scr', 'script'=>'runRegression.pl' );
my $os = $^O;
my $path_separator;
my $relative_path;
if ( $os=~/MSWin32/ ) {
	$path_separator = "\\";
	$relative_path = ".\\..\\..\\";
} else {
	$path_separator = "/";
	$relative_path = "./../../";
}

# Start executing the script or show the help
if( @ARGV > 0 ){
	while (@ARGV > 0){
		my $opt = $ARGV[0];
		if ($opt=~m/^--help$/ || $opt=~m/^-h$/) {
			print $HELP;
			exit 1;
		} elsif ($opt=~m/^--directory$/ || $opt=~m/^-d$/) {
			shift;
			$flag_option{directory} = $ARGV[0];
			shift;
		} elsif ($opt=~m/^--fileType$/ || $opt=~m/^-f$/) {
			shift;
			$flag_option{fileType} = $ARGV[0];
			shift;
		} else {
			$flag_option{script} = $opt;
			shift
		}
	};
}

my $output_file = "outReg_$flag_option{script}.csv";
$output_file =~ s/\.pl//;

print "----------------------------\n";
print "Running in: $os\n";
print "Traversing: $regression_dir/\"$flag_option{directory}\"\n";
print "Execute in: \"$flag_option{fileType}\" script \"$flag_option{script}\"\n";
print "Output dmp: $output_file\n";
print "----------------------------\n\n";

main();

sub runRegression {
	my $path;
	my $suit;
	($path,$suit) = (@_);
	my @test_cases = glob $flag_option{fileType};
	my $current_test;
	foreach $current_test (@test_cases) {
		if ($current_test!~m{^__.*}){
			print "    Test: $current_test \n";
			print OUT "$suit,$current_test";			
			my @output = `${path}Scripts${path_separator}$flag_option{script} $current_test $path`;				  
			print OUT "@output\n";

		}
	}
}

sub main {
	open OUT, "> $output_file" or die "ERROR: can't create $output_file\n";      	
	print OUT "SUIT,TEST,RESULT,PASSED,FAILED,ELAPSED TIME,EXIT STATUS\n";
	chdir $regression_dir;
	my @test_folders;
	if ($flag_option{directory}=~m{\*}) {
		@test_folders = glob "*";
	} else {
		@test_folders = ($flag_option{directory});  
	}
	my $current_suit;
	foreach $current_suit (@test_folders) {
		if( (-d $current_suit) && ($current_suit!~m{^__.*}) ) {
			chdir $current_suit;
			print "*** Regression Suit: $current_suit\n";
			runRegression($relative_path,$current_suit);
			my @test_cases = glob $flag_option{fileType};
			my $current_test;
			my @sub_test_folders = glob "*";
			my $sub_suit;
			foreach $sub_suit (@sub_test_folders) {
				if( (-d $sub_suit) && ($sub_suit!~m{^__.*}) ) {
					chdir $sub_suit;
					print "****** Sub Reg Suit: $current_suit\_$sub_suit\n";
					runRegression($relative_path."..".$path_separator,$current_suit."_".$sub_suit);
					chdir "../";
				}
			}
			chdir "../";
		}
	}
	close OUT;
	print "\n";
	print "----------------------------\n";
	print "Done\n";
}

