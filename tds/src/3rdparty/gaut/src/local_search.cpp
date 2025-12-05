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

  The University of S          outh Britany does not provide any warranty about
  the usage of GAUT2 and distributes it "as is" for scientific cooperation
  purpose only. All users are greatly advised to provide all the necessary
  protection issues to protect their data.
  ------------------------------------------------------------------------------
  */
//	File:		Local_search.cpp
//	Purpose:
//	Author:		Kods Trabelsi, LESTER, UBS
//	Date:		15/12/2007

#include <math.h>
#include "data.h"
#include "function.h"
#include "operator.h"
#include "reg.h"
#include "switches.h"
#include "cdfg.h"
#include "scheduling.h"
#include "graph.h"
#include "operation.h"
#include "port.h"
#include "resizing.h"
#include "local_search.h"
using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                  /////
////                                    Coding a solution			                                                  /////
////                                                                                                                  /////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! Register cost in slices
int reg_cost = 9;
//! Mux cost in slices
/*int mux_cost[50] ={ 0, 0, 9, 16, 16, 46, 46, 53, 53, 60, 76, 74,  82, 82, 94, 109, 119, 123, 132, 141, 142, 154, 162, 173,
  182, 183, 193, 202, 203, 212, 221, 223, 232, 233, 243, 252, 252, 264, 274,  283, 293, 294, 304, 314,
  314, 327, 336, 346, 347, 348};*/



int mux_cost[200] ={ 0,0, 9,16,16,25,32,32,32,41,48,57,57,64,76,64,64,73,80,89,89,96,108,105,105,112,124,124,128,140,140,128,128,137,144,153,153,160,172,169,169,176,188,188
	,192,204,204,201,201,208,220,220,224,236,236,240,240,252,252,265,268,268,281,265,265,272,272,281,288,288,297,304,304,304,313,320,320,329,336,336,336,336
		,345,352,352,361,368,368,368,377,384,393,393,400,412,400,400,400,409,416,416,425,432,432,432,441,448,457,457,464,476,464,464,473,480,489,489,496,508,505
		,505,512,524,523,528,540,539,528,528,537,544,544,544,560,572,569,569,576,588,588,592,604,604,601,601,608,620,620,624,636,636,640,640,652,652,665,668,668
		,681,665,665,672,684,684,688,700,700,704,704,716,716,729,732,732,745,736,736,748,748,761,764,764,777,780,780,780,793,802,796,809,823,800,800,800,800,809
		,828,816,825,844
};





void Solution::compute_interconnections_cost()
{

	long mux_dimension;
	long _multiplexers_cost;
	int i;
	// reset mux_list
	for(int k=0; k<200; k++)
	{
		_mux_list[k]=0;
	}
	// compute number of multiplexer precedding operators
	for(i=0; i<_dico_ports.size();i++)
	{
		mux_dimension =0;
		for(int j=0; j<regs()->size();j++)
			if(_score_array_port_var[i][j]>0)mux_dimension ++;

		if(mux_dimension >= 200)cout << "************************************ mux plus grand que 200 "<<endl;
		_mux_list[mux_dimension%200]++;

	}
	// compute number of multiplexer precedding registers
	for(i=0; i<_oi_lists.size();i++)
	{
		mux_dimension =0;
		for(int j=0; j<regs()->size();j++)
			if(_score_array_oper_var[i][j]>0)mux_dimension ++;
		_mux_list[mux_dimension%200]++;
	}

	/*for(int j=0; j<50; j++)
	  {
	  cout << _mux_list[j];
	  }
	  cout<<endl;*/

	// compute the cost of multiplexers
	_multiplexers_cost =0;
	for(int j=0; j<200; j++)
	{
		_multiplexers_cost = _multiplexers_cost + mux_cost[j]*_mux_list[j];
	}
	this->interconections_cost(_multiplexers_cost);
}



long max(long a, long b)
{
	if(a>b)return a;
	else return b;
}

// compute the total cost of the solution
void Solution::costing()
{

	int operators_area =0;
	RegOut::iterator reg_it;	//!< a register iterator
	int used_reg =0;
	// compute interconnections cost
	compute_interconnections_cost();
	// compute regiters cost
	for(reg_it = _regs->begin();  reg_it != _regs->end();reg_it++)
	{
#if 0
		Reg*reg = (*reg_it).second;
#else
					Reg*reg = (*reg_it);
#endif
		if(reg->_datas.size()!= 0)
		{
			used_reg++;
		}
	}
	if(delete_reg == true)
		used_reg--;
	registers_cost(used_reg*reg_cost);
	// compute operators cost
	for(int n = 0; n < this->_oi_lists.size(); n++)
	{
		operators_area += _oi_lists[n].Op->oper()->area();
	}
	operators_cost(operators_area);
	// compute total cost
	total_cost(max(operators_cost()+interconections_cost(),registers_cost()));
}



