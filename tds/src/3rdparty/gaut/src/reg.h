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

//	File:		reg.h
//	Purpose:	Registers synthesis.
//	Author:		Pierre Bomel, LESTER, UBS

#ifndef __REG_H__
#define __REG_H__

#include <fstream>
#include <iostream>
#include <algorithm>
#include "cdfg.h"
#include "operation.h"
#include "munkres.h"
#include "map.h"
//#include "scheduling.h"

using namespace std;

class SchedulingOut;

//! Registers Use(Emmanuel Juin 09/2007)
class InputOperatorRegUse
{
public:
	const Reg*			_reg;		//!< reference on register
	const long			_inputpos;		//!< operator input position
	string 			_name;		//!< name of the input operator use reg regname+"@"+operatorinstancename()+"@"input_pos;
	//! Create a new use of register
	InputOperatorRegUse(const Reg*reg,const long pos,const string name): _reg(reg), _inputpos(pos),_name(name) {
	}

	string name()const
	{
		return _name;
	}
};

//! A smart pointer to a CDFG node.
class InputOperatorRegUseRef  : public MapRef <InputOperatorRegUse>
{
public:
	InputOperatorRegUseRef(InputOperatorRegUse*d): MapRef<InputOperatorRegUse>(d) {}
};


//! List of smart pointers to Patterns.
class InputOperatorRegUseRefs : public MapRefs<InputOperatorRegUse, InputOperatorRegUseRef>
{
};



//! Registers Use(Emmanuel Juin 09/2007)
class RegUse
{

private:

public:
	const Data*			_data;		//!< Name of variable store in register
	const Operation*	_oper;		//!< Operation
	const long			_pos;		//!< operator input/output position
	long				_load_no_modulo_cadency;		//!< load time of this use
	const bool			_input;		//!< is input or output from oper
	//! Create a new use of register
	RegUse(const Data*data,  const Operation*oper,  const long pos,const bool input): _data(data),_oper(oper), _pos(pos),  _input(input) {
		_load_no_modulo_cadency = _data->get_start_no_modulo_cadency();
	}

	void info()const
	{
		cout << " Reg Use : " <<  _data->name()<< "[" << _data->get_start_modulo_cadency()<< ":" << _data->get_end_modulo_cadency()<< "]" << endl;
		if(_input)
			cout << " Reg Use : input of oper = " << _oper->name()<< endl;
		else
			cout << " Reg Use : output of oper = " << _oper->name()<< endl;
		cout << " Reg Use : load_no_modulo_cadency = " << _load_no_modulo_cadency << endl;
		cout << " Reg Use : pos = " << _pos << endl;
		cout<<endl;
	}
};

//! A smart pointer to a CDFG node.
class RegUseRef  : public MapRef <RegUse>
{
public:
	RegUseRef(RegUse*d): MapRef<RegUse>(d) {}
};


//! List of smart pointers to Patterns.
class RegUseRefs : public MapRefs<RegUse, RegUseRef>
{
};

//! Registers
class Reg
{
private:
	string	_name;	//!< Register unique name = its ID.
	long	_no;	//!< unique number
	static  long	_count;	//!< internal count of allocated registers.
	long _maxStage;			//!< register stage(register = 0, fifo > 0)
	int _bitwidth;			//!< register width(max width of allocated data)
	//DH: UNUSED long _first;			//!< first use of register(for gantt)
	//DH: UNUSED long _last;				//!< last use of register(for gantt)
	/* KT 05/05/2008*/
	int _index;
	/* End KT*/

	bool _false_register_hardwire_constant; //!< reg is a false register(hardwaire constant)

public:

	//int index;
	/* KT 15/12/2007*/
	long pos;		//!< the index of datas we have to check on the next iteration
	vector<Data*>	_datas;	//!< A list of pointers to the bound Datas.
	bool contain_const;
	bool checked;
	bool failed_as_destination;
	/* End KT*/
	/*  End KT*/
	long _load_index;		//!< load index(EJ 09/2007)
	vector<RegUse*> _uses;  //!< uses of register(EJ 09/2007)

	void*_addr;			//!< Generic pointer to extend easily.

	Reg()
	{
		ostringstream myString;

		_no=_count++;
		myString << "reg_" << _no;
		_name=myString.str();
		_maxStage=0;
		//DH: UNUSED_last=_first=0;
		_uses.clear();
		_datas.clear();
		checked=false;
		contain_const=false;
		failed_as_destination=false;
		pos=0;
		_maxStage=0;
		_load_index=0;
		_addr= (void*)0;
		_bitwidth =0;
		_false_register_hardwire_constant=false;
	}

	Reg(const Reg*reg):_name(reg->_name),_no(reg->_no)
	{
		_maxStage=reg->_maxStage;
		//_last=reg->_last;
		//_first=reg->_first;
		_uses=reg->_uses;
		_load_index= reg->_load_index;
		_addr=reg->_addr;
		_datas= reg->_datas;
		checked=reg->checked;
		contain_const =reg->contain_const;
		pos =reg->pos;
		_index = reg->_index;
		failed_as_destination = false;
		_bitwidth =0;
		_false_register_hardwire_constant=reg->_false_register_hardwire_constant;
	}

