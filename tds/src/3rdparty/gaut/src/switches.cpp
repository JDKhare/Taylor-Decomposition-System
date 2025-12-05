/*------------------------------------------------------------------------------
Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

Cette licence est un accord légal conclu entre vous et l'Université de
Bretagne Sud (UBS) concernant l'outil GAUT2.

Cette licence est une licence CeCILL-B dont deux exemplaires (en langue
française et anglaise) sont joints aux codes sources. Plus d'informations
concernant cette licence sont accessibles à http://www.cecill.info.

AUCUNE GARANTIE n'est associée à cette license !

L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
de GAUT2 et le distribue en l'état à des fins de coopération scientifique
seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
de protection de leurs données qu'il jugeront nécessaires.
------------------------------------------------------------------------------
2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

This license is a legal agreement between you and the University of
South Britany (UBS) regarding the GAUT2 software tool.

This license is a CeCILL-B license. Two exemplaries (one in French and one in
English) are provided with the source codes. More informations about this
license are available at http://www.cecill.info.

NO WARRANTY is provided with this license.

The University of South Britany does not provide any warranty about
the usage of GAUT2 and distributes it "as is" for scientific cooperation
purpose only. All users are greatly advised to provide all the necessary
protection issues to protect their data.
------------------------------------------------------------------------------
*/

//	File:		switches.cpp
//	Purpose:	command line parsing
//	Author:		Pierre Bomel, LESTER, UBS

#include <fstream>
#include <strstream>
//#include "init.h"
#include "os_tools.h"
#include "parser.h"
#include "switches.h"
#include "check.h"
using namespace std;

const string	Switches::GAUT_VAR						= "GAUT";	// environment variable name
const string	Switches::GAUT_NAME						= "gaut";	// gaut tool name
const string	Switches::VERSION						= "2.4.2 December 2009";	// gaut tool version

const string	Switches::BUILTINS_SW					= "-builtins";
const string	Switches::CADENCY_SW					= "-cadency";
const string	Switches::CDFG_SW						= "-cdfg";
/* Caaliph 27/06/2007 */
const string	Switches::CADENCY_PRIM_SW				= "-cadency_prim";//add by Caaliph
const bool  	Switches::CADENCY_PRIM_DEF				= false; //add by Caaliph
const int  	    Switches::CADENCY_PRIM_VAL				= -1; //add by Caaliph
////////
//
const string	Switches::CDFGs_SW						= "-cdfgs"; //add by Caaliph
const bool  	Switches::CDFGs_DEF						= false; //add by Caaliph
const string	Switches::CDFGs_FILE_DEF				= ""; //add by Caaliph
const string	Switches::N_ORDO_SW						= "-n_ordo"; //add by Caaliph
const bool  	Switches::N_ORDO_DEF					= false; //add by Caaliph
const string	Switches::N_ORDO_FILE_DEF				= ""; //add by Caaliph
const string	Switches::SPACT_SW					= "-spact_mr"; //add by Caaliph
const bool  	Switches::SPACT_DEF					= false; //add by Caaliph
const string	Switches::SPACT_FILE_DEF				= ""; //add by Caaliph

const string	Switches::MODE_SW						= "-mode"; //add by Caaliph
const bool  	Switches::MODE_DEF						= false; //add by Caaliph
const string	Switches::MODE_FILE_DEF					= ""; //add by Caaliph
const long	    Switches::MODE_VAL					    = 0; //add by Caaliph
const string	Switches::BIND_SW						= "-binding_case"; //add by Caaliph
const bool  	Switches::BIND_DEF						= false; //add by Caaliph
const string	Switches::BIND_FILE_DEF					= ""; //add by Caaliph
const string	Switches::BIND_CASE					    = "mwbm_cycles"; //add by Caaliph
//Caaliph: 14/12/09 : Switch pour le flot multimode
const string	Switches::MM_SW						    = "-multimode_flow"; //add by Caaliph
const bool  	Switches::MM_DEF						= false; //add by Caaliph
const string	Switches::MM_FILE_DEF					= ""; //add by Caaliph
const string	Switches::MM_CASE					    = "union"; //add by Caaliph
// Fin Caaliph
const string	Switches::DUMP_STAR_CDFG_SW				= "-dumpSTAR_CDFG";
const bool		Switches::DUMP_STAR_CDFG_DEF			= false;
const string	Switches::DUMP_STAR_CDFG_FILE_DEF		= "";
//
/* End Caaliph */
const string	Switches::CLOCK_SW						= "-clock";
const string	Switches::DUMP_BEST_SELECTION_SW		= "-dumpSel";
const string	Switches::DUMP_CDFG_SW					= "-dumpCDFG";
const string	Switches::BITWIDTH_AWARE_SW				= "-bitwidth_aware";
/* GL 16/04/2007 :	Add a switch to specify the resizing
					mode when bitwidthAware is used*/
const string	Switches::RESIZING_MODE_SW				= "-resizingMode";
/* Fin GL */

/* GL 07/05/2007 :	Add a switch to specify the scheduling / binding and clustering
					mode when bitwidthAware is used*/
const string	Switches::CLUSTERING_MODE_SW			= "-clusteringMode";
const string	Switches::SCHEDULING_MODE_SW			= "-schedulingMode";
const string	Switches::BINDING_MODE_SW				= "-bindingMode";
/* Fin GL */

/* DH 27/05/2009 */
const string	Switches::REGISTERALLOCATION_MODE_SW			= "-registerAllocation";
/* FIN DH 27/05/2009 */

