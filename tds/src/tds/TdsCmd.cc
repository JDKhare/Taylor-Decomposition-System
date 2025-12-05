/*
 * =====================================================================================
 *
 *        Filename:  Tds.cc
 *     Description:  Tds class
 *         Created:  04/19/2007 02:04:40 AM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 209                                          $: Revision of last commit
 *  $Author:: daniel@linux                                   $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include <string>
#include <map>
#include <climits>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdio>
#include <cstdarg>
#include <cstring>
using namespace std;

#include "Bitwidth.h"
using data::Bitwidth;

#include "Quartus.h"
#include "TedOrderCostCustom.h"
#include "TedOrderStrategy.h"
#include "TedOrderCost.h"
#include "TedMan.h"
#include "TedNode.h"
#include "ETedNode.h"
#include "TedVarMan.h"
#include "TedDecompose.h"
#include "XMLImport.h"
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
#include "DfgOperator.h"
#include "DfgBitReduction.h"
using dfg::DfgMan;
using dfg::DfgOperator;
using dfg::DfgBitReduction;

#include "pnode.h"
#include "poly.h"
using polyparser::PolyParser;

using polyparser::PNode;

#include "Ntk.h"
#include "Gaut.h"
using namespace network;

#include "ParseOption.h"
#include "util.h"
using util::Util;

#include "matrice.h"

#include "Environment.h"
#include "Tds.h"

#include "Convert.h"
using namespace convert;

namespace tds {

	/** @brief Internal linkage*/
	namespace {
		/**
		 * @brief Prints a common header help
		 *
		 * @param cmdName the command name
		 * @param brief a brief description of the command
		 * @param moreOptions list of specific options for the command
		 */
		inline void printHelpHeader(const char* cmdName, const char* brief, const char* options) {
			cout << "NAME" << endl;
			cout << "  " << cmdName << " - " << brief << endl;
			cout << "SYNOPSIS" <<endl;
			cout << "  " << cmdName << " " << options << endl;
			cout << "OPTIONS" << endl;
			cout << "  -h, --help" << endl;
			cout << "    Print this message." << endl;
		}
		const char* CMD_OPTIONS = "[options]";
		const char* CMD_NO_OPTIONS = "";
		const char* BRIEF_LISTVARS = "List the variables in a top to bottom order.";
		const char* BRIEF_TED2DFG = "Generate a DFG from the TED.";
		const char* BRIEF_DFG2TED = "Generate a TED from the DFG.";
		const char* BRIEF_POLY = "Construct a TED from a polynomial expression.";
		const char* BRIEF_SCSE = "CSE Static, extract one candidate at a time.";
		const char* BRIEF_BOTTOM_SCSE = "CSE Static, move candidates to bottom.";
		const char* BRIEF_BOTTOM_DCSE = "CSE Dynamic, move candidates to bottom.";
		const char* BRIEF_DCSE = "CSE Dynamic, extract all candidates available.";
		const char* BRIEF_LCSE = "CSE for linearized TED.";
		const char* BRIEF_CANDIDATE = "Show the candidates expression for CSE.";
		const char* BRIEF_TRANSFORM = "Construct a TED from a set of DSP transforms.";
		const char* BRIEF_SHOW = "Show the TED, DFG or Netlist graph.";
		const char* BRIEF_PURGE = "Purge the TED, DFG and/or Netlist.";
		const char* BRIEF_VARS = "Preset the order of variables, before any TED is given.";
		const char* BRIEF_BBLUP = "Move up the given variable one position.";
		const char* BRIEF_BBLDOWN = "Move down the given variable one position.";
		const char* BRIEF_BOTTOM = "Move the given variable to the bottom.";
		const char* BRIEF_TOP = "Move the given variable to the top.";
		const char* BRIEF_RELOC = "Relocate the given variable to the desired position.";
		const char* BRIEF_DFG2NTL = "Generate a Netlist from the DFG.";
		const char* BRIEF_PRINT = "Print out TED information: statistic, etc.";
		const char* BRIEF_READ = "Read a script, a CDFG, a TED or a DFG.";
		const char* BRIEF_WRITE = "Write the existing NTL|DFG|TED into a*.[cdfg|dfg|ted] file.";
		const char* BRIEF_NTL2TED = "Extract from a Netlist all parts representable by TED.";
		const char* BRIEF_TED2NTL = "Generate a Netlist from the TED.";
		const char* BRIEF_SUB = "Substitute an arithmetic expression by a variable";
		const char* BRIEF_ERASE = "Erase a primary output from the TED.";
		const char* BRIEF_EXTRACT = "Extract primary outputs from the TED or Netlist.";
		const char* BRIEF_LINEARIZE = "Transform a non linear TED into a linear one.";
		const char* BRIEF_SIFT = "Heuristically optimize the level of the variable.";
		const char* BRIEF_FLIP = "Flip the order of a linearized variable.";
		const char* BRIEF_REORDER = "Reorder the variables in the TED (Pre-fixed cost).";
		const char* BRIEF_CUSTOM_REORDER = "Reorder the variables in the TED. (User defined cost)";
		const char* BRIEF_DFACTOR = "Dynamic factorization.";
		const char* BRIEF_SHIFTER = "Replace TED edge multiplications by TED node constant variables.";
		const char* BRIEF_SHIFT = "Replace constant multipliers by shifters.";
		const char* BRIEF_DFG_BALANCE = "Balance the DFG or Netlist to minimize latency.";
		const char* BRIEF_DFG_AREA = "Balance the DFG to minimize the area.";
		const char* BRIEF_DFG_SCHEDULE = "Perform the scheduling of the DFG.";
		const char* BRIEF_NTL_PRINT = "Print out statistics of the Netlist.";
		const char* BRIEF_UNLINEARIZE = "Un-linearize a DFG linearized in TED.";
		const char* BRIEF_REMAP_SHIFTER = "Remap chain of constant multipliers and additions in the DFG by shifters (<<).";
		const char* BRIEF_DECOMPOSE = "Decompose the TED in its Normal Factor Form.";
		const char* BRIEF_SET = "Set the variable bitwidth and other options.";
		const char* BRIEF_PRINTENV = "Print the environment variables.";
		const char* BRIEF_SETENV = "Set a environment variable.";
		const char* BRIEF_LOADENV = "Load the environment.";
		const char* BRIEF_SAVEENV = "Save the environment.";
		const char* BRIEF_COMPUTE = "Annotate the bitwidths required for exact computation.";
		const char* BRIEF_OPTIMIZE = "Minimizes the bitwidth of a DFG keeping the maximum error bound.";
		const char* BRIEF_EXPLORE = "Performs a space exploration of the architecture, on development.";
		const char* BRIEF_RETIME = "Performs (forward/backward) retiming in TED.";
		const char* BRIEF_JUMPABOVE = "Moves a variable above another one.";
		const char* BRIEF_JUMPBELOW = "Moves a variable below another one.";
		const char* BRIEF_EXCHANGE = "Exchange the position of two variables.";
		const char* BRIEF_COST = "Prints out the cost associated to this TED.";
		const char* BRIEF_EVALUATE_CONSTANTS = "Evaluates explicit DFG constants";
		const char* BRIEF_EVAL = "Evaluates a TED node.";
		const char* BRIEF_REORDERRETIMEDVARIABLES = "Fixes the order broken by a retime operation.";
		const char* BRIEF_QUARTUS = "Generates a Quartus file project, compiles it and report its frequency and logic elements.";
		const char* BRIEF_VERIFY = "Verifies that two TED outputs are the same.";
		const char* BRIEF_DFGRELINK = "Smooths out all DFG outputs used as DFG inputs";
	}

#define CATCH_ERRORS catch(const string &_error) { \
	cerr << "Error: " << _error << endl; \
	return CMD_ERROR; \
} catch(...) { \
	cerr << "Fatal Error!" << endl; \
	return CMD_FATAL; \
}

#define MAN_REORDER_SCHEME \
	"  [comparison scheme]" << endl; \
cout << "  -le, --lessequal" << endl; \
cout << "    By default the cost are compared using a strict less rule, this scheme" << endl; \
cout << "    does not accept any swap with the same cost. To accept a swap with the" << endl; \
cout << "    same cost use this flag." << endl; \
cout << "    NOTE 1: It is important to define this flag before defining the ordering cost" << endl; \
cout << "    NOTE 2: Useful under sift and annealing iterations, useless under exhaustive"

#define MAN_REORDER_ITERATION_STRATEGY \
	"  [iteration strategy]" << endl; \
cout << "  -a, --annealing [--no_stride] [--stride_backtrack]" << endl; \
cout << "    It minimizes the cost function by using the annealing algorithm" << endl; \
cout << "    Sub option: --no_stride" << endl; \
cout << "    Prevents (groups of a) multiple output TEDs to be treated as individual TEDs" << endl; \
cout << "    Sub option: --stride_backtrack" << endl; \
cout << "    Enables backtracking after annealing each single stride" << endl; \
cout << "  -e, --exhaustive [--end] [--no_stride]" << endl; \
cout << "    Tries all possible orders by doing permutation, is O(N!)" << endl; \
cout << "    Sub option: --end" << endl; \
cout << "    Prevents the abortion of the permutation when 'ESC' is pressed" << endl; \
cout << "    Sub option: --no_stride" << endl; \
cout << "    Prevents (groups of a) multiple output TEDs to be treated as individual TEDs" << endl; \
cout << "  -s, --sift [-g, --group]" << endl; \
cout << "    Moves each variable at a time through out the height of the TED" << endl; \
cout << "    graph till its best position is found, is O(N^2)[DEFAULT BEHAVIOR]." << endl; \
cout << "    Sub option: -g, --group" << endl; \
cout << "    This option only affects the SIFT algorithm. If grouping is selected, " << endl; \
cout << "    the sifting is done preserving the relative grouping of all variables" << endl; \
cout << "    that were linearized. If not every single variable is moved regardless" << endl; \
cout << "    of its grouping."

#define MAN_REORDER_ALGORITHM \
	"  [reorder algorithm]" << endl; \
cout << "  -p, --proper" << endl; \
cout << "    The proper algorithm to reorder the TED as described in the paper" << endl; \
cout << "    http:/" << "/doi.ieeecomputersociety.org/10.1109/HLDVT.2004.1431235" << MARK_DEFAULT_REORDER("proper") << endl; \
cout <<	"  -r, --reloc" << endl; \
cout << "    Reconstructs the TED with the desired order of variables. This is" << endl; \
cout << "    slow, but serves as golden reference" << MARK_DEFAULT_REORDER("reloc")<< endl; \
cout << "  -s, --swap" << endl; \
cout << "    A Hybrid implementation, that re-structures the TED with the desired" << endl; \
cout << "    orden, but internally uses reconstructs and operations on the bottom" << endl; \
cout << "    graph" << MARK_DEFAULT_REORDER("swap")

#define MAN_REORDER_COST  \
	"  [cost functions]" << endl; \
cout << "     Definitions:" <<endl; \
cout << "       ted_subexpr_candidates = # of product terms in the TED with 2+ parents connecting to ONE" << endl; \
cout << "       ted_nodes              = # of nodes in the TED graph" << endl; \
cout << "       nMUL                   = # of multiplications in the DFG graph" << endl; \
cout << "       nADD                   = # of additions in the DFG graph" << endl; \
cout << "       nSUB                   = # of substractions in the DFG graph" << endl; \
cout << "       rMPY                   = # of multipliers after scheduling the DFG" << endl; \
cout << "       rADD                   = # of adders and subtractors after scheduling the DFG" << endl; \
cout << "       dLatency               = the latency of the DFG" << endl; \
cout << "       gLatency               = the latency of the Gaut implementation" << endl; \
cout << "     Environment variables used to set the:" << endl; \
cout << "       1) delay of the DFG operators:" << endl; \
cout << "          " << Environment::DELAYADD  << " [Default value = " << Environment::getInt(Environment::DELAYADD) << "]" << endl; \
cout << "          " << Environment::DELAYSUB  << " [Default value = " << Environment::getInt(Environment::DELAYSUB) << "]" << endl; \
cout << "          " << Environment::DELAYMPY  << " [Default value = " << Environment::getInt(Environment::DELAYMPY) << "]" << endl; \
cout << "          " << Environment::DELAYREG  << " [Default value = " << Environment::getInt(Environment::DELAYREG) << "]" << endl; \
cout << "       2) maximum number of resources used by the DFG scheduler: " << endl; \
cout << "          " << Environment::RMPY  << " [Default value = " <<((Environment::getInt(Environment::RMPY) == 0) ? UINT_MAX : Environment::getInt(Environment::RMPY)) << "]" << endl; \
cout << "          " << Environment::RADD << " [Default value = " <<((Environment::getInt(Environment::RADD) == 0) ? UINT_MAX : Environment::getInt(Environment::RADD)) << "]" << endl; \
cout << "          " << Environment::RSUB  << " [Default value = " <<((Environment::getInt(Environment::RSUB) == 0) ? UINT_MAX : Environment::getInt(Environment::RSUB)) << "]" << endl; \
cout << "  --node" << endl; \
cout << "    Minimizes the function \"10*ted_nodes - ted_subexpr_candidates\"" << endl; \
cout << "   -nm, --nMUL {legacy -m, --mul}" << endl; \
cout << "    Minimizes the function \"10*nMUL - ted_subexpr_candidates\" [DEFAULT BEHAVIOR]" << endl; \
cout << "  --op" << endl; \
cout << "    Minimizes the function \"10*(nMUL + nADD + nSUB)- ted_subexpr_candidates\"" << endl; \
cout << "  --opscheduled" << endl; \
cout << "    Minimizes nMUL, followed by(nADD+nSUB), dfg latency, rMPY, rADD" << endl; \
cout << "  -dl,--dLatency {legacy --latency}" << endl; \
cout << "    Minimizes the DFG latency subject to the resources specified in the environment variables" << endl; \
cout << "  --bitwidth" << endl; \
cout << "    Minimizes the bitwidth of the HW implementation subject to unlimited latency|resources" << endl; \
cout << "  -gm, --gMUX {legacy --gmux}" << endl; \
cout << "    Minimizes the Gaut mux count in the Gaut implementation(each mux is considered 2 to 1)" <<endl; \
cout << "  -gl, --gLatency {legacy --glatency}" << endl; \
cout << "    Minimizes the Gaut latency in the Gaut implementation" <<endl; \
cout << "  -gr, --gREG {legacy --garch}" << endl; \
cout << "    Minimizes the Gaut register count in the Gaut implementation" <<endl; \
cout << "  --gappa" << endl; \
GAPPA_SWITCH

#if !defined(_WIN32)
#define GAPPA_SWITCH \
	cout << "    Minimizes the upper tighter bound found trough Gappa"
#else
#define GAPPA_SWITCH \
	cout << "    Switch not available under WINDOWS, reverts to default behavior"
#endif

#define MAN_CUSTOM_REORDER_COST \
	"  [list of cost functions] {LICF ... MICF}" << endl; \
cout << "    Where LICF and MICF stands for the Least/Most Important Cost Function to optimize" << endl; \
cout << "  --node" << endl; \
cout << "    Minimizes the number of TED nodes" << endl; \
cout << "  --edge" << endl; \
cout << "    Minimizes the total number of TED edges" << endl; \
cout << "  --edge0" << endl; \
cout << "    Minimizes the number of additive TED edges" << endl; \
cout << "  --edgeN" << endl; \
cout << "    Minimizes the number of multiplicative TED edges" << endl; \
cout << "  -nm, --nMUL" << endl; \
cout << "    Minimizes the number of multiplications in the DFG" << endl; \
cout << "  -na, --nADD" << endl; \
cout << "    Minimizes the number of additions(and substractions)in the DFG" << endl; \
cout << "   -rm, --rMPY" << endl; \
cout << "    Minimizes the number of multipliers erOfCandidates\" [DEFAULT BEHAVIOR]" << endl; \
cout << "  -dl, --dLatency" << endl; \
cout << "    Minimizes the lantecy in the DFG" << endl; \
cout << "  --bitwidth" << endl; \
cout << "    Minimizes the bitwidth of the HW implementation subject to unlimited latency|resources" << endl; \
cout << "  --gappa" << endl; \
GAPPA_SWITCH << endl; \
cout << "  -gm, --gMUX" << endl; \
cout << "    Minimizes the number of muxes in the GAUT implementation" <<endl; \
cout << "  -gl, --gLatency" << endl; \
cout << "    Minimizes the latency in the GAUT implementation" <<endl; \
cout << "  -gr, --gREG" << endl; \
cout << "    Minimizes the number of registers in the GAUT implementation" << endl; \
cout << "  -ga, --gArea" << endl; \
cout << "    Minimizes the total area of the operators in the GAUT implementation"


#define ENSURE_TED_EXIST \
	/* assert(ATedNode::transitory_container_empty());*/ \
if(!_pTedMan || _pTedMan->isEmpty()) { \
	cerr << "Info: No current or empty TED." << endl; \
	return CMD_INFO; \
}

#define ENSURE_DFG_EXIST  \
	if(!_pDfgMan) { \
		if(!_pTedMan || _pTedMan->isEmpty()) { \
			cerr << "Info: No DFG structure, and no current or empty TED." << endl; \
			return CMD_INFO; \
		} else { \
			/* cerr << "Info: Implicit transformation from TED to DFG" << endl;*/ \
			Convert from(_pTedMan); \
			_pDfgMan = from.ted2dfgFactor(); \
		} \
	}

#define MARK_DEFAULT_REORDER(item)\
	((string::npos != Environment::getStr(Environment::REORDER_TYPE).find(item)) ? " [DEFAULT BEHAVIOR]." : ".")

#define DEFAULT_REORDER_TYPE(order)\
	string reorder_type = Environment::getStr(Environment::REORDER_TYPE); \
TedOrderStrategy* order = NULL; \
if(string::npos!=reorder_type.find("proper")) { \
	order = new TedOrderProper(_pTedMan); \
} else if(string::npos!=reorder_type.find("reloc")) { \
	order = new TedOrderReloc(_pTedMan); \
} else if(string::npos!=reorder_type.find("swap")) { \
	order = new TedOrderSwap(_pTedMan); \
} else { \
	cerr << "Info: Unknown reordering algorithm given as environment variable" << endl; \
	order = new TedOrderSwap(_pTedMan); \
}

