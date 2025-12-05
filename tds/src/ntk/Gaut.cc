/*
 * =====================================================================================
 *
 *       Filename:  Gaut.cc
 *    Description:
 *        Created:  04/06/2011 08:13:56 PM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "Gaut.h"
#include "Environment.h"
#include "util.h"
#include "types.h"

namespace network {
	using namespace std;
	using util::Util;
	using tds::Environment;

	Gaut::Gaut(string cdfg,int cadency,int mem, int clock,const char* library) {
		assert(!cdfg.empty());
		_cdfg_base_name = cdfg;
		assert(cadency>0);
		assert(mem>0);
		assert(clock>0);
		assert(library);
		_bin = Environment::getStr(Environment::GAUT_BIN_PATH)+ Environment::DEF_PRG_GAUT;
		_builtins = string(" -builtins ")+ Environment::getStr(Environment::GAUT_LIB_PATH)+ Util::fileSeparator + "CDFG_functions.txt";
		_lib = string(" -lib ")+ Environment::getStr(Environment::GAUT_LIB_PATH)+Util::fileSeparator+library;
		_switch = " -f -cadency ";
		_switch += Util::itoa(cadency);
		_switch += " -mem ";
		_switch += Util::itoa(mem);
		_switch += " -clock ";
		_switch += Util::itoa(clock);
		if (Environment::getBool(Environment::GAUT_OPTIMIZE_OPERATOR)) {
			_switch += " -operator_optimization ";
		}
		_switch += " -registerAllocation ";
		_switch += Environment::getStr(Environment::GAUT_REGISTER_STRATEGY);
		if (!Environment::getStr(Environment::GAUT_SCHEDULE_STRATEGY).empty()) {
			_switch += " -scheduling ";
			_switch += Environment::getStr(Environment::GAUT_SCHEDULE_STRATEGY);
		}
		_switch += " -vhdl_type fsm_sigs ";
		_switch += "-distributed_th_lb -IHM";
		_executed = false;
	}

	Gaut::Gaut(string cdfg) {
		assert(!cdfg.empty());
		_cdfg_base_name = cdfg;
		_bin = Environment::getStr(Environment::GAUT_BIN_PATH) + Environment::DEF_PRG_GAUT;
		_builtins = string(" -builtins ")+ Environment::getStr(Environment::GAUT_LIB_PATH) + "CDFG_functions.txt";
		_lib = string(" -lib ")+ Environment::getStr(Environment::GAUT_LIB_PATH)+Util::fileSeparator+Environment::getStr(Environment::GAUT_TECH_LIB);
		_switch = " -f -cadency ";
		_switch += Environment::getStr(Environment::GAUT_CADENCY);
		_switch += " -mem ";
		_switch += Environment::getStr(Environment::GAUT_MEM);
		_switch += " -clock ";
		_switch += Environment::getStr(Environment::GAUT_CLOCK);
		_switch += " -registerAllocation ";
		_switch += Environment::getStr(Environment::GAUT_REGISTER_STRATEGY);
		if (!Environment::getBool(Environment::GAUT_GANTT_GENERATION)) {
			_switch += " -ngantt";
		}
		if (!Environment::getBool(Environment::GAUT_MEM_GENERATION)) {
			_switch += " -nmem";
		}
		if (!Environment::getBool(Environment::GAUT_SOCLIB_GENERATION)) {
			_switch += " -nsoclib";
		}
		if (Environment::getBool(Environment::GAUT_OPTIMIZE_OPERATOR)) {
			_switch += " -operator_optimization";
		}
#if 0
		// use structural vhdl (no flag)
		_switch += "-vhdl_type fsm_sigs ";
#endif
		if (!Environment::getStr(Environment::GAUT_SCHEDULE_STRATEGY).empty()) {
			_switch += " -scheduling ";
			_switch += Environment::getStr(Environment::GAUT_SCHEDULE_STRATEGY);
		}
		_switch += " ";
		_switch += Environment::getStr(Environment::GAUT_ALLOCATE_STRATEGY);
		_switch += " -IHM";
		_executed = false;
	}

	bool Gaut::run(void) {
		_executed = true;
		try {
			string cdfg_file = string(" -cdfg ")+ _cdfg_base_name+ string(".cdfg");
			string vhdl_file = string(" -vhdl_prefix ")+ _cdfg_base_name;
			string dump_file = _cdfg_base_name+ string(".dump");
			string gaut_exec = _bin + cdfg_file + _builtins + _lib + vhdl_file + _switch;
#if _DEBUG
			gaut_exec += " -dumpCDFG ";
			gaut_exec += _cdfg_base_name;
			gaut_exec += "_dump.cdfg";
#endif
			gaut_exec += string(" 1>")+ dump_file;
			int ret = system(gaut_exec.c_str());
			if(ret) {
				_executed = false;
				cerr << "Info: Gaut returned code " << ret << " at " << _cdfg_base_name << endl;
			}
		} catch(...) {
			_executed = false;
			cerr << "Error: Gaut execution was aborted at " << _cdfg_base_name << endl;
		}
		return _executed;
	}

	FixCost* Gaut::compute_cost(void) {
		FixCost* cost = new FixCost();
		if (_executed) {
			string cost_file = _cdfg_base_name;
			cost_file += Environment::getStr(Environment::GAUT_COST_EXTENSION);
			//ifstream in;
			//in.open(cost_file.c_str());
			try {
				ifstream in(cost_file.c_str());
				if (in.is_open()) {
					string line;
					vector<string> envToken(2);
					while(Util::readline(in,line,"#")!=0) {
						envToken = Util::split(line, "=");
						if(envToken.size() ==2) {
							string key = Util::trim(envToken[0]);
							string svalue = Util::trim(envToken[1]);
							pair<bool,wedge> value = Util::atoi(svalue.c_str());
							if(value.first) {
								if(key=="mux2to1") {
									cost->muxes = value.second;
								} else if(key=="cdfg_latency") {
									cost->latency = value.second;
								} else if(key=="ffs") {
									cost->registers = value.second;
								} else if(key=="op_area") {
									cost->area = value.second;
								}
							}
							envToken.clear();
						}
					}
					in.close();
				} else {
					cerr << "Error: Failed to open cost file " << cost_file << " associated to Gaut " << endl;
				}
			} catch(...) {
				cerr << "Error: ifstream failed" << endl;
			}
		}
		return cost;
	}

}