Solution::Solution(const Switches*sw,
				   const Cadency*cadency,				// in
				   const Clock*clk,					// in
				   RegOut*regs,
				   vector< Data*> datas,
				   vector< Operation*> operations,
				   OI_Lists oi_lists,
				   DICO_PORTS dico_ports,
				   short**port_reading_the_var,
				   short**oper_writing_the_var,
				   short**score_array_port_var,
				   short**score_array_oper_var,
				   short**datas_compatibility,
				   short**ops_compatibility)
{

	int i;
	// copy of main parameters
	_sw = sw;
	_cadency = cadency;
	_clk = clk;
	_datas_compatibility = datas_compatibility;
	delete_reg =false;
	nothing_changed = true;
	pos =0;


	//Memory allocation
	_port_reading_the_var= (short**)malloc(sizeof(short*)*dico_ports.size());
	if(_port_reading_the_var == NULL)
	{
		printf("failure of dynamic memory allocation of _port_reading_the_var");
		exit(1);
	}
	for(i = 0; i < dico_ports.size(); i++)
	{
		_port_reading_the_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(_port_reading_the_var[i]== NULL)
		{
			printf("failure of dynamic memory allocation of _port_reading_the_var[i]");
			exit(1);
		}
	}

	_oper_writing_the_var= (short**)malloc(sizeof(short*)*oi_lists.size());
	if(_oper_writing_the_var == NULL)
	{
		printf("failure of dynamic memory allocation of _oper_writing_the_var");
		exit(1);
	}
	for(i = 0; i < oi_lists.size(); i++)
	{
		_oper_writing_the_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(_oper_writing_the_var[i] == NULL)
		{
			printf("failure of dynamic memory allocation of _oper_writing_the_var[i]");
			exit(1);
		}
	}


	_score_array_port_var= (short**)malloc(sizeof(short*)*dico_ports.size());
	if(_score_array_port_var == NULL)
	{
		printf("failure of dynamic memory allocation of _score_array_port_var");
		exit(1);
	}
	for(i = 0; i < dico_ports.size(); i++)
	{
		_score_array_port_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(_score_array_port_var[i] == NULL)
		{
			printf("failure of dynamic memory allocation of _score_array_port_var[i]");
			exit(1);
		}
	}

	_score_array_oper_var= (short**)malloc(sizeof(short*)*oi_lists.size());
	if(_score_array_oper_var == NULL)
	{
		printf("failure of dynamic memory allocation of _score_array_oper_var");
		exit(1);
	}
	for(i = 0; i < oi_lists.size(); i++)
	{
		_score_array_oper_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(_score_array_oper_var[i] == NULL)
		{
			printf("failure of dynamic memory allocation of _score_array_oper_var[i]");
			exit(1);
		}
	}

	// copy of arrays
	for(i = 0; i < dico_ports.size(); i++)
		for(int j = 0; j < datas.size(); j++)
			_port_reading_the_var[i][j]= port_reading_the_var[i][j];

	for(i = 0; i < oi_lists.size(); i++)
		for(int j = 0; j < datas.size(); j++)
			_oper_writing_the_var[i][j]= oper_writing_the_var[i][j];

	for(i = 0; i < dico_ports.size(); i++)
		for(int j = 0; j < regs->size(); j++)
			_score_array_port_var[i][j]= score_array_port_var[i][j];

	for(i = 0; i < oi_lists.size(); i++)
		for(int j = 0; j < regs->size(); j++)
			_score_array_oper_var[i][j]= score_array_oper_var[i][j];

	//RegOut::iterator it;	//!< a register iterator
	for(i = 0; i < operations.size(); i++)
		//for(it_o = operations->begin();  it_o != operations->end();it_o++) {
		//Operation*o = it_o.second();
		_operations.push_back(operations[i]);
	//	}



	// copy of regout
	_regs = new RegOut();
	all_reg_cheked = false;
	RegOut::iterator it;	//!< a register iterator
	for(it = regs->begin();  it != regs->end();it++)
	{
#if 0
		Reg*reg = (* it).second;
#else
	Reg*reg = (*it);
#endif
		Reg*reg_copie=new Reg(reg);
		_regs->add(reg_copie);
	}

	// copy of oi_lists
	for(i = 0; i < oi_lists.size(); i++)
	{
		OI_List _oi_list;
		_oi_list.Operations=oi_lists[i].Operations;
		_oi_list.Op=oi_lists[i].Op;
		_oi_list.Nops=oi_lists[i].Nops;
		_oi_lists.push_back(_oi_list);
	}

	// copy of dico_ports
	for(i = 0; i < dico_ports.size(); i++)
	{
		struct Dico_port p;
		p.oper_index=dico_ports[i].oper_index;
		p.name = dico_ports[i].name;
		_dico_ports.push_back(p);

	}

	for(int k=0; k<200; k++)
	{
		_mux_list[k]=0;
	}

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                  /////
////                                    Neigborhood explorating module                                                /////
////                                                                                                                  /////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                 /////
////		                        First Neigborhood 			            					////
////                                                                                           /////
////////////////////////////////////////////////////////////////////////////////////////////////////


bool testCompatibility(const Data*d1,const Data*d2,const Cadency*cadency,const Clock*clk)
{
	vector<int> tab1;
	vector<int> tab2;
	tab1.clear();
	tab2.clear();
	tab1.resize(cadency->length()/clk->period());
	tab2.resize(cadency->length()/clk->period());

	if(d1->type() == Data::CONSTANT || d2->type() == Data::CONSTANT)return false;
	for(int i= d1->start(); i<d1->end();i=i+clk->period())
	{
		tab1[(i%cadency->length())/clk->period()]=1;
	}

	for(int j= d2->start(); j<d2->end();j=j+clk->period())
	{
		tab2[(j%cadency->length())/clk->period()]=1;
	}

	for(int t=0;t <(cadency->length()/clk->period());t++)
	{
		if((tab1[t] ==1)&&(tab2[t] == 1))return false;
	}
	return true;
}

//!This function find the register preceded by the larger multiplexer
//!We will move a data from this register
//!@param Input: regs is a pointer to the  RegOut
Reg* reg_source(const RegOut*regs)
{
	RegOut::const_iterator reg_it;	//!< a register iterator
	Reg*_reg_source;
	for(reg_it = regs->begin(); reg_it != regs->end(); reg_it++)
	{
#if 0
		Reg*reg = (*reg_it).second;
#else
		Reg*reg = (*reg_it);
#endif
		if((reg->checked == false)&&(reg->fifoStage() ==0)&&(reg->contain_const == false)&&(reg->_datas.size()> 0))
		{
			_reg_source = reg;
			if(reg->pos +1 == reg->_datas.size())reg->checked = true;
			return _reg_source;
		}
	}
	return NULL;
}

//!This function find the registers preceded by 0 multiplexer
//!We will move a data to one of this registers
//!@param Input: regs is a pointer to the  RegOut
vector <Reg*> reg_destination(const RegOut*regs, Reg*reg1)
{
	RegOut::const_iterator reg_it;	//!< a register iterator
	vector <Reg*> _reg_destination;
	if(reg1 != NULL)
	{
		for(reg_it = regs->begin(); reg_it != regs->end(); reg_it++)
		{
#if 0
		Reg*reg = (*reg_it).second;
#else
		Reg*reg = (*reg_it);
#endif
			if((reg != reg1)&&(reg->fifoStage() ==0)&&(reg->contain_const == false)&&(reg->_datas.size()> 0)&&(reg->failed_as_destination == false))
				_reg_destination.push_back(reg);
		}
	}
	return _reg_destination;
}




bool reg_fusion(Data*d1, Reg*reg2,const Cadency*cadency, const Clock*clk, short**compatibility)
{
	bool compatible = true;
	for(int j=0; j< reg2->_datas.size(); j++)
	{
		Data* d2 =reg2->_datas[j];
		if(compatibility[d1->getindex()][d2->getindex()]!= 1)compatible = false;
	}
	return compatible;
}

void update1(Solution*sol,int data_index, int reg1_index, int reg2_index)
{
	int i;
	// update the score arrays
	for(i=0; i<sol->_oi_lists.size();i++)
	{
		if(sol->_oper_writing_the_var[i][data_index] ==1)
		{
			sol->_score_array_oper_var[i][reg1_index]--;
			sol->_score_array_oper_var[i][reg2_index]++;
		}

	}
	for(i=0; i<sol->_dico_ports.size();i++)
	{
		if(sol->_port_reading_the_var[i][data_index] ==1)
		{
			sol->_score_array_port_var[i][reg1_index]--;
			sol->_score_array_port_var[i][reg2_index]++;
		}
	}

}


Solution*change_var_binding(Solution*sol)
{

	RegOut::iterator it;
	RegOut::iterator reg_it;
	bool moved;
	Operation* op;
	CDFGnode*node;

	sol->nothing_changed =false;
	Reg*reg1= reg_source(sol->regs());

	vector <Reg*> reg2= reg_destination(sol->regs(), reg1);
	if((reg1 == NULL))
	{
		sol->all_reg_cheked = true;
		sol->nothing_changed =true;
		return  sol;
	}

	Data* d1 =reg1->_datas[reg1->pos];

	sol->reg_source = reg1;
	moved = false;
	for(int j=0; j< reg2.size()&&(!moved); j++)
	{
		if(reg_fusion(d1, reg2[j],sol->cadency(), sol->clk(),sol->_datas_compatibility))
		{
			moved = true;
			d1->regno(reg2[j]->no());
			sol->reg_destination = reg2[j];
			sol->d = d1;
			if(reg1->_datas.size() ==1)
				sol->delete_reg =true;
			update1(sol,d1->getindex(),reg1->index(),reg2[j]->index());
			sol->nothing_changed = false;
			return(sol);
		}
	}

	if(!moved)
	{
		reg1->pos++;
		sol->nothing_changed =true;
	}
	return(sol);
}
////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                 /////
////		                        Second Neigborhood 			            					////
////                                                                                           /////
////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0

void update2_score_arrays(Operation* o1,Operation* o2,OperatorInstance*inst1,OperatorInstance*inst2,Solution*sol)
{

	int colomn;
	if((inst1 != NULL)&&(inst2 != NULL)&&(o1 != NULL)&&(o2 != NULL))
	{
		int j;
		for(j=0; j<o1->predecessors.size();j++)
		{
			CDFGedge*e = o1->predecessors[j];
			Data*d = (Data*)e->source;
#if 0
			Reg*r= (Reg*)sol->regs()->getReg(0,d);
#else
			Reg*r= (Reg*)sol->regs()->getReg(0);
#endif
			colomn= r->index();
			for(short k=0; k < inst1->inputInstancePorts();k++)
			{
				sol->_score_array_port_var[inst1->getInputInstancePort(k)->index()][colomn]--;
				sol->_score_array_port_var[inst2->getInputInstancePort(k)->index()][colomn]++;
			}
		}
		for(j=0; j<o2->predecessors.size();j++)
		{
			CDFGedge*e = o1->predecessors[j];
			Data*d = (Data*)e->source;
#if 0
			Reg*r = (Reg*)sol->regs()->getReg(0,d);
#else
			Reg*r= (Reg*)sol->regs()->getReg(0);
#endif
			colomn = r->index();
			for(short k=0; k < inst2->inputInstancePorts();k++)
			{
				sol->_score_array_port_var[inst2->getInputInstancePort(k)->index()][colomn]--;
				sol->_score_array_port_var[inst1->getInputInstancePort(k)->index()][colomn]++;
			}
		}

		for(j=0; j<o1->successors.size();j++)
		{
			CDFGedge*e = o1->predecessors[j];
			Data*d = (Data*)e->source;
#if 0
			Reg*r = (Reg*)sol->regs()->getReg(0,d);
#else
			Reg*r= (Reg*)sol->regs()->getReg(0);
#endif
			colomn = r->index();
			sol->_score_array_oper_var[inst1->index()][colomn]--;
			sol->_score_array_oper_var[inst2->index()][colomn]++;
		}
		for(j=0; j<o2->successors.size();j++)
		{
			CDFGedge*e = o1->predecessors[j];
			Data*d = (Data*)e->source;
#if 0
			Reg*r = (Reg*)sol->regs()->getReg(0,d);
#else
			Reg*r= (Reg*)sol->regs()->getReg(0);
#endif
			colomn = r->index();
			sol->_score_array_oper_var[inst2->index()][colomn]--;
			sol->_score_array_oper_var[inst1->index()][colomn]++;
		}

	}

}

void complete_update2(Operation* o1,Operation* o2,OperatorInstance*inst1,OperatorInstance*inst2,Solution*sol)
{

	//update the inst space in operations objects
	o1->inst(inst2);
	o2->inst(inst1);

	//update the oi_lists
	int n;
	int i;
	n =sol->_oi_lists[inst1->index()].Operations.size();
	for(i=0; i<n; i++)
	{
		if(sol->_oi_lists[inst1->index()].Operations[i]->name() == o1->name())
			sol->_oi_lists[inst1->index()].Operations.erase(sol->_oi_lists[inst1->index()].Operations.begin()+i);
	}
	sol->_oi_lists[inst2->index()].Operations.push_back(o1);

	for(i=0; i<sol->_oi_lists[inst2->index()].Operations.size(); i++)
	{
		if(sol->_oi_lists[inst2->index()].Operations[i]->name() == o2->name())
			sol->_oi_lists[inst2->index()].Operations.erase(sol->_oi_lists[inst2->index()].Operations.begin()+i);
	}
	sol->_oi_lists[inst1->index()].Operations.push_back(o2);

	//update the port_reading_the_var array
	short colomn=0;
	short j;
	for(j=0; j<o1->predecessors.size();j++)
	{
		colomn = o1->predecessors[j]->index();
		for(short k=0; k < inst1->inputInstancePorts();k++)
		{
			sol->_port_reading_the_var[inst1->getInputInstancePort(k)->index()][colomn]--;
			sol->_port_reading_the_var[inst2->getInputInstancePort(k)->index()][colomn]++;
		}
	}
	for(j=0; j<o2->predecessors.size();j++)
	{
		colomn = o2->predecessors[j]->index();
		for(short k=0; k < inst1->inputInstancePorts();k++)
		{
			sol->_port_reading_the_var[inst2->getInputInstancePort(k)->index()][colomn]--;
			sol->_port_reading_the_var[inst1->getInputInstancePort(k)->index()][colomn]++;
		}
	}

	//update operator_writing_the_var array
	for(j=0; j<o1->successors.size();j++)
	{
		sol->_oper_writing_the_var[inst1->index()][o1->successors[j]->index()]--;
		sol->_oper_writing_the_var[inst2->index()][o1->successors[j]->index()]++;
	}
	for(j=0; j<o2->successors.size();j++)
	{
		sol->_oper_writing_the_var[inst2->index()][o2->successors[j]->index()]--;
		sol->_oper_writing_the_var[inst1->index()][o2->successors[j]->index()]++;
	}
}


///!This function find the operation
//!We will move the operation from his actual operator to an other one
Operation* operation_source(Type*t)
{
	Operation* _o;
	if(t->pos_source < t->operations.size())
	{
		_o = t->operations[t->pos_source];
		t->pos_destination ++;
		return _o;
	}
	return NULL;
}

//!This function find the operation
//!We will move the operation from his actual operator to an other one
Operation* operation_destination(Type*t)
{
	;
	Operation* _o;
	if(t->pos_destination < t->operations.size())
	{
		_o = t->operations[t->pos_destination];
		return _o;
	}
	return NULL;
}


bool cycle_contain_function(const Function*function,Cycle*cycle)
{
	for(int i=0; i< cycle->types.size(); i++)
	{
		if(function == cycle->types[i]->function)return true;
	}
	return false;
}


Cycle* working_cycle(Cycles cycles)
{
	for(int i =0; i< cycles.size();i++)
		if(cycles[i]->checked == false)
		{
			return cycles[i];
		}
	return NULL;
}


Type*working_type(Cycle*c)
{
	for(int i =0; i< c->types.size();i++)
	{
		if((c->types[i]->checked == false)&&(c->types[i]->function->name()!= "assign"))
		{
			return  c->types[i];
		}
	}
	return NULL;
}


Solution*swap_op_binding(Solution*sol,Cycles cycles)
{

	//choose the operations
	Operation* o1;
	Operation* o2;
	Cycle*c;
	Type*t;

	c = working_cycle(cycles);
	if(c == NULL)
	{
		sol->all_cycles_cheked = true;
		sol->nothing_changed = true;
		return  sol;

	}

	t = working_type(c);
	if(t == NULL)
	{
		c->checked= true;
		sol->nothing_changed = true;
		return  sol;
	}

	o1= operation_source(t);
	o2= operation_destination(t);
	if((o1 == NULL))
	{
		sol->nothing_changed = true;
		t->checked = true;
		return  sol;
	}
	if((o2 == NULL))
	{
		sol->nothing_changed = true;
		t->pos_source++;
		t->pos_destination = t->pos_source;
		return  sol;
	}
	//update the solution
	OperatorInstance*inst1= (OperatorInstance*)o1->inst();
	OperatorInstance*inst2= (OperatorInstance*)o2->inst();
	sol->o1 =o1;
	sol->o2 =o2;
	sol->inst1 =inst1;
	sol->inst2 =inst2;
	update2_score_arrays(o1,o2,inst1,inst2,sol);
	sol->nothing_changed =false;
	//initialize
	t = NULL;
	o1= NULL;
	o2= NULL;
	c = NULL;

	return(sol);
}

////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                 /////
////		                        Third Neigborhood 			            					////
////                                                                                           /////
////////////////////////////////////////////////////////////////////////////////////////////////////

void update3_score_arrays(Operation* o1,OperatorInstance*inst1,OperatorInstance*inst2,Solution*sol)
{

	int colomn;
	if((inst1 != NULL)&&(inst2 != NULL)&&(o1 != NULL))
	{

		int j;
		for(j=0; j<o1->predecessors.size();j++)
		{
			CDFGedge*e = o1->predecessors[j];
			Data*d = (Data*)e->source;
			Reg*r= (Reg*)sol->regs()->getReg(0,d);
			colomn= r->index();
			for(short k=0; k < inst1->inputInstancePorts();k++)
			{
				sol->_score_array_port_var[inst1->getInputInstancePort(k)->index()][colomn]--;
				sol->_score_array_port_var[inst2->getInputInstancePort(k)->index()][colomn]++;
			}
		}

		for(j=0; j<o1->successors.size();j++)
		{
			CDFGedge*e = o1->predecessors[j];
			Data*d = (Data*)e->source;
			Reg*r = (Reg*)sol->regs()->getReg(0,d);
			colomn = r->index();
			sol->_score_array_oper_var[inst1->index()][colomn]--;
			sol->_score_array_oper_var[inst2->index()][colomn]++;
		}

	}

}

void complete_update3(Operation* o1,OperatorInstance*inst1,OperatorInstance*inst2,Solution*sol)
{

	//update the inst space in operations objects
	o1->inst(inst2);

	//update the oi_lists
	int n;
	n =sol->_oi_lists[inst1->index()].Operations.size();
	for(int i=0; i<n; i++)
	{
		if(sol->_oi_lists[inst1->index()].Operations[i]->name() == o1->name())
			sol->_oi_lists[inst1->index()].Operations.erase(sol->_oi_lists[inst1->index()].Operations.begin()+i);
	}
	sol->_oi_lists[inst2->index()].Operations.push_back(o1);

	//update the port_reading_the_var array
	short colomn=0;
	short j;
	for(j=0; j<o1->predecessors.size();j++)
	{
		colomn = o1->predecessors[j]->index();
		for(short k=0; k < inst1->inputInstancePorts();k++)
		{
			sol->_port_reading_the_var[inst1->getInputInstancePort(k)->index()][colomn]--;
			sol->_port_reading_the_var[inst2->getInputInstancePort(k)->index()][colomn]++;
		}
	}

	//update operator_writing_the_var array
	for(j=0; j<o1->successors.size();j++)
	{
		sol->_oper_writing_the_var[inst1->index()][o1->successors[j]->index()]--;
		sol->_oper_writing_the_var[inst2->index()][o1->successors[j]->index()]++;
	}

}

Solution*change_op_binding(Solution*sol,const Cadency*cadency, const Clock*clk)
{

	Operation* o1;
	OperatorInstance*inst2;
	bool compatible;

	if(sol->pos >=  sol->_operations.size())
	{
		sol->nothing_changed = true;
		sol->all_operations_cheked = true;
		return(sol);
	}
	o1 = sol->_operations[sol->pos];
	OperatorInstance*inst1= (OperatorInstance*)o1->inst();
	for(int i=0;i<sol->_oi_lists.size();i++)
	{
		if((o1->function() == sol->_oi_lists[i].Op->function())&&(sol->_oi_lists[i].Op != inst1))
		{
			compatible = true;
			for(int k=0; k <o1->cycles(); k++)
			{
				if(sol->_oi_lists[i].Op->_reservation_table[(o1->start()% cadency->length())/ clk->period()+ k ]!=0)
					compatible = false;
			}
			if(compatible)
			{
				inst2=sol->_oi_lists[i].Op;
				sol->o1 =o1;
				sol->inst1 =inst1;
				sol->inst2 =inst2;
				sol->nothing_changed =false;
				update3_score_arrays(o1,inst1,inst2,sol);
				break;
			}
		}
	}
	if(!compatible)
	{
		sol->nothing_changed =true;
	}
	sol->pos++;
	return(sol);
}


////////////////////////////////////////////////////////////////////////////////////////////////////                                                                                                                 /////
////		                        Main of the local search	            					////
////                                                                                           /////
////////////////////////////////////////////////////////////////////////////////////////////////////


void Local_search::initialize(CDFG*cdfg,OI_Lists*oi_lists)
{
	const Operation*o;	// the current operation
	const Data*d;	// the current data
	const CDFGnode*n;// the current node

	for(int i = 0; i < cdfg->nodes().size(); i++)
	{
		n = cdfg->nodes()[i];
		switch(n->type())
		{
		case CDFGnode::DATA:
			datas.push_back((Data*)n);
		case CDFGnode::CONTROL:
			break;
		case CDFGnode::OPERATION:
			o = (const Operation*)n;	// dynamic link here
			operations.push_back((Operation*)n);

			if(o->inst()!= NULL)
			{
				if(o->inst()->no()>= oi_lists->size())
				{
					oi_lists->resize(o->inst()->no()+1);
					oi_lists->at(o->inst()->no()).Op = (OperatorInstance*)o->inst();
				}
				if(oi_lists->at(o->inst()->no()).Op == NULL)
				{
					oi_lists->at(o->inst()->no()).Op = (OperatorInstance*)o->inst();
				}
				oi_lists->at(o->inst()->no()).Operations.push_back(o);
			}
		}
	}
	//delete the free case
	for(int j=0; j<oi_lists->size(); j++)
	{
		if(oi_lists->at(j).Op == NULL)
		{
			oi_lists->erase(oi_lists->begin()+j);
			j--;
		}
	}
}




void local_search1(Solution*sol_neighbour,Solution*sol_ref)
{

	RegOut::iterator reg_it;	//!< a register iterator
	Reg*reg;
	int count =0;

	do
	{
		sol_neighbour = change_var_binding(sol_neighbour); //! Change the binding of variables
		sol_neighbour->costing();
		if(sol_ref->total_cost()> sol_neighbour->total_cost())
		{
			if((sol_neighbour->reg_destination != NULL)&&(sol_neighbour->reg_source != NULL))
			{
				//rebind the data
				for(reg_it =sol_ref->regs()->begin(); reg_it !=sol_ref->regs()->end(); reg_it++)
				{
#if 0
					reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
					if(reg->no() == sol_neighbour->reg_source->no())
					{
						reg->_datas.erase(reg->_datas.begin()+ sol_neighbour->reg_source->pos);
					}
					if(reg->no() == sol_neighbour->reg_destination->no())
					{
						reg->_datas.push_back(sol_neighbour->d);
					}
				}

				for(reg_it =sol_neighbour->regs()->begin(); reg_it !=sol_neighbour->regs()->end(); reg_it++)
				{
#if 0
					reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
					if(reg->no() == sol_neighbour->reg_source->no())
					{
						reg->_datas.erase(reg->_datas.begin()+ sol_neighbour->reg_source->pos);
					}
					if(reg->no() == sol_neighbour->reg_destination->no())
					{
						reg->_datas.push_back(sol_neighbour->d);
					}
				}
				//upate the cost arrays
				update1(sol_ref,sol_neighbour->d->getindex(),sol_neighbour->reg_source->index(),sol_neighbour->reg_destination->index());

				sol_ref->costing();
				// initialize
				sol_neighbour->reg_destination = NULL;
				sol_neighbour->reg_source = NULL;
				for(reg_it =sol_neighbour->regs()->begin(); reg_it !=sol_neighbour->regs()->end(); reg_it++)
				{
#if 0
					reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
					reg->failed_as_destination= false;
				}
				sol_neighbour->nothing_changed = false;
				sol_neighbour->delete_reg =false;
			}
			else
			{
				cout<< " erreur "<<endl;
			}
		}
		else
		{
			if(sol_neighbour->nothing_changed == true)
			{
				sol_neighbour->reg_destination = NULL;
				sol_neighbour->reg_source = NULL;
				for(reg_it =sol_neighbour->regs()->begin(); reg_it !=sol_neighbour->regs()->end(); reg_it++)
				{
#if 0
					reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
					reg->failed_as_destination= false;
				}
				sol_neighbour->nothing_changed == false;
				sol_neighbour->delete_reg =false;
			}
			else
			{
				update1(sol_neighbour,sol_neighbour->d->getindex(),sol_neighbour->reg_destination->index(),sol_neighbour->reg_source->index());
				sol_neighbour->reg_destination->failed_as_destination = true;
				sol_neighbour->reg_destination = NULL;
				sol_neighbour->reg_source = NULL;
				sol_neighbour->nothing_changed = false;
				sol_neighbour->delete_reg =false;
			}
		}
		count++;

	}
	while(sol_neighbour->all_reg_cheked == false);
	sol_neighbour->costing();
	cout << "le cout aprés recherche local 1 est" <<sol_neighbour->total_cost()<<endl;

}


void local_search2(Solution*sol_neighbour,Solution*sol_ref,Cycles cycles)
{

	int count_2 =0;

	do
	{
		sol_neighbour = swap_op_binding(sol_neighbour,cycles); //! Change the binding of variables
		sol_neighbour->costing();

		if(sol_ref->total_cost()> sol_neighbour->total_cost())
		{

			//upate the cost arrays
			update2_score_arrays(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst1,sol_neighbour->inst2,sol_ref);
			//rebind the operations
			complete_update2(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst1,sol_neighbour->inst2,sol_ref);
			complete_update2(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst1,sol_neighbour->inst2,sol_neighbour);
			sol_ref->costing();
			// initialize
			sol_neighbour->o1 = NULL;
			sol_neighbour->o2 = NULL;
			sol_neighbour->inst1 = NULL;
			sol_neighbour->inst2 = NULL;
			sol_neighbour->nothing_changed = false;

		}
		else
		{
			if(sol_neighbour->nothing_changed == false)
				update2_score_arrays(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst2,sol_neighbour->inst1,sol_neighbour);

			// initialize
			sol_neighbour->o1 = NULL;
			sol_neighbour->o2 = NULL;
			sol_neighbour->inst1 = NULL;
			sol_neighbour->inst2 = NULL;
			sol_neighbour->nothing_changed = false;
		}
		count_2++;

	}
	while(sol_neighbour->all_cycles_cheked == false);

}


void local_search3(Solution*sol_neighbour,Solution*sol_ref,const Cadency*cadency, const Clock*clk)
{
	int count_3 =0;
	do
	{
		sol_neighbour = change_op_binding(sol_neighbour,cadency,clk); //! Change the binding of variables
		sol_neighbour->costing();

		if(sol_ref->total_cost()> sol_neighbour->total_cost())
		{
			cout << " j'ai gagné dans le 3 eme voisinge "<<endl;
			//upate the cost arrays
			update3_score_arrays(sol_neighbour->o1,sol_neighbour->inst1,sol_neighbour->inst2,sol_ref);
			//rebind the operations
			complete_update3(sol_neighbour->o1,sol_neighbour->inst1,sol_neighbour->inst2,sol_ref);
			complete_update3(sol_neighbour->o1,sol_neighbour->inst1,sol_neighbour->inst2,sol_neighbour);
			sol_ref->costing();
			// initialize
			sol_neighbour->o1 = NULL;
			sol_neighbour->inst1 = NULL;
			sol_neighbour->inst2 = NULL;
			sol_neighbour->nothing_changed = false;

		}
		else
		{
			if(sol_neighbour->nothing_changed == false)
			{
				update3_score_arrays(sol_neighbour->o1,sol_neighbour->inst2,sol_neighbour->inst1,sol_neighbour);
				cout << " j'ai fait un changement dans le 3 eme voisinge mais j 'ai pas gagné"<<endl;
			}
			// initialize
			sol_neighbour->o1 = NULL;
			sol_neighbour->inst1 = NULL;
			sol_neighbour->inst2 = NULL;
			sol_neighbour->nothing_changed = false;
		}
		count_3++;
	}
	while(sol_neighbour->all_operations_cheked == false);

}


void shake1(Solution*sol_neighbour,Solution*sol_ref)
{

	RegOut::iterator reg_it;	//!< a register iterator
	Reg*reg;
	for(int i= 0; i< 3; i++)
	{
		sol_neighbour = change_var_binding(sol_neighbour); //! Change the binding of variables
		sol_neighbour->costing();
		if((sol_neighbour->reg_destination != NULL)&&(sol_neighbour->reg_source != NULL))
		{
			//rebind the data
			for(reg_it =sol_ref->regs()->begin(); reg_it !=sol_ref->regs()->end(); reg_it++)
			{
#if 0
				reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
				if(reg->no() == sol_neighbour->reg_source->no())
				{
					reg->_datas.erase(reg->_datas.begin()+ sol_neighbour->reg_source->pos);
				}
				if(reg->no() == sol_neighbour->reg_destination->no())
				{
					reg->_datas.push_back(sol_neighbour->d);
				}
			}
			for(reg_it =sol_neighbour->regs()->begin(); reg_it !=sol_neighbour->regs()->end(); reg_it++)
			{
#if 0
				reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
				if(reg->no() == sol_neighbour->reg_source->no())
				{
					reg->_datas.erase(reg->_datas.begin()+ sol_neighbour->reg_source->pos);
				}
				if(reg->no() == sol_neighbour->reg_destination->no())
				{
					reg->_datas.push_back(sol_neighbour->d);
				}
			}
			//upate the cost arrays
			update1(sol_ref,sol_neighbour->d->getindex(),sol_neighbour->reg_source->index(),sol_neighbour->reg_destination->index());
			sol_ref->costing();
			// initialize
			sol_neighbour->reg_destination = NULL;
			sol_neighbour->reg_source = NULL;
			for(reg_it =sol_neighbour->regs()->begin(); reg_it !=sol_neighbour->regs()->end(); reg_it++)
			{
#if 0
				reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
				reg->failed_as_destination= false;
			}
			sol_neighbour->nothing_changed = false;
			sol_neighbour->delete_reg =false;
		}
		else
		{
			cout<< " erreur "<<endl;
		}
	}
}

void shake2(Solution*sol_neighbour,Solution*sol_ref,Cycles cycles)
{

	for(int i= 0; i< 3; i++)
	{

		sol_neighbour = swap_op_binding(sol_neighbour,cycles); //! Change the binding of variables
		sol_neighbour->costing();

		if(sol_neighbour->nothing_changed == false)
		{
			update2_score_arrays(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst1,sol_neighbour->inst2,sol_ref);
			//rebind the operations
			complete_update2(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst1,sol_neighbour->inst2,sol_ref);
			complete_update2(sol_neighbour->o1,sol_neighbour->o2,sol_neighbour->inst1,sol_neighbour->inst2,sol_neighbour);
			sol_ref->costing();
			// initialize
			sol_neighbour->o1 = NULL;
			sol_neighbour->o2 = NULL;
			sol_neighbour->inst1 = NULL;
			sol_neighbour->inst2 = NULL;
			sol_neighbour->nothing_changed = false;
		}

	}
}


Local_search :: Local_search(const Switches*sw,
							 const Cadency*cadency,		// in
							 const Clock*clk,			// in
							 const SchedulingOut*sched, // in
							 RegOut*regs,
							 CDFG*cdfg)
{

	if(_debug)cout << "Local search ..." << endl;

	_sw = sw;
	_cadency = cadency;
	_clk = clk;
	_sched = sched;
	_cdfg = cdfg;
	_regs = regs;
	RegOut::iterator reg_it;	//!< a register iterator
	Reg*reg;
	const RegUse*use;
	long position;
	int pos=0;
	int row;
	int colomn;
	const Operation*operation;
	const OperatorInstance*inst;
	const InstancePort*port;
	bool erased =false;
	int i;

	// full datas and operations arrays and sort Operations By Instance
	initialize(_cdfg, &_oi_lists);

	// full the index's fields
	for(i=0; i< datas.size();i++)
	{
		datas[i]->index(i);
	}
	for(i=0; i< operations.size();i++)
	{
		operations[i]->index(i);
	}
	for(i=0,reg_it = _regs->begin(); reg_it != _regs->end(); reg_it++,i++)
	{
#if 0
		reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
		reg->index(i);
	}
	for(i=0; i< _oi_lists.size();i++)
	{
		if(_oi_lists[i].Op == NULL)
		{
			cout << "oi_liste vide"<<endl;
		}
		else
		{
			_oi_lists[i].Op->index(i);
		}
	}
	int v=0;
	//full dico_ports
	for(i=0; i< _oi_lists.size();i++)
	{
		for(int i_port=0; i_port< _oi_lists[i].Op->inputInstancePorts();i_port++)
		{
			struct Dico_port p;
			p.oper_index=_oi_lists[i].Op->index();
			p.name = _oi_lists[i].Op->getInputInstancePort(i_port)->name();
			dico_ports.push_back(p);
			_oi_lists[i].Op->getInputInstancePort(i_port)->index(v);
			v++;
		}
	}


	// full the field _connected register of the port instances
	for(reg_it = _regs->begin(); reg_it != _regs->end(); reg_it++)
	{
#if 0
		reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
		for(i = 0; i < reg->use_nb(); i++)
		{
			use = reg->getUse(i);
			position = use->_pos;
			operation = use->_oper;
			inst = operation->inst();
			if(!inst)continue;
			port = inst->getInputInstancePort(position);
			((InstancePort*)port)->addConnectedReg(reg);
		}
	}


	//Memory allocation
	datas_compatibility = (short**)malloc(sizeof(short*)*datas.size());
	if(datas_compatibility == NULL)
	{
		printf("failure of dynamic memory allocation of datas_compatibility");
		exit(1);
	}
	for(i = 0; i < datas.size(); i++)
	{
		datas_compatibility[i] = (short*)malloc(sizeof(short)*datas.size());
		if(datas_compatibility[i] == NULL)
		{
			printf("failure of dynamic memory allocation of datas_compatibility[i]");
			exit(1);
		}
	}


	port_reading_the_var= (short**)malloc(sizeof(short*)*dico_ports.size());
	if(port_reading_the_var == NULL)
	{
		printf("failure of dynamic memory allocation of port_reading_the_var");
		exit(1);
	}
	for(i = 0; i < dico_ports.size(); i++)
	{
		port_reading_the_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(port_reading_the_var[i] == NULL)
		{
			printf("failure of dynamic memory allocation of port_reading_the_var[i]");
			exit(1);
		}
	}


	oper_writing_the_var= (short**)malloc(sizeof(short*)*_oi_lists.size());
	if(oper_writing_the_var == NULL)
	{
		printf("failure of dynamic memory allocation of oper_writing_the_var");
		exit(1);
	}
	for(i = 0; i < _oi_lists.size(); i++)
	{
		oper_writing_the_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(oper_writing_the_var[i] == NULL)
		{
			printf("failure of dynamic memory allocation of oper_writing_the_var[i]");
			exit(1);
		}
	}

	score_array_port_var= (short**)malloc(sizeof(short*)*dico_ports.size());
	if(score_array_port_var == NULL)
	{
		printf("failure of dynamic memory allocation of score_array_port_var");
		exit(1);
	}
	for(i = 0; i < dico_ports.size(); i++)
	{
		score_array_port_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(score_array_port_var [i]== NULL)
		{
			printf("failure of dynamic memory allocation of score_array_port_var[i]");
			exit(1);
		}
	}

	score_array_oper_var= (short**)malloc(sizeof(short*)*_oi_lists.size());
	if(score_array_oper_var == NULL)
	{
		printf("failure of dynamic memory allocation of score_array_oper_var");
		exit(1);
	}
	for(i = 0; i < _oi_lists.size(); i++)
	{
		score_array_oper_var[i] = (short*)malloc(sizeof(short)*datas.size());
		if(score_array_oper_var[i] == NULL)
		{
			printf("failure of dynamic memory allocation of score_array_oper_var[i]");
			exit(1);
		}
	}

	//full the datas_compatibility array;
	for(int j=0; j< datas.size();j++)
	{
		for(int k=0; k< datas.size();k++)
		{
			if(testCompatibility(datas[j],datas[k],cadency,clk))
			{
				datas_compatibility[j][k] =1;
				datas_compatibility[k][j] =1;
			}
			else
			{
				datas_compatibility[j][k] =0;
				datas_compatibility[k][j] =0;
			}
		}
	}


	//full the cycles array
	for(i=0; i< cadency->length()/clk->period();i++)
	{
		Cycle*c = new Cycle();
		c->id=i;
		c->checked =false;
		cycles.push_back(c);
	}
	for(i=0; i< operations.size();i++)
	{
		Operation*oo =operations[i];
		int start = operations[i]->start();
		int cycle_id = (operations[i]->start()%cadency->length())/clk->period();
		if(cycle_contain_function(operations[i]->function(),cycles[cycle_id]) ==false)
		{
			Type*t = new Type();
			t->function = operations[i]->function();
			t->operations.push_back(operations[i]);
			t->checked = true;
			t->pos_source =0;
			t->pos_destination =0;
			cycles [cycle_id]->types.push_back(t);

		}
		else
		{
			for(int j=0; j< cycles [cycle_id]->types.size(); j++)
			{
				Type*tt = cycles [cycle_id]->types[j];
				if(operations[i]->function() == cycles [cycle_id]->types[j]->function)
				{
					cycles[cycle_id]->types[j]->operations.push_back(operations[i]);
					cycles[cycle_id]->types[j]->checked = false;
					break;
				}
			}
		}
	}
	// we can not do permutations wih a single operation
	for(i=0; i< cycles.size();i++)
	{
		for(int j=0; j< cycles[i]->types.size();j++)
		{
			if(cycles[i]->types[j]->operations.size() == 1)cycles[i]->types[j]->checked == true;
			if(cycles[i]->types[j]->function->name() == "assign")cycles[i]->types[j]->checked == true;
		}
	}

	//full operator_writing_the_var(rows=operator; colomns=datas)
	for(i=0; i< _oi_lists.size();i++)
		for(int j=0; j< datas.size();j++)
			oper_writing_the_var[i][j]=0;

	for(i=0; i< _oi_lists.size();i++)
	{
		if(_oi_lists.at(i).Op != NULL)
			for(int j=0; j< _oi_lists[i].Operations.size();j++)
			{
				operation = _oi_lists[i].Operations[j];
				for(int k=0; k< operation->successors.size();k++)
				{
					CDFGedge*e = operation->successors[k];
					Data*d = (Data*)e->target;
					row = _oi_lists[i].Op->index();
					colomn =d->getindex();
					oper_writing_the_var[row][colomn]=1;
				}
			}
	}

	//port_reading_the_var(rows=ports; colomns=datas)
	for(i=0; i< dico_ports.size();i++)
		for(int j=0; j< datas.size();j++)
			port_reading_the_var[i][j]=0;

	for(i=0; i< _oi_lists.size();i++)
	{
		for(int i_port=0; i_port< _oi_lists[i].Op->inputInstancePorts();i_port++)
		{
			InstancePort*Port= _oi_lists[i].Op->getInputInstancePort(i_port);
			row = Port->index();
			for(int j=0;j< Port->getConnectedRegs().size(); j++)
			{
				Reg*reg= Port->getConnectedRegs()[j];
				Data*d=reg->_datas[0];
				colomn = d->getindex();
				port_reading_the_var[row][colomn]=1;
			}
		}
	}


	//full score_array_oper_var(rows=operator; colomns=datas)

	for(i=0; i< _oi_lists.size();i++)
		for(int j=0; j< _regs->size();j++)
			score_array_oper_var[i][j]=0;

	for(i=0; i< _oi_lists.size();i++)
	{
		if(_oi_lists.at(i).Op != NULL)
			for(int j=0; j< _oi_lists[i].Operations.size();j++)
			{
				operation = _oi_lists[i].Operations[j];
				for(int k=0; k< operation->successors.size();k++)
				{
					CDFGedge*e = operation->successors[k];
					Data*d = (Data*)e->target;
					row = _oi_lists[i].Op->index();
					Reg*reg = (Reg*)_regs->getReg(0,d);
					colomn = reg->index();
					score_array_oper_var[row][colomn]=1;
				}
			}
	}

	/*for(i=0; i< _oi_lists.size();i++) {
	  cout <<endl;
	  for(int j=0; j< _regs->size();j++) {
	  cout<< score_array_oper_var[i][j]<<" / ";
	  }
	  }
	  cout <<endl;
	  cout <<endl;*/



	//full score_array_port_var(rows=ports; colomns=datas)
	for(i=0; i< dico_ports.size();i++)
		for(int j=0; j< _regs->size();j++)
			score_array_port_var[i][j]=0;

	for(i=0; i< _oi_lists.size();i++)
	{
		for(int i_port=0; i_port< _oi_lists[i].Op->inputInstancePorts();i_port++)
		{
			InstancePort*Port= _oi_lists[i].Op->getInputInstancePort(i_port);
			row = Port->index();
			for(int j=0;j< Port->getConnectedRegs().size(); j++)
			{
				Reg*reg= Port->getConnectedRegs()[j];
				colomn = reg->index();
				score_array_port_var[row][colomn]=1;
			}
		}
	}


	/*for(i=0; i< dico_ports.size();i++) {
	  cout <<endl;
	  for(int j=0; j< _regs->size();j++) {
	  cout<< score_array_port_var[i][j]<<" / ";
	  }
	  }
	  cout <<endl;*/

	//! The original solution's GAUT

	initial_solution(new Solution(_sw,_cadency,_clk,_regs,datas,operations,_oi_lists,dico_ports,port_reading_the_var,
								  oper_writing_the_var,score_array_port_var,score_array_oper_var,datas_compatibility,ops_compatibility)); //! The original solution's GAUT
	initial_solution()->costing();

	sol_ref(new Solution(initial_solution()->sw(), initial_solution()->cadency(), initial_solution()->clk(),
						 initial_solution()->regs(),datas,operations, initial_solution()->_oi_lists,
						 initial_solution()->_dico_ports,initial_solution()->_port_reading_the_var, initial_solution()->_oper_writing_the_var,
						 initial_solution()->_score_array_port_var, initial_solution()->_score_array_oper_var,datas_compatibility,ops_compatibility));
	sol_ref()->costing();
	cout << "total cost before optimisation: " << sol_ref()->total_cost()<<endl;
	cout << "  registers cost before optimisation: " << sol_ref()->registers_cost()<<endl;
	cout << " operators cost before optimisation: " << sol_ref()->operators_cost()<<endl;
	cout << " multiplexers cost before optimisation: " << sol_ref()->interconections_cost()<<endl;

	int count =0;

	sol_neighbour(new Solution(sol_ref()->sw(), sol_ref()->cadency(), sol_ref()->clk(),
							   sol_ref()->regs(),datas,operations, sol_ref()->_oi_lists,
							   sol_ref()->_dico_ports,sol_ref()->_port_reading_the_var, sol_ref()->_oper_writing_the_var,
							   sol_ref()->_score_array_port_var, sol_ref()->_score_array_oper_var,datas_compatibility,ops_compatibility));
	// initialize
	sol_neighbour()->o1 = NULL;
	sol_neighbour()->o2 = NULL;
	sol_neighbour()->inst1 = NULL;
	sol_neighbour()->inst2 = NULL;
	sol_neighbour()->nothing_changed = false;
	sol_neighbour()->reg_destination = NULL;
	sol_neighbour()->reg_source = NULL;
	for(reg_it =sol_neighbour()->regs()->begin(); reg_it !=sol_neighbour()->regs()->end(); reg_it++)
	{
#if 0
		reg = (*reg_it).second;
#else
					reg = (*reg_it);
#endif
		reg->failed_as_destination= false;
	}
	sol_neighbour()->nothing_changed = false;
	sol_neighbour()->delete_reg =false;
	sol_neighbour()->all_cycles_cheked = false;
	sol_neighbour()->all_operations_cheked = false;
	int neighbourhood = 1;


	if(sw->vns())
	{
		do
		{
			//!First neighbourhood
			if(neighbourhood == 1)
			{
				//shake1(sol_neighbour(),sol_ref());
				local_search1(sol_neighbour(),sol_ref());
				shake1(sol_neighbour(),sol_ref());
				cout << "total cost after n1:" << sol_ref()->total_cost()<<endl;

			}

			//!Second neighbourhood
			if(neighbourhood == 2)
			{
				shake2(sol_neighbour(),sol_ref(),cycles);
				local_search2(sol_neighbour(),sol_ref(), cycles);
				cout << "total cost after n2:" << sol_ref()->total_cost()<<endl;

			}

			//!Third neighbourhood
			if(neighbourhood == 3)
			{
				local_search3(sol_neighbour(),sol_ref(),_cadency,_clk);
				cout << "total cost after n3:" << sol_ref()->total_cost()<<endl;


			}

			if(initial_solution()->total_cost()> sol_ref()->total_cost())
			{
				//free initial_solution(
				initial_solution(new Solution(sol_ref()->sw(), sol_ref()->cadency(), sol_ref()->clk(),
											  sol_ref()->regs(),datas,operations, sol_ref()->_oi_lists,
											  sol_ref()->_dico_ports,sol_ref()->_port_reading_the_var, sol_ref()->_oper_writing_the_var,
											  sol_ref()->_score_array_port_var, sol_ref()->_score_array_oper_var,datas_compatibility,ops_compatibility));
			}
			else
				neighbourhood++;


		}
		while(neighbourhood < 4);

		cout << "total cost after optimisation(vns):" << sol_ref()->total_cost()<<endl;

	}

	if(sw->n1())
	{
		local_search1(sol_neighbour(),sol_ref());
		cout << "cost after optimisation(register neighbourhood):" << sol_ref()->total_cost()<<endl;
	}
	if(sw->n3())
	{
		local_search3(sol_neighbour(),sol_ref(),_cadency,_clk);
		cout << "cost after optimisation(change operators binding):" << sol_ref()->total_cost()<<endl;
	}

}

#endif