#define DEFAULT_ERROR_UPDATE \
	"Error. Check variable name and index."

#define UPDATE_TED_MANAGER(pManNew,errMsg) \
	if (update_ted_manager(pManNew,errMsg) ==CMD_ERROR) return CMD_ERROR;

ReturnValue Tds::update_ted_manager(TedMan* pManNew,string errMsg) {
	if(!pManNew) {
		cerr << errMsg << endl;
		return CMD_ERROR;
	} else {
		if(_pTedMan && pManNew!=_pTedMan) {
			delete _pTedMan;
			TedMan::purge_all_but(pManNew);
			_pTedMan = pManNew;
			if(_pDfgMan) {
				delete _pDfgMan;
				_pDfgMan = NULL;
			}
		}
	}
	return CMD_OK;
}


/** @brief Builds an initial DFG out of the TED*/
ReturnValue Tds::CmdTED2DFG(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-normal,n|-factor,-flatten,f|-show");
	enum switchcase { case_brief, case_help, case_normal, case_factor, case_showFactor};
	bool useFactor = true;
	bool showFactor = false;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_normal:
			useFactor = false;
			break;
		case case_factor:
			useFactor = true;
			break;
		case case_showFactor:
			showFactor = true;
			break;
		case case_brief:
			cout << BRIEF_TED2DFG << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_TED2DFG, "[method]");
			cout << "  [method]" << endl;
			cout << "  -f, --factor, --flatten [--show]" << endl;
			cout << "    Flatten the DFG by factorizing common terms in the TED graph [DEFAULT BEHAVIOR]." << endl;
			cout << "    Sub option: --show" << endl;
			cout << "    Treat each factor found as a pseudo output in the DFG graph." << endl;
			cout << "  -n, --normal" << endl;
			cout << "    Generate a one to one translation of the DFG through a TED NFF traversal." << endl;
			cout << "DETAILS" << endl;
			cout << "  Most of the times this construction is made implicit. For instance when" << endl;
			cout << "  an operation in a DFG is requested(i.e. show -d)and no DFG exist yet" << endl;
			cout << "  an implicit conversion occurs. If a DFG already exist this command will" << endl;
			cout << "  overwrite it." << endl;
			cout << "EXAMPLE" << endl;
			cout << "  poly X = a-b+c" << endl;
			cout << "  poly Y = a+b-c" << endl;
			cout << "  poly F = X+Y" << endl;
			cout << "  dfg2ted --normal" << endl;
			cout << "  show --dfg" << endl;
			cout << "  echo produces a DFG with outputs X,Y and F" << endl;
			cout << "  purge --dfg" << endl;
			cout << "  dfg2ted --factor" << endl;
			cout << "  show --dfg" << endl;
			cout << "  echo polynomials X and Y disappear in the DFG as evaluation of F, " << endl;
			cout << "  echo the resulting polynomial F = 2*a, has no record of X or Y" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  ted2ntl, ntl2ted, dfg2ted, dfg2ntl" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(!_pTedMan || _pTedMan->isEmpty()) {
			cerr << "Info: no current or empty TED." << endl;
			if(_pDfgMan) {
				cerr << "Info: existing DFG has been kept." << endl;
			}
			return CMD_INFO;
		} else {
			if(_pDfgMan) {
				delete _pDfgMan;
			}
			Convert from(_pTedMan);
			if(useFactor) {
				_pDfgMan = from.ted2dfgFactor(showFactor);
			} else {
				_pDfgMan = from.ted2dfg();
			}
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Flatten a DFG into TEDs*/
ReturnValue Tds::CmdDFG2TED(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-flatten,f|-error,e");
	enum switchcase { case_brief, case_help, case_flatten, case_error};
	bool extractError = false;
	bool flatten = false;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_flatten:
			flatten = true;
			break;
		case case_error:
			extractError = true;
			break;
		case case_brief:
			cout << BRIEF_DFG2TED << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFG2TED, CMD_NO_OPTIONS);
			cout << "DETAILS" << endl;
			cout << "  The DFG is traversed to reproduce the contained algebraic expression. " << endl;
			cout << "SEE ALSO" << endl;
			cout << "  ted2ntl, ted2dfg, ntl2ted, dfg2ntl" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		if(_pTedMan) {
#if 0
			// resolve some issues on the Linux version
			// but at the same time it prevents flattening of binary nodes
			// as the condition of binary is casted away when the variables are cleared
			_pTedMan->clearVar();
#endif
			delete _pTedMan;
			Mark<TedNode>::resetMark();
			Mark<TedKids>::resetMark();
			ATedNode::purge_transitory_container();
			TedKids::purge_transitory_container();
			_pTedMan  = new TedMan();
			TedMan::_factor_counter = 1;
			TedDecompose::_pt_count = 0;
			TedDecompose::_st_count = 0;
		}
		Convert from(_pDfgMan);
		if(extractError)
			_pTedMan = from.dfg2tedError();
		else
			_pTedMan = from.dfg2ted(flatten);
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Construct a TED from a polynomial expression*/
ReturnValue Tds::CmdParsePoly(unsigned int argc, char** argv) {
	bool intoConstants= Environment::getBool(Environment::CONST_AS_VARS);
	int c;
	ParseOption opt(argc, argv, "-brief|-help, h|-const");
	enum switchcase { case_brief, case_help, case_const };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_const:
			intoConstants = !intoConstants;
			break;
		case case_brief:
			cout << BRIEF_POLY << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_POLY, "[--const] {[function=] expression}");
			cout << "  --const"<<endl;
			cout << "    " <<(intoConstants ? "Disables" : "Enables")<< " the use of constant as nodes." << endl;
			cout << "  expression" << endl;
			cout << "    Any polynomial expression with operators \"+, -,*, ^, <<, @\". The expression is"<<endl;
			cout << "    written as [function = algebraic expression]. If \"function\" is omitted, " << endl;
			cout << "    its name will be F$, where $ is an integer incremented with each call." << endl;
			cout << "NOTES" << endl;
			cout << "  1) The variable's name in the expression can not contain any of the following" << endl;
			cout << "     symbols ` # $ \\ \" ';, ?" << endl;
			cout << "  2) The polynomial expression can not start with a negative term. i.e -a*b" << endl;
			cout << "     if the above is the desired behavior, it has to be recoded as a*b*(-1)" << endl;
			cout << "  3) The operations can be grouped with opening brackets [,{,(and closing" << endl;
			cout << "     brackets ],},). Any pair combination of brackets is allowed." << endl;
			cout << "  4) A variable followed with ':' is considered from that point on to be a binary variable" << endl;
			cout << "  5) The shift (<<) and retime (@) operators require that its right hand operand is a constant value." << endl;
			cout << "  6) Any variable starting with the prefix \"" << Environment::getStr(Environment::CONST_PREFIX) << "\" will be treated as constant variable." << endl;
			cout << "EXAMPLES" << endl;
			cout << "  poly F = ((a1+b_1)*c-D)^2+D" << endl;
			cout << "  poly (a+b)^2+a^(1+1)" << endl;
			cout << "  poly --const Y = 4*(a+6*(b+7))" << endl;
			cout << "  poly Y2 = (a+(b+7)<<2+(b+7)<<1)<<2" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  read, sub, show, tr, purge, linearize, decompose" << endl;
			//cout << "NOTE" << endl;
			//cout << "  The symbol @ is an alias symbol for the shifter operator <<" << endl;
			return CMD_HELP;
		}
	}
	string p = "";
	while(opt.current()) {
		p += opt.current();
		opt.next();
	}
	try {
		parsePoly((char*)p.c_str(), intoConstants);
		if (!_manual_purge) {
			_pTedMan->purge_transitory_container();
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Common sub expression variable*/
ReturnValue Tds::Cmdscse(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_SCSE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SCSE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  lcse, dfactor, dcse, bottomscse, bottomdcse, candidate" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew=_pTedMan->scse();
		if(pManNew) {
			delete _pTedMan;
			_pTedMan = pManNew;
			if(_pDfgMan) {
				delete _pDfgMan;
				_pDfgMan = NULL;
			}
		}
		_MainShell->execCmd("print -s");
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief bottom scse*/
ReturnValue Tds::Cmdbottomscse(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_BOTTOM_SCSE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_BOTTOM_SCSE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  scse, lcse, dfactor, dcse, bottomdcse, candidate" << endl;
			return CMD_HELP;
		}
	}
	try{
		TedMan* pManNew=_pTedMan->bottomscse();
		UPDATE_TED_MANAGER(pManNew,"Error. No TED manager produced");
		_MainShell->execCmd("print -s");
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief bottom dcse*/
ReturnValue Tds::Cmdbottomdcse(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_BOTTOM_DCSE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_BOTTOM_DCSE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  scse, lcse, dfactor, bottomcse, bottomdcse, candidate" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew=_pTedMan->bottomdcse();
		UPDATE_TED_MANAGER(pManNew,"Error. No TED manager produced");
		_MainShell->execCmd("print -s");
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief bottom dcse*/
ReturnValue Tds::Cmddcse(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_DCSE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DCSE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  lcse, dfactor, dcse, bottomdcse, bottomcse, candidate" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew=_pTedMan->dcse();
		UPDATE_TED_MANAGER(pManNew,"Error. No TED manager produced");
		_MainShell->execCmd("print -s");
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief l common sub expression variable*/
ReturnValue Tds::Cmdlcse(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help ,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_LCSE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_LCSE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  scse, dfactor, dcse, bottomscse, bottomdcse, candidate" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew=_pTedMan->lcse();
		UPDATE_TED_MANAGER(pManNew,"Error. No TED manager produced");
		_MainShell->execCmd("print -s");
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Show the candidate expression for cse or factorization*/
ReturnValue Tds::CmdCandidate(unsigned int argc, char**argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_CANDIDATE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_CANDIDATE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  lcse, scse, dfactor, dcse, bottomscse, bottomdcse" << endl;
			return CMD_HELP;
		}
	}
	try {
		_pTedMan->printCandidates();
	} CATCH_ERRORS;
	return CMD_OK;
}


/** @brief Show the number of additive edge and mpy edge associated with the TED(this can be maximal bound for the resources need)*/
ReturnValue Tds::CmdCost(unsigned int argc, char**argv) {
	int c;
	int previousNTL = 0;
	pair<bool, int> pe;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-dfg,d|-netlist,n:0|-ignore_ntl,i");
	enum switchcase { case_brief, case_help, case_ted, case_dfg, case_nl, case_ignore_ntl };
	enum switchgraph { graph_ted, graph_dfg, graph_nl, graph_unknown };
	bool ignore_ntl = false;
	switchgraph graph = graph_ted;
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_ignore_ntl:
			ignore_ntl = true;
			break;
		case case_ted:
			graph = graph_ted;
			break;
		case case_dfg:
			graph = graph_dfg;
			break;
		case case_nl:
			graph = graph_nl;
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				previousNTL = pe.second;
			} else {
				// backtrack one argument, use current netlist
				opt.disregardOptionArgument();
			}
			break;
		case case_brief:
			cout << BRIEF_COST << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_COST,"{-i} [data structure]");
			cout << "  -i, --ignore_ntl" << endl;
			cout << "    Ignores any extracted NTL, and derives the cost directly from the DFG data structure." << endl;
			cout << "  [data structure]" << endl;
			cout << "  -t, --ted [-bf, --bigfont]" << endl;
			cout << "    Show the TED graph [DEFAULT BEHAVIOR]." << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    Show the DFG graph correspondent to the TED." << endl;
			cout << "  -n, --netlist [integer]" << endl;
			cout << "    Show the current NTL graph, or a previous NTL in history given by the integer argument." << endl;
			cout << endl;
			return CMD_HELP;
		}
	}
	try{
		cout << "OP delays: ADD=" << Environment::getInt(Environment::DELAYADD) << " SUB=" << Environment::getInt(Environment::DELAYSUB) << " MPY=" << Environment::getInt(Environment::DELAYMPY) << endl;
		cout << "resources: ADD=" << ((Environment::getInt(Environment::RADD) ==0) ?UINT_MAX:Environment::getInt(Environment::RADD));
		cout <<           " MPY=" << ((Environment::getInt(Environment::RMPY) ==0) ?UINT_MAX: Environment::getInt(Environment::RMPY)) << endl;
		string design_name;
		switch(graph) {
		case graph_ted:
			ENSURE_TED_EXIST;
			_pTedMan->cost();
			design_name = "ted";
			break;
		case graph_dfg:
			{
				if(!_pDfgMan) {
					cerr << "Info: no DFG found. To contruct a DFG from a TED used the ted2dfg command" << endl;
					return CMD_INFO;
				}
				DfgStat stat;
				_pDfgMan->schedule(Environment::getInt(Environment::RMPY),Environment::getInt(Environment::RADD),Environment::getInt(Environment::RSUB));
				_pDfgMan->scheduleStat(stat);
				if(!ignore_ntl && getCurrentNetlist()&& getCurrentNetlist()->isExtracTed()) {
					Netlist* nl = getCurrentNetlist();
					_pDfgMan->strash();
					Convert fromdfg(_pDfgMan, nl);
					bool evaluateConstantNodes = Environment::getBool(Environment::CONST_CDFG_EVAL);
					Netlist* sol = fromdfg.dfg2ntl(evaluateConstantNodes);
					design_name = nl->getName();
					string design = design_name + ".cdfg";
					sol->writeCDFG(design.c_str());
				} else {
					design_name = "dfg2ntl";
					_pDfgMan->writeCDFG("dfg2ntl.cdfg");
				}
				Gaut gaut(design_name);
				gaut.run();
				FixCost* cost = gaut.compute_cost();
				cost->stat = stat;
				cost->report();
				delete cost;
				cout << endl;
				break;
			}
		case graph_nl:
			{
				Netlist* nl = NULL;
				if(0 != previousNTL) {
					int hsize = _lHistory.size();
					if(0==hsize) {
						cerr << "Info: There are no netlists" << endl;
						return CMD_INFO;
					} else if(previousNTL > hsize - 1 || previousNTL < 0) {
						previousNTL = (hsize>0) ? hsize-1 : 0;
						cout << "Warning: " << hsize << " netlists in record [0-" << previousNTL << "]. Last one is used." << endl;
					}
					nl = getPreviousNetlist(previousNTL);
				} else {
					nl = getCurrentNetlist();
				}
				if(!nl || nl->empty()) {
					cerr << "Info: No netlist found or empty netlist, please load a netslit first\n";
					return CMD_INFO;
				}
				design_name = nl->getName();
				DfgStat stat;
				if(nl->isExtracTed()) {
					if(_pDfgMan) {
						_pDfgMan->strash();
						_pDfgMan->schedule(Environment::getInt(Environment::RMPY),Environment::getInt(Environment::RADD),Environment::getInt(Environment::RSUB));
						_pDfgMan->scheduleStat(stat);
						Convert fromdfg(_pDfgMan, nl);
						bool evaluateConstantNodes = Environment::getBool(Environment::CONST_CDFG_EVAL);
						design_name += "_dfg";
						string design = design_name + ".cdfg";
						Netlist* sol = fromdfg.dfg2ntl(evaluateConstantNodes);
						sol->writeCDFG(design.c_str());
					} else {
						cerr << "Info: Netlist has been extracted. There is no DFG to reconstruct it." << endl;
						return CMD_INFO;
					}
				}
				Gaut gaut(design_name);
				gaut.run();
				FixCost* cost = gaut.compute_cost();
				cost->stat = stat;
				cost->report();
				delete cost;
				cout << endl;
				break;
			}
		case graph_unknown:
		default:
			throw(string("03010. UNKOWN graph type.\n"));
			break;
		}
		cout << "design name: " << design_name << endl;
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Construct a TED from a predetermine transform*/
ReturnValue Tds::CmdBuildTransforms(unsigned int argc, char** argv) {
	int c;
	unsigned int nCol = 0, nRow = 0;
	pair<bool, int> pe;
	Matrice* m = NULL;
	CMatrice* cm = NULL;
	bool iscpl=false;
	char* filename = NULL;
	ParseOption opt(argc,argv,"-brief|-help,h|-col,c:|-row,r:|-out,o:");
	enum switchcase { case_brief, case_help, case_col, case_row, case_out };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_col:
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				nCol = pe.second;
			} else {
				cerr << "Info: The argument of --col must be an integer" << endl;
				return CMD_INFO;
			}
			break;
		case case_row:
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				nRow = pe.second;
			} else {
				cerr << "Info: The argument of --row must be an integer" << endl;
				return CMD_INFO;
			}
			break;
		case case_out:
			filename = opt.getOptionArgument();
			break;
		case case_brief:
			cout << BRIEF_TRANSFORM << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_TRANSFORM, "{-c colnum} [-r rownum] [-o filename] {transform}");
			cout << "  -c, --col integer" << endl;
			cout << "    Number of colums of the tranform"<<endl;
			cout << "  -r, --row integer" << endl;
			cout << "    Number of rows of the tranform"<<endl;
			cout << "  -o, --out filename" << endl;
			cout << "    Writes the equation of the transform into a file." << endl;
			cout << "  tranform" << endl;
			cout << "    Names the desired transform to build. It could be any of the following dct, dft, " << endl;
			cout << "    dst, dfht, wht. The dht and wht transforms require square matrices, therefore " << endl;
			cout <<"    only the parameter --col should be given." << endl;
			cout << "EXAMPLES" << endl;
			cout << "   tr -c 3 -r 4 -o dct.dump dct" << endl;
			cout << "   tr -c 3 -r 4 -o bidct.dump bidct" << endl;
			cout << "   tr -c 5 -r 5 dft" << endl;
			cout << "   tr -c 4 -r 3 dst" << endl;
			cout << "   tr -c 3 dht" << endl;
			cout << "   tr -c 3 wht" << endl;
			cout << "WARNING" << endl;
			cout << "   bidct has to use equal dimension for the col and row, otherwise it crashes" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  read, purge, poly" << endl;
			return CMD_HELP;
		}
	}
	if(NULL == opt.current()) {
		cerr << "Info: No transform selected" << endl;
		return CMD_INFO;
	}
	if(nCol == 0 || nRow == 0) {
		cerr << "Info: The transform must have col and row size greater than 0" << endl;
		return CMD_INFO;
	}
	try {
		char* option = opt.current();
		if(strcmp(option, "dct") == 0) {
			cout << "Building DCT" << endl;
			m = Matrice::dct(nRow, nCol);
			iscpl=false;
		} else if(strcmp(option, "bidct") == 0) {
			cout << "Building Bidimensional DCT" << endl;
			m = Matrice::bidct(nRow, nCol);
			iscpl=false;
		} else if(strcmp(option, "dft") == 0) {
			cout << "Building DFT" << endl;
			cm = CMatrice::dft(nRow, nCol);
			iscpl=true;
		} else if(strcmp(option, "dst") == 0) {
			cout << "Building DST" << endl;
			m = Matrice::dst(nRow, nCol);
			iscpl=false;
		} else if(strcmp(option, "dht") == 0) {
			cout << "Building DHT" << endl;
			m = Matrice::dht(nCol);
			iscpl=false;
		} else if(strcmp(option, "wht") == 0) {
			cout << "Building WHT" << endl;
			m = Matrice::wht(nCol);
			iscpl=false;
		} else {
			throw(string("03030. UNKOWN transform \"")+ opt.current()+ string("\""));
			return CMD_ERROR;
		}
		if(iscpl) {
			printTransform(cm);
		} else {
			printTransform(m);
		}
		if(_pTedMan)
			delete _pTedMan;
		_pTedMan = new TedMan();
		if(iscpl) {
			buildTransform(cm);
		} else {
			buildTransform(m);
		}
		if(filename) {
			if(iscpl) {
				writeTransformToC(cm, filename);
			} else {
				writeTransformToC(m, filename);
			}
		}
		if(iscpl) {
			delete cm;
		} else {
			delete m;
		}
		if(_pDfgMan) {
			delete _pDfgMan;
			_pDfgMan = NULL;
		}
		if (!_manual_purge) {
			_pTedMan->purge_transitory_container();
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Show the TED, DFG or NTL graph*/
ReturnValue Tds::CmdShow(unsigned int argc, char** argv) {
	int c;
	bool bVerbose = Environment::getBool(Environment::SHOW_VERBOSE);
	bool bigFont = Environment::getBool(Environment::SHOW_BIGFONT);
	bool debug = false;
	bool visualize = true;
	bool compact_graph = false;
	bool show_ids = false;
	pair<bool, int> pe;
	int previousNTL = 0;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-dfg,d|-netlist,n:0|-verbose,v|-bigfont,bf|-compact,c|-debug|-fileonly,fo|-ids");
	char* filename = NULL;
	enum switchcase { case_brief, case_help, case_ted, case_dfg, case_nl, case_verbose, case_bigfont, case_compact, case_debug, case_fileonly,case_ids };
	enum switchgraph { graph_ted, graph_dfg, graph_nl, graph_unknown };
	switchgraph graph = graph_ted;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_fileonly:
			visualize = false;
			break;
		case case_ted:
			graph = graph_ted;
			filename = Util::tempFileName(Environment::getStr(Environment::SHOW_DIRECTORY).c_str(), "ted_", "dot");
			break;
		case case_dfg:
			graph = graph_dfg;
			filename = Util::tempFileName(Environment::getStr(Environment::SHOW_DIRECTORY).c_str(), "dfg_", "dot");
			break;
		case case_nl:
			graph = graph_nl;
			filename = Util::tempFileName(Environment::getStr(Environment::SHOW_DIRECTORY).c_str(), "nl_", "dot");
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				previousNTL = pe.second;
			} else {
				// backtrack one argument, use current netlist
				opt.disregardOptionArgument();
			}
			break;
		case case_bigfont:
			bigFont = !bigFont;
			break;
		case case_compact:
			compact_graph = true;
			break;
		case case_verbose:
			bVerbose = !bVerbose;
			break;
		case case_debug:
			debug = true;
			break;
		case case_ids:
			show_ids = true;
			break;
		case case_brief:
			cout << BRIEF_SHOW << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SHOW, "[data structure] [-v -db -fo] [outputfile]");
			cout << "  [data structure]" << endl;
			cout << "  -t, --ted [-bf, --bigfont] [-c, --compact]" << endl;
			cout << "    Show the TED graph [DEFAULT BEHAVIOR]." << endl;
			cout << "    Sub option: -bf, --bigfont" << endl;
			cout << "    Toogle big fonts in the TED graph. [DEFAULT=" <<(bigFont?"true]":"false]")<< endl;
			cout << "    Sub option: -c, --compact" << endl;
			cout << "    variable names with 5+ chars are not printed. (see environment show_level) [DEFAULT=" << (compact_graph?"true]":"false]") << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    Show the DFG graph correspondent to the TED." << endl;
			cout << "  -n, --netlist [integer]" << endl;
			cout << "    Show the current NTL graph, or a previous NTL in history given by the integer argument." << endl;
			cout << endl;
			cout << "  -v, --verbose" << endl;
			cout << "    Toogle additional information in the graph. [DEFAULT=" <<(bVerbose?"true]":"false]")<<endl;
			cout << "  -fo, --fileonly" << endl;
			cout << "    Generate the dot file but do not visualize it."<<endl;
#ifndef NDEBUG
			cout << "  --debug [--ids]" << endl;
			cout << "    Show debugging information in the graph" << endl;
			cout << "    Sub option: --ids" << endl;
			cout << "    Prints a unique ID associated to each operand. [DEFAULT=" << (show_ids?"true]":"false]") << endl;
#endif
			cout << "  outputfile" << endl;
			cout << "    The output file name to hold the dot graph and its postcript dump." << endl;
			cout << "NOTE" << endl;
			cout << "  1)if more than one of -t, -d, -n is specified only the last one will take effect." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  write, read" << endl;
			return CMD_HELP;
		}
	}
	try {
		ofstream ofile;
		char* myname = opt.current();
		if(myname) {
			free(filename);
			filename = myname;
		}
		if(!filename &&(graph == graph_ted)) {
			//if no option provided and no file name was given, filename is still empty
			filename = Util::tempFileName(Environment::getStr(Environment::SHOW_DIRECTORY).c_str(), "ted_", "dot");
		}
		if(filename)
			ofile.open(filename);
		if(!ofile.is_open()) {
			throw(string("03009. Can not open temporary file for writing.\n"));
		}
		switch(graph) {
		case graph_ted:
			if(bVerbose || debug)
				_pTedMan->writeDotDetailed(ofile);
			else
				_pTedMan->writeDot(ofile, bigFont, compact_graph);
			break;
		case graph_dfg:
			if(!_pDfgMan && !_pTedMan->isEmpty()) {
				Convert from(_pTedMan);
				_pDfgMan = from.ted2dfgFactor();
				//_pDfgMan = new DfgMan(_pTedMan);
			}
			if(_pDfgMan) {
				if(!debug)
					_pDfgMan->writeDot(ofile, 1, bVerbose, show_ids);
				else
					_pDfgMan->writeDotDetailed(ofile);
			} else {
				cerr << "Info: no DFG found, and none could be inferred from TED" << endl;
			}
			break;
		case graph_nl:
			Netlist* nl;
			if(0 != previousNTL) {
				int hsize = _lHistory.size();
				if(0==hsize) {
					cerr << "Info: There are no netlists" << endl;
					return CMD_INFO;
				} else if(previousNTL > hsize - 1 || previousNTL < 0) {
					previousNTL = (hsize>0) ? hsize-1 : 0;
					cout << "Warning: " << hsize << " netlists in record [0-" << previousNTL << "]. Last one is used." << endl;
				}
				nl = getPreviousNetlist(previousNTL);
			} else {
				nl = getCurrentNetlist();
			}
			if(!nl || nl->empty()) {
				cerr << "Info: No netlist found or empty netlist, please load a netslit first\n";
				return CMD_INFO;
			}
			nl->writeDot(ofile, bVerbose);
			break;
		case graph_unknown:
		default:
			throw(string("03010. UNKOWN graph type.\n"));
			break;
		}
		ofile.close();
		if(visualize)
			Util::showDotFile(filename);
		else
			cout << "Info: File " << filename << " was generated." << endl;
		if(!myname) {
			free(filename);
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Purge the TED, DFG and/or the NTL data structure*/
ReturnValue Tds::CmdPurge(unsigned int argc, char** argv) {
	bool purge_ted = false;
	bool purge_dfg = false;
	bool purge_nl = false;
	bool all = false;
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-dfg,d|-netlist,n|-all,a");
	enum switchcase { case_brief, case_help, case_ted, case_dfg, case_nl, case_all };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_all:
			purge_ted = true;
			purge_dfg = true;
			purge_nl = true;
			all = true;
			break;
		case case_nl:
			purge_nl = true;
			break;
		case case_ted:
			purge_ted = true;
			break;
		case case_dfg:
			purge_dfg = true;
			break;
		case case_brief:
			cout << BRIEF_PURGE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_PURGE, "[-t -d] [-n] {design name} [--all]");
			cout << "  -t, --ted" << endl;
			cout << "    Purge TED only." << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    Purge DFG only." << endl;
			cout << "  -n, --netlist {design name}" << endl;
			cout << "    Purge the netlist with the given design name." << endl;
			cout << "    If no design names is provided, all netlist are purged." << endl;
			cout << "  -a, --all" << endl;
			cout << "    Purge all internal data structures: TED, DFG and NTL." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  read, tr, poly" << endl;
			return CMD_HELP;
		}
	}
	try {
		if((purge_nl || purge_ted || purge_dfg) == false) {
			purge_ted = purge_dfg = purge_nl = true;
			all = true;
		}
		if(purge_ted) {
			Mark<TedNode>::resetMark();
			Mark<TedKids>::resetMark();
			if(_pTedMan) {
				_pTedMan->clearVar();
				delete _pTedMan;
			}
			ATedNode::purge_transitory_container();
			TedKids::purge_transitory_container();
			_pTedMan  = new TedMan();
			TedMan::_factor_counter = 1;
			TedDecompose::_pt_count = 0;
			TedDecompose::_st_count = 0;
		}
		if(purge_dfg) {
			if(_pDfgMan)
				delete _pDfgMan;
			_pDfgMan = NULL;
			DfgOperator::idcount = 0;
			DfgOperator::manager.clear();
		}
		if(purge_nl) {
			if (all || !opt.current()) {
				clearHistory();
				setNtlExtracTed(NULL);
			} else {
				string name = opt.current();
				bool found = false;
				list<Netlist*>::iterator p = _lHistory.begin();
				for (; p != _lHistory.end(); p++) {
					Netlist* nl =*p;
					if(!strcmp(nl->getName(),name.c_str())) {
						delete nl;
						found = true;
						break;
					}
				}
				if (found) {
					_lHistory.erase(p);
				}
			}
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Preset the order of variables, before any TED is constructed*/
ReturnValue Tds::CmdVars(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_VARS << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_VARS, "variable1 variable2 .... variableN");
			cout << "  variable1 variable2 .... variableN" << endl;
			cout << "    Pre-sets the order of the TED to be constructed to the order given, where variable1 is at top" << endl;
			cout << "    and variableN is at bottom." << endl;
			cout << "    Notes: " << endl;
			cout << "* any other variable not specified in the list is assumed to be lower than variableN" <<endl;
			cout << "* any variable followed by a colon \":\" is set to be a binary variable" <<endl;
			cout << "EXAMPLE" << endl;
			cout << "  vars z y: w x" << endl;
			cout << "NOTE:" << endl;
			cout << "  1)The order of retimed variables cannot be pre-set" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  set, listvars, binvars" << endl;
			return CMD_HELP;
		}
	}
	if(NULL == opt.current()) {
		cerr << "Info: Missing list of variables in the desired order" << endl;
		return CMD_INFO;
	}
	try {
		DEFAULT_REORDER_TYPE(ordest);
		while(opt.current()) {
			string var(opt.current());
			if(var.find_first_of(PolyParser::REG) ==string::npos) {
				int last = strlen(var.c_str());
				bool binary = false;
				if(var[last-1]==PolyParser::BIN) {
					var.erase(last-1);
					binary = true;
				}
				if (_pTedMan->isEmpty() || !_pTedMan->hasVar(var)) {
					_pTedMan->registerVar(var,binary);
				} else {
					_pTedMan->bottomVariable(var,ordest);
				}
			} else {
				cout << "Info: Retimed variable " << var << " cannot be pre-set, skipping directive." << endl;
			}
			opt.next();
		}
		delete ordest;
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Jump below the given variable*/
ReturnValue Tds::CmdExchange(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			delete orderst;
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_EXCHANGE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_EXCHANGE, "[reorder algorithm] variable1 variable2");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable1 variable2" << endl;
			cout << "    The variable1 will be moved below the the variable2." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew;
		char* varname1 = opt.current();
		opt.next();
		char* varname2 = opt.current();
		if(!varname1 || !varname2) {
			cerr << "Info: Missing variable" << endl;
			return CMD_INFO;
		}
		//TedOrderProper orderst(_pTedMan);
		pManNew = _pTedMan->exchangeVariable(varname1, varname2,orderst);
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Jump below the given variable*/
ReturnValue Tds::CmdJumpAbove(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			delete orderst;
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_JUMPABOVE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_JUMPABOVE, "[reorder algorithm] variable1 variable2");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable1 variable2" << endl;
			cout << "    The nodes with variable1 will be moved above the nodes with variable2." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew;
		char* varname1 = opt.current();
		opt.next();
		char* varname2 = opt.current();
		if(!varname1 || !varname2) {
			cerr << "Info: Missing variable" << endl;
			return CMD_INFO;
		}
		pManNew = _pTedMan->jumpAboveVariable(varname1, varname2,orderst);
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Jump below the given variable*/
ReturnValue Tds::CmdJumpBelow(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			delete orderst;
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_JUMPBELOW << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_JUMPBELOW, "[reorder algorithm] variable1 variable2");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable1 variable2" << endl;
			cout << "    The nodes with variable1 will be moved below the nodes with variable2." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpAbove, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew;
		char* varname1 = opt.current();
		opt.next();
		char* varname2 = opt.current();
		if(!varname1 || !varname2) {
			cerr << "Info: Missing variable" << endl;
			return CMD_INFO;
		}
		pManNew = _pTedMan->jumpBelowVariable(varname1, varname2,orderst);
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Move up one position the given variable*/
ReturnValue Tds::CmdBblup(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			delete orderst;
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_BBLUP << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_BBLUP, "[reorder algorithm] variable [number of times]");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable" << endl;
			cout << "    The variable name to move up." << endl;
			cout << "  [number of times]" << endl;
			cout << "    The number of times bblup should be executed. [DEFAULT=1]" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	TedMan* pManNew;
	char* varname = opt.current();
	opt.next();
	if(!varname) {
		cerr << "Info: Missing variable to bouble up" << endl;
		return CMD_INFO;
	}
	try {
		char* cjump = opt.current();
		if(cjump) {
			pair<bool, unsigned long> tt = Util::atoi(cjump);
			if(tt.first == false) {
				cerr << "Info: The index has to be a positive integer value" << endl;
				return CMD_INFO;
			}
			pManNew = _pTedMan->bblUpVariable(varname,orderst,tt.second);
		} else {
			pManNew = _pTedMan->bblUpVariable(varname,orderst);
		}
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Move down one position the given variable*/
ReturnValue Tds::CmdBbldown(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_BBLDOWN << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_BBLDOWN, "[reorder algorithm] variable [number of times]");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable" << endl;
			cout << "    The variable name to move down." << endl;
			cout << "  [number of times]" << endl;
			cout << "    The number of times bblup should be executed. [DEFAULT=1]" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bblup, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	TedMan* pManNew;
	char* varname = opt.current();
	opt.next();
	if(!varname) {
		cerr << "Info: Missing variable to bobble down" << endl;
		return CMD_INFO;
	}
	try {
		char* cjump = opt.current();
		if(cjump) {
			pair<bool, unsigned long> tt = Util::atoi(cjump);
			if(tt.first == false) {
				cerr << "Info: The index has to be a positive integer value" << endl;
				return CMD_INFO;
			}
			pManNew = _pTedMan->bblDownVariable(varname,orderst,tt.second);
		} else {
			pManNew = _pTedMan->bblDownVariable(varname,orderst);
		}
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Move the given variable to the bottom*/
ReturnValue Tds::CmdBottom(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_BOTTOM << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_BOTTOM, "[reorder algorithm] variable");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable" << endl;
			cout << "    The variable name to move to the bottom." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew;
		char* varname = opt.current();
		if(!varname) {
			cerr << "Info: Missing variable to send to bottom" << endl;
			return CMD_INFO;
		}
		pManNew = _pTedMan->bottomVariable(varname, orderst);
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Move the given variable to the top*/
ReturnValue Tds::CmdTop(unsigned int argc, char** argv) {
	DEFAULT_REORDER_TYPE(orderst);
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r,|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_TOP << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_TOP, "[reorder algorithm] variable");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable" << endl;
			cout << "    The variable name to move to the top." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv" << endl;
			return CMD_HELP;
		}
	}
	try {
		char* varname = opt.current();
		if(!varname) {
			cerr << "Info: No variable specified" << endl;
			return CMD_INFO;
		}
		TedMan* pManNew = _pTedMan->topVariable(varname, orderst);
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Relocate the given variable to the desired position*/
ReturnValue Tds::CmdReloc(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_RELOC << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_RELOC, "variable index");
			cout << "  variable" << endl;
			cout << "    The variable name to relocate." << endl;
			cout << "  index" << endl;
			cout << "    The new index of the variable." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew;
		char* variableName = opt.current();
		opt.next();
		char* newVariableIndex = opt.current();
		if((NULL==variableName)||(NULL==newVariableIndex)) {
			cerr << "Info: Missing variable to relocate" << endl;
			return CMD_INFO;
		}
		pair<bool, unsigned long> tt = Util::atoi(newVariableIndex);
		if(tt.first == false) {
			cerr << "Info: The index has to be an integer value" << endl;
			return CMD_INFO;
		}
		pManNew = _pTedMan->relocateVariable(variableName, tt.second);
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Construct the NTL corresponding to the DFG*/
ReturnValue Tds::CmdDFG2NTL(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|b|-const,c");
	bool evaluateConstantNodes = Environment::getBool(Environment::CONST_CDFG_EVAL);
	enum switchcase { case_brief, case_help, case_balance, case_toggleConstEval };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_balance:
			//bBalance ^= true;
			break;
		case case_toggleConstEval:
			evaluateConstantNodes = !evaluateConstantNodes;
			break;
		case case_brief:
			cout << BRIEF_DFG2NTL << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFG2NTL, "[-c]");
			//cout << "  -b" << endl;
			//cout << "    Toogle latency optimization. [Default=" <<(bBalance ? "true]" : "false]")<< endl;
			cout << "  -c, --const" << endl;
			cout << "    " <<(evaluateConstantNodes ? "Disables" : "Enables")<< " the evaluation of constants in the Netlist." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  ted2ntl, ted2dfg, ntl2ted, dfg2ted" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		Netlist* oNl = NULL;
		Netlist* iNl = getCurrentNetlist();
		_pDfgMan->strash();
		if(iNl) {
			if(!iNl->isExtracTed())
				throw(string("03007. Cannot annotate DFG back into NTL. Current Netlist has not been extracted"));
			Convert from(_pDfgMan, iNl);
			oNl = from.dfg2ntl(evaluateConstantNodes);
			//			string nl_name = oNl->getName();
			//			nl_name += ".cdfg";
			//			oNl->writeCDFG(nl_name.c_str());
		} else {
			_pDfgMan->writeCDFG("dfg_2ntl.cdfg");
			oNl = Netlist::readCDFG("dfg_2ntl.cdfg", true);
		}
		pushToHistory(oNl);
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Print out TED information: statistic, normal factor form, etc*/
ReturnValue Tds::CmdPrint(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-factor,f|-stat,s|-extract,e|nff");
	enum switchcase { case_brief, case_help, case_factor, case_statistic, case_extract,case_nff};
	switchcase op = case_factor;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_nff:
			op = case_nff;
			break;
		case case_factor:
			op = case_factor;
			break;
		case case_statistic:
			op = case_statistic;
			break;
		case case_extract:
			op = case_extract;
			break;
		case case_brief:
			cout << BRIEF_PRINT << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_PRINT, CMD_NO_OPTIONS);
			cout << "  -e, --extract" << endl;
			cout << "    Print factored form after common subexpression elimination, " << endl;
			cout << "    listing all extracted expressions." << endl;
			cout << "  -f, --factor" << endl;
			cout << "    Print factored form [DEFAULT BEHAVIOR]." << endl;
			cout << "  -nff" << endl;
			cout << "    Print normal factor form" << endl;
			cout << "  -s, --stat" << endl;
			cout << "    Print statistics." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_TED_EXIST;
		map <string, string> mExprs;
		map <string, string>* mCSEs;
		DfgStat t;
		bool factorIsOn = (TedMan::_factor_counter > 1);
		DfgMan* printDfgMan = NULL;
		Convert from(_pTedMan);;
		switch(op) {
		case case_nff:
			{
				printDfgMan = from.ted2dfg();
				printDfgMan->getExpression(mExprs, NULL, factorIsOn);
				for(map<string, string>::iterator p = mExprs.begin(); p != mExprs.end(); p++) {
					string out = p->first;
					string expr = p->second;
					if (expr.size()<8000) {
						cout << "poly  " << out << " = " << expr << endl;
					} else {
						string filename = Util::tempFileName(".", out.c_str(), "scr");
						ofstream ofile(filename.c_str());
						if(ofile.is_open()) {
							cout << "#### " << out << " -> " << "see file " << filename << endl;
							ofile << "poly " << out << " = " << expr << endl;
						} else {
							cout << "Error while dumping equation " << out << " into a file" << endl;
						}
						ofile.close();
					}
				}
				break;
			}
		case case_factor:
			{
				printDfgMan = from.ted2dfgFactor(factorIsOn);
				printDfgMan->getExpression(mExprs, NULL, factorIsOn);
				for(map<string, string>::iterator p = mExprs.begin(); p != mExprs.end(); p++) {
					string out = p->first;
					string expr = p->second;
					if (expr.size()<8000) {
						cout << "poly " << out << " = " << expr << endl;
					} else {
						string filename = Util::tempFileName(".", out.c_str(), "scr");
						ofstream ofile(filename.c_str());
						if(ofile.is_open()) {
							cout << "#### " << out << " -> " << "see file " << filename << endl;
							ofile << "poly " << out << " = " << expr << endl;
						} else {
							cout << "Error while dumping equation " << out << " into a file" << endl;
						}
						ofile.close();
					}
				}
				break;
			}
		case case_statistic:
			{
				printDfgMan = from.ted2dfgFactor();
				printDfgMan->getNumOperators(t);
				//printf("  nNodes     =%5d, nFacCandid=%5d\n", _pTedMan->_mNodes.nodeCount(), _pTedMan->getNumOfCandidates());
				printf("  nMUL       =%5d, nADD      =%5d, nSUB     =%5d\n", t.nMul, t.nAdd, t.nSub);
				printf("  nMULconst  =%5d, nADDconst =%5d, nSUBconst=%5d\n", t.nMulConst, t.nAddConst, t.nSubConst);
				vector <DfgNode* > vNodes;
				int level = printDfgMan->collectDFS(vNodes);
				printf("  DFG Latency=%5d\n", level);
				break;
			}
		case case_extract:
			{
				printDfgMan = from.ted2dfgFactor(factorIsOn);
				mCSEs = new map <string, string>;
				printDfgMan->getExpression(mExprs, mCSEs, factorIsOn);
				cout << "# Primary Outputs" << endl;
				for(map<string, string>::iterator p = mExprs.begin(); p != mExprs.end(); p++) {
					cout << "poly " << p->first << " = " << p->second << endl;
				}
				cout << "# Common Expressions Extracted" << endl;
				for(map<string, string>::iterator p = mCSEs->begin(); p != mCSEs->end(); p++) {
					cout << "poly " << p->first << " = " << p->second << endl;
				}
				delete mCSEs;
				break;
			}
		default:
			break;
		}
		assert(printDfgMan);
		delete printDfgMan;
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Print out TED information: statistic, normal factor form, etc*/
ReturnValue Tds::CmdInfo(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-dfg,d|-ted,t|-ntl,n");
	enum switchcase { case_brief, case_help, case_dfg, case_ted, case_ntl};
	switchcase op = case_dfg;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_ntl:
			op = case_ntl;
			break;
		case case_dfg:
			op = case_dfg;
			break;
		case case_ted:
			op = case_ted;
			break;
		case case_brief:
			cout << BRIEF_PRINT << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_PRINT, "[-t -d -n]");
			cout << "  -t, --ted" << endl;
			cout << "    Print TED information" << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    Print DFG hierarchy information [DEFAULT BEHAVIOR]" << endl;
			cout << "  -n" << endl;
			cout << "    Print NTL information." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show" << endl;
			return CMD_HELP;
		}
	}
	try {
		switch(op) {
		case case_ntl:
			{
				break;
			}
		case case_dfg:
			{
				ENSURE_DFG_EXIST;
				_pDfgMan->info_hierarchy();
				break;
			}
		case case_ted:
			{
				ENSURE_TED_EXIST;
				break;
			}
		default:
			break;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Read a script file or a cdfg file*/
ReturnValue Tds::CmdRead(unsigned int argc, char** argv) {
	int c = 0;
	ParseOption opt(argc,argv,"-brief|-help,h|-no_dff");
	enum switchcase { case_brief, case_help, case_no_dff };
	bool inferFF = true;
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_no_dff:
			inferFF = false;
			break;
		case case_brief:
			cout << BRIEF_READ << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_READ, "inputfile.[c|cpp|scr|poly|mx|ted|cdfg|xml]");
			cout << "  inputfile" << endl;
			cout << "    The file to read could be any of the following extensions:" << endl;
			cout << "     - c|cpp     for c files" << endl;
			cout << "     - poly|scr  for script files" << endl;
			cout << "     - mx        for matrix files" << endl;
			cout << "     - ted       for ted data structure files" << endl;
			cout << "     - cdfg      for files generated from GAUT" << endl;
			cout << "     - xml       for files generated from GECO" << endl;
			cout << "  [--no_dff]" << endl;
			cout << "     dissables register \"DFF\" discovering when reading the cdfg" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  write, show, purge" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(NULL == opt.current()) {
			cout << "missing input file" << endl;
		}
		string filename = opt.current();
		Netlist* nl = NULL;
		ifstream* in = NULL;
		string line;

		if(Util::isFileExtension(filename, "cdfg")) {
			nl = Netlist::readCDFG(filename.c_str(), inferFF);
			pushToHistory(nl);
		} else if(Util::isFileExtension(filename, "scr") || Util::isFileExtension(filename, "poly")) {
			_manual_purge = true;
			in = new ifstream(filename.c_str());
			if(!in->is_open()) {
				throw(string("03035. Cannot read script file ")+ filename);;
				return CMD_ERROR;
			}
			clock_t	startCmd = 0;
			clock_t finishCmd = 0;
			in->seekg(0,ios::end);
			long end = in->tellg();
			in->seekg(0,ios::beg);
			shell::ReturnValue ret;
			while(Util::readline(*in, line, "#")!= 0) {
				startCmd = clock();
				ret = _MainShell->execCmd(line);
				finishCmd += (clock()- startCmd);
				if(ret!= shell::CMD_OK) {
					in->seekg(0,ios::end);
					_manual_purge = false;
					cout << "Warning 03036. Stopped execution at line \"" << line  << "\"" << endl;
				}
				long current = in->tellg();
				Util::progressBar(current,end," ");
			}
			_manual_purge = false;
			cout << endl;
			_pTedMan->purge_transitory_container();
			//cerr << "Elapsed time is " <<(float)finishCmd/CLOCKS_PER_SEC << " ms." << endl;
			in->close();
			delete in;
		} else if(Util::isFileExtension(filename, "ted")) {
			_pTedMan->read(filename);
		} else if(Util::isFileExtension(filename, "dfg")) {
			if(!_pDfgMan) {
#if 0
				Convert from(_pTedMan);
				_pDfgMan = from.ted2dfgFactor();
#else
				_pDfgMan = new DfgMan();
#endif
			}
			_pDfgMan->read(filename);
		} else if(Util::isFileExtension(filename, "xml")) {
			if(!_pDfgMan) {
				_pDfgMan = new DfgMan();
			}
			DfgNode::setOpDelays(Environment::getInt(Environment::DELAYADD),
								 Environment::getInt(Environment::DELAYSUB),
								 Environment::getInt(Environment::DELAYMPY),
								 Environment::getInt(Environment::DELAYLSH),
								 Environment::getInt(Environment::DELAYREG));
			XMLImport toDfg(_pDfgMan);
			XMLDocument xmldoc;
			xmldoc.LoadFile(filename.c_str());
			xmldoc.Accept(&toDfg);
			xmldoc.PrintError();
		} else if(Util::isFileExtension(filename, "mx")) {
			nl = Netlist::readMatrix(filename.c_str());
			pushToHistory(nl);
		} else if(Util::isFileExtension(filename, "c")|| Util::isFileExtension(filename, "cpp")) {
			string cdfgbin = Environment::getStr(Environment::CDFG_BIN_PATH);
			if(cdfgbin.empty()) {
				cerr << "Info: The environment variable \"cdfg_bin\" does not know about the cdfg compiler";
				return CMD_INFO;
			}
			cdfgbin += " -S -c2dfg -O2 ";
			cdfgbin += filename;
			if(-1 == Util::launchProc(cdfgbin.c_str(), true)) {
				throw(string("03046. Aborting cdfg generation from c/c++ source file."));
				return CMD_ERROR;
			}
			filename = Util::replaceFileExtension(filename, "cdfg");
			string genfile = filename;
			if(genfile.find(Util::fileSeparator, 0)!=string::npos) {
				genfile.erase(0, genfile.find_last_of(Util::fileSeparator)+1);
				string cmd = "cp " + genfile + " " + filename;
				Util::launchProc(cmd.c_str(), true);
			}
			ifstream* in = new ifstream(filename.c_str());
			if(!in->is_open()) {
				throw(string("03047. Cannot read generated cdfg file ")+ filename);;
				return CMD_ERROR;
			}
			in->close();
			delete in;
			nl = Netlist::readCDFG(filename.c_str(), inferFF);
			pushToHistory(nl);
		} else {
			throw(string("03037. UNKOWN file format ")+ filename);
			return CMD_ERROR;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Write the existing netlist into gc or cdfg file*/
ReturnValue Tds::CmdWrite(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-dfg,d|-netlist,n");
	enum switchcase { case_brief, case_help, case_ted, case_dfg, case_ntl, case_none };
	switchcase fromStructure = case_none;
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_ted:
			fromStructure = case_ted;
			break;
		case case_dfg:
			fromStructure = case_dfg;
			break;
		case case_ntl:
			fromStructure = case_ntl;
			break;
		case case_brief:
			cout << BRIEF_WRITE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_WRITE, "[cdfg options] outputfile.[cdfg|dfg|ted|scr]");
			cout << "[cdfg options]" << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    Uses the DFG data structure as starting point" << endl;
			cout << "  -t, --ted" << endl;
			cout << "    Uses the TED data structure as starting point" << endl;
			cout << "  -n, --ntl" << endl;
			cout << "    Uses the NTL data structure as starting point [DEFAULT BEHAVIOR]" << endl;
			cout << "  outputfile" << endl;
			cout << "    The desired output file name. The extension defines the format:" <<endl;
			cout << "     - c        c language." << endl;
			cout << "     - cdfg     current GAUT format." << endl;
			cout << "     - dfg      internal DFG data structure." << endl;
			cout << "     - ted      internal TED data structure." << endl;
			cout << "     - gappa    GAPPA script for computing the accuracy of the DFG data structure." << endl;
			cout << "     - scr|poly generates a script file from the command history." << endl;
			cout << "NOTE" << endl;
			cout << "  By default the file format determines the data structure from which" << endl;
			cout << "  the file will be writed: cdfg->NTL, dfg->DFG, ted->TED" << endl;
			cout << "EXAMPLE" << endl;
			cout << "  write poly2.cdfg" << endl;
			cout << "    ... writes the NTL in a cdfg file format" << endl;
			cout << "  write --ted poly1.cdfg" << endl;
			cout << "    ... converts the TED into NTL and then writes its into a cdfg file" << endl;
			cout << "  write poly1.dfg" << endl;
			cout << "    ... writes the DFG in file poly1.dfg" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  read, purge" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(NULL == opt.current()) {
			cerr << "Info: Missing output file" << endl;
			return CMD_INFO;
		}
		string filename = opt.current();
		Netlist* nl = getCurrentNetlist();
		if(Util::isFileExtension(filename, "cdfg")) {
			switch(fromStructure) {
			case case_ted:
				{
					if(!_pTedMan || _pTedMan->isEmpty())
						throw(string("03012. no TED Manager"));
					if(_pDfgMan) {
						delete _pDfgMan;
					}
					Convert from(_pTedMan);
					_pDfgMan = from.ted2dfgFactor();
					//_pDfgMan = new DfgMan(_pTedMan);
				}
			case case_dfg:
				if(!_pDfgMan)
					throw(string("03013. no DFG Manager"));
				_pDfgMan->writeCDFG(filename.c_str());
				break;
			case case_none:
			case case_ntl:
				if(!nl || nl->empty()) {
					cerr << "Info: There are no netlist, or empty netlist" << endl;
				} else {
					nl->writeCDFG(filename.c_str());
				}
				break;
			default:
				throw(string("03014. UNKOWN options"));
			}
		} else if(Util::isFileExtension(filename, "c")) {
			switch(fromStructure) {
			case case_ted:
			case case_dfg:
				{
					cout << "Info: The c extension can only be run from an existing netlist" << endl;
					break;
				}
			case case_none:
			case case_ntl:
				if(!nl || nl->empty()) {
					cerr << "Info: There are no netlist, or empty netlist" << endl;
				} else {
					nl->writeC(filename.c_str());
				}
				break;
			default:
				throw(string("03014. UNKOWN options"));
			}
		} else if(Util::isFileExtension(filename, "ted")) {
			switch(fromStructure) {
			case case_none:
			case case_ted:
				if(_pTedMan && !_pTedMan->isEmpty())
					_pTedMan->write(filename);
				else
					cerr << "Info: There are no TEDs" << endl;
				break;
			case case_dfg:
			case case_ntl:
			default:
				throw(string("03015. only accept --ted option or no option"));
			}
		} else if(Util::isFileExtension(filename, "dfg")) {
			switch(fromStructure) {
			case case_none:
			case case_dfg:
				if(_pDfgMan) {
					_pDfgMan->write(filename);
				} else {
					cerr << "Info: There are no DFGs" << endl;
				}
				break;
			case case_ted:
			case case_ntl:
			default:
				throw(string("03016. only accept --dfg option or no option"));
			}
		} else if(Util::isFileExtension(filename, "gappa")) {
			if(_pDfgMan) {
				_pDfgMan->writeGappa(filename);
			} else {
				cerr << "Info: There are no DFGs" << endl;
			}
		} else if(Util::isFileExtension(filename, "scr") || Util::isFileExtension(filename, "poly")) {
			cerr << "Info: Automatic script generation under development" << endl;
		} else {
			throw(string("03038. UNKOWN file format: ")+filename);
			return CMD_ERROR;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Convert a netlist into TED*/
ReturnValue Tds::CmdNTL2TED(unsigned int argc, char** argv) {
	int c;
	bool evaluateConstantNodes = Environment::getBool(Environment::CONST_CDFG_EVAL);
	ParseOption opt(argc,argv,"-brief|-help,h|-const,c");
	enum switchcase { case_brief, case_help, case_toggle};
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_toggle:
			evaluateConstantNodes = !evaluateConstantNodes;
			break;
		case case_brief:
			cout << BRIEF_NTL2TED << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_NTL2TED, CMD_NO_OPTIONS);
			cout << "  -c, --const" << endl;
			cout << "    " <<(evaluateConstantNodes ? "Disables" : "Enables")<< " the evaluation of CDFG constant nodes." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  ted2ntl, ted2dfg, dfg2ted, dfg2ntl" << endl;
			return CMD_HELP;
		}
	}
	try {
		Netlist* nl = getCurrentNetlist();
		if(!nl || nl->empty()) {
			cerr << "Info: No netlist found or empty netwlist, please use \"read\" first\n";
			return CMD_INFO;
		}
		Convert from(nl);
		TedMan* pManNew = from.ntl2ted(evaluateConstantNodes);
		string err("Error 03055. Cannot unlink primary output ");
		UPDATE_TED_MANAGER(pManNew,err);
		Netlist* nlNew = from.getNewNetlist();
		//Netlist* nlNew = ntl2ted(nl, evaluateConstantNodes);
		nlNew->setExtracTed();
		pushToHistory(nlNew);
		setNtlExtracTed(nlNew);
	} CATCH_ERRORS;
	return CMD_OK;
}

