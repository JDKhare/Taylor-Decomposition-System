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

//	File:		Soclib.cpp
//	Purpose:	CDFG to soclib C++
//	Author:		Dominique Heller, Lab-Sticc, UBS

#include <iostream>
#include <iomanip>
#include "cdfg2c.h"
#include "reg.h"
#include "ordo2c.h"
#include "soclib.h"
using namespace std;


//! Local copy of CDFG parameter to avoid passing constant parameters.

static const CDFG *Soclib_cdfg;	//!< local copy of cdfg reference
static const Clock *Soclib_clk;	//!< local copy of clk reference
static const Cadency *Soclib_cadency;	//!< local copy of cadency reference
static const BusOut *Soclib_buses;						//!< local copy of buses
static const Switches *Soclib_sw;				// !< local copy of sw

string Soclib_entity_name;



//! Local storage for IC cycles.
static IC_Cycles ic_cycles;

static void recursive_aging(ofstream &f,const Data *data);//recursive method for aging generation
static void process_loopback(ofstream &f,const CDFG *cdfg);
static void process_aging(ofstream &f,const CDFG *cdfg);

static void sort_operations(const CDFGnode *n)
{
	const Operation *o;									// the current operation
	const Data *d;										// the current Data
	Reg *r, *rr;										// registers
	long start, end;									// its starting and ending times
	long c_start, c_end;								// its starting and ending cycles
	long period = Soclib_clk->period();						// clock period

	start = n->start() % Soclib_cadency->length();			// get start
	c_start = start / period;							// compute starting cycle
	end = (start + n->length()) % Soclib_cadency->length();	// compute end
	c_end = end / period;

	// compute ending cycle
	if (c_end && c_end < c_start)
	{
		if ((n->type()==CDFGnode::DATA) && (Ordo2c::unused_data((const Data *)n)==true))
		{
			//skip unused data
		}
		else
		{
		cerr << "Internal error: end(" << end << ") of node " << n->name() << " preceeds start(" << start << ")" << endl;
		exit(1);
	}
	}
	switch (n->type())
	{
	case CDFGnode::DATA:
		// nothing to do
		break;
	case CDFGnode::CONTROL:
		// nothing to do
		break;
	case CDFGnode::OPERATION:
		o = (const Operation *) n;						// dynamic link here
		/* GL 10/11/07 : add lines */
		if (o->isChained())
		{
			start = n->start() % Soclib_cadency->length();			// get start
			c_start = start / period;							// compute starting cycle
			end = (start + o->chainingLength()) % Soclib_cadency->length();	// compute end
			c_end = end / period;
		}
		/* Fin GL */
		ic_cycles[c_start].start.push_back(o);			// add operation at start cycle c_start
		ic_cycles[c_end].end.push_back(o);				// add operation at end cycle c_end
		//cout << "operation " << o->name() << " start=" << start << "/" << c_start << " end=" << end << "/" << c_end << endl;
		break;
	}
}


