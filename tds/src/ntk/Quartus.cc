/*
 * =====================================================================================
 *
 *       Filename:  Quartus.cc
 *    Description:
 *        Created:  07/23/2011 10:59:40 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */


#include "Quartus.h"

#include "Environment.h"
using namespace tds;

#include <ctime>
#include <cstdio>

#ifdef _MSC_VER
# define sprintf sprintf_s
#endif
using namespace std;

#include "util.h"
using namespace util;

const char* Quartus::ACTION_COMPILE = "_compile";
const char* Quartus::ACTION_CREATE = "_create";
const char* Quartus::EXTENSION_TCL = ".tcl";
const char* Quartus::EXTENSION_REPORT = ".quartus.report";
const char* Quartus::EXTENSION_VHD = ".vhd";

string Quartus::project_file_tcl(const string& action) {
	string project_file = _project_name + action + EXTENSION_TCL;
	return project_file;
}

string Quartus::report_file(void) {
	string report_file = _project_name + EXTENSION_REPORT;
	return report_file;
}

string Quartus::main_file_vhd(void) {
	string main_file = _project_name + _project_postfix + EXTENSION_VHD;
	return main_file;
}

bool Quartus::check_project_files(void) {
	bool ret = true;
	string libpath = Environment::getStr(Environment::GAUT_LIB_PATH);
	string tech = Environment::getStr(Environment::GAUT_TECH_VHD);
	if (!Util::fileExist(tech)) {
		if (!Util::copyFile(libpath+tech,tech)) {
			cout << "Error: Cannot copy file " << libpath << tech << " to working directory" << endl;
			ret = false;
		}
	}
	string tech_lib = Environment::getStr(Environment::GAUT_TECH_LIB_VHD);
	if (!Util::fileExist(tech_lib)) {
		if (!Util::copyFile(libpath+tech_lib,tech_lib)) {
			cout << "Error: Cannot copy file " << libpath << tech_lib << " to working directory" << endl;
			ret = false;
		}
	}
	if (!Util::fileExist(main_file_vhd())) {
		cout << "Error: Please run first \"cost -n\" to generate the vhd file for the current netlist" << endl;
		ret = false;
	}
	return ret;
}

bool Quartus::generate_quartus_script(const string& action) {
	ofstream outfile;
	outfile.open(project_file_tcl(action).c_str());
	if (!outfile.is_open()) {
		cout << "Error: Failed to create Tcl script " << project_file_tcl(action) << endl;
		cout << "       the script file could not be opened or created" << endl;
		return false;
	}
	void (Quartus::*functor)(ofstream&);
	functor = TclActions[action];
	(this->*functor)(outfile);
	outfile.close();
	return true;
}

bool Quartus::execute(const string& action) {
	if (!check_project_files()) {
		return false;
	}
	string cmd = Environment::getStr(Environment::QUARTUS_BIN_PATH) + Environment::DEF_PRG_QUARTUS + "_sh -t ";
	cmd += project_file_tcl(action);

	cmd += string(" 1>")+ _project_name + string(".quartus.msgs") + string(" 2>") + _project_name + string(".quartus.log") ;

	bool result = (Util::launchProc(cmd.c_str(),true) ==0);
#if 0
	// very cute but how to do this in quartus_sta and not quartus_tan
	result &= report_logic_levels();
#endif
	return result;
}