#if 0
/** @brief Convert a netlist into TED*/
ReturnValue Tds::CmdTED2NTK(unsigned int argc, char** argv) {
	int c;
	bool bChangeConstant = false;

	enum switchcase { case_brief, case_help, case_toggle};
	while((c = opt.getOption(argc, argv, "-brief|-help, h|-toggle, t"))!= EOF) {
		switch(c) {
		case case_toggle:
			bChangeConstant ^=1;
			break;
		case case_brief:
			cout << BRIEF_TED2NTK << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.getCommand(), BRIEF_TED2NTK, CMD_NO_OPTIONS);
			cout << "  -t, --toggle" << endl;
			cout << "    Toggle the use of constant values in edges into constant nodes. By default it" << endl;
			cout << "is " <<(bChangeConstant ? "on." : "off.")<< endl;
			return CMD_HELP;
		}
	}
	try {
		if(_pTedMan && !_pTedMan->isEmpty()) {
			if(!_pDfgMan) {
				_pDfgMan = new DfgMan(_pTedMan);
			}
			_pDfgMan->writeCDFG("internal_ted2ntk.cdfg");
			Netlist* nlNew = Netlist::readCDFG("internal_ted2ntk.cdfg", bChangeConstant);
			pushToHistory(nlNew);
		} else {
			cerr << "Info: The TED manager is Empty, please use \"poly\" first\n";
			return CMD_INFO;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}
#endif

/** @brief Substitute an arithmetic expression by a variable*/
ReturnValue Tds::CmdSub(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_SUB << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SUB, "[new variable =] expression");
			cout << "  expression" << endl;
			cout << "    The expression to substitute in the TED. It is usually written in the form " << endl;
			cout << "    [newVariable = algebraic expression] but only the \"algebraic expression\" is" << endl;
			cout << "    mandatory. If \"newVariable =\" is omitted, a variable named S$ is given, where" << endl;
			cout << "    $ is an integer value incremented in every substitution." << endl;
			cout << "EXAMPLE" << endl;
			cout << "  sub s1 = a+b" << endl;
			cout << "  sub c+d" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  write, read, show, poly" << endl;
			return CMD_HELP;
		}
	}
	try {
		string p = "";
		while(opt.current()) {
			p += opt.current();
			opt.next();
		}
		TedMan* pMan = _pTedMan;
		_pTedMan = new TedMan();
		parsePoly((char*)p.c_str(), false);
		TedMan* pTemp = _pTedMan;
		_pTedMan = pMan;
		pMan = pTemp;
		vector <string> vVars;
		pMan->getVars(vVars);
		for(unsigned int i = 0; i < vVars.size(); i++) {
			pTemp = _pTedMan->relocateVariable(vVars[i].c_str(), 1);
			delete _pTedMan;
			_pTedMan = pTemp;
		}
		vVars.clear();
		_pTedMan->getVars(vVars);
		Convert from(_pTedMan);
		DfgMan* pDfgMan = from.ted2dfgFactor();
		map <string, string> mExprs, mSub;
		pDfgMan->getExpression(mExprs, NULL);
		delete pDfgMan;
		from(pMan);
		pDfgMan = from.ted2dfgFactor();
		pDfgMan->getExpression(mSub, NULL);
		delete pDfgMan;
		assert(mSub.size() == 1);
		string func = mSub.begin()->first;
		string subs = mSub.begin()->second;
		for(map <string, string>::iterator p = mExprs.begin(); p != mExprs.end(); p++) {
			while((c = p->second.find(subs))!= string::npos) {
				p->second.replace(c, subs.size(), func);
			}
		}
		delete _pTedMan;
		_pTedMan = new TedMan();
		for(unsigned int i = 0; i < vVars.size(); i++) {
			_pTedMan->registerVar(vVars[i]);
		}
		for(map <string, string>::iterator p = mExprs.begin(); p != mExprs.end(); p++) {
			string s = p->first + "="+p->second;
			parsePoly((char*)s.c_str(), false);
		}
		func = func + "="+subs;
		parsePoly((char*)func.c_str(), false);
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Unlink a primary output from the TED*/
ReturnValue Tds::CmdErase(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_ERASE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_ERASE, "function");
			cout << "  function" << endl;
			cout << "    The primary output function to unlink from a multiple output TED." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  purge, extract" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(NULL == opt.current()) {
			cerr << "Info: No primary output specified" << endl;
			return CMD_INFO;
		}
		set <string> s;
		while(opt.current()) {
			s.insert(opt.current());
			opt.next();
		}
		TedMan* pManNew = _pTedMan->unlinkPO(s);
		string err("Error 03039. Cannot unlink primary output ");
		UPDATE_TED_MANAGER(pManNew,err);
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdExtract(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-netlist,n");
	enum switchcase { case_brief, case_help, case_ted, case_nl };
	enum switchgraph { graph_ted, graph_dfg, graph_nl, graph_unknown };
	switchgraph graph = graph_ted;
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_EXTRACT << endl;
			return CMD_BRIEF;
		case case_ted:
			graph = graph_ted;
			break;
		case case_nl:
			graph = graph_nl;
			break;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_EXTRACT, "[data structure] PO(s)");
			cout << "  [data structure]" << endl;
			cout << "  -t, --ted" << endl;
			cout << "    Extract PO(s)from the TED [DEFAULT BEHAVIOR]." << endl;
			cout << "  -n, --netlist" << endl;
			cout << "    Extract PO(s)from the Netlist." << endl;
			cout << endl;
			cout << "  PO(s)" << endl;
			cout << "    List of space separated primary outputs." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  purge, erase" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(NULL == opt.current()) {
			cerr << "Info: No primary output specified" << endl;
			return CMD_INFO;
		}
		switch(graph) {
		case graph_ted:
			{
				set <string> s;
				while(opt.current()) {
					s.insert(opt.current());
					opt.next();
				}
				if (!s.empty()) {
					TedMan* pManNew = _pTedMan->extractPO(s);
					string err("Error 03040. Cannot unlink primary output(s).");
					UPDATE_TED_MANAGER(pManNew,err);
				} else {
					cerr << "Info: check the output names to extract, none where found, nothing extracted." << endl;
				}
				break;
			}
		case graph_nl:
			{
				Netlist* nl = getCurrentNetlist();
				if(!nl || nl->empty()) {
					cerr << "Info: No netlist found or empty netwlist, nothing to extract\n";
					return CMD_INFO;
				}
				vector <Node*>* v = new vector <Node*>;
				while(opt.current()) {
					Node* pNode = nl->get(opt.current());
					if(pNode && pNode->isPO()) {
						v->push_back(pNode);
					}
					opt.next();
				}
				if (!v->empty()) {
					Netlist* newNl = nl->duplicateExtract(v);
					pushToHistory(newNl);
				} else {
					cerr << "Info:  Check the output names to extract, none where found, nothing extracted." << endl;
				}
				break;
			}
		default:
			throw(string("03017. UNKOWN graph type.\n"));
			break;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Transform non linear TEDs into linear by splitting non linear variables*/
ReturnValue Tds::CmdLinearize(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help, case_verbose };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_LINEARIZE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_LINEARIZE, CMD_NO_OPTIONS);
			cout << "  -v, --verbose" << endl;
			cout << "    Print to screen any information of the process." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show, listvars" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew = _pTedMan->linearize();
		UPDATE_TED_MANAGER(pManNew,"Info: Can not linearize current Ted");
		assert(_pTedMan->getContainer().check_children_are_included());
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdFixOrder(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-verbose,v|-register,r|-general,g");
	bool verboseOnly = false;
	bool regs = true;
	enum switchcase { case_brief, case_help, case_verbose, case_register, case_general };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_register:
			regs = true;
			break;
		case case_general:
			regs = false;
			break;
		case case_verbose:
			verboseOnly = true;
			break;
		case case_brief:
			cout << BRIEF_REORDERRETIMEDVARIABLES << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_REORDERRETIMEDVARIABLES, "[-r -v] [-g]");
			cout << "  -r, --register" << endl;
			cout << "    Fixes the relative order of retimed variables only [DEFAULT BEHAVIOR]" << endl;
			cout << "  -v, --verbose" << endl;
			cout << "    Print the reorder commands to be executed, but do not execute those commands." << endl;
			cout << "  -g, --general" << endl;
			cout << "    Check the overall order of variables and fix any out of order variable." << endl;
			cout << "SEE ALSO" <<endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		TedMan* pManNew = NULL;
		if (regs) {
			pManNew = _pTedMan->fixorderRetimedVars(verboseOnly);
		} else {
#if SEE_BUG_IN_FIXORDERVARS
			pManNew = _pTedMan->fixorderVars(verboseOnly);
#else
			pManNew = _pTedMan->patchorderVars();
#endif
		}
		UPDATE_TED_MANAGER(pManNew,DEFAULT_ERROR_UPDATE);
	} CATCH_ERRORS;
	return CMD_OK;

}