static void process_undefined_operation(ofstream &f)
{
	int i,j;
	map<string,const Operation*> operations;

	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *n = Soclib_cdfg->nodes().at(i);
		if (n->type() == CDFGnode::OPERATION)
		{
			const Operation *o = (const Operation *) n;
			if (operations.find(o->function_signature(Soclib_cdfg))==operations.end())
			{
				operations[o->function_signature(Soclib_cdfg)]=o;
			}
		}
	}
	//declare method for user defined operation
	f << "\n" << "/* predefined or user defined  operation */\n";
	for( map<string,const Operation*>::iterator ii=operations.begin(); ii!=operations.end(); ++ii)
	{
			const Operation *operation = (const Operation *)(*ii).second;
			
			string operation_name=Cdfg2c::get_predefined_operation(operation->function_name());
			if ((operation_name.compare("undefined")==0) ||
				(operation_name.compare("predefined")==0)) 
			{
				f << "static void " << operation->function_signature(Soclib_cdfg) << "(";
				for (i = 0; i < operation->predecessors.size(); i++)
				{
					if (i) f << ", ";

					CDFGnode *predecessor = operation->predecessors[i]->source;	
					Data *d = ((Data*)predecessor);
					f << "const " << d->get_soclib_type() << " &input_" << i;
				}
	
				f << ", ";
				for (i = 0; i < operation->successors.size(); i++)
				{
					if (i) f << ", ";
					CDFGnode *successor = operation->successors[i]->target;
					Data *d = ((Data*)successor);
					f << d->get_soclib_type() << " &output_" << i;

				}
				f << ")" << "\n";
				f << "{" << "\n";
				if (operation_name.compare("undefined")==0)
				{
					f << "\t//TODO complete method internal code" << "\n";
					for (i = 0; i < operation->successors.size(); i++)
					{
						f << "\toutput_" << i << " =...;\n";
					}
						f << "\treturn;\n";
				}
			 	else if	((operation->function_name().compare("eqmux")==0) ||
						 (operation->function_name().compare("eqmux_fp")==0))
				{
					f << "\tif (input_0 == input_1)\n";
					f << "\t\toutput_0 = input_2;\n";
					f << "\telse\n";
					f << "\t\toutput_0 = input_3;\n";
				}
			 	else if	((operation->function_name().compare("nemux")==0) ||
						 (operation->function_name().compare("nemux_fp")==0))
				{
					f << "\tif (input_0 != input_1)\n";
					f << "\t\toutput_0 = input_2;\n";
					f << "\telse\n";
					f << "\t\toutput_0 = input_3;\n";
				}
			 	else if	((operation->function_name().compare("ltmux")==0) ||
						  (operation->function_name().compare("ltmux_fp")==0))
				{
					f << "\tif (input_0 < input_1)\n";
					f << "\t\toutput_0 = input_2;\n";
					f << "\telse\n";
					f << "\t\toutput_0 = input_3;\n";
				}
			 	else if	((operation->function_name().compare("lemux")==0) ||
						 (operation->function_name().compare("lemux_fp")==0))
				{
					f << "\tif (input_0 <= input_1)\n";
					f << "\t\toutput_0 = input_2;\n";
					f << "\telse\n";
					f << "\t\toutput_0 = input_3;\n";
				}
			 	else if	((operation->function_name().compare("gemux")==0) ||
						(operation->function_name().compare("gemux_fp")==0))
				{
					f << "\tif (input_0 >= input_1)\n";
					f << "\t\toutput_0 = input_2;\n";
					f << "\telse\n";
					f << "\t\toutput_0 = input_3;\n";
				}
			 	else if	((operation->function_name().compare("gtmux")==0) ||
						(operation->function_name().compare("gtmux_fp")==0))
				{
					f << "\tif (input_0 > input_1)\n";
					f << "\t\toutput_0 = input_2;\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;\n";
				}
			 	else if	(operation->function_name().compare("bitselect")==0)
				{
					f << "\toutput_0=input_0[input_1.to_uint()];\n";
				}
			 	else if	(operation->function_name().compare("slicewrite")==0)
				{
					f << "\toutput_0=input_0.set_slc(input_0,input_1);\n";
				}
				else
				{
					f << "\t//TODO complete method internal code\n";
					for (i = 0; i < operation->successors.size(); i++)
					{
						f << "\toutput_" << i << " =...;\n";
					}
						f << "\treturn;\n";
				}
				f << "}\n";
			}
	}
	operations.clear();
}

static void process_constant(ofstream &f)
{
	int i;
	f << "\t/* constant */\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::CONSTANT)
				f << "\t" << data->get_soclib_type() << " " << data->get_data_name_soclib() <<";\n";
		}
	}
}


static void constructor_constant(ofstream &f)
{
	int i;
	f << "\t/* constant */\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::CONSTANT)
				f << "\t" << data->get_data_name_soclib() << " = " << data->debug_value() << ";\n";
		}
	}
}


static void reset_variable(ofstream &f)
{
	int i,j;
	f << "\t\t/* variable */\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::VARIABLE)
			{
				f << "\t\t";
				// memory dynamic ?
				if (data->isADynamicAccess() && data->dynamic_access()->name() == data->get_data_name_soclib())
				{
					f << data->get_data_name_soclib() << "[" << data->dynamic_elements()->size() << "]={";
					for (j=0; j<data->dynamic_elements()->size(); j++)
					{
						f << data->dynamic_element(j)->resetvalue();
						if (j == data->dynamic_elements()->size()-1)
							f << "};\n";
						else
							f << ",";
					}
				}
				else if (data->dynamic_address() == -1)
				f  << data->get_data_name_soclib() << " = " << data->resetvalue() << ";\n";
			}  
		}
	}
}

static void process_variable(ofstream &f)
{
	int i,j;
	f << "\t/* variable */\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::VARIABLE)
			{
				f << "\t";
				// memory dynamic ?
				if (data->isADynamicAccess() && data->dynamic_access()->name() == data->get_data_name_soclib())
				{
					f << data->get_soclib_type() << " " << data->get_data_name_soclib() << "[" << data->dynamic_elements()->size() << "];\n";
				}
				else if (data->dynamic_address() == -1)
				f  << data->get_soclib_type() << " " << data->get_data_name_soclib() << ";\n";
			}  
		}
	}
}


static void process_input(ofstream &f)
{
	int i;
	f << "\t/* input */"  << "\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::INPUT)
			{
				f  << "\t" << data->get_soclib_type() << " " << data->get_data_name_soclib() << ";\n";
			}
		}
	}
}


static void reset_output(ofstream &f)
{
	int i;
	f << "\t\t/* output */\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::OUTPUT)
			{
				f  << "\t\t" << data->get_data_name_soclib() << " = 0;\n";
			}
		}
	}
}


static void process_output(ofstream &f)
{
	int i;
	f << "\t/* output */\n";
	for (i = 0; i < Soclib_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Soclib_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::OUTPUT)
			{
				f  << "\t" << data->get_soclib_type() << " " << data->get_data_name_soclib() << ";\n";
			}
		}
	}
}

