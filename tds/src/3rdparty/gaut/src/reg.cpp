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

//	File:		reg.cpp
//	Purpose:	registers
//	Author:		Pierre Bomel, LESTER, UBS

#include "scheduling.h"
#include "reg.h"
#include <limits>


long Reg::_count = 0;

bool Reg::is_chained(const Data *data)
{

	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
	const CDFGedge *e;		// an edge
	const CDFGnode *n_dst,*n_src;	// a destination node
	const Operation *oper;

	bool result=false;
	//debug data->info();
	for (e_it = data->predecessors.begin(); e_it != data->predecessors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_src = e->source ;	// get the target node
		if (n_src->type() != CDFGnode::OPERATION) continue;
		oper=(const Operation*)n_src;
		if (oper->isChained()==-1)
		{
			result=true;
			break;
		}
	}
	return result;
}


//! Allocate data to register.
static void data_allocation(Switches::Vhdl_type vhdl_type, Data *data,Reg *reg,long cadency,long period,bool bitwidth_aware,long _bits,const CDFG *cdfg)
{
	//debug cout << "Allocate Data " << data->name() << " on register " << reg->name() << endl; 
	if (data->regno()==-1)
	{
		const CDFGedge *e;		// an edge
		const CDFGnode *n_dst,*n_source;	// a destination node
		const Operation *oper;			// an operation
		Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
		long pos, stage;
		
		if (bitwidth_aware==true)
		{
			if (data->bitwidth()>reg->bitwidth())
				reg->bitwidth(data->bitwidth());
		}
		else 
				reg->bitwidth(_bits);
		reg->_datas.push_back(data);				// KT 15/12/2007
		if (data->type() == Data::CONSTANT)
			reg->contain_const= true;
		else
			reg->contain_const= false;						// KT 15/12/2007
		reg->checked= false;
		reg->pos = 0;								// KT 16/04/2008
		data->regno(reg->no());						// KT register which contain data
		data->setReg(reg); // DH 26/10/08
		if ((data->type()==Data::OUTPUT) ||
			((data->type()==Data::VARIABLE) ))
		{
			// scan all edges to predecessors
			for (e_it = data->predecessors.begin(); e_it != data->predecessors.end(); e_it++)
			{
				e = *e_it;			// get the edge
				n_source = e->source ;	// get the target node
				if (n_source->type() == CDFGnode::OPERATION) {
					oper=(const Operation *) n_source;
					pos=oper->getOutputPos(data);
					/* if (oper->function_name() == "mem_read")
					reg->addUse(data, oper,pos,true,cdfg); // add use of register
					else n'a pas d'inportance : il faut générer le load que l'entree du registre vienne du bus ou de la sortie d'un pseudo operator mem_read */
					reg->addUse(data, oper,pos,false,cdfg); // add use of register
				} else if (n_source->type()  == CDFGnode::DATA) {
					const CDFGedge* e2 = *n_source->predecessors.begin();
					const CDFGnode* s2 = e2->source;
					if (s2->type() != CDFGnode::OPERATION) 
						continue;
					oper = (const Operation*) s2;
					pos = oper->getOutputPos(data);
					if (pos < 0) {
						pos = oper->getOutputPos((const Data*)n_source);
						reg->addUse((const Data*)n_source, oper,pos,false,cdfg);
					} else {
						reg->addUse(data, oper,pos,false,cdfg);
					}
				}
			}
		}
		if (data->type()==Data::VARIABLE || data->type()==Data::INPUT || data->type()==Data::CONSTANT)
		{
			// scan all edges to successors
			for (e_it = data->successors.begin(); e_it != data->successors.end(); e_it++)
			{
				e = *e_it;			// get the edge
				n_dst = e->target ;	// get the target node
				if (n_dst->type() != CDFGnode::OPERATION) continue;
				oper=(const Operation *) n_dst;
				pos=oper->getInputPos(data);
				reg->addUse(data, oper, pos,true,cdfg); // add use of register
			}
		}

	}
}


static	long get_max_stage(Switches::Vhdl_type vhdl_type,Data *data,long period,long cadency) 
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_dst,*n_source;	// a destination node
	const Operation *oper;			// an operation
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
	long stage;
	long max_stage=0; 


	if (data->type()==Data::VARIABLE || data->type()==Data::INPUT || data->type()==Data::CONSTANT)
	{
		// scan all edges to successors
		for (e_it = data->successors.begin(); e_it != data->successors.end(); e_it++)
		{
			e = *e_it;			// get the edge
			n_dst = e->target ;	// get the target node
			if (n_dst->type() != CDFGnode::OPERATION) continue;
			oper=(const Operation *) n_dst;
			// calcul de la profondeur du registre (registre ou fifo ?)
			if (oper->isChained()==-1)
				stage = (oper->start()+oper->chainingLength() - data->start()) / cadency;
			else
				stage = (oper->start()+oper->length() - data->start()) / cadency;
			// calcul de la profondeur du registre (registre ou fifo ?)
			if (stage>max_stage)
				max_stage=stage;
		}
	}
	return max_stage;
} 


