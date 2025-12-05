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

//	File:		main.cpp
//	Purpose:	GAUT top level
//	Author:		Pierre Bomel, LESTER, UBS

#include "switches.h"			// user interface
#include "timeUnits.h"			// time Units
#include "cadency.h"			// cadency
#include "clock.h"				// system clock model
#include "mem.h"				// memory model
#include "lib.h"				// library model
#include "cdfg.h"				// CDFG model
#include "check.h"				// check of objects creation
#include "selection.h"			// Selection process
#include "allocation.h"			// Allocation process
#include "scheduling.h"			// Scheduling process
#include "resizing.h"			// Resizing process
#include "reg.h"				// Registers synthesis process
#include "bus.h"				// Bus synthesis process
#include "dot_mem.h"			// .mem file generation
#include "dot_gantt.h"			// .gantt file generation
#include "ic.h"					// IC VHDL RTL model generation
//TODO_DH#include "multimode_tools.h"
#include "multimode_binding.h"
/* GL 25/10/07 : pattern(add lines)*/
#include "pattern.h"
/* Fin GL*/
/* KT 15/12/07*/
#include "local_search.h"
/* Fin KT*/
/* DH : graph to c*/
#include "cdfg2c.h"
#include "soclib.h"
#include "ordo2c.h"
/* FIN DH : graph to c*/
#include <time.h>
#include <ostream>


/* CA 10/04/08*/
#define NbGMax 100				// Maximal number of graphs; if you want to change this value, you should change
// the array indexes(equals to 100)in the Switches class
/* Fin CA*/
using namespace std;

//! Dump a CDFG node.
//! @param node is a pointer to the CDFG node to dump at console.
static void dump_node(const CDFGnode* node)
{
	if(node->type() == CDFGnode::OPERATION)
	{
		const Operation*o = (const Operation*)node;
		const OperationSchedulingState*o_ss = (OperationSchedulingState*)o->_addr;
		cout << o->name()<< " scheduled at " << o_ss->start()
			<< " on instance " << o_ss->instance->no()
			<< "/" << o_ss->instance->oper()->name()
			<< endl;
	}
}