static void process_operation(ofstream &f,long cycles,long *in_buses_Id,long *out_buses_Id)
{

	long cycle, cycle_start,  cycle_stop, oi;
	//long fifo;											//!< a fifo index
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const BusCycle *bus_cycle;							//!< a bus cycle
	const Data *data;									//!< a data node
	//Reg *reg;											//!< a register
	
	long period = Soclib_clk->period();						// clock period
	long cadency=Soclib_cadency->length();
	bool first_output=true;
	f << "\tm_vci_fsm.transition();\n";
	f << "\tswitch (m_state_cycle) {\n";
	for (cycle = 0; cycle < cycles; cycle++)
	{
		cycle_start = (cycle * period) % cadency;
		cycle_stop = (cycle_start + period) % cadency;
		f << "\t\tcase S" << cycle << ":\n";
		//input access ?
		bool has_input_access=false;
		bool has_output_access=false;
		// INPUTS from buses
		bus_cycle = Soclib_buses->getBusCycle(cycle_start );
		if (bus_cycle)
		{
			//input access ?
			for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end() && !has_input_access; ba_it++)
			{
				bus_access = *ba_it;
				if ((bus_access->type() == BusAccess::IN) && (bus_access->source() == BusAccess::IO)) has_input_access=true;
			}
			//
			if (has_input_access)
			{	
				bool fisrt_input=true;
				f << "\t\t\tif (";
				for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
				{
					bus_access = *ba_it;
					if (bus_access->source() != BusAccess::IO) continue;
					if (bus_access->type() != BusAccess::IN) continue;
					if (fisrt_input)
					{
						f << "m_fifo_in_" << in_buses_Id[bus_access->id()]+1 <<".rok()";
						fisrt_input=false;
					}
					else
						f << "\n\t\t\t&& m_fifo_in_" << in_buses_Id[bus_access->id()]+1 <<".rok()";
				}
				f << ")\n";
				f << "\t\t\t{\n";
				for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
				{
					bus_access = *ba_it;
					if (bus_access->source() != BusAccess::IO) continue;
					if (bus_access->type() != BusAccess::IN) continue;
					data = (const Data *) bus_access->addr();
					f << "\t\t\t\t" << data->get_data_name_soclib()<<"=m_fifo_in_" << in_buses_Id[bus_access->id()]+1 <<".read();\n";  
					f << "\t\t\t\tm_fifo_in_" << in_buses_Id[bus_access->id()]+1 <<".simple_get();\n";
				}
				f << "\t\t\t\tm_wait_IO = false;\n";		
				f << "\t\t\t}\n";
				f << "\t\t\telse m_wait_IO = true;\n";
			}
		}
		for (oi = 0; oi < ic_cycles[cycle].end.size(); oi++)
		{

			// get the current operation
			const Operation *operation = ic_cycles[cycle].end[oi];
	
			f << "\t\t\t";
			string operation_name=Cdfg2c::get_predefined_operation(operation->function_name());
			if ((operation_name.compare("undefined")==0) ||
				(operation_name.compare("predefined")==0))
			{
				int j;
				f << operation->function_signature(Soclib_cdfg) << "(";
				for (j = 0; j < operation->predecessors.size(); j++)
				{
					if (j) f << ", ";
					CDFGnode *predecessor = operation->predecessors[j]->source;
					Data *d = (Data *)predecessor;
					f << d->get_data_name_soclib();
				}
				f << ", ";
				for (j = 0; j < operation->successors.size(); j++)
				{
					if (j) f << ", ";
					CDFGnode *successor = operation->successors[j]->target;
					Data *d = (Data *)successor;
					f << d->get_data_name_soclib();
				}
				f << ");" << "\n";
			}
			else //predefined operator: unary or binary operator or slc
			{
				if (operation_name.compare("mem_write")!=0) 
				{
					for (int j = 0; j < operation->successors.size(); j++)
					{
						if (j) f << " = ";
						CDFGnode *successor = operation->successors[j]->target;
						Data *d = (Data *)successor;
						f << d->get_data_name_soclib();
					}
					f << " = ";
				}
				//unary operator ?
				if ((operation_name.compare("")==0) ||
					(operation_name.compare("uminus")==0) || (operation_name.compare("abs")==0))
				{
					//assignment or acfixed_resize(rndsat;...)
					if (operation->predecessors.size()!=1)
					{
						cout << "Invalid operand number for " << operation_name << "\n";
						exit (-1);
					}
					else
					{
						if (operation_name.compare("uminus")==0)
							f << " -";
						CDFGnode *predecessor = operation->predecessors[0]->source;
						Data *d = (Data *)predecessor;
						f << d->get_data_name_soclib() << ";" << "\n";
					}
				}
				else if (operation_name.compare("slc")==0) 
				{
					//slice_read
					CDFGnode *operand0_node = operation->predecessors[0]->source;
					Data *operand0 = (Data *)operand0_node;
					CDFGnode *index_node = operation->predecessors[1]->source;
					Data *index_operand = (Data *)index_node;
					CDFGnode *rang_node = operation->predecessors[2]->source;
					Data *rang_operand = (Data *)rang_node;
					f <<  operand0->get_data_name_soclib() << ".slc<" <<rang_operand->value() << ">(" << index_operand->get_data_name_soclib() << ");" << "\n";
				}
				else if (operation_name.compare("mem_read")==0) 
				{
					//mem_read
					CDFGnode *successsor = operation->successors[0]->target;
					Data *d = (Data *)successsor;
					const Data *dynamic_access =  d->dynamic_access();
					CDFGnode *index_node = operation->predecessors[0]->source;
					Data *index_operand = (Data *)index_node;
					f <<	  dynamic_access->get_data_name_soclib() << "[" << index_operand->get_data_name_soclib() << "];" << "\n";
				}
				else if (operation_name.compare("mem_write")==0) 
				{//mem_write
						CDFGnode *successsor = operation->successors[0]->target;
						Data *d = (Data *)successsor;
						const Data *dynamic_access =  d->dynamic_access();
						CDFGnode *index_node = operation->predecessors[0]->source;
						Data *index_operand = (Data *)index_node;
						CDFGnode *value_node = operation->predecessors[1]->source;
						Data *value_operand = (Data *)value_node;
						f << dynamic_access->get_data_name_soclib() << "[" << index_operand->get_data_name_soclib() << "] = " <<  value_operand->get_data_name_soclib() << ";" << "\n";
				}
				else if (operation_name.compare("resize")==0) 
				{//ac_int_resize (sign_extend)
						CDFGnode *successor = operation->successors[0]->target;
						Data *d = (Data *)successor;
						CDFGnode *operand0_node = operation->predecessors[0]->source;
						Data *operand0 = (Data *)operand0_node;
						f << "(" << d->get_soclib_type() << ")" << operand0->get_data_name_soclib() <<";" << "\n";
				}
				else //binary operator
				{
					if (operation->predecessors.size()!=2)
					{
						cout << "Invalid operand number for " << operation_name << "\n";
						operation->info();
						exit (-1);
					}
					else
					{
						CDFGnode *operand0_node = operation->predecessors[0]->source;
						Data *operand0 = (Data *)operand0_node;
						CDFGnode * operand1_node = operation->predecessors[1]->source;
						Data *operand1 = (Data *)operand1_node;
						f << operand0->get_data_name_soclib() << " "<< operation_name << " " <<operand1->get_data_name_soclib() << ";" << "\n";
					}
				}
			}

		}
		bus_cycle = Soclib_buses->getBusCycle(cycle_stop);
		if (bus_cycle)
		{
			//output access ?
			for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end() && !has_input_access; ba_it++)
			{
				bus_access = *ba_it;
				if ((bus_access->type() == BusAccess::OUT) && (bus_access->source() == BusAccess::IO)) has_output_access=true;
			}
			if (has_output_access)
			{	
				bool fisrt_output=true;
				f << "\t\t\tif (";
				for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
				{
					bus_access = *ba_it;
					if (bus_access->source() != BusAccess::IO) continue;
					if (bus_access->type() != BusAccess::OUT) continue;
					if (fisrt_output)
					{
						f << "m_fifo_out_" << out_buses_Id[bus_access->id()]+1 <<".wok()";
						fisrt_output=false;
					}
					else
						f << "\n\t\t\t&& m_fifo_out_" << out_buses_Id[bus_access->id()]+1 <<".wok()";
				}
				f << ")\n";
				f << "\t\t\t{\n";
				for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
				{
					bus_access = *ba_it;
					if (bus_access->source() != BusAccess::IO) continue;
					if (bus_access->type() != BusAccess::OUT) continue;
					data = (const Data *) bus_access->addr();
					f << "\t\t\t\tm_fifo_out_" << out_buses_Id[bus_access->id()]+1 <<".simple_put(" << data->get_data_name_soclib() <<".to_int());\n";  
				}
				if (first_output)
				{
					first_output=false;
					f << "\t\t\t\tr_status.output_available = true;\n";
				}
				f << "\t\t\t\tm_wait_IO = false;\n";		
				f << "\t\t\t}\n";
				f << "\t\t\telse m_wait_IO = true;\n";
			}
		}
		if (cycle == cycles-1)
		{
			if (has_input_access || has_output_access)
			{
				f << "\t\t\tif (!m_wait_IO)\n"; 
				f << "\t\t\t{\n"; 
				f << "\t\t\t\tm_state_cycle=S0;\n";
				process_loopback(f,Soclib_cdfg);
				process_aging(f,Soclib_cdfg);
				f << "\t\t\t\tr_status.done = true;\n";   
				f << "\t\t\t}\n"; 
			}
			else
			{
				f << "\t\t\tm_state_cycle=S0;\n";
				process_loopback(f,Soclib_cdfg);
				process_aging(f,Soclib_cdfg);
				f << "\t\t\tr_status.done = true;\n";   
			}
		}
		else
		{
			if (has_input_access || has_output_access)
				f << "\t\t\tif (!m_wait_IO) m_state_cycle=S" << cycle+1 << ";\n";
			else
				f << "\t\t\tm_state_cycle=S" << cycle+1 << ";\n";
		}
		f << "\t\tbreak;\n";
	}
	f << "\t}\n";

}