//! Get the register inside which is stored a given Data at a given time. (EJ 09/2007)
//! @param Input. time is the time.
//! @param Input. date is a pointer to the data stored in the register at time "time".
//!
//! @result a pointer to a Register.
//const Reg * getReg(long time, const Data *data) const {
//	RegOut::const_iterator it;
//	for(it = begin(); it != end(); it++) {
//		const Reg *reg = (*it).second;
//		for(long i = 0; i < reg->use_nb(); i++) {
//			const RegUse *use = reg->getUse(i);
//			if (use->_data->name() == data->name()) return reg;
//		}
//	}
//	cerr << "Internal error: no register contains Data " << data->name() << " at time " << time << endl;
//	exit(1);
//}

//! Add use of register (EJ 09/2007)
void Reg::addUse(const Data *data, const Operation *oper/* , long cadency, long stage*/, long pos,bool isinput,const CDFG *cdfg)
{

	const OperatorInstance *inst=oper->inst();
	if ((inst!=NULL) && (isinput))
	{
		ostringstream reguse_name;
		reguse_name << name() << "@" << inst->instance_name() << "@" << pos;
		((OperatorInstance *)inst)->add_inputUseReg(new InputOperatorRegUse(this, pos,reguse_name.str()));
	}
	/* si il s'agit d'une sortie d'un operateur ou d'une entree lue sur le bus il faut générer le load : la cas particulier d'une lecture dynamique est 
	pour l'instant considéré comme la sortie d'une operator fictif mem_read */
	if ((!isinput) ||  (data->isASuccessorOf(cdfg->source())))
	{
		RegUse *use = new RegUse(data, oper, /* cadency, stage,*/ pos, isinput);
		_uses.push_back(use);
	}
}

//! Get use of register (EJ 09/2007)
const RegUse* Reg::getUse(long i) const
{
	if (i<0 || i>=_uses.size())
	{
		cerr << "fatal error : register " << _name << " hasn't more than " << _uses.size() << " uses : " << i << endl;
		exit(1);
	}
	return _uses[i];
}

//! Get number of use of register (EJ 09/2007)
long Reg::use_nb() const
{
	return _uses.size();
}

//! register load a new value at cycle_time (EJ 09/2007)
bool Reg::is_load_at(long cycle_time,long cadency) const
{
	for (int i=0; i<_uses.size(); i++)
	{
		RegUse *use = _uses[i];
		if (use->_load_no_modulo_cadency % cadency == cycle_time) return true;
	}
	return false;
}



/* ! tell if reg is connected to an input of operator instance
bool Reg::isConnectedToInputOperatorInstance(const OperatorInstance *inst,long input_pos) const
{
	long width=0;
	for (int i=0; i<_uses.size(); i++)
	{
		RegUse *use = _uses[i];
		if (use->_input)
		{
			const Operation*	oper=use->_oper;
			const OperatorInstance *inst_reg=oper->inst();	
			long pos= use->_pos;
			if ((pos==input_pos) && (inst_reg==inst))
			{
				return true;
			}
		}
	}
	return false;
} */



//! count number of time reg is connected to an output of an different operator instance
void Reg::numberOfInputMuxFromOutputOperatorInstance(long max_operator_number,long *nb_mux,long *mux_width) const
{

	bool *operator_list_used=new bool[max_operator_number+1];
	for (int op_i=0;op_i<max_operator_number+1;op_i++)
		operator_list_used[op_i]=false;
	bool already_connect_to_an_output_operator=false;
	for (int i=0; i<_uses.size(); i++)
	{
		RegUse *use = _uses[i];
		if (!use->_input)
		{
			const Operation*	oper=use->_oper;
			const Data*	data=use->_data;
			const OperatorInstance *inst_reg=oper->inst();	
			if (/* assign */(inst_reg!=NULL))
			{
					if (operator_list_used[inst_reg->no()]==false)
					{
						operator_list_used[inst_reg->no()]=true;
						if (already_connect_to_an_output_operator==false)
							already_connect_to_an_output_operator=true;
						else 
							(*nb_mux)++;	//need a mux 	
					}
					//update width
					long width=_bitwidth;
					if (data->bitwidth()>width)
						width=data->bitwidth();
					if (*mux_width<width)
						*mux_width=width;
			}
		}
	}
	delete [] operator_list_used;
}




//! count number of time reg is connected to an output of an different operator instance except operator inst
void Reg::numberOfInputMuxFromOutputOperatorInstanceExceptThisOperator(const OperatorInstance *inst,long max_operator_no,long *nb_mux,long *mux_width) const
{

	bool *operator_list_used=new bool[max_operator_no+1];
	for (int op_i=0;op_i<max_operator_no+1;op_i++)
		operator_list_used[op_i]=false;
	for (int i=0; i<_uses.size(); i++)
	{
		RegUse *use = _uses[i];
		if (!use->_input)
		{
			const Operation*	oper=use->_oper;
			const Data*	data=use->_data;
			const OperatorInstance *inst_reg=oper->inst();	
			if (/* assign */(inst_reg!=NULL) && (inst_reg!=inst))
			{
					if (operator_list_used[inst_reg->no()]==false)
					{
						operator_list_used[inst_reg->no()]=true;
						(*nb_mux)++;	//need a mux	
					}
					//update width
					long width=_bitwidth;
					if (data->bitwidth()>width)
						width=data->bitwidth();
					if (*mux_width<width)
						*mux_width=width;
			}
		}
	}
	delete [] operator_list_used;
}