	~Reg()
	{
		_uses.clear();
		_datas.clear();
		_name="destroyed";
	}


	//! get false_register_hardwire_constant flag
	bool false_register_hardwire_constant()const
	{
		return _false_register_hardwire_constant;
	}
	//! set false_register_hardwire_constant flag
	void false_register_hardwire_constant(bool false_register_hardwire_constant)
	{
		_false_register_hardwire_constant = false_register_hardwire_constant;
	}
	//! get width
	int bitwidth()const
	{
		return _bitwidth;
	}
	//! set width
	void bitwidth(int width)
	{
		_bitwidth = width;
	}


	/*KT 05/05/2008*/
	//! get index
	int index()const
	{
		return _index;
	}
	//! set index
	void index(int index)
	{
		_index = index;
	}

	//! Get ID.
	string name()const
	{
		return _name;
	}

	//! Get no.
	long no()const
	{
		return _no;
	}
	//! Set maxStage
	void fifoStage(long maxStage)
	{
		if(maxStage>_maxStage)_maxStage = maxStage;
	}
	//! Get maxStage
	long fifoStage()const
	{
		if(_false_register_hardwire_constant==true)
			return 0;
		else
			return _maxStage;
	}
	/*DH: UNUSED //! Set first
	  void first(long first)
	  {
	  _first = first;
	  }
	//! Get first
	long first()const
	{
	return _first;
	}
	//! Set last
	void last(long last)
	{
	if(_last < last)_last = last;
	}
	//! Get last
	long last()const
	{
	return _last;
	}*/
	//! Add use of register(EJ 09/2007)
	void addUse(const Data*data, const Operation*oper /*, long cadency, long stage*/, long pos,bool isinput,const CDFG*cdfg);
	//! Get use of register(EJ 09/2007)
	const RegUse* getUse(long i)const;
	//! Get number of use of register(EJ 09/2007)
	long use_nb()const;
	//! register load a new value at time ?(EJ 09/2007)
	bool is_load_at(long cycle_time,long cadency)const;
	//! Return true if register is free in an intervalle(EJ 09/2007)
	bool isRegFree(long start, long stop)const;
	/* tell if reg is connected to an input of operator instance
	   bool isConnectedToInputOperatorInstance(constis OperatorInstance*inst,long input_pos)const;*/

	//! tell if reg is connected to an output of operator instance
	long isConnectedToOutputOperatorInstance(const OperatorInstance*inst,long output_pos)const;

	//! count number of time reg is connected to an output of an different operator instance except operator inst
	void numberOfInputMuxFromOutputOperatorInstanceExceptThisOperator(const OperatorInstance*inst,long nb_operators,long*nb_mux,long*mux_width)const;

	void numberOfInputMuxFromOutputOperatorInstance(long max_operator_number,long*nb_mux,long*mux_width)const;

	//! Return true if register is use by an assign
	bool is_use_by_assign()const;

	void signal_vhdl_declaration(ofstream &f, const string &tabul, const Switches*_sw)const;

	void variable_vhdl_declaration(ofstream &f, const string &tabul, const Switches*_sw)const;

	void component_sigs_vhdl_declaration(ofstream &f, const string &tabul, const Switches*_sw,long _bits)const;

	void instantiate_component_vhdl(ofstream &f, const string &tabul, const Switches*_sw,long _bits)const;

	/* Get active stage of register from tuple(oper,data,input)no used see ic.cpp
	   long get_active_stage_for_operation(const Operation*oper,const Data*data,const bool isinput)const
	   {
	   bool found= false;
	   long result=0;
	   for(int i=0; i<_uses.size()&&(found==false); i++)
	   {
	   RegUse*use = _uses[i];
	   if((use->_oper==oper)&&(use->_data==data)&&(use->_input==isinput))
	   {
	   result=use->_stage;
	   found=true;
	   }
	   }
	   return result;
	   }*/

	//! compute reg bitwidth : defined as max width of succesors or predecessors port bitwidth.
	void reg_width();

	//
	const Data*data(long cycle_time)const
	{
		bool found= false;
		const Data*result=_datas[0];
		for(int i=0; i<_uses.size()&&(found==false); i++)
		{
			RegUse*use = _uses[i];
			if((cycle_time >= use->_data->get_start_modulo_cadency())&&(cycle_time < use->_data->get_end_modulo_cadency()))
			{
				result=use->_data;
				found=true;
			}
		}
		return result;
	}

	static bool is_chained(const Data*data);

	void info()const
	{
		int i;
		for(i=0;i<_uses.size(); i++)
		{
			((const RegUse*)_uses[i])->info();
		}
	}
};


struct UnusedRegister
{
	inline bool operator()(const Reg* reg)const
	{
		bool result=reg->_uses.empty();
		return result;
	}
};


//! Register synthesis output. This is a array of registers.
class RegOut
{

private:

	vector<Reg*> _reg_list;

public:
	typedef vector<Reg*>::const_iterator const_iterator;
	typedef vector<Reg*>::iterator iterator;