static void generate_soclib_license(ofstream &f)
{
	f << "/* -*- c++ -*-\n"; 
	f << "*\n"; 
	f << "* SOCLIB_LGPL_HEADER_BEGIN\n";
	f << "*\n"; 
	f << "* This file is part of SoCLib, GNU LGPLv2.1.\n"; 
	f << "*\n"; 
	f << "* SoCLib is free software; you can redistribute it and/or modify it\n"; 
	f << "* under the terms of the GNU Lesser General Public License as published\n"; 
	f << "* by the Free Software Foundation; version 2.1 of the License.\n"; 
	f << "*\n"; 
	f << "* SoCLib is distributed in the hope that it will be useful, but\n"; 
	f << "* WITHOUT ANY WARRANTY; without even the cabaied warranty of\n"; 
	f << "* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"; 
	f << "* Lesser General Public License for more details.\n"; 
	f << "*\n"; 
	f << "* You should have received a copy of the GNU Lesser General Public\n"; 
	f << "* License along with SoCLib; if not, write to the Free Software\n"; 
	f << "* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA\n"; 
	f << "* 02110-1301 USA\n"; 
	f << "*\n"; 
	f << "* SOCLIB_LGPL_HEADER_END\n";
	f << "*\n"; 
	f << "* Copyright (c) Lab-sticc, file generated by " << Soclib_sw->gaut_var_name() << "(Version " << Soclib_sw->gaut_version() << ")\n";
	f << "*\n"; 
	f << "*\n"; 
	f << "*/\n"; 
}