//! Return true if register is free in an intervalle (DH 02/2009)
bool Reg::isRegFree(long time_start_modulo_cadency,long time_end_modulo_cadency) const
{

	for (int i=0; i<_uses.size(); i++)
	{
		//TODO_DH
		RegUse *use = _uses[i];
		if (((time_start_modulo_cadency >= use->_data->get_start_modulo_cadency()) && (time_start_modulo_cadency < use->_data->get_end_modulo_cadency())) ||
			((time_end_modulo_cadency > use->_data->get_start_modulo_cadency()) && (time_end_modulo_cadency <= use->_data->get_end_modulo_cadency())) ||
			((time_start_modulo_cadency <= use->_data->get_start_modulo_cadency()) && (time_end_modulo_cadency >= use->_data->get_end_modulo_cadency()))

			)
		{
			//cout << "not free" << endl;
			return false;
		}
	}
	//cout << "free" << endl;
	//cout << "Reg Free [" << time_start_modulo_cadency  << ":" << time_end_modulo_cadency << "] : " << endl;
	//info();
	//cout << "reg " << name() << ": " << endl;
	return true;
}

//! Return true if register is use by an assign
bool Reg::is_use_by_assign() const
{
	bool use_by_assign = false;
	for (int i=0; i<_uses.size(); i++)
	{
		RegUse *use = _uses[i];
		if (!use->_oper) continue;
		if (!use->_oper->inst() && _uses.size() != 1)
		{
			;//cout << "warning : register is use by assign and an other operator" << endl;
			//exit(1);
		}
		if (!use->_oper->inst()) use_by_assign = true;
	}
	return use_by_assign;
}

/* GL 27/04/2007 :	compute data bitwidth */
//! data bitwidth is defined as max width of succesors or predecessors.
//! @param Input. d is a pointer to the data.
void Reg::reg_width()
{
	long width = 0;
	for (int i = 0; i < use_nb(); i++)
	{
		const RegUse *use = getUse(i);
		const Operation* oper=use->_oper;
		const OperatorInstance *inst = oper->inst();
		if (inst)
		{
			const Operator *op=inst->oper();
			const Port *port = op->getInputPort(use->_pos);

			width = (width> port->bitwidth())? width : port->bitwidth();
		}
	}
	if (width>bitwidth()) bitwidth(width);
}


void Reg::variable_vhdl_declaration(ofstream &f, const string &tabul, const Switches *_sw) const
{

	if (_sw->bitwidth_aware() == false)
	{
		if (fifoStage())
		{
			f << tabul << "type " <<  name() << "_fifo" << " is array (0 to " << fifoStage() << ") of word;\n";
			f << tabul << "variable " << name() << " : " << name() << "_fifo;\n";
		}
		else
		{
			f << tabul << "variable " << name() << " : word;\n" ;
		}
	}
	else
	{
		if (fifoStage())
		{
			f << "    type " << name() << "_fifo" << " is array (0 to " << fifoStage() << ") of std_logic_vector(" << bitwidth()-1 << " downto 0);" << "\n";
			f << "    variable " << name() << " : " << name() << "_fifo;" << "\n";
		}
		else
		{
			f << tabul << "variable " << name() << " : std_logic_vector(" << bitwidth()-1 << " downto 0);" << "\n";
		}
	}

}

void Reg::component_sigs_vhdl_declaration(ofstream &f, const string &tabul, const Switches *_sw,long _bits) const
{
	f << tabul << "signal " << name() << "_load : std_logic;\n" ;
	if (_sw->bitwidth_aware() == false)
	{
			f << tabul << "signal " << name() << "_in : word;\n" ;
			if (fifoStage())
			{
				f << tabul << "signal " << name() << "_out : std_logic_vector(" << (fifoStage()+1)*_bits-1 <<" downto 0);\n" ;
			}
			else
				f << tabul << "signal " << name() << "_out : word;\n" ;
	}
	else
	{
			f << tabul << "signal " << name() << "_in : std_logic_vector(" << bitwidth()-1 << " downto 0);\n" ;
			if (fifoStage())
			{
				f << tabul << "signal " << name() << "_out : std_logic_vector(" << (fifoStage()+1)*bitwidth()-1 <<" downto 0);\n" ;
			}
			else
				f << tabul << "signal " << name() << "_out : std_logic_vector(" << bitwidth()-1 << " downto 0);\n" ;
	}

}

void Reg::instantiate_component_vhdl(ofstream &f, const string &tabul, const Switches *_sw,long _bits) const
{
	if (fifoStage())
	{
		f << tabul << "comp_" << name() << " : reg_fifo_op\n" ;
		if (_sw->bitwidth_aware() == false)
			f << tabul << "generic map(size_reg=>" << _bits << ", stage=>"<< fifoStage()+1 <<")\n";
		else
			f << tabul << "generic map(size_reg=>" << bitwidth() << ", stage=>"<< fifoStage()+1 <<")\n";
	}
	else
	{
		f << tabul << "comp_" << name() << " : reg_op\n" ;
		if (_sw->bitwidth_aware() == false)
			f << tabul << "generic map(size_reg=>" << _bits << ")\n";
		else
			f << tabul << "generic map(size_reg=>" << bitwidth() << ")\n";
	}
	f << tabul << "port map(" ;
	f << "d=>" << name() << "_in," ;
	f << " load=>" << name() << "_load," ;
	f << " clrb=>rstb," ;
	f << " clk=>clk," ;
	f << " q=>" << name() << "_out);\n\n" ;
}


