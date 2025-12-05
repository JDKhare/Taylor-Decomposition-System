#!/usr/bin/perl 
##  @file     traverseSource.pl
#   @brief    Traverses all folders in traverseSource recursively, executing test cases scripts *.scr
#   @details  Usage:
#              @verbatim  traverseSource.pl [-h|--help] @endverbatim
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
  	$what [-h|--help] [-d directory] [-f fileType] script

  Description:
	$what, traverses folders in src recursively, 
	searching for certain file types to execute a script on
	them, one by one. Files and Folders starting with "__"
	are always skipt.

  Options:
	  -d directory {default is *}
	  	Traverses only the folder "directory" looking for
		files with extension "filetype". This is usefull
		if we have a set of test common to one feature 
		that we are currently developping.
	  -f fileType {default is *.c *.cc *.cpp *.C *.h *.hh *.hpp *.H *.inl}
	  	Executes in all files of type "fileType" the passed
		script
	  script {defautl is dummy.pl}

  Example:
  	$what -d dfg -f "*.cc" strip_trail_ws.pl
	  
EOF
;

my $output_file = "outputSource.csv";
my $source_dir = "src";
my %flag_option = ( 'directory'=>'*', 'fileType'=>'*.c *.cc *.cpp *.C *.h *.hh *.hpp *.H *.inl', 'script'=>'dummy.pl' );
my $os = $^O;

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

my $output_file = "outSrc_$flag_option{script}.csv";
$output_file =~ s/\.pl//;

print "----------------------------\n";
print "Running in: $os\n";
print "Traversing: $source_dir/\"$flag_option{directory}\"\n";
print "Execute in: \"$flag_option{fileType}\" script \"$flag_option{script}\"\n";
print "Output dmp: $output_file\n";
print "----------------------------\n\n";

main();

sub main {
  open OUT, "> $output_file" or die "ERROR: can't create $output_file\n";      	
  print OUT "FOLDER,FILE,OPERATION\n";
  chdir $source_dir;
  my @source_folders;
  if ($flag_option{directory}=~m{\*}) {
  	@source_folders = glob "*";
  } else {
	@source_folders = ($flag_option{directory});  
  }
  my $current_folder;
  foreach $current_folder (@source_folders) {
	  if( (-d $current_folder) && ($current_folder!~m{^__.*}) ) {
		  chdir $current_folder;
		  my @test_cases = glob $flag_option{fileType};
		  my $current_source;
		  print "*** Sub Project: $current_folder\n";
		  foreach $current_source (@test_cases) {
			  if ($current_source!~m{^__.*}){
				  print "    source: $current_source \n";
				  print OUT "$current_folder,$current_source";
				  #system("./../../Scripts/$flag_option{script} $current_source");
				  my @output = `./../../Scripts/$flag_option{script} $current_source`;
				  print OUT "@output\n";
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

