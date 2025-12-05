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

//	File:		ic.cpp
//	Purpose:	Generation of VHDL model of IC
//	Author:		Lab-sticc, UBS

#include "scheduling.h"
#include "operation.h"
#include "control.h"
#include "data.h"
#include "parser.h"
#include "ic.h"
#include "mem.h"
using namespace std;

//! Local storage for IC cycles.
static IC_Cycles ic_cycles;
//! Local storage for system clock.
static const Clock *ic_clk;
//! Local storage for cadency.
static const Cadency *ic_cadency;
//! Local storage for allocated registers
static const RegOut *regs_output;
//! Local storage for hardwire constant
vector <const Data *> hardwire_constant_list;

vector <string> sigs;		// store sigs value and next address each cycle (in binary)
vector <const Operator *> multi_function_operators;
vector <const Operator *> synchronous_operators;
vector <const Operator *> noFSM_operators;

//! Sort operations among cycles
//! @param input. node is a pointer to a CDFG node.
static void sort_operations(const CDFGnode *n)
{
	const Operation *o;									// the current operation
	const Data *d;										// the current Data
	Reg *r, *rr;										// registers
	long start, end;									// its starting and ending times
	long c_start, c_end, r_end;								// its starting and ending cycles
	long period = ic_clk->period();						// clock period
	
	switch (n->type())
	{
	case CDFGnode::DATA:
		// create hardwire constant list
		d = (const Data *)n;
		if ((d->type()==Data::CONSTANT) && (d->hardwire() == true))
			hardwire_constant_list.push_back(d);
		break;
	case CDFGnode::CONTROL:
		// nothing to do
		break;
	case CDFGnode::OPERATION:
		// compute ending cycle
		o = (const Operation *) n;						// dynamic link here
		//debug o->info();
		if (o->isChained()==-1)
		{
			start = n->start() % ic_cadency->length();			// get start
			c_start = start / period;							// compute starting cycle
			end = (start + o->chainingLength()) % ic_cadency->length();	// compute end
			c_end = end / period;
			if (end ==0)
			{
				if ((start + o->chainingLength()) ==ic_cadency->length())
				end = ic_cadency->length();
			} 
			r_end=end / period;
		}
		else
		{
			start = n->start() % ic_cadency->length();			// get start
			c_start = start / period;							// compute starting cycle
			end = (start + n->length()) % ic_cadency->length();	// compute end
			c_end = end / period;
			if (end ==0)
			{
				if (start + n->length()==ic_cadency->length())
				end = ic_cadency->length();	
			} 
			r_end=end / period;
		}
		if (r_end && r_end < c_start)
		{
			cerr << "Internal error: end(" << end << ") of operation " << n->name() << " preceeds start(" << start << ")" << endl;
			exit(1);
		}
		ic_cycles[c_start].start.push_back(o);			// add operation at start cycle c_start
		ic_cycles[c_end].end.push_back(o);// add operation at stop cycle c_end
		// add operation at end cycle c_end
		//debug o->info();
		int i;
		for (i = c_start+1; i<r_end; i++)
			ic_cycles[i].run.push_back(o);				// add operation at run cycles [c_start, r_end]
		// the operation has a bound operator: this is an operator instance
		const OperatorInstance *inst = o->inst();
		// the instance is the instance of an operator: get the operator
		const Operator *op = inst ? inst->oper() : 0;		
		if (op)
		{
			/* multifunction operator */
			if (op->isMultiFunction())
			{
				multi_function_operators.push_back(op);
			}
			/* synchronous operator */
			if (op->synchronous())
			{
				synchronous_operators.push_back(op);
			}
			/* no FSM operator */
			if (op->noFSM())
			{
				noFSM_operators.push_back(op);
			}

		}
		break;
		
	}
}

//! declaration of constant which represent hardwire constant 
static void hardwire_constant_declaration(ofstream &f, const string &tabul,bool bitwidth_aware, long bits)
{
	bool comment = true;
	int i;
	
	if (hardwire_constant_list.empty()==false)
	{
		for (int r_it = 0; r_it <hardwire_constant_list.size(); r_it++)
		{
			const Data *data = (const Data *)hardwire_constant_list[r_it];
			if (data->type() != Data::CONSTANT) continue;
			if (data->hardwire() == false) continue;	
			if (data->reg() == NULL) continue;	
			if (comment)
			{
				f << tabul << "-- hardwire constant\n";
				comment = !comment;
			}
			data->constant_declaration(f,tabul,bitwidth_aware,bits,data->reg()->name(),data->reg()->bitwidth());
		}
	}
}

/* GL 20/11/07 */
//! Build a signal name from an instance an a port.
//! @param Input. inst is a pointer to the instance.
//! @param Input. port is a pointer to the instance's port.
//! @result a string
string IC::signal_name(const OperatorInstance *inst, const string port_name) const
{
	string name = inst->instance_name(_sw);
	name += "_";
	name += port_name;
	return name;
}

/* Fin GL */
//! Build a bus name from a bus access.
//! @param Input. ba is a pointer to a bus access.
//! @result a string
string IC::bus_name(const BusAccess *ba) const
{
	string name("BUS_DONNEES_");
	name += Parser::itos(ba->id() + 1);		// buses are numbered from 1 to N in VHDL
	name += "_";
	/* Caaliph 27/06/2007 */
	if (_sw->cdfgs()||_sw->n_ordo() || _sw->spact())
		name = name + _vhdl_name;//add by Caaliph
	else /* End Caaliph */
		name += _sw->vhdl_prefix();
	return name;
}

//! Build a data bus name from a bus id (0..N-1).
//! @param Input. id is a us ID.
//! @result a string
string IC::bus_name(long id) const
{
	string name("BUS_DONNEES_");
	name += Parser::itos(id + 1);		// buses are numbered from 1 to N in VHDL
	name += "_";
	/* Caaliph 27/06/2007 */
	if (_sw->cdfgs()||_sw->n_ordo() || _sw->spact())
		name += _vhdl_name;//add by Caaliph
	else /* End Caaliph */
		name += _sw->vhdl_prefix();
	return name;
}

//! Build a bus name from a bus access.	// EJ 14/02/2008 : dynamic memory
//! @param Input. ba is a pointer to a bus access.
//! @result a string
string IC::address_bus_name(const BusAccess *ba) const
{
	string name("BUS_ADDRESS_");
	name += Parser::itos(ba->id() + 1);		// buses are numbered from 1 to N in VHDL
	name += "_";
	name += _sw->vhdl_prefix();
	return name;
}

//! Build a address bus name from a bus id (0..N-1). // EJ 14/02/2008 : dynamic memory
//! @param Input. id is a us ID.
//! @result a string
string IC::address_bus_name(long id) const
{
	string name("BUS_ADDRESS_");
	name += Parser::itos(id + 1); // buses are numbered from 1 to N in VHDL
	name += "_";
	name += _sw->vhdl_prefix();
	return name;
}

//! Map a register to a component port
/* 
	cast, resizing and conversion functions are defined in  fixed_pkg_c_body.vhdl
	register is a std_logic_vector (bit_with_aware), word (old mode)
	data may be signed, unsigned, sfixed ufixed or word

  Conversion need two step :

  1) register -> data : resizing may be need because data may be allocated on wider register (trunc and wrap)

   | register		| data				| conversion register->data								| mode
   |				|					|														|
   |	slv			| sfixed/ufixed		| to_s[u]fixed(slv(data'bitwidth-1 downto 0), integer_part, -fractional_part)		| bitwidth_aware
   |				|					|														|
   |	slv			| signed/unsigned	| [un]signed	(slv(data'bitwidth-1 downto 0))			| bitwidth_aware
   |				|					|														|
   |	word		| word				| nothing to do											| !bitwidth_aware
   |				|					|
   |   [un]signed	| [un]signed		| nothing to do (hardwire_constant)	 					|
   |				|					|	
   |   [u|s]fixed	| [u|s]fixed		| nothing to do (hardwire_constant)	 					|



  2/ data -> port : resizing may be need because operator's input may be wider (sign extend) 

   data may be signed, unsigned, sfixed ufixed or word
   port component is signed,unsigned, sfixed, ufixed or word  

   | data				|	port		|	conversion data->port					|	mode
   |					|				|											|
   | sfixed/ufixed		|	[un]signed  |	to_[un]signed(s[u]fixed, port'bitwidth)	| bitwidth_aware
   |					|				|											|
   | sfixed/ufixed		|	[u]sfixed	|	done in component library : operator_fp	| bitwidth_aware			
   |					|				|											|
   | signed/unsigned	|	[un]signed	|	resize([un]signed, port'bitwidth)		| bitwidth_aware
   |					|				|											|
   | signed/unsigned	|	[u]sfixed	|	to_fixed([un]signed, port'bitwidth)		| bitwidth_aware
   |					|				|											|
   | word				|	word		|	nothing to do							| !bitwidth_aware

  */

void IC::map_register_to_component_port(ofstream &f,const string &tabul, const string &signal_port_name,Reg *reg, const Data * data,const Port *port, const Operation*oper,const long current_cycle) const
{
	string reg_name;
	ostringstream convert_register_to_data;
	ostringstream convert_data_to_port;


	
	long fifo = reg->fifoStage();
	// is a register or a fifo ?
	if (fifo)
	{
		long oper_start_cycle = (oper->start() % _cadency->length())/_clk->period();
		long offset_run_time = (current_cycle-oper_start_cycle)*_clk->period();
        if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
		{
			long current_stage=(oper->start()+offset_run_time-data->start())/_cadency->length()+1;
			long reg_width=reg->bitwidth();
			long data_width=data->bitwidth();
			long index_range_max=0;
			long index_range_min=0;
			if (_sw->bitwidth_aware())
			{
				index_range_max=current_stage*data_width-1;
				index_range_min=(current_stage-1)*reg_width;
			}
			else
			{
				index_range_max=current_stage*reg_width-1;
				index_range_min=(current_stage-1)*reg_width;
			}
			reg_name = reg->name() + 
					"_out(" + Parser::itos(index_range_max) + " downto " + Parser::itos(index_range_min) +")"; // which fifo stage ?
		}
		else
			reg_name = reg->name() + 
					"(" + Parser::itos((oper->start()+offset_run_time-data->start())/_cadency->length()) + ")"; // which fifo stage ?
	}
	else 
	{
        if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
		{
			if ((data->type()==Data::CONSTANT) && (data->hardwire() == true))
				reg_name = reg->name();
			else
				reg_name = reg->name()+"_out";
		}
		else
			reg_name = reg->name();
	}
	if (_sw->bitwidth_aware())
	{
		//1) conversion register -> data
		if ((data->type()==Data::CONSTANT) && (data->hardwire() == true))
			convert_register_to_data << reg_name;
		else
		{
			//bitwidth_aware
			if ( data->fixedPoint() != -999) // data is sfixed/ufixed
			{
				//convert slv to sfixed/ufixed
				if (fifo)
					convert_register_to_data << "to_" << data->getStringSigned() << "( " << reg_name << "," << data->fixedPoint() -1 << " , -" << data->bitwidth() - data->fixedPoint() << ")";
				else
					convert_register_to_data << "to_" << data->getStringSigned() << "( " << reg_name << "(" << data->bitwidth() - 1 <<" downto 0)"  <<"," << data->fixedPoint() -1 << " , -" << data->bitwidth() - data->fixedPoint() << ")";
			}
			else
			{
				//convert slv to signed/unsigned
				if (data->bitwidth()==reg->bitwidth())
					convert_register_to_data << data->getStringSigned() << "(" << reg_name << ")";
				else
					convert_register_to_data << data->getStringSigned() << "(" << reg_name << "(" << data->bitwidth()-1 << " downto 0))";
			}
		}
		//2) conversion data -> port
		if ((port->fixedPoint() != -999 && data->fixedPoint() != -999) || 
			(port->fixedPoint() == -999 && data->fixedPoint() == -999))
		{
			// same arithmetic 
			if (port->fixedPoint() == -999)
			{
				//assume port'bitwidth is always greater or equal data'bitwidth
				//because we can't allocate a wider operation on a smaller operator
				//signed/unsigned -> [un]signed (resize([un]signed, port'bitwidth))
				if (data->bitwidth() < port->bitwidth()) //need sign extend ?
					convert_data_to_port << "resize(" << convert_register_to_data.str() << "," << port->bitwidth() << ")";
				else
					convert_data_to_port << convert_register_to_data.str();
			}
			else
			{
				//sfixed/ufixed -> [u]sfixed (nothing to do : all done in component library operator_fp)
				convert_data_to_port << convert_register_to_data.str();
			}
		}
		else
		{
			// different arithmetic: sfixed/ufixed -> [un]signed  
			//or [un]signed -> sfixed/ufixed
				convert_data_to_port << "to_" << port->getStringSigned() << "( " << convert_register_to_data.str() << "," << port->bitwidth() << ")";
		}
		f << tabul << signal_port_name << " <= " << convert_data_to_port.str() << ";\n";
	}
	else
	{
		//!bitwidth_aware : nothing to do data,port and register are word.
		f << tabul << signal_port_name << " <= " << reg_name << ";\n";
	}
}