/** @brief Heuristically optimize the level of the variable*/
ReturnValue Tds::CmdSift(unsigned int argc, char** argv) {
	int c;
	DEFAULT_REORDER_TYPE(orderst);
	ParseOption opt(argc,argv,"-brief|-help,h|"\
					"-reloc,r|-swap,s|-proper,p|"\
					"-node|-nMUL,-nm,-mpy|-op|-opscheduled|-bitwidth|-gappa|"\
					"-gMUX,gm,-gmux|-gLatency,gl,-glatency|-gREG,-gr,-garch");
	TedOrderCost* cmpFunc = new MinOpCount();
	enum switchcase { case_brief, case_help,
		case_reloc, case_swap, case_proper,
		case_node, case_mul, case_op, case_opscheduled, case_bitwidth, case_gappa,case_gmux,case_glatency,case_garch };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			delete orderst;
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_node:
			delete cmpFunc;
			cmpFunc = new MinNodes();
			break;
		case case_mul:
			delete cmpFunc;
			cmpFunc = new MinMpyCount();
			break;
		case case_op:
			delete cmpFunc;
			cmpFunc = new MinOpCount();
			break;
		case case_opscheduled:
			delete cmpFunc;
			cmpFunc = new MinOperations();
			break;
		case case_bitwidth:
			delete cmpFunc;
			cmpFunc = new MinBitwidth();
			break;
		case case_gmux:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new NtlMinMux();
			break;
		case case_glatency:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new NtlMinLatency();
			break;
		case case_garch:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new NtlMinArch();
			break;
		case case_gappa:
#if !defined(_WIN32)
			delete cmpFunc;
			cmpFunc = new MinGappaBound();
#else
			cout << "Info: switch not available in WINDOWS, reverts to default behavior" << endl;
#endif
			break;
		case case_brief:
			cout << BRIEF_SIFT << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SIFT, "[reorder algorithm] [cost function] variable");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << MAN_REORDER_COST << endl;
			cout << "  variable" << endl;
			cout << "    The variable name to be sifted." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		char* varname = opt.current();
		if(!varname) {
			cerr << "Info: No variable specified, nothing to sift" << endl;
			return CMD_INFO;
		}
		TedMan* pManNew = _pTedMan->siftVar(varname, orderst, cmpFunc);
		UPDATE_TED_MANAGER(pManNew,"Error 0341. Cannot linearize current TED");
		print_order_report("sift.report",orderst);
	} CATCH_ERRORS;
	delete orderst;
	delete cmpFunc;
	return CMD_OK;
}