static void generate_interface(ofstream &f,const string &entity_class_name,long cycles,long in_buses,long out_buses)
{
	string entity_upper_name=entity_class_name;
	std::transform(entity_upper_name.begin(), entity_upper_name.end(), entity_upper_name.begin(), ::toupper);
	generate_soclib_license(f);
	f << "#ifndef _SOCLIB_CABA_"<< entity_upper_name << "_H_\n";
	f << "#define _SOCLIB_CABA_"<< entity_upper_name << "_H_\n";
	f << "#include <stdio.h>\n";
	f << "#include <systemc.h>\n";
	f << "#include \"caba_base_module.h\"\n";
	f << "#include \"generic_fifo.h\"\n";
	f << "#include \"vci_target.h\"\n";
	f << "#include \"mapping_table.h\"\n";
	f << "#include \"vci_target_fsm.h\"\n";
	f << "\nnamespace soclib { namespace caba {\n";
	f << "\nusing namespace sc_core;\n";
	f << "\ntemplate<typename vci_param> class " << entity_class_name << "\n";
	f << "\t: public  soclib::caba::BaseModule\n";
	f << "{\n";
	f << "public:\n";
    f << "\tsc_in<bool> p_clk;\n";
    f << "\tsc_in<bool> p_resetn;\n";
	f << "\tsoclib::caba::VciTarget<vci_param> p_t_vci;\n";
	f << "private:\n";
	f << "\tcaba::VciTargetFsm<vci_param, true> m_vci_fsm;\n"; 
    f << "\tconst soclib::common::Segment       m_segment;\n"; 
    f << "\tbool on_write(int seg,\n";  
    f << "\t\ttypename vci_param::addr_t addr,\n"; 
    f << "\t\ttypename vci_param::data_t data,\n"; 
    f << "\t\tint be);\n";   
    f << "\tbool on_read(int seg,\n";  
    f << "\t\ttypename vci_param::addr_t addr,\n"; 
    f << "\t\ttypename vci_param::data_t &data);\n"; 
    f << "\tvoid transition();\n";
    f << "\tvoid genMoore();\n";
	f << "\t/* STRUCTURAL members */\n";
	f << "\t/* states of the GAUT FSMD */\n";
	f << "\ttypedef enum {";
	long cycle;
	for (cycle = 0; cycle < cycles; cycle++)
	{
		f << "S" << cycle;
		if (cycle < (cycles-1)) f << ",";
		if ((cycle & 0xf) == 0x8) f << "\n" << "                      ";
	}
	f << "} GAUT_FSM_t;\n";
	f << "\tstruct Status {\n";
	f << "\t\tunsigned char done;\n";
	f << "\t\tunsigned char output_available;\n";
	f << "\t} r_status;\n";	
	long n;
	for (n = 0; n < in_buses; n++)
		f << "\tsoclib::caba::GenericFifo<int32_t> m_fifo_in_" << n+1 <<";\n";
	for (n = 0; n < out_buses; n++)
	    f << "\tsoclib::caba::GenericFifo<int32_t> m_fifo_out_" << n+1 <<"; \n";
    f << "\tint32_t m_wait_IO;\n";
    f << "\tGAUT_FSM_t m_state_cycle;\n";
	process_constant(f);
	process_input(f);
	process_output(f);
	process_variable(f);

	f << "protected:\n";
	f << "\tSC_HAS_PROCESS("<<entity_class_name << ");\n";
	f << "public:\n";
	f << "\t" << entity_class_name <<"(sc_module_name insname,\n";
	f << "\t\tconst soclib::common::MappingTable &mt,\n";
	f << "\t\tconst soclib::common::IntTab &t_index\n";
	f << "\t\t);\n";
	f << "\t~" << entity_class_name <<"() {}\n";
	f << "};" << "//en class " << entity_class_name <<"\n";
	f << "\tenum " <<  entity_class_name <<"Registers {\n"; 
    f << "\t\tSTATUS,\n";
	for (n = 0; n < in_buses; n++)
	    f << "\t\tIN_DATA_"<< n+1 <<",\n";
	for (n = 0; n < out_buses; n++)
	    f << "\t\tOUT_DATA_" << n+1 <<",\n";
	f << "};\n";
	f << "}}\n";
	f << "#endif /* _SOCLIB_CABA_"<< entity_upper_name << "_H_ */" << endl;
}

