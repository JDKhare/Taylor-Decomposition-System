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

//	File:		ordo2c.cpp
//	Purpose:	shceduled CDFG to C (debug)
//	Author:		Lab-Sticc, UBS

#include <iostream>
#include <iomanip>
#include "cdfg2c.h"
#include "reg.h"
#include "ordo2c.h"
using namespace std;


//! Local copy of CDFG parameter to avoid passing constant parameters.

static const CDFG *Ordo2c_cdfg;	//!< local copy of cdfg reference
static const Clock *Ordo2c_clk;	//!< local copy of clk reference
static const Cadency *Ordo2c_cadency;	//!< local copy of cadency reference
static const BusOut *Ordo2c_buses;						//!< local copy of buses
string Ordo2c_entity_name;



//! Local storage for IC cycles.
static IC_Cycles ic_cycles;


static void sort_operations(const CDFGnode *n)
{
	const Operation *o;									// the current operation
	const Data *d;										// the current Data
	Reg *r, *rr;										// registers
	long start, end;									// its starting and ending times
	long c_start, c_end;								// its starting and ending cycles
	long period = Ordo2c_clk->period();						// clock period

	start = n->start() % Ordo2c_cadency->length();			// get start
	c_start = start / period;							// compute starting cycle
	end = (start + n->length()) % Ordo2c_cadency->length();	// compute end
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
			start = n->start() % Ordo2c_cadency->length();			// get start
			c_start = start / period;							// compute starting cycle
			end = (start + o->chainingLength()) % Ordo2c_cadency->length();	// compute end
			c_end = end / period;
		}
		/* Fin GL */
		ic_cycles[c_start].start.push_back(o);			// add operation at start cycle c_start
		ic_cycles[c_end].end.push_back(o);				// add operation at end cycle c_end
		//cout << "operation " << o->name() << " start=" << start << "/" << c_start << " end=" << end << "/" << c_end << endl;
		break;
	}
}