/** @brief Flip the order of a linearized variable*/
ReturnValue Tds::CmdFlip(unsigned int argc, char** argv) {
	int c;
	DEFAULT_REORDER_TYPE(orderst);
	ParseOption opt(argc,argv,"-brief|-help,h|-reloc,r|-swap,s|-proper,p");
	enum switchcase { case_brief, case_help, case_reloc, case_swap, case_proper };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			//default
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			delete orderst;
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_brief:
			cout << BRIEF_FLIP << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_FLIP, "[reorder algorithm] variable");
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << "  variable" << endl;
			cout << "    The linearized variable name to flip." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, fixorder, jumpAbove, jumpBelow, reloc, reorder, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		char* varname = opt.current();
		if(!varname) {
			cerr << "Info: No variable specified, nothing to flip" << endl;
			return CMD_INFO;
		}
		TedMan* pManNew = _pTedMan->flipVar(varname, orderst);
		UPDATE_TED_MANAGER(pManNew,"Error 0342. No TED manager produced");
		print_order_report("flip.report",orderst);
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}

/** @brief Reorder the variables in the TED*/
ReturnValue Tds::CmdReorder(unsigned int argc, char** argv) {
	int c;
	bool bUseReloc = true;
	int  reorder_mode = 1;
	bool bExhaustive = false;
	bool grouped = false;
	bool unstopable = false;
	bool break_into_strides = true;
	bool stride_backtrack = false;
	vector<string> splitWindow;
	//TedOrderStrategy* orderst = new TedOrderSwap(_pTedMan);
	DEFAULT_REORDER_TYPE(orderst);
	TedCompareCost* cmpFunc = NULL;
	Cost::setCompare(Cost::STRICTLESS);
	ParseOption opt(argc,argv,"-brief|-help,h|" "-reloc,r|-swap,s|-proper,p|" \
					"-node|-nMUL,nm,-mul,m|-op|-opscheduled|-dLatency,dl,-latency|" \
					"-bitwidth|-gappa|" \
					"-gMUX,-gm,-gmux|-gLatency,gl,-glatency|-gREG,gr,-garch|" \
					"-exhaustive,e|-sift,s|-group,g|-annealing,a|" \
					"-no_stride|-stride_backtrack|-le|-end|w,-window:");
	enum Iterations { EXHAUSTIVE, SIFT, ANNEALING };
	Iterations niter = SIFT;
	enum switchcase { case_brief, case_help, \
		case_reloc, case_swap, case_proper, \
			case_node, case_mul, case_op, case_opscheduled, \
			case_latency, case_bitwidth, case_gappa, case_gmux, case_glatency, case_greg, \
			case_exhaustive, case_sift, case_group, case_anneal, \
			case_no_stride, case_stride_backtrack, \
			case_le, case_end, case_window};
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_end:
			unstopable = true;
			break;
		case case_proper:
			delete orderst;
			orderst = new TedOrderProper(_pTedMan);
			break;
		case case_swap:
			//default
			delete orderst;
			orderst = new TedOrderSwap(_pTedMan);
			break;
		case case_reloc:
			orderst = new TedOrderReloc(_pTedMan);
			break;
		case case_node:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinNodes();
			break;
		case case_mul:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinMpyCount();
			break;
		case case_op:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinOpCount();
			break;
		case case_opscheduled:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinOperations();
			break;
		case case_latency:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinLatency();
			break;
		case case_bitwidth:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinBitwidth();
			break;
		case case_gappa:
#if !defined(_WIN32)
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new MinGappaBound();
#else
			cout << "Info: switch not available in WINDOWS, reverts to default behavior" << endl;
#endif
			break;
		case case_gmux:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new NtlMinMux();
			break;
		case case_glatency:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new NtlMinLatency();
			break;
		case case_greg:
			if(cmpFunc)delete cmpFunc;
			cmpFunc = new NtlMinArch();
			break;
		case case_exhaustive:
			niter = EXHAUSTIVE;
			if(grouped) {
				cout << "Warning: Exhaustive tries all possible permutations, therefore the option" << endl;
				cout << "\"--group\" will be disregarded. \"--group\" cannot be used with \"--exhaustive\"." << endl;
				grouped = false;
			}
			break;
		case case_sift:
			grouped = false;
			niter = SIFT;
			break;
		case case_anneal:
			niter = ANNEALING;
			break;
		case case_group:
			grouped = true;
			if(niter==EXHAUSTIVE) {
				cout << "Warning: Grouping clusters linearized variables, thus preventing from" << endl;
				cout << "exploring all possible permutations, therefore the option \"--exhaustive\"" << endl;
				cout << "will be disregarded." << endl;
				niter = SIFT;
			}
			break;
		case case_no_stride:
			break_into_strides = false;
			break;
		case case_stride_backtrack:
			stride_backtrack = true;
			break;
		case case_le:
			Cost::setCompare(Cost::LESSEQUAL);
			break;
		case case_window:
			{
				string window_size = opt.getOptionArgument();
				splitWindow = Util::split(window_size, ", ");
				if (splitWindow.size()!=2) {
					cout << "Info: Argument require both a lower variable and an upper variable" << endl;
					return CMD_INFO;
				}
			}
			break;
		case case_brief:
			cout << BRIEF_REORDER << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_REORDER, "[comparison scheme] [reorder algorithm] [cost function] [iteration strategy] [window size]");
			cout << "  -v, --verbose" << endl;
			cout << "    Print to screen any information of the process." << endl;
			cout << MAN_REORDER_SCHEME << endl;
			cout << MAN_REORDER_ALGORITHM << endl;
			cout << MAN_REORDER_COST << endl;
			cout << MAN_REORDER_ITERATION_STRATEGY << endl;
			cout << "  [window size]" << endl;
			cout << "  --window {lower_variable,upper_variable}" << endl;
			cout << "  Constraint the iteration strategy to the window defined by the lower and upper variables" << endl;
			cout << "  if no variables are provided, the entire variable set is used which is the default behavior." << endl;
			cout << "EXAMPLE" << endl;
			cout << "  vars e c b f d a" << endl;
			cout << "  poly F = (a-b+c)*(c+d)*(e+f)" << endl;
			cout << "  show" << endl;
			cout << "  reorder --" << Environment::getStr(Environment::REORDER_TYPE)<< " --node --sift" << endl;
			cout << "  show" << endl;
			cout << "  echo the above is equivalent to \"reorder --node\"" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(!cmpFunc) {
			cmpFunc = new MinMpyCount();
		}
		TedMan* pManNew = NULL;
		ENSURE_TED_EXIST;
		if (!splitWindow.empty()) {
			orderst->defineWindow(splitWindow[0], splitWindow[1]);
		}
		switch(niter) {
		case EXHAUSTIVE:
			{
				orderst->unstopable();
				pManNew = _pTedMan->permuteOrdering(orderst, cmpFunc,break_into_strides);
				break;
			}
		case ANNEALING:
			{
				pManNew = _pTedMan->annealing(orderst,cmpFunc,break_into_strides,stride_backtrack);
				break;
			}
		case SIFT: //fallthrough
		default:
			{
				if(grouped)
					pManNew = _pTedMan->siftGroupedAll(orderst, cmpFunc);
				else
					pManNew = _pTedMan->siftAll(orderst, cmpFunc);
				break;
			}
		}
		print_order_report("reorder.report",orderst);
		UPDATE_TED_MANAGER(pManNew,"Info. Cannot optimize current TED");
	} CATCH_ERRORS;
	delete orderst;
	delete cmpFunc;
	return CMD_OK;
}