/* 
   register is a std_logic_vector or word
   port component is signed,unsigned, sfixed, ufixed or word  
   data may be signed, unsigned, sfixed ufixed or word

  1) port -> data : cast and resizing may be needed because operator's output may be wider than data (rounding and quantization)

   | port		| data				| conversion port->data									|  mode
   |			|					|														|
   | [un]signed	| sfixed/ufixed		| to_s[u]fixed(signal, integer_part, -fractional_part,	| bitwidth_aware
   |			|					|		_quantization_mode, overflow_mode)				|
   |			|					|														|
   | [u]sfixed	| sfixed/ufixed		| resize(signal, integer_part, -fractional_part,		| bitwidth_aware
   |			|					|		_quantization_mode, overflow_mode)				|
   |			|					|														|
   | [un]signed	| signed/unsigned	| to_[un]signed	(signal, bitdidth)						| bitwidth_aware
   |			|					|														|
   | word		| word				| nothing to do											| !bitwidth_aware


  2) data -> register : resizing may be need because data may be allocated on wider register (sign extend)

  | data			| register 	|	conversion data->register	|	mode
  |					|			| 								|
  | sfixed/ufixed	|	slv		|	to_slv(data,register'size)	| bitwidth_aware
  |					|			|								|
  | signed/unsigned	|	slv		|	to_slv(data,register'size)	| bitwidth_aware
  |					|			|								|
  | word			|	word	|	nothing to do				| !bitwidth_aware

  */

void IC::map_component_port_to_register(ofstream &f,const string &tabul, const string &signal_port_name,Reg *reg, const Data * data,const Port *port,const Operation *oper) const
{

	string reg_name;
	ostringstream convert_data_to_register;
	ostringstream convert_port_to_data;
	// is a register or a fifo ?
	if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
		reg_name = reg->name()+"_in";
	else 
	{
		if (reg->fifoStage())
			reg_name = reg->name()+"(0)";
		else
			reg_name = reg->name();

	}
	if (_sw->bitwidth_aware())
	{
		//1) port -> data
		if ((port->fixedPoint() == -999) && (data->fixedPoint() != -999))
		{
			//[un]signed -> sfixed/ufixed
			convert_port_to_data << "to_" << data->getStringSigned() << "(" << signal_port_name << "," << data->fixedPoint() -1 << " , -" << data->bitwidth() - data->fixedPoint() << " , " << data->quantization() << " , " << data->overflow() << ")";
		}
		else if ((port->fixedPoint() != -999) && (data->fixedPoint() != -999))
		{
			//[u]sfixed -> sfixed/ufixed	
			convert_port_to_data <<  "resize(" << signal_port_name << "," << data->fixedPoint() -1 << " , -" << data->bitwidth() - data->fixedPoint() << " , " << data->quantization() << " , " << data->overflow() << ")";
		}
		else
		{
			//[un]signed -> signed/unsigned
			if (port->getStringSigned().compare(data->getStringSigned())==0)
			{
				if (data->bitwidth()==port->bitwidth())
					convert_port_to_data << signal_port_name;
				else
					convert_port_to_data << "resize(" << signal_port_name << "," << data->bitwidth() << ")";
			}
			else
				convert_port_to_data << "to_" << data->getStringSigned() << "(" << signal_port_name << "," << data->bitwidth() << ")";
		}
		//2) data -> register
		if ((port->fixedPoint() == -999) && (data->fixedPoint() == -999))
		{
			//signed/unsigned  -> std_logic_vector
			if (data->bitwidth()==reg->bitwidth())
				convert_data_to_register << "std_logic_vector(" <<  convert_port_to_data.str() << ")";
			else
				convert_data_to_register << "std_logic_vector( resize(" <<  convert_port_to_data.str() << "," << reg->bitwidth() << "))";
		}	
		else
		{
			//sfixed/ufixed -> std_logic_vector
			convert_data_to_register << "to_slv(" <<  convert_port_to_data.str() << "," << reg->bitwidth() << ")";
		}
		if (_sw->vhdl_type()==Switches::UT_FSM_D)
			f << tabul << reg_name << " := " << convert_data_to_register.str() << ";\n";
		else
			f << tabul << reg_name << " <= " << convert_data_to_register.str() << ";\n";
	}
	else
	{
		if (_sw->vhdl_type()==Switches::UT_FSM_D)
		{
			f << tabul << reg_name << " := " << signal_port_name << ";\n";
		}
		else
		{
			//!bitwidth_aware : nothing to do data,port and register are word.
			f << tabul << reg_name << " <= " << signal_port_name << ";\n";
		}
	}
	if (_debug_vhdl)
	{
		debug_vhdl(f,tabul,data);
	}
}




/* 
   port component is signed,unsigned, sfixed, ufixed or word  
   data may be signed, unsigned, sfixed ufixed or word

  1) port -> data : cast and resizing may be needed because operator's output may be wider than data (rounding and quantization)

   | port		| data				| conversion port->data									|  mode
   |			|					|														|
   | [un]signed	| sfixed/ufixed		| to_s[u]fixed(signal, integer_part, -fractional_part,	| bitwidth_aware
   |			|					|		_quantization_mode, overflow_mode)				|
   |			|					|														|
   | [u]sfixed	| sfixed/ufixed		| resize(signal, integer_part, -fractional_part,		| bitwidth_aware
   |			|					|		_quantization_mode, overflow_mode)				|
   |			|					|														|
   | [un]signed	| signed/unsigned	| to_[un]signed	(signal, bitdidth)						| bitwidth_aware
   |			|					|														|
   | word		| word				| nothing to do											| !bitwidth_aware


  2/ data -> port : resizing may be need because operator's input may be wider (sign extend) 

   data may be signed, unsigned, sfixed ufixed or word
   port component is signed,unsigned, sfixed, ufixed or word  

   | data				|	port		|	conversion data->port					|	mode
   |					|				|											|
   | sfixed/ufixed		|	[un]signed  |	to_[un]signed(s[u]fixed, port'bitwidth)	| bitwidth_aware
   |					|				|											|
   | sfixed/ufixed		|	[u]sfixed	|	done in component library : operator_fp	| bitwidth_aware			
   |					|				|											|
   | signed/unsigned	|	[un]signed	|	resize([un]signed, port'bitwidth)		| bitwidth_aware
   |					|				|											|
   | word				|	word		|	nothing to do							| !bitwidth_aware

  */

void IC::map_component_port_to_component_port(ofstream &f,const string &tabul, const Port *port_out, const string &signal_port_out,
											  const Data * data,const Port *port_in,const string &signal_port_in) const
{

	ostringstream convert_data_to_port_in;
	ostringstream convert_port_out_to_data;
	if (_sw->bitwidth_aware())
	{
		//1) port_out -> data
		if ((port_out->fixedPoint() == -999) && (data->fixedPoint() != -999))
		{
			//[un]signed -> sfixed/ufixed
			convert_port_out_to_data << data->getStringSigned() << "(" << signal_port_out << "," << data->fixedPoint() -1 << " , -" << data->bitwidth() - data->fixedPoint() << " , " << data->quantization() << " , " << data->overflow() << ")";
		}
		else if ((port_out->fixedPoint() != -999) && (data->fixedPoint() != -999))
		{
			//[u]sfixed -> sfixed/ufixed	
			convert_port_out_to_data <<  "resize(" << signal_port_out << "," << data->fixedPoint() -1 << " , -" << data->bitwidth() - data->fixedPoint() << " , " << data->quantization() << " , " << data->overflow() << ")";
		}
		else
		{
			//[un]signed -> signed/unsigned
			convert_port_out_to_data << data->getStringSigned() << "(" << signal_port_out << "," << data->bitwidth() << ")";
		}
		//2) conversion data -> port
		if ((port_in->fixedPoint() != -999 && data->fixedPoint() != -999) || 
			(port_in->fixedPoint() == -999 && data->fixedPoint() == -999))
		{
			// same arithmetic 
			if (port_in->fixedPoint() == -999)
			{
				//assume port'bitwidth is always greater or equal data'bitwidth
				//because we can't allocate a wider operation on a smaller operator
				//signed/unsigned -> [un]signed (resize([un]signed, port'bitwidth))
				if (data->bitwidth() < port_in->bitwidth()) //need sign extend ?
					convert_data_to_port_in << "resize(" << convert_port_out_to_data.str() << "," << port_in->bitwidth() << ")";
				else
					convert_data_to_port_in << convert_port_out_to_data.str();
			}
			else
			{
				//sfixed/ufixed -> [u]sfixed (nothing to do : all done in component library operator_fp)
				convert_data_to_port_in << convert_port_out_to_data.str();
			}
		}
		else
		{
			// different arithmetic: sfixed/ufixed -> [un]signed  
			//or [un]signed -> sfixed/ufixed
				convert_data_to_port_in << "to_" << port_in->getStringSigned() << "( " << convert_port_out_to_data.str() << "," << port_in->bitwidth() << ")";
		}
		f << tabul << signal_port_in << " <= " << convert_data_to_port_in.str() << ";\n";
	}
	else
	{
		//!bitwidth_aware : nothing to do data,port_in  and port_out are word.
		f << tabul << signal_port_in << " <= " << signal_port_out << ";\n";
	}
}


//! trace input of synchronous operator on file in vhdl simulation => for vqm estimator of J. Cong
void IC::trace_input_vhdl(ofstream &f,const string &tabul,const OperatorInstance *inst,const Data *data,long current_cycle) const
{
	const Reg *reg = data->reg();
	long fifo = reg->fifoStage();

	string trace_name = reg->name();
	if (fifo)
		trace_name += "(0)";
	f << tabul << "write(outputFile_Trace_line,STRING'(\"input " << data->name() << " (" << trace_name << ", cycle S"<< current_cycle << ")   :  \"));\n";
	if ((data->fixedPoint() == -999))
	{
		if ((data->getSigned()==Data::STD_LOGIC_VECTOR) || !(_sw->bitwidth_aware()))
			f << tabul <<"write(outputFile_Trace_line,word_to_int(" << trace_name << "));\n";
		else 
			f << tabul << "write(outputFile_Trace_line,TO_INTEGER(" << trace_name << "));\n";
	}
	else
	{
		int fractionalFactor = 1<<(data->bitwidth() - data->fixedPoint());
		if (data->getSigned()==Data::SFIXED)
			f << tabul << "write(outputFile_Trace_line,sfixed_to_real(" << trace_name << "," << fractionalFactor <<"));\n";
		else 
			f << tabul << "write(outputFile_Trace_line,ufixed_to_real(" << trace_name<< "," << fractionalFactor <<"));\n";			
	}
	f << tabul << "writeline(ioFile_Trace_"<< inst->instance_name() <<",outputFile_Trace_line);\n";
}


//! trace output of synchronous operator on file in vhdl simulation => for vqm estimator of J. Cong
void IC::trace_output_vhdl(ofstream &f,const string &tabul,const OperatorInstance *inst,const Data *data,long current_cycle,string trace_name) const
{
	const Reg *reg = data->reg();
	
	f << tabul << "write(outputFile_Trace_line,STRING'(\"output " << data->name() << " (" << reg->name() << ", cycle S"<< current_cycle << ")   :  \"));\n";
	if ((data->fixedPoint() == -999))
	{
		if ((data->getSigned()==Data::STD_LOGIC_VECTOR) || !(_sw->bitwidth_aware()))
			f << tabul <<"write(outputFile_Trace_line,word_to_int(" << trace_name << "));\n";
		else 
			f << tabul << "write(outputFile_Trace_line,TO_INTEGER(" << trace_name << "));\n";
	}
	else
	{
		int fractionalFactor = 1<<(data->bitwidth() - data->fixedPoint());
		if (data->getSigned()==Data::SFIXED)
			f << tabul << "write(outputFile_Trace_line,sfixed_to_real(" << trace_name << "," << fractionalFactor <<"));\n";
		else 
			f << tabul << "write(outputFile_Trace_line,ufixed_to_real(" << trace_name<< "," << fractionalFactor <<"));\n";			
	}
	f << tabul << "writeline(ioFile_Trace_"<< inst->instance_name() <<",outputFile_Trace_line);\n";
}


//! trace assigned register value on file for debug in vhdl simulation 
void IC::debug_vhdl(ofstream &f,const string &tabul,const Data *dest_data) const
{
	const Reg *dest_reg = dest_data->reg();
	long dest_fifo = dest_reg->fifoStage();

	string trace_name = dest_reg->name();
	if (dest_fifo)
		trace_name += "(0)";
	f << tabul << "write(outputFile_Trace_line,STRING'(\">" << trace_name << "  :  \"));\n";
	if ((dest_data->fixedPoint() == -999))
	{
		if ((dest_data->getSigned()==Data::STD_LOGIC_VECTOR) || !(_sw->bitwidth_aware()))
			f << tabul <<"write(outputFile_Trace_line,word_to_int(" << trace_name << "));\n";
		else 
			f << tabul << "write(outputFile_Trace_line,TO_INTEGER(" << trace_name << "));\n";
	}
	else
	{
		int fractionalFactor = 1<<(dest_data->bitwidth() - dest_data->fixedPoint());
		if (dest_data->getSigned()==Data::SFIXED)
			f << tabul << "write(outputFile_Trace_line,sfixed_to_real(" << trace_name << "," << fractionalFactor <<"));\n";
		else 
			f << tabul << "write(outputFile_Trace_line,ufixed_to_real(" << trace_name<< "," << fractionalFactor <<"));\n";			
	}
	f << tabul << "writeline(outputFile_Trace,outputFile_Trace_line);\n";
}

