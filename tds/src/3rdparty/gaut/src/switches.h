/*------------------------------------------------------------------------------
  Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

  Cette licence est un accord légal conclu entre vous et l'Université de
  Bretagne Sud(UBS)concernant l'outil GAUT2.

  Cette licence est une licence CeCILL-B dont deux exemplaires(en langue
  française et anglaise)sont joints aux codes sources. Plus d'informations
  concernant cette licence sont accessibles à http://www.cecill.info.

  AUCUNE GARANTIE n'est associée à cette license !

  L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
  de GAUT2 et le distribue en l'état à des fins de coopération scientifique
  seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
  de protection de leurs données qu'il jugeront nécessaires.
  ------------------------------------------------------------------------------
  2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

  This license is a legal agreement between you and the University of
  South Britany(UBS)regarding the GAUT2 software tool.

  This license is a CeCILL-B license. Two exemplaries(one in French and one in
  English)are provided with the source codes. More informations about this
  license are available at http://www.cecill.info.

  NO WARRANTY is provided with this license.

  The University of South Britany does not provide any warranty about
  the usage of GAUT2 and distributes it "as is" for scientific cooperation
  purpose only. All users are greatly advised to provide all the necessary
  protection issues to protect their data.
  ------------------------------------------------------------------------------
  */

//	File:		switches.h
//	Purpose:	command line parsing
//	Author:		Pierre Bomel, LESTER, UBS

// forward refs
class Switches;

#ifndef __SWITCHES_H__
#define __SWITCHES_H__

class Switches;

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "timeUnits.h"
#include "check.h"
using namespace std;

//! User switches
class Switches
{
public:
	//type
	enum Scheduling_strategy
	{
		DEFAULT,
		FORCE_NO_MOBILITY,
		FORCE_NO_PIPELINE,
		NO_MORE_STAGE,
		NO_PIPELINE_MIN_FSM,
		// Caaliph 10/09/08
		AS_SO_AS_PO
	};

	enum Vhdl_type
	{
		UT_FSM_S,// default
		UT_FSM_R,
		UT_ROM,
		UT_FSM_D
	};


	enum RegisterAllocation_type
	{
		REG_CLUSTER_MWBM,// default
		//REG_GLOBAL_MWBM,
		REG_MLEA,
		REG_LeftEdge,
		REG_NONE
	};

	/* Caaliph 27/06/2007*/
	string		_cdfgs_file_name[100];//add by Caaliph
	string		_vhdl_prefixs[100]; //add by Caaliph
	string      _vhdl_file_names[100]; //add by Caaliph
	string		_mem_file_names[100];		// //add by Caaliph
	long        _cadencys[100];//add by Caaliph
	long        _modes[100];//add by Caaliph
	string		_mode_numbers[100];//add by Caaliph
	/* End Caaliph*/
private:
	// environment variables
	static const string		GAUT_VAR;
	// GAUT IDENTITY
	static const string		GAUT_NAME;
	static const string		VERSION;
	// command line switches
	static const string		BUILTINS_SW;
	static const string		CADENCY_SW;
	static const string		CDFG_SW;
	/* Caaliph 27/06/2007*/
	static const string		CADENCY_PRIM_SW; //add by Caaliph
	static const bool		CADENCY_PRIM_DEF;
	static const int		CADENCY_PRIM_VAL;
	static const string		CDFGs_SW; //add by Caaliph
	static const bool		CDFGs_DEF; //add by Caaliph
	static const string		CDFGs_FILE_DEF; //add by Caaliph
	static const string		N_ORDO_SW; //add by Caaliph
	static const bool		N_ORDO_DEF; //add by Caaliph
	static const string		N_ORDO_FILE_DEF; //add by Caaliph
	static const string		SPACT_SW; //add by Caaliph
	static const bool		SPACT_DEF; //add by Caaliph
	static const string		SPACT_FILE_DEF; //add by Caaliph
	static const string		MODE_SW; //add by Caaliph
	static const bool		MODE_DEF; //add by Caaliph
	static const string		MODE_FILE_DEF; //add by Caaliph
	static const long		MODE_VAL; //add by Caaliph
	static const string		BIND_SW; //add by Caaliph
	static const bool		BIND_DEF; //add by Caaliph
	static const string		BIND_FILE_DEF; //add by Caaliph
	static const string		BIND_CASE; //add by Caaliph
	// Multimode flow
	static const string		MM_SW; //add by Caaliph
	static const bool		MM_DEF; //add by Caaliph
	static const string		MM_FILE_DEF; //add by Caaliph
	static const string		MM_CASE; //add by Caaliph
	// End MM flow
	static const string		DUMP_STAR_CDFG_SW;
	static const string		DUMP_STAR_CDFG_FILE_DEF;
	static const bool		DUMP_STAR_CDFG_DEF;
	/* End Caaliph*/
	static const string		CLOCK_SW;
	static const string		DUMP_BEST_SELECTION_SW;
	static const string		DUMP_LIB_SW;
	static const string		DUMP_SELECTIONS_SW;
	static const string		DUMP_CDFG_SW;