void Quartus::generate_report(ofstream& outfile) {
	outfile << endl;
	outfile << "#\n# Generating report\n#" << endl;
	outfile << "puts \"Info: Compiling report\"" << endl;
	outfile << "load_package report" << endl;
	outfile << "load_report" << endl;
	outfile << "set csv_file \"" << report_file() << "\"" << endl;
	outfile << "set fh [open $csv_file w]" << endl;
	outfile << "# print all panel names" << endl;
	outfile << "#set panel_names [get_report_panel_names]" << endl;
	outfile << "#foreach panel_name $panel_names {" << endl;
	outfile << "#	post_message \"$panel_name\"" << endl;
	outfile << "#}" << endl;
	// print all panel names
	// set panel_names [get_report_panel_names]
	// foreach panel_name $panel_names {
	//	post_message "$panel_name"
	// }
	outfile << "#" << endl;
	outfile << "# Clock Information" << endl;
	outfile << "#" << endl;
	//outfile << "set panel_name_fmax \"TimeQuest Timing Analyzer||Slow 900mV 0C Model||Slow 900mV 0C Model Fmax Summary\"" << endl;
	outfile << "set panel_name_fmax \"TimeQuest Timing Analyzer||Slow Model||Slow Model Fmax Summary\"" << endl;

	outfile << "set panel_name_clock \"TimeQuest Timing Analyzer||Clocks\"" << endl;
	outfile << "set data [get_report_panel_row -name $panel_name_clock -row 1]" << endl;
	outfile << "set clock_name [lindex $data 0]" << endl;
	outfile << "set clock_target [lindex $data 3]" << endl;
	outfile << "set data [get_report_panel_row -name $panel_name_fmax -row 1]" << endl;
	outfile << "set fmax [lindex $data 0]" << endl;
	outfile << "puts $fh \"clock name | target freq | design freq\"" << endl;
	outfile << "puts $fh \"--------------------------------------\"" << endl;
	outfile << "puts $fh [format \"%-*s | %-*s | %-*s\" 10 $clock_name 11 $clock_target 11 $fmax]" << endl;
	outfile << "puts $fh \"\"" << endl;
	outfile << " " << endl;
	outfile << "# Analysis & Synthesis Report and Fitter Report" << endl;
	outfile << "#" << endl;
	outfile << "set panel_name_analysis \"Analysis & Synthesis||Analysis & Synthesis Resource Usage Summary\"" << endl;
	outfile << "set panel_name_fitter \"Fitter||Resource Section||Fitter Resource Usage Summary\"" << endl;
	outfile << "puts $fh [format \"%-*s | Synthesis | Fitter\" 33 \"Resources\"]" << endl;
	outfile << "puts $fh \"--------------------------------------------------------------------\"" << endl;
	//  outfile << "set total_lab_row_fitter 22" << endl;
	//	outfile << "set fitter_data [get_report_panel_row -name $panel_name_fitter -row $total_lab_row_fitter]" << endl;
	//	outfile << "set name [lindex $fitter_data 0]" << endl;
	//	outfile << "set labs [lindex $fitter_data 1]" << endl;
	//	outfile << "set col0 [lindex $fitter_data 0]" << endl;
	//	outfile << "set col2 [lindex $fitter_data 1]" << endl;
	//	outfile << "puts $fh [format \"%-*s | %-*s | %s\" 33 $col0 9 \"0\" $col2] " << endl;
	//	outfile << "foreach analysis_row {1 2 3 4 13 14 15 16 17 27 28 37 39} fitter_row {1 2 3 4 8 9 10 11 12 35 36 89 93} {" << endl;
	outfile << "foreach analysis_row {1 8 9 10 11 12 2} fitter_row {1 5 6 7 8 9 2} {" << endl;
	outfile << "	set data [get_report_panel_row -name $panel_name_analysis -row $analysis_row]" << endl;
	outfile << "	set fitter_data [get_report_panel_row -name $panel_name_fitter -row $fitter_row]" << endl;
	outfile << "	set col0 [string trim [lindex $fitter_data 0]]" << endl;
	outfile << "	set col1 [lindex $data 1] " << endl;
	outfile << "	set col2 [lindex $fitter_data 1]" << endl;
	outfile << "	puts $fh [format \"%-*s | %-*s | %s\" 33 $col0 9 $col1 $col2] " << endl;
	outfile << "}" << endl;
	outfile << "close $fh" << endl;
	outfile << "unload_report" << endl;
}