/* assign operation : register ->  data -> data ->register							*/
/* target data bitwidth  may be less or equal source data bitwidth : trunc and wrap otherwise specific round_operator are in cdfg */
/* allocated register bitwidth may be greater than data bitwidth 
/* so we only have to do sign extend on register if necessary */ 
void IC::assign_register_to_register(ofstream &f,const string &tabul,const Data *dest_data, const Operation *oper,const long current_cycle) const
{
	ostringstream src_reg_name;
	// value is put on register dest_reg
	const Reg *dest_reg = dest_data->reg();
	long dest_fifo = dest_reg->fifoStage();
	// value is coming from another register src_reg
	const Data *src_data = (const Data *) (*oper->predecessors.begin())->source;	// get source Data
	const Reg *src_reg = src_data->reg();
	long src_fifo = src_reg->fifoStage();
	string dest_reg_name,assign_op;
	
	if (_sw->vhdl_type()==Switches::UT_FSM_D)
		assign_op = ":=";
	else
		assign_op = "<=";

	if ((_sw->vhdl_type()==Switches::UT_FSM_D) || (_sw->vhdl_type()==Switches::UT_FSM_S))
	{
		if (dest_fifo)
			dest_reg_name = dest_reg->name()+"(0)";
		else
			dest_reg_name = dest_reg->name();
		src_reg_name << src_reg->name();
		if (src_fifo)
		{
			src_reg_name << "(" << (oper->start()-src_data->start())/_cadency->length() << ")"; 
		}
	}
	else
	{
		dest_reg_name = dest_reg->name()+"_in";
		src_reg_name << src_reg->name()+"_out";
		if (src_fifo)
		{
			long current_stage=(oper->start()-src_data->start())/_cadency->length()+1;
			long width=src_reg->bitwidth();
			long index_range_max=current_stage*width-1;
			long index_range_min=(current_stage-1)*width;
			src_reg_name << "(" << index_range_max << " downto " << index_range_min << ")"; 
		}
	}

	if (!_sw->bitwidth_aware())
			f << tabul << dest_reg_name << " " << assign_op << src_reg_name.str() << ";\n";
	else
	{
		if (dest_data->bitwidth()==src_data->bitwidth())
		{

			if (dest_reg->bitwidth()>dest_data->bitwidth())
			{
				f <<  tabul << dest_reg_name << " " << assign_op << " (others => '0');\n";
			}
			f << tabul << dest_reg_name << "(" << dest_data->bitwidth()-1 << " downto 0) " << assign_op << " " << src_reg_name.str() << "(" << dest_data->bitwidth()-1 << " downto 0);\n";
		}
		else if (dest_data->bitwidth()>src_data->bitwidth())
		{
			if (dest_data->getSigned() == Data::UNSIGNED ||  dest_data->getSigned() == Data::UFIXED)
				f << tabul << dest_reg_name << " " << assign_op << " ( others => ('0'));\n";
			else
				f << tabul << dest_reg_name << " " << assign_op << " ( others => " << src_reg_name.str() << "(" << src_data->bitwidth()-1 << "));\n";
			f << tabul << dest_reg_name << "(" << src_data->bitwidth()-1 << " downto 0) " << assign_op << " " << src_reg_name.str() << "(" << src_data->bitwidth()-1 << " downto 0);\n";
		}
		else
		{
			if (dest_data->getSigned() == Data::UNSIGNED ||  dest_data->getSigned() == Data::UFIXED)
				f << tabul << dest_reg_name << " " << assign_op << " ( others => ('0'));\n";
			else
				f << tabul << dest_reg_name << " " << assign_op << " ( others => " << src_reg_name.str() << "(" << dest_data->bitwidth()-1 << "));\n";
			f << tabul << dest_reg_name << "(" << dest_data->bitwidth()-1 << " downto 0) " << assign_op << " " << src_reg_name.str() << "(" << dest_data->bitwidth()-1 << " downto 0);\n";
		}
	}
	if (_debug_vhdl)
		debug_vhdl(f,tabul,dest_data);
}

/* sliceread operation : register(slice) ->  data -> data ->register							*/
/* target data bitwidth  may be less or equal source data bitwidth : trunc and wrap otherwise specific round_operator are in cdfg */
/* allocated register bitwidth may be greater than data bitwidth 
/* so we only have to do sign extend on register if necessary */ 
void IC::sliceread_register_to_register(ofstream &f,const string &tabul,const Data *dest_data, const Operation *oper) const
{
	CDFGnode *operand0_node = oper->predecessors[0]->source;
	Data *operand0 = (Data *)operand0_node;
//	long offset=operand0->getOffset();

	CDFGnode *index_node = oper->predecessors[1]->source;
	Data *index_operand = (Data *)index_node;
	CDFGnode *rang_node = oper->predecessors[2]->source;
	Data *rang_operand = (Data *)rang_node;
	if (index_operand->type()==Data::CONSTANT)
	{
		const Reg *dest_reg = dest_data->reg();
		long dest_fifo = dest_reg->fifoStage();
		string dest_reg_name;
		if ((_sw->vhdl_type()==Switches::UT_FSM_D) || (_sw->vhdl_type()==Switches::UT_FSM_S))
		{
			if (dest_fifo)
				dest_reg_name = dest_reg->name()+"(0)";
			else
				dest_reg_name = dest_reg->name();
		}
		else
			dest_reg_name = dest_reg->name()+"_in";
		const Reg *src_reg = operand0->reg();
		long src_fifo = src_reg->fifoStage();
		ostringstream src_reg_name;
		string assign_op;
		if (_sw->vhdl_type()==Switches::UT_FSM_D)
			assign_op = ":=";
		else
			assign_op = "<=";

		if ((_sw->vhdl_type()==Switches::UT_FSM_D) || (_sw->vhdl_type()==Switches::UT_FSM_S))
		{
			src_reg_name << src_reg->name();
			if (src_fifo)
			{
				src_reg_name << "(" << (oper->start()-operand0->start())/_cadency->length() << ")";
			}
		}
		else
		{
			src_reg_name << src_reg->name()+"_out";
			if (src_fifo)
			{
				src_reg_name << "(" << (oper->start()-operand0->start())/_cadency->length() << ")";
			}
		}
		if (dest_reg->bitwidth()>dest_data->bitwidth())
		{
			f <<  tabul << dest_reg_name << " " << assign_op << " (others => '0');\n";
		}
		f <<  tabul << dest_reg_name << "(" <<  dest_data->bitwidth()-1 << " downto 0) " << assign_op << " " << src_reg_name.str() << "(" << rang_operand->value()-1+index_operand->value()/* -offset */ << " downto " << index_operand->value() /* -offset */ << ");\n";
		if (_debug_vhdl)
			debug_vhdl(f,tabul,dest_data);
	}
}


void IC::gen_enable_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const
{
	
	const Operator *op;									//!< an operator
	const Function *func;								//!< a function
	const OperatorInstance *inst;						//!< an operator instance

	// the operation has a bound operator: this is an operator instance
	inst = oper->inst();
	// the instance is the instance of an operator: get the operator
	op = inst ? inst->oper() : 0;
	// the operation has a function
	func = oper->function();
	
	if (op)
	{
		if((op->synchronous()) && (op->synCOM()) )
		{
			f << tabul << "-- enable of operation " << oper->name() << "\n";
			if (op->enable_polarity() == Operator::LOW)
				f << tabul << signal_name(inst, op->enable_name()) << "<= '0';\n";
			else
				f << tabul << signal_name(inst, op->enable_name()) << "<= '1';\n";
		}
	}
}

void IC::gen_cmd_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const
{
	
	const Operator *op;									//!< an operator
	const Function *func;								//!< a function
	const OperatorInstance *inst;						//!< an operator instance
	long ctrl_width;
	string signal;										//!< a signal name
	bool start;

	// the operation has a bound operator: this is an operator instance
	inst = oper->inst();
	// the instance is the instance of an operator: get the operator
	op = inst ? inst->oper() : 0;
	// the operation has a function
	func = oper->function();
	
	if (op)
	{
		/* multifunction operator */
		if (op->isMultiFunction())
		{
			ctrl_width = ceil(log((double)(op->getCodes()->size())) /log(2.0) ) - 1;/*GL 26/11/07: correction formule*/
			f << tabul << "-- cmd of operation " << oper->name() << "\n";
			signal = signal_name(inst, op->ctrl_name());
			if (ctrl_width)
				f << tabul << signal << " <= " << "int_to_word(" << op->getCtrlCode(oper->function()->symbolic_function()) << ") (" << ctrl_width << " downto 0);\n";
			else
				f << tabul << signal << " <= " << "'" << op->getCtrlCode(oper->function()->symbolic_function()) << "';\n";
		}
		if (op->noFSM())
		{
			long c_start = oper->start() % ic_cadency->length();// get start
			c_start = c_start / _clk->period();							// compute starting cycle
			f << tabul <<"-- FSM code of operation " << oper->name() << " for this cycle\n";
			f << tabul << signal_name(inst, op->command_name()) << " <= \"" << op->give_code(oper->function(), current_cycle-c_start) << "\";\n";
		}
	}
}

void IC::trace_input_reg_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const
{
	
	const Operator *op;									//!< an operator
	const OperatorInstance *inst;						//!< an operator instance
	string signal;										//!< a signal name
	Graph<CDFGnode, CDFGedge>::EDGES::const_iterator it;//!< a Graph node iterator
	Graph<CDFGnode, CDFGedge>::EDGES::const_iterator it_pred;//!< a Graph node iterator
	bool start;
	const CDFGedge *edge;								//!< a CDFG edge
	CDFGnode *node;						//!< CDFG nodes
	const Data *data;									//!< a data node
	Reg *reg;											//!< a register

	// the operation has a bound operator: this is an operator instance
	inst = oper->inst();
	// the instance is the instance of an operator: get the operator
	op = inst ? inst->oper() : 0;
	
	if ((op) && op->synchronous())
	{
		// The operation has inputs, they are the predecessors of the CDFG node.
		// These inputs are stored in the declaration order of the function symbolic ports.
		for (it = oper->predecessors.begin(), start = false; it != oper->predecessors.end(); it++)
		{
			if (!start)
			{
				f << tabul <<"-- trace inputs of operation " << oper->name() << "\n";
				start = true;
			}
			edge = *it;					// get edge
			node = edge->source;		// get source node
			switch (node->type())
			{
				case CDFGnode::DATA:
					data = (const Data *) node; // dynamic link: get the Data node
					if (!Reg::is_chained(data)) 
					{
						trace_input_vhdl(f,tabul,inst,data,current_cycle);
					}
					break;
				case CDFGnode::CONTROL:
				// nothing to do
					break;
				case CDFGnode::OPERATION:
					// error
					cerr << "Internal error: no variable between operations " << oper->name() << " and " << node->name() << endl;
					exit(1);
			}
		}
	}
}

void IC::gen_input_reg_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const
{
	
	const Operator *op;									//!< an operator
	const Function *func;								//!< a function
	const OperatorInstance *inst;						//!< an operator instance
	string signal;										//!< a signal name
	Graph<CDFGnode, CDFGedge>::EDGES::const_iterator it;//!< a Graph node iterator
	Graph<CDFGnode, CDFGedge>::EDGES::const_iterator it_pred;//!< a Graph node iterator
	bool start;
	long pi;											//!< a port index
	const CDFGedge *edge;								//!< a CDFG edge
	CDFGnode *node;						//!< CDFG nodes
	const Data *data;									//!< a data node
	Reg *reg;											//!< a register
	const Port *port;									//!< a port

	// the operation has a bound operator: this is an operator instance
	inst = oper->inst();
	// the instance is the instance of an operator: get the operator
	op = inst ? inst->oper() : 0;
	// the operation has a function
	func = oper->function();
	
	if (op)
	{
		// The operation has inputs, they are the predecessors of the CDFG node.
		// These inputs are stored in the declaration order of the function symbolic ports.
		for (it = oper->predecessors.begin(), pi = 0, start = false; it != oper->predecessors.end(); it++, pi++)
		{
			if (!start)
			{
				f << tabul <<"-- inputs of operation " << oper->name() << "\n";
				start = true;
			}
			edge = *it;					// get edge
			node = edge->source;		// get source node
			switch (node->type())
			{
				case CDFGnode::DATA:
					data = (const Data *) node; // dynamic link: get the Data node
					if (Reg::is_chained(data)) 
					{
						// now find pi-mapped operator input port
						port = op->getInputPortBoundTo(pi);
						// build signal name
						signal = port->signal_name(inst, _sw);
						//find chaining operator
						for (it_pred = data->predecessors.begin(); it_pred != data->predecessors.end(); it_pred++)
						{
								const CDFGedge *edge_pred = *it_pred;
								const CDFGnode *n_pred = edge_pred->source;
								if (n_pred->type()!=CDFGnode::OPERATION) continue;
								const Operation *o_pred = (const Operation *) n_pred;
								if (o_pred->isChained()!=-1) continue;
								const OperatorInstance *inst_chained=o_pred->inst();
								const Operator *op_pred;							
								op_pred = inst_chained ? inst_chained->oper() : 0;
								const Port *port_out;
								if (op_pred)
								{
									port_out = op_pred->getOutputPortBoundTo(o_pred->getOutputPos(data));
									map_component_port_to_component_port(f,tabul,port_out,port_out->signal_name(inst_chained, _sw),
											  data,port,signal);
								}
						}
					}
					else
					{
						// the source register is the one inside which the data is stored at this clock cycle
						reg = data->reg();
						// now find pi-mapped operator input port
						port = op->getInputPortBoundTo(pi);
						// build signal name
						signal = port->signal_name(inst, _sw);
						map_register_to_component_port(f,tabul,signal,reg,data,port,oper,current_cycle);
					}
					break;
				case CDFGnode::CONTROL:
				// nothing to do
					break;
				case CDFGnode::OPERATION:
					// error
					cerr << "Internal error: no variable between operations " << oper->name() << " and " << node->name() << endl;
					exit(1);
			}
		}
	}
}