//! GAUT data flow.
//! @param Input. are the usual C argc and argv parameters.
static void GAUT(int argc, char**argv)
{
	/* DH : measure time of phase*/
	clock_t t1,t2;


	/* Caaliph 27/06/2007*/
	vector<const Operation*> compatible_op, non_compatible_op;
	string dumpCDFGfilename[NbGMax];
	/* End Caaliph*/

	// 0 = Convert external representation to internal representation

	// Extract all the user defined parameters from the command line.
	Switches sw(argc, argv);
	// From here we are 100 % independant of the User Interface

	// Print gaut tool info
	cout << "----------------------------------------------------------------------------------------------" << endl;
	cout << sw.gaut_var_name()<< "  version : " << sw.gaut_version()<< endl;
	cout << "----------------------------------------------------------------------------------------------" << endl;

	// From user defined switches: create time unit
	TimeUnit tu(sw.time_unit());
	// From user defined switches: create clock model
	Clock clk(sw.system_clock_period());
	// From user defined switches: create cadency
	Cadency cadency(sw.cadency());
	// From user defined switches: create memory model
	Mem mem(sw.memory_access_time());
	// trace
	if(sw.verbose())
	{
		tu.info();
		clk.info();
		cadency.info();
		mem.info();
	}

	// From user defined switches: create target library from a library file
	Lib lib(sw.builtins(),			// builtins functions
			sw.lib_file_name(),		// target operators
			sw.userlib(),			// user library or ""
			/* GL 25/10/07 : pattern(add lines)*/
			sw.pattern(),
			sw.chaining(),
			/* Fin GL*/
			sw.bitwidth_aware(),		// new flow mode
			sw.verbose(),		// verbose
			/* GL 18/05/2007 : Cluster mode*/
			sw.clusteringMode(),	// clustering mode
			/* Fin GL*/
			&clk					// clock model
		   );
	// dump library
	if(sw.dumpLib())lib.serialize(sw.dumpLibFile());
	/* Caaliph 27/06/2007*/
	if(!sw.cdfgs()&& !sw.n_ordo()&& !sw.spact())
		/*******************************************************************************************************/
		/************  This part is used only for the mono-mode case(one "cdfg" is present)*******************/
		/*******************************************************************************************************/
	{
		/* End Caaliph*/



		// From user defined switches: create CDFG from a CDFG file
		if(!sw.silent_mode())
		{
			t1 = clock();
			cout << "Parsing CDFG ... " << endl;
		}
		CDFG cdfg(
				  sw.cdfg_file_name(),													// user CDFG file
				  sw.ioConstraintsFile() ? sw.ioConstraintsFileName(): "",				// optionalm IO file
				  sw.mappingConstraintsFile() ? sw.mappingConstraintsFileName(): "",		// optional map file
				  lib.functions(),														// to have access to function list
				  /* GL 25/10/07 : pattern(add lines)*/
				  lib.patterns(),
				  /* Fin GL*/
				  sw.ioConstraints(),														// ioConstraints
				  sw.mappingConstraints(),												// mappingConstraints
				  sw.bitwidth_aware(),
				  /* GL 18/05/2007 : Cluster mode*/
				  sw.clusteringMode(),	// clustering mode
				  /* Fin GL*/
				  &sw,
				  &clk																	// clock model
				 );
		if(!sw.silent_mode())
		{
			cout << "nodes = " << cdfg.nodes().size()<< endl;
			t2 = clock();
			cout << "Time used for parsing cdfg : " << t2-t1 << " ms " << endl;
		}
		//cdfg.info();

		if(sw.dumpDebug2c() == true)
		{
			t1 = clock();
			cout << "debug  CDFG2C ... " << endl;
			// DH: dump graph to c 0 = cdfg2c
			Cdfg2c Cdfg2c(&cdfg);
			t2 = clock();
			cout << "Time used for debug CDFG2C : " << t2-t1 << " ms " << endl;
		}

		// 1 = SELECTION = chose operators for operations
		if(!sw.silent_mode())
		{
			t1 = clock();
			cout << "Selection ... " << endl;
		}
		SelectionOutRefs sel;												// to store the output of the selection
		Selection s_process(&sw, &cadency, &mem, &clk, &lib, &cdfg, &sel);	// the selection process
		if(!sel.size())
		{
			cerr << "Internal error: unable to find a selection" << endl;
			exit(1);
		}
		if(!sw.silent_mode())
		{
			cout << "area = " << sel.cost()<< endl;
			t2 = clock();
			cout << "Time used for Selection : " << t2-t1 << " ms " << endl;
		}
		/* DEBUG_DH*/ if(sw.verbose()&& !sw.global_allocation())sel.info();

		// 2 = ALLOCATION = allocate operators and optimize "not used at 100% ones" with multi-function ones
		AllocationOut alloc;												// to store the output of the allocation
		if(!sw.silent_mode())
		{
			t1 = clock();
			cout << "Allocation ... " << endl;
		}
		Allocation a_process(&sw, &clk, &lib, &cdfg, &sel, &alloc);			// the allocation process
		long stages = alloc.size();
		if(!sw.silent_mode())
		{
			cout << "operators = " << alloc.operators_noPassThrough()<< ", stages = " << stages << endl;
			/* DH : 26/10/2009 cout << "CDFG latency = " << cdfg.sink()->asap()/clk.period()<<  " clock cycles " <<   endl;*/
			cout << "CDFG latency = " <<(cdfg.sink()->asap()+clk.period())/clk.period()<<  " clock cycles " <<   endl;
			t2 = clock();
			cout << "Time used for Allocation : " << t2-t1 << " ms " << endl;
		}
		if(sw.verbose())
		{
			for(int stage = 0; stage < stages; stage++)
			{
				if(!sw.global_allocation())cout << "stage " << stage << endl;
				alloc[stage].info();
			}
		}

		if(!sw.silent_mode())
		{
			t1 = clock();
			cout << "Scheduling ... " << endl;
		}
		/* EJ 26/06/2007*/
		// if "force no pipeline" or "force_no_mobility" or "no_more_stage": locked sink node
		if(sw.scheduling_strategy()!= Switches::DEFAULT)
		{
			cdfg.lockedSinkNode(cadency.length(), clk.period(), stages, sw.scheduling_strategy());
		}
		// propagate locked if there are sink node locked
		cdfg.propagateLocks();
		/* End EJ*/

		// 3 = SCHEDULING = schedule operations with IO and mapping constraints
		//cdfg.info();
		SchedulingOut sched;												// to store the output of the scheduling
		Scheduling sched_process(&sw, &cdfg, &cadency, &clk, &mem, &sel, &alloc, &lib, &sched);// scheduling process
		if(!sw.silent_mode())
		{
			cout << "operators = " << sched.operators()<< ", latency = " << cdfg.latency()<< ", stages = " << sched.stages.size()<< endl;
		}
		if(sw.verbose())
		{
			cout << "Scheduling results ... " << endl;
			sched.info();
		}
		if(!sw.silent_mode())
		{
			cout << "Scheduling summary results ... " << endl;    // EJ 09/2007
			sched.summary(&cdfg, &cadency, &clk);
			t2 = clock();
			cout << "Time used for Scheduling : " << t2-t1 << " ms " << endl;
		}

		//trace debug sched.debug_oper();

		/* DEBUG_DH selec/alloc/sched for(int i = 0; i < cdfg.nodes().size(); i++)
		   {
		   const CDFGnode*n = cdfg.nodes()[i];
		   if(n->type()!= CDFGnode::OPERATION)continue;
		   const Operation*o = (const Operation*)n;
		   const OperationSchedulingState*oss = (const OperationSchedulingState*)n->_addr; // dynamic link here
		   const OperatorInstance*inst=o->inst();
		   if(inst!=NULL)
		   cout << inst->oper()->name()<< endl;
		   }*/

		// EJ 30/04/2008 : minimise number of fsm stage
		if(sw.scheduling_strategy() == Switches::NO_PIPELINE_MIN_FSM)
		{
			long new_cadency = 0;
			for(long i = 0; i < cdfg.nodes().size(); i++)
			{
				const CDFGnode*n = cdfg.nodes()[i];
				if(n->type()!= CDFGnode::OPERATION)continue;
				const Operation*o = (const Operation*)n;
				//DH : 12/11/2008 add clock_period for output
				bool hasAOutputSuccesor=o->hasAOutputSuccessor(&cdfg);
				long end_time=o->start()+o->length();
				if(hasAOutputSuccesor==true)
					end_time+=clk.period();
				if(end_time > new_cadency)
					new_cadency = end_time;
			}
			Banks::BANKS::const_iterator b_it;			// to scan banks
			const Bank*bank;
			// scan all banks
			if(sched.banks.size())
			{
				for(b_it = sched.banks.banks.begin(); b_it != sched.banks.banks.end(); b_it++)
				{
					bank = &(*b_it).second;		// get current bank
					Bank::USES::const_iterator u_it;			// to scan banks' accesses
					const BankUse*use;
					long time;
					// scan all reads/writes of the bank
					for(u_it = bank->reads.begin(); u_it != bank->reads.end(); u_it++)
					{
						use = &(*u_it).second;	// get current bank use
						time = use->time_end();		// get end time of bank use
					}
					//DH: 11/10/2008
					if(time > new_cadency)
						new_cadency = time;

					for(u_it = bank->writes.begin(); u_it != bank->writes.end(); u_it++)
					{
						use = &(*u_it).second;	// get current bank use
						time = use->time_end();		// get end time of bank use
					}
					if(time > new_cadency)
						new_cadency = time;
				}
			}
			if(!sw.silent_mode())cout << "WARNING NEW CADENCY : " << cadency.length()<< " to ";
			if(!sw.silent_mode())cout << new_cadency << " " << endl;
			cadency.update(new_cadency);
		}





		if(sw.dumpSTAR_CDFG()|| sw.n1()|| sw.vns()|| sw.n3())

			data_WR(&cdfg, &cadency, &clk ,&mem, &sched.banks);//TO_VERIFY

		// output scheduled CDFG after HLS
		if(sw.dumpCDFG())
		{
			cout << "Dump cdfg... " << endl;
			t1 = clock();
			cdfg.serialize(sw.dumpCDFGfile());
			t2 = clock();
			cout << "Time used for cdfg dump : " << t2-t1 << " ms " << endl;
		}
		//cdfg.info();

		// WARNING
		if(!sw.silent_mode()&&(cdfg.latency()< cadency.length()))
		{
			cout << "WARNING: CDFG latency(" << cdfg.latency()<< ")< cadency(" << cadency.length()<< ")" << endl;
		}

		/* GL 10/04/2007 :	Add following lines
		   Resize operators after Binding*/
		if((sw.bitwidth_aware() == true)&&(sw.resizingMode()!= 0))
		{
			cout << "Resizing operators... " << endl;
			t1 = clock();
			Resizing r_process(&sw, &sched, &cdfg);
			//if(sw.verbose())r_process.info();
			//GL 07/09/2007 : combinatorial area
			if(!sw.silent_mode())cout << "Expected area = " << r_process.area()<< endl;
			///Fin GL
			t2 = clock();
			cout << "Time used for resizing operators : " << t2-t1 << " ms " << endl;
		}
		if((sw.bitwidth_aware() == true)&&(sw.resizingMode() == 0))
		{
			cout<< "WARNING : With new flow mode setting resizing mode to 0 can generate wrong VHDL" << endl;
		}
		/* Fin GL*/

		// 4 = REGISTERS ALLOCATION(really simple now)
		RegOut regs;														// to store the output of the register synthesis
		int nb_datas=0;
		int nb_op=0;
		if(!sw.silent_mode())
		{
			cout << "Registers allocation ... " << endl;
			t1 = clock();
		}
		RegSynthesis regs_process(&sw, &cdfg, &sched,&regs, cadency.length(), clk.period(),mem.access_time());
		if(sw.registerAllocationMode() ==Switches::REG_NONE)
			regs_process.RegisterAllocation_NONE(lib.bits());
		else if(sw.registerAllocationMode() ==Switches::REG_LeftEdge)
			regs_process.RegisterAllocation_LeftEdge(lib.bits());
		else if(sw.registerAllocationMode() ==Switches::REG_MLEA)
			regs_process.RegisterAllocation_MLEA(lib.bits());
		/* else if(sw.registerAllocationMode() ==Switches::REG_CLUSTER_MWBM)
		   regs_process.RegisterAllocation_CLUSTER_MWBM(lib.bits(),false);*/
		else
			regs_process.RegisterAllocation_CLUSTER_MWBM(lib.bits()/*,true*/);
		// registers synthesis process
		if(!sw.silent_mode())
		{
			t2 = clock();
			cout << "Time used for registers allocation : " << t2-t1 << " ms " << endl;
			//cout << regs.size()<< " registers" << endl;
		}

		//  = LOCAL SEARCH
		if((sw.n1())||(sw.vns())||(sw.n3()))
		{
			//TODO_DH	Local_search l_search(&sw, &cadency, &clk, &sched, &regs, &cdfg);
			//TODO_DH	regs =*(l_search.sol_ref()->regs());
		}

		if(!sw.silent_mode())
		{
			cout << "Data Bus allocation ... " << endl;
			t1 = clock();
		}
		// 5 = DATA BUS ALLOCATION(really simple now))
		BusOut buses(&cadency, &clk);										// to store the output of the data bus synthesis
		BusOut abuses(&cadency, &clk);
		// EJ 15/02/2008 : Dynamic access replace
		//BusSynthesis buses_process(&cadency, &clk, &mem, &cdfg, &sched, &buses, sw.ioConstraints());	// data bus synthesis process
		// by
		BusSynthesis buses_process(&cadency, &clk, &mem, &cdfg, &sched, &buses, &abuses, &sw);	// address & data bus synthesis process
		if(!sw.silent_mode())cout << buses.size()<< " data buses" << endl;
		if(!sw.silent_mode()&& abuses.size())cout << abuses.size()<< " address buses" << endl; // EJ 14/02/2008 : dynamic memory
		/* GL 11/09/08 : print info*/
		int n, in_buses = 0, IO_buses = 0;
		for(n = 0; n < buses.size(); n++)
		{
			if(buses.is_locked(n)) {
				IO_buses++;
				if(buses.BusHasAnInputAccess(n))in_buses++;
			}
		}
		if(!sw.silent_mode())cout << in_buses << " inputs" << endl;
		if(!sw.silent_mode())cout << IO_buses - in_buses << " outputs" << endl;
		/* Fin GL*/
		//update _bits with max bus width for input/ouput/memory bus
		long nb_bits;
		if(sw.bitwidth_aware() == true)
		{
			if(buses.maxBusWith()>lib.bits())//DH: 14/01/2010
				nb_bits=buses.maxBusWith();
			else
				nb_bits=lib.bits();
		}
		else
			nb_bits=lib.bits();

		/* Caaliph 03/07/2007*/
		///data_WR(&cdfg, &cadency, &clk);
		if(sw.dumpSTAR_CDFG())cdfg.serialize_STAR(sw.dumpSTAR_CDFGfile(), Tools::prefix(sw.cdfg_file_name()), &cadency);
		/* End Caaliph*/


		// Produce .mem file
		if(!sw.genMEM())
		{
			if(!sw.silent_mode())
			{
				t1 = clock();
				cout << ".mem generation ... " << endl;
			}
			// EJ 14/02/2008 : dynamic memory replace
			//DotMem dot_mem(&sw, &lib, &cdfg, &clk, &mem, &cadency, &regs, &buses); by
			DotMem dot_mem(&sw, NULL,nb_bits, &cdfg, &clk, &mem, &cadency, &regs, &buses, &abuses);
			if(!sw.silent_mode())
			{
				t2 = clock();
				cout << "Time used for .mem generation  : " << t2-t1 << " ms " << endl;
			}
		}
		if(sw.dumpDebug2c() == true)
		{
			t1 = clock();
			cout << "debug  ORDO2C ... " << endl;
			// DH: dump graph to c 0 = cdfg2c
			Ordo2c Ordo2c(&cdfg,&clk,&cadency, &buses,Tools::prefix(sw.cdfg_file_name()),false);
			t2 = clock();
			cout << "Time used for debug ORDO2C : " << t2-t1 << " ms " << endl;
		}

		if(sw.dumpDebug2c() == true)
		{
			t1 = clock();
			cout << "debug  ARCHI2C ... " << endl;
			// DH: dump ordo+reg to c
			Ordo2c Ordo2c(&cdfg,&clk,&cadency, &buses,Tools::prefix(sw.cdfg_file_name()),true);
			t2 = clock();
			cout << "Time used for debug ARCHI2C : " << t2-t1 << " ms " << endl;
		}


		//  6 = generate a BCA SOCLIB model of the synchronous IC
		if(!sw.genSOCLIB())
		{
			if(sched.stages.size()!=1)
			{
				if(!sw.silent_mode())cout << "No soclib generation with pipeline !!! " << endl;
			}
			else
			{
				if(!sw.silent_mode())
				{
					t1 = clock();
					cout << "Soclib generation ... " << endl;
				}
				Soclib Soclib(&sw,&cdfg,&clk,&cadency, &buses,sw.soclib_file_name());
				if(!sw.silent_mode())
				{
					t2 = clock();
					cout << "Time used for soclib generation : " << t2-t1 << " ms " << endl;
				}
			}

		}
		// 7 = generate a VHDL RTL model of the synchronous IC
		if(!sw.genVHDL())
		{
			if(!sw.silent_mode())
			{
				t1 = clock();
				cout << ".vhd generation ... " << endl;
			}
			IC ic(&sw, &mem, &cadency, &cdfg, &clk, &buses, &abuses, &regs, &sched, nb_bits);
			if(!sw.silent_mode())
			{
				t2 = clock();
				cout << "Time used for .vhd generation   : " << t2-t1 << " ms " << endl;
			}
		}

		// Produce UT.gantt file
		if(!sw.genGANTT())
		{
			if(!sw.silent_mode())
			{
				t1 = clock();
				cout << ".gantt generation ... " << endl;
			}
			DotGantt dot_gantt(&sw, &cdfg, &clk, &cadency, &regs, &buses);
			if(!sw.silent_mode())
			{
				t2 = clock();
				cout << "Time used for .gantt generation   : " << t2-t1 << " ms " << endl;
			}
		}

		//
		// the GAUT_COST_EXTENSION should be the same as in the
		// TDS environment variable gaut_cost_extension
		//
		const char* GAUT_COST_EXTENSION = ".gcost";
		string tds = Tools::prefix(sw.cdfg_file_name())+GAUT_COST_EXTENSION;
		ofstream ofile(tds.c_str());
		if(ofile.is_open()) {
			ofile << "#######################################" << endl;
			ofile << "# GAUT Cost generated for TDS.        #" << endl;
			ofile << "# Daniel Gomez-Prado                  #" << endl;
			ofile << "#######################################" << endl;
			ofile << "# " << tds << endl;
			//cdfg
			ofile << "cdfg_latency = " <<(cdfg.sink()->asap()+clk.period())/clk.period()<<   endl;
			//scheduling summary
			sched.summary(&cdfg, &cadency, &clk,ofile);
			ofile << "mux2to1_io_instance = " << sched.NumberOfMux2to1InputOperatorInstance()<< endl;
			//register allocation
			int mux2to1 = regs_process._reg_out->nbMux2to1(sched.max_operator_no())+sched.NumberOfMux2to1InputOperatorInstance();
			ofile << "mux2to1 = " << mux2to1 << endl;
			ofile << "ffs = " << regs_process._reg_out->nbFlipFlop()<< endl;
			ofile.close();
		}

		return;

		//	return; // End process GAUT




		/*******************************************************************************************************/

		/**********  This part is used only for the multimode case(several "cdfgs" are presents)**************/

		/*******************************************************************************************************/

	}
	else
	{

		// Caaliph 14/12/09

		if(sw.multimode_case() == "union")// Flow with unified allocation

			multimode_union(argc, argv, sw, clk, tu, cadency, mem, lib);

		else // Flow with incremental allocation

			multimode_incremental(argc, argv, sw, clk, tu, cadency, mem, lib);

		// End Caaliph 14/12/09
	}
	return;

	/*******************************************************************************************************/

	/**************************************  End of multimode**********************************************/

	/*******************************************************************************************************/


}

//! GAUT main entry point.
//! @param Input. are the usual C argc and argv parameters.
int main(int argc, char**argv)
{
	GAUT(argc, argv);
	return 0;
	//checkDump();
}

//	End of:		main.cpp

