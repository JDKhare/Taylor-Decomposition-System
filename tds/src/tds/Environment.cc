/*
 * =====================================================================================
 *
 *       Filename:  Environment.cc
 *    Description:
 *        Created:  05/09/2009 08:05:52 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 200                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include "Environment.h"
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

#include "util.h"
using util::Util;

namespace tds {

	map<string,string> Environment::_env;

		const string Environment::CONST_PREFIX = "const_prefix";
		const string Environment::NEGATIVE_PREFIX = "negative_prefix";
		const string Environment::REORDER_TYPE = "reorder_type";
		const string Environment::DOT_BIN = "dot_bin";
		const string Environment::PS_BIN = "ps_bin";

		const string Environment::SHOW_DIRECTORY = "show_directory";
		const string Environment::CDFG_BIN_PATH = "cdfg_bin_path";
		const string Environment::GAUT_BIN_PATH = "gaut_bin_path";
		const string Environment::GAUT_LIB_PATH = "gaut_lib_path";
		const string Environment::QUARTUS_BIN_PATH = "quartus_bin_path";

		const string Environment::FPGA_DEVICE = "fpga_device";
		const string Environment::FPGA_FAMILY = "fpga_family";

		const string Environment::GAUT_TECH_LIB = "gaut_tech_lib";
		const string Environment::GAUT_TECH_VHD = "gaut_tech_vhd";
		const string Environment::GAUT_TECH_LIB_VHD = "gaut_tech_lib_vhd";
		const string Environment::GAUT_SCHEDULE_STRATEGY = "gaut_schedule_strategy";
		const string Environment::GAUT_ALLOCATE_STRATEGY = "gaut_allocate_strategy";
		const string Environment::GAUT_REGISTER_STRATEGY = "gaut_register_strategy";
		const string Environment::GAUT_COST_EXTENSION = "gaut_cost_extension";
		const string Environment::DEFAULT_DESIGN_NAME = "default_design_name";
		//
		// Additionally some can be interpreted as booleans
		//
		const string Environment::GAUT_GANTT_GENERATION = "gaut_gantt_generation";
		const string Environment::GAUT_MEM_GENERATION = "gaut_mem_generation";
		const string Environment::GAUT_SOCLIB_GENERATION = "gaut_soclib_generation";
		const string Environment::GAUT_OPTIMIZE_OPERATOR = "gaut_optimize_operator";
		const string Environment::CONST_AS_VARS = "const_as_vars";
		const string Environment::CONST_CDFG_EVAL = "const_cdfg_eval";
		const string Environment::SHOW_BIGFONT = "show_bigfont";
		const string Environment::SHOW_VERBOSE = "show_verbose";
		const string Environment::SHOW_LEVEL = "show_level";
		//
		// Some other can also be interpreted as integers
		//
		const string Environment::GAUT_CADENCY = "gaut_cadency";
		const string Environment::GAUT_MEM = "gaut_mem";
		const string Environment::GAUT_CLOCK = "gaut_clock";
		const string Environment::RMPY = "rMPY";
		const string Environment::RADD = "rADD";
		const string Environment::RSUB = "rSUB";
		const string Environment::DELAYADD = "delayADD";
		const string Environment::DELAYSUB = "delaySUB";
		const string Environment::DELAYMPY = "delayMPY";
		const string Environment::DELAYLSH = "delayLSH";
		const string Environment::DELAYRSH = "delayRSH";
		const string Environment::DELAYREG = "delayREG";
		const string Environment::BITWIDTH_INTEGER = "bitwidth_integer";
		const string Environment::BITWIDTH_FIXEDPOINT = "bitwidth_fixedpoint";
		//
		// Environment variables
		//
		const string Environment::DEF_ENV_PATH = "PATH";
		const string Environment::DEF_ENV_QUARTUS = "QUARTUS_ROOTDIR";
		//
		// Program names
		//
		const string Environment::DEF_PRG_VSIM = "modelsim";
		const string Environment::DEF_PRG_PSVIEW = "gsview";
		const string Environment::DEF_PRG_QUARTUS = "quartus";
		const string Environment::DEF_PRG_DOT = "Graphviz";
		const string Environment::DEF_PRG_CDFG = "cdfgcompiler";
#if defined(_WIN32)
		const string Environment::DEF_PRG_GAUT = "gaut" _TDS_ARCH_ ".exe";
#else
		const string Environment::DEF_PRG_GAUT = "gaut";
#endif
		//
		// Library names
		//
		const string Environment::LIB_GAUT = "lib";



	void Environment::setDefault(void) {
		//
		// All environment variables are strings
		//
		_env[CONST_PREFIX] = "const_";
		_env[NEGATIVE_PREFIX] = "moins_";
		//	string showdir = Util::fileSeparator;
		//	showdir += "dotfiles";
		_env[REORDER_TYPE] = "proper";
		_env[DOT_BIN] = "dot";
#if defined(_WIN32)

		_env[SHOW_DIRECTORY] = "dotfiles";
		_env[CDFG_BIN_PATH] = "C:\\Users\\daniel\\Documents\\Gaut2.4.3\\GautC\\cdfgcompiler\\bin\\";

		#define _TDS_PRJ_PATH_ "C:\\Users\\daniel\\Documents\\TDS\\workingTds\\"
		// NOTE:
		// the variables below _TDS_ARCH_, etc are defined in the Debug Property page of the Visual Studio Solution
		// there is one variable _TDS_PATH_ that contains the same information as _TDS_PRJ_PATH_ but it seems that 
		// the ending \" fools visual studio and the string becomes malformed

		_env[PS_BIN] = "gsview" _TDS_ARCH_ ".exe";
		_env[GAUT_BIN_PATH] = _TDS_PRJ_PATH_  _TDS_CONF_ "\\" _TDS_PLATF_ "Gaut\\";
		_env[GAUT_LIB_PATH] = _TDS_PRJ_PATH_ "src\\3rdparty\\gaut\\lib\\";
#else
		_env[SHOW_DIRECTORY] = "./dotfiles";
		_env[PS_BIN] = "evince";
		_env[CDFG_BIN_PATH] = "~/Gaut/ver2_4_2/GautC/cdfgcompiler/bin/";
		_env[GAUT_BIN_PATH] = "~/Code/tds/workingTds/src/3rdparty/gaut/src/";
		_env[GAUT_LIB_PATH] = "~/Code/tds/workingTds/src/3rdparty/gaut/lib/";
#endif
		
		_env[QUARTUS_BIN_PATH] = "";
		_env[FPGA_FAMILY] = "\"Stratix II\""; 	/* "\"Stratix IV\"" */
		_env[FPGA_DEVICE] = "AUTO";
		_env[GAUT_TECH_LIB] = "notech_16b.lib";
		_env[GAUT_TECH_VHD] = "notech.vhd";
		_env[GAUT_TECH_LIB_VHD] = "notech_lib.vhd";
		_env[GAUT_SCHEDULE_STRATEGY] = "force_no_pipeline";
		_env[GAUT_ALLOCATE_STRATEGY] = "-distributed_th_lb";
		_env[GAUT_REGISTER_STRATEGY] = "0";
		_env[GAUT_COST_EXTENSION] = ".gcost";
		_env[DEFAULT_DESIGN_NAME] = "tds2ntl";
		//
		// Additionally some can be interpreted as booleans
		//
		_env[GAUT_GANTT_GENERATION] = "false";
		_env[GAUT_MEM_GENERATION] = "false";
		_env[GAUT_SOCLIB_GENERATION] = "false";
		_env[GAUT_OPTIMIZE_OPERATOR] = "true";
		_env[CONST_AS_VARS] = "false";
		_env[CONST_CDFG_EVAL] = "false";
		_env[SHOW_BIGFONT] = "false";
		_env[SHOW_VERBOSE] = "false";
		_env[SHOW_LEVEL] = "true";
		//
		// Some other can also be interpreted as integers
		//
		_env[GAUT_CADENCY] = "200";
		_env[GAUT_MEM] = "10";
		_env[GAUT_CLOCK] = "10";
		_env[RMPY] = "0";
		_env[RADD] = "0";
		_env[RSUB] = "0";
		_env[DELAYADD] = "1";
		_env[DELAYSUB] = "1";
		_env[DELAYMPY] = "2";
		_env[DELAYLSH] = "1";
		_env[DELAYRSH] = "1";
		_env[DELAYREG] = "1";
		_env[BITWIDTH_INTEGER] = "16";
		_env[BITWIDTH_FIXEDPOINT] = "4,12";
	}

	void Environment::set(const string& key, const string& value) {
		if(_env.find(key)!=_env.end()) {
			_env[key]=value;
		} else {
			throw(string("03001. UNKNOWN Environment variable \"")+ key + string("\""));
		}
	}

	string Environment::getStr(const string& key) {
		if(_env.find(key)!=_env.end()) {
			return _env[key];
		}
		throw(string("03002. UNKNOWN Environment string variable \"")+ key + string("\""));
	}

	bool Environment::getBool(const string& key) {
		if(_env.find(key)!=_env.end()) {
			if(_env[key] == "true")
				return true;
			else
				return false;
		}
		throw(string("03003. UNKNOWN Environment bool variable \"")+ key + string("\""));
	}

	int Environment::getInt(const string& key) {
		if(_env.find(key)!=_env.end()) {
			pair<bool,int> value = Util::atoi(_env[key].c_str());
			if(value.first)
				return value.second;
			else
				return 1;
		}
		throw(string("03019. UNKNOWN Environment int variable \"")+ key + string("\""));
	}

	pair<int,int> Environment::getFraction(const string& key) {
		if(_env.find(key)!=_env.end()) {
			vector<string> fraction = Util::split(_env[key],",");
			if(2==fraction.size()) {
				pair<bool,int> value_int = Util::atoi(fraction[0].c_str());
				pair<bool,int> value_fra = Util::atoi(fraction[1].c_str());
				if(value_int.first && value_fra.first)
					return pair<int,int>(value_int.second,value_fra.second);
				else
					return pair<int,int>(0,0);
			} else {
				throw(string("03020. Environment \"")+ key + string("\" must have a comma separated decimal and fractional part."));
			}
		}
		throw(string("03019. UNKNOWN Environment variable \"")+ key + string("\""));
	}

	void Environment::clean(void) {
		_env.clear();
	}

	void Environment::print(ostream& out) {
		out.fill(' ');
		out.width(22);
		out << "# Environment variable  |  Value " << endl;
		out << "#-----------------------|---------------------" << endl;
		for(map<string,string>::iterator it = _env.begin(), end = _env.end(); it!=end; it++) {
			out << " ";
			out.fill(' ');
			out.width(22);
			out << it->first << " | " << it->second << endl;
		}
		out << "#---------------------------------------------" << endl;
		out << "# Other possible values | " << endl;
		out << "#------------------------ " << endl;
		out << "# gaut_schedule_strategy  {\"\", \"force_no_pipeline\", \"force_no_mobility\", \"no_more_stage\", \"\"}" << endl;
		out << "# gaut_allocate_strategy  {\"-distributed_th_lb\", \"-distributed_reuse_th\", \"-distributed_reuse_pr_ub\", \"-distributed_reuse_pr\", \"-global_pr_lb\", \"-global_pr_ub\"}" << endl;
		out << "# gaut_register_strategy  {\"0\", \"1\", \"2\", \"3\"}" << endl;
		out << "#                           0 MWBM [default]" << endl;
		out << "#                           1 MLEA" << endl;
		out << "#                           2 Left edge" << endl;
		out << "#                           3 None" << endl;
		out << "#                 ps_bin  {\"evince\", \"gv\", \"gsview32.exe\"}" << endl;
		out << "#           reorder_type  {\"proper\", \"swap\", \"reloc\"}" << endl;
	}

	void Environment::load(const char* filename) {
		ifstream in(filename);
		if(in.is_open()) {
			string line;
			vector<string> envToken(2);
			while(Util::readline(in, line, "#")!= 0) {
				envToken = Util::split(line, "|=");
				if(envToken.size() ==2) {
					string key = Util::trim(envToken[0]);
					string value = Util::trim(envToken[1]);
					Environment::set(key, value);
				}
				envToken.clear();
			}
			in.close();
		}
	}

	void Environment::save(const char* filename) {
		ofstream ofile(filename);
		if(ofile.is_open()) {
			ofile << "#######################################" << endl;
			ofile << "# Environment file generated by TDS.  #" << endl;
			ofile << "# http://incascout.ecs.umass.edu/main #" << endl;
			ofile << "#######################################" << endl;
			Environment::print(ofile);
			ofile.close();
		}
	}

	void Environment::parseSystem(void) {
#ifdef _WIN32
		Util::SystemEnvironment sysenv = Util::getEnvironment();
		bool has_cdfg = false;
		bool has_quartus = false;
		bool has_vsim = false;
		bool has_dot = false;
		bool has_psview = false;
		if (sysenv.find(DEF_ENV_PATH)!= sysenv.end()) {
			vector<string> program_path = Util::split(sysenv[DEF_ENV_PATH],";");
			//cout << "program_path size = " << program_path.size() << endl;
			for (int i=0; i < program_path.size(); i++) {
				string current_path = program_path[i];
				//cout << "program_path[" << i << "] = " << current_path << endl;
				if (current_path.find(DEF_PRG_CDFG) != string::npos) {
					current_path += Util::fileSeparator;
					has_cdfg = true;
					if (_env[CDFG_BIN_PATH].empty()) {
						_env[CDFG_BIN_PATH] = current_path;
					} else if ( _env[CDFG_BIN_PATH] != current_path) {
						cout << "Info: The CDFG compiler in the tds environment points to " << _env[CDFG_BIN_PATH] << endl;
						cout << "      but Windows' environment has the CDFG compiler at " << current_path << endl;
					}
					string common_path = current_path;
					common_path.erase(current_path.find(DEF_PRG_CDFG));
					if (!common_path.empty()) {
						common_path.erase(common_path.end()-1);
						if (_env[GAUT_BIN_PATH].empty()) {
							_env[GAUT_BIN_PATH] = common_path + Util::fileSeparator + string("bin") +Util::fileSeparator;
						}
						if (_env[GAUT_LIB_PATH].empty()) {
							_env[GAUT_LIB_PATH] = common_path + Util::fileSeparator + string("lib") + Util::fileSeparator;
						}
					}
			 } else if (current_path.find(DEF_PRG_VSIM) != string::npos) {
					has_vsim = true;
				} else if (current_path.find(DEF_PRG_QUARTUS) != string::npos) {
					has_quartus = true;
				} else if (current_path.find(DEF_PRG_DOT) != string::npos) {
					has_dot = true;
				} else if (current_path.find(DEF_PRG_PSVIEW) != string::npos) {
					has_psview = true;
				}
			}
		}
		if (sysenv.find(DEF_ENV_QUARTUS)!=sysenv.end()) {
			if (!sysenv[DEF_ENV_QUARTUS].empty()) {
				has_quartus = true;
				_env[QUARTUS_BIN_PATH] = sysenv[DEF_ENV_QUARTUS] + Util::fileSeparator + "bin" + Util::fileSeparator;
			}
		}
		if (!has_cdfg) {
			cout << "Info: Cannot find " << DEF_PRG_GAUT << " in the system" << endl;
		}
		if (!has_quartus) {
			cout << "Info: Cannot find " << DEF_PRG_QUARTUS << " in the system" << endl;
		}
		if (!has_vsim) {
			cout << "Info: Cannot find" << DEF_PRG_VSIM << " in the system" << endl;
		}
		if (!has_dot) {
			cout << "Info: Cannot find " << DEF_PRG_DOT << " in the system" << endl;
		}
		if (!has_psview) {
			cout << "Info: Cannot find " << DEF_PRG_PSVIEW << " in the system" << endl;
		}
#endif
	}

}