	static const string		BITWIDTH_AWARE_SW;

	/* GL 16/04/2007 :	Add a switch to specify the resizing
	   mode when bitwidthAware is used*/
	static const string		RESIZING_MODE_SW;
	/* Fin GL*/
	/* GL 07/05/2007 :	Add a switch to specify the scheduling
	   mode when bitwidthAware is used*/
	static const string		CLUSTERING_MODE_SW;
	static const string		SCHEDULING_MODE_SW;
	static const string		BINDING_MODE_SW;
	/* Fin GL*/
	static const string		REGISTERALLOCATION_MODE_SW;
	/* GL 25/09/07 :(new generation strategy)*/
	static const string		GENERATION_MODE_SW;
	/* Fin GL*/
	/* GL 25/10/07 : pattern(add lines)*/
	static const string		PATTERN_SW;
	static const string		ALLOW_CHAINING_SW;
	/* Fin GL*/
	/* GL 20/12/08 : Complex operators*/
	static const string		H_SCHEDULE_SW;
	/* Fin GL*/
	static const string		DUMP_DEBUG2C_SW;
	static const string		DYNAMIC_ALLOCATION_SW;
	static const string		FORCE_OVERWRITE_SW;
	static const string		IOC_SW;
	static const string		LIB_SW;
	static const string		MAPC_SW;
	static const string		MEM_SW;
	static const string		MEMFILE_SW;
	/* Caaliph 27/06/2007*/
	static const string		MEMFILES_SW;
	/* End Caaliph*/
	static const string		SELECTION_SW;
	static const string		SILENT_SW;
	static const string		TIMEUNIT_SW;
	static const string		USERLIB_SW;
	static const string		VERBOSE_SW;
	static const string		VHDL_SW;
	static const string		VHDL_PREFIX_SW;
	static const string		VHDL_TYPE_SW; // EJ 09/2007
	/* Caaliph 27/06/2007*/
	static const string		VHDLS_SW;
	static const string		VHDL_PREFIXS_SW;
	/* End Caaliph*/
	static const string		NVHDL_SW;
	static const string		NSOCLIB_SW;
	static const string		NGANTT_SW;
	static const string		NMEM_SW;
	static const string		SCH_SGY_SW;
	static const string		OP_OPTIMIZATION_SW;

	/* KT 15/12/2007 :	Add switchs,*/
	static const string		COSTING_SW;
	static const string		N1_SW;
	static const string		N2_SW;
	static const string		N3_SW;
	static const string		N4_SW;
	static const string		VNS_SW;

	/* end KT*/

	/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling*/
	static const string		GLOBAL_TH_LB_SW;
	static const string		GLOBAL_PR_UB_SW;
	static const string		GLOBAL_PR_LB_SW;
	static const string		DISTRIBUTED_TH_SW;
	static const string		DISTRIBUTED_REUSE_TH_SW;
	static const string		DISTRIBUTED_REUSE_PR_UB_SW;
	static const string		DISTRIBUTED_REUSE_PR_SW;
	/* switchs specifying allocation mode*/
	static const string		GLOBAL_ALLOCATION_SW;
	static const string		DISTIBUTED_ALLOCATION_SW;
	static const string		PRACTICAL_SW;
	static const string		THEORETICAL_SW;
	/* switchs specifying scheduling mode*/
	static const string		WITHREUSE_SW;
	static const string		LOWER_BOUND_SW;
	static const string		UPPER_BOUND_SW;
	/* end KT*/