	const_iterator begin() const { return _reg_list.begin(); }
	const_iterator end() const { return _reg_list.end(); }
	iterator begin() { return _reg_list.begin(); }
	iterator end() { return _reg_list.end(); }

	RegOut()
	{
	}

	~RegOut()
	{
		_reg_list.clear();
	}

	void init(int nb_register)
	{
		_reg_list.resize(nb_register);
		_reg_list.clear();
		for(int i=0;i<nb_register;i++)
			_reg_list.push_back(new Reg());
	}


	void add(Reg* newReg) {
		_reg_list.push_back(newReg);
	}
	void add(int nb_register_added)
	{
		//_reg_list.resize(_reg_list.size()+nb_register_added);
		for(int i=0;i<nb_register_added;i++)
			_reg_list.push_back(new Reg());
	}

	int size()const
	{
		return _reg_list.size();
	}

	void clear()
	{
		for(int i=0;i<_reg_list.size();i++)
			_reg_list[i]=0;
	}

	Reg*getReg(int reg_index)const
	{
		return _reg_list[reg_index];
	}

	//! Print info
	void info()const
	{
		for(int i=0;i<_reg_list.size();i++)
		{
			Reg*reg = _reg_list[i];
			cout << "Reg " << reg->name()<< endl;
			reg->info();
		}
	}


	//! Print info of mux to input register
	long nbMux2to1(long max_operator_number)const
	{

		long nb_mux_2to1=0;

		for(int i=0;i<_reg_list.size();i++)
		{
			long nb_mux=0;
			long mux_width=0;
			Reg*reg = _reg_list[i];
			if((reg->data(0)->type() ==Data::CONSTANT)&&(reg->data(0)->hardwire() == true))
				continue;//skip hardwire register
			reg->numberOfInputMuxFromOutputOperatorInstance(max_operator_number,&nb_mux,&mux_width);
			nb_mux_2to1+=nb_mux*mux_width;
		}
		return nb_mux_2to1;
	}

	//! Print number if Flip Flop
	long nbFlipFlop()const
	{
		long nb_FlipFlop=0;
		for(int i=0;i<_reg_list.size();i++)
		{
			Reg*reg = _reg_list[i];
			if((reg->data(0)->type() ==Data::CONSTANT)&&(reg->data(0)->hardwire() == true))
				continue;
			if(reg->fifoStage())
				nb_FlipFlop+= (reg->fifoStage()+1)*reg->bitwidth();
			else
				nb_FlipFlop+=reg->bitwidth();
		}
		return nb_FlipFlop;
	}



	void eliminateUnusedRegister()
	{
		vector<Reg*>::iterator new_end=std::remove_if(_reg_list.begin(), _reg_list.end(), UnusedRegister());
		vector<Reg*>::iterator reg_it;
		for(reg_it=new_end;reg_it!=_reg_list.end();reg_it++)
		{
			delete(*reg_it);
		}
		_reg_list.erase(new_end,_reg_list.end());
	}


};



/* DH : 9/03/2009*/
struct CompareDataStart
{
	inline bool operator()(const Data* a1, const Data* a2)const
	{
		return a1->get_start_modulo_cadency()< a2->get_start_modulo_cadency();
	}
};


//! Register synthesis.
//! As a first version, the register allocation process is very simple.
//! We allocate one register/= per variable without taking care about the lifetimes.
//! This is absolutely a waste of silicon space. But it will lead shortly to
//! a first version of GAUT. Regs optimization can occur after, when we have
//! enough RTL code synthesized to make testing about it and check
//! expected results(speed, area, power, ...)with commercial tools(ISE, Quartus, DC, ...).
class RegSynthesis
{
public:

	RegOut*_reg_out;
	const Switches*_sw;
	const CDFG*_cdfg;
	const SchedulingOut*_sched;
	long _cadency;
	long _period;
	long _mem_access;

	//! Create a new register synthesis process.
	RegSynthesis(const Switches*sw, const CDFG*cdfg, const SchedulingOut*sched,RegOut*reg_out, long cadency, long period,long mem_access)
	{
		// copy local parameter
		_sw=sw;
		_reg_out = reg_out;
		_cdfg=cdfg;
		_sched = sched;
		_cadency=cadency;
		_period=period;
		_mem_access=mem_access;
	}

	//void RegisterAllocation_GLOBAL_MWBM(long bits);

	void RegisterAllocation_CLUSTER_MWBM(long bits /*, bool fine_allocation*/);

	void RegisterAllocation_NONE(long bits);

	void RegisterAllocation_LeftEdge(long bits);

	void RegisterAllocation_MLEA(long bits);

	double computeWeight(const Data*data,const Reg*reg,long max_operator_no);

	void compute_data_life(vector <const Data*>*hardwired_constant_reg,vector <const Data*>*fifo_data_reg,vector <const Data*>*sorted_interval_lifetime);

	int MinimumRegisterNumber(vector <const Data*> &sorted_interval_lifetime);

};

#endif // __REG_H__


