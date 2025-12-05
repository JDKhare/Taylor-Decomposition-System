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


/* CA 10/04/08*/
#define NbGMax 100				// Maximal number of graphs; if you want to change this value, you should change
// the array indexes(equals to 8)in the Switches class
/* Fin CA*/
using namespace std;

// Begin of multimode with union allocation
//! multimode with union allocation data flow.
//! @param Input. are the usual C argc and argv parameters.
void multimode_union(int argc, char**argv, Switches sw, Clock clk, TimeUnit tu, Cadency cadency, Mem mem, Lib lib)
{
	/* DH : measure time of phase*/
		clock_t t1,t2;

		/* Caaliph 27/06/2007*/
		vector<const Operation*> compatible_op, non_compatible_op;
		string dumpCDFGfilename[NbGMax];
		/* End Caaliph*/

		// 0 = Convert external representation to internal representation
		/* Caaliph 27/06/2007*/
		// From user defined switches: create CDFG from a CDFG file
		/****** Parsing of the all cdfgs********/
		if(!sw.silent_mode())cout << "Parsing CDFG ... "<<endl;
			CDFG*cdfg_s[NbGMax];
				long NbOps[NbGMax];
				int nb_graphes=0;
				bool sw_cdfgs_modif=false;
				bool sw_nordo_modif=false;
				bool sw_spact_modif=false;
				int i;
				for(i=0; i<NbGMax; i++)
				{
					if(sw._cdfgs_file_name[i]!="")
					{
						nb_graphes++;
							cdfg_s[i]= new CDFG(
												sw._cdfgs_file_name[i],													// user CDFG file
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
							if(!sw.silent_mode())cout <<sw._cdfgs_file_name[i]<<" nodes = " << cdfg_s[i]->nodes().size()<< endl;
								NbOps[i]=cdfg_s[i]->nodes().size();
					}
					else i=NbGMax-1;
				}

		/***	DFGs pre-scheduling***/
		long max_cadence;
		AllocationOutStep allocsRes; // RRT
	if(sw.cdfgs())
	{

			CDFG*cdfg_sched[NbGMax];
			FunctionRefs used_functions;
			for(i=0; i<nb_graphes; i++)
			{
				cdfg_s[i]->extract_list_of_used_functions(&used_functions);

					Cadency cadencies(sw._cadencys[i]);
					// 1 = SELECTION = chose operators for operations

					SelectionOutRefs sel0;
					/* to store the output of the selection*/
					Selection s_process0(&sw, &cadencies, &mem, &clk, &lib, cdfg_s[i], &sel0);	// the selection process
				if(!sel0.size())
				{
					cout << "Internal error: unable to find a selection" << endl;
						exit(1);
				}

					// propagate locked if there are sink node locked
					cdfg_s[i]->propagateLocks();

					// 2 = ALLOCATION = allocate operators and optimize "not used at 100% ones" with multi-function ones
					AllocationOut alloc0;												// to store the output of the allocation

					Allocation a_process0(&sw, &clk, &lib, cdfg_s[i], &sel0, &alloc0);			// the allocation process
				long stages = alloc0.size();

					/* EJ 26/06/2007*/
					// if "force no pipeline" or "force_no_mobility" or "no_more_stage": locked sink node
					if(sw.scheduling_strategy()!= Switches::DEFAULT)
					{
						cdfg_s[i]->lockedSinkNode(cadencies.length(), clk.period(), stages, sw.scheduling_strategy());
					}
				// propagate locked if there are sink node locked
				cdfg_s[i]->propagateLocks();
					/* End EJ*/

					// 3 = SCHEDULING = schedule operations with IO and mapping constraints
					//cdfg_s[0]->info();
					if(sw.cdfgs())
					{
						sw.cdfgs(0);
							sw_cdfgs_modif=true;
					}
				if(sw.n_ordo())
				{
					sw.n_ordo(0);
						sw_nordo_modif=true;
				}
				if(sw.spact())
				{
					sw.spact(0);
						sw_spact_modif=true;
				}
				SchedulingOut sched0;
					// to store the output of the scheduling
					Scheduling sched_process0(&sw, cdfg_s[i], &cadencies, &clk, &mem, &sel0, &alloc0/*&allocRes*/, &lib, &sched0);// scheduling process

					// Generate the dump main cdfg_s[0]: the dump file is used to create the resource reservation table(RRT)
					dumpCDFGfilename[i] = Tools::prefix(sw._cdfgs_file_name[i])+"_out";
					cdfg_s[i]->serialize(dumpCDFGfilename[i]+".sched", sw._mode_numbers[i]);

					// Parsing of the dump file obtained by the processing of the primary CDFG
					cdfg_sched[i]= new CDFG(
											dumpCDFGfilename[i]+".sched",													// user CDFG file
											sw.ioConstraintsFile() ? sw.ioConstraintsFileName(): "",				// optionalm IO file
											sw.mappingConstraintsFile() ? sw.mappingConstraintsFileName(): "",		// optional map file
											lib.functions(),										// to have access to function list
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
					OperatorInstance(0, &cadencies, &clk);
			}

			/*** End DFGs pre-scheduling***/

			/*** RRTs generation combined with their union***/
			long max_cadency=0;

			for(i=0; i<nb_graphes; i++)
			{
				Cadency cadency_max(sw._cadencys[i]);
					if(cadency_max.length()>max_cadency)
						max_cadency=cadency_max.length();
			}
		Cadency cadencys_max(max_cadency);
			sw.cadency_prim_val(cadencys_max.length());
			rrt_construction(cdfg_sched[0], &allocsRes, &cadencys_max, &clk, &sw);

			// RRTs union
			for(i=1; i<nb_graphes; i++)
			{
				AllocationOutStep allocsCur;
					sw.cadency_prim_val(sw._cadencys[i]);
					rrt_construction(cdfg_sched[i], &allocsCur, &cadencys_max, &clk, &sw);
					for(int cycle = 0; cycle < cadencys_max.length()/clk.period(); cycle++)
					{

							AllocationOutCycle*mc = &allocsCur.allocationOutCycle()[cycle];
							AllocationOutCycle*mc1 = &allocsRes.allocationOutCycle()[cycle];
							FunctionRefs::const_iterator f_it;			// Functions iterator
						for(f_it = used_functions.begin(); f_it != used_functions.end(); f_it++)
						{
							Function*f = (*f_it).second;			// get list element
							if(f->passThrough())continue;
								int Nbfunction_Cur=0;
									int Nbfunction_Res=0;
									long nopsCur = mc->operators().size();				// get number of autres choses
							long nopsRes = mc1->operators().size();				// get number of autres choses
							long nops=nopsCur;
								if(nops<nopsRes)nops=nopsRes;
									for(int k = 0; k < nops; k++)// for all instances
									{
										if(nopsCur>0)
										{
											Operator*oCur = (Operator*)mc->operators()[k];	// get operator identity
											if(oCur->name() == (f->name()+"_op"))
												Nbfunction_Cur++;
													nopsCur--;
										}
										if(nopsRes>0)
										{
											Operator*oRes = (Operator*)mc1->operators()[k];	// get operator identity
											if(oRes->name() == (f->name()+"_op"))
												Nbfunction_Res++;
													nopsRes--;
										}
									}
							int Nbfunction_to_add=Nbfunction_Cur-Nbfunction_Res;
								while(Nbfunction_to_add>0)
								{
									OperatorRefs::const_iterator it;
										for(it = f->implemented_by().begin(); it!= f->implemented_by().end(); it++)
										{
											OperatorRef op_ref = (*it).second;
												const Operator*op = op_ref;
												if(op->isMultiFunction())continue;
													vector<const Operator*>*AOC_op_v = &mc1->operators();
														AOC_op_v->push_back(op);
										}
									Nbfunction_to_add--;
								}
						}

					}
			}
		/*** End RRT Union***/

			cout<<endl;

			max_cadence = cadencys_max.length();

			/*** DFGs sorting***/
			for(i=0; i<nb_graphes; i++)
			{
				cdfg_s[i]= new CDFG(
									sw._cdfgs_file_name[i],													// user CDFG file
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
			}
		for(i=0; i<nb_graphes-1; i++)
			get_compatibles_op(cdfg_s[i], cdfg_s[i+1], &compatible_op, &non_compatible_op);
				for(i=0; i<non_compatible_op.size(); i++)
					erase_element(&compatible_op, non_compatible_op[i]);
						/****** Order the DFG in function of the priority array******/
						float priority_order[NbGMax];
						long NbOps_comp[NbGMax];
						int NbOps_type_comp;
						int opr_latency;
						for(i=0; i<nb_graphes; i++)
						{
							priority_order[i]=0;
								Cadency cadencyes(sw._cadencys[i]);
								NbOps_comp[i]=get_ops_comp(cdfg_s[i], &compatible_op);
								SelectionOutRefs sel;
								/* to store the output of the selection*/
								Selection s_process0(&sw, &cadencyes, &mem, &clk, &lib, cdfg_s[i], &sel);	// the selection process
							FunctionRefs used_functions;
								cdfg_s[i]->extract_list_of_used_functions(&used_functions);
								FunctionRefs::const_iterator f_it;			// Functions iterator
							for(f_it = used_functions.begin(); f_it != used_functions.end(); f_it++)
							{
								Function*f = (*f_it).second;			// get list element
								int n_it;
									NbOps_type_comp=0;
									vector<CDFGnode*> v_node = cdfg_s[i]->nodes();
									for(n_it=0; n_it < v_node.size(); n_it++)
									{
										const CDFGnode*node = v_node[n_it];
											if(node->type() ==CDFGnode::OPERATION)
											{
												if(((Operation*)node)->function()->passThrough())continue;
													if(((Operation*)node)->function()->name() ==f->name())
													{
														if(((Operation*)node)->compatible_ops())
														{
															NbOps_type_comp++;
																opr_latency= ((Operation*)node)->cycles()*clk.period();
														}
													}
											}
									}
								const double C = cadencyes.length();
									priority_order[i]=priority_order[i]+(float)(ceil((double)(NbOps_type_comp)/floor(C/(double)(opr_latency))));
							}

						}
		// Caaliph : 15/04/08
		tri_bulle(priority_order,NbOps_comp, NbOps, cdfg_s, sw._cdfgs_file_name, sw._cadencys, sw._modes, sw._mode_numbers, sw._vhdl_file_names,
				  sw._vhdl_prefixs, sw._mem_file_names, nb_graphes); // Sort the CDFG by the decreasing priority
		cout<<endl;
			// Fin Caaliph
			/****  End DFGs sorting****/
	}

		CDFG*cdfg_prim[NbGMax];
		if(sw_cdfgs_modif)sw.cdfgs(1);
			if(sw_nordo_modif)sw.n_ordo(1);
				if(sw_spact_modif)sw.spact(1);
					/****** This part consists in the mutimode scheduling and operation binding of the CDFGs******/
						for(i=0; i<nb_graphes; i++)
						{
							AllocationOut alloc_s;
								SelectionOutRefs sel_s;
								SchedulingOut sched_s;
								RegOut regs_s;

								Cadency cadencys(sw._cadencys[i]);
								cout<<endl;
								cout<<sw._cdfgs_file_name[i]<<" Processing ...."<<endl<<endl;
								delay_init(cdfg_s[i]);
								//cdfg.info();
								propagate_mode(cdfg_s[i], sw._modes[i]);
								// 1 = SELECTION = chose operators for operations
								if(!sw.silent_mode())cout << "Selection ... ";
									///	SelectionOutRefs sel;												// to store the output of the selection
									Selection s_process(&sw, &cadencys, &mem, &clk, &lib, cdfg_s[i], &sel_s);	// the selection process
							if(!sel_s.size())
							{
								cout << "Internal error: unable to find a selection" << endl;
									exit(1);
							}
							if(!sw.silent_mode())cout << "area = " << sel_s.cost()<< endl;
								if(sw.verbose())sel_s.info();

										// 2 = ALLOCATION = allocate operators and optimize "not used at 100% ones" with multi-function ones
										///	AllocationOut alloc;												// to store the output of the allocation
										if(!sw.silent_mode())cout << "Allocation ... ";
											Allocation a_process(&sw, &clk, &lib, cdfg_s[i], &sel_s, &alloc_s);			// the allocation process
							long stages = alloc_s.size();
								if(!sw.silent_mode())
								{
									cout << "operators = " << alloc_s.operators_noPassThrough()<< ", stages = " << stages << endl;
										cout << sw._cdfgs_file_name[i]<<" latency = " << cdfg_s[i]->sink()->asap()<< endl;
								}
							if(sw.verbose())
							{
								for(int stage = 0; stage < stages; stage++)
								{
									cout << "stage " << stage << endl;
										alloc_s[stage].info();
								}
							}

								/* EJ 26/06/2007*/
								// if "force no pipeline" or "force_no_mobility" or "no_more_stage": locked sink node
								if(sw.scheduling_strategy()!= Switches::DEFAULT)
								{
									cdfg_s[i]->lockedSinkNode(cadencys.length(), clk.period(), stages, sw.scheduling_strategy());
								}
							// propagate locked if there are sink node locked
							cdfg_s[i]->propagateLocks();
								/* End EJ*/

								// 3 = SCHEDULING = schedule operations with IO and mapping constraints
								//cdfg.info();
								// to store the output of the scheduling

								if(sw.n_ordo()||sw.spact())
								{
									if(!sw.silent_mode())cout << "Scheduling of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
										Scheduling sched_process(&sw, cdfg_s[i], &cadencys, &clk, &mem, &sel_s, &alloc_s, &lib, &sched_s);// scheduling process
								}
								else
								{
									if(!sw.silent_mode())cout << "Scheduling of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
										Scheduling sched_process(&sw, cdfg_s[i], &cadencys, max_cadence, &clk, &mem, &sel_s, &alloc_s, &allocsRes, &sched_s);// scheduling process
								}

								// Trouver le max entre les cadences précedantes et celle courante
								if(cadencys.length()>max_cadence)max_cadence=cadencys.length();
									// Generate the dump secondary cdfg with the approach considering the scheduling and the MWBM binding
									//and the the approach condiring only the MWBM binding
									vector<const Operation*> bound_operations_temp, bound_operations;
										vector<const CDFGnode*> bound_oper, datapath_sharing_temp, datapath_sharing;
										int previous_graphs=i;
										/* Caaliph 03/07/2007*/
										data_WR(cdfg_s[i], &cadencys, &clk, &mem, &sched_s.banks);
										dumpCDFGfilename[i]=Tools::prefix(sw._cdfgs_file_name[i])+"_out";
										if(i==0)
										{
											if(sw.cdfgs())
											{
												// First DFG binding
												if(!sw.silent_mode())cout << "Binding of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
													one_dfg_binding(cdfg_s[i], &clk, &cadencys, sched_s.stages.size(), sw.binding_case(), &bound_operations);
											}
											else
											{
												cdfg_s[i]->serialize(dumpCDFGfilename[i]+".sched", sw._mode_numbers[i]);
													cdfg_s[i]->serialize_STAR(dumpCDFGfilename[i]+".tmp", Tools::prefix(sw._cdfgs_file_name[i]), &cadencys, sw._mode_numbers[i]);
											}
										}
										else
										{
											if(sw.cdfgs()|| sw.n_ordo())
											{
												if(!sw.silent_mode())cout << "Binding of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
													dfgs_binding(cdfg_s[i], cdfg_prim, previous_graphs, &clk, &cadencys, sw._cadencys, sched_s.stages.size(), sw.binding_case(), &bound_operations);
											}
											if(sw.spact())
											{
												if(!sw.silent_mode())cout << "Binding of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
													dfgs_binding(cdfg_s[i], cdfg_prim, previous_graphs, &clk, &cadencys, sw._cadencys, sched_s.stages.size(), "mwmr_global", &bound_operations);
											}
										}
							for(int j=0; j<bound_operations.size(); j++)
							{
								bound_oper.push_back(((CDFGnode*)bound_operations[j]));
							}
							/* Caaliph 03/07/2007*/
								if(i>0 || sw.cdfgs())
								{
									cdfg_s[i]->serialize(dumpCDFGfilename[i]+".sched", &bound_oper, sw._mode_numbers[i]);
										cdfg_s[i]->serialize_STAR(dumpCDFGfilename[i]+".tmp", Tools::prefix(sw._cdfgs_file_name[i]), &cadencys, &bound_oper, sw._mode_numbers[i]);
								}
							/// The scheduling result is filled here to be used for VHDL generation
							fill_scheduling_out(cdfg_s[i], &sched_s, &cadencys, &clk);
								if(!sw.silent_mode())
								{
									cout <<", latency = " << cdfg_s[i]->latency()<< endl;
								}
							if(sw.verbose())
							{
								cout << "Scheduling results ... " << endl;
									sched_s.info();
							}

								// WARNING
								if(!sw.silent_mode()&&(cdfg_s[i]->latency()< cadencys.length()))
								{
									cout << "WARNING: "<<sw._cdfgs_file_name[i]<<" latency(" << cdfg_s[i]->latency()<< ")< cadency(" << cadencys.length()<< ")" << endl;
								}
							cdfg_prim[i]= new CDFG(
												   dumpCDFGfilename[i]+".sched",													// user CDFG file
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
								propagate_mode(cdfg_prim[i], sw._modes[i]);
								// 4 = REGISTERS ALLOCATION(really simple now)
								//RegOut regs;														// to store the output of the register synthesis
								int nb_datas=0;
								int nb_op=0;
								RegSynthesis regs_process(&sw, cdfg_s[i], &sched_s,&regs_s, cadencys.length(), clk.period(),mem.access_time());
								regs_process.RegisterAllocation_NONE(lib.bits());

								// 5 = BUS ALLOCATION(really simple now))
								BusOut buses_s(&cadencys, &clk);  // to store the output of the bus synthesis
							BusOut abuses_s(&cadencys, &clk);
								if(!sw.silent_mode())cout << "Data Bus allocation ... ";
									// EJ 15/02/2008 : Dynamic access replace
									//BusSynthesis buses_process(&cadency, &clk, &mem, &cdfg, &sched, &buses, sw.ioConstraints());	// data bus synthesis process
									// by
									BusSynthesis buses_process(&cadencys, &clk, &mem, cdfg_s[i], &sched_s, &buses_s, &abuses_s, &sw);	// address & data bus synthesis process
							if(!sw.silent_mode())cout << buses_s.size()<< " data buses" << endl;
								if(!sw.silent_mode()&& abuses_s.size())cout << abuses_s.size()<< " address buses" << endl; // EJ 14/02/2008 : dynamic memory
							/*if(!sw.silent_mode())cout << "Bus allocation ... ";
							  BusSynthesis buses_process(&cadencys, &clk, &mem, cdfg_s[i], &sched_s, &buses_s, sw.ioConstraints());	// bus synthesis process
							  if(!sw.silent_mode())cout << buses_s.size()<< " buses" << endl;*/
							//update _bits with max bus width for input/ouput/memory bus
							long nb_bits;
								if(sw.bitwidth_aware() == true)
									nb_bits=buses_s.maxBusWith();
								else
									nb_bits=lib.bits();

										// Produce .mem file
										if(!sw.genMEM())
										{
											if(!sw.silent_mode())cout << ".mem generation ... ";
												DotMem dot_mems(&sw, &sw._mem_file_names[i], nb_bits, cdfg_s[i], &clk, &mem, &cadencys, &regs_s, &buses_s, &abuses_s);
													if(!sw.silent_mode())cout << "OK" << endl;
										}
						}
}
// End of multimode with union allocation

// Begin of multimode with incremental allocation
//! multimode with incremental allocation data flow.
//! @param Input. are the usual C argc and argv parameters.
void multimode_incremental(int argc, char**argv, Switches sw, Clock clk, TimeUnit tu, Cadency cadency, Mem mem, Lib lib)
{
	/* DH : measure time of phase*/
		clock_t t1,t2;

		/* Caaliph 27/06/2007*/
		vector<const Operation*> compatible_op, non_compatible_op;
		string dumpCDFGfilename[NbGMax];
		/* End Caaliph*/
		// 0 = Convert external representation to internal representation
		/* Caaliph 27/06/2007*/
		// From user defined switches: create CDFG from a CDFG file
		/****** Parsing of the all cdfgs********/
		if(!sw.silent_mode())cout << "Parsing CDFG ... "<<endl;
			CDFG*cdfg_s[NbGMax];
				long NbOps[NbGMax];
				int nb_graphes=0;
				bool sw_cdfgs_modif=false;
				bool sw_nordo_modif=false;
				bool sw_spact_modif=false;
				int i;
				for(i=0; i<NbGMax; i++)
				{
					if(sw._cdfgs_file_name[i]!="")
					{
						nb_graphes++;
							cdfg_s[i]= new CDFG(
												sw._cdfgs_file_name[i],													// user CDFG file
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
							if(!sw.silent_mode())cout <<sw._cdfgs_file_name[i]<<" nodes = " << cdfg_s[i]->nodes().size()<< endl;
								NbOps[i]=cdfg_s[i]->nodes().size();
					}
					else i=NbGMax-1;
				}
	//if(!sw.silent_mode())cout << "nodes = " << cdfg_s[0]->nodes().size()<< endl;
	if(sw.cdfgs())
	{
		/****** get the compatible and incompatible functions******/
			for(i=0; i<nb_graphes-1; i++)
				get_compatibles_op(cdfg_s[i], cdfg_s[i+1], &compatible_op, &non_compatible_op);
					for(i=0; i<non_compatible_op.size(); i++)
						erase_element(&compatible_op, non_compatible_op[i]);
							/****** Order the DFG in function of the priority array******/
							float priority_order[NbGMax];
							long NbOps_comp[NbGMax];
							int NbOps_type_comp;
							int opr_latency;
							for(i=0; i<nb_graphes; i++)
							{
								priority_order[i]=0;
									Cadency cadencyes(sw._cadencys[i]);
									NbOps_comp[i]=get_ops_comp(cdfg_s[i], &compatible_op);
									SelectionOutRefs sel;
									/* to store the output of the selection*/
									Selection s_process0(&sw, &cadencyes, &mem, &clk, &lib, cdfg_s[i], &sel);	// the selection process
								FunctionRefs used_functions;
									cdfg_s[i]->extract_list_of_used_functions(&used_functions);
									FunctionRefs::const_iterator f_it;			// Functions iterator
								for(f_it = used_functions.begin(); f_it != used_functions.end(); f_it++)
								{
									Function*f = (*f_it).second;			// get list element
									int n_it;
										NbOps_type_comp=0;
										vector<CDFGnode*> v_node = cdfg_s[i]->nodes();
										for(n_it=0; n_it < v_node.size(); n_it++)
										{
											const CDFGnode*node = v_node[n_it];
												if(node->type() ==CDFGnode::OPERATION)
												{
													if(((Operation*)node)->function()->passThrough())continue;
														if(((Operation*)node)->function()->name() ==f->name())
														{
															if(((Operation*)node)->compatible_ops())
															{
																NbOps_type_comp++;
																	opr_latency= ((Operation*)node)->cycles()*clk.period();
															}
														}
												}
										}
									const double C = cadencyes.length();
										priority_order[i]=priority_order[i]+(float)(ceil((double)(NbOps_type_comp)/floor(C/(double)(opr_latency))));
								}

							}
		tri_bulle(priority_order,NbOps_comp, NbOps, cdfg_s, sw._cdfgs_file_name, sw._cadencys, sw._modes, sw._mode_numbers, sw._vhdl_file_names,
				  sw._vhdl_prefixs, sw._mem_file_names, nb_graphes); // Sort the CDFG by the decreasing priority
		cout<<endl;
	}
	/****** This part consist in the selection, allocation, so and so of the primary DFG******/
		cout<<sw._cdfgs_file_name[0]<<" Processing ...."<<endl<<endl;
		Cadency cadencys(sw._cadencys[0]);
		// 1 = SELECTION = chose operators for operations
		if(!sw.silent_mode())cout << "Selection ... ";
			SelectionOutRefs sel0;
				/* to store the output of the selection*/
				Selection s_process0(&sw, &cadencys, &mem, &clk, &lib, cdfg_s[0], &sel0);	// the selection process
	if(!sel0.size())
	{
		cerr << "Internal error: unable to find a selection" << endl;
			exit(1);
	}
	if(!sw.silent_mode())cout << "area = " << sel0.cost()<< endl;
		if(sw.verbose())sel0.info();

				// propagate locked if there are sink node locked
				cdfg_s[0]->propagateLocks();

				// 2 = ALLOCATION = allocate operators and optimize "not used at 100% ones" with multi-function ones
				AllocationOut alloc0;												// to store the output of the allocation
	if(!sw.silent_mode())cout << "Allocation ... ";
		Allocation a_process0(&sw, &clk, &lib, cdfg_s[0], &sel0, &alloc0);			// the allocation process
	long stages = alloc0.size();
		if(!sw.silent_mode())
		{
			cout << "operators = " << alloc0.operators_noPassThrough()<< ", stages = " << stages << endl;
				cout <<sw._cdfgs_file_name[0]<<" latency = " << cdfg_s[0]->sink()->asap()<< endl;
		}
	if(sw.verbose())
	{
		for(int stage = 0; stage < stages; stage++)
		{
			cout << "stage " << stage << endl;
				alloc0[stage].info();
		}
	}

		/* EJ 26/06/2007*/
		// if "force no pipeline" or "force_no_mobility" or "no_more_stage": locked sink node
		if(sw.scheduling_strategy()!= Switches::DEFAULT)
		{
			cdfg_s[0]->lockedSinkNode(cadencys.length(), clk.period(), stages, sw.scheduling_strategy());
		}
	// propagate locked if there are sink node locked
	cdfg_s[0]->propagateLocks();
		/* End EJ*/

		// 3 = SCHEDULING = schedule operations with IO and mapping constraints
		//cdfg_s[0]->info();
		if(sw.cdfgs())
		{
			sw.cdfgs(0);
				sw_cdfgs_modif=true;
		}
	if(sw.n_ordo())
	{
		sw.n_ordo(0);
			sw_nordo_modif=true;
	}
	if(sw.spact())
	{
		sw.spact(0);
			sw_spact_modif=true;
	}
	SchedulingOut sched0;
		// to store the output of the scheduling
		if(!sw.silent_mode())cout << "Scheduling ... ";
			Scheduling sched_process0(&sw, cdfg_s[0], &cadencys, &clk, &mem, &sel0, &alloc0, &lib, &sched0);// scheduling process
	if(!sw.silent_mode())
	{
		cout << "operators = " << sched0.operators()<< ", latency = " << cdfg_s[0]->latency()<< ", stages = " << sched0.stages.size()<< endl;
	}
	if(sw.verbose())
	{
		cout << "Scheduling results ... " << endl;
			sched0.info();
	}
	/* Caaliph 03/07/2007*/
		data_WR(cdfg_s[0], &cadencys, &clk, &mem, &sched0.banks);
		// Generate the dump secondary cdfg with the approach considering the scheduling and the MWBM binding
		//and the the approach condiring only the MWBM binding
		vector<const Operation*> bound_operations_temp, bound_operations;
		vector<const CDFGnode*> bound_oper, datapath_sharing_temp, datapath_sharing;
		// Generate the dump main cdfg_s[0]: the dump file is used to create the resource reservation table(RRT)
		dumpCDFGfilename[0] = Tools::prefix(sw._cdfgs_file_name[0])+"_out";
		if(sw.cdfgs())
		{
			// First DFG binding
			if(!sw.silent_mode())cout << "Binding of "<<sw._cdfgs_file_name[0]<<"..."<<endl;
				one_dfg_binding(cdfg_s[0], &clk, &cadencys, sched0.stages.size(), sw.binding_case(), &bound_operations);
		}
		else
		{
			cdfg_s[0]->serialize(dumpCDFGfilename[0]+".sched", sw._mode_numbers[0]);
				cdfg_s[0]->serialize_STAR(dumpCDFGfilename[0]+".tmp", Tools::prefix(sw._cdfgs_file_name[0]), &cadencys, sw._mode_numbers[0]);
		}
	// WARNING
	if(!sw.silent_mode()&&(cdfg_s[0]->latency()< cadencys.length()))
	{
		cout << "WARNING: "<<sw._cdfgs_file_name[0]<<" latency(" << cdfg_s[0]->latency()<< ")< cadency(" << cadencys.length()<< ")" << endl;
	}
	/// The scheduling result is filled here to be used for VHDL generation
	fill_scheduling_out(cdfg_s[0], &sched0, &cadencys, &clk);
		// 4 = REGISTERS ALLOCATION(really simple now)
		sw.vhdl_prefix(sw._vhdl_prefixs[0]);
		RegOut regs0;														// to store the output of the register synthesis
	// 4 = REGISTERS ALLOCATION(really simple now)
	//RegOut regs;														// to store the output of the register synthesis
	int nb_datas=0;
		int nb_op=0;
		RegSynthesis regs_process(&sw, cdfg_s[0], &sched0,&regs0, cadencys.length(), clk.period(),mem.access_time());
		regs_process.RegisterAllocation_NONE(lib.bits());

		// 5 = BUS ALLOCATION(really simple now))
		BusOut buses0(&cadencys, &clk);  // to store the output of the bus synthesis
	BusOut abuses0(&cadencys, &clk);
		if(!sw.silent_mode())cout << "Data Bus allocation ... ";
			// EJ 15/02/2008 : Dynamic access replace
			//BusSynthesis buses_process(&cadency, &clk, &mem, &cdfg, &sched, &buses, sw.ioConstraints());	// data bus synthesis process
			// by
			BusSynthesis buses_process(&cadencys, &clk, &mem, cdfg_s[0], &sched0, &buses0, &abuses0, &sw);	// address & data bus synthesis process
	if(!sw.silent_mode())cout << buses0.size()<< " data buses" << endl;
		if(!sw.silent_mode()&& abuses0.size())cout << abuses0.size()<< " address buses" << endl; // EJ 14/02/2008 : dynamic memory
	/*if(!sw.silent_mode())cout << "Bus allocation ... ";
	  BusSynthesis buses_process(&cadencys, &clk, &mem, cdfg_s[i], &sched_s, &buses_s, sw.ioConstraints());	// bus synthesis process
	  if(!sw.silent_mode())cout << buses_s.size()<< " buses" << endl;*/
	//update _bits with max bus width for input/ouput/memory bus
	long nb_bits;
		if(sw.bitwidth_aware() == true)
			nb_bits=buses0.maxBusWith();
		else
			nb_bits=lib.bits();

				// Produce .mem file
				if(!sw.genMEM())
				{
					if(!sw.silent_mode())cout << ".mem generation ... ";
						DotMem dot_mems(&sw, &sw._mem_file_names[0], nb_bits, cdfg_s[0], &clk, &mem, &cadencys, &regs0, &buses0, &abuses0);
							if(!sw.silent_mode())cout << "OK" << endl;
				}

		CDFG*cdfg_prim[NbGMax];
		// Parsing of the dump file obtained by the processing of the primary CDFG
		cdfg_prim[0]= new CDFG(
							   dumpCDFGfilename[0]+".sched",													// user CDFG file
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

		sw.cadency_prim_val(cadencys.length());
		AllocationOutStep allocs; // RRT
	if(sw_cdfgs_modif)sw.cdfgs(1);
		if(sw_nordo_modif)sw.n_ordo(1);
			if(sw_spact_modif)sw.spact(1);
				//Search the cadency max
				long max_cadency=0;
					for(i=0; i<nb_graphes; i++)
					{
						Cadency cadency_max(sw._cadencys[i]);
							if(cadency_max.length()>max_cadency)
								max_cadency=cadency_max.length();
					}

		propagate_mode(cdfg_prim[0], sw._modes[0]);
		Cadency cadencys_max(max_cadency);
		//Resource reservation table construction(only for multimode case)
		if(sw.cdfgs())
		{
			rrt_construction(cdfg_prim[0], &allocs, &cadencys_max, &clk, &sw);
				/*	for(int cstep=0; cstep<max_cadency/clk.period(); cstep++) {
					long nbOpr=allocs.allocationOutCycle().at(cstep).operators().size();
					cout<<allocs.allocationOutCycle().at(cstep).operators().size()<<"(";
					for(int nb=0; nb<nbOpr; nb++) {
					vector<const Operator*>::iterator it;
					cout<<allocs.allocationOutCycle().at(cstep).operators()[nb]->name()<<";";
					}
					cout<<")";
					}
					cout<<endl;*/
		}
	long max_cadence = cadencys.length();
		/****** This part consists in the processing of the remainder CDFG******/
		for(i=1; i<nb_graphes; i++)
		{
			AllocationOut alloc_s;
				SelectionOutRefs sel_s;
				SchedulingOut sched_s;
				RegOut regs_s;

				Cadency cadencys(sw._cadencys[i]);
				cout<<endl;
				cout<<sw._cdfgs_file_name[i]<<" Processing ...."<<endl<<endl;
				delay_init(cdfg_s[i]);
				//cdfg.info();
				propagate_mode(cdfg_s[i], sw._modes[i]);
				// 1 = SELECTION = chose operators for operations
				if(!sw.silent_mode())cout << "Selection ... ";
					///	SelectionOutRefs sel;												// to store the output of the selection
					Selection s_process(&sw, &cadencys, &mem, &clk, &lib, cdfg_s[i], &sel_s);	// the selection process
			if(!sel_s.size())
			{
				cerr << "Internal error: unable to find a selection" << endl;
					exit(1);
			}
			if(!sw.silent_mode())cout << "area = " << sel_s.cost()<< endl;
				if(sw.verbose())sel_s.info();

						// 2 = ALLOCATION = allocate operators and optimize "not used at 100% ones" with multi-function ones
						///	AllocationOut alloc;												// to store the output of the allocation
						if(!sw.silent_mode())cout << "Allocation ... ";
							Allocation a_process(&sw, &clk, &lib, cdfg_s[i], &sel_s, &alloc_s);			// the allocation process
			long stages = alloc_s.size();
				if(!sw.silent_mode())
				{
					cout << "operators = " << alloc_s.operators_noPassThrough()<< ", stages = " << stages << endl;
						cout << sw._cdfgs_file_name[i]<<" latency = " << cdfg_s[i]->sink()->asap()<< endl;
				}
			if(sw.verbose())
			{
				for(int stage = 0; stage < stages; stage++)
				{
					cout << "stage " << stage << endl;
						alloc_s[stage].info();
				}
			}

				/* EJ 26/06/2007*/
				// if "force no pipeline" or "force_no_mobility" or "no_more_stage": locked sink node
				if(sw.scheduling_strategy()!= Switches::DEFAULT)
				{
					cdfg_s[i]->lockedSinkNode(cadencys.length(), clk.period(), stages, sw.scheduling_strategy());
				}
			// propagate locked if there are sink node locked
			cdfg_s[i]->propagateLocks();
				/* End EJ*/

				// 3 = SCHEDULING = schedule operations with IO and mapping constraints
				//cdfg.info();
				// to store the output of the scheduling

				if(sw.n_ordo()||sw.spact())
				{
					if(!sw.silent_mode())cout << "Scheduling of the next DFG... ";
						Scheduling sched_process(&sw, cdfg_s[i], &cadencys, &clk, &mem, &sel_s, &alloc_s, &lib, &sched_s);// scheduling process
				}
				else
				{
					if(!sw.silent_mode())cout << "Scheduling of the secondary DFG... "<<endl;
						Scheduling sched_process(&sw, cdfg_s[i], &cadencys, max_cadence, &clk, &mem, &sel_s, &alloc_s, &allocs, &sched_s);// scheduling process
					/*	for(int cstep=0; cstep<max_cadency/clk.period(); cstep++) {
						long nbOpr=allocs.allocationOutCycle().at(cstep).operators().size();
						cout<<allocs.allocationOutCycle().at(cstep).operators().size()<<"(";
						for(int nb=0; nb<nbOpr; nb++) {
						vector<const Operator*>::iterator it;
						cout<<allocs.allocationOutCycle().at(cstep).operators()[nb]->name()<<";";
						}
						cout<<")";
						}
						cout<<endl;*/
				}

				// Trouver le max entre les cadences précedantes et celle courante
				if(cadencys.length()>max_cadence)max_cadence=cadencys.length();
					// Generate the dump secondary cdfg with the approach considering the scheduling and the MWBM binding
					//and the the approach condiring only the MWBM binding
					vector<const Operation*> bound_operations_temp, bound_operations;
						vector<const CDFGnode*> bound_oper, datapath_sharing_temp, datapath_sharing;
						if(!sw.silent_mode())cout << "Binding of the secondary DFG... ";
							int previous_graphs=i;
								if(sw.cdfgs()|| sw.n_ordo())
								{
									if(!sw.silent_mode())cout << "Binding of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
										dfgs_binding(cdfg_s[i], cdfg_prim, previous_graphs, &clk, &cadencys, sw._cadencys, sched_s.stages.size(), sw.binding_case(), &bound_operations);
								}
			if(sw.spact())
			{
				if(!sw.silent_mode())cout << "Binding of "<<sw._cdfgs_file_name[i]<<"..."<<endl;
					dfgs_binding(cdfg_s[i], cdfg_prim, previous_graphs, &clk, &cadencys, sw._cadencys, sched_s.stages.size(), "mwbm_global", &bound_operations);
			}
			for(int j=0; j<bound_operations.size(); j++)
			{
				bound_oper.push_back(((CDFGnode*)bound_operations[j]));
			}
			/* Caaliph 03/07/2007*/
				data_WR(cdfg_s[i], &cadencys, &clk, &mem, &sched_s.banks);
				dumpCDFGfilename[i]=Tools::prefix(sw._cdfgs_file_name[i])+"_out";
				cdfg_s[i]->serialize(dumpCDFGfilename[i]+".sched", &bound_oper, sw._mode_numbers[i]);
				cdfg_s[i]->serialize_STAR(dumpCDFGfilename[i]+".tmp", Tools::prefix(sw._cdfgs_file_name[i]), &cadencys, &bound_oper, sw._mode_numbers[i]);
				/// The scheduling result is fill here to be used for VHDL generation
				fill_scheduling_out(cdfg_s[i], &sched_s, &cadencys, &clk);
				if(!sw.silent_mode())
				{
					cout <<", latency = " << cdfg_s[i]->latency()<< endl;
				}
			if(sw.verbose())
			{
				cout << "Scheduling results ... " << endl;
					sched_s.info();
			}

				// WARNING
				if(!sw.silent_mode()&&(cdfg_s[i]->latency()< cadencys.length()))
				{
					cout << "WARNING: "<<sw._cdfgs_file_name[i]<<" latency(" << cdfg_s[i]->latency()<< ")< cadency(" << cadencys.length()<< ")" << endl;
				}
			cdfg_prim[i]= new CDFG(
								   dumpCDFGfilename[i]+".sched",													// user CDFG file
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
				propagate_mode(cdfg_prim[i], sw._modes[i]);
				// 4 = REGISTERS ALLOCATION(really simple now)
				int nb_datas=0;
				int nb_op=0;
				RegSynthesis regs_process(&sw, cdfg_s[i], &sched_s,&regs_s, cadencys.length(), clk.period(),mem.access_time());
				regs_process.RegisterAllocation_NONE(lib.bits());

				// 5 = BUS ALLOCATION(really simple now))
				BusOut buses_s(&cadencys, &clk);  // to store the output of the bus synthesis
			BusOut abuses_s(&cadencys, &clk);
				if(!sw.silent_mode())cout << "Data Bus allocation ... ";
					// EJ 15/02/2008 : Dynamic access replace
					//BusSynthesis buses_process(&cadency, &clk, &mem, &cdfg, &sched, &buses, sw.ioConstraints());	// data bus synthesis process
					// by
					BusSynthesis buses_process(&cadencys, &clk, &mem, cdfg_s[i], &sched_s, &buses_s, &abuses_s, &sw);	// address & data bus synthesis process
			if(!sw.silent_mode())cout << buses_s.size()<< " data buses" << endl;
				if(!sw.silent_mode()&& abuses_s.size())cout << abuses_s.size()<< " address buses" << endl; // EJ 14/02/2008 : dynamic memory
			/*if(!sw.silent_mode())cout << "Bus allocation ... ";
			  BusSynthesis buses_process(&cadencys, &clk, &mem, cdfg_s[i], &sched_s, &buses_s, sw.ioConstraints());	// bus synthesis process
			  if(!sw.silent_mode())cout << buses_s.size()<< " buses" << endl;*/
			//update _bits with max bus width for input/ouput/memory bus
			long nb_bits;
				if(sw.bitwidth_aware() == true)
					nb_bits=buses_s.maxBusWith();
				else
					nb_bits=lib.bits();

						// Produce .mem file
						if(!sw.genMEM())
						{
							if(!sw.silent_mode())cout << ".mem generation ... ";
								DotMem dot_mems(&sw, &sw._mem_file_names[i], nb_bits, cdfg_s[i], &clk, &mem, &cadencys, &regs_s, &buses_s, &abuses_s);
									if(!sw.silent_mode())cout << "OK" << endl;
						}
		}
}
/****** End of multimode with incremental allocation******/


//	End of:		multimode.cpp