void Tds::print_order_report(const char* filename, TedOrderStrategy* orderst) {
	ofstream ofile(filename);
	if(ofile.is_open()) {
		ofile << "#########################################" << endl;
		ofile << "# Last entry might be duplicated and it #" << endl;
		ofile << "# correspond to the best ordering found #" << endl;
		ofile << "# http://incascout.ecs.umass.edu/main   #" << endl;
		ofile << "#########################################" << endl;
		orderst->printHistory(ofile);
		ofile.close();
	}
}


/** @brief Customizable cost function for Reorder the variables in the TED*/
ReturnValue Tds::CmdCustomReorder(unsigned int argc, char** argv) {
	int c;
	bool bUseReloc = true;
	int  reorder_mode = 1;
	bool bExhaustive = false;
	bool grouped = false;
	bool unstopable = false;
	double stop_value = 0.0;
	bool break_into_strides = true;
	bool stride_backtrack=false;
	TedOrderStrategy* orderst = new TedOrderProper(_pTedMan);
	TedCompareCost* cmpFunc = new TedCompareBaseStop();
	TedCompareCost* compare_end = cmpFunc;
	Cost::setCompare(Cost::STRICTLESS);
	pair<bool, int> argument;
	ParseOption opt(argc,argv,"-brief|-help,h|" \
					"-node:0|-edge:0|-edge0:0|-edgeN:0|"\
					"-nMUL,nm:0|-nADD,na:0|-rMPY,rm:0|-rADD,ra:0|-dLatency,dl:0|"\
					"-bitwidth|-gappa|"\
					"-gMUX,gm:0|-gLatency,gl:0|-gREG,gr:0|-gArea,ga:0|"\
					"-exhaustive,e:0|-sift,s:0|-group,g|-annealing,a:0|-no_stride|-stride_backtrack|-lessequal,le|-end");
	enum Iterations { EXHAUSTIVE, SIFT, ANNEALING };
	Iterations niter = SIFT;
	enum switchcase { case_brief, case_help, case_node, case_edge, case_edge0, case_edgeN, \
		case_nmul, case_nadd, case_rmpy, case_radd, case_dlatency, case_bitwidth, case_gappa, \
			case_gmux, case_glatency, case_greg, case_garea, \
			case_exhaustive, case_sift, case_group, case_anneal, case_no_stride, case_stride_backtrack,case_le, case_end};
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_end:
			unstopable = true;
			break;
		case case_node:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			TedNodes::stop = argument.second;
			cmpFunc = new TedNodes(cmpFunc);
			break;
		case case_edge:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			TedEdges::stop = argument.second;
			cmpFunc = new TedEdges(cmpFunc);
			break;
		case case_edge0:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			TedEdges0::stop = argument.second;
			cmpFunc = new TedEdges0(cmpFunc);
			break;
		case case_edgeN:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			TedEdgesN::stop = argument.second;
			cmpFunc = new TedEdgesN(cmpFunc);
			break;
		case case_nmul:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			DfgOpMpy::stop = argument.second;
			cmpFunc = new DfgOpMpy(cmpFunc);
			break;
		case case_nadd:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			DfgOpAdd::stop = argument.second;
			cmpFunc = new DfgOpAdd(cmpFunc);
			break;
		case case_rmpy:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			DfgScheduleMpyRes::stop = argument.second;
			cmpFunc = new DfgScheduleMpyRes(cmpFunc);
			break;
		case case_radd:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			DfgScheduleOpAddRes::stop = argument.second;
			cmpFunc = new DfgScheduleOpAddRes(cmpFunc);
			break;
		case case_dlatency:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			DfgScheduleOpLatency::stop = argument.second;
			cmpFunc = new DfgScheduleOpLatency(cmpFunc);
			break;
		case case_bitwidth:
			cmpFunc = new TedWidth(cmpFunc);
			break;
		case case_gappa:
#if !defined(_WIN32)
			cmpFunc = new DfgGappa(cmpFunc);
#else
			cout << "Info: gappa switch is not available in WINDOWS, ignoring" << endl;
#endif
			break;
		case case_gmux:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			GautMux::stop = argument.second;
			cmpFunc = new GautMux(cmpFunc);
			break;
		case case_glatency:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			GautLatency::stop = argument.second;
			cmpFunc = new GautLatency(cmpFunc);
			break;
		case case_greg:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			GautReg::stop = argument.second;
			cmpFunc = new GautReg(cmpFunc);
			break;
		case case_garea:
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			GautArea::stop = argument.second;
			cmpFunc = new GautArea(cmpFunc);
			break;
		case case_exhaustive:
			niter = EXHAUSTIVE;
			if(grouped) {
				cout << "Warning: Exhaustive tries all possible permutations, therefore the option" << endl;
				cout << "\"--group\" will be disregarded. \"--group\" cannot be used with \"--exhaustive\"." << endl;
				grouped = false;
			}
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			stop_value = argument.second;
			break;
		case case_sift:
			grouped = false;
			niter = SIFT;
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			stop_value = argument.second;
			break;
		case case_anneal:
			niter = ANNEALING;
			argument = Util::atoi(opt.getOptionArgument());
			if(!argument.first) {
				opt.disregardOptionArgument();
			}
			stop_value = argument.second;
			break;
		case case_group:
			grouped = true;
			if(niter==EXHAUSTIVE) {
				cout << "Warning: Grouping clusters linearized variables, thus preventing from" << endl;
				cout << "exploring all possible permutations, therefore the option \"--exhaustive\"" << endl;
				cout << "will be disregarded." << endl;
				niter = SIFT;
			}
			break;
		case case_no_stride:
			break_into_strides = false;
			break;
		case case_stride_backtrack:
			stride_backtrack = true;
			break;
		case case_le:
			Cost::setCompare(Cost::LESSEQUAL);
			break;
		case case_brief:
			cout << BRIEF_CUSTOM_REORDER << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_CUSTOM_REORDER, "[comparison scheme] [reorder algorithm] [list of cost functions] [number of iterations]");
			cout << "  -v, --verbose" << endl;
			cout << "    Print to screen any information of the process." << endl;
			cout << MAN_REORDER_SCHEME << endl;
			cout << MAN_CUSTOM_REORDER_COST << endl;
			cout << MAN_REORDER_ITERATION_STRATEGY << endl;
			cout << "EXAMPLE" << endl;
			cout << "  vars e c b f d a" << endl;
			cout << "  poly F = (a-b+c)*(c+d)*(e+f)" << endl;
			cout << "  show" << endl;
			cout << "  creorder --gMUX --gLatency" << endl;
			cout << "  echo minimizes the latency of the GAUT implementation and then its number of muxes" << endl;
			cout << "  creorder --gLatency --gMUX" << endl;
			cout << "  echo minimizes the latency of the GAUT implementation and then its number of muxes" << endl;
			cout << "  show" << endl;
			cout << "  echo the above is equivalent to \"reorder --node\"" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  bbldown, bblup, bottom, exchange, flip, fixorder, jumpAbove, jumpBelow, reloc, reorder*, sift, setenv, top" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(cmpFunc == compare_end) {
			delete cmpFunc;
			cout << "Info: missing optimization rule, nothing to do" << endl;
		} else {
			ENSURE_TED_EXIST;
			TedMan* pManNew = NULL;
			orderst->stop_criteria(stop_value);
			switch(niter) {
			case EXHAUSTIVE:
				{
					orderst->unstopable();
					pManNew = _pTedMan->permuteOrdering(orderst,cmpFunc,break_into_strides);
					break;
				}
			case ANNEALING:
				{
					pManNew = _pTedMan->annealing(orderst,cmpFunc,break_into_strides,stride_backtrack);
					break;
				}
			case SIFT: //fallthrough
			default:
				{
					if(grouped)
						pManNew = _pTedMan->siftGroupedAll(orderst, cmpFunc);
					else
						pManNew = _pTedMan->siftAll(orderst, cmpFunc);
					break;
				}
			}
			UPDATE_TED_MANAGER(pManNew,"Info. Cannot optimize current TED");
			ofstream ofile("tedorder.report");
			if(ofile.is_open()) {
				Cost* compare = cmpFunc->compute_cost(*_pTedMan);
				compare->set_stream(&ofile);
				ofile << "#########################################" << endl;
				ofile << "# Last entry might be duplicated and it #" << endl;
				ofile << "# correspond to the best ordering found #" << endl;
				ofile << "# http://incascout.ecs.umass.edu/main   #" << endl;
				ofile << "#########################################" << endl;
				orderst->printHistory(ofile);
				ofile << "#########################################" << endl;
				compare->report(cmpFunc);
				ofile.close();
				delete compare;
			}
		}
	} CATCH_ERRORS;
	delete orderst;
	return CMD_OK;
}


/** @brief Perform dynamic factorization*/
ReturnValue Tds::CmdDFactor(unsigned int argc, char** argv) {
	int c;
	unsigned int toplevel = 0;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_DFACTOR << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFACTOR, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  scse, lcse, dcse, candidate, bottomscce, bottomdcse" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(toplevel == 0) {
			toplevel = _pTedMan->getContainer().getMaxLevel();
		}
		TedMan* pManNew = _pTedMan->dynFactorize();
		UPDATE_TED_MANAGER(pManNew,"Info. Cannot optimize current TED");
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Replace constant multipliers by shifters*/
ReturnValue Tds::CmdShifter(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-dfg,d");
	enum switchcase { case_brief, case_help, case_ted, case_dfg };
	enum switchgraph { graph_ted, graph_dfg, graph_nl, graph_unknown };
	switchgraph graph = graph_ted;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_ted:
			graph = graph_ted;
			break;
		case case_dfg:
			graph = graph_dfg;
			break;
		case case_brief:
			cout << BRIEF_SHIFTER << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SHIFTER, "[Data Structure]");
			cout << "  [data structure]" << endl;
			cout << "  -t, --ted" << endl;
			cout << "    Replace TED edge multiplications by constant multiplications and additions" << endl;
			cout << "    in the TED [DEFAULT BEHAVIOR]." << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    It creates a DFG representation of the \"shifter --ted\" operation, without" << endl;
			cout << "    changing the TED data structure." << endl;
			cout << "    WARNING:" << endl;
			cout << "      This feature is only good for visualization. Any further operations" << endl;
			cout << "      in the DFG structure most likely will result in ERROR 01014." << endl;
			cout << "EXAMPLE" << endl;
			cout << "  poly 2*a+b*9-19*c" << endl;
			cout << "  show" << endl;
			cout << "  show -d" << endl;
			cout << "  shifter" << endl;
			cout << "  show" << endl;
			cout << "  show -d" << endl;
			cout << "  purge" << endl;
			cout << "  echo compare the above DFG graph with the one produced below" << endl;
			cout << "  poly 2*a+b*9-19*c" << endl;
			cout << "  shifter --dfg" << endl;
			cout << "  show" << endl;
			cout << "  show -d" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  remapshift" << endl;
			return CMD_HELP;
		}
	}
	try {
		switch(graph) {
		case graph_ted:
			{
				ENSURE_TED_EXIST;
				TedMan* pTedManNew = useShifter();
				UPDATE_TED_MANAGER(pTedManNew,"Error 03043. No TED manager produced");
				break;
			}
		case graph_dfg:
			{
				ENSURE_DFG_EXIST;
				string shiftName = Environment::getStr(Environment::CONST_PREFIX);
				shiftName += "2";
				DfgMan* pDfgManNew = _pDfgMan->useShifter(shiftName);
				if(pDfgManNew && pDfgManNew!=_pDfgMan) {
					delete _pDfgMan;
					_pDfgMan = pDfgManNew;
				}
				break;
			}
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Replace constant multipliers by shifters*/
ReturnValue Tds::CmdEvaluateConstants(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_EVALUATE_CONSTANTS << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_EVALUATE_CONSTANTS, CMD_NO_OPTIONS);
			cout << "NOTE:" << endl;
			cout << "  It replaces in the DFG graph any constant variable predefined by" << endl;
			cout << "  \"" << Environment::getStr(Environment::CONST_PREFIX) << "\" by its corresponding numeric value." << endl;
			cout << "EXAMPLE" << endl;
			cout << "  poly 18*a+7*b+3*c" << endl;
			cout << "  shifter -d" << endl;
			cout << "  show -d" << endl;
			cout << "  dfgevalconst" << endl;
			cout << "  show -d" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		_pDfgMan->evaluateConstants();
	} CATCH_ERRORS;
	return CMD_OK;
}

#ifdef FUNCTIONS_DEPRECATED
/** @brief Replace constant multipliers by shifter*/
ReturnValue Tds::CmdShiftreplace(unsigned int argc, char** argv) {
	int c;
	bool isUpto = true;
	TedMan* pTedManNew;
	enum switchcase { case_brief, case_help, case_upto, case_only };
	while((c=opt.getOption(argc,argv,"-brief|-help,h|-upto|-only"))!= EOF) {
		switch(c) {
		case case_upto:
			isUpto = true;
			break;
		case case_only:
			isUpto = false;
			break;
		case case_brief:
			cout << BRIEF_SHIFT << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.getCommand(), BRIEF_SHIFT, "[--upto --only] [level]");
			cout << "  --upto" << endl;
			cout << "    Transforms all constants up to the given \"level\" into its CSD representation" << endl;
			cout << "  --only" << endl;
			cout << "    Transforms all constants with CSD \"level\" into its CSD representation" << endl;
			cout << "  level" << endl;
			cout << "    The desired level in the Canonical Signed Digit(CSD)representation. For i.e." << endl;
			cout << "     - number 4 is level 1 as it is expressed as 2^2(10)" << endl;
			cout << "     - number 3 is level 2 as it is expressed as 2^2-1(1-1)" << endl;
			cout << "     - number 5 is level 2 as it is expressed as 2^2+1(11)" << endl;
			cout << "     - number 13 is level 3 as it is expressed as 2^4-2^2+1(10-11)" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  shifter" << endl;
			return CMD_HELP;
		}
	}
	try {
		if(NULL == opt.currentArgument()) {
			cerr << "Info: Missing level" << endl;
			return CMD_INFO;
		}
		pTedManNew = useShifter();
		UPDATE_TED_MANAGER(pTedManNew,"Error 03044. No TED manager produced");
	} CATCH_ERRORS;
	return CMD_OK;
}
#endif