static void generate_vci_interface_fifo(ofstream &f,const string &entity_class_name,long in_buses,long out_buses)
{
	long n;	
	f << "\n/* Read interface vci */\n";
	f << "tmpl(bool)::on_read(int seg,\n";
    f << "\ttypename vci_param::addr_t addr,\n";
    f << "\ttypename vci_param::data_t & data)\n";
	f << "{\n";
	f << "\tint cell = (int)addr / vci_param::B;\n";
	f << "\tbool nerror = false;\n"; 
	f << "\tswitch ((enum "<< entity_class_name<<"Registers)cell)\n";
	f << "\t{\n";
	for (n = 0; n < out_buses; n++)
	{
		f << "\t\tcase OUT_DATA_" << n+1 <<":\n";
		f << "\t\t\tif(m_fifo_out_" << n+1 <<".rok())\n";
		f << "\t\t\t{\n";
		f << "\t\t\t\tdata=m_fifo_out_" << n+1 <<".read();\n";  
		f << "\t\t\t\tm_fifo_out_" << n+1<<".simple_get();\n";
		f << "\t\t\t\tnerror = true;\n";		
		f << "\t\t\t}\n";
		f << "\t\t\telse nerror = false;\n";
		f << "\t\tbreak;\n";
	}
	f << "\t\tcase STATUS:\n";
	f << "\t\t{\n";
	f << "\t\t\tint local_data=0;\n";
	f << "\t\t\tlocal_data=r_status.done+((unsigned int)r_status.output_available << 8);\n";
	f << "\t\t\tdata = local_data;\n"; 
	f << "\t\t\t/* reset flag after status register read */\n";
	f << "\t\t\tif ((r_status.done) || (r_status.output_available))\n";
	f << "\t\t\t{\n";
	f << "\t\t\t\tr_status.done=false;\n";
	f << "\t\t\t\tr_status.output_available=false;\n";
	f << "\t\t\t}\n";
	f << "\t\t\tnerror = true;\n";
	f << "\t\t}\n";
	f << "\t\tbreak;\n";
	f << "\t\tdefault :\n"; 
	f << "\t\tbreak;\n";
	f << "\t}\n";
	f << "\treturn nerror;\n";    
	f << "}\n";
	f << "\n/* Write interface vci */\n";
	f << "tmpl(bool)::on_write(int seg,\n";
    f << "\ttypename vci_param::addr_t addr,\n";
    f << "\ttypename vci_param::data_t data,\n";
    f << "\tint be)\n";
	f << "{";
	f << "\tint cell = (int)addr / vci_param::B;\n";
	f << "\tbool nerror = false;\n"; 
	f << "\tswitch ((enum "<< entity_class_name<<"Registers)cell)\n";
	f << "\t{\n";
	for (n = 0; n < in_buses; n++)
	{
		f << "\t\tcase IN_DATA_" << n+1 <<":\n";
		f << "\t\t\tif(m_fifo_in_" << n+1 <<".wok())\n";
		f << "\t\t\t{\n";
		f << "\t\t\t\tm_fifo_in_" << n+1 <<".simple_put(data);\n";  
		f << "\t\t\t\tnerror = true;\n";		
		f << "\t\t\t}\n";
		f << "\t\t\telse nerror = false;\n";
		f << "\t\tbreak;\n";
	}
	f << "\t\tdefault :\n"; 
	f << "\t\tbreak;\n";
	f << "\t}\n";
	f << "\treturn nerror;\n";    
	f << "}\n";

}