static void init(ofstream &f)
{
	int i,j;
	f << "#include <fstream>"							<< "\n";
	f << "#include \"ac_fixed.h\"" << "\n"; // ac_fixed include ac_int
	f << "#include \"fixed_dec.h\"" << "\n"; 
	f << "using namespace std;" << "\n"<< "\n";

	f << "#define iteration 10"							<< "\n";
	f << "#define verbose   0"							<< "\n";
	f << "#define trace     1"							<< "\n" << "\n";

	map<string,const Operation*> operations;

	for (i = 0; i < Ordo2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *n = Ordo2c_cdfg->nodes().at(i);
		if (n->type() == CDFGnode::OPERATION)
		{
			const Operation *o = (const Operation *) n;
			if (operations.find(o->function_signature(Ordo2c_cdfg))==operations.end())
			{
				operations[o->function_signature(Ordo2c_cdfg)]=o;
			}
		}
	}
	//declare method for user defined operation
	f << "\n" << "//predefined or user defined  operation"	<< "\n";
	for( map<string,const Operation*>::iterator ii=operations.begin(); ii!=operations.end(); ++ii)
	{
			const Operation *operation = (const Operation *)(*ii).second;
			
			string operation_name=Cdfg2c::get_predefined_operation(operation->function_name());
			if ((operation_name.compare("undefined")==0) ||
				(operation_name.compare("predefined")==0)) 
			{
				f << "void " << operation->function_signature(Ordo2c_cdfg) << "(";
				for (i = 0; i < operation->predecessors.size(); i++)
				{
					if (i) f << ", ";

					CDFGnode *predecessor = operation->predecessors[i]->source;	
					Data *d = ((Data*)predecessor);
					f << "const " << d->get_mentor_type() << " &input_" << i;
				}
	
				f << ", ";
				for (i = 0; i < operation->successors.size(); i++)
				{
					if (i) f << ", ";
					CDFGnode *successor = operation->successors[i]->target;
					Data *d = ((Data*)successor);
					f << d->get_mentor_type() << " &output_" << i;

				}
				f << ")" << "\n";
				f << "{" << "\n";
				if (operation_name.compare("undefined")==0)
				{
					f << "\t//TODO complete method internal code" << "\n";
					for (i = 0; i < operation->successors.size(); i++)
					{
						f << "\toutput_" << i << " =...;" << "\n";
					}
						f << "\treturn;" << "\n";
				}
			 	else if	((operation->function_name().compare("eqmux")==0) ||
						 (operation->function_name().compare("eqmux_fp")==0))
				{
					f << "\tif (input_0 == input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("nemux")==0) ||
						 (operation->function_name().compare("nemux_fp")==0))
				{
					f << "\tif (input_0 != input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("ltmux")==0) ||
						  (operation->function_name().compare("ltmux_fp")==0))
				{
					f << "\tif (input_0 < input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("lemux")==0) ||
						 (operation->function_name().compare("lemux_fp")==0))
				{
					f << "\tif (input_0 <= input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("gemux")==0) ||
						(operation->function_name().compare("gemux_fp")==0))
				{
					f << "\tif (input_0 >= input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("gtmux")==0) ||
						(operation->function_name().compare("gtmux_fp")==0))
				{
					f << "\tif (input_0 > input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	(operation->function_name().compare("bitselect")==0)
				{
					f << "\toutput_0=input_0[input_1.to_uint()];" << "\n";
				}
			 	else if	(operation->function_name().compare("slicewrite")==0)
				{
					f << "\toutput_0=input_0.set_slc(input_0,input_1);" << "\n";
				}
				else
				{
					f << "\t//TODO complete method internal code" << "\n";
					for (i = 0; i < operation->successors.size(); i++)
					{
						f << "\toutput_" << i << " =...;" << "\n";
					}
						f << "\treturn;" << "\n";
				}
				f << "}" << "\n";
			}
	}
	operations.clear();
	//constant
	f << "//constant" << "\n";
	for (i = 0; i < Ordo2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Ordo2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::CONSTANT)
			{


				f << "const " << data->get_mentor_type() << " " << data->name() << " = " << data->debug_value() << ";" << "\n";
			}
		}
	}
	//input
	f << "//input" << "\n";
	for (i = 0; i < Ordo2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Ordo2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::INPUT)
			{
				f  << data->get_mentor_type() << " " << data->name() << "[iteration] = {0,1,2,3,4,5,6,7,8,9};"  << "\n";
			}
		}
	}
	//output
	f << "//output" << "\n";
	for (i = 0; i < Ordo2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Ordo2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::OUTPUT)
			{
				f  << data->get_mentor_type() << " " << data->name() << " = 0;"  << "\n";
			}
		}
	}
	//variable
	f << "//variable" << "\n";
	for (i = 0; i < Ordo2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Ordo2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::VARIABLE)
			{
				// memory dynamic ?
				if (data->isADynamicAccess() && data->dynamic_access()->name() == data->name())
				{
					f <<data->get_mentor_type() << " " << data->name() << "[" << data->dynamic_elements()->size() << "]={";
					for (j=0; j<data->dynamic_elements()->size(); j++)
					{
						f << data->dynamic_element(j)->resetvalue();
						if (j == data->dynamic_elements()->size()-1)
							f << "};" << "\n";
						else
							f << ",";
					}
				}
				else if (data->dynamic_address() == -1)
				f  << data->get_mentor_type() << " " << data->name() << " = " << data->resetvalue() << ";" << "\n";
			}  
		}
	}
	f << "\n";

	f << "// process" << "\n";
	f << "int main (int argc, char *argv[])"				<< "\n";
	f << "{"												<< "\n";
	f << "\tint it;"										<< "\n";
	f << "\t#include \"stimulipre.h\"\n"; 
	f << "\tfor (it=0; it<iteration; it++)" << "\n" << " \t{"	<< "\n";
	f << "\t\t#include \"stimuli.h\"\n"; 
	f << "\t\tif(trace) cout <<  \"Iteration : \" << it << endl;"	<< "\n";
}

