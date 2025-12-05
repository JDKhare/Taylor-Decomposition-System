#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  port_repository.pl
#
#        USAGE:  ./port_repository.pl 
#
#  DESCRIPTION:  used to replace the content of the local incascout tds repository
#                with a copy of the home repository
#
# REQUIREMENTS:  input repository folder, output repository folder
#        NOTES:  ---
#       AUTHOR:  Daniel Fracisco Gomez-Prado
#      VERSION:  1.0
#      CREATED:  11/07/2008 11:57:49 AM EST
#===============================================================================

use strict;
use warnings;
use Shell qw(cat);
use File::Basename;
use File::Copy;
use Cwd qw(abs_path);

# hold the name of the current script file
my $what = $0;
my $root_dir = qx(pwd);	
if($root_dir=~m{^/cygdrive/([A-Za-z])/(.*)}){
	$root_dir = "$1:/$2";
}
chomp($root_dir);
my $HELP = <<EOF
  Usage: 
	  $what from to filetypes

	  from:      can be given in relative or absolute path
	  to:        must be given in absolute path
	  filetypes: must be given with quotation "*"

  Description:
	  $what used to replace the content of one repository with other;
	  the repository "from" is copied in the repository "to".

  Warning:
  	  Copies only one folder/sub_folders at a time. In the case of the
	  folder Scripts which doesn't have subfolders it fails.
  
  Example:
	  $what ./src ~/incascout/tds/src "*.cc *.cpp *.h *.hp p *.inl"
EOF
;

my $from = "";
my $to = "";
my $filetypes = "";

#chomp($currentDir);

# Start executing the script or show the help
if( @ARGV == 1 ){
	if ($ARGV[0]=~m/^--help$/ || $ARGV[0]=~m/^-h$/) {
		print $HELP;
	} else {
		print "ERROR: Unknow option $ARGV[0] or missing parameters\n";
		print "ERROR: Type $what --help for help\n";
	}
	exit 1;
} elsif (@ARGV == 3) { 	
	$from = $ARGV[0];
	$to = $ARGV[1];
	$filetypes = $ARGV[2];
} else {
	print $HELP;
	exit 1;
}

main();

sub traverse_source_folders {
	chdir "$from";
	my @sub_folders = glob "*";
	my $current_folder;
	foreach $current_folder (@sub_folders) {
		if( (-d $current_folder) && ($current_folder!~m{.svn}) ) {
			print "$current_folder project\n";
			if( !(-d "$to/$current_folder" )) {
				print "\toutput folder $to/$current_folder doesn't exist\n";
				print "\tGenerating output folder\n";
				mkdir "$to/$current_folder";
			}
			chdir $current_folder;
			my @files = glob $filetypes;
			my $current_file;			
			print "\tcopying "; 
			foreach $current_file (@files) {
				print"$current_file, ";
				copy($current_file,"$to/$current_folder/$current_file") or die "ERROR: File $current_file can not be copied";
			}
			print "\n";
			chdir "../";
		} else {
			print "WARNING: skipping $current_folder\n";
		}
	}
}

sub main {  
	print "Syncronizing TDS repositories\n";
	print "From: $from\n";
	print "To  : $to\n";
	#print "copying Makefile, ";
	#copy("$from/Makefile","$to/Makefile") or die "ERROR: Makefile can not be copied\n";
	#print "ToDo, ";
	#copy("$from/ToDo","$to/ToDo") or die "ERROR: ToDo can not be copied\n";
	#print "Revision ";
	#copy("$from/Revision","$to/Revision") or die "ERROR: Revision can not be copied\n";
	print "\n--------------------\n";
	if ( (-d $from) && (-d $to) ) {
		traverse_source_folders;
		print "--------------------\n";
		print "Syncronization finished\n";
	} else {
		print "ERROR: The source or destination folder does not exist\n";
	}
}