void Quartus::compile_project(ofstream& outfile) {
	char time_stamp[100];
	time_t ctime = time(NULL);
	struct tm* ctm = gmtime(&ctime);
	sprintf(time_stamp, "%02d:%02d:%02d  %02d %02d, %04d", ctm->tm_hour, ctm->tm_min, ctm->tm_sec, ctm->tm_mon+1, ctm->tm_mday, ctm->tm_year + 1900);

	outfile << "# TDS: Tcl File to compile Quartus II Project" << endl;
	outfile << "# Project File: " << project_file_tcl(ACTION_COMPILE) << endl;
	outfile << "# Generated on: " << time_stamp << endl;
	outfile << endl;
	outfile << "#\n# Load Quartus II Tcl Project package\n#" << endl;
	outfile << "package require ::quartus::project" << endl;
	outfile << endl;
	outfile << "if {[project_exists " << _project_name << "]} {" << endl;
	outfile << "	puts \"Info: opening existing project " << _project_name << "\"" << endl;
	outfile << "	project_open -revision " << _project_name << " " << _project_name << endl;
	outfile << "} else {" << endl;
	outfile << "    puts \"Info: Quartus project " << _project_name << " does not exist\"" << endl;
	outfile << "    puts \"      please run first \\\"quartus --project\\\"\"" << endl;
	outfile << "    exit 1" << endl;
	outfile << "}" << endl;
	outfile << "if {[is_project_open]} {" << endl;
	outfile << "if {[string compare $quartus(project) \"" << _project_name << "\"]==0} {" << endl;
	outfile << "#\n# Compile project\n#" << endl;
	outfile << "puts \"Info: Compiling project\"" << endl;
	outfile << "load_package flow" << endl;
	outfile << "execute_flow -compile" << endl;
	outfile << endl;
	generate_report(outfile);
	outfile << endl;
	outfile << "project_close" << endl;
	outfile << "exit 0" << endl;
	outfile << "} else {" << endl;
	outfile << "	puts \"Info: The project $quartus(project) was open instead of " << _project_name << "\"" << endl;
	outfile << "	puts \"      the project was not compiled\"" << endl;
	outfile << "}" << endl;
	outfile << "} else {" << endl;
	outfile << "	puts \"Info: Failed to open project " << _project_name << "\"" << endl;
	outfile << "	puts \"      the project was not compiled\"" << endl;
	outfile << "}" << endl;
	outfile << "exit 1" << endl;
}