void Reg::signal_vhdl_declaration(ofstream &f, const string &tabul, const Switches *_sw) const
{
	if (_sw->bitwidth_aware() == false)
	{
		if (fifoStage())
		{
			f << tabul << "type " << name() << "_fifo" << " is array (0 to " << fifoStage() << ") of word;\n";
			f << tabul << "signal " << name() << " : " << name() << "_fifo;\n";
		}
		else
		{
			f << tabul << "signal " << name() << " : word;\n" ;
		}
	}
	else
	{
		if (fifoStage())
		{
			f << tabul << "type " << name() << "_fifo" << " is array (0 to " << fifoStage() << ") of std_logic_vector(" << bitwidth()-1 << " downto 0);\n";
			f << tabul << "signal " << name() << " : " << name() << "_fifo;\n";
		}
		else
		{
			f << tabul << "signal " << name() << " : std_logic_vector(" << bitwidth()-1 << " downto 0);\n" ;
		}
	}

}


//! tell if reg is connected to an output of operator instance
long Reg::isConnectedToOutputOperatorInstance(const OperatorInstance *inst,long output_pos) const
{
	for (int i=0; i<_uses.size(); i++)
	{
		RegUse *use = _uses[i];
		if ((!use->_input) &&  (use->_pos==output_pos))
		{
			const Operation*	oper=use->_oper;
			const OperatorInstance *inst_reg=oper->inst();	
			if (inst_reg==inst)
			{
				return true;
			}
		}
	}
	return false;
}

/* see lopass, J. Cong */
/* cost an reg allocation in terms of mux 2->1 */
double RegSynthesis::computeWeight(const Data *data,const Reg *reg,long max_operator_no)
{

	vector<const Operation *> data_predecessors;
	vector<const Operation *> data_successors;
	int i;

	long result;
	/* A MUX is introduced before a register r when more than one
		functional units produce results and store them into this register. We use MUXR(r) to represent this MUX. */
	long MUXR_InputsWidth=0;
	long MUXR_Number=0;
	long MUXR_Width=data->bitwidth();
	long over_cost=0;
	//data has only one functional unit producer but data predessor may be control (sink) or operation node. 
	get_operations_predecessors(data, &data_predecessors);
	if  (data_predecessors.size()==1)
	{
			const Operation *pre = data_predecessors.at(0);
			reg->numberOfInputMuxFromOutputOperatorInstanceExceptThisOperator(pre->inst(),max_operator_no,&MUXR_Number,&MUXR_Width);
			long output_pos=pre->getOutputPos(data);
			if (reg->isConnectedToOutputOperatorInstance(pre->inst(),output_pos))
			{
				//the functional unit producing data already drove register reg
				over_cost=0;
			}
			else 
				over_cost=1;

	}
	MUXR_InputsWidth=(MUXR_Number+over_cost)*MUXR_Width;//need MUXR_Width mux MUXR_Number to 1
	/* A MUX is introduced before a port p of a functional unit when more than one registers feeding data to this port.
	MUXP(p) is used to represent this MUX. */
	long MUXP_InputsWidth=0;
	get_operations_successors(data, &data_successors);
	for (i = 0; i < data_successors.size(); i++)
	{
			const Operation *succ = data_successors.at(i);
			long inputpos=succ->getInputPos(data);
			if (inputpos==-1) {
				inputpos=succ->getInputPosDFF(data);
			}
			const Port *port=succ->inst()->oper()->getInputPort(inputpos);
			MUXP_InputsWidth+=succ->inst()->numberOfInputMuxToInputOperatorInstanceExceptThisRegister(reg,inputpos)*port->bitwidth();
	}

	result=MUXR_InputsWidth+MUXP_InputsWidth;	
	return result;
}


void RegSynthesis::compute_data_life(vector <const Data *> *hardwired_constant_reg,vector <const Data *> *fifo_data_reg,vector <const Data *> *sorted_interval_lifetime)
{
	for (int i = 0; i < _cdfg->nodes().size(); i++)
	{
		if (_cdfg->nodes()[i]->type()==CDFGnode::DATA)
		{
			Data * d = (Data *)_cdfg->nodes()[i];
			//debug cout << d->name() << "[" << d->start() << ":" <<  d->end() << "]\n";
			d->compute_data_life_no_modulo_cadency(_period,_mem_access);
			d->compute_data_life_modulo_cadency(_cadency,_period,_mem_access);
			//debug cout << d->name() << "[" << d->get_start_modulo_cadency() << ":" <<  d->get_end_modulo_cadency() << "]\n";

			//skip data between chained operations 
			if (Reg::is_chained(d))
				continue;
			//skip unused constant (mul to shift conversion)
			if ((d->type()==Data::CONSTANT) &&
				(d->successors.empty()==true))
				continue;
			//skip unused variable (unused delay element)
			if ((d->type()==Data::VARIABLE) &&
				(d->successors.empty()==true))
			{
				//debug cout << d->name() << endl;
				continue;
			}
			//skip input only used in delay
			if ((d->type()==Data::INPUT ) && (d->successors.empty()) && (d->readvar()))
			{
				continue;
			}
			//skip unused dynamic element 
			if ((d->isADynamicElement()) && (d->predecessors.empty()==true))
				continue;
			//skip unused dynamic access only keep dynamic read access storage
			if ((d->isADynamicAccess()) && (d->predecessors.size()==1) )
			{
				CDFGnode *predecessor= (CDFGnode *)d->predecessors.at(0)->source;
				if (predecessor->type()==CDFGnode::CONTROL)
					continue;
				if (predecessor->type()==CDFGnode::OPERATION)
				{
					Operation *mem_operation = (Operation *)predecessor;
					if (mem_operation->function()->symbolic_function()=="mem_write")
					continue;
				}
			}
			if ((d->type()==Data::CONSTANT) && (d->hardwire() == true))
			{
				hardwired_constant_reg->push_back((const Data *)d);
			}
			else	
			{
				if (get_max_stage(_sw->vhdl_type(),d,_period,_cadency)>0)
				{
					fifo_data_reg->push_back((const Data *)d);
				}
				else				
					sorted_interval_lifetime->push_back((const Data *)d);
			}
		}
	}
}