static void process_operation(ofstream &f,long period,long cadency,long cycles,bool trace_reg)
{

	long cycle, cycle_start,  cycle_stop, oi;
	//long fifo;											//!< a fifo index
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const BusCycle *bus_cycle;							//!< a bus cycle
	const Data *data;									//!< a data node
	//Reg *reg;											//!< a register
	
	
	for (cycle = 0; cycle <= cycles; cycle++)
	{
		cycle_start = (cycle * period) % cadency;
		cycle_stop = (cycle_start + period) % cadency;
		f << "\t\t//cycle S" << cycle << " (time " << cycle_start			<< ")\n";
		// INPUTS from buses
		bus_cycle = Ordo2c_buses->getBusCycle(cycle_start );
		if (bus_cycle)
			for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
			{
				bus_access = *ba_it;
				if (bus_access->type() != BusAccess::IN) continue;
				//cout << " cycle " << cycle << " bus " << bus_name(bus_access) << endl;
				if (bus_access->source() == BusAccess::IO)
				{
					data = (const Data *) bus_access->addr();
					//reg = (Reg*)_regs->getReg(cycle_start, data);
					//fifo= reg->fifoStage();
				}
				else continue;

				f << "\t\t//input " <<  data->get_data_name() <<" from bus " <<	bus_access->id() << "\n";
				f << "\t\tif (trace)" << "\n";
				f << "\t\t{" << "\n";
				if (data->getSigned()==Data::STD_LOGIC_VECTOR)
				{ 
					if (trace_reg)
					{
						f << "\t\t\tcout << \"" << data->reg()->name() << "[ " << cycle_start << "] = \"" << " << " << data->get_data_name()  << " << endl;" << "\n";
					}
					else
					{
						f << "\t\t\tcout << \"" << data->get_data_name() << "[ \" << it << \"] = \"" << " << " << data->get_data_name()  << "<< endl;" << "\n";
					}
				}
				else
				{
					if (trace_reg)
					{
						/* if (data->getSigned() == Data::UNSIGNED ||  data->getSigned() == Data::UFIXED)
							f << "\t\t\tcout << \"" << data->reg()->name() << "[ " << cycle_start << "] = \"" << " << " << data->get_data_name()  << " << \"(\" << " << "Fixed_dec::fixed_todec(" << data->get_data_name() << ".to_string(AC_BIN).c_str(),"<<  data->bitwidth() << ",UNSIGNED_M)"   << " << \")\"" << " << endl;" << "\n";
						else */
							f << "\t\t\tcout << \"" << data->reg()->name() << "[ " << cycle_start << "] = \"" << " << " << data->get_data_name()  << " << \"(\" << " << data->get_data_name() << ".to_string(AC_BIN)" " << \",\"" << " << Fixed_dec::fixed_todec(" << data->get_data_name() << ".to_string(AC_BIN).c_str(),"<<  data->bitwidth() << ",SIGNED_M)"   << " << \")\"" << " << endl;" << "\n";
					}
						else
					{
						f << "\t\t\tcout << \"" << data->get_data_name() << "[ \" << it << \"] = \"" << " << " << data->get_data_name()  << "<< \"(\" << " << data->get_data_name() << ".to_string(AC_BIN)" << " << \")\"" << " << endl;" << "\n";
					}
				}
				f << "\t\t}" << "\n";

			}
		for (oi = 0; oi < ic_cycles[cycle].end.size(); oi++)
		{

			// get the current operation
			const Operation *operation = ic_cycles[cycle].end[oi];
	
			f << "\t\t";
			string operation_name=Cdfg2c::get_predefined_operation(operation->function_name());
			if ((operation_name.compare("undefined")==0) ||
				(operation_name.compare("predefined")==0))
			{
				int j;
				f << operation->function_signature(Ordo2c_cdfg) << "(";
				for (j = 0; j < operation->predecessors.size(); j++)
				{
					if (j) f << ", ";
					CDFGnode *predecessor = operation->predecessors[j]->source;
					Data *d = (Data *)predecessor;
					f << d->get_data_name();
				}
				f << ", ";
				for (j = 0; j < operation->successors.size(); j++)
				{
					if (j) f << ", ";
					CDFGnode *successor = operation->successors[j]->target;
					Data *d = (Data *)successor;
					f << d->get_data_name();
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
						f << d->get_data_name();
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
						f << d->get_data_name() << ";" << "\n";
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
					f <<  operand0->get_data_name() << ".slc<" <<rang_operand->value() << ">(" << index_operand->get_data_name() << ");" << "\n";
				}
				else if (operation_name.compare("mem_read")==0) 
				{
					//mem_read
					CDFGnode *successsor = operation->successors[0]->target;
					Data *d = (Data *)successsor;
					const Data *dynamic_access =  d->dynamic_access();
					CDFGnode *index_node = operation->predecessors[0]->source;
					Data *index_operand = (Data *)index_node;
					f <<	  dynamic_access->get_data_name() << "[" << index_operand->get_data_name() << "];" << "\n";
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
						f << dynamic_access->get_data_name() << "[" << index_operand->get_data_name() << "] = " <<  value_operand->get_data_name() << ";" << "\n";
				}
				else if (operation_name.compare("resize")==0) 
				{//ac_int_resize (sign_extend)
						CDFGnode *successor = operation->successors[0]->target;
						Data *d = (Data *)successor;
						CDFGnode *operand0_node = operation->predecessors[0]->source;
						Data *operand0 = (Data *)operand0_node;
						f << "(" << d->get_mentor_type() << ")" << operand0->get_data_name() <<";" << "\n";
				}
				else //binary operator
				{
					if (operation->predecessors.size()!=2)
					{
						cout << "Invalid operand number for " << operation_name << "\n";
						exit (-1);
					}
					else
					{
						CDFGnode *operand0_node = operation->predecessors[0]->source;
						Data *operand0 = (Data *)operand0_node;
						CDFGnode * operand1_node = operation->predecessors[1]->source;
						Data *operand1 = (Data *)operand1_node;
						f << operand0->get_data_name() << " "<< operation_name << " " <<operand1->get_data_name() << ";" << "\n";
					}
				}
			}
			//trace
			for (int j = 0; j < operation->successors.size(); j++)
			{
				CDFGnode *successsor = operation->successors[j]->target;
				Data *d = (Data *)successsor;
				/* if (d->type()!=Data::OUTPUT)
				{ */
					f << "\t\tif (trace)" << "\n";
					f << "\t\t{" << "\n";
					if (d->getSigned()==Data::STD_LOGIC_VECTOR) 
					{
						if (trace_reg)
						{
							f << "\t\t\tcout << \"" << d->reg()->name() << "[ " << cycle_start << "] = \"" << " << " << d->get_data_name()  << " << endl;" << "\n";
						}
						else
						{
							f << "\t\t\tcout << \"" << d->get_data_name() << "[ \" << it << \"] = \"" << " << " << d->get_data_name()  << " << endl;" << "\n";
						}

					}
					else
					{
						if (trace_reg)
						{
							/* if (d->getSigned() == Data::UNSIGNED ||  d->getSigned() == Data::UFIXED)
								f << "\t\t\tcout << \"" << d->reg()->name() << "[ " << cycle_start << "] = \"" << " << " << d->get_data_name()  << " << \"(\" << " << "Fixed_dec::fixed_todec(" << d->get_data_name() << ".to_string(AC_BIN).c_str(),"<<  d->bitwidth() << ",UNSIGNED_M)"   << " << \")\"" << " << endl;" << "\n";
							else */
								f << "\t\t\tcout << \"" << d->reg()->name() << "[ " << cycle_start << "] = \"" << " << " << d->get_data_name()  << " << \"(\" << " << d->get_data_name() << ".to_string(AC_BIN)" " << \",\"" << " << Fixed_dec::fixed_todec(" << d->get_data_name() << ".to_string(AC_BIN).c_str(),"<<  d->bitwidth() << ",SIGNED_M)"   << " << \")\"" << " << endl;" << "\n";
						}
						else
						{
							f << "\t\t\tcout << \"" << d->get_data_name() << "[ \" << it << \"] = \"" << " << " << d->get_data_name()  << " << \"(\" << " << d->get_data_name() << ".to_string(AC_BIN)" << " << \")\"" << " << endl;" << "\n";
						}
					}
					f << "\t\t}" << "\n";
				}
			/* } */

		}

		bus_cycle = Ordo2c_buses->getBusCycle(cycle_stop);
		if (bus_cycle)
			for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
			{
				bus_access = *ba_it;
				if (bus_access->type() != BusAccess::OUT) continue;
				//cout << " cycle " << cycle << " bus " << bus_name(bus_access) << endl;
				if (bus_access->source() == BusAccess::IO)
				{
					data = (const Data *) bus_access->addr();
					f << "\t\tcout << \"" << data->get_data_name() << "[ \" << it << \"] = \"" << " << " << data->get_data_name()  << " << endl;" << "\n";
				}
			}
	}


}