	static const string		MANUAL_ALLOCATION_SW;
	static const string		USE_IHM_SW;


	// default values
	static const string		BUILTINS_DEF;
	static const int		CADENCY_DEF;
	static const int		CLOCK_DEF;
	static const bool		DUMP_BEST_SELECTION_DEF;
	static const bool		DUMP_CDFG_DEF;

	static const bool		BITWIDTH_AWARE_DEF;

	/* GL 16/04/2007 :	Add a switch to specify the resizing
	   mode when bitwidthAware is used*/
	static const int		RESIZING_MODE_DEF;
	/* Fin GL*/
	/* GL 07/05/2007 :	Add a switch to specify the scheduling
	   mode when bitwidthAware is used*/
	static const int		CLUSTERING_MODE_DEF;
	static const int		SCHEDULING_MODE_DEF;
	static const int		BINDING_MODE_DEF;
	/* Fin GL*/
	static const int		REGISTERALLOCATION_MODE_DEF;
	/* GL 25/09/07 :(new generation strategy)*/
	static const int		GENERATION_MODE_DEF;
	/* Fin GL*/
	/* GL 25/10/07 : pattern(add lines)*/
	static const string		PATTERN_DEF;
	static const bool		ALLOW_CHAINING_DEF;
	/* GL 20/12/08 : Complex operators*/
	static const string		H_SCHEDULE_DEF;
	/* Fin GL*/
	static const bool		DUMP_DEBUG2C_DEF;
	static const bool		DUMP_LIB_DEF;
	static const string		DUMP_LIB_FILE_DEF;
	static const bool		DUMP_SELECTIONS_DEF;
	static const string		DUMP_BEST_SELECTION_FILE_DEF;
	static const string		DUMP_CDFG_FILE_DEF;
	static const string		DUMP_SELECTIONS_FILE_DEF;
	static const bool		DYNAMIC_ALLOCATION_DEF;
	static const bool		FORCE_OVERWRITE_DEF;
	static const bool		IOC_DEF;
	static const bool		IOC_FILE_DEF;
	static const string		IOC_FILE_NAME_DEF;
	static const string		LIB_DEF;
	static const bool		MAPC_DEF;
	static const bool		MAPC_FILE_DEF;
	static const string		MAPC_FILE_NAME_DEF;
	static const int		MEM_DEF;
	static const string		MEMFILE_DEF;
	static const bool		SELECTION_DEF;
	static const string		SELECTION_FILE_DEF;
	static const bool		SILENT_DEF;
	static const string		TIMEUNIT_DEF;
	static const bool		VERBOSE_DEF;
	static const string		VHDL_DEF;
	static const string		VHDL_PREFIX_DEF;
	static const string		VHDL_TYPE_DEF; // EJ 09/2007
	static const bool		NVHDL_DEF;
	static const bool		NSOCLIB_DEF;
	static const bool		NGANTT_DEF;
	static const bool		NMEM_DEF;
	static const string		SCH_SGY_DEF;
	static const bool		OP_OPTIMIZATION_DEF;

	/* KT 15/12/2007 :	Add switchs,*/
	static const bool		COSTING_DEF;
	static const bool		N1_DEF;
	static const bool		N2_DEF;
	static const bool		N3_DEF;
	static const bool		N4_DEF;
	static const bool		VNS_DEF;
	/* end KT*/

	/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling*/
	static const bool	GLOBAL_TH_LB_DEF;
	static const bool	GLOBAL_PR_UB_DEF;
	static const bool	GLOBAL_PR_LB_DEF;
	static const bool	DISTRIBUTED_TH_DEF;
	static const bool	DISTRIBUTED_REUSE_TH_DEF;
	static const bool	DISTRIBUTED_REUSE_PR_UB_DEF;
	static const bool	DISTRIBUTED_REUSE_PR_DEF;
	/* switchs specifying allocation mode*/
	static const bool	GLOBAL_ALLOCATION_DEF;
	static const bool	DISTIBUTED_ALLOCATION_DEF;
	static const bool	PRACTICAL_DEF;
	static const bool	THEORETICAL_DEF;
	/* switchs specifying scheduling mode*/
	static const bool	WITHREUSE_DEF;
	static const bool	LOWER_BOUND_DEF;
	static const bool	UPPER_BOUND_DEF;
	/* end KT*/

	static const bool		MANUAL_ALLOCATION_DEF;
	static const bool		USE_IHM_DEF;


	TimeUnit	_time_unit;
	long		_system_clock_period;
	long		_cadency;
	/* Caaliph 27/06/2007*/
	long		_cadency_prim_val;/// Add by Caaliph
	bool		_cadency_prim;/// Add by Caaliph
	/* End Caaliph*/
	bool		_silent_mode;
	long		_memory_access_time;
	string		_lib_file_name;
	string		_cdfg_file_name;
	/* Caaliph 27/06/2007*/
	bool		_cdfgs; //add by Caaliph
	string		_n_ordo_file_name;//add by Caaliph
	bool		_n_ordo; //add by Caaliph
	string		_spact_file_name;//add by Caaliph
	bool		_spact; //add by Caaliph
	string		_mode_number;//add by Caaliph
	long		_mode_val;//add by Caaliph
	bool		_mode; //add by Caaliph
	string		_binding_number;//add by Caaliph
	string		_binding_case;//add by Caaliph
	bool		_binding; //add by Caaliph
	// Multimode flow
	string		_multimode_number;//add by Caaliph
	string		_multimode_case;//add by Caaliph
	bool		_multimode; //add by Caaliph
	// End MM flow
	bool		_dumpSTAR_CDFG;		//!< toggle to generate CDFG
	string		_dumpSTAR_CDFGfile;	//!< when _dumpCDFGafterSelection == true, this is the file where to dump
	/* End Caaliph*/
	void		usage(int argc, char*argv[])const;
	void		info(int argc, char*argv[])const;
	void		check_is_multiple(long val, string val_str, long ref, string ref_str)const;
	string		_userlib;		//!< user library extension file name
	string		_builtins;		//!< CDFG model for functions(target independant)
	bool		_dumpCDFG;		//!< toggle to generate CDFG
	string		_dumpCDFGfile;	//!< when _dumpCDFGafterSelection == true, this is the file where to dump

	bool		_bitwidth_aware;//!<

	/* GL 16/04/2007 :	Add a switch to specify the resizing
	   mode when bitwidthAware is used*/
	int			_resizingMode;	//!< specify the resizing mode when bitwidthAware is used
	/* Fin GL*/
	/* GL 07/05/2007 :	Add a switch to specify the scheduling
	   mode when bitwidthAware is used*/
	int			_clusteringMode;	//!< specify the clusterling mode when bitwidthAware is used
	int			_schedulingMode;	//!< specify the scheduling mode when bitwidthAware is used
	int			_bindingMode;		//!< specify the binding mode when bitwidthAware is used
	/* Fin GL*/
	int			_registerAllocationMode;	//!< specify the register allocation mode(0=MWBM, 1=Left Edge, 2=NONE)
	/* GL 25/10/07 : pattern(add lines)*/
	string		_pattern;
	bool		_chaining;
	/* Fin GL*/
	/* GL 20/12/08 : Complex operators*/
	string		_hSchedule;
	/* Fin GL*/
	bool		_dumpDebug2c;	//!< toggle to generate CDFG to C
	bool		_selection;		//!< tells a selection has already been made by the user
	string		_selectionFile;	//!< when _selection == true, gives the file name
	bool		_dumpBestSelection;
	string		_dumpBestSelectionFile;
	bool		_dumpSelections;
	string		_dumpSelectionsFile;
	bool		_dynamicAllocation;
	bool		_verbose;		// verbose mode toggle
	bool		_io_constraints;
	bool		_io_constraints_file;
	string		_io_constraints_file_name;
	bool		_mapping_constraints;
	bool		_mapping_constraints_file;
	string		_mapping_constraints_file_name;
	string		_vhdl_file_name;
	string		_soclib_file_name;
	string		_vhdl_prefix;
	string		_vhdl_type; // EJ 09/2007
	string		_mem_file_name;		// .mem file name
	bool		_force_overwrite;	// to allow overwriting of output files
	bool		_dumpLib;			// to allow debug of the lib
	string		_dumpLibFile;
	bool		_genVHDL, _genGANTT, _genMEM, _genSOCLIB;
	string		_scheduling_strategy;
	bool		_operator_optimization;


