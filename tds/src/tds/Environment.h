/*
 * =====================================================================================
 *
 *       Filename:  Enviroment.h
 *    Description:
 *        Created:  05/09/2009 08:00:50 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 198                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-06-26 23:31:32 -0400 (Tue, 26 Jun 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <string>
#include <ostream>
#include <map>

using namespace std;

namespace tds{

	class Environment {
	private:
		static map<string,string> _env;
		Environment(void) {};
		~Environment(void) {};
	public:
		static void clean(void);
		static void setDefault(void);
		static void reset(void) { setDefault(); }
		static void set(const string& key, const string& value);
		static string getStr(const string& key);
		static bool getBool(const string& key);
		static int getInt(const string& key);
		static pair<int,int> getFraction(const string& key);
		static void load(const char*);
		static void save(const char*);
		static void print(ostream& out);
		static void parseSystem(void);

		static const string CONST_PREFIX;
		static const string NEGATIVE_PREFIX;
		//	string showdir;
		//	showdir += "dotfiles";
		static const string REORDER_TYPE;
		static const string DOT_BIN;
		static const string PS_BIN;
		static const string SHOW_DIRECTORY;
		static const string CDFG_BIN_PATH;
		static const string GAUT_BIN_PATH;
		static const string GAUT_LIB_PATH;
		static const string QUARTUS_BIN_PATH;

		static const string GAUT_TECH_LIB;
		static const string GAUT_TECH_VHD;
		static const string GAUT_TECH_LIB_VHD;
		static const string GAUT_SCHEDULE_STRATEGY;
		static const string GAUT_ALLOCATE_STRATEGY;
		static const string GAUT_REGISTER_STRATEGY;
		static const string GAUT_COST_EXTENSION;
		static const string DEFAULT_DESIGN_NAME;
		//
		// Additionally some can be interpreted as booleans
		//
		static const string GAUT_GANTT_GENERATION;
		static const string GAUT_MEM_GENERATION;
		static const string GAUT_SOCLIB_GENERATION;
		static const string GAUT_OPTIMIZE_OPERATOR;
		static const string CONST_AS_VARS;
		static const string CONST_CDFG_EVAL;
		static const string SHOW_BIGFONT;
		static const string SHOW_VERBOSE;
		static const string SHOW_LEVEL;
		//
		// Some other can also be interpreted as integers
		//
		static const string GAUT_CADENCY;
		static const string GAUT_MEM;
		static const string GAUT_CLOCK;
		static const string RMPY;
		static const string RADD;
		static const string RSUB;
		static const string DELAYADD;
		static const string DELAYSUB;
		static const string DELAYMPY;
		static const string DELAYLSH;
		static const string DELAYRSH;
		static const string DELAYREG;
		static const string BITWIDTH_INTEGER;
		static const string BITWIDTH_FIXEDPOINT;
		//
		// FPGA
		//
		static const string FPGA_FAMILY;
		static const string FPGA_DEVICE;
		//
		// Environment variables
		//
		static const string DEF_ENV_PATH;
		static const string DEF_ENV_QUARTUS;
		//
		static const string DEF_PRG_VSIM;
		static const string DEF_PRG_PSVIEW;
		static const string DEF_PRG_QUARTUS;
		static const string DEF_PRG_DOT;
		static const string DEF_PRG_GAUT;
		static const string DEF_PRG_CDFG;
		//
		static const string LIB_GAUT;

	};

}
#endif