/* GL 25/10/07 : pattern (add lines)*/
const string	Switches::PATTERN_SW					= "-pattern";
const string	Switches::ALLOW_CHAINING_SW				= "-allowChaining";
/* Fin GL */
/* GL 20/12/08 : Complex operators */
const string	Switches::H_SCHEDULE_SW					= "-hSchedule";
/* Fin GL */
const string	Switches::DUMP_DEBUG2C_SW				= "-dumpDEBUG2C";
const string	Switches::DUMP_LIB_SW					= "-dumpLib";
const string	Switches::DUMP_SELECTIONS_SW			= "-dumpSels";
const string	Switches::DYNAMIC_ALLOCATION_SW			= "-dynamicAllocation";
const string	Switches::FORCE_OVERWRITE_SW			= "-f";
const string	Switches::IOC_SW						= "-io";
const string	Switches::LIB_SW						= "-lib";
const string	Switches::MAPC_SW						= "-map";
const string	Switches::MEM_SW						= "-mem";
const string	Switches::MEMFILE_SW					= "-memfile";
const string	Switches::SILENT_SW						= "-s";
const string	Switches::SELECTION_SW					= "-selection";
const string	Switches::TIMEUNIT_SW					= "-tu";
const string	Switches::USERLIB_SW					= "-userlib";
const string	Switches::VERBOSE_SW					= "-v";
const string	Switches::VHDL_SW						= "-vhdl";
const string	Switches::VHDL_PREFIX_SW				= "-vhdl_prefix";
const string	Switches::VHDL_TYPE_SW					= "-vhdl_type"; // EJ 09/2007
/* Caaliph 27/06/2007 */
const string	Switches::MEMFILES_SW					= "-memfiles";
const string	Switches::VHDLS_SW						= "-vhdls";//add by Caaliph
const string	Switches::VHDL_PREFIXS_SW				= "-vhdl_prefixs";//add by Caaliph
/* End Caaliph */
const string	Switches::NVHDL_SW						= "-nvhdl";
const string	Switches::NSOCLIB_SW						= "-nsoclib";
const string	Switches::NGANTT_SW						= "-ngantt";
const string	Switches::NMEM_SW						= "-nmem";
const string	Switches::SCH_SGY_SW					= "-scheduling";
const string	Switches::OP_OPTIMIZATION_SW			= "-operator_optimization";

/* KT 15/12/2007 :	Add switchs,*/
const string		Switches::COSTING_SW				="-costing";
const string		Switches::N1_SW						="-n1";
const string		Switches::N2_SW						="-n2";
const string		Switches::N3_SW						="-n3";
const string		Switches::N4_SW						="-n4";
const string		Switches::VNS_SW					="-vns";
/* end KT */

/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling */
const string	Switches::GLOBAL_TH_LB_SW				= "-global_th_lb";
const string	Switches::GLOBAL_PR_UB_SW				= "-global_pr_ub";
const string	Switches::GLOBAL_PR_LB_SW				= "-global_pr_lb";
const string	Switches::DISTRIBUTED_TH_SW				= "-distributed_th_lb";
const string	Switches::DISTRIBUTED_REUSE_TH_SW		= "-distributed_reuse_th";
const string	Switches::DISTRIBUTED_REUSE_PR_SW		= "-distributed_reuse_pr";
const string	Switches::DISTRIBUTED_REUSE_PR_UB_SW	= "-distributed_reuse_pr_ub";

/* switchs specifying allocation mode */
const string	Switches::GLOBAL_ALLOCATION_SW			= "-global_allocation";
const string	Switches::DISTIBUTED_ALLOCATION_SW		= "-distributed_allocation";
const string	Switches::PRACTICAL_SW					= "-practical";
const string	Switches::THEORETICAL_SW				= "-theoretical";
/* switchs specifying scheduling mode */
const string	Switches::WITHREUSE_SW			        = "-withreuse";
const string	Switches::LOWER_BOUND_SW				= "-lower_bound";
const string	Switches::UPPER_BOUND_SW				= "-upper_bound";
/* end KT */

const string	Switches::MANUAL_ALLOCATION_SW			= "-manualAllocation";
const string	Switches::USE_IHM_SW					= "-IHM";


const string	Switches::BUILTINS_DEF					= "CDFG_functions.txt";
const int		Switches::CADENCY_DEF					= -1;	// means "not specified"
const int		Switches::CLOCK_DEF						= 10;
const bool		Switches::DUMP_BEST_SELECTION_DEF		= false;
const bool		Switches::DUMP_CDFG_DEF					= false;

const bool		Switches::BITWIDTH_AWARE_DEF				= false;

/* GL 16/04/2007 :	Add a switch to specify the resizing
					mode when bitwidthAware is used*/
const int		Switches::RESIZING_MODE_DEF				= 2; // means nothing to do
/* Fin GL */
/* GL 07/05/2007 :	Add a switch to specify the scheduling
					mode when bitwidthAware is used*/
const int		Switches::CLUSTERING_MODE_DEF			= 0; // means no changes on algo
const int		Switches::SCHEDULING_MODE_DEF			= 0; // means no changes on algo
const int		Switches::BINDING_MODE_DEF				= 0; // means no changes on algo
/* Fin GL */
const int		Switches::REGISTERALLOCATION_MODE_DEF			= 0; // means mwbm algo

/* GL 25/09/07 :  (new generation strategy)*/
const int		Switches::GENERATION_MODE_DEF			= 0;
/* Fin GL */

/* GL 25/10/07 : pattern (add lines)*/
const string	Switches::PATTERN_DEF					= "chaining.pat";
const bool		Switches::ALLOW_CHAINING_DEF			= false;
/* Fin GL */

/* GL 20/12/08 : Complex operators */
const string	Switches::H_SCHEDULE_DEF				= "coarse_grain";
/* Fin GL */