bool Ordo2c::unused_data(const Data *d) 
{
	//DH: 12/11/2008 skip unused constant (mul to shift conversion)
	if ((d->type()==Data::CONSTANT) &&
			(d->successors.empty()==true))
		return true;
	//DH: 12/11/2008 : skip unused dynamic element 
	if ((d->isADynamicElement()) && (d->predecessors.empty()==true))
		return true;
	//DH: 12/11/2008 : skip unused dynamic access only keep dynamic read access storage
	if ((d->isADynamicAccess()) && (d->predecessors.size()==1) )
	{
		CDFGnode *predecessor= (CDFGnode *)d->predecessors.at(0)->source;
		if (predecessor->type()==CDFGnode::CONTROL)
			return true;
		if (predecessor->type()==CDFGnode::OPERATION)
		{
			Operation *mem_operation = (Operation*)predecessor;
			if (mem_operation->function()->symbolic_function()=="mem_write")
				return true;
		}
	}
	return false;
}

Ordo2c::Ordo2c(
    const CDFG *cdfg,				//!< local copy of cdfg
	const Clock *clk,				//!< local copy of clk
	const Cadency *cadency,	//!< local copy of cadency reference
	const BusOut *buses,						//!< local copy of buses
	const string &_entity_name,
	const bool trace_reg

)
{

	long cycles;										//!< a number of cycles

	// copy parameters
	Ordo2c_cdfg = cdfg;
	Ordo2c_clk = clk;
	Ordo2c_cadency = cadency;
	Ordo2c_buses = buses;
	Ordo2c_entity_name = _entity_name;


	// compute number of cycles
	long end = cadency->length();
	long period = clk->period();						//!< clock period
	cycles = (end-period) / period;

	
	// scan CDFG and sort operations among cycles
	ic_cycles.resize(cycles+1);
	Ordo2c_cdfg->scan_nodes(&sort_operations);

	// ofstream cdfg to C
	string cc_file_string;
	if (trace_reg==false)
		cc_file_string = Ordo2c_entity_name+"_sched.cpp";
	else
		cc_file_string = Ordo2c_entity_name+"_reg.cpp";
	char *cc_file_name=strdup(cc_file_string.c_str());
	ofstream fc(cc_file_name, ios::out);
	init(fc);
	process_operation(fc,period,cadency->length(),cycles,trace_reg);
	Cdfg2c::process_loopback(fc,Ordo2c_cdfg);
	Cdfg2c::process_aging(fc,Ordo2c_cdfg);
	fc << "  }" << "\n";
	fc << "  return 0;"						<< "\n";
	fc << "#include \"stimulipost.h\""						<< "\n";
	fc << "}"												<< endl;
	fc.close();
	ic_cycles.clear();

}

// end of: Ordo2c.cpp