void IC::gen_static_write_to_memory(ofstream &f, long cycle_write)
{
    const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	Reg *reg;		
	bool start;									//!< a boolean
	const BankUse *bank_use;							//!< a bank use
	const BankAddress *bank_address;					//!< a bank address

	/* static write to memory */
	bus_cycle = _buses->getBusCycle(cycle_write);
    if (bus_cycle)
    for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
    {
		bus_access = *ba_it;
		if (bus_access->type() != BusAccess::OUT) continue;
		if (bus_access->source() == BusAccess::BANK_USE)
		{
			bank_use = (const BankUse *) bus_access->addr();
            bank_address = bank_use->address();
            data = bank_address->data();
            if (bank_use->_is_dynamic!=NULL)
                 data = bank_use->_is_dynamic;
            reg = data->reg();
            if (!data->isADynamicAccess())
            {
				if (!start)
				{
					f << "        -- static write to memory\n";
                    start = true;
                }
                f <<  data->get_data_bus_assignment("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),_sw);
            }
        }
    }
}

void IC::gen_output_on_buses(ofstream &f, long cycle_write)
{
	const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	Reg *reg;		
	bool start;									//!< a boolean

	bus_cycle = _buses->getBusCycle(cycle_write);
	if (bus_cycle)
	{
		for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
		{
			bus_access = *ba_it;
			if (bus_access->type() != BusAccess::OUT) continue;
			if (bus_access->source() == BusAccess::IO)
			{
				data = (const Data *) bus_access->addr();
				reg = data->reg();
				if (!reg) {
					f << " -- MISSING REGISTER ASSOCIATED TO \"" << bus_name(bus_access) << "\" \n";
				} else {
				if (!data->isADynamicAccess())
				{
					if (!start)
					{
						f << "        -- outputs on buses\n";
						start = true;
					}
					f << data->get_data_bus_assignment("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),_sw);
				}
				}
			}
		}
	}
}


void IC::gen_dynamic_address(ofstream &f, long cycle_adress)
{
	const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	Reg *reg;		
	bool start;									//!< a boolean
	const BankUse *bank_use;							//!< a bank use
	const BankAddress *bank_address;					//!< a bank address
	//dynamic address
	bus_cycle = _abuses->getBusCycle(cycle_adress);
	if (bus_cycle)
	for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
	{
		bus_access = *ba_it;
		if (bus_access->source() == BusAccess::BANK_USE)
		{
			bank_use = (const BankUse *) bus_access->addr();
			if (!bank_use->_is_dynamic->isADynamicAccess()) continue;
			bank_address = bank_use->address();
			data = bank_use->_is_dynamic;
		}
		const CDFGedge *e = data->predecessors[0];
		const CDFGnode *n = e->source;
		const Operation *o = (const Operation *) n;
					
		if ((o->function_name() == "mem_write") ||
			(o->function_name() == "mem_read"))
			// DH 22/09/2008 : 
		{
				CDFGnode *index=o->predecessors[0]->source; //index
				const Data* data_index=(const Data*)index;
				reg=data_index->reg();
				long dynamic_address_bus = ((const Data*) o->successors[0]->target)->dynamic_access()->_dynamic_address_bus;
				f << "        " << "-- dynamic address\n";
				f <<  data_index->get_data_bus_assignment("        ",_bits,address_bus_name(dynamic_address_bus),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),_sw);
		}
	}
}


void IC::gen_dynamic_memory_write(ofstream &f, long cycle_write)
{
	const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	Reg *reg;		
	bool start;									//!< a boolean
	const BankUse *bank_use;							//!< a bank use
	const BankAddress *bank_address;					//!< a bank address
	// dynamic write to memory
	bus_cycle = _buses->getBusCycle(cycle_write);
	if (bus_cycle)
	for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
	{
		bus_access = *ba_it;
		if (bus_access->source() == BusAccess::BANK_USE)
		{
			bank_use = (const BankUse *) bus_access->addr();
			if ((bank_use->_is_dynamic==NULL) || (!bank_use->_is_dynamic->isADynamicAccess())) continue;
			bank_address = bank_use->address();
			data = bank_use->_is_dynamic;
			const CDFGedge *e = data->predecessors[0];
			const CDFGnode *n = e->source;
			const Operation *o = (const Operation *) n;
						
			if (o->function_name() == "mem_write")
			{
				CDFGnode *write_value = o->predecessors[1]->source;
				const Data* data_write_value=(const Data*)write_value;
				reg = data_write_value->reg();
				f << "        " << "-- dynamic write to memory\n";
				f << data_write_value->get_data_bus_assignment("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),_sw);
			}
		}
	}
}

void IC::gen_dynamic_memory_read(ofstream &f, long cycle_read)
{
	const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	long fifo;											//!< a fifo index
	Reg *reg;		
	bool start;									//!< a boolean
	const BankUse *bank_use;							//!< a bank use
	const BankAddress *bank_address;					//!< a bank address
	// dynamic read to memory 
	bus_cycle = _buses->getBusCycle(cycle_read);
	if (bus_cycle)
	{
		for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
		{
			bus_access = *ba_it;
			//cout << " cycle " << cycle << " address bus " << bus_name(bus_access) << endl;
			if (bus_access->source() == BusAccess::BANK_USE)
			{
				bank_use = (const BankUse *) bus_access->addr();
				if ((bank_use->_is_dynamic==NULL) || (!bank_use->_is_dynamic->isADynamicAccess())) continue;
				bank_address = bank_use->address();
				data = bank_use->_is_dynamic;
				const CDFGedge *e = data->predecessors[0];
				const CDFGnode *n = e->source;
				const Operation *o = (const Operation *) n;
					
				if (o->function_name() == "mem_read")
				{
					CDFGnode *read_value = o->successors[0]->target;
					const Data* data_read_value=(const Data*)read_value;
					reg = data_read_value->reg();
					fifo= reg->fifoStage();
					//long bus_num = ((const Data*) o->successors[0]->target)->dynamic_access()->_dynamic_data_bus;
					f << "        " << "-- dynamic read to memory " << "\n";
					string assign_op;
					if (_sw->vhdl_type()==Switches::UT_FSM_D)
						assign_op = ":=";
					else
						assign_op = "<=";
					if (!fifo)
						f <<	((const Data*)read_value)->get_data_bus_read("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),assign_op,_sw);
					else
					{
						if ((_sw->vhdl_type()!=Switches::UT_ROM) && (_sw->vhdl_type()!=Switches::UT_FSM_R)) //done inside component reg_fifo_op
						{
							if (fifo == 1)
								f << "        " << reg->name() << "(1) <= " << reg->name() << "(0);\n";
							else
								f << "        " << "for i in " << fifo << " downto 1" << " loop " << reg->name() << "(i) <= " << reg->name() << "(i-1); end loop;\n";
						}
						f <<	((const Data*)read_value)->get_data_bus_read_fifo("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),assign_op,_sw);						
					}
				}
			}
		}
	}
}

void IC::gen_static_memory_read(ofstream &f, long cycle_read)
{
	const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	long fifo;											//!< a fifo index
	Reg *reg;		
	bool start;									//!< a boolean
	const BankUse *bank_use;							//!< a bank use
	const BankAddress *bank_address;					//!< a bank address

	bus_cycle = _buses->getBusCycle(cycle_read);
	if (bus_cycle)
	{
		for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
		{
			bus_access = *ba_it;
			if (bus_access->type() != BusAccess::IN) continue;
			//cout << " cycle " << cycle << " bus " << bus_name(bus_access) << endl;
			if (bus_access->source() == BusAccess::BANK_USE)
			{
				bank_use = (const BankUse *) bus_access->addr();
				bank_address = bank_use->address();
				data = bank_address->data();
				if (bank_use->_is_dynamic!=NULL)
					continue;
				reg = data->reg();
				fifo= reg->fifoStage();
			}
			else continue;
			
			if (!start)
			{
				f << "        -- static memory read from buses \n";
				start = true;
			}
			string assign_op;
			if (_sw->vhdl_type()==Switches::UT_FSM_D)
				assign_op = ":=";
			else
				assign_op = "<=";
			if (!fifo)
			{
				f << data->get_data_bus_read("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),assign_op,_sw);
			}
			else
			{
				if (!reg->_addr)
				{
					if ((_sw->vhdl_type()!=Switches::UT_ROM) && (_sw->vhdl_type()!=Switches::UT_FSM_R))//done inside component reg_fifo_op
					{
						if (fifo == 1)
							f << "        " << reg->name() << "(1) <= " << reg->name() << "(0);\n";
						else
							f << "        " << "for i in " << fifo << " downto 1" << " loop " << reg->name() << "(i) <= " << reg->name() << "(i-1); end loop;\n";
					}
					reg->_addr = (bool*)true;
				}
				f << data->get_data_bus_read_fifo("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),assign_op,_sw);
			}
		}
	}
}

void IC::gen_input_from_buses(ofstream &f, long cycle_read)
{
	const BusCycle *bus_cycle;							//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;			//!< a bus accesses iterator
	const BusAccess *bus_access;						//!< a bus access
	const Data *data;									//!< a data node
	long fifo;											//!< a fifo index
	Reg *reg;		
	bool start;									//!< a boolean
	string assign_op;
	//!< a register
	// INPUTS from buses
	bus_cycle = _buses->getBusCycle(cycle_read);
	if (bus_cycle)
	{
		for (ba_it = bus_cycle->accesses.begin(), start = false; ba_it != bus_cycle->accesses.end(); ba_it++)
		{
			bus_access = *ba_it;
			if (bus_access->type() != BusAccess::IN) continue;
			//cout << " cycle " << cycle << " bus " << bus_name(bus_access) << endl;
			if (bus_access->source() == BusAccess::IO)
			{
				data = (const Data *) bus_access->addr();
				//skip reg no for input only used in delay
				if ((data->type()==Data::INPUT ) && (data->successors.empty()) && (data->readvar()))
				{
					continue;
				}
				reg = data->reg();
				fifo= reg->fifoStage();
			}
			else continue;
				
			if (_sw->vhdl_type()==Switches::UT_FSM_D)
				assign_op = ":=";
			else
				assign_op = "<=";
			if (!start)
			{
				f << "        -- inputs from buses\n";
				start = true;
			}
			if (!fifo)
			{
				f << data->get_data_bus_read("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),assign_op,_sw);
			}
			else
			{
				if (!reg->_addr)
				{
					if ((_sw->vhdl_type()!=Switches::UT_ROM) && (_sw->vhdl_type()!=Switches::UT_FSM_R)) //done inside component reg_figo_op
					{
						if (fifo == 1)
							f << "        " << reg->name() << "(1) " << assign_op <<" " << reg->name() << "(0);\n";
						else
							f << "        " << "for i in " << fifo << " downto 1" << " loop " << reg->name() << "(i) " << assign_op << " " << reg->name() << "(i-1); end loop;\n";
					}
					reg->_addr = (bool*)true;
				}
				f << data->get_data_bus_read_fifo("        ",_bits,bus_name(bus_access),_sw->bitwidth_aware(),reg->name(),reg->bitwidth(),assign_op,_sw);
			}
		}
	}

}



static long long_to_nb_bit (long x)
{
	long n = 1;

	if ( x == 1 || x == 0 )  	  /* Le log a base 2 de 1 est 0					 */
	{
		return(0);
	}
	while ( ((x / 2) != 1) || ((x % 2) != 0) )    /* On calcul le logarithme a base 2		 */
	{
		n++;
		if ( (x % 2) != 0 )
		{
			x = x / 2 + 1;
		}
		else
		{
			x = x / 2;
		}
	}
	return(n);		   /* Retourne le logarithme a base 2			 */
}

static long puissance(long base, long n)
{
	long i,p=1;
	for ( i=1;i<=n;i++ )
	{
		p=p*base;
	}
	return(p);
}

static string long_to_string_bin(long num, long nbbit)
{
	string s;
	long resul,i;

	s="";
	resul= num % (puissance(2,nbbit));

	for ( i=1;i<=nbbit;i++ )
	{
		if ( resul / (puissance(2,nbbit-i)) == 1 )
		{
			s+="1";
		}
		else
		{
			s+="0";
		}
		resul=resul % (puissance(2,nbbit-i));
	}
	return(s);
}

//! Build a hexa string from a binary string.
//! @param Input. bin is a binary string.
//! @result a string
static string binary_to_hexa(string bin)
{
	long n = bin.length()%4;
	if (n) n = 4 - n;
	while (n--) bin = "0"+bin;
	string c = "";
	for (n = 0; n < bin.length(); n+=4)
	{
		string t;
		ostringstream myStream;
		myStream << bin.c_str()[n] << bin.c_str()[n+1] << bin.c_str()[n+2] << bin.c_str()[n+3];
		t = myStream.str();
		if (t == "0000") c += "0";
		else if (t ==  "0001") c += "1";
		else if (t ==  "0010") c += "2";
		else if (t ==  "0011") c += "3";
		else if (t ==  "0100") c += "4";
		else if (t ==  "0101") c += "5";
		else if (t ==  "0110") c += "6";
		else if (t ==  "0111") c += "7";
		else if (t ==  "1000") c += "8";
		else if (t ==  "1001") c += "9";
		else if (t ==  "1010") c += "A";
		else if (t ==  "1011") c += "B";
		else if (t ==  "1100") c += "C";
		else if (t ==  "1101") c += "D";
		else if (t ==  "1110") c += "E";
		else if (t ==  "1111") c += "F";
	}
	return c;
}

void IC::gen_rom_fsm(long cycles,long period,bool one_hot_coding)
{
	long oi;											//!< an operation index
	long cycle_start,cycle_stop,cycle;
	Reg *reg;
	int r_it,i,operator_it;

	// sigs
	for (cycle = 0; cycle < cycles; cycle++)
	{
		cycle_start =(cycle * period) % _cadency->length();
		cycle_stop = (cycle_start + period) % _cadency->length();
		//
		string s = "";
		//enable of synchronous operators
		for (operator_it = 0; operator_it <synchronous_operators.size(); operator_it++)
		{
			const Operator *synchronous_operator=synchronous_operators[operator_it];
			//enable at start cycle ?
			bool enable_operator=false;
			for (oi = 0; oi < ic_cycles[(cycle/* +1 */) % cycles].start.size() && (enable_operator==false); oi++)
			{
				// get the current operation
				const Operation *oper = ic_cycles[(cycle /* +1 */) % cycles].start[oi];
	        	const Operator *op;									//!< an operator
				// the operation has a bound operator: this is an operator instance
				const OperatorInstance *inst=oper->inst();//!< an operator instance
				// the instance is the instance of an operator: get the operator
				op = inst ? inst->oper() : 0;
				if (op==synchronous_operator) enable_operator=true;
			}
			//enable at run cycle ?
			if (!enable_operator)
			{
				for (oi = 0; oi < ic_cycles[(cycle /* +1 */ ) % cycles].run.size() && (enable_operator==false); oi++)
				{
					// get the current operation
					const Operation *oper = ic_cycles[(cycle /* +1 */) % cycles].run[oi];
	        		const Operator *op;									//!< an operator
					// the operation has a bound operator: this is an operator instance
					const OperatorInstance *inst=oper->inst();//!< an operator instance
					// the instance is the instance of an operator: get the operator
					op = inst ? inst->oper() : 0;
					if (op==synchronous_operator) enable_operator=true;
				}
			}
			if (enable_operator)
			{
				if(synchronous_operator->enable_polarity() == Operator::LOW)
					s += "0";
				else 
					s += "1";
			}
			else
			{
				if(synchronous_operator->enable_polarity() == Operator::LOW)
					s += "1";
				else 
					s += "0";
			}
		}
		//cmd of multi_function operator
		for (operator_it = 0; operator_it <multi_function_operators.size(); operator_it++)
		{
			const Operator *multifunction_operator=multi_function_operators[operator_it];
			//cmd at start cycle ?
			string function_name = "";

			for (oi = 0; oi < ic_cycles[(cycle/* +1 */) % cycles].start.size() && (function_name==""); oi++)
			{
				// get the current operation
				const Operation *oper = ic_cycles[(cycle /* +1 */) % cycles].start[oi];
	        	const Operator *op;									//!< an operator
				// the operation has a bound operator: this is an operator instance
				const OperatorInstance *inst=oper->inst();//!< an operator instance
				// the instance is the instance of an operator: get the operator
				op = inst ? inst->oper() : 0;
				if (op==multifunction_operator) function_name=oper->function()->symbolic_function();
			}
			//cmd at run cycle ?
			if (function_name=="")
			{
				for (oi = 0; oi < ic_cycles[(cycle /* +1 */) % cycles].run.size() && (function_name==""); oi++)
				{
					// get the current operation
					const Operation *oper = ic_cycles[(cycle /* +1 */) % cycles].run[oi];
	        		const Operator *op;									//!< an operator
					// the operation has a bound operator: this is an operator instance
					const OperatorInstance *inst=oper->inst();//!< an operator instance
					// the instance is the instance of an operator: get the operator
					op = inst ? inst->oper() : 0;
					if (op==multifunction_operator) function_name=oper->function()->symbolic_function();
				}
			}
			long ctrl_width = ceil(log((double)(multifunction_operator->getCodes()->size()) /log(2.0)) );
			if (function_name!="")
			{
				s += long_to_string_bin(multifunction_operator->getCtrlCode(function_name),ctrl_width);
			}
			else
				s +=long_to_string_bin(multifunction_operator->getCtrlCode(0),ctrl_width);//default
		}
		//cmd of nofsm operator
		for (operator_it = 0; operator_it <noFSM_operators.size(); operator_it++)
		{
			const Operator *noFSM_operator=noFSM_operators[operator_it];
			const Operation *no_FSM_oper = NULL;
			//cmd at start cycle ?
			for (oi = 0; oi < ic_cycles[(cycle/* +1 */) % cycles].start.size() && (no_FSM_oper==NULL); oi++)
			{
				// get the current operation
				const Operation *oper = ic_cycles[(cycle /* +1 */) % cycles].start[oi];
	        	const Operator *op;									//!< an operator
				// the operation has a bound operator: this is an operator instance
				const OperatorInstance *inst=oper->inst();//!< an operator instance
				// the instance is the instance of an operator: get the operator
				op = inst ? inst->oper() : 0;
				if (op==noFSM_operator) no_FSM_oper=oper;
			}
			//cmd at run cycle ?
			if (no_FSM_oper==NULL)
			{
				for (oi = 0; oi < ic_cycles[(cycle /* +1 */) % cycles].run.size() && (no_FSM_oper==NULL); oi++)
				{
					// get the current operation
					const Operation *oper = ic_cycles[(cycle /* +1 */) % cycles].run[oi];
	        		const Operator *op;									//!< an operator
					// the operation has a bound operator: this is an operator instance
					const OperatorInstance *inst=oper->inst();//!< an operator instance
					// the instance is the instance of an operator: get the operator
					op = inst ? inst->oper() : 0;
					if (op==noFSM_operator) no_FSM_oper=oper;
				}
			}
			long ctrl_width = noFSM_operator->command_size();
			if (no_FSM_oper!=NULL)
			{
				long c_start = no_FSM_oper->start() % ic_cadency->length();// get start
				c_start = c_start / _clk->period();							// compute starting cycle
				s += noFSM_operator->give_code(no_FSM_oper->function(), cycle-c_start);
			}
			else
				s +=long_to_string_bin(0,ctrl_width);//default
		}
		//load of registers
		for (r_it = 0; r_it <regs_output->size(); r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
				continue;
			if (reg->is_load_at(cycle_stop,_cadency->length()))	s += "1";
			else s += "0";
		}
		// inverse
		char *tmp = (char *)s.c_str();
		for (i=0; i<strlen(tmp); i++)
		{
			if (i>=strlen(tmp)-1-i) break;
			char c = tmp[i];
			tmp[i]=tmp[strlen(tmp)-1-i];
			tmp[strlen(tmp)-1-i] = c;
		}
		string addr = "";
		/* if (one_hot_coding==false)
			addr = long_to_string_bin( (cycle+1)%cycles, long_to_nb_bit(cycles));
		else */
		if (one_hot_coding==true)
		{
			string addr_onehot = "";
			long onehotbit;
			long next_cycle=(cycle+1)%cycles;
			for (onehotbit=0;onehotbit< cycles;onehotbit++)
			{
				if (onehotbit==cycles-cycle-1)
					addr_onehot+= "1";
				else addr_onehot+= "0";
			}
			s += addr_onehot;
		}
		addr = long_to_string_bin( (cycle+1)%cycles, long_to_nb_bit(cycles));
		s += addr;
		//debug cout << "cycle : " << cycle << " s = bin : " << s << "hex : " << binary_to_hexa(s) << endl;
		sigs.push_back(binary_to_hexa(s));
	}
}


void IC::gen_output_op(ofstream &f, long cycle_output_op,long cycles)
{
	long oi;											//!< an operation index
	bool start;
	Graph<CDFGnode, CDFGedge>::EDGES::const_iterator it;//!< a Graph node iterator
	long pi;											//!< a port index
	const Data *data;
	Reg *reg;
	long fifo;


	for (oi = 0; oi < ic_cycles[cycle_output_op % cycles].end.size(); oi++)
	{
		// get the current operation
		const Operation *oper = ic_cycles[cycle_output_op % cycles].end[oi];
		// the operation has a bound operator: this is an operator instance
		const OperatorInstance *inst = oper->inst();
		// the instance is the instance of an operator: get the operator
		const Operator *op = inst ? inst->oper() : 0;
		// the operation has a function
		const Function *func = oper->function();
			
		if ((oper->function_name() == "mem_write") ||  (oper->function_name() == "mem_read")) continue;
		
		//chaining 
		if (oper->isChained()==-1) continue;
		// The operation has outputs, they are the successors of the CDFG node.
		// These outputs are stored in the declaration order of the function symbolic ports.
		for (it = oper->successors.begin(), pi = 0, start = false; it != oper->successors.end(); it++, pi++)
		{
			const CDFGedge *edge = *it;					// get edge
			CDFGnode *node = edge->target;		// get target node
			//if ((node->start() + node->length()) > cycle_start) continue;
			if (!start)
			{
				f << "        -- outputs of operation " << oper->name() << "\n";
				start = true;
			}
			switch (node->type())
			{
			case CDFGnode::DATA:
				data = (const Data *) node; // dynamic link: get the Data node
				// the destination register is the one inside which the data is stored at this clock cycle
				reg = data->reg();
				if (!reg) {
					f << " -- MISSING REGISTER\n";
					fifo = 0L;
				} else {
					fifo = reg->fifoStage();
				}
				if (fifo)
				{
					if (_sw->vhdl_type()==Switches::UT_FSM_D)
					{
						if (fifo == 1)	f << "        " << reg->name() << "(1) := " << reg->name() << "(0);\n";
						else			f << "        " << "for i in " << fifo << " downto 1" << " loop " << reg->name() << "(i) := " << reg->name() << "(i-1); end loop;\n";
					}
					else if(_sw->vhdl_type()==Switches::UT_FSM_S)//UT_ROM/UT_FSM_REGS: done inside component reg_fifo_op
					{
						if (fifo == 1)	f << "        " << reg->name() << "(1) <= " << reg->name() << "(0);\n";
						else			f << "        " << "for i in " << fifo << " downto 1" << " loop " << reg->name() << "(i) <= " << reg->name() << "(i-1); end loop;\n";
					}
				}
				if (op)
				{
					// now find pi-mapped operator output port
					const Port *port = op->getOutputPortBoundTo(pi);
					// build signal name
					string signal = port->signal_name(inst,_sw);
					if (!reg) {
						f << " -- MISSING PORT COMPONENT DUE TO MISSING REGISTERS\n";
					} else {
						map_component_port_to_register(f,"        ",signal,reg,data,port,oper);
					}
					if((op->synchronous()) && (op->synCOM()))
						trace_output_vhdl(f,"        ",inst,data,cycle_output_op,signal);
				}
				else if (oper->function_name() == "sliceread")
				{
					sliceread_register_to_register(f,"        " ,data,oper);
				}
				else if (oper->function_name() != "mem_read" && oper->function_name() != "mem_write") // EJ 14/02/2008 : dynamic memory
				{
					assign_register_to_register(f,"        " ,data,oper,cycle_output_op);
				}
				/* Fin GL*/
			break;
			case CDFGnode::CONTROL:
				// nothing to do
				break;
			case CDFGnode::OPERATION:
				// error
				cerr << "Internal error: no variable between operations " << oper->name() << " and " << node->name() << endl;
				exit(1);
			}
		}
		if ((op) && op->synchronous() && op->synCOM())
		{
			//disable synchronous operator
			f << "        -- disable of operation " << oper->name() << "\n";
			if(op->enable_polarity() == Operator::LOW)
				f << "        " << signal_name(inst, op->enable_name()) << "<= '1';" << "\n";
			else
				f << "        " << signal_name(inst, op->enable_name()) << "<= '0';" << "\n"; 
			f << "        write(outputFile_Trace_line,STRING'(\"\"));\n";
			f << "        writeline(ioFile_Trace_"<< inst->instance_name() <<",outputFile_Trace_line);\n";
		}
	}
}



void IC::gen_reg_tri_vhdl(ofstream &f, bool reg_fifo)
{                                                                                                
        f << "library IEEE;\n";
	f << "use IEEE.std_logic_1164.all;\n\n";
	f << "-- sync clear active low\n";
	f << "-- synchronism on clock transition from low to high\n";
	f << "-- sync load\n";
	f << "entity reg_op is\n";
	f << "		 generic (size_reg : integer := " << _bits << ");\n";
	f << "       port ( d : in std_logic_vector(size_reg-1 downto 0);\n";
	f << "				load : in std_logic;\n";
	f << "				clrb : in std_logic;\n";
	f << "				clk :in std_logic;\n";
	f << "				q :out std_logic_vector(size_reg-1 downto 0)\n";
	f << "		);\n";
	f << "end;\n\n";
	f << "architecture reg_op_arch of reg_op is\n";
	f << "begin\n";
	f << "	process (clrb, clk)\n";
	f << "		begin\n";
	f << "			if clk = '1' and clk'event then -- clk transition from low to high\n";
	f << "				if(clrb = '0') then -- sync clear active low\n";
	f << "					q <= (others => '0');\n";
	f << "				elsif load = '1' then		-- load and d sync with clk\n";
	f << "					q <= d;\n";
	f << "				end if;\n";
	f << "			end if;\n";
	f << "	end process;\n";
	f << "end;\n\n";
	if (reg_fifo)
	{
        f << "library IEEE;\n";
        f << "use IEEE.std_logic_1164.all;\n\n";
		f << "-- sync clear active low\n";
		f << "-- synchronism on clock transition from low to high\n";
		f << "-- sync load\n";
		f << "entity reg_fifo_op is\n";
		f << "generic (stage : integer :=1;size_reg : integer := " << _bits << ");\n";
		f << "port ( d : in std_logic_vector(size_reg-1 downto 0);\n";
	    f << "		load : in std_logic;\n";
	    f << "		clrb : in std_logic;\n";
		f << "		clk :in std_logic;\n";
		f << "		q :out std_logic_vector((size_reg*stage)-1 downto 0)\n";
		f << ");\n";
		f << "end;\n\n";

		f << "architecture reg_fifo_op_arch of reg_fifo_op is\n";
		f << "        type stages is array (0 to stage-1) of std_logic_vector(size_reg-1  downto 0);\n";
		f << "	signal tmp : stages;\n";
		f << "	begin\n";
		f << "		process (clrb, clk)\n";
		f << "		begin\n";
		f << "			if clk = '1' and clk'event then -- clk transition from low to high\n";
		f << "				if(clrb = '0') then -- sync clear active low\n";
		f << "					for i in 0 to stage-1 loop\n";
		f << "						tmp(i) <= (others => '0');\n";
		f << "					end loop;\n";
		f << "				elsif load = '1' then		-- load and d sync with clk\n";
		f << "					for i in stage-1 downto 1 loop\n";
		f << "						tmp(i) <= tmp(i-1);\n";
		f << "					end loop;\n";
		f << "					tmp(0) <= d;\n";
		f << "				end if;\n";
		f << "			end if;\n";
		f << "                       for i in stage downto 1 loop\n";
		f << "				q((size_reg*i)-1 downto (size_reg*(i-1)))<= tmp(i-1);\n";
		f << "		       end loop;\n";
		f << "		end process;\n";
		f << "end;\n";
	}
}


//! generate vhdl file
//! @param Input ofstream f.
void IC::gen_vhdl(ofstream &f)
{
	string entity_name;
	/* Caaliph 27/06/2007 */
	if (_sw->cdfgs()||_sw->n_ordo() || _sw->spact())
		entity_name = _vhdl_name;//add by Caaliph
	else /* Caaliph 27/06/2007 */
		entity_name = _sw->vhdl_prefix();			//!< an entity name
	string architecture_name = entity_name + "_arch";	//!< an architecture name
	long cycles;										//!< a number of cycles
	long cycle,onehotbit;											//!< a cycle index
	const Operation *oper;
	const OperatorInstance *inst;
	const Operator *op;
	Reg *reg;
	const Port *port;									//!< a port
	long oi;											//!< an operation index
	Graph<CDFGnode, CDFGedge>::EDGES::const_iterator it;//!< a Graph node iterator
	long cycle_start,cycle_stop;									//!< cycle start time
	long i;												//!< a counter
	bool start;									//!< a boolean
	SchedulingOut::STAGES::const_iterator s_it;			//!< a scheduling stage iterator
	const SchedulingStage *stage;						//!< a scheduling stage
	SchedulingStage::INSTANCES::const_iterator i_it;	//!< a scheduling stage instances iterator
	int r_it;
	long ctrl_width;
	long a_width,s_width;

	// compute number of cycles
	long end = _cadency->length();
	long period = _clk->period();						//!< clock period
	cycles = end / period;

	//type_state coding ?
	bool one_hot_coding = false;

	// scan CDFG and sort operations among cycles
	ic_cycles.resize(cycles+1);
	ic_clk = _clk;
	ic_cadency = _cadency;
	regs_output = _regs;
	_cdfg->scan_nodes(&sort_operations);

	/* update reg bitwidth with with max of successor/predecessor bitwidth
	DH : not useful now because reg->width is the max witdh off data allocated to register
	and it is more efficient (less flip-flop) to do sign extend during reg->port conversion
	for (r_it = 0; r_it <regs_output->size(); r_it++)
	{
		reg = regs_output->getReg(r_it);
		reg->reg_width();
	} */


	string type = "word;";								//!< word or std_logic_vector ....
	if (_sw->bitwidth_aware())
	{
		type = "std_logic_vector(";
		type += Parser::itos(_bits-1) + " downto 0);";
	}



	if (_sw->vhdl_type()==Switches::UT_ROM)
	{
		gen_rom_fsm(1,period,false);
		long cmd_enable_load_width = sigs.at(0).length()*4-1-long_to_nb_bit(cycles);
		sigs.clear();
		long size_rom=(cycles+cmd_enable_load_width)*cycles;
		/* DH disable oen hot coding  : not well explot by ISE ; how to indiciate whath the siganl is onehot coding to symplify logic decoder one_hot_coding=(size_rom<= 36864);   */
 		gen_rom_fsm(cycles,period,one_hot_coding);
		a_width = long_to_nb_bit(cycles)-1;
		s_width = sigs.at(0).length()*4-1;

	
		f << "\nlibrary ieee;\n";
		f << "use ieee.std_logic_1164.all;\n";
		f << "use ieee.numeric_std.all;\n";

		f << "entity " << entity_name << "_rom is\n";
		f << "	port (addr : in std_logic_vector(" << a_width <<" downto 0);\n";
		f << "		rom_out : out std_logic_vector(" << s_width <<" downto 0)\n";
		f << "		);\n";
		f << "	attribute rom_extract: string;\n";
		f << "	attribute rom_extract of " << entity_name << "_rom : entity is \"yes\";\n";
		f << "	attribute rom_style: string;\n"; 
		f << "	attribute rom_style of " << entity_name <<"_rom : entity is \"block\";\n";
		f << "end;\n";

		f << "architecture " << entity_name << "_rom_arch of " << entity_name <<"_rom is\n";
		f << "  type rom_type is array (0 to " << cycles-1 << ") of std_logic_vector ("<< s_width <<" downto 0) ;\n";
		f << "  constant rom : rom_type := \n";
		f << "  (\n";
		f << "    ";
		for (cycle = 0; cycle < cycles-1; cycle++)
		{
			f << "X\"" << sigs.at(cycle) << "\" ,";
			if ((cycle & 0xf) == 0x4) f << "\n" << "    ";
		}
		f << "X\"" << sigs.at(cycles-1) << "\" \n";
		f << "  );\n";
		f << "begin\n";
		f << "	rom_out <= rom(to_integer(unsigned(addr)));\n";
		f << "end;\n";	
	}


    bool reg_fifo=false;
    if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
    {
		//generate entity reg_op, reg_fifo_op 
        for (r_it = 0; r_it <regs_output->size() && reg_fifo==false; r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
				continue;
			if (reg->fifoStage()) reg_fifo=true;
		}
		 gen_reg_tri_vhdl(f,reg_fifo);
    }

	

	
	// libraries
	f << "library ieee;\n";
	f << "use ieee.std_logic_1164.all;\n";
	f << "use ieee.numeric_std.all;\n";
	f << "use std.textio.all;\n";
	if (_sw->bitwidth_aware() == true)
	{
		/* GL 10/10/07 : add line */
		if (_cdfg->fixed_detected()) f  << "use work.fixed_pkg.all;\n";
		/* Fin GL */
		f  << "use work.bitwidth_notech.all;\n";
	}
	else
		f  << "use work.notech.all;\n";

	if (_debug_vhdl)
		f  << "use std.textio.all;"	<< "\n\n";
	else f  << "\n";

	// entity start
	f << "entity " << entity_name << " is\n";
	// ports
	f << "  port(\n";
	/* GL 05/12/08 : Automization of gals & lis generation */
	// is done by putting all synchronous operators signals as input/output of the design */
	f << "-- I/O to connect to operator blocs" << "\n";
	for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
	{
		stage = &(*s_it);		// get current scheduling stage
		for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
		{
			inst = *i_it;		// get current instance
			op = inst->oper();		// get operator model
			if (op->synCOM()) continue;
			for (i = 0; i < op->inputPorts(); i++)
			{
				port = op->getInputPort(i);
				port->entity_port_vhdl_declatation(f,"    ",_sw,inst,"in");
			}
			for (i = 0; i < op->outputPorts(); i++)
			{
				port = op->getOutputPort(i);
				port->entity_port_vhdl_declatation(f,"    ",_sw,inst,"out");
			}
		}
	}

	/* Fin GL */
	for (i = 0; i < _buses->buses(); i++)
	{
		if (_buses->OnlyBusInputAccess(i))
			f << "    " << bus_name(i) << " : in " << type << "\n";
		else if (_buses->OnlyBusOutputAccess(i))
			f << "    " << bus_name(i) << " : out " << type << "\n";
		else
			f << "    " << bus_name(i) << " : inout " << type << "\n";
	}

	// EJ 14/02/2008 : dynamic memory
	for (i = 0; i < _abuses->buses(); i++)
	{
		f << "    " << address_bus_name(i) << " : out " << type << "\n";
	}
	// End EJ

	f << "    enable, rstb, clk : in std_logic\n";
	f << " );\n";
	// entity stop
	f << "end;\n";

	// architecture start
	f << "architecture " << architecture_name << " of " << entity_name << " is\n";

    f << "  attribute period:string;\n";
    f << "  attribute period of clk : signal is \""<< period << " ns\";\n";

    if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
    {
		// component register
	     f << "  component reg_op\n";
	     f << "  generic (size_reg : integer := " << _bits << ");\n";
	     f << "  port (\n";
	     f << "  \td : in std_logic_vector(size_reg-1 downto 0) ;\n";
	     f << "  \tload : in std_logic ;\n";
	     f << "  \tclrb : in std_logic ;\n";
	     f << "  \tclk : in std_logic ;\n";
	     f << "  \tq : out std_logic_vector(size_reg-1 downto 0) ) ;\n";
	     f << "  end component ;\n\n";
	     if (reg_fifo)
	     {
			// component register
		    f << "  component reg_fifo_op\n";
			f << "  generic (size_reg : integer := " << _bits << ";stage : integer := 1);\n";
			f << "  port (\n";
			f << "  \td : in std_logic_vector(size_reg-1 downto 0) ;\n";
			f << "  \tload : in std_logic ;\n";
			f << "  \tclrb : in std_logic ;\n";
			f << "  \tclk : in std_logic ;\n";
			f << "  \tq :out std_logic_vector((size_reg*stage)-1 downto 0) );\n";
			f << "  end component ;\n\n";
         }
    }


    if (_sw->vhdl_type()==Switches::UT_ROM)
	{
		f << "  component " << entity_name << "_rom\n";
		f << "	port (addr : in std_logic_vector(" << a_width <<" downto 0);\n";
		f << "		rom_out : out std_logic_vector(" << s_width <<" downto 0)\n";
		f << "		);\n";
		f << "  end component ;\n\n";
	}

	//DH: 28/11/2008 : add vhdl trace for debug
	if (_debug_vhdl)
	{
		f << "\tfile outputFile_Trace : text is out \"Simu_" << entity_name<< "//SimuTrace.txt\";\n";
		f << "\tshared variable outputFile_Trace_line : line;\n";
	}

	 // signals list
	//state of fsm
	 if (_sw->vhdl_type()==Switches::UT_FSM_D)
		f << "  -- states of the FSMD\n";
	 else /* if ((_sw->vhdl_type()==Switches::UT_FSM_R) || (_sw->vhdl_type()==Switches::UT_FSM_S)) */
		 f << "  -- states of the FSM\n";
	//brom limit 36kbit in virtex 5 => 192*192, so limit one hot coding in fsm  
	//one hot coding ?
	if ((_sw->vhdl_type()==Switches::UT_FSM_R) || (_sw->vhdl_type()==Switches::UT_FSM_S))
	{
		//optimize speed, reduce power dissipation => only one flip_flop is active at any one time
		//easier decode state
		/* DH disable one hot coding  : not well exploited by ISE ; how to indiciate that the siganl is onehot coding to symplify logic :  one_hot_coding=true /* (cycles <= 192);   */
		one_hot_coding=false;
	}
	if (_sw->vhdl_type()!=Switches::UT_ROM)
	{
		f << "  type state_type is (";
		for (cycle = 0; cycle < cycles; cycle++)
		{
			f << "S" << cycle;
			if (cycle < (cycles-1)) f << ",";
			if ((cycle & 0xf) == 0x8) f << "\n" << "                      ";
		}
		f << ");\n";
		if (one_hot_coding==true)
		{
			f << "  --one hot coding for state_type for ise/quartus\n";
			f << "  --attribute enum_encoding : string;\n";
			f << "  --attribute enum_encoding of state_type : type is \"one_hot\";\n";
			f << "  --one hot coding for state_type for synplify\n";
			f << "  --attribute syn_encoding : string;\n";
			f << "  --attribute syn_encoding of state_type : type is \"onehot\";\n";
		}
		else
		{
			f << "  --sequential coding for state_type\n";
			f << "  --attribute enum_encoding : string;\n";
			f << "  --attribute enum_encoding of state_type : type is \"sequential\";\n";
			f << "  --sequentail coding for state_type for synplify\n";
			f << "  --attribute syn_encoding : string;\n";
			f << "  --attribute syn_encoding of state_type : type is \"sequential\";\n";
		}
		f << "  signal next_state : state_type;\n";
	}
	else
	{
		if (one_hot_coding==true)
		{
			f << "  --one hot coding for state_type\n";
			f << "  subtype state_type is std_logic_vector(" << cycles-1 << " downto 0);\n";
			for (cycle = 0; cycle < cycles; cycle++)
			{
				f << "  constant S" << cycle << " : state_type := \"";
				for (onehotbit=0;onehotbit< cycles;onehotbit++)
				{
					if (onehotbit==cycles-cycle-1)
						f << "1";
					else f << "0";
				}
				f << "\";\n";
			}
			f << "  signal	next_addr_rom : std_logic_vector(" << a_width << " downto 0);\n";
		}
		else
		{
			f << "  --sequentail coding for state_type\n";
			f << "  subtype state_type is std_logic_vector(" <<a_width << " downto 0);\n";
			for (cycle = 0; cycle < cycles; cycle++)
			{
				f << "  constant S" << cycle << " : state_type := \"";
				f << long_to_string_bin(cycle,a_width+1);
				f << "\";\n";
			}
			f << "  signal	next_state : state_type;\n";
		}
		f << "  signal	rom_out : std_logic_vector(" << s_width << " downto 0);\n";
		f << "  signal	addr_rom : std_logic_vector(" << a_width << " downto 0);\n";
	}
	f << "  signal state : state_type;\n";

	// signals connected to component ports
	f << "  -- signals to connect registers to operators\n";
	for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
	{
		stage = &(*s_it);		// get current scheduling stage
		for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
		{
			inst = *i_it;		// get current instance
			op = inst->oper();		// get operator model
		
			// EJ 14/02/2008 : dynamic memory
			if (op->component() == "mem_read_op" || op->component() == "mem_write_op") continue;
		
			/* GL 05/12/08 : Automization of gals & lis generation */				
			if(!op->synCOM()) continue;
			/* Fin GL */
		
			for (i = 0; i < op->inputPorts(); i++)
			{
				port = op->getInputPort(i);
				port->signal_port_vhdl_declatation(f,"  ",_sw,inst);
			}
		
			/* GL 20/11/07 : multifunction operators */
			if (op->isMultiFunction())
			{
				ctrl_width = ceil(log((double)(op->getCodes()->size()) /log(2.0)) ) - 1;/*GL 26/11/07: correction formule*/
				if (ctrl_width)
					f << "  signal " << signal_name(inst, op->ctrl_name())  << " : std_logic_vector(" << ctrl_width << " downto 0);\n";
				else
					f << "  signal " << signal_name(inst, op->ctrl_name())  << " : std_logic;\n";
			}
			/* Fin GL */
			/* GL 24/04/08 : synchronous operators */
			if (op->synchronous())
			{
				f << "  signal " << signal_name(inst, op->clk_name())  << " : std_logic;\n";
				f << "  signal " << signal_name(inst, op->rst_name())  << " : std_logic;\n";
				f << "  signal " << signal_name(inst, /* GL 17/07/08 */op->enable_name()) /**/ << " : std_logic;\n"; 
			}
			/* GL 22/10/08  */
			if (op->noFSM())
				f << "  signal " << signal_name(inst, op->command_name())  << " : std_logic_vector(" << op->command_size()-1 << " downto 0);\n";
			/* Fin GL */
			for (i = 0; i < op->outputPorts(); i++)
			{
				port = op->getOutputPort(i);
				port->signal_port_vhdl_declatation(f,"  ",_sw,inst);
			}
		}
	}
	bool needoutputFile_Trace_line=false;
	//trace de simulation for synchronous operator
	for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
	{
		stage = &(*s_it);		// get current scheduling stage
		for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
		{
			inst = *i_it;		// get current instance
			op = inst->oper();		// get operator model
			if (op->synchronous())
			{
				f << "  file ioFile_Trace_"<< inst->instance_name()<<" : text is out \"Simu_" << entity_name<< "//" << inst->instance_name() << "_Trace.txt\";\n";
				needoutputFile_Trace_line=true;
			}
		}
	}
	if ((!_debug_vhdl) && (needoutputFile_Trace_line==true))
		f << "  shared variable outputFile_Trace_line : line;\n";
	if (_sw->vhdl_type()==Switches::UT_FSM_S)
	{
		f << "  -- data path registers" << "\n";
		// list of registers
		for (r_it = 0; r_it <regs_output->size(); r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
				continue;
			reg->_addr = 0;
			reg->signal_vhdl_declaration(f,"  ",_sw);
		}
	}
    else if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
	{
		// list of registers
		f << "  -- signals to connect data path registers" << "\n";
		for (r_it = 0; r_it <regs_output->size(); r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
				continue;
			reg->_addr = 0;
			reg->component_sigs_vhdl_declaration(f,"  ",_sw,_bits);
		}
    }
	hardwire_constant_declaration(f, "  ",_sw->bitwidth_aware(), _bits);	//hardwire constant declaration
	f << "\n";
	f  << "begin" << "\n";
	if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
	{
		//instantiate component reg_op or reg_fifo_op
		for (r_it = 0; r_it <regs_output->size(); r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
				continue;
			reg->instantiate_component_vhdl(f,"  ",_sw,_bits);
		}
	}

	if (_sw->vhdl_type()==Switches::UT_ROM) 
	{
		//instantiate ROM component
		f  << "  rom_fsm : " << entity_name << "_rom\n";
		f  << "  port map(addr=>addr_rom, rom_out=>rom_out);\n";
	}


	/* GL 24/04/08 : synchronous operators */
	// clock and reset signals generation
	bool commnentClockResetGeneration=false;
	for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
	{
		stage = &(*s_it);		// get current scheduling stage
		for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
		{
			inst = *i_it;		// get current instance
			op = inst->oper();		// get operator model
			/* GL 05/12/08 : Automization of gals & lis generation */
			if(!op->synCOM()) continue;
			/* Fin GL */
			if (!op->asynchronous())
			{
				if (!commnentClockResetGeneration)
				{
					f << "  -- Clock and Reset signals to connect to synchronous operators\n";
					commnentClockResetGeneration=true;
				}
				// synchronous design
				f << "  " << signal_name(inst, op->clk_name())  << " <= clk;\n";
				/* GL 17/07/08 */
				if (op->rst_polarity()== Operator::HIGH)
					f << "  " << signal_name(inst, op->rst_name())  << " <= not rstb;\n";
				else
					/* Fin GL */
					f << "  " << signal_name(inst, op->rst_name())  << " <= rstb;\n";
			}
		}
	}
	/* Fin GL */
	// instances of operator
	for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
	{
		stage = &(*s_it);		// get current scheduling stage
		for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
		{
			inst = *i_it;		// get current instance
			op = inst->oper();		// get operator model
		
			// EJ 14/02/2008 : dynamic memory
			if (op->component() == "mem_read_op" || op->component() == "mem_write_op") continue;
			/* GL 05/12/08 : Automization of gals & lis generation */
			if(!op->synCOM()) continue;
			/* Fin GL */
		
			f << "  " << "-- instance of operator " << op->name() << " to implement function " << inst->function()->name() << "\n";
			f << "  " << inst->instance_name(_sw) << " : " << inst->oper()->component()	<< "\n";
			f << "    port map(\n";
			f << "      -- input ports\n";
			for (i = 0; i < op->inputPorts(); i++)
			{
				port = op->getInputPort(i);
				f << "      " << port->name() << " => " << port ->signal_name(inst,_sw) << ",\n";
			}
			/* GL 20/11/07 : multifunction operators */
			if (op->isMultiFunction())
			{
				f << "      -- ctrl ports" << "\n";
				f << "      " << op->ctrl_name() << " => " << signal_name(inst, op->ctrl_name()) << ",\n";
			}
			/* Fin GL */
			/* GL 24/04/08 : synchronous operators */
			if (op->synchronous())
			{
				f << "      -- enable, clock and reset ports" << endl;
				f << "      " << op->enable_name() << " => " << signal_name(inst, op->enable_name()) << ",\n"	
				<< "      " << op->clk_name() << " => " << signal_name(inst, op->clk_name()) << ",\n"
				<< "      " << op->rst_name() << " => " << signal_name(inst, op->rst_name()) << ",\n";
			}
			/* Fin GL */
			/* GL 22/10/08  */
			if (op->noFSM())
				f << "      " << op->command_name() << " => "  << signal_name(inst, op->command_name())  << ",\n";
			/* Fin GL */
			f << "      -- output ports" << "\n";
			for (i = 0; i < op->outputPorts(); i++)
			{
				port = op->getOutputPort(i);
				f << "      " << port->name() << " => " << port->signal_name(inst,_sw);
				if (i < (op->outputPorts()-1)) f << ",";
				f << "\n";
			}
			f << "    );\n";
		}
	}
	f << "\n";
	
	f << "  -- The synchronous process\n"
		<< "  SYNC_PROC: process (clk)\n"
		<< "  begin\n"
		<< "    if (clk'event and clk = '1') then\n"
		<< "      if (rstb = '0') then\n";
	if (_sw->vhdl_type()!=Switches::UT_FSM_D)
	{
		if ((_sw->vhdl_type()==Switches::UT_ROM) && (one_hot_coding==true))
		f << "        addr_rom <=  \"" << long_to_string_bin(cycles-1,a_width+1) <<"\";\n";
		else
		f << "        state <= S" << cycles-1 <<";\n";
	}
	else
		f << "        state <= S0;\n";
		f << "      elsif (enable = '1') then\n";
		if ((_sw->vhdl_type()==Switches::UT_ROM) && (one_hot_coding==true))
			f << "        addr_rom <= next_addr_rom;\n";
		else
			f << "        state <= next_state;\n";
		f << "      end if;\n"
		<< "    end if;\n"
		<< "  end process;\n"
		<< "\n";
	
	if (_sw->vhdl_type()==Switches::UT_FSM_S)
		f << "  MUX_REGISTER: process (clk)\n";
	else if (_sw->vhdl_type()==Switches::UT_FSM_R)
	{
		f << "  LOAD_REGISTER: process (clk)\n";
	}
	else if (_sw->vhdl_type()==Switches::UT_ROM)
	{
		if (a_width >=0)
		{
			if (one_hot_coding==false)
			{
				f << "  addr_rom <= state("<< a_width <<" downto 0);\n";
				f << "  next_state <= rom_out("<< a_width <<" downto 0);\n";
				// list of registers
				long index_reg=0;
				for (r_it = 0; r_it <regs_output->size(); r_it++)
				{
					reg = regs_output->getReg(r_it);
					if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
						continue;
					index_reg++;
					f << "  " << reg->name() << "_load <= rom_out("<< a_width+index_reg <<") ;\n"; ;
				}
			}
			else 
			{
				f << "  next_addr_rom <= rom_out("<< a_width <<" downto 0);\n";
				f << "  state <= rom_out("<< cycles+a_width <<" downto " << a_width+1 << ");\n";

				// list of registers
				long index_reg=0;
				for (r_it = 0; r_it <regs_output->size(); r_it++)
				{
					reg = regs_output->getReg(r_it);
					if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
						continue;
					index_reg++;
					f << "  " << reg->name() << "_load <= rom_out("<< cycles+a_width+index_reg <<") ;\n"; ;
				}
			}
		}
		else
		{
			// list of registers
			long index_reg=0;
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				index_reg++;
				f << "  " << reg->name() << "_load <= rom("<< index_reg <<") ;\n"; ;
			}
		}
		f << "\n";
	}
	else
	{
		f << "  OUTPUT_FSM_D: process (clk)\n";
	
		f << "  -- data path variables\n";
		// list of registers
		for (r_it = 0; r_it <regs_output->size(); r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
				continue;
			reg->_addr = 0;
			reg->variable_vhdl_declaration(f,"  ",_sw);
		}

	}
	if (_sw->vhdl_type()!=Switches::UT_ROM)
	{
		f << "  begin\n";
		f << "  if (clk'event and clk = '1') then\n";
		f << "    if (rstb = '0') then\n";
		if (_sw->vhdl_type()==Switches::UT_FSM_S)
		{
			f << "    -- reset value on registers\n";
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				else if (reg->fifoStage())
				{
					f << "    " << reg->name() << "(0) <= (others=>'0');\n";
				}
				else
				{
					f << "    " << reg->name() << " <= (others=>'0');\n";
				}
			}
		}
		else if (_sw->vhdl_type()==Switches::UT_FSM_R)
		{
			f << "    -- defaults value on input load registers\n";
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				if (reg->is_load_at(0,_cadency->length()))
					f << "    " << reg->name() << "_load <= '1';\n";
				else 
					f << "    " << reg->name() << "_load <= '0';\n";
			}
		}
		else if (_sw->vhdl_type()==Switches::UT_FSM_D)
		{
			f << "    -- reset value on variable\n";
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				else if (reg->fifoStage())
				{
					f << "    " << reg->name() << "(0) := (others=>'0');\n";
				}
				else
				{
					f << "    " << reg->name() << " := (others=>'0');\n";
				}
			}
			f << "    -- high impedance on buses to avoid bus pollution\n";
			for (i = 0; i < _buses->buses(); i++)
			{
				if (!_buses->OnlyBusInputAccess(i))
				f << "    " << bus_name(i) << " <= (others => 'Z');\n";
			}
			for (i = 0; i < _abuses->buses(); i++)
			{
				f << "    " << address_bus_name(i) << " <= (others => 'Z');\n";
			}
		}
		f << "    elsif (enable = '1') then\n";
		long cycle_dynamic_index, cycle_dynamic_write, cycle_dynamic_read;
		// End EJ
		if (_sw->vhdl_type()==Switches::UT_FSM_D)
		{
			f << "    -- high impedance on buses to avoid bus pollution\n";
			for (i = 0; i < _buses->buses(); i++)
			{
				if (!_buses->OnlyBusInputAccess(i))
				f << "    " << bus_name(i) << " <= (others => 'Z');\n";
			}
			for (i = 0; i < _abuses->buses(); i++)
			{
				f << "    " << address_bus_name(i) << " <= (others => 'Z');\n";
			}
		}
		/*	if (_sw->vhdl_type()==Switches::UT_FSM_R)
		{
			f << "    -- defaults value on input load registers\n";
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				f << "    " << reg->name() << "_load <= '0';\n";
			}
		} not nessary now */
		f  << "    case (state) is\n";
		for (cycle = 0; cycle < cycles; cycle++)
		{
			cycle_start = (cycle * period) % _cadency->length();
			cycle_stop = ((cycle+1) * period) % _cadency->length();
			long cycle_load = ((cycle+2) * period) % _cadency->length();

			f << "      when S" << cycle << " => -- time " << cycle_start			<< "\n";
		
			// ENABLE INPUTS OF OPERATIONS
			for (oi = 0; oi < ic_cycles[(cycle/* +1 */) % cycles].start.size(); oi++)
			{
				// get the current operation
				oper = ic_cycles[(cycle/* +1 */) % cycles].start[oi];
				gen_enable_operator(f,"        " ,oper,cycle);
				trace_input_reg_operator(f,"        " ,oper,cycle);
			}
			// CMD INPUTS OF OPERATIONS
			for (oi = 0; oi < ic_cycles[(cycle /* +1 */) % cycles].start.size(); oi++)
			{
				// get the current operation
				oper = ic_cycles[(cycle /* +1 */) % cycles].start[oi];
				gen_cmd_operator(f,"        " ,oper,cycle);
			}
			for (oi = 0; oi < ic_cycles[(cycle /* +1 */) % cycles].run.size(); oi++)
			{
				// get the current operation
				oper = ic_cycles[(cycle /* +1 */) % cycles].run[oi];
				gen_cmd_operator(f,"        " ,oper,cycle);
			}
			if (_sw->vhdl_type()==Switches::UT_FSM_D)
			{
				gen_output_op(f,cycle,cycles);
				gen_output_on_buses(f,cycle_stop); 
				gen_static_write_to_memory(f,cycle_stop);
				gen_input_from_buses(f,cycle_start);
				gen_static_memory_read(f,cycle_start);
				gen_dynamic_address(f,cycle_start);
				gen_dynamic_memory_write(f,cycle_start);
				gen_dynamic_memory_read(f,cycle_start);
				// INPUTS OF OPERATIONS
				for (oi = 0; oi < ic_cycles[cycle].start.size(); oi++)
				{
					oper=ic_cycles[cycle].start[oi];
					gen_input_reg_operator(f,"        ",oper,cycle);
				}
			}
			else if (_sw->vhdl_type()==Switches::UT_FSM_S)
			{
				gen_output_op(f,cycle+1,cycles);
				gen_input_from_buses(f,cycle_stop);
				gen_static_memory_read(f,cycle_stop);
				gen_dynamic_memory_read(f,cycle_stop);
			}
			else if (_sw->vhdl_type()==Switches::UT_FSM_R)
			{

				//enable of synchronous operators
				//cmd of multi_function operator
				//cmd of nofsm operator all done before if wrong see UT_ROM generation
				//load of registers
				for (r_it = 0; r_it <regs_output->size(); r_it++)
				{
					reg = regs_output->getReg(r_it);
					if	((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
							continue;
					if (reg->is_load_at(cycle_load,_cadency->length()))	
						f << "        " << reg->name() << "_load <= '1';\n";
					else 
						f << "        " << reg->name() << "_load <= '0';\n";
				}
			}
		}
		f << "    end case; end if;\n";
		f << "    end if;\n";
		f << "  end process;\n\n";
	}	
		
		
	if (_sw->vhdl_type()!=Switches::UT_FSM_D)
	{
		long cpt_sensitivity_list=1;
		//generate sensitivity list
		if (_sw->vhdl_type()==Switches::UT_ROM)
			f << "  MUX_REGISTER_MUX_OPERATOR_TRI_BUS: process (state";
		else if (_sw->vhdl_type()==Switches::UT_FSM_R)
			f << "  MUX_REGISTER_MUX_OPERATOR_TRI_BUS_NEXT_STATE: process (state";
		else
			f << "  MUX_OPERATOR_TRI_BUS_NEXT_STATE: process (state";
		for (r_it = 0; r_it <regs_output->size(); r_it++)
		{
			reg = regs_output->getReg(r_it);
			if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
			continue;
			f << ",";
			if ((cpt_sensitivity_list & 0xf) == 0x4) f << "\n" << "                      ";
			if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
				f << reg->name() <<"_out";
			else
				f << reg->name();
			cpt_sensitivity_list++;

		}
		if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
		{
			//internal or input BUS 
			for (i = 0; i < _buses->buses(); i++)
			{
				if (!_buses->OnlyBusOutputAccess(i))
				{
					f << ",";
					if ((cpt_sensitivity_list & 0xf) == 0x4) f << "\n" << "                      ";
					cpt_sensitivity_list++;
					f << bus_name(i);
				}
			}
			//operator output
			for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
			{
				stage = &(*s_it);		// get current scheduling stage
				for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
				{
					inst = *i_it;		// get current instance
					op = inst->oper();		// get operator model
		
					// EJ 14/02/2008 : dynamic memory
					if (op->component() == "mem_read_op" || op->component() == "mem_write_op") continue;
					for (i = 0; i < op->outputPorts(); i++)
					{
						port = op->getOutputPort(i);
						f << ",";
						if ((cpt_sensitivity_list & 0xf) == 0x4) f << "\n" << "                      ";
						cpt_sensitivity_list++;
						f << port->signal_name(inst,_sw);
					}
				}	
			}
		}
		f << ")\n";

		f << "  begin\n";
		f << "    -- high impedance on buses to avoid bus pollution\n";
		for (i = 0; i < _buses->buses(); i++)
		{
			if (!_buses->OnlyBusInputAccess(i))
				f << "    " << bus_name(i) << " <= (others => 'Z');\n";
		}
		//long cycle_dynamic_index, cycle_dynamic_write, cycle_dynamic_read;
		for (i = 0; i < _abuses->buses(); i++)
		{
			f << "    " << address_bus_name(i) << " <= (others => 'Z');\n";
		}

		// signals connected to input component ports
		f << "    -- signals to connect registers to operators\n";
		for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
		{
			stage = &(*s_it);		// get current scheduling stage
			for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
			{
				inst = *i_it;		// get current instance
				op = inst->oper();		// get operator model
			
			
				// EJ 14/02/2008 : dynamic memory
				if (op->component() == "mem_read_op" || op->component() == "mem_write_op") continue;
			
				/* GL 05/12/08 : Automization of gals & lis generation */				
				if(!op->synCOM()) continue;
				/* Fin GL */
			
				for (i = 0; i < op->inputPorts(); i++)
				{
					port = op->getInputPort(i);
					/* DH: 25/09/2009  change '-' into 'X'see altera hdl style guide */
					f << "    " << port ->signal_name(inst,_sw) << " <= (others => 'X');\n";
				}
				
			}
		}
		// signals connected to input registers 
		if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
		{
			f << "    -- defaults value on input registers\n";
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				/* DH: 25/09/2009  change '-' into 'X'see altera hdl style guide */
				f << "    " << reg->name() << "_in <= (others=> 'X');\n";
			}
		}

		
		f  << "    case (state) is\n";
		for (cycle = 0; cycle < cycles; cycle++)
		{
			cycle_start = (cycle * period) % _cadency->length();
			cycle_stop = ((cycle+1) * period) % _cadency->length();
			
			f << "      when S" << cycle << " => -- time " << cycle_start			<< "\n";
			if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
			{
				gen_output_op(f,cycle+1 /* cycle_stop*/,cycles);//generate data_in register
				gen_input_from_buses(f,cycle_stop);//generate data_in register
				gen_static_memory_read(f,cycle_stop);//generate data_in register
				gen_dynamic_memory_read(f,cycle_stop);//generate data_in register
			}
			// OUTPUTS on buses
			/* operation output on bus */
			 gen_output_on_buses(f,cycle_stop);
              /* static write to memory */
			gen_static_write_to_memory(f,cycle_stop);
			/* Fin GL */
			gen_dynamic_address(f,cycle_start);
			gen_dynamic_memory_write(f,cycle_start);
			// INPUTS OF OPERATIONS
			for (oi = 0; oi < ic_cycles[cycle].start.size(); oi++)
			{
				oper=ic_cycles[cycle].start[oi];
				gen_input_reg_operator(f,"        ",oper,cycle);
			}
			for (oi = 0; oi < ic_cycles[cycle].run.size(); oi++)
			{
				oper=ic_cycles[cycle].run[oi];
				gen_input_reg_operator(f,"        ",oper,cycle);
			}			
			if (_sw->vhdl_type()!=Switches::UT_ROM)
			{
				f << "        next_state <= S";
				if (cycle == cycles-1)		f << "0";
				else						f << (cycle+1);
				f << ";\n";
			}
		}
		if (_sw->vhdl_type()==Switches::UT_ROM)
		{
			//signal state coding is not an enumerate type so test all case
			f << "      when OTHERS => -- unreachable state \n";
			// signals connected to input component ports
			f << "        -- default values on input operators\n";
			for (s_it = _sched->stages.begin(); s_it != _sched->stages.end(); s_it++)
			{
				stage = &(*s_it);		// get current scheduling stage
				for (i_it = stage->instances.begin(); i_it != stage->instances.end(); i_it++)
				{
					inst = *i_it;		// get current instance
					op = inst->oper();		// get operator model
			
			
					// EJ 14/02/2008 : dynamic memory
					if (op->component() == "mem_read_op" || op->component() == "mem_write_op") continue;
			
					/* GL 05/12/08 : Automization of gals & lis generation */				
					if(!op->synCOM()) continue;
					/* Fin GL */
			
					for (i = 0; i < op->inputPorts(); i++)
					{
						port = op->getInputPort(i);
						/* DH: 25/09/2009  change '-' into 'X'see altera hdl style guide */
						f << "        " << port ->signal_name(inst,_sw) << " <= (others => 'X');\n";
					}
				
				}
			}
			// signals connected to input registers 
			f << "        -- defaults value on input registers\n";
			for (r_it = 0; r_it <regs_output->size(); r_it++)
			{
				reg = regs_output->getReg(r_it);
				if ((reg->data(0)->type()==Data::CONSTANT) && (reg->data(0)->hardwire() == true))
					continue;
				/* DH: 25/09/2009  change '-' into 'X'see altera hdl style guide */
				f << "        " << reg->name() << "_in <= (others=> 'X');\n";
			}
		}
		f << "    end case;\n"
		<< "  end process;\n\n";
	}
	
		
	if (_sw->vhdl_type()==Switches::UT_FSM_D)
	{
		f << "  NEXT_STATE_DECODE: process (state)\n"
		<< "  begin\n"
		<< "    next_state <= state;  -- default is to stay in current state\n"
		<< "    case (state) is\n";
		for (cycle = 0; cycle < cycles; cycle++)
		{
			f << "      when S" << cycle << " => -- time " << cycle * period		<< "\n";
			f << "        next_state <= S";
			if (cycle == cycles-1)		f << "0";
			else						f << (cycle+1);
			f << ";\n";
		}
		f << "    end case;\n"
			<< "  end   process;\n" << "\n"; 
	}
	// end
	f  << "end " << architecture_name << ";\n" << endl;
	// free memory
	ic_cycles.clear();
	hardwire_constant_list.clear();
	sigs.clear();
	multi_function_operators.clear();
	synchronous_operators.clear();
	noFSM_operators.clear();
}
// end of: ic.cpp