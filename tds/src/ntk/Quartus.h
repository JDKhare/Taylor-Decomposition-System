/*
 * =====================================================================================
 *
 *       Filename:  Quartus.h
 *    Description:
 *        Created:  07/23/2011 10:53:57 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __QUARTUS_H__
#define __QUARTUS_H__ 1

#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

class Quartus {
private:
	// typedef map<const string&,void(Quartus::*functor)(ofstream&)> FunctorMap;
	string _project_name;
	string _project_postfix;

	map<const string, void (Quartus::*)(ofstream&)> TclActions;
public:
	Quartus(string name, string postfix) : _project_name(name), _project_postfix(postfix) {
		void (Quartus::*functor_p)(ofstream&);
		void (Quartus::*functor_c)(ofstream&);
		functor_p = (&Quartus::open_or_create_project);
		functor_c = (&Quartus::compile_project);
		TclActions[ACTION_CREATE] = functor_p;
		TclActions[ACTION_COMPILE] = functor_c;
	}
	~Quartus(void) {}

	bool check_project_files(void);

	bool generate_quartus_script(const string& action);
	bool execute(const string& action);
	string project_file_tcl(const string& action);

	bool report_logic_levels(void);

	void open_or_create_project(ofstream&);

	void compile_project(ofstream&);
	void generate_report(ofstream&);

	string main_file_vhd(void);
	string report_file(void);

	static const char* ACTION_CREATE;
	static const char* ACTION_COMPILE;
	static const char* EXTENSION_TCL;
	static const char* EXTENSION_REPORT;
	static const char* EXTENSION_VHD;
};

#endif