static void generate_transition(ofstream &f,const string &entity_class_name,long in_buses,long out_buses,long cycles,long *in_buses_Id,long *out_buses_Id)
{
    f << "/* Transition function */\n";
	f << "/* Evaluated on Positive Clock Edge */\n";
	f << "tmpl(void)::transition()\n";
	f << "{\n";
	f << "\tif (p_resetn == false)\n";
	f << "\t{\n";
	f << "\t\tm_vci_fsm.reset();\n";
	f << "\t\tr_status.done = false;\n";
	f << "\t\tr_status.output_available = false;\n";
	f << "\t\tm_state_cycle = S0;\n";
	f << "\t\tm_wait_IO = false;\n";
	reset_variable(f);
	reset_output(f);
	f << "\t\treturn;\n";
	f << "\t}\n";
	process_operation(f,cycles,in_buses_Id,out_buses_Id);
	f << "}\n";		
}

static void generate_genMoore(ofstream &f)
{
	f << "/* Moore fsm generation */\n";
	f << "/* Evaluated on Negative Clock Edge */\n";
	f << "tmpl(void)::genMoore()\n";
	f << "{\n";
	f << "\tm_vci_fsm.genMoore();\n";
	f << "}\n";
}

static void generate_python_sd(ofstream &f,const string &entity_class_name)
{
	string module_name=entity_class_name;
	std::transform(module_name.begin(), module_name.end(), module_name.begin(), ::tolower);

	f << "# -*- python -*-\n";
	f << "\n";
	f << "Module('caba:" << module_name << "',\n";
	f << "\tclassname = 'soclib::caba::" << entity_class_name << "',\n";
	f << "\ttmpl_parameters = [\n";
	f << "\tparameter.Module('vci_param', default = 'vci_param'),\n";
	f << "\t],\n";
	f << "\theader_files = [\n";
	f << "\t'../source/include/" << module_name << ".h',\n";
	f << "\t],\n";
	f << "\timplementation_files = [\n";
	f << "\t'../source/src/" << module_name <<".cpp',\n";
	f << "\t],\n";
	f << "\tuses = [\n";
	f << "\tUses('base_module'),\n";
	f << "\tUses('generic_fifo'),\n";
    f << "\tUses('caba:vci_target_fsm', default_target = 'true', support_llsc = 'false'),\n";
	f << "\tUses('common:mapping_table'),\n";
	f << "\t],\n";
	f << "\tports = [\n";
	f << "\tPort('vci_target', 'p_t_vci'),\n";
	f << "\tPort('bit_in', 'p_resetn', auto = 'resetn'),\n";
	f << "\tPort('clock_in', 'p_clk', auto = 'clock'),\n";
	f << "\t],\n";
	f << "\tinstance_parameters = [\n";
	f << "\tparameter.Module('mt', 'common:mapping_table', auto='env:mapping_table'),\n";
	f << "\tparameter.IntTab('t_ident'),\n";
	f << "\t],\n";
	f << ")" << endl;

}

static void generate_implementation(ofstream &f,const string &entity_class_name,long cycles,const string &header_file_string,long in_buses,long out_buses,long *in_buses_Id,long *out_buses_Id)
{

	long n;
	generate_soclib_license(f);

	f << "#include <stdlib.h>\n";
	f << "#include \"base_module.h\"\n";
	f << "#include \"../include/" << header_file_string << "\"\n";
	f << "\nnamespace soclib { namespace caba {\n\n";
	f << "#define tmpl(x) template <typename vci_param> x " << entity_class_name <<"<vci_param>\n\n";
	f << "/* Constructor */\n";
	f << "tmpl(/**/)::" << entity_class_name <<"(sc_module_name insname,\n";
	f << "\tconst soclib::common::MappingTable &mt,\n";
	f << "\tconst soclib::common::IntTab &t_index)\n";
    f << "\t:soclib::caba::BaseModule(insname),\n";
	for (n = 0; n < in_buses; n++)
		f << "\tm_fifo_in_" << n+1 <<"(\"fifo_in_"<< n+1 <<"\",1),\n";
	for (n = 0; n < out_buses; n++)
		f << "\tm_fifo_out_" << n+1 <<"(\"fifo_out_"<< n+1 <<"\",1),\n";
    f << "\tm_segment(mt.getSegment(t_index)),\n";
    f << "\tp_clk(\"clk\"),\n";
	f << "\tp_resetn(\"resetn\"),\n";
	f << "\tp_t_vci(\"t_vci\"),\n";
	f << "\tm_vci_fsm(p_t_vci, mt.getSegmentList(t_index),1)\n";
  	f << "{\n";
	f << "\tSC_METHOD(transition);\n";
	f << "\tdont_initialize();\n";
	f << "\tsensitive << p_clk.pos();\n";
	f << "\tSC_METHOD(genMoore);\n";
	f << "\tdont_initialize();\n";
	f << "\tsensitive << p_clk.neg();\n";
    f << "\tm_vci_fsm.on_read_write(on_read, on_write);\n";
	f << "\tr_status.done = false;\n";
	f << "\tr_status.output_available = false;\n";
	f << "\tm_state_cycle = S0;\n";
	f << "\tm_wait_IO = false;\n";
	constructor_constant(f);
	f << "}\n\n";
	generate_vci_interface_fifo(f,entity_class_name,in_buses,out_buses);
	f << "\n";
	process_undefined_operation(f);
	f << "\n";
	generate_transition(f,entity_class_name,in_buses,out_buses,cycles,in_buses_Id,out_buses_Id);
	generate_genMoore(f);
	f << "}}" << endl;
}