int RegSynthesis::MinimumRegisterNumber(vector <const Data *> &sorted_interval_lifetime)
{
	int row,col;
	//search maximum register number : use matrix of bool to find max lifetime overlap 
	//create matrix row=data,column =register;
	int nrows=_cadency/_period;
	int ncols=sorted_interval_lifetime.size();
	Matrix<int> data_fife_table(nrows,ncols);
	// Initialize matrix with 1 or 0 
	for ( col = 0 ; col < ncols ; col++ )
	{
		const Data *current_data_to_store=sorted_interval_lifetime[col];
		for ( row = 0 ; row < nrows ; row++ )
		{
			if (current_data_to_store->is_alive_at_cycletime(row*_period))
				data_fife_table(row,col) = 1;
			else
				data_fife_table(row,col) = 0;
		}
	}
	/*debug
	for ( row = 0 ; row < nrows ; row++ )
	{
		for ( col = 0 ; col < ncols ; col++ )
		{
			cout << "  " << data_fife_table(row,col) ;
		}
		cout << endl;
	} */
	int nb_register=0;
	int control_step_max=0;
	for ( row = 0 ; row < nrows ; row++ )
	{
		int sum=0;
		for ( col = 0 ; col < ncols ; col++ )
		{
			sum+=data_fife_table(row,col);
		}
		if (sum>nb_register)
		{
			nb_register=sum;
			control_step_max=row;
		}
	}
	return nb_register;
}