	/* KT 15/12/2007 :	Add switchs,*/
	bool	_costing;
	bool	_n1;
	bool	_n2;
	bool	_n3;
	bool	_n4;
	bool	_vns;
	/* end KT*/
	/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling*/
	bool	_global_th_lb;
	bool	_global_pr_ub;
	bool	_global_pr_lb;
	bool	_distributed_th;
	bool	_distributed_reuse_th;
	bool	_distributed_reuse_pr_ub;
	bool	_distributed_reuse_pr;
	/* switchs specifying allocation mode*/
	bool	_global_allocation;
	bool	_distributed_allocation;
	bool	_practical;
	bool	_theoretical;
	/* switchs specifying scheduling mode*/
	bool		_withreuse;
	bool	_lower_bound;
	bool	_upper_bound;
	/* end KT*/

	bool		_manual_allocation; //!< manual allocation
	bool		_use_IHM;			//!<
public:
	//! Create a new user switche object.
	Switches(int argc, char**argv);
	//! Delete a user switches object.
	~Switches()
	{
#ifdef CHECK
		switches_delete++;
#endif
	}
	//! Get time unit
	TimeUnit time_unit()const
	{
		return _time_unit;
	}
	//! Get cadency
	long cadency()const
	{
		return _cadency;
	}
	//! Get system clock period
	long system_clock_period()const
	{
		return _system_clock_period;
	}
	//! Get memory access time
	long memory_access_time()const
	{
		return _memory_access_time;
	}
	//! Get library file name
	string lib_file_name()const
	{
		return _lib_file_name;
	}
	//! Get CDFG file name
	string cdfg_file_name()const
	{
		return _cdfg_file_name;
	}
	/* Caaliph 27/06/2007*/
	//! Get cadency; add by Caaliph
	bool cadency_prim()const
	{
		return _cadency_prim;
	}
	long cadency_prim_val()const
	{
		return _cadency_prim_val;
	}
	void cadency_prim_val(long cadency_main)
	{
		_cadency_prim_val=cadency_main;
	}
	//! Get CDFG file name
	bool cdfgs()const
	{
		return _cdfgs;
	}
	//! Get CDFG file name
	void cdfgs(bool cdfgs)
	{
		_cdfgs=cdfgs;
	}
	bool n_ordo()const
	{
		return _n_ordo;
	}
	void n_ordo(bool n_ordo)
	{
		_n_ordo=n_ordo;
	}
	string n_ordo_file_name()const
	{
		return _n_ordo_file_name;
	}
	bool spact()const
	{
		return _spact;
	}
	void spact(bool spact)
	{
		_spact=spact;
	}
	string spact_file_name()const
	{
		return _spact_file_name;
	}
	//! Get CDFG file name
	bool mode()const
	{
		return _mode;
	}
	string mode_number()const
	{
		return _mode_number;
	}
	long mode_val()const
	{
		return _mode_val;
	}
	//! Get CDFG file name
	bool binding()const
	{
		return _binding;
	}
	string binding_number()const
	{
		return _binding_number;
	}
	string binding_case()const
	{
		return _binding_case;
	}
	// Multimode flow
	bool multimode()const
	{
		return _multimode;
	}
	string multimode_number()const
	{
		return _multimode_number;
	}
	string multimode_case()const
	{
		return _multimode_case;
	}
	//! Get "dump STAR CDFG" toggle
	bool dumpSTAR_CDFG()const
	{
		return _dumpSTAR_CDFG;
	}
	//! Get "dump STAR CDFG" file name
	string dumpSTAR_CDFGfile()const
	{
		return _dumpSTAR_CDFGfile;
	}
	/* End Caaliph*/
	//! Get silent mode toggle
	bool silent_mode()const
	{
		return _silent_mode;
	}
	//! Get user library name
	string userlib()const
	{
		return _userlib;
	}
	//! Get builtins name
	string builtins()const
	{
		return _builtins;
	}
	void findCheck(string & file_name)const;
	//! Get "dump CDFG" toggle
	bool dumpCDFG()const
	{
		return _dumpCDFG;
	}
	//! Get "dump CDFG" file name
	string dumpCDFGfile()const
	{
		return _dumpCDFGfile;
	}