const bool		Switches::DUMP_DEBUG2C_DEF				= false;
const bool		Switches::DUMP_LIB_DEF					= false;
const string	Switches::DUMP_LIB_FILE_DEF				= "";
const bool		Switches::DUMP_SELECTIONS_DEF			= false;
const string	Switches::DUMP_BEST_SELECTION_FILE_DEF	= "";
const string	Switches::DUMP_CDFG_FILE_DEF			= "";
const string	Switches::DUMP_SELECTIONS_FILE_DEF		= "";
const bool		Switches::DYNAMIC_ALLOCATION_DEF		= false;
const bool		Switches::FORCE_OVERWRITE_DEF			= false;
const bool		Switches::IOC_DEF						= false;
const bool		Switches::IOC_FILE_DEF					= false;
const string	Switches::IOC_FILE_NAME_DEF				= "";
const string	Switches::LIB_DEF						= "notech_operators.txt";
const bool		Switches::MAPC_DEF						= false;
const bool		Switches::MAPC_FILE_DEF					= false;
const string	Switches::MAPC_FILE_NAME_DEF			= "";
const int		Switches::MEM_DEF						= 50;
const string    Switches::MEMFILE_DEF					= "ut.mem";
const bool		Switches::SELECTION_DEF					= false;
const string	Switches::SELECTION_FILE_DEF			= "";
const bool		Switches::SILENT_DEF					= false;
const string	Switches::TIMEUNIT_DEF					= "ns";
const bool		Switches::VERBOSE_DEF					= true;  //false
const string	Switches::VHDL_DEF						= "ut.vhd";
const string	Switches::VHDL_PREFIX_DEF				= "ut";
const string	Switches::VHDL_TYPE_DEF					= "fsm_sigs"; // EJ 09/2007
const bool		Switches::NVHDL_DEF						= false;
const bool		Switches::NSOCLIB_DEF						= false;
const bool		Switches::NGANTT_DEF					= false;
const bool		Switches::NMEM_DEF						= false;
const string    Switches::SCH_SGY_DEF					= "default";
const bool		Switches::OP_OPTIMIZATION_DEF			= false;
/* KT 15/12/2007 :	Add switchs,*/
const bool		Switches::COSTING_DEF					= true;
const bool		Switches::N1_DEF						= false;
const bool		Switches::VNS_DEF						= false;
const bool		Switches::N2_DEF						= false;
const bool		Switches::N3_DEF						= false;
const bool		Switches::N4_DEF						= false;

/* end KT */
/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling */
const bool		Switches::GLOBAL_TH_LB_DEF				= false;
const bool		Switches::GLOBAL_PR_UB_DEF				= false;
const bool		Switches::GLOBAL_PR_LB_DEF				= false;
const bool		Switches::DISTRIBUTED_TH_DEF			= true;
const bool		Switches::DISTRIBUTED_REUSE_TH_DEF		= false;
const bool		Switches::DISTRIBUTED_REUSE_PR_UB_DEF	= false;
const bool		Switches::DISTRIBUTED_REUSE_PR_DEF		= false;
/* switchs specifying allocation mode */
const bool		Switches::GLOBAL_ALLOCATION_DEF			= false;
const bool		Switches::DISTIBUTED_ALLOCATION_DEF		= false;
const bool		Switches::PRACTICAL_DEF					= false;
const bool		Switches::THEORETICAL_DEF				= false;	/* switchs specifying scheduling mode */
const bool		Switches::WITHREUSE_DEF			        = false;
const bool		Switches::LOWER_BOUND_DEF				= false;
const bool		Switches::UPPER_BOUND_DEF				= false;
/* end KT */


const bool		Switches::MANUAL_ALLOCATION_DEF			= false;
const bool		Switches::USE_IHM_DEF					= false;


// print switches' values
void Switches::info(int argc, char *argv[]) const
{
	cout << "Command line:" << endl;
	for (int i = 0; i < argc; i++) cout << argv[i] << " ";
	cout << endl;
};