//! LeftEdge register allocation   (no mux optimisation)
void RegSynthesis::RegisterAllocation_LeftEdge(long bits)
{
	int i;
	vector <const Data *> hardwired_constant_reg;
	vector <const Data *> fifo_data_reg;
	vector <const Data *> sorted_interval_lifetime;

	compute_data_life(&hardwired_constant_reg,&fifo_data_reg,&sorted_interval_lifetime);
	std::sort(sorted_interval_lifetime.begin(), sorted_interval_lifetime.end(),CompareDataStart()); 
	int minimal_nb_register=MinimumRegisterNumber(sorted_interval_lifetime);

	cout << "Number of hardwired constant false register : " << hardwired_constant_reg.size() << endl;
	cout << "Number of fifo register : " << fifo_data_reg.size() << endl;
	cout << "Minimal Number of simple register : " << minimal_nb_register << endl;

	//registers assignment/allocation
	int nb_register=hardwired_constant_reg.size()+fifo_data_reg.size();
	_reg_out->init(nb_register);
	int current_reg_index=0;
	//first false register allocation on hardwired_constant
	if (hardwired_constant_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<hardwired_constant_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)hardwired_constant_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->false_register_hardwire_constant(true);
			 current_reg_index++;
		}
	}
	//then register allocation on fifo_data (pipeline)
	if (fifo_data_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<fifo_data_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)fifo_data_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->fifoStage(get_max_stage(_sw->vhdl_type(),current_data,_period,_cadency));
			 current_reg_index++;
		}
	}
	int current_reg_nofifo_nohardwired_offset=current_reg_index;

	//finally simple register allocation 
	if (sorted_interval_lifetime.empty()==false)
	{
		for (int current_data_index=0;current_data_index<sorted_interval_lifetime.size();current_data_index++)
		{
			 bool allocated=false;
			 Data *current_data=(Data *)sorted_interval_lifetime[current_data_index];
			 int current_simple_reg_index=current_reg_nofifo_nohardwired_offset;
			 while ((allocated==false) && current_simple_reg_index<_reg_out->size())
			 {
				Reg *current_reg=_reg_out->getReg(current_simple_reg_index);
				if (current_reg->isRegFree(current_data->get_start_modulo_cadency(),current_data->get_end_modulo_cadency())==true)
				{
					 data_allocation(_sw->vhdl_type(), current_data,current_reg,_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
					 allocated=true;
				}
				current_simple_reg_index++;
			 }
			 //no reg free : add new one
			 if (allocated==false)
			 {
				_reg_out->add(1);
				Reg *current_reg=_reg_out->getReg(current_simple_reg_index);
				data_allocation(_sw->vhdl_type(), current_data,current_reg,_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 }
		}
	}
	cout << "\nNumber of simple register (Left Edge) : " << _reg_out->size()-hardwired_constant_reg.size()-fifo_data_reg.size() << endl;
	cout << "Number of flip flop : " << _reg_out->nbFlipFlop() << endl;
	cout << "Number of mux 2 to 1 : " << _reg_out->nbMux2to1(_sched->max_operator_no())  + _sched->NumberOfMux2to1InputOperatorInstance() << endl;

	//debug
	//_reg_out->info();

	//clear vector used
	fifo_data_reg.clear();
	hardwired_constant_reg.clear();
	sorted_interval_lifetime.clear();
	
}

void RegSynthesis::RegisterAllocation_MLEA(long bits)
{
	vector <const Data *> hardwired_constant_reg;
	vector <const Data *> fifo_data_reg;
	vector <const Data *> sorted_interval_lifetime;

	compute_data_life(&hardwired_constant_reg,&fifo_data_reg,&sorted_interval_lifetime);
	std::sort(sorted_interval_lifetime.begin(), sorted_interval_lifetime.end(),CompareDataStart()); 
	int minimal_nb_register=MinimumRegisterNumber(sorted_interval_lifetime);

	cout << "Number of hardwired constant false register : " << hardwired_constant_reg.size() << endl;
	cout << "Number of fifo register : " << fifo_data_reg.size() << endl;
	cout << "Minimal Number of simple register : " << minimal_nb_register << endl;

	//registers assignment/allocation
	int nb_register=hardwired_constant_reg.size()+fifo_data_reg.size();
	_reg_out->init(nb_register+1);
	int current_reg_index=0;
	//first false register allocation on hardwired_constant
	if (hardwired_constant_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<hardwired_constant_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)hardwired_constant_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->false_register_hardwire_constant(true);
			 current_reg_index++;
		}
	}
	//then register allocation on fifo_data (pipeline)
	if (fifo_data_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<fifo_data_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)fifo_data_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->fifoStage(get_max_stage(_sw->vhdl_type(),current_data,_period,_cadency));
			 current_reg_index++;
		}
	}
	int current_reg_nofifo_nohardwired_offset=current_reg_index;
	long max_operator_no=_sched->max_operator_no();

	//finally simple register allocation 
	if (sorted_interval_lifetime.empty()==false)
	{
		for (int current_data_index=0;current_data_index<sorted_interval_lifetime.size();current_data_index++)
		{
			 Data *current_data=(Data *)sorted_interval_lifetime[current_data_index];
			 int current_simple_reg_index=current_reg_nofifo_nohardwired_offset;
			 Reg *allocated_reg=NULL;
			 double min_cost=numeric_limits< double >::max();
			 double current_cost;

			 while (current_simple_reg_index<_reg_out->size())
			 {
				Reg *current_reg=_reg_out->getReg(current_simple_reg_index);
				if (current_reg->isRegFree(current_data->get_start_modulo_cadency(),current_data->get_end_modulo_cadency())==true)
				{
					current_cost=computeWeight(current_data,current_reg,max_operator_no);
					if (current_cost<min_cost)
					{
						min_cost=current_cost;
						allocated_reg=current_reg;
					}
				}
				current_simple_reg_index++;
			 }
			 //no reg free : add new one
			 if (allocated_reg!=NULL)
			 {
				data_allocation(_sw->vhdl_type(), current_data,allocated_reg,_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 }
		     else
			 {
				_reg_out->add(1);
				Reg *current_reg=_reg_out->getReg(current_simple_reg_index);
				data_allocation(_sw->vhdl_type(), current_data,current_reg,_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 }
		}
	}
	cout << "\nNumber of simple register (MLEA) : " << _reg_out->size()-hardwired_constant_reg.size()-fifo_data_reg.size() << endl;
	cout << "Number of flip flop : " << _reg_out->nbFlipFlop() << endl;
	cout << "Number of mux 2 to 1 : " << _reg_out->nbMux2to1(_sched->max_operator_no())+_sched->NumberOfMux2to1InputOperatorInstance() << endl;

	//debug
	//_reg_out->info();

	//clear vector used
	fifo_data_reg.clear();
	hardwired_constant_reg.clear();
	sorted_interval_lifetime.clear();
}



//! False register allocation  one register per data  (no reg fuse)
void RegSynthesis::RegisterAllocation_NONE(long bits)
{
	vector <const Data *> hardwired_constant_reg;
	vector <const Data *> fifo_data_reg;
	vector <const Data *> simple_data_reg;
	
	compute_data_life(&hardwired_constant_reg,&fifo_data_reg,&simple_data_reg);
	cout << "Number of hardwired constant : " << hardwired_constant_reg.size() << endl;
	cout << "Number of fifo variable : " << fifo_data_reg.size() << endl;
	cout << "Number of simple variable : " << simple_data_reg.size() << endl;
	int nb_register=hardwired_constant_reg.size()+ fifo_data_reg.size()+simple_data_reg.size();
	_reg_out->init(nb_register);
	int current_reg_index=0;
	//first false register allocation on hardwired_constant
	if (hardwired_constant_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<hardwired_constant_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)hardwired_constant_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->false_register_hardwire_constant(true);
			 current_reg_index++;
		}
	}
	//then register allocation on fifo_data (pipeline)
	if (fifo_data_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<fifo_data_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)fifo_data_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->fifoStage(get_max_stage(_sw->vhdl_type(),current_data,_period,_cadency));
			 current_reg_index++;
		}
	}
	//finally simple register allocation 
	if (simple_data_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<simple_data_reg.size();current_data_index++)
		{
			 Data *current_data=(Data *)simple_data_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 current_reg_index++;
		}
	}
	cout << "Maximum umber of flip flop : " << _reg_out->nbFlipFlop() << endl;
	cout << "Maximum Number of mux 2 to 1 : " << _reg_out->nbMux2to1(_sched->max_operator_no()) +_sched->NumberOfMux2to1InputOperatorInstance() << endl;

	//clear vector used
	simple_data_reg.clear();
	hardwired_constant_reg.clear();
	fifo_data_reg.clear();

}