	//! Get "new flow mode" toggle

	bool bitwidth_aware()const
	{
		return _bitwidth_aware;
	}

	/* GL 16/04/2007 :	Add a switch to specify the resizing
	   mode when bitwidthAware is used*/
	//! Get "resizing mode" toggle

	int resizingMode()const
	{
		return _resizingMode;
	}
	/* Fin GL*/
	/* GL 07/05/2007 :	Add a switch to specify the scheduling
	   mode when bitwidthAware is used*/
	//! Get "scheduling mode" toggle

	int clusteringMode()const
	{
		return _clusteringMode;
	}
	int schedulingMode()const
	{
		return _schedulingMode;
	}
	int bindingMode()const
	{
		return _bindingMode;
	}


	int registerAllocationMode()const
	{
		return _registerAllocationMode;
	}


	/* GL 25/10/07 : pattern(add lines)*/
	string pattern()const
	{
		return _pattern;
	}
	bool chaining()const
	{
		return _chaining;
	}
	/* Fin GL*/
	/* GL 20/12/08 : Complex operators*/
	string hSchedule()const
	{
		return _hSchedule;
	}
	/* Fin GL*/
	//! Get "cdfg2c" toggle
	bool dumpDebug2c()const
	{
		return _dumpDebug2c;
	}
	//! Get selection toggle
	bool selection()const
	{
		return _selection;
	}
	//! Get selection file name
	string selectionFile()const
	{
		return _selectionFile;
	}
	//! Get "dump best selection" toggle
	bool dumpBestSelection()const
	{
		return _dumpBestSelection;
	}
	//! Get "dump best selection" file name
	string dumpBestSelectionFile()const
	{
		return _dumpBestSelectionFile;
	}
	//! Get "dump selection" toggle
	bool dumpSelections()const
	{
		return _dumpSelections;
	}
	//! Get "dump selection" file name
	string dumpSelectionsFile()const
	{
		return _dumpSelectionsFile;
	}
	//! Get "dynamic allocation of operators" toggle
	bool dynamicAllocation()const
	{
		return _dynamicAllocation;
	}
	//! Get "IO constraints" toggle
	bool ioConstraints()const
	{
		return _io_constraints;
	}
	//! Get "IO constraints file" toggle
	bool ioConstraintsFile()const
	{
		return _io_constraints_file;
	}
	//! Get "IO constraints file" name
	string ioConstraintsFileName()const
	{
		return _io_constraints_file_name;
	}
	//! Get "memory bank mapping constraints" toggle
	bool mappingConstraints()const
	{
		return _mapping_constraints;
	}
	//! Get "memory bank mapping constraints file" toggle
	bool mappingConstraintsFile()const
	{
		return _mapping_constraints_file;
	}
	//! Get "memory bank mapping constraints file" name
	string mappingConstraintsFileName()const
	{
		return _mapping_constraints_file_name;
	}
	//! Get verbose toggle
	bool verbose()const
	{
		return _verbose;
	}

	/* KT 15/12/2007 :	Add switchs,*/
	bool costing()const
	{
		return _costing;
	};
	bool n1()const
	{
		return _n1;
	};
	bool n2()const
	{
		return _n2;
	};
	bool n3()const
	{
		return _n3;
	};
	bool n4()const
	{
		return _n4;
	};
	bool vns()const
	{
		return _vns;
	};
	/* end KT*/

	/* KT 06/06/2007 :	Add switchs, combinations of various modes of allocation and scheduling*/
	//! Get scenario
	bool	global_th_lb()const
	{
		return _global_th_lb;
	};
	bool	global_pr_ub()const
	{
		return _global_pr_ub;
	};
	bool	global_pr_lb()const
	{
		return _global_pr_lb;
	};
	bool	distributed_th()const
	{
		return _distributed_th;
	};
	bool	distributed_reuse_th()const
	{
		return _distributed_reuse_th;
	};
	bool	distributed_reuse_pr_ub()const
	{
		return _distributed_reuse_pr_ub;
	};
	bool	distributed_reuse_pr()const
	{
		return _distributed_reuse_pr;
	};
	//! Get allocation mode*/
	bool	global_allocation()const
	{
		return _global_allocation;
	};
	bool	distributed_allocation()const
	{
		return _distributed_allocation;
	};
	bool	practical()const
	{
		return _practical;
	};
	bool	theoretical()const
	{
		return _theoretical;
	};
	//! Get scheduling mode*/
	bool	withreuse()const
	{
		return _withreuse;
	};
	bool	lower_bound()const
	{
		return _lower_bound;
	};
	bool	upper_bound()const
	{
		return _upper_bound;
	};
	/* end KT*/