// X(3) = X(2);
// X(2) = X(1);
// X(1) = Xn;
static void recursive_aging(ofstream &f,const Data *data)
{
	if (data->writevar())
	{
		f << "\t\t\t" << data->get_data_name_soclib() << " = " << data->writevar()->get_data_name_soclib() << "; // aging" << "\n";
		recursive_aging(f,data->writevar());
	}
} 

static void process_loopback(ofstream &f,const CDFG *cdfg)
{
	for (int i = 0; i < cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (!data->aging()  && data->writevar())
			{
				f << "\t\t\t" << data->get_data_name_soclib() << " = " << data->writevar()->get_data_name_soclib() << "; //loopback" << "\n";
			}
		}
	}
}

static void process_aging(ofstream &f,const CDFG *cdfg)
{
	for (int i = 0; i < cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			//end of aging vector
			if (data->aging() && !(data->readvar()))
			{
				f << "\t\t\t" << data->get_data_name_soclib() << " = " << data->writevar()->get_data_name_soclib() << "; // aging" << "\n";
				recursive_aging(f,data->writevar());
			}
		}
	}
}


Soclib::Soclib(
    const Switches *sw,					// in
    const CDFG *cdfg,				//!< local copy of cdfg
	const Clock *clk,				//!< local copy of clk
	const Cadency *cadency,	//!< local copy of cadency reference
	const BusOut *buses,						//!< local copy of buses
	const string &_entity_name
)
{

	long cycles;										//!< a number of cycles

	// copy parameters
	Soclib_sw = sw;
	Soclib_cdfg = cdfg;
	Soclib_clk = clk;
	Soclib_cadency = cadency;
	Soclib_buses = buses;
	Soclib_entity_name = _entity_name;


	// compute number of cycles
	long end = cadency->length();
	long period = clk->period();						//!< clock period
	//cycles = (end-period) / period;
	cycles = end / period;
	//compute nb in put buses and ouput buses
	long n, in_buses = 0,out_buses = 0;
	long *in_buses_Id= new long[Soclib_buses->size()+1];
	long *out_buses_Id= new long[Soclib_buses->size()+1];
	for (n = 0; n <= Soclib_buses->size(); n++)
	{
		in_buses_Id[n]=0;
		out_buses_Id[n]=0;
	}	
	for (n = 0; n < Soclib_buses->size(); n++)
	{
		if (Soclib_buses->BusHasAnInputAccess(n))
		{
			in_buses_Id[n+1]=in_buses;
			in_buses++;
		}
		if (Soclib_buses->BusHasAnOutputAccess(n))
		{
			out_buses_Id[n+1]=out_buses;
			out_buses++;
	}
	}

	string entity_class_name=Soclib_entity_name;
	std::transform(entity_class_name.begin(), entity_class_name.end(), entity_class_name.begin(), ::tolower);
	entity_class_name[0]=toupper(entity_class_name[0]);

	//generate python file.sd
	string python_file_string = Soclib_entity_name+".sd";
	std::transform(python_file_string.begin(), python_file_string.end(), python_file_string.begin(), ::tolower);
	char *python_file_name=strdup(python_file_string.c_str());
	ofstream fsd(python_file_name, ios::out);
	generate_python_sd(fsd,entity_class_name);
	fsd.close();


	//generate interface file.h
	string header_file_string = Soclib_entity_name+".h";
	std::transform(header_file_string.begin(), header_file_string.end(), header_file_string.begin(), ::tolower);
	char *header_file_name=strdup(header_file_string.c_str());
	ofstream fh(header_file_name, ios::out);
	generate_interface(fh,entity_class_name,cycles,in_buses,out_buses);
	fh.close();
	
	// scan CDFG and sort operations among cycles
	ic_cycles.resize(cycles+1);
	Soclib_cdfg->scan_nodes(&sort_operations);

	//generate implementation file.cpp
	string cc_file_string = Soclib_entity_name+".cpp";
	std::transform(cc_file_string.begin(), cc_file_string.end(), cc_file_string.begin(), ::tolower);
	char *cc_file_name=strdup(cc_file_string.c_str());
	ofstream fc(cc_file_name, ios::out);
	generate_implementation(fc,entity_class_name,cycles,header_file_string,in_buses,out_buses,in_buses_Id,out_buses_Id);
	fc.close();
	ic_cycles.clear();
	delete []in_buses_Id;
	delete []out_buses_Id;

}

// end of: Soclib.cpp