// in case of error, print help and stop
void Switches::usage(int argc, char **argv) const
{
	cout << "usage: " << gaut_name() << " (version "   << gaut_version() << ")" << endl;
	cout << "mandatory switches" << endl;
	cout << "  " << CADENCY_SW  << " <int>        = input cadency (expressed in tu)" << endl;
	cout << "  " << CDFG_SW     << " <filename>      = CDFG file" << endl;
	cout << "optional switches" << endl;
	/* Caaliph 27/06/2007 */
	cout << "  " << MM_SW      << " <union, incremental> = multimode flows" << endl;
	cout << "  " << CDFGs_SW     << " <filename> <cadency> <mode> <filename> <cadency> <mode> = MULTIMODE" << endl;
	cout << "  " << N_ORDO_SW    << " <filename> <cadency> <mode> <filename> <cadency> <mode> = MULTIMODE NO ORDO" << endl;
	cout << "  " << SPACT_SW   << " <filename>  = primary CDFG file for multimode binding technique like SPACT-MR" << endl;
	cout << "  " << BIND_SW      << " <mwbm_cycles, mwbm_global, mlea_cycles, mlea_global> = Binding Techniques" << endl;
	cout << "  " << DUMP_STAR_CDFG_SW << "<filename> = output file for STAR usage" <<endl;
	/* End Caaliph */
	cout << "  " << BUILTINS_SW << " <filename>  = GAUT builtin functions file" << endl;
	cout << "                          default value = " << BUILTINS_DEF << endl;
	cout << "  " << CLOCK_SW    << " <int>          = system clock period (expressed in tu)" << endl;
	cout << "                          default value = " << CLOCK_DEF << " tu" << endl;
	cout << "  " << DYNAMIC_ALLOCATION_SW   << "    = dynamic allocation of operators at each stage" << endl;
	cout << "  " << MANUAL_ALLOCATION_SW << "     = manual allocation of operators at each stage" << endl;
	cout << "  " << FORCE_OVERWRITE_SW << "                    = force overwrite of output files" << endl;
	cout << "                          default value is \"doesn't overwrite output files\"" << endl;
	cout << "  " << IOC_SW  << " [<filename>]      = process IO constraints" << endl;
	cout << "  " << LIB_SW  << " <filename>       = target library file" << endl;
	cout << "                          default value = " << LIB_DEF << endl;
	cout << "  " << MAPC_SW << " [<filename>]     = process memory mapping constraints" << endl;
	cout << "  " << MEM_SW      << " <int>            = memory access time (expressed in tu)" << endl;
	cout << "                          default value = " << MEM_DEF << " tu" << endl;
	cout << "  " << MEMFILE_SW << " <string>     = .mem file name" << endl;
	cout << "                          default value = <CDFG file prefix>.mem" << endl;
	cout << "  " << TIMEUNIT_SW << " <timeunit>        = time unit, unit is one of (fs, ps, ns, us, ms, s)" << endl;
	cout << "                          default value = " << TIMEUNIT_DEF << endl;
	cout << "  " << USERLIB_SW  << " <filename>   = user defined functions and operators file" << endl;
	cout << "  " << VHDL_SW << " <filename>      = generate VHDL into file" << endl;
	cout << "                          default value = <CDFG file prefix>.vhd" << endl;
	cout << "  " << VHDL_PREFIX_SW << " <string> = VHDL entities prefix" << endl;
	cout << "                          default value = " << VHDL_PREFIX_DEF << endl;
	cout << "  " << VHDL_TYPE_SW << " <string> = VHDL type" << endl;
	cout << "                          default value = " << VHDL_TYPE_DEF << endl;
	cout << "  " << SCH_SGY_SW << " <string>  = scheduling strategy, values are one of (default, force_no_pipeline, force_no_mobility)" << endl;
	cout << "                          default value = " << SCH_SGY_DEF << endl;
	cout << "  " << OP_OPTIMIZATION_SW << "= optimize for scheduling operator(s) allocation" << endl;
	/* KT 15/12/2007 :	Add switchs,*/
	cout << "  " << COSTING_SW			<<	"=	costing"<< endl;
	cout << "  " << N1_SW		<<	"=	local search (registers neighbouhood)"<< endl;
	cout << "  " << N2_SW		<<	"=	local search (operators neighbouhood)"<< endl;
	cout << "  " << N3_SW		<<	"=	local search (third neighbouhood)"<< endl;
	cout << "  " << N4_SW		<<	"=	local search (fourth neighbouhood)"<< endl;
	cout << "  " << VNS_SW		<<	"=	vns"<< endl;
	/* end KT */

	/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling */
	cout << "  " << GLOBAL_TH_LB_SW			<< "=  theoretical and global allocation (lower bound) + scheduling with operators reuse  "<< endl;
	cout << "  " << GLOBAL_PR_UB_SW			<< "=  practical and global allocation (upper bound) + scheduling with operators reuse"<< endl;
	cout << "  " << GLOBAL_PR_LB_SW			<< "= practical and global allocation (lower bound) + scheduling with operators reuse"<< endl;
	cout << "  " << DISTRIBUTED_TH_SW	<< "=  theoretical and distributed allocation (lower bound) + scheduling without operators reuse "<< endl;
	cout << "  " << DISTRIBUTED_REUSE_TH_SW	<< "=  theoretical and distributed allocation (dynamic allocation) + scheduling with operators reuse "<< endl;
	cout << "  " << DISTRIBUTED_REUSE_PR_UB_SW  << "= practical and distributed allocation (upper bound) + scheduling with operators reuse"<< endl;
	cout << "  " << DISTRIBUTED_REUSE_PR_SW  << "= practical and distributed allocation (dynamic allocation) + scheduling with operators reuse"<< endl;
	/* switchs specifying allocation mode */
	cout << "  " << GLOBAL_ALLOCATION_SW		<< "= a global allocation is done"<< endl;
	cout << "  " << DISTIBUTED_ALLOCATION_SW    << "= a distributed allocation is done"<< endl;
	cout << "  " << PRACTICAL_SW				<< "= a practical allocation is done"<< endl;
	cout << "  " << THEORETICAL_SW				<< "= a theoretical allocation is done"<< endl;
	/* switchs specifying scheduling mode */
	cout << "  " << WITHREUSE_SW			<< "= scheduling with operators reuse "<< endl;
	cout << "  " << LOWER_BOUND_SW			<< "= the result of allocation is considered as a lower bound "<< endl;
	cout << "  " << UPPER_BOUND_SW			<< "= the result of allocation is considered as an upper bound"<< endl;
	/* end KT */


	cout << "  " << BITWIDTH_AWARE_SW << "			= new flow mode which support data bitwidth multiple" << endl;
	/* GL 16/04/2007 :	Add a switch to specify the resizing
						mode when bitwidthAware is used*/
	cout << "  " << RESIZING_MODE_SW << "<int>	= resizing mode when new Flow Mode is used" << endl;
	/* Fin GL */
	/* GL 07/05/2007 :	Add a switch to specify the scheduling
						mode when bitwidthAware is used*/
	cout << "  " << CLUSTERING_MODE_SW << "<int> = clustering mode when new Flow Mode is used" << endl;
	cout << "  " << SCHEDULING_MODE_SW << "<int> = scheduling mode when new Flow Mode is used" << endl;
	cout << "  " << REGISTERALLOCATION_MODE_SW << "<int> = register allocation mode" << endl;
	cout << "  " << BINDING_MODE_SW << "<int>	 = binding mode when new Flow Mode is used" << endl;
	/* Fin GL */
	/* GL 25/10/07 : pattern (add lines)*/
	cout << "  " << PATTERN_SW << "<filename>  = GAUT patterns file" << endl;
	cout << "                          default value = " << PATTERN_DEF << endl;
	cout << "  " << ALLOW_CHAINING_SW << " = Allow operation chaining according to pattern file" << endl;
	/* Fin GL */
	/* GL 20/12/08 : Complex operators */
	cout << "  " << H_SCHEDULE_SW << "<string> = Hierachical scheduling definition" << endl;
	/* Fin GL */
	cout << "trace and debug switches" << endl;
	cout << "  " << DUMP_CDFG_SW << " <filename>  = dump CDFG after HLS into file" << endl;
	cout << "  " << DUMP_DEBUG2C_SW << "          = dump DEBUG to C (cdfg2c, ordo2c, archi2c)" << endl;
	cout << "  " << DUMP_LIB_SW << " <filename>   = dump LIB into file" << endl;
	cout << "  " << DUMP_BEST_SELECTION_SW << " <filename>   = dump best selection into file after selection phase" << endl;
	cout << "  " << DUMP_SELECTIONS_SW << " <filename>  = dump all selections into file after selection phase" << endl;
	cout << "  " << SELECTION_SW   << " <filename> = input selection(s) file" << endl;
	cout << "  " << SILENT_SW   << "                    = silent mode" << endl;
	cout << "  " << VERBOSE_SW   << "                    = verbose mode" << endl;
	cout << "  " << NVHDL_SW   << "                    = not generate .vhdl file" << endl;
	cout << "  " << NSOCLIB_SW   << "                    = not generate .soclib file" << endl;
	cout << "  " << NGANTT_SW   << "                    = not generate .gantt files" << endl;
	cout << "  " << NMEM_SW   << "                    = not generate .mem file" << endl;
	exit(1);
}