/** @brief Balance the DFG or NTL to minimize clock steps*/
ReturnValue Tds::CmdBalance(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-dfg,d|-netlist,n");
	enum switchcase { case_brief, case_help, case_dfg, case_nl };
	enum switchgraph { graph_ted, graph_dfg, graph_nl, graph_unknown };
	switchgraph graph = graph_dfg;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_DFG_BALANCE << endl;
			return CMD_BRIEF;
		case case_dfg:
			graph = graph_dfg;
			break;
		case case_nl:
			graph = graph_nl;
			break;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFG_BALANCE, "[data structure]");
			cout << "  [data structure]" << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    Balance the DFG [DEFAULT BEHAVIOR]." << endl;
			cout << "  -n, --netlist" << endl;
			cout << "    Balance the NTL." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show, dfgschedule, dfgarea" << endl;
			return CMD_HELP;
		}
	}
	try {
		Netlist* nl = NULL;
		switch(graph) {
		case graph_dfg:
			ENSURE_DFG_EXIST;
			_pDfgMan->balance();
			break;
		case graph_nl:
			nl = getCurrentNetlist();
			if(!nl || nl->empty()) {
				cerr << "Info: There are no netlists, or empty netlist." << endl;
				return CMD_INFO;
			}
			nl = nl->balance();
			break;
		default:
			throw(string("03018. UNKOWN graph type.\n"));
			break;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Balance the DFG to minimize the area*/
ReturnValue Tds::CmdDfgArea(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_DFG_AREA << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFG_AREA, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  show, dfgschedule, balance" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		_pDfgMan->balanceAreaRecovery();
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Perform the scheduling of the dfg*/
ReturnValue Tds::CmdDfgSchedule(unsigned int argc, char** argv) {
	int c;
	unsigned int rMPY=0, rADD=0, rSUB=0;
	bool pipeline;
	pair<bool, int> pe;
	ParseOption opt(argc,argv,"-brief|-help,h|-rMPY,rm,-mul,m:|-rADD,ra,-add,a:|-rSUB,rs,-sub,s:|-pipeline,p");
	enum switchcase { case_brief, case_help, case_mul, case_add, case_sub, case_pipeline };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_mul:
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				rMPY = pe.second;
			} else {
				cerr << "Info: The argument of --mul must be an integer" << endl;
				return CMD_INFO;
			}
			break;
		case case_add:
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				rADD= pe.second;
			} else {
				cerr << "Info: The argument of --add must be an integer" << endl;
				return CMD_INFO;
			}
			break;
		case case_sub:
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				rSUB= pe.second;
			} else {
				cerr << "Info: The argument of --sub must be an integer" << endl;
				return CMD_INFO;
			}
			break;
		case case_pipeline:
			pipeline = true;
			break;
		case case_brief:
			cout << BRIEF_DFG_SCHEDULE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFG_SCHEDULE, "[-rMPY num_mpy] [-rADD num_add] [-rSUB num_sub]");
			cout << "  -rm, --rMPY positive integer {legacy -m, --mul}" << endl;
			cout << "    The maximum number of multipliers to be used by the scheduler" << endl;
			cout << "  -ra, --rADD positive integer {legacy -a, --add}" << endl;
			cout << "    The maximum number of adders to be used by the scheduler" << endl;
			cout << "  -rs, --rSUB positive integer {legacy -s, --sub}" << endl;
			cout << "    The maximum number of subtractors to be used by the scheduler" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show, dfgarea, balance" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		_pDfgMan->schedule(rMPY, rADD, rSUB, pipeline);
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdEval(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_EVAL << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_EVAL, "variable value");
			cout << "  variable" << endl;
			cout << "    The variable name to evaluate." << endl;
			cout << "  value" << endl;
			cout << "    The value of the variable." << endl;
			cout << "NOTE" << endl;
			cout << "  At least one remaining variable must be left in the TED graph" << endl;
			cout << "WARNING" << endl;
			cout << "  This command has not been fully tested!. Results could be wrong." << endl;
			//cout << "SEE ALSO" << endl;
			//cout << "  eval_const" << endl;
			return CMD_HELP;
		}
	}
	try {
		char* variableName = opt.current();
		opt.next();
		char* variableValue = opt.current();
		if((NULL==variableName)||(NULL==variableValue)) {
			cerr << "Info: Missing information. Require both variable and its value" << endl;
			return CMD_INFO;
		}
		pair<bool, unsigned long> value = Util::atoi(variableValue);
		if(value.first == false) {
			cerr << "Info: The value has to be an integer value" << endl;
			return CMD_INFO;
		}
		_pTedMan->eval(variableName,value.second);
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Print out statistics of the number of operators in the netlist*/
ReturnValue Tds::CmdPrintNTL(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-names|-stat,s:0");
	enum switchcase { case_brief, case_help, case_names,case_stats };
	switchcase op = case_stats;
	int previousNTL = 0;
	pair<bool, int> pe;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_stats:
			op = case_stats;
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				previousNTL = pe.second;
			} else {
				// backtrack one argument, use current netlist
				opt.disregardOptionArgument();
			}

			break;
		case case_names:
			op = case_names;
			break;
		case case_brief:
			cout << BRIEF_NTL_PRINT << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_NTL_PRINT, "[--names] [--stat {integer}]");
			cout << "  --names" << endl;
			cout << "    Prints the design names in the netlist history and its history index" << endl;
			cout << "  --stat {postive integer}" << endl;
			cout << "    Prints statistic of the netlist design with the history index provided," << endl;
			cout << "    the current netlist always have a histoy index value of 0." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show, print" << endl;
			return CMD_HELP;
		}
	}
	try {
		Netlist* nl = getCurrentNetlist();
		if(!nl || nl->empty()) {
			cerr << "Info: No netlist found or empty netlist, please load a netslit first\n";
			return CMD_INFO;
		}
		switch (op) {
		case case_names:
			{
				int history = _lHistory.size();
				for (list<Netlist*>::reverse_iterator p = _lHistory.rbegin(); p != _lHistory.rend(); p++) {
					nl =*p;
					cout << "(" << --history << ") " << nl->getName() << (nl->isExtracTed() ? " (extracted)" : "") << endl;
				}
				break;
			}
		case case_stats:
			{
				int nPi, nPo, nOther;
				int nAdd, nSub, nMul;
				int nReg;
				int nLatency;
				if(0 != previousNTL) {
					int hsize = _lHistory.size();
					if(0==hsize) {
						cerr << "Info: There are no netlists" << endl;
						return CMD_INFO;
					} else if(previousNTL > hsize - 1 || previousNTL < 0) {
						previousNTL = (hsize>0) ? hsize-1 : 0;
						cout << "Warning: " << hsize << " netlists in record [0-" << previousNTL << "]. Last one is used." << endl;
					}
					nl = getPreviousNetlist(previousNTL);
				}

				nl->getStats(nAdd, nSub, nMul, nOther, nPi, nPo, nLatency, nReg);
				printf("%s\n", nl->getName());
				printf("  nPOs =%5d, nPIs =%5d, nStr =%5d\n", nPo, nPi, nOther);
				printf("  nMUL =%5d, nADD =%5d, nSUB =%5d\n", nMul, nAdd, nSub);
				printf("  nREG =%5d, Ltcy =%5d", nReg, nLatency);
				printf("\n");
				break;
			}
		default: break;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

#ifdef FUNCTIONS_DEPRECATED
/** @brief Unlinearize a DFG linearized in TED*/
ReturnValue Tds::CmdDfgUnlinearize(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_UNLINEARIZE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_UNLINEARIZE, "variable");
			cout << "  variable" << endl;
			cout << "    The name of the variable to unlinearize. If ommited, all linearized variables" << endl;
			cout << "    are unlinearized" << endl;
			cout << "NOTE" << endl;
			cout << "  To be deprecated" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		if(NULL != opt.current()) {
			_pDfgMan->unlinearize(opt.current());
		}
	} CATCH_ERRORS;
	return CMD_OK;
}
#endif

/** @brief Remaps the L shifters to <<*/
ReturnValue Tds::CmdDfgRemapShifter(unsigned int argc, char** argv) {
	int c;
	int maxShiftLevel = 100;
	pair<bool, int> pe;
	ParseOption opt(argc,argv,"-brief|-help,h|-level,l:3");
	enum switchcase { case_brief, case_help, case_level };
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_level:
			pe = Util::atoi(opt.getOptionArgument());
			if(pe.first) {
				maxShiftLevel = pe.second;
			} else {
				cerr << "Info: The argument of --max must be an integer" << endl;
				return CMD_INFO;
			}
			break;
		case case_brief:
			cout << BRIEF_REMAP_SHIFTER << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_REMAP_SHIFTER, CMD_NO_OPTIONS);
			cout << "EXAMPLE" << endl;
			cout << "  poly 9*a+19*b" << endl;
			cout << "  shifter" << endl;
			cout << "  show -d" << endl;
			cout << "  remapshif" << endl;
			cout << "WARNING" << endl;
			cout << "  This command works only after executing \"shifter --ted\" or \"shifter\"." << endl;
			cout << "  Its execution after \"shifter --dfg\" results on an Error that can be" << endl;
			cout << "  reverted by executing \"purge --dfg\"." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  shifter" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		_pDfgMan->remapShifter(maxShiftLevel);
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Used to dump lines into the screen in script files*/
ReturnValue Tds::CmdEcho(unsigned int argc, char** argv) {
	for(unsigned int c=1; c<argc; c++) {
		cout << argv[c] << " ";
	}
	cout << endl;
	return CMD_OK;
}

/** @brief Used by regression tests to check all POs are zero*/
ReturnValue Tds::CmdVerify(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_VERIFY << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_VERIFY, "output1 = output2");
			return CMD_HELP;
		}
	}
	try {
		string verLine;
		while(opt.current()) {
			verLine += opt.current();
			opt.next();
		}
		if(verLine.empty()) {
			cerr << "Info: no outputs provided to be verified equality." << endl;
			return CMD_INFO;
		}
		vector<string> verToken = Util::split(verLine, "=");
		if(verToken.size() ==2) {
			cout << (_pTedMan->verify(verToken[0], verToken[1]) ? "true" : "false") << endl;
		} else {
			cerr << "Info: use the format, output1=output2" << endl;
			return CMD_INFO;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Used by regression tests to check all POs are zero*/
ReturnValue Tds::CmdRegressionTest(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-ted,t|-dfg,d");
	bool do_dfg = false;
	bool do_ted = true;
	enum switchcase { case_ted, case_dfg};
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_dfg:
			do_dfg = true;
			break;
		case case_ted:
		default:
			do_ted = true;
		}
	}
	if(do_dfg) {
		if(_pDfgMan) {
			stringstream message;
			// first double check the DFG has an "_diff" output
			do_ted = _pDfgMan->regressionTest(message);
			if(do_ted) {
				cout << "[DFG passed";
				//send the current passing DFG with PO_DIFF=PO-PO_READ to TED
				delete _pTedMan;
				Convert from(_pDfgMan);
				_pTedMan = from.dfg2ted();
				//now prepare the evaluation of TED, only PO_DIFF outputs exists
				delete _pDfgMan;
				from(_pTedMan);
				_pDfgMan = from.ted2dfgFactor();
				//_pDfgMan = new DfgMan(_pTedMan);
				//evaluate PO_DIFF
				delete _pTedMan;
				from(_pDfgMan);
				_pTedMan = from.dfg2ted();
			} else {
				cout << "[DFG FAILED";
			}
			cout << message.str()<< "]";
		} else {
			cout << ", Stopped(empty DFG Manager)";
		}
	}
	if(do_ted) {
		if(_pTedMan) {
			stringstream message;
			bool test = _pTedMan->regressionTest(message);
			if(test)
				cout << ", Passed";
			else
				cout << ", FAILED";
			cout << message.str();
		} else {
			cout << ", Stopped(empty TED Manager)";
		}
	}
	flush(cout);
	return CMD_OK;
}