	//! Get SOCLIB file name
	string soclib_file_name()const
	{
		return _soclib_file_name;
	}

	//! Get VHDL file name
	string vhdl_file_name()const
	{
		return _vhdl_file_name;
	}
	//! Get VHDL prefix name
	string vhdl_prefix()const
	{
		return _vhdl_prefix;
	}
	//! Get VHDL type(EJ 09/2007)
	Vhdl_type vhdl_type()const
	{
		if(_vhdl_type == "rom_regs")
			return UT_ROM;
		else if(_vhdl_type == "fsm_data")
			return UT_FSM_D;
		else if(_vhdl_type == "fsm_regs")
			return UT_FSM_R;
		return UT_FSM_S;
	}
	/* Caaliph 27/06/2007*/
	void vhdl_prefix(string vhdl_prefix)
	{
		_vhdl_prefix=vhdl_prefix;
	}
	/* End Caaliph*/
	//! Get .mem file name
	string mem_file_name()const
	{
		return _mem_file_name;
	}
	//! Get "force overwrite of files" toggle
	bool force_overwrite()const
	{
		return _force_overwrite;
	}
	//! Get GAUT environment variable name
	string gaut_var_name()const
	{
		return GAUT_VAR;
	}
	//! Get GAUT environment variable value
	string gaut_var_value()const
	{
#if 0
		const char* gaut_val = getenv(gaut_var_name().c_str());
		if(gaut_val == 0 || strlen(gaut_val) == 0)
		{
			cerr << "Error: no " << gaut_var_name()<< " variable defined" << endl;
			exit(1);
		}
		return string(gaut_val);
#else
		return gaut_var_name();
#endif
	}
	//! Get GAUT name
	string gaut_name()const
	{
		return GAUT_NAME;
	}
	//! Get GAUT version
	string gaut_version()const
	{
		return VERSION;
	}
	//! Get "dump library" toggle
	bool dumpLib()const
	{
		return _dumpLib;
	}
	//! Get "dump library" file name
	string dumpLibFile()const
	{
		return _dumpLibFile;
	}
	//! Tells if a word is a switch or not.
	//! All switches start with a "-"
	bool isAswitch(string txt)const
	{
		return(txt.c_str()[0] == '-');
	}
	bool genVHDL()const
	{
		return _genVHDL;
	}
	bool genSOCLIB()const
	{
		return _genSOCLIB;
	}
	bool genGANTT()const
	{
		return _genGANTT;
	}
	bool genMEM()const
	{
		return _genMEM;
	}

	/* EJ 26/06/2007*/
	//! Get scheduling strategy
	Scheduling_strategy scheduling_strategy()const
	{
		if(_scheduling_strategy == "force_no_pipeline")
			return FORCE_NO_PIPELINE;
		else if(_scheduling_strategy == "force_no_mobility")
			return FORCE_NO_MOBILITY;
		else if(_scheduling_strategy == "no_more_stage")
			return NO_MORE_STAGE;
		else if(_scheduling_strategy == "force_no_pipeline+min_length_fsm")
			return NO_PIPELINE_MIN_FSM;
		// Caaliph 10/09/08
		else if(_scheduling_strategy == "asap")
			return AS_SO_AS_PO;
		return DEFAULT;
	}
	//! Get operator optimization
	bool operator_optimization()const
	{
		return _operator_optimization;
	}
	//! Get "manual allocation" toggle
	bool manual_allocation()const
	{
		return _manual_allocation;
	}
	//! Get "use IHM" toggle
	bool use_IHM()const
	{
		return _use_IHM;
	}
	/* End EJ*/
};

#endif // __SWITCHES_H__

//	End of:		switches.h