// check a property of multiplicity between two values
void Switches::check_is_multiple(long val, string val_str, long ref, string ref_str) const
{
	long res = 0;
	if (ref>0) res = val % ref;
	if (res || ref <= 0)
	{
		cerr << val_str << " (" << val
		<< ") is not a multiple of "
		<< ref_str << "(" << ref << ")"
		<< endl;
		exit(1);
	}
}

// Seach for files wity a specific strategy
//	1/ search "here"
//	2/ search under directory $GAUT
//	else not found
void Switches::findCheck(string & file_name) const
{
	if (Tools::fileExists(file_name)) return;	// found here
	string gaut_str(gaut_var_value());
	string new_file_name = gaut_str+"\\"+file_name;
	if (!Tools::fileExists(new_file_name))
	{
		cerr << "Error: unable to find " << file_name << " file" << endl;
		exit(1);
	}
	file_name = new_file_name;
}

// interpret command line, check there are no incoherences
Switches::Switches(int argc, char *argv[])
		: // default values for switches are here !
		_time_unit(TIMEUNIT_DEF),
		_system_clock_period(CLOCK_DEF),
		_cadency(CADENCY_DEF),
		_silent_mode(SILENT_DEF),
		_memory_access_time(MEM_DEF),
		_lib_file_name(LIB_DEF),
		_cdfg_file_name(""),
		/* Caaliph 27/06/2007 */
		_cadency_prim(CADENCY_PRIM_DEF),
		_cadency_prim_val(CADENCY_PRIM_VAL),
		_cdfgs(CDFGs_DEF), //add by Caaliph
		_n_ordo(N_ORDO_DEF), //add by Caaliph
		_spact(SPACT_DEF), //add by Caaliph
		_spact_file_name(SPACT_FILE_DEF), //add by Caaliph
		_mode(MODE_DEF), //add by Caaliph
		_mode_number(MODE_FILE_DEF), //add by Caaliph
		_mode_val(MODE_VAL), //add by Caaliph
		_binding(BIND_DEF), //add by Caaliph
		_binding_number(BIND_FILE_DEF), //add by Caaliph
		_binding_case(BIND_CASE), //add by Caaliph
		// Multimode flow
		_multimode(MM_DEF), //add by Caaliph
		_multimode_number(MM_FILE_DEF), //add by Caaliph
		_multimode_case(MM_CASE), //add by Caaliph
		// End MM flow
		_dumpSTAR_CDFG(DUMP_STAR_CDFG_DEF),
		_dumpSTAR_CDFGfile(DUMP_STAR_CDFG_FILE_DEF),
		/* End Caaliph */
		_userlib(""),
		_builtins(BUILTINS_DEF),
		_verbose(VERBOSE_DEF),
		_dumpCDFG(DUMP_CDFG_DEF),
		_dumpCDFGfile(DUMP_CDFG_FILE_DEF),
		_bitwidth_aware(BITWIDTH_AWARE_DEF),
		/* GL 16/04/2007 :	Add a switch to specify the resizing
							mode when bitwidthAware is used*/
		_resizingMode(RESIZING_MODE_DEF),
		/* Fin GL */
		/* GL 07/05/2007 :	Add a switch to specify the scheduling
							mode when bitwidthAware is used*/
		_clusteringMode(CLUSTERING_MODE_DEF),
		_schedulingMode(SCHEDULING_MODE_DEF),
		_bindingMode(BINDING_MODE_DEF),
		/* Fin GL */
		_registerAllocationMode(REGISTERALLOCATION_MODE_DEF),
		/* GL 25/10/07 : pattern (add lines)*/
		_pattern(PATTERN_DEF),
		_chaining(ALLOW_CHAINING_DEF),
		/* Fin GL */
		/* GL 20/12/08 : Complex operators */
		_hSchedule(H_SCHEDULE_DEF),
		/* Fin GL */
		_dumpDebug2c(DUMP_DEBUG2C_DEF),
		_dumpBestSelection(DUMP_SELECTIONS_DEF),
		_dumpBestSelectionFile(DUMP_SELECTIONS_FILE_DEF),
		_dumpSelections(DUMP_BEST_SELECTION_DEF),
		_dumpSelectionsFile(DUMP_BEST_SELECTION_FILE_DEF),
		_selection(SELECTION_DEF),
		_selectionFile(SELECTION_FILE_DEF),
		_dynamicAllocation(DYNAMIC_ALLOCATION_DEF),
		_io_constraints(IOC_DEF),
		_io_constraints_file(IOC_FILE_DEF),
		_io_constraints_file_name(IOC_FILE_NAME_DEF),
		_mapping_constraints(MAPC_DEF),
		_mapping_constraints_file(MAPC_FILE_DEF),
		_mapping_constraints_file_name(MAPC_FILE_NAME_DEF),
		_vhdl_file_name(""),
		_vhdl_prefix(VHDL_PREFIX_DEF),
		_vhdl_type(VHDL_TYPE_DEF), // EJ 09/2007
		_mem_file_name(""),
		_force_overwrite(FORCE_OVERWRITE_DEF),
		_dumpLib(DUMP_LIB_DEF),
		_dumpLibFile(DUMP_LIB_FILE_DEF),
		_genVHDL(NVHDL_DEF),
		_genSOCLIB(NSOCLIB_DEF),
		_genGANTT(NGANTT_DEF),
		_genMEM(NMEM_DEF),
		_scheduling_strategy(SCH_SGY_DEF),
		_operator_optimization(OP_OPTIMIZATION_DEF),

		/* KT 15/12/2007 :	Add switchs,*/
		_costing(COSTING_DEF),
		_n1(N1_DEF),
		_n2(N2_DEF),
		_n3(N3_DEF),
		_n4(N4_DEF),
		_vns(VNS_DEF),
		/* end KT */

		/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling */
		_global_th_lb(GLOBAL_TH_LB_DEF),
		_global_pr_ub (GLOBAL_PR_UB_DEF),
		_global_pr_lb(GLOBAL_PR_LB_DEF),
		_distributed_th(DISTRIBUTED_TH_DEF),
		_distributed_reuse_th(DISTRIBUTED_REUSE_TH_DEF),
		_distributed_reuse_pr_ub(DISTRIBUTED_REUSE_PR_UB_DEF),
		_distributed_reuse_pr(DISTRIBUTED_REUSE_PR_DEF),
		/* switchs specifying allocation mode */
		_global_allocation(GLOBAL_ALLOCATION_DEF),
		_distributed_allocation(DISTIBUTED_ALLOCATION_DEF),
		_practical(PRACTICAL_DEF),
		_theoretical(THEORETICAL_DEF),
		/* switchs specifying scheduling mode */
		_withreuse(WITHREUSE_DEF),
		_lower_bound(LOWER_BOUND_DEF),
		_upper_bound(UPPER_BOUND_DEF),
		/* end KT */

		_manual_allocation(MANUAL_ALLOCATION_DEF),
		_use_IHM(USE_IHM_DEF)
{
	int units;
	int mode;// GL 16/04/2007

	string file_name;

	// get user switches
	for (int i = 1; i < argc; i++)
	{
		string word = argv[i];
		if (word == BUILTINS_SW)
		{
			// GAUT builtins functions and operators
			// next word is the file name
			i++;
			istrstream txt(argv[i]);
			if ((txt >> file_name).fail()) usage(argc, argv);
			findCheck(file_name);
			_builtins = file_name;
		}
		else if (word == CADENCY_SW)
		{
			// next word is the number of time units
			i++;
			istrstream txt(argv[i]);
			if ((txt >> units).fail()) usage(argc, argv);
			_cadency = units;
		}
		else if (word == CDFG_SW)
		{
			// next word is the name of the library file
			i++;
			_cdfg_file_name = argv[i];
			/* Caaliph 27/06/2007 */
		}
		else if (word == CADENCY_PRIM_SW)
		{
			_cadency_prim = true;
			i++;
			_cadency_prim_val = atol(argv[i]);
		}
		else if (word == CDFGs_SW)
		{
			// next word is the name of the library file
			_cdfgs = true;
			i++;
			int j=0;
			while (word != "-fin_cdfgs")
			{
				_cdfgs_file_name[j] = argv[i];
				i++;
				_cadencys[j] = atol(argv[i]);
				i++;
				_modes[j] = atol(argv[i]);
				_mode_numbers[j] = argv[i];
				i++;
				j++;
				word = argv[i];
			}
		}
		else if (word == N_ORDO_SW)
		{
			// next word is the name of the library file
			_n_ordo = true;
			i++;
			int j=0;
			while (word != "-fin_nordo")
			{
				_cdfgs_file_name[j] = argv[i];
				i++;
				_cadencys[j] = atol(argv[i]);
				i++;
				_modes[j] = atol(argv[i]);
				_mode_numbers[j] = argv[i];
				i++;
				j++;
				word = argv[i];
			}
		}
		else if (word == SPACT_SW)
		{
			// next word is the name of the library file
			_spact = true;
			i++;
			int j=0;
			while (word != "-fin_spact")
			{
				_cdfgs_file_name[j] = argv[i];
				i++;
				_cadencys[j] = atol(argv[i]);
				i++;
				_modes[j] = atol(argv[i]);
				_mode_numbers[j] = argv[i];
				i++;
				j++;
				word = argv[i];
			}
		}
		else if (word == MODE_SW)
		{
			// next word is the name of the library file
			_mode = true;
			i++;
			_mode_number = argv[i];
			_mode_val = atol(argv[i]);
		}
		else if (word == BIND_SW)
		{
			// next word is the name of the library file
			_binding = true;
			i++;
			_binding_number = argv[i];
			_binding_case = argv[i];
		}
		else if (word == MM_SW)
		{
			// next word is the name of the library file
			_multimode = true;
			i++;
			_multimode_number = argv[i];
			_multimode_case = argv[i];
		}
		else if (word == MEMFILES_SW)
		{
			i++;
			int j=0;
			while (word != "-fin_mems")
			{
				_mem_file_names[j] = argv[i];
				j++;
				i++;
				word = argv[i];
			}
		}
		else if (word == VHDLS_SW)
		{
			// next word is the file name
			i++;
			int j=0;
			while (word != "-fin_vhdl_names")
			{
				_vhdl_file_names[j] = argv[i];
				j++;
				i++;
				word = argv[i];
			}
		}
		else if (word == VHDL_PREFIXS_SW)
		{
			// next word is the file name
			i++;
			int j=0;
			while (word != "-fin_prefix_names")
			{
				_vhdl_prefixs[j] = argv[i];
				j++;
				i++;
				word = argv[i];
			}
		}
		else if (word == DUMP_STAR_CDFG_SW)
		{
			_dumpSTAR_CDFG = true;
			// next word is the file name
			i++;
			_dumpSTAR_CDFGfile = argv[i];
			/* End Caaliph */
		}
		else if (word == CLOCK_SW)
		{
			// next word is the number of time units
			i++;
			istrstream txt(argv[i]);
			if ((txt >> units).fail()) usage(argc, argv);
			_system_clock_period = units;
		}
		else if (word == DUMP_BEST_SELECTION_SW)
		{
			_dumpBestSelection = true;
			// next word is the file name
			i++;
			_dumpBestSelectionFile = argv[i];
		}
		else if (word == DUMP_CDFG_SW)
		{
			_dumpCDFG = true;
			// next word is the file name
			i++;
			_dumpCDFGfile = argv[i];
		}
		else if (word == BITWIDTH_AWARE_SW)
		{
			_bitwidth_aware = true;
			/* GL 16/04/2007 :	Add a switch to specify the resizing
								mode when bitwidthAware is used*/
			//! Get "resizing mode" toggle
		}
		else if (word == RESIZING_MODE_SW)
		{
			// next word is the mode id (no check performed)
			i++;

			istrstream txt(argv[i]);
			if ((txt >> mode).fail()) usage(argc, argv);
			_resizingMode = mode;
			/* Fin GL */
			/* GL 07/05/2007 :	Add a switch to specify the scheduling
								mode when bitwidthAware is used*/
			//! Get "clustering mode" toggle
		}
		else if (word == CLUSTERING_MODE_SW)
		{
			// next word is the mode id (no check performed)
			i++;
			istrstream txt(argv[i]);
			if ((txt >> mode).fail()) usage(argc, argv);
			_clusteringMode = mode;
			//! Get "scheduling mode" toggle
		}
		else if (word == SCHEDULING_MODE_SW)
		{
			// next word is the mode id (no check performed)
			i++;
			istrstream txt(argv[i]);
			if ((txt >> mode).fail()) usage(argc, argv);
			_schedulingMode = mode;
			//! Get "binding mode" toggle
		}
		else if (word == REGISTERALLOCATION_MODE_SW)
		{
			// next word is the mode id (no check performed)
			i++;
			istrstream txt(argv[i]);
			if ((txt >> mode).fail()) usage(argc, argv);
			_registerAllocationMode = mode;
			//! Get "binding mode" toggle
		}
		else if (word == BINDING_MODE_SW)
		{
			// next word is the mode id (no check performed)
			i++;
			istrstream txt(argv[i]);
			if ((txt >> mode).fail()) usage(argc, argv);
			_bindingMode = mode;
			/* Fin GL */
			/* GL 25/09/07 :  (new generation strategy)*/
		}
		else if (word == PATTERN_SW)
		{
			// next word is the name of the pattern file
			i++;
			_pattern = argv[i];
		}
		else if (word == ALLOW_CHAINING_SW)
		{
			_chaining = true;
			/* Fin GL */
			/* GL 18/06/08 :  output on buses */
		}
		else if (word == H_SCHEDULE_SW)
		{
			// next word is the hierachical schaduling
			i++;
			_hSchedule = argv[i];
		/* Fin GL */
		}
		else if (word == DUMP_DEBUG2C_SW)
		{
			_dumpDebug2c = true;
		}
		else if (word == DUMP_LIB_SW)
		{
			_dumpLib = true;
			// next word is the file name
			i++;
			_dumpLibFile = argv[i];
		}
		else if (word == DUMP_SELECTIONS_SW)
		{
			_dumpSelections = true;
			// next word is the file name
			i++;
			_dumpSelectionsFile = argv[i];
		}
		else if (word == DYNAMIC_ALLOCATION_SW)
		{
			_dynamicAllocation = true;
		}
		else if (word == FORCE_OVERWRITE_SW)
		{
			_force_overwrite = true;
		}
		else if (word == IOC_SW)
		{
			_io_constraints = true;
			if (!isAswitch(argv[i+1]))
			{
				i++;
				_io_constraints_file = true;
				_io_constraints_file_name = argv[i];;
			}
		}
		else if (word == MAPC_SW)
		{
			_mapping_constraints = true;
			if (!isAswitch(argv[i+1]))
			{
				i++;
				_mapping_constraints_file = true;
				_mapping_constraints_file_name = argv[i];;
			}
		}
		else if (word == LIB_SW)
		{
			// next word is the name of the library file
			i++;
			_lib_file_name = argv[i];
		}
		else if (word == MEM_SW)
		{
			// next word is the number of time units
			i++;
			istrstream txt(argv[i]);
			if ((txt >> units).fail()) usage(argc, argv);
			_memory_access_time = units;
		}
		else if (word == MEMFILE_SW)
		{
			i++;
			_mem_file_name = argv[i];
		}
		else if (word == SELECTION_SW)
		{
			_selection = true;
			// next word is the file name
			i++;
			_selectionFile = argv[i];
		}
		else if (word == SILENT_SW)
		{
			_silent_mode = true;
		}
		else if (word == TIMEUNIT_SW)
		{
			// next word is time unit
			i++;
			string tu = argv[i];
			if (tu == "fs") _time_unit = FS;
			else if (tu == "ps") _time_unit = PS;
			else if (tu == "ns") _time_unit = NS;
			else if (tu == "us") _time_unit = US;
			else if (tu == "ms") _time_unit = MS;
			else if (tu == "s")  _time_unit = S;
			else usage(argc, argv);
		}
		else if (word == USERLIB_SW)
		{
			// user library file name
			// next word is the file name
			i++;
			istrstream txt(argv[i]);
			if ((txt >> file_name).fail()) usage(argc, argv);
			findCheck(file_name);
			_userlib = file_name;
		}
		else if (word == VERBOSE_SW)
		{
			_verbose = true;
		}
		else if (word == VHDL_SW)
		{
			// next word is the file name
			i++;
			_vhdl_file_name = argv[i];
		}
		else if (word == VHDL_PREFIX_SW)
		{
			// next word is the file name
			i++;
			_vhdl_prefix = argv[i];
		}
		else if (word == VHDL_TYPE_SW)
		{
			// next word is output vhdl type
			i++;
			_vhdl_type = argv[i];
			if (_vhdl_type != "fsm_sigs" && _vhdl_type != "fsm_regs"
			        && _vhdl_type != "rom_regs" && _vhdl_type != "fsm_data" )
				usage(argc, argv);
		}
		else if (word == NVHDL_SW)
		{
			_genVHDL = true;
		}
		else if (word == NSOCLIB_SW)
		{
			_genSOCLIB = true;
		}
		else if (word == NGANTT_SW)
		{
			_genGANTT = true;
		}
		else if (word == NMEM_SW)
		{
			_genMEM = true;

			/* KT 15/12/2007 :	Add  switchs*/
		}
		else if (word == COSTING_SW)
		{
			_costing = true;
		}
		else if (word == N1_SW)
		{
			_costing = true;
			_n1 = true;
		}
		else if (word == N2_SW)
		{
			_costing = true;
			_n2 = true;
		}
		else if (word == N3_SW)
		{
			_costing = true;
			_n3 = true;
		}
		else if (word == N4_SW)
		{
			_costing = true;
			_n4 = true;
		}
		else if (word == VNS_SW)
		{
			_costing = true;
			_vns = true;
			/* End KT */
			/* KT 06/06/2007 :	Add  switchs*/

		}
		else if (word == GLOBAL_TH_LB_SW)
		{
			_global_th_lb = true;
			_withreuse = true;
			_lower_bound = true;
			_global_allocation= true;
			_theoretical= true;
		}
		else if (word == GLOBAL_PR_UB_SW)
		{
			_global_pr_ub = true;
			_upper_bound = true;
			_withreuse = true;
			_practical= true;
			_global_allocation= true;
		}
		else if (word == GLOBAL_PR_LB_SW)
		{
			_global_pr_lb= true;
			_global_allocation= true;
			_practical= true;
			_lower_bound= true;
			_withreuse = true;
		}
		else if (word == DISTRIBUTED_TH_SW)
		{
			_distributed_th = true;
			_distributed_allocation = true;
			_withreuse = false;
			_lower_bound = true;
			_theoretical = true;
		}
		else if (word == DISTRIBUTED_REUSE_TH_SW)
		{
			_distributed_reuse_th = true;
			_distributed_allocation = true;
			_withreuse = true;
			_lower_bound = true;
			_theoretical = true;
		}
		else if (word == DISTRIBUTED_REUSE_PR_UB_SW)
		{
			_distributed_reuse_pr_ub = true;
			_distributed_allocation = true;
			_withreuse = true;
			_upper_bound = true;
			_practical= true;
		}
		else if (word == DISTRIBUTED_REUSE_PR_SW)
		{
			_distributed_reuse_pr= true;
			_distributed_allocation= true;
			_withreuse = true;
			_lower_bound= true;
			_practical= true;
			/* End KT */

			/* EJ 26/06/2007 */
		}
		else if (word == SCH_SGY_SW)
		{
			// next word is the scheduling strategy
			i++;
			_scheduling_strategy = argv[i];
			if (_scheduling_strategy != "default" && _scheduling_strategy != "force_no_pipeline"
			        && _scheduling_strategy != "force_no_mobility" && _scheduling_strategy != "no_more_stage"
			        && _scheduling_strategy != "force_no_pipeline+min_length_fsm"
			        // Caaliph 10/09/08
			        && _scheduling_strategy != "asap")
				// End Caaliph
				usage(argc, argv);
		}
		else if (word == OP_OPTIMIZATION_SW)
		{
			_operator_optimization = true;
		}
		else if (word == MANUAL_ALLOCATION_SW)
		{
			_manual_allocation = true;
		}
		else if (word == USE_IHM_SW)
		{
			_use_IHM = true;
			/* End EJ */
		}
		else
			usage(argc, argv);
	}

	// look for possible errors

	// Cadency must be specified
	if (_cadency == -1)
		/* Caaliph 28/06/2007 */
		if ((_cdfgs==false) && (_n_ordo==false) && (_spact==false))
			/* End Caaliph */
			usage(argc, argv);
	// Cadency must be a multiple of system_clock_period
	/* Caaliph 28/06/2007 */
	if (_cadency != -1)
		/* End Caaliph */
		check_is_multiple(_cadency, "cadency", _system_clock_period, "system clock period");
	// Memory access time must be a multiple of system clock
	check_is_multiple(_memory_access_time, "memory access time", _system_clock_period, "system clock period");
	// Cadency must be a multiple of memory access time
	/* Caaliph 28/06/2007 */
	if (_cadency != -1)
		/* End Caaliph */
		check_is_multiple(_cadency, "cadency", _memory_access_time, "memory access time");
	// Check library exists
	if (_lib_file_name.size() == 0) Tools::userErrStop("Library file required");
	Tools::check_file_exists(_lib_file_name);
	// check cdfg file exists
	if (_cdfg_file_name.size() == 0)
		/* Caaliph 28/06/2007 */
		if (_cdfgs_file_name->size() == 0)
			/* End Caaliph */
			Tools::userErrStop("CDFG file required or CDFGs files required");
	/* Caaliph 28/06/2007 */
	if (_cdfgs==false && _n_ordo==false && _spact==false)
		/* End Caaliph */
		Tools::check_file_exists(_cdfg_file_name);

	// default values "computed"
	/* Caaliph 28/06/2007 */
	if (_cdfgs==false && _n_ordo==false && _spact==false)
	{
		/* End Caaliph */
		if (_mem_file_name.size() == 0) _mem_file_name = Tools::prefix(_cdfg_file_name)+".mem";
		if (_vhdl_file_name.size() == 0) _vhdl_file_name = Tools::prefix(_cdfg_file_name)+".vhd";
		if (_soclib_file_name.size() == 0) _soclib_file_name = "vci_"+Tools::prefix(_cdfg_file_name);
		/* Caaliph 28/06/2007 */
	}
	/* End Caaliph */

	// dump switches values if not in silent mode
	if (!_silent_mode) info(argc, argv);

#ifdef CHECK
	switches_create++;
#endif

};

//	End of:		switches.cpp