ReturnValue Tds::CmdDecompose(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-all,a|-st|-pt|-force,f");
	enum action { st_only, pt_only, both_pt_st, do_all, none };
	action todo = both_pt_st;
	enum switchcase { case_brief, case_help, case_all, case_sumterm, case_productterm, case_force };
	bool onlyPos = true;
	while((c = opt.getOption())!= EOF) {
		switch(c) {
		case case_sumterm:
			todo = st_only;
			break;
		case case_productterm:
			todo = pt_only;
			break;
		case case_all:
			todo = do_all;
			break;
		case case_force:
			onlyPos = false;
			break;
		case case_brief:
			cout << BRIEF_DECOMPOSE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DECOMPOSE, "[-a|--pt|--st] [--force]");
			cout << "  --force" << endl;
			cout << "    perform aggressive extraction by treating support as primary outputs" << endl;
			cout << "  --st"<<endl;
			cout << "    Decompose all sum terms available in the current TED" << endl;
			cout << "  --pt"<<endl;
			cout << "    Decompose all product terms available in the current TED" << endl;
			cout << "  -a, --all"<< endl;
			cout << "    Decompose changing the given order if necessary until"<< endl;
			cout << "    the entire TED is reduced to a single node" << endl;
			cout << "DETAILS" << endl;
			cout << "  The TED must be linearized first" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  show" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_TED_EXIST;
		switch(todo) {
		case do_all:
			{
				// when onlyPos is false one chain +/*  might be
				// re-extracted letting to an infinite recursion
				TedMan* pTedManNew = _pTedMan->decomposeAll(true);
				if(pTedManNew) {
					delete _pTedMan;
					_pTedMan = pTedManNew;
				}
				break;
			}
		case both_pt_st:
			_pTedMan->decompose(onlyPos);
			break;
		case st_only:
			_pTedMan->decomposeST(onlyPos);
			break;
		case pt_only:
			_pTedMan->decomposePT(onlyPos);
			break;
		default:
			throw(string("03045. No decomposition type selected"));
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Builds an initial DFG out of the TED*/
ReturnValue Tds::CmdListVars(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-verbose,v");
	enum switchcase { case_brief, case_help, case_verbose};
	void(TedVar::*task)(void)const = &TedVar::printInfo;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_verbose:
			task = &TedVar::printInfoDetailed;
			break;
		case case_brief:
			cout << BRIEF_LISTVARS << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_LISTVARS, "[-v]");
			cout << "  -v, --verbose"<< endl;
			cout << "    Print all information available in the variable"<< endl;
			cout << "SEE ALSO" << endl;
			cout << "  vars, set" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_TED_EXIST;
		cout << "List of TED variables in the Container(from top to bottom):" << endl;
		_pTedMan->foreach(task);
		if(&TedVar::printInfo==task)
			cout << endl;
		cout << "Primary Outputs" << endl;
		_pTedMan->listOutputs();
	} CATCH_ERRORS;
	return CMD_OK;
}

/** @brief Set environment and operational variables*/
ReturnValue Tds::CmdSet(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-bitwidth,b:|-range,r:|-error,e:");
	enum switchcase { case_brief, case_help, case_bitwidth, case_range, case_error };
	enum bitwidthtype { isUnknown, isInteger, isFixedpoint };
	bitwidthtype btype = isUnknown;
	vector <string> piBitwidths;
	vector <string> piRanges;
	vector <string> poError;
	bool annotateRange = false;
	bool annotateBitwidth = false;
	bool annotateError = false;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_error:
			poError.push_back(opt.getOptionArgument());
			while(opt.current()&& opt.current()[0] != '-') {
				poError.push_back(opt.current());
				opt.next();
			}
			annotateError = true;
			break;
		case case_range:
			//WARNING: we are inserting const string& objects to piRanges
			//         that are created from argv. There are no guarantees
			//         on how long will this strings will exists, and they
			//         might be subject to abrupt deallocation
			piRanges.push_back(opt.getOptionArgument());
			while(opt.current()&& opt.current()[0] != '-') {
				piRanges.push_back(opt.current());
				opt.next();
			}
			annotateRange = true;
			break;
		case case_bitwidth:
			if(!strcmp(opt.getOptionArgument(), "integer")|| !strcmp(opt.getOptionArgument(), "int")) {
				btype = isInteger;
			} else if(!strcmp(opt.getOptionArgument(), "fixedpoint")|| !strcmp(opt.getOptionArgument(), "fxp")) {
				btype = isFixedpoint;
			} else {
				cerr << "INFO: Define the bitwidth type right after --bitwidth, \"integer\" or \"fixedpoint\"." << endl;
				cerr << "      One can also use the abbreviated form \"int\" or \"fxp\"" << endl;
				return CMD_INFO;
			}
			while(opt.current()&& opt.current()[0] != '-') {
				piBitwidths.push_back(opt.current());
				opt.next();
			}
			annotateBitwidth = true;
			break;
		case case_brief:
			cout << BRIEF_SET << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SET, "[bitwidth] [range] [maximal error]");
			cout << "  [bitwidth]" << endl;
			cout << "  -b, --bitwidth integer|int|fixedpoint|fxp [var1:bitwidth1 var2:bitwidth2 ...]" << endl;
			cout << "    Set the initial bitwidth of the variables in the TED." << endl;
			cout << "    All other variables not specified in the list, take a" << endl;
			cout << "    default bitwidth depending on its type:" << endl;
			cout << "* integer(int)-> " << Environment::getStr(Environment::BITWIDTH_INTEGER)<< endl;
			cout << "* fixedpoint(fxp)-> " << Environment::getStr(Environment::BITWIDTH_FIXEDPOINT)<< endl;
			cout << "  [range]" << endl;
			cout << "  -r, --range var1:interval1 [var2:interval2 ...]" << endl;
			cout << "    Where interval has the syntax: [minval, maxval]" << endl;
			cout << "  [maximal error]" << endl;
			cout << "  -e, --error po1:maxerror1 [po2:maxerror2 ...]" << endl;
			cout << "    Where maxerror1 is the maximal error allowed at the primary output po1" << endl;
			cout << "EXAMPLE" << endl;
			cout << "  poly F1 = a-b+c+d" << endl;
			cout << "  poly F2 = (a+b)*(c+d)^2" << endl;
			cout << "  set -b fixedpoint a:4,16 c:2,10" << endl;
			cout << "  set -r d:[0.128, 1.123432] b:[-5.3223, 321.32e-3] c:[0, 1]" << endl;
			cout << "  set -e F1:1.324 F2:0.983" << endl;
			cout << "SEE ALSO" << endl;
			cout << "  listvars, compute" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_TED_EXIST;
		if(!annotateBitwidth && !annotateRange && !annotateError) {
			cerr << "INFO: set requires an option." << endl;
			return CMD_INFO;
		}
		if(annotateBitwidth) {
			if(TedNode::getOne()->getBitwidth()) {
				delete TedNode::getOne()->getBitwidth();
				TedNode::getOne()->setBitwidth(NULL);
			}
			//set first default bitwidth
			switch(btype) {
			case isInteger:
				TedVarMan::instance().foreach(&TedVar::defaultBitwidthForInteger);
				TedNode::getOne()->setBitwidth(new Integer(0));
				break;
			case isFixedpoint:
				TedVarMan::instance().foreach(&TedVar::defaultBitwidthForFixedpoint);
				TedNode::getOne()->setBitwidth(new FixedPoint(0, 0));
				break;
			case isUnknown:
			default:
				throw(string("03019. No bitwith type has been selected."));
				break;
			}
			//TODO: there is a _bitw in TedNodeRoot that is leaking.
			_pTedMan->foreach(&TedNode::cleanBitwidth);
			for(int index = 0; index < piBitwidths.size(); index++) {
				size_t pos = piBitwidths[index].find_first_of(":");
				if(pos==string::npos) {
					throw(string("03020. Expecting \"variable:bitwidth\" instead of \"")+ piBitwidths[index]+string("\""));
				}
				string value = piBitwidths[index].substr(pos+1, string::npos);
				string varname = piBitwidths[index].substr(0, pos);
				TedVar* pvar = TedVarMan::instance().getIfExist(varname);
				if(!pvar) {
					throw(string("03021. Variable \"")+ varname + string("\" does not exist"));
				}
				Bitwidth* precRaw = pvar->getBitwidth();
				switch(btype) {
				case isInteger:
					{
						Integer* prec = dynamic_cast<Integer*>(precRaw);
						assert(prec);
						pair<bool, long> val = Util::atoi(value.c_str());
						if(val.first) {
							prec->set(val.second);
							//although the line below seems redundant, it ensures that if the variable is a TedVarGroup
							//the inserted bitwidth gets propagated to all its members
							pvar->setBitwidth(prec);
						} else {
							cerr << "INFO: Using default bitwidth assignment for variable \"" << varname << "\"." << endl;
							cerr << "      Its bitwith should be an unsigned integer. i.e " << varname << ":32" << endl;
						}
					}
					break;
				case isFixedpoint:
					{
						FixedPoint* prec = dynamic_cast<FixedPoint*>(precRaw);
						vector<string> splitValue = Util::split(value, ", ");
						bool through_info = false;
						if(2==splitValue.size()) {
							pair<bool, long> val_int = Util::atoi(splitValue[0].c_str());
							pair<bool, long> val_fra = Util::atoi(splitValue[1].c_str());
							if(val_int.first && val_fra.first) {
								prec->set(val_int.second, val_fra.second);
								//although the line below seems redundant, it ensures that if the variable is a TedVarGroup
								//the inserted bitwidth gets propagated to all its members
								pvar->setBitwidth(prec);
							} else {
								through_info = true;
							}
						} else {
							through_info = true;
						}
						if(through_info) {
							cerr << "INFO: Using default bitwidth assigment for variable \"" << varname << "\"." << endl;
							cerr << "      Its bitwidth should be two unsigned integers, separeted by a comma without space." << endl;
							cerr << "      i.e " << varname << ":4, 12" << endl;
						}
					}
				case isUnknown:
				default:
					assert(false);
					break;
				}
			}
		}
		if(annotateRange) {
			for(int index = 0; index < piRanges.size(); index++) {
				size_t pos = piRanges[index].find_first_of(":");
				if(pos==string::npos) {
					throw(string("03022. Expecting \"variable:interval\" instead of \"")+ piRanges[index]+string("\""));
				}
				string interval = piRanges[index].substr(pos+1, string::npos);
				string varname = piRanges[index].substr(0, pos);
				TedVar* pvar = TedVarMan::instance().getIfExist(varname);
				if(!pvar) {
					throw(string("03023. Variable \"")+ varname + string("\" does not exist"));
				}
				size_t comma = interval.find_first_of(", ");
				if(interval.find_first_of("[")!=0 || \
				   interval.find_first_of("]", 1)!= (interval.size()-1)|| \
				   comma==string::npos) {
					throw(string("03024. Interval expression is incorrect."));
				}
				string minval = interval.substr(1, comma);
				string maxval = interval.substr(comma+1, string::npos);
				//check is a good interval range
				double minvalue;
				double maxvalue;
				try {
					minvalue = atof(minval.c_str());
				} catch(...) {
					throw(string("03025. Incorrect format for the minimum value of \"")+varname+string("\""));
				}
				try {
					maxvalue = atof(maxval.c_str());
				} catch(...) {
					throw(string("03025. Incorrect format for the maximum value of \"")+varname+string("\""));
				}
				if(minvalue > maxvalue) {
					throw(string("03026. Interval format for \"")+varname+string("\" should be minvalue, maxvalue"));
				}
				pvar->setRange(interval);
			}
		}
		if(annotateError) {
			TedMan::PrimaryOutputs& pomap = _pTedMan->getPOs();
			for(int index = 0; index < poError.size(); index++) {
				size_t pos = poError[index].find_first_of(":");
				if(pos==string::npos) {
					throw(string("03027. Expecting \"PO:maximal_error\" instead of \"")+ poError[index]+string("\""));
				}
				string targetError = poError[index].substr(pos+1, string::npos);
				string poname = poError[index].substr(0, pos);
				TedMan::PrimaryOutputs::iterator itpo = pomap.find(poname);
				if(itpo==pomap.end()) {
					throw(string("03028. PO \"")+ poname + string("\" does not exist"));
				}
				try {
					double error = atof(targetError.c_str());
					itpo->second.setError(error);
				} catch(...) {
					throw(string("03029. Incorrect format for the target maximal error of PO \"")+poname+string("\""));
				}
			}
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdPrintEnv(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_PRINTENV << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_PRINTENV, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  setenv, save, load" << endl;
			return CMD_HELP;
		}
	}
	try {
		Environment::print(std::cout);
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdSetEnv(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_SETENV << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SETENV, "variable = condition");
			cout << "SEE ALSO" << endl;
			cout << "  load, printenv, save" << endl;
			return CMD_HELP;
		}
	}
	try {
		string envLine;
		while(opt.current()) {
			envLine += opt.current();
			opt.next();
		}
		if(envLine.empty()) {
			cerr << "Info: no environment variable given." << endl;
			return CMD_INFO;
		}
		vector<string> envToken = Util::split(envLine, "=");
		if(envToken.size() ==2) {
			Environment::set(envToken[0], envToken[1]);
			if(_pDfgMan && envToken[0].find("delay")!= string::npos) {
				DfgNode::setEnvOpDelays();
				_pDfgMan->updateDelayAndNumRefs();
				_pDfgMan->updateRequiredTime();
			}
		} else {
			cerr << "Info: use the format, variable1=condition1." << endl;
			return CMD_INFO;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdLoadEnv(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-default,d");
	enum switchcase { case_brief, case_help, case_default};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_default:
			Environment::reset();
			return CMD_OK;
		case case_brief:
			cout << BRIEF_LOADENV << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_LOADENV, "[filename]");
			cout << "SEE ALSO" << endl;
			cout << "  printenv, save, setenv" << endl;
			return CMD_HELP;
		}
	}
	try {
		string default_environment_file = "tds.env";
		if (opt.current()) {
			default_environment_file = opt.current();
		}
		Environment::load(default_environment_file.c_str());
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdSaveEnv(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_SAVEENV << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_SAVEENV, "[filename]");
			cout << "SEE ALSO" << endl;
			cout << "  load, printenv, setenv" << endl;
			return CMD_HELP;
		}
	}
	try {
		string default_environment_file = "tds.env";
		if (opt.current()) {
			default_environment_file = opt.current();
		}
		Environment::save(default_environment_file.c_str());
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdCompute(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-ted,t|-dfg,d|-bitwidth,b|-gappa,g|-snr");
	enum switchcase { case_brief, case_help, case_ted, case_dfg, case_bitwidth, case_gappa, case_snr };
	enum switchgraph { graph_ted, graph_dfg, graph_nl, graph_unknown };
	enum computetask { compute_bitwidth, compute_gappa, compute_snr, compute_nothing };
	switchgraph graph = graph_ted;
	computetask task = compute_bitwidth;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_snr:
			task = compute_snr;
			break;
		case case_bitwidth:
			task = compute_bitwidth;
			break;
		case case_gappa:
#if !defined(_WIN32)
			task = compute_gappa;
#else
			cout << "Info: switch not available in WINDOWS, reverts to default behavior" << endl;
#endif
			break;
		case case_brief:
			cout << BRIEF_COMPUTE << endl;
			return CMD_BRIEF;
		case case_ted:
			graph = graph_ted;
			break;
		case case_dfg:
			graph = graph_dfg;
			break;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_COMPUTE, "[-t -b --snr] | [-d -b -g]");
			cout << "  -b, --bitwidth" << endl;
			cout << "    Compute the bitwidth at each point in the graph [DEFAULT BEHAVIOR]." << endl;
			cout << "  -g, --gappa" << endl;
#if !defined(_WIN32)
			cout << "    Compute a bound on the maximal error." << endl;
#else
			cout << "    Switch not available in WINDOWS, reverts to default behavior." << endl;
#endif
			cout << "  --snr" << endl;
			cout << "    Compute the Signal to Noise Ratio of the architecture." << endl;
			cout << "  -t, --ted" << endl;
			cout << "    In the TED graph [DEFAULT BEHAVIOR]." << endl;
			cout << "  -d, --dfg" << endl;
			cout << "    In the DFG graph." << endl;
			cout << "SEE ALSO" << endl;
			cout << "  optimize" << endl;
			return CMD_HELP;
		}
	}
	try {
		ENSURE_TED_EXIST;
		switch(graph) {
		case graph_ted:
			_pTedMan->computeBitwidth();
			break;
		case graph_dfg:
			if(!_pDfgMan && !_pTedMan->isEmpty()) {
				Convert from(_pTedMan);
				_pDfgMan = from.ted2dfgFactor();
				//_pDfgMan = new DfgMan(_pTedMan);
			}
			if(_pDfgMan) {
				//_pDfgMan->computeBitwidth();
				if(compute_gappa==task) {
					MinGappaBound gappa;
					FixCost* cost = gappa.compute_fix_cost(*_pDfgMan);
					cout << "The maximal error associated with this DFG is " << cost->bitwidth << endl;
					delete cost;
				} else if(compute_snr==task) {
					_pDfgMan->computeSNR();
					_pDfgMan->getInfoSNR();
				} else {
					_pDfgMan->computeBitwidth();
				}
			} else {
				cerr << "Info: no DFG found, and none could be inferred from TED" << endl;
			}
			break;
		default:
			break;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdOptimize(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-sen|-topo|-cone|r,-report");
	enum switchcase { case_brief, case_help, case_se, case_topo, case_cone, case_report};
	switchcase optype = case_se;
	bool report = true;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_OPTIMIZE << endl;
			return CMD_BRIEF;
		case case_report:
			report = true;
			break;
		case case_se:
			report = true;
			optype = case_se;
			break;
		case case_topo:
			report = false; //default for topo
			optype = case_topo;
			break;
		case case_cone:
			report = false; //default for topo
			optype = case_cone;
			break;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_OPTIMIZE, CMD_NO_OPTIONS);
			cout << "SEE ALSO" << endl;
			cout << "  compute" << endl;
			return CMD_HELP;
		}
	}
#if !defined(_WIN32)
	try {
		ENSURE_TED_EXIST;
		//start with the polynomial
		TedMan::PrimaryOutputs& po = _pTedMan->getPOs();
		double maxerror = 0.0;
		for(TedMan::PrimaryOutputs::iterator it = po.begin(); it != po.end(); it++) {
			double error = it->second.getError();
			maxerror = (error > maxerror) ? error : maxerror;
		}
		//_pTedMan->computeBitwidth();

		//find a quick minimum architecture

		/* MinBitwidth bitwidth;
		   TedOrderSwap orderst(_pTedMan);
		   TedMan* pTedNew = _pTedMan->siftAll(&orderst, &bitwidth);
		   if(_pTedMan!=pTedNew)
		   delete _pTedMan;
		   _pTedMan=pTedNew;*/
		if(!_pDfgMan) {
			Convert from(_pTedMan);
			_pDfgMan = from.ted2dfgFactor();
		}
		_pDfgMan->computeBitwidth();
		MinGappaBound gappa;
		FixCost* cost = gappa.compute_fix_cost(*_pDfgMan);
		if(cost->bitwidth > maxerror) {
			cerr << "Info: the architecture error(" << cost->bitwidth << ")is greater than the error allowed(" << maxerror << ")" << endl;
			cerr << "      there is no room for optimization, do you want to do a" << endl;
			cerr << "      space exploration instead?. If so, run explore." << endl;
			return CMD_INFO;
		}
		delete cost;
		DfgBitReduction red(_pDfgMan, maxerror, report);
		switch(optype) {
		case case_cone: red.opPriorityCone(); break;
		case case_topo: red.opTopological(); break;
		case case_se: //fall through
		default: red.opSensitivity(); break;
		}
	} CATCH_ERRORS;
#else
	cout << "Info: Command not available in WINDOWS, refucing to execute." << endl;
#endif
	return CMD_OK;
}

ReturnValue Tds::CmdExplore(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_EXPLORE << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_EXPLORE, CMD_NO_OPTIONS);
			return CMD_HELP;
		}
	}
	try {
		cerr << "Info: removed temporarily, it requires modifications to handle registers" << endl;
		return CMD_INFO;
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdDfgRelink(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h");
	enum switchcase { case_brief, case_help};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_DFGRELINK << endl;
			return CMD_BRIEF;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_DFGRELINK, CMD_NO_OPTIONS);
			return CMD_HELP;
		}
	}
	try {
		ENSURE_DFG_EXIST;
		_pDfgMan->relink_pseudo_factors();
		_pDfgMan->strash();
	} CATCH_ERRORS;
	return CMD_OK;
}

ReturnValue Tds::CmdRetime(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-up,u|-down,d|-forward,f|-backward,b|a,-all|-force");
	enum switchcase { case_brief, case_help, case_up, case_down, case_forward, case_backward, case_all, case_force };
	bool allof = false;
	bool for_or_back_ward = true;
	bool force =false;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_brief:
			cout << BRIEF_RETIME << endl;
			return CMD_BRIEF;
		case case_forward:
		case case_up:
			for_or_back_ward = true;
			break;
		case case_backward:
		case case_down:
			for_or_back_ward = false;
			break;
		case case_force:
			force = true;
			break;
		case case_all:
			allof = true;
			break;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_RETIME, "[-a] [-u -f] [-d -b --force] {variable} {max number}");
			cout << "  -u, --up, -f, --forward" << endl;
			cout << "    Moves registers in a variable toward the root of the TED [DEFAULT BEHAVIOR]." << endl;
			cout << "  -d, --down, -b, --backward" << endl;
			cout << "    Moves registers in a variable toward the node ONE." << endl;
			cout << "  -a, --all" << endl;
			cout << "    Instead of retiming the variable name, it retimes all variables which derive from the same" << endl;
			cout << "    un-retimed base variable." << endl;
			cout << "  --force" << endl;
			cout << "    when retiming down a variable referenced by their parents through multiple retimed edges" << endl;
			cout << "    it might be necessary to use this argument to retime all the way to the primary inputs." << endl;
			cout << "  {variable}" <<endl;
			cout << "    When defined only the specific variable is retimed, otherwise all variables are retime." << endl;
			cout << "  {max number}" << endl;
			cout << "    Defines the maximum amount of registers to retime (disregarded when --force is specified)." << endl;
			//			cout << "SEE ALSO" << endl;
			//			cout << "  optimize" << endl;
			return CMD_HELP;
		}
	}
	TedRegister uptoR = INT_MAX;
	char* varname = NULL;
	if(opt.current()) {
		char* option = opt.current();
		pair<bool, unsigned long> tt = Util::atoi(option);
		if(tt.first == true) {
			uptoR = tt.second;
		} else {
			varname = option;
			opt.next();
			if(opt.current()) {
				option = opt.current();
				tt = Util::atoi(option);
				if(tt.first == true) {
					uptoR = tt.second;
				}
			}
		}
	}
	try {
		ENSURE_TED_EXIST;
		if(!varname) {
			_pTedMan->retime(for_or_back_ward,uptoR);
		} else {
			if (force && !for_or_back_ward) {
				_pTedMan->retime_down_forced(varname);
			} else {
				_pTedMan->retime(varname,allof,for_or_back_ward,uptoR);
			}
		}
	} CATCH_ERRORS;
	return CMD_OK;
}


ReturnValue Tds::CmdQuartus(unsigned int argc, char** argv) {
	int c;
	ParseOption opt(argc,argv,"-brief|-help,h|-report_only,ro|-project,p|-compile,c");
	enum switchcase { case_brief, case_help, case_report_only,case_project,case_compile};
	bool report_only = false;
	bool create_project = false;
	bool compile_project = false;
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_report_only:
			report_only = true;
			break;
		case case_brief:
			cout << BRIEF_QUARTUS << endl;
			return CMD_BRIEF;
		case case_project:
			create_project = true;
			break;
		case case_compile:
			compile_project = true;
			break;
		case case_help:
		default:
			printHelpHeader(opt.command(), BRIEF_QUARTUS, "[-p -c -ro]");
			cout << "  -p, --project" << endl;
			cout << "    Creates a Tcl script for a Quartus project for the current netlist" << endl;
			cout << "  -c, --compile" << endl;
			cout << "    Creates a Tcl script to compile the Quartus project for the current netlist" << endl;
			cout << "  -ro, --report_only" << endl;
			cout << "    Loads and prints an existing report for the current netlist" << endl;
			return CMD_HELP;
		}
	}
	try {
		if( getCurrentNetlist()&& !getCurrentNetlist()->isExtracTed()) {
			Quartus script(getCurrentNetlist()->getName(),"");
			if (create_project) {
				script.generate_quartus_script(Quartus::ACTION_CREATE);
				script.execute(Quartus::ACTION_CREATE);
				report_only = false;
			}
			if (compile_project) {
				script.generate_quartus_script(Quartus::ACTION_COMPILE);
				report_only = script.execute(Quartus::ACTION_COMPILE);
				if (report_only) {
					cout << "Info: Report file saved in \"" << script.report_file() << "\"" << endl;
				} else {
					cout << "Info: Failed to generate a report" << endl;
				}
			}
			if (report_only) {
				ifstream file(script.report_file().c_str());
				if (file.is_open()) {
					cout << "Info: Loading report file \"" << script.report_file() << "\"" << endl;
					cout << endl;
					string line;
					while(Util::readline(file, line, "#")!= 0) {
						cout << line << endl;
					}
					file.close();
				} else {
					cout << "Info: Report file " << script.report_file() << " does not exist" << endl;
				}
			}
		} else {
			cout << "Info: Quartus command requires a valid and compiled CDFG netlist" << endl;
		}
	} CATCH_ERRORS;
	return CMD_OK;
}

}