//! Register allocation based on Cluster and MWBM (DH 02/2009)
void RegSynthesis::RegisterAllocation_CLUSTER_MWBM(long bits /* , bool fine_allocation */)
{
	int row,col;
	//sort data with start time 
	vector <const Data *> sorted_interval_lifetime;
	vector <const Data *> hardwired_constant_reg;
	vector <const Data *> fifo_data_reg;
	
	int index_data_sorted;
	int current_cluster_index;


	compute_data_life(&hardwired_constant_reg,&fifo_data_reg,&sorted_interval_lifetime);
	std::sort(sorted_interval_lifetime.begin(), sorted_interval_lifetime.end(),CompareDataStart()); 
	/* debug : used  in excel to view gantt
	for (index_data_sorted = 0; index_data_sorted < sorted_interval_lifetime.size(); index_data_sorted++)
	{	
		const Data *d=sorted_interval_lifetime[index_data_sorted];
		cout << d->name() << "\t" << d->get_start_modulo_cadency() << "\t" << d->get_end_modulo_cadency() << endl;
	}*/
	//search minimum register number : use matrix of bool to find max lifetime overlap 
	int nb_register=MinimumRegisterNumber(sorted_interval_lifetime);
	//data life cluster : create cluster of mutually unsharable variables (two variables are
	//unsharable if and only if their lifetimes overlap).
	vector< vector<const Data *> > data_life_cluster;
	data_life_cluster.resize(10);
	int data_life_cluster_index=0;
	data_life_cluster[data_life_cluster_index].push_back(sorted_interval_lifetime[0]);
	//debug cout << "put " << sorted_interval_lifetime[0]->name() <<" on cluster 0" << endl;
	for (index_data_sorted = 1; index_data_sorted < sorted_interval_lifetime.size(); index_data_sorted++)
	{
		const Data *current_data_to_store=sorted_interval_lifetime[index_data_sorted];
		vector<const Data *> current_cluster=data_life_cluster[data_life_cluster_index];
		bool not_overlap=false;
		for (int index_data_cluster = 0; (index_data_cluster < current_cluster.size()) && !not_overlap; index_data_cluster++)
		{
			const Data *current_data_cluster=(const Data *)current_cluster[index_data_cluster];
			not_overlap =  current_data_to_store->get_start_modulo_cadency()>=current_data_cluster->get_end_modulo_cadency();
		}
		if (not_overlap)
		{
			data_life_cluster_index++;
			if (data_life_cluster_index>=data_life_cluster.size())
				data_life_cluster.resize(data_life_cluster.size()*2);
		}
		//debug cout << "put " << current_data_to_store->name() <<" on cluster " << data_life_cluster_index << endl;
		data_life_cluster[data_life_cluster_index].push_back(current_data_to_store);
	}
	//
	//cout << "Number of cluster data life : " << data_life_cluster_index << endl;
	if (hardwired_constant_reg.empty()==false)
		nb_register+=hardwired_constant_reg.size();
	if (fifo_data_reg.empty()==false)
		nb_register+=fifo_data_reg.size();
	cout << "Number of hardwired constant false register : " << hardwired_constant_reg.size() << endl;
	cout << "Number of fifo register : " << fifo_data_reg.size() << endl;
	cout << "Minimal Number of simple register : " << nb_register-hardwired_constant_reg.size()-fifo_data_reg.size() << endl;
	//registers assignment/allocation
	nb_register=hardwired_constant_reg.size()+fifo_data_reg.size()+data_life_cluster[0].size();
	_reg_out->init(nb_register);
	int current_reg_index=0;
	//first false register allocation on hardwired_constant
	if (hardwired_constant_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<hardwired_constant_reg.size();current_data_index++)
		{
			 Data *current_data_cluster=(Data *)hardwired_constant_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data_cluster,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
			 reg->false_register_hardwire_constant(true);
			 current_reg_index++;
		}
	}
	//then register allocation on fifo_data (pipeline)
	if (fifo_data_reg.empty()==false)
	{
		for (int current_data_index=0;current_data_index<fifo_data_reg.size();current_data_index++)
		{
			 Data *current_data_cluster=(Data *)fifo_data_reg[current_data_index];
			 data_allocation(_sw->vhdl_type(), current_data_cluster,_reg_out->getReg(current_reg_index),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
			 Reg *reg=_reg_out->getReg(current_reg_index);
 			 reg->fifoStage(get_max_stage(_sw->vhdl_type(),current_data_cluster,_period,_cadency));
			 current_reg_index++;
		}
	}
	int current_reg_nofifo_nohardwired_offset=current_reg_index;
	//then register allocation on data_life_cluster[0]
	vector<const Data *>  current_cluster=data_life_cluster[0];
	for (int current_data_index=0;current_data_index<current_cluster.size();current_data_index++)
	{
		 Data *current_data_cluster=(Data *)current_cluster[current_data_index];
		 data_allocation(_sw->vhdl_type(), current_data_cluster,_reg_out->getReg(current_reg_index++),_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
	}
	if (!_sw->use_IHM())
	{
		cout << "Register allocation processing  :  0%";
		cout.flush();
	}
	long max_operator_no=_sched->max_operator_no();

	//MWBM for each data_life_cluster
	for (current_cluster_index=1;current_cluster_index<=data_life_cluster_index;current_cluster_index++)
	{
		vector<const Data *>  current_cluster=data_life_cluster[current_cluster_index];
		long nb_register_add=current_cluster.size();
		nb_register+=nb_register_add;
		_reg_out->add(nb_register_add);
		//_reg_out->info();

		/* useful for file_allocation long nb_data_allocated=0;
		long nb_data_to_allocate=current_cluster.size();
		while (nb_data_allocated!=nb_data_to_allocate)
		{ */
			//create matrix row=data,column =register;
			int nrows=current_cluster.size();
			int ncols=nb_register-current_reg_nofifo_nohardwired_offset;
			Matrix<double> matrix(nrows,ncols);
			//Matrix<double> matrix_cost(nrows,ncols);
			int nb_register_free=0;
			// Initialize matrix with weigth
			for ( row = 0 ; row < nrows ; row++ )
			{
				for ( col = 0 ; col < ncols ; col++ )
				{
					Data *current_data_cluster=(Data *)current_cluster[row];	
					Reg *current_reg=_reg_out->getReg(current_reg_nofifo_nohardwired_offset+col);
					if (current_reg->isRegFree(current_data_cluster->get_start_modulo_cadency(),current_data_cluster->get_end_modulo_cadency())==true)
					{
						double cost = computeWeight(current_data_cluster,current_reg,max_operator_no);
						matrix(row,col) = cost;
						//matrix_cost(row,col) = cost;
						nb_register_free++;
					}
					else
					{
						matrix(row,col) = std::numeric_limits< double >::max();
						//matrix_cost(row,col) = std::numeric_limits< double >::max();
					}
				}
			}
			//matrix.info();
			if (nb_register_free<nrows)
			{
				cerr << "Internal Error : not enough register allocated" <<endl;
				exit(1);
			}
			// Apply Munkres algorithm to matrix.
			Munkres m;
			m.solve(matrix);
			//matrix.info();
			/* if (!fine_allocation)
			{ */
				//register allocation on current data_life_cluster
				for ( row = 0 ; row < nrows ; row++ )
				{
					for (  col = 0 ; col < ncols ; col++ ) 
					{
						if ( matrix(row,col) == 0 )
						{
							Data *current_data_cluster=(Data *)current_cluster[row];	
							Reg *current_reg=_reg_out->getReg(col+current_reg_nofifo_nohardwired_offset);
							data_allocation(_sw->vhdl_type(), current_data_cluster,current_reg,_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
							/* nb_data_allocated++; */
						}
					}
				}
			/* }
			else
			{
				// search best candidate for allocation
				double min_cost=numeric_limits< double >::max();
				int min_row=0;
				for ( row = 0 ; row < nrows ; row++ )
				{
					for ( col = 0 ; col < ncols ; col++ )
					{
						if (( matrix(row,col) == 0 ) && (matrix_cost(row,col)<min_cost))
						{
							min_cost=matrix_cost(row,col);
							min_row=row;
						}

					}
				}
				bool current_data_allocated=false;
				for (  col = 0 ; col < ncols && current_data_allocated==false ; col++ ) 
				{
					if ( matrix(min_row,col) == 0 )
					{
						Data *current_data_cluster=(Data *)current_cluster[min_row];	
						Reg *current_reg=_reg_out->getReg(col+current_reg_nofifo_nohardwired_offset);
						nb_data_allocated++;
						current_data_allocated=true;			
						data_allocation(_sw->vhdl_type(), current_data_cluster,current_reg,_cadency,_period,_sw->bitwidth_aware(),bits,_cdfg);
						current_cluster.erase(current_cluster.begin()+min_row);
					}
				}
			}
		} */
		/* eliminate unused register */
	//	_reg_out->info();
		_reg_out->eliminateUnusedRegister();
		nb_register=_reg_out->size();
	//	_reg_out->info();
		if (!_sw->use_IHM())
		{
			cout << "\rRegister allocation processing  : " << (current_cluster_index*100/data_life_cluster_index) << " %" ;
			cout.flush();
		}
	}
	//debug _reg_out->info();
	cout << "\nOptimal number of simple register for mux optimization (CLUSTER_MWBM) : " << nb_register-hardwired_constant_reg.size()-fifo_data_reg.size() << endl;
	cout << "Number of flip flop : " << _reg_out->nbFlipFlop() << endl;
	cout << "Number of mux 2 to 1 : " << _reg_out->nbMux2to1(_sched->max_operator_no())+ _sched->NumberOfMux2to1InputOperatorInstance() << endl;

	//_reg_out->info();
	// clear data_file_cluster
	for (current_cluster_index=0;current_cluster_index<=data_life_cluster_index;current_cluster_index++)
	{
		data_life_cluster[current_cluster_index].clear();
	}
	data_life_cluster.clear();
	//clear vector used
	sorted_interval_lifetime.clear();
	hardwired_constant_reg.clear();
	fifo_data_reg.clear();
}

// end of: reg.cpp