void Quartus::open_or_create_project(ofstream& outfile) {
	char time_stamp[100];
	time_t ctime = time(NULL);
	struct tm* ctm = gmtime(&ctime);
	sprintf(time_stamp, "%02d:%02d:%02d  %02d %02d, %04d", ctm->tm_hour, ctm->tm_min, ctm->tm_sec, ctm->tm_mon+1, ctm->tm_mday, ctm->tm_year + 1900);

	outfile << "# TDS: Tcl File to create Quartus II Project" << endl;
	outfile << "# Project File: " << project_file_tcl(ACTION_CREATE) << endl;
	outfile << "# Generated on: " << time_stamp << endl;
	outfile << endl;
	outfile << "#\n# Load Quartus II Tcl Project package\n#" << endl;
	outfile << "package require ::quartus::project" << endl;
	outfile << endl;
	outfile << "set need_to_close_project 0" << endl;
	outfile << "set make_assignments 1" << endl;
	outfile << endl;
	outfile << "# Check that the right project is open" << endl;
	outfile << "if {[is_project_open]} {" << endl;
	outfile << "	if {[string compare $quartus(project) " << _project_name << "]==0} {" << endl;
	outfile << "		puts \"Info: project $quartus(project) is open\"" << endl;
	outfile << "        puts \"      project " << _project_name << " cannot be opened\"" << endl;
	outfile << "		set make_assignments 0" << endl;
	outfile << "	}" << endl;
	outfile << "} else {" << endl;
	outfile << "	# Only open if not already open" << endl;
	outfile << "	if {[project_exists " << _project_name << "]} {" << endl;
	outfile << "        puts \"Info: opening existing project " << _project_name << "\"" << endl;
	outfile << "		project_open -revision " << _project_name << " " << _project_name << endl;
	outfile << "	} else {" << endl;
	outfile << "        puts \"Info: Creating project " << _project_name << "\"" << endl;
	outfile << "		project_new -revision " << _project_name << " " << _project_name << endl;
	outfile << "	}" << endl;
	outfile << "	set need_to_close_project 1" << endl;
	outfile << "}" << endl;
	outfile << endl;
	outfile << "#\n# Make assignments\n#" << endl;
	outfile << "if {$make_assignments} {" << endl;
	outfile << "	set_global_assignment -name FAMILY " << Environment::getStr(Environment::FPGA_FAMILY) << endl;
	outfile << "	set_global_assignment -name DEVICE " << Environment::getStr(Environment::FPGA_DEVICE) << endl;
	outfile << "	set_global_assignment -name ORIGINAL_QUARTUS_VERSION 11.0" << endl;
	outfile << "	set_global_assignment -name PROJECT_CREATION_TIME_DATE \"" << time_stamp << "\"" << endl;
	outfile << "	set_global_assignment -name LAST_QUARTUS_VERSION 11.0" << endl;
	outfile << "	set_global_assignment -name VHDL_FILE " << Environment::getStr(Environment::GAUT_TECH_VHD) << endl;
	outfile << "	set_global_assignment -name VHDL_FILE " << Environment::getStr(Environment::GAUT_TECH_LIB_VHD) << endl;
	outfile << "	set_global_assignment -name VHDL_FILE " << main_file_vhd() << endl;
	outfile << "	set_global_assignment -name PARTITION_NETLIST_TYPE SOURCE -section_id Top" << endl;
	outfile << "	set_global_assignment -name PARTITION_FITTER_PRESERVATION_LEVEL PLACEMENT_AND_ROUTING -section_id Top" << endl;
	outfile << "	set_global_assignment -name PARTITION_COLOR 16764057 -section_id Top" << endl;
	outfile << "	set_global_assignment -name POWER_USE_PVA OFF" << endl;
	outfile << "	set_global_assignment -name PHYSICAL_SYNTHESIS_COMBO_LOGIC ON" << endl;
	outfile << "	set_global_assignment -name PHYSICAL_SYNTHESIS_REGISTER_RETIMING ON" << endl;
	outfile << "	set_global_assignment -name PHYSICAL_SYNTHESIS_EFFORT EXTRA" << endl;
	outfile << "	set_global_assignment -name OPTIMIZE_HOLD_TIMING \"IO PATHS AND MINIMUM TPD PATHS\"" << endl;
	outfile << "	set_global_assignment -name FITTER_EFFORT \"STANDARD FIT\"" << endl;
	outfile << "	set_global_assignment -name TIMEQUEST_DO_REPORT_TIMING ON" << endl;
	outfile << "	set_global_assignment -name STRATIX_TECHNOLOGY_MAPPER LUT" << endl;
	outfile << "	set_global_assignment -name STRATIX_OPTIMIZATION_TECHNIQUE SPEED" << endl;
	outfile << "	set_global_assignment -name STRATIXII_OPTIMIZATION_TECHNIQUE SPEED" << endl;
	outfile << "	set_global_assignment -name ALLOW_POWER_UP_DONT_CARE OFF" << endl;
	outfile << "	set_global_assignment -name SYNTH_TIMING_DRIVEN_SYNTHESIS OFF" << endl;
	outfile << "	set_global_assignment -name OPTIMIZE_MULTI_CORNER_TIMING ON" << endl;
	outfile << "	set_instance_assignment -name PARTITION_HIERARCHY root_partition -to | -section_id Top" << endl;
	outfile << endl;
	outfile << "	# Including default assignments" << endl;
	outfile << "	set_global_assignment -name PROGRAMMABLE_POWER_MAXIMUM_HIGH_SPEED_FRACTION_OF_USED_LAB_TILES 1.0" << endl;
	outfile << "	set_global_assignment -name GUARANTEE_MIN_DELAY_CORNER_IO_ZERO_HOLD_TIME ON" << endl;
	outfile << "	set_global_assignment -name OPTIMIZE_POWER_DURING_FITTING \"NORMAL COMPILATION\"" << endl;
	outfile << "	set_global_assignment -name OPTIMIZE_TIMING \"NORMAL COMPILATION\"" << endl;
	outfile << "	set_global_assignment -name FITTER_AGGRESSIVE_ROUTABILITY_OPTIMIZATION AUTOMATICALLY" << endl;
	outfile << "	set_global_assignment -name TOP_LEVEL_ENTITY " << _project_name << endl;
/*
	outfile << "	set_global_assignment -name MAX_RAM_BLOCKS_M512 -1" << endl;
	outfile << "	set_global_assignment -name MAX_RAM_BLOCKS_M4K -1" << endl;
	outfile << "	set_global_assignment -name MAX_RAM_BLOCKS_MRAM -1" << endl;
	outfile << "	set_global_assignment -name MAX_LABS -1" << endl;
	outfile << "	set_global_assignment -name MAX_NUMBER_OF_REGISTERS_FROM_UNINFERRED_RAMS -1" << endl;
*/
	outfile << endl;
	outfile << "	# Commit assignments" << endl;
	outfile << "	export_assignments" << endl;
	outfile << endl;
	outfile << "}" << endl;
	outfile << endl;
	outfile << "#\n# Close project\n#" << endl;
	outfile << "if {$need_to_close_project} {" << endl;
	outfile << "	project_close" << endl;
	outfile << "}" << endl;
}

bool Quartus::report_logic_levels(void) {
	ofstream outfile;
	string logic_level_tcl = _project_name;
	logic_level_tcl += "_logic_level.tcl";
	outfile.open(logic_level_tcl.c_str());
	if (!outfile.is_open()) {
		cerr << "Failed to create tcl project script " << logic_level_tcl << endl;
		return false;
	}
	outfile << "load_package advanced_timing"<< endl;
	outfile << "package require cmdline"<< endl;
	outfile << ""<< endl;
	outfile << "set options {"<< endl;
	outfile << "    { \"name_pattern.arg\" \"*\" \"Restrict to registers matching this pattern\"}"<< endl;
	outfile << "}"<< endl;
	outfile << ""<< endl;
	outfile << "array set opts [::cmdline::getoptions quartus(args) $options]"<< endl;
	outfile << "array set num_levels [list]"<< endl;
	outfile << ""<< endl;
	outfile << "# Open the project and get the revision name"<< endl;
	outfile << "        puts \"Info: Opening existing project " << _project_name << "\"" << endl;
	outfile << "		project_open -revision " << _project_name << " " << _project_name << endl;
	//outfile << "    project_open " << _project_name << " -current_revision"<< endl;
	outfile << "set rev [get_current_revision]"<< endl;
	outfile << ""<< endl;
	outfile << "# Prepare the timing netlist"<< endl;
	outfile << "if { [catch { create_timing_netlist; create_p2p_delays } res] } {"<< endl;
	outfile << "    post_message -type error $res"<< endl;
	outfile << "    project_close"<< endl;
	outfile << "    qexit -error"<< endl;
	outfile << "}"<< endl;
	outfile << ""<< endl;
	outfile << "# Iterate through every register in the design"<< endl;
	outfile << "foreach_in_collection dest [get_timing_nodes -type reg] {"<< endl;
	outfile << ""<< endl;
	outfile << "    # Get a list of keepers (registers, pins, clocks)"<< endl;
	outfile << "    # that feed to the register node"<< endl;
	outfile << "    set delays_from_keepers [get_delays_from_keepers $dest]"<< endl;
	outfile << "    "<< endl;
	outfile << "    # If the destination register name doesn't match the pattern,"<< endl;
	outfile << "    # simply go on to the next one."<< endl;
	outfile << "    set dest_name [get_timing_node_info -info name $dest]"<< endl;
	outfile << "    if { ! [string match $opts(name_pattern) $dest_name] } {"<< endl;
	outfile << "        continue"<< endl;
	outfile << "    }"<< endl;
	outfile << "    # Walk through all keepers feeding the register node"<< endl;
	outfile << "    foreach delay $delays_from_keepers {"<< endl;
	outfile << ""<< endl;
	outfile << "        set src [lindex $delay 0]"<< endl;
	outfile << ""<< endl;
	outfile << "        # Keeper can include pins and clocks, and we want only  registers."<< endl;
	outfile << "        if { ! [string equal \"reg\" [get_timing_node_info -info type $src]] } {"<< endl;
	outfile << "            continue"<< endl;
	outfile << "        }"<< endl;
	outfile << ""<< endl;
	outfile << "        # If the source register name doesn't match the pattern,"<< endl;
	outfile << "        # simply go on to the next one"<< endl;
	outfile << "        set src_name [get_timing_node_info -info name $src]"<< endl;
	outfile << "        if { ! [string match $opts(name_pattern) $src_name] } {"<< endl;
	outfile << "            continue"<< endl;
	outfile << "        }"<< endl;
	outfile << ""<< endl;
	outfile << "        # At this point, both the source and destination names"<< endl;
	outfile << "        # match the pattern, and it's a register-to-register path."<< endl;
	outfile << "        # The get_delay_path command returns a list of nodes on"<< endl;
	outfile << "        # a path. The length of the path is the length of the list."<< endl;
	outfile << "        # The list includes the source and destination registers,"<< endl;
	outfile << "        # so the levels of logic between registers is  - 2"<< endl;
	outfile << "        set path [get_delay_path -type longest -from $src -to $dest]"<< endl;
	outfile << "        set levels_of_logic [expr { [llength $path] - 2 } ]"<< endl;
	outfile << "        "<< endl;
	outfile << "        # Save the information in an array"<< endl;
	outfile << "        if { [info exists num_levels($levels_of_logic)] } {"<< endl;
	outfile << "            incr num_levels($levels_of_logic)"<< endl;
	outfile << "        } else {"<< endl;
	outfile << "            set num_levels($levels_of_logic) 1"<< endl;
	outfile << "        }"<< endl;
	outfile << "    }"<< endl;
	outfile << "}"<< endl;
	outfile << ""<< endl;
	outfile << "project_close"<< endl;
	outfile << ""<< endl;
	outfile << "# Write the information out to a file"<< endl;
	outfile << "if { [catch {open " << _project_name << ".levels_of_logic.csv w} fh] } {"<< endl;
	outfile << "    post_message -type error $fh"<< endl;
	outfile << "} else {"<< endl;
	outfile << ""<< endl;
	outfile << "    # Write a descriptive header into the file"<< endl;
	outfile << "    puts $fh \"Levels of logic for project " << _project_name << "\"" << endl;
	outfile << "    puts $fh \"Reporting paths for register names matching $opts(name_pattern)\""<< endl;
	outfile << "    puts $fh \"Levels of logic,Number in design\""<< endl;
	outfile << ""<< endl;
	outfile << "    foreach level [lsort -integer [array names num_levels]] {"<< endl;
	outfile << ""<< endl;
	outfile << "        if { [catch { puts $fh \"$level,$num_levels($level)\" } res] } {"<< endl;
	outfile << "            post_message -type error $res"<< endl;
	outfile << "            break"<< endl;
	outfile << "        }"<< endl;
	outfile << "    }"<< endl;
	outfile << "    catch { close $fh }"<< endl;
	outfile << "}"<< endl;
	outfile.close();

	string cmd("quartus_tan -t ");
	cmd += logic_level_tcl;

	cmd += string(" 1>")+ _project_name + string(".quartus.logic_level_tcl.msgs");

	return (Util::launchProc(cmd.c_str(),true) ==0);
}
