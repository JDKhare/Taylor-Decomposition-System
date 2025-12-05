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

//	File:		scheduling.h
//	Purpose:	Scheduling process
//	Author:		Pierre Bomel, LESTER, UBS

#ifndef __SCHEDULING_H__
#define __SCHEDULING_H__

class OperatorInstance;
class OperationSchedulingState;
class Mobility;
/* GL 21/06/07 : add class*/
class Priority;
/* Fin GL*/
class OperatorSchedulingState;
class FunctionSchedulingState;
class SchedulingStage;
class SchedulingOut;
class Scheduling;
class RegUseRefs;
class RegUse;
class InputOutputOperatorRegRef;
class InputOperatorRegUse;
//class InputOperatorRegUseRefs;

#include <iostream>
#include <list>
#include <queue>
#include <deque>
#include <functional>
using namespace std;
#include "operator.h"
#include "operation.h"
#include "allocation.h"
#include "cdfg.h"
#include "mapping_constraints.h"
#include "check.h"
#include "map.h"
#include "reg.h"
/* GL 02/10/07 : connections with previous instances*/
#include "connection.h"
#include "multimode_tools.h"
/* Fin GL*/

//! Emmanuel Juin : 09/2007
//! class Instance Port
//! specific parameters for instance port with an
//! operator which can to be the same with the same port
class InstancePort
{
private:
	const string			_name;				//! port name
	int _index;
	//vector<const Reg*>		_connected_reg;		//! register list connected to this port
	vector<Reg*>		_connected_reg;		//! register list connected to this port

public:
	InstancePort(string name): _name(name) {}

	//! add a connection with a register
	void addConnectedReg(Reg*r)
	{
		for(int i=0; i<_connected_reg.size(); i++)
		{
			Reg*reg = _connected_reg[i];
			if(reg == r)
			{
				return;
			}
		}
		_connected_reg.push_back(r);
	}
	//! get name
	string name()const
	{
		return _name;
	}
	//! get connected register list
	vector<Reg*> getConnectedRegs()
	{
		return _connected_reg;
	}
	/* KT 05/05/2008*/
	//! get index
	int index()
	{
		return _index;
	}
	//! set index
	void index(int index)
	{
		_index = index;
	}
	/*  End KT*/
};

//! This is the scheduling state of an operator(GAUT's FifoR).
class OperatorInstance
{
public:
	vector<int> _reservation_table; //!< KT 06/06/2007
	long cpt_use;//!< KT 12/07/2007
	int cpt_max_use;//!< KT 12/07/2007
	void*_addr;
	//DH: 4/06/2009 to accelerate computeWeight for mux optimization

private:
	static long count;
	enum OperatorInstanceState {READY, RUNNING};	//!< Enum type.
	OperatorInstanceState	_state;					//!< Instance's state.
	/* GL 26/04/2007 :	Replace fllowing line
	   to attribute a new operator after resizing
	   const Operator*_oper;					//!< Instance's pointer to operator operator type.
	   By*/
	Operator*_oper;						//!< Instance's pointer to operator operator type.
	/* Fin GL*/
	const Function*_func;					//!< Pointer to function bound to the operator.
	const long				_id;					//!< Unique instance number.
	/* GL 14/05/07 : Add Input and Output bitwidths and signeds*/
	vector<long>			_inputsBitwidth;		//!< operator input widths
	vector<bool>			_inputsSigned;			//!< operator input signeds
	vector<long>			_outputsBitwidth;		//!< operator output widths
	vector<bool>			_outputsSigned;			//!< operator output signeds
	/* Fin GL*/

	InputOperatorRegUseRefs _inputUseRegs;

	/* GL 02/10/07 : connections with previous instances*/
	ConnectionRefs*_connections;			//!< connections with previous instances
	/* Fin GL*/

	/* Caaliph 27/06/2007*/
	long				_id2;
	/* End Caaliph*/
	/* KT 05/05/2008*/
	int _index;
	/* End KT*/



public:
	//! Delete operator instance.
	~OperatorInstance()
	{
#ifdef CHECK
		OperatorInstance_delete++;	// debug
#endif
	}
	//! Create an operator instance.
	//! Each operator has a unique "number" which will be
	//! used later to identify it.
	//! @param Input. oper is a pointer to an operator
	/* GL 26/04/2007 :	Replace fllowing line to attribute a new operator after resizing
	   OperatorInstance(const Operator*oper): _oper(oper), _id(count++) {
	   By*/
	OperatorInstance(Operator*oper, const Cadency*cadency, const Clock*clk): _id(count++)
	{
		_oper = oper;
		/* DEBUG_DH trace debug inst->oper if(oper->name().compare("sub_op") ==0)
		   cout <<  "ici "  << oper->name()<< endl;
		   */
		/* Fin GL*/
		_state = READY;
		_func = 0;		// no binding initially
		if(oper == 0)count = 0;
		cpt_use =0; // the number of time the courant operator instance are used
		/* 06/06/2007 KT*/
		_reservation_table.clear();
		_reservation_table.resize(cadency->length()/clk->period());
		/* End KT*/
		/* GL 27/06/07 : correction bug*/
		if(oper != 0)
		{
			int i;
			/* Fin GL*/
			/* GL 14/05/2007 :	Copy oper Bitwidths en signeds*/
			_inputsBitwidth.assign(oper->inputPorts(), 16);
			_inputsSigned.assign(oper->inputPorts(), 0);
			_outputsBitwidth.assign(oper->outputPorts(), 16);
			_outputsSigned.assign(oper->outputPorts(), 0);
			/* GL 27/06/07 : correction bug*/
		}
		/* Fin GL*/
		/* GL 02/10/07 : connections with previous instances*/
		_connections = new ConnectionRefs();
		/* Fin GL*/
#ifdef CHECK
		OperatorInstance_create++;
#endif

	}
	/* KT 15/07/2007*/
	OperatorInstance(Operator*oper, const Cadency*cadency, const Clock*clk, SelectionOutRefs*soRefs): _id(count++)
	{
		/* DEBUG_DH
		   trace debug inst->oper if(oper->name().compare("sub_op") ==0)
		   cout <<  "ici OperatorInstance id"  << _id  << endl;


		   if(_id > 40)
		   cout <<  "bug OperatorInstance id"  << _id  << endl;
		   FIN  DEBUG_DH*/


		_oper = oper;
		_state = READY;
		_func = 0;		// no binding initially
		if(oper == 0)
		{
			count = 0;
			return;
		}
		cpt_use =0; // the number of time the courant operator instance are used
		long D = 0;
		SelectionOutRefs::const_iterator m_it;			// SelectionOut iterator
		for(m_it = soRefs->begin(); m_it != soRefs->end(); m_it++)
		{
			SelectionOut*m = (*m_it).second;			// get current SelectionOut
			if(m->oper() == _oper)
				D = m->length();
		}
		/* GL 20/11/07 : solution provisoire pour les multi-fonctions(vue avec KT)*/
		if(D==0)cpt_max_use = 1000;
		else
			/* Fin GL*/
			cpt_max_use= floor(((double)(cadency->length()/ D)));// the max number of times we can use this operator instance
		//cout <<" lopérateur "<< _oper->name()<<" ne doit pas depasser " <<cpt_max_use<<endl;
		_reservation_table.clear();
		_reservation_table.resize(cadency->length()/clk->period());
		/* GL 27/06/07 : correction bug*/
		if(oper != 0)
		{
			int i;
			/* Fin GL*/
			/* GL 14/05/2007 :	Copy oper Bitwidths en signeds*/
			_inputsBitwidth.assign(oper->inputPorts(), 16);
			_inputsSigned.assign(oper->inputPorts(), 0);
			_outputsBitwidth.assign(oper->outputPorts(), 16);
			_outputsSigned.assign(oper->outputPorts(), 0);
			/* GL 27/06/07 : correction bug*/
		}
		/* Fin GL*/
		/* GL 02/10/07 : connections with previous instances*/
		_connections = new ConnectionRefs();
		/* Fin GL*/
#ifdef CHECK
		OperatorInstance_create++;
#endif

	}


	/* OperatorInstance(const OperatorInstance &titi)
	   {
	   cout << "bug" << endl;
	   }*/


	/* End KT*/
	/* KT 05/05/2008*/

	//! set index
	void index(int index)
	{
		_index = index;
	}
	//! get index
	int index()
	{
		return _index;
	}
	/*  End KT*/
	/* GL 26/04/2007 :	Replace fllowing line
	   to attribute a new operator after resizing
	   const Operator* oper()const {return _oper; }		//!< Get operator.
	   By*/
	Operator* oper()const
	{
		return _oper;    //!< Get operator.
	}
	/* And add a setter*/
	void oper(Operator*oper)
	{
		_oper = oper;    //!< Set operator.
	}
	/* Fin GL*/
	bool ready()const
	{
		return(_state == READY);    //!< Ask if instance is ready.
	}
	bool running()const
	{
		return(_state == RUNNING);    //!< Ask if instance is running.
	}
	void isRunning()
	{
		_state = RUNNING;    //!< Set instance's state to running.
	}
	void isReady()
	{
		_state = READY;    //!< Set instance's state to ready.
	}
	long no()const
	{
		return _id;    //!< Get operator instance unique number.
	}
	/* Caaliph 27/06/2007*/
	void change_no(long no)
	{
		_id2 = no;    //add by Caaliph
	}
	long change_no()const
	{
		return _id2;    //add by Caaliph
	}
	/* End Caaliph*/
	void info()const  									//!< Print info.
	{
		cout << _oper->name()<< endl;
	}
	// BINDING
	const Function* binding()const
	{
		return _func;    //!< Get bound function.
	}
	//! Bind a function to an operator instance.
	//! @param f Input. Is a pointer to the function to bind.
	void bind(const Function* f)
	{
		_func = f;
	}
	//! Get function
	const Function* function()const
	{
		return _func;
	}
	//! KT 06/06/2007
	bool freeOpr(int numcstep)
	{
		return _reservation_table[numcstep]==0?false:true;
	}
	/* GL 14/05/2007 :	Add fllowing lines*/
	long inputsBitwidth(long i)const
	{
		return _inputsBitwidth[i];
	};				//!< Get inputBitwidth by pos.
	bool inputsSigned(long i)const
	{
		return _inputsSigned[i];
	};					//!< Get inputSigned by pos.
	long outputsBitwidth(long i)const
	{
		return _outputsBitwidth[i];
	};				//!< Get outputBitwidth by pos.
	bool outputsSigned(long i)const
	{
		return _outputsSigned[i];
	};					//!< Get outputSigned by pos.
	void inputsBitwidth(long i, long width)
	{
		_inputsBitwidth[i]=width;
	};	//!< Set inputBitwidth by pos.
	void inputsSigned(long i, bool signe)
	{
		_inputsSigned[i]=signe;
	};		//!< Set inputBitwidth by pos.
	void outputsBitwidth(long i, long width)
	{
		_outputsBitwidth[i]=width;
	};	//!< Set inputBitwidth by pos.
	void outputsSigned(long i, bool signe)
	{
		_outputsSigned[i]=signe;
	};		//!< Set inputBitwidth by pos.

	/* Fin GL*/
	/* GL 16/05/2007 : Function sum inputs bitwidth*/
	long sumOfwidths()
	{
		long sum=0;
		for(int i =0; i<oper()->inputPorts();i++)sum += inputsBitwidth(i);
		return sum;
	};
	/* Fin GL*/
	/* GL 26/06/2007 : ok with operation bitwidth or not*/
	bool fitsOperation(const Operation*o)//!< Ask if instance fits operation bitwidth.
	{
		bool OK = true;
		for(int j = 0; j<_oper->inputPorts()&& OK;j++)
			OK = (inputsBitwidth(j)>= o->inputsBitwidth(j));
		return OK;
	}
	/* Fin GL*/
	/* GL 02/10/07 : connections with previous instances*/
	//!< return connections number with a given instance
	long NumberOfConnection(OperatorInstance*oi)
	{
		Connection*c;
		const string name = Parser::itos(_id)+ "_" + Parser::itos(oi->no());
		if(_connections->search(name, &c))return c->NumberOfConnections();
		return -1;
	}
	//!< add new connections with an instance
	void NewConnection(OperatorInstance*oi)
	{
		Connection*c;
		const string name = Parser::itos(_id)+ "_" + Parser::itos(oi->no());
		if(_connections->search(name, &c))
		{
			c->operator ++();
		}
		else
		{
			Connection*con = new Connection(name);
			ConnectionRef c_ref(con);
			_connections->add(c_ref);
		}
	}
	/* Fin GL*/

	void add_inputUseReg(InputOperatorRegUse*use);

	//! count number of register connected to an input of operator instance execpt the register
	long numberOfInputMuxToInputOperatorInstanceExceptThisRegister(const Reg*reg,long input_pos)const;

	//! count number of register connected to all inputs of operator instance
	long numberOfInputMuxToAllInputsOperatorInstance()const;



	//! Build a component name from an instance.
	//! @result a string
	string instance_name()const
	{
		string name("comp_");
		name += Parser::itos(no());
		name += "_";
		name += oper()->name();
		return name;
	}



	//! Build a component name from an instance.
	//! @result a string
	string instance_name(const Switches*_sw)const
	{
		string name("comp_");
		/* Caaliph 27/06/2007*/
		if(_sw->cdfgs()||_sw->n_ordo()|| _sw->spact())
			name += Parser::itos(change_no());//add by Caaliph
		else
			/* End Caaliph*/
			name += Parser::itos(no());
		name += "_";
		name += oper()->name();
		return name;
	}


	void updateWidth(const Operation*o)
	{
		for(int i = 0; i<oper()->inputPorts();i++)
		{
			if(o->inputsBitwidth(i)>inputsBitwidth(i))/* Fin GL*/inputsBitwidth(i, o->inputsBitwidth(i));
		}
	}


	//!< update instance connections when bound to opeartion
	void UpdateConnection(const Operation*o);
	/* Fin GL*/

};

//! This is the scheduling state of an operation.
class OperationSchedulingState
{
public:
	OperationSchedulingState()
	{
		to_schedule = false;
		instance = 0;
		mobility = -1;
		_start = -1;
#ifdef CHECK
		OperationSchedulingState_create++;
#endif
	}
	OperationSchedulingState(const OperationSchedulingState &o)
	{
		_start = o._start;
		//scheduled = o.scheduled;
		to_schedule = o.to_schedule;
		instance = o.instance;
		mobility = o.mobility;
#ifdef CHECK
		OperationSchedulingState_create++;
#endif
	}
	OperationSchedulingState & operator= (const OperationSchedulingState &o)
	{
		_start = o._start;
		//scheduled = o.scheduled;
		to_schedule = o.to_schedule;
		instance = o.instance;
		mobility = o.mobility;
#ifdef CHECK
		OperationSchedulingState_create++;
#endif
		return*this;
	}
	~OperationSchedulingState()
	{
#ifdef CHECK
		OperationSchedulingState_delete++;
#endif
	}
	//bool				scheduled;	//!< Scheduling state: true = scheduled, false = to schedule.
	bool				to_schedule; //!< Scheduling state: true = to schedule, false = not to schedule
	OperatorInstance*instance;	//!< Pointer to associated operator instance.
	long				mobility;	//!< Difference between _time and alap = mobility.
	//! Set start time.
	void start(long s)
	{
		if(s < 0)
		{
			cerr << "Internal error: operation scheduling state has a negative(" << s << ")start time" << endl;
			exit(1);
		}
		_start = s;
	}
	//! get start time
	long start()const
	{
		return _start;
	}
private:
	long				_start;		//!< Start time.
};

//! This is the ordering function for operation's mobility.
class Mobility
{
private :
public:
	Mobility()
	{

#ifdef CHECK
		Mobility_create++;
#endif
	}
	Mobility(const Mobility &m)
	{
#ifdef CHECK
		Mobility_create++;
#endif
	}
	Mobility & operator= (const Mobility &m)
	{
#ifdef CHECK
		Mobility_create++;
#endif
		return*this;
	}
	~Mobility()
	{
#ifdef CHECK
		Mobility_delete++;
#endif
	}
	bool operator()(const Operation*a, const Operation*b)const
	{
		OperationSchedulingState*a_ss = (OperationSchedulingState*)a->_addr;
		OperationSchedulingState*b_ss = (OperationSchedulingState*)b->_addr;

		return(a_ss->mobility > b_ss->mobility);
	}
};

// CLASS Priority
//! This is the ordering function for operation's priority.
class Priority
{
private :
public:
	Priority()
	{

#ifdef CHECK
		Priority_create++;
#endif
	}
	Priority(const Priority &m)
	{
#ifdef CHECK
		Priority_create++;
#endif
	}
	Priority & operator= (const Priority &m)
	{
#ifdef CHECK
		Priority_create++;
#endif
		return*this;
	}
	~Priority()
	{
#ifdef CHECK
		Priority_delete++;
#endif
	}
	// Here are declared all ordering functions
	static bool mobility(const Operation* a, const Operation* b)
	{

		OperationSchedulingState*a_ss = (OperationSchedulingState*)a->_addr;
		OperationSchedulingState*b_ss = (OperationSchedulingState*)b->_addr;

		return(a_ss->mobility > b_ss->mobility);
	}

	static bool mobility_upBitwidth(const Operation*a, const Operation*b)
	{
		OperationSchedulingState*a_ss = (OperationSchedulingState*)a->_addr;
		OperationSchedulingState*b_ss = (OperationSchedulingState*)b->_addr;

		if(a_ss->mobility == b_ss->mobility)
			return(a->sumOfwidths()> b->sumOfwidths());
		return(a_ss->mobility > b_ss->mobility);
	}

	static bool mobility_downBitwidth(const Operation*a, const Operation*b)
	{
		OperationSchedulingState*a_ss = (OperationSchedulingState*)a->_addr;
		OperationSchedulingState*b_ss = (OperationSchedulingState*)b->_addr;

		if(a_ss->mobility == b_ss->mobility)
			return(a->sumOfwidths()< b->sumOfwidths());
		return(a_ss->mobility > b_ss->mobility);
	}

	static bool upBitwidth(const Operation*a, const Operation*b)
	{
		return(a->sumOfwidths()> b->sumOfwidths());
	}

	static bool downBitwidth(const Operation*a, const Operation*b)
	{
		return(a->sumOfwidths()< b->sumOfwidths());
	}
	static bool mobility_bitwidth(const Operation*a, const Operation*b)
	{
		const double alpha = 0.3;
		OperationSchedulingState*a_ss = (OperationSchedulingState*)a->_addr;
		OperationSchedulingState*b_ss = (OperationSchedulingState*)b->_addr;

		double priority_a=0, priority_b=0;
		priority_a = a_ss->mobility* alpha + a->overcost()/*sumOfwidths()*/*(1 - alpha);
																			priority_b = b_ss->mobility* alpha + b->overcost()/*sumOfwidths()*/*(1 - alpha);
																																				return(priority_a > priority_b);
																																				}
																																				};


		// CLASS priority_list
		//template<class _Ty, class _C = vector<_Ty> >

		class priority_list
		{
		public:
		typedef const Operation* _Ty;
		typedef vector<_Ty> _C;
		typedef _C::allocator_type allocator_type;
		typedef _C::value_type value_type;
		typedef _C::size_type size_type;

		explicit priority_list(const allocator_type& _Al = allocator_type()): c(_Al) {}

		typedef const value_type*_It;

		priority_list(_It _F, _It _L,const allocator_type& _Al = allocator_type()): c(_Al)
		{
		for(; _F != _L; ++_F)
		push(*_F);
		}

		bool empty()const
		{
		return(c.empty());
		}

		size_type size()const
		{
		return(c.size());
		}

		value_type& top()
		{
		return(c.front());
		}

		const value_type& top()const
		{
		return(c.front());
		}

		void push(const value_type& _X)
		{
		c.push_back(_X);
		push_heap(c.begin(), c.end(), comp);
		}

		void pop()
		{
		pop_heap(c.begin(), c.end(), comp);
		c.pop_back();
		}

		bool(*comp)(_Ty, _Ty);
		protected:
		_C c;
		};

		/* Fin GL*/
		//! This is the scheduling state of an operator.
		class OperatorSchedulingState
{
private:
public:
	OperatorSchedulingState()
	{
		freed = false;
		no_more_hardware = false;
#ifdef CHECK
		OperatorSchedulingState_create++;
#endif
	}
	OperatorSchedulingState(const OperatorSchedulingState &o)
	{
		no_more_hardware = o.no_more_hardware;
		freed = false;
		E = o.E;
		D = o.D;
		//operations = o.operations;
#ifdef CHECK
		OperatorSchedulingState_create++;
#endif
	}
	OperatorSchedulingState & operator= (const OperatorSchedulingState &o)
	{
		no_more_hardware = o.no_more_hardware;
		freed = false;
		E = o.E;
		D = o.D;
		//operations = o.operations;
#ifdef CHECK
		OperatorSchedulingState_create++;
#endif
		return*this;
	}
	~OperatorSchedulingState()
	{
		free();
#ifdef CHECK
		OperatorSchedulingState_delete++;
#endif
	}
	//! Local type of lists of operators.
	typedef deque<OperatorInstance*> OPERATORS;
	//! Already selected operators. GAUT's E list. 1rst priority.
	OPERATORS						E,
									//! never selected operators. GAUT's D list. 2nd priority.
									D;
	//! Ordered list of operations for operation selection.
	//! This list is updated after each clock cycle.
	//	typedef priority_queue<const Operation*, deque<const Operation*>, Mobility> OPERATIONS;
	//! List of operations.
	//	OPERATIONS operations;
	//! To mark operator when there are no more instances ready
	bool no_more_hardware;
	//! TO signal when object has been freed.
	bool freed;								// freed toggle
	//! Explicit free of operators lists.
	void free()
	{
		int i;
		//DH :17/07/2009  bug inst->oper for(i = 0; i < E.size(); i++)delete E[i];
		//DH :17/07/2009  bug inst->operfor(i = 0; i < D.size(); i++)delete D[i];
		E.clear();
		D.clear();

	}
};

//! This is the scheduling state of a function in the CDFG(GAUT's FileS).
class FunctionSchedulingState
{
private:
	const Function* _func;
public:
	FunctionSchedulingState(const Function* func): _func(func)
	{
		no_more_hardware = false;
#ifdef CHECK
		FunctionSchedulingState_create++;
#endif
	}
	FunctionSchedulingState(const FunctionSchedulingState &f)
	{
		_func = f._func;
		no_more_hardware = f.no_more_hardware;
		EnsX = f.EnsX;
		EnsC = f.EnsC;
		EnsCC = f.EnsCC;
		allocated = f.allocated;
#ifdef CHECK
		FunctionSchedulingState_create++;
#endif
	}
	FunctionSchedulingState & operator= (const FunctionSchedulingState &f)
	{
		_func = f._func;
		no_more_hardware = f.no_more_hardware;
		EnsX = f.EnsX;
		EnsC = f.EnsC;
		EnsCC = f.EnsCC;
		allocated = f.allocated;
#ifdef CHECK
		FunctionSchedulingState_create++;
#endif
		return*this;
	}
	~FunctionSchedulingState()
	{
#ifdef CHECK
		FunctionSchedulingState_delete++;
#endif
	}
	//! Local type for list of operations.
	typedef deque<const Operation*> OPERATIONS;
	//! Executables operations.
	OPERATIONS						EnsX,
									//! Running operations on non-pipeline operators(op not released).
									EnsC,
									//! Running operations on pipeline-operators(but op released).
									EnsCC;
	OperatorRefs					allocated; //!< Set of allocated operators.
	//! to mark functions when there is no more hardware available
	bool no_more_hardware;
	//! tell if there are still operations to schedule now or later.
	bool operationsToSchedule()const;
	//! Print info
	void info()const;
	/* GL 05/06/07 : binding mode 2, 3; assign at the end of cycle
	   correct bug*/
	typedef deque<OperatorInstance*> A_OPERATORS;
	A_OPERATORS assigned; //!< Set of assigned instances in current cycle
	/* Fin GL*/
	/* GL 07/09/07 : new algo*/
	OPERATIONS to_bind; //!< Set of scheduled operations in current cycle
	/* Fin GL*/
};

//! The result of the scheduling for a pipeline stage.
class SchedulingStage
{
public:
	typedef vector<OperatorInstance*> INSTANCES;
	typedef vector<Operator*> REMINED_INSTANCES;
	SchedulingStage()
	{
#ifdef CHECK
		SchedulingStage_create++;
		instances.clear();
#endif
	}
	SchedulingStage(const SchedulingStage &s)
	{
		instances = s.instances;
#ifdef CHECK
		SchedulingStage_create++;
#endif
	}
	SchedulingStage & operator= (const SchedulingStage &s)
	{
		instances = s.instances;
#ifdef CHECK
		SchedulingStage_create++;
#endif
		return*this;
	}
	~SchedulingStage()
	{
#ifdef CHECK
		SchedulingStage_delete++;
#endif
	}
	INSTANCES	instances;	//!< Vector of the operator instances used in the stage.
	/* KT 15/07/2007*/
	REMINED_INSTANCES	remined_instances;//!< Vector of the operator remined after scheduling the courant stage.
	/* End KT*/
	//! Print info.
	void info()const
	{
		cout << "(" << instances.size()<<")" << endl;
		for(int i = 0; i < instances.size(); i++)
		{
			cout << "  Instance #" << instances[i]->no()<< " ";
			instances[i]->info();
			//cout <<" the reservation table is : ";
			// KT 06/06/2007
			//for(int j = 0; j < instances[i]->_reservation_table.size(); j++)
			//	cout << instances[i]->_reservation_table[j];
			//cout << endl;
		}
		// End KT
	}
};

//! Result of the scheduling process.
//! It contains informations about each pipeline stage and about memory banks.
class SchedulingOut
{
public:
	typedef vector<SchedulingStage> STAGES;
	SchedulingOut()
	{
#ifdef CHECK
		SchedulingOut_create++;
#endif
	}
	SchedulingOut(const SchedulingOut &so)
	{
		stages = so.stages;
#ifdef CHECK
		SchedulingOut_create++;
#endif
	}
	SchedulingOut & operator= (const SchedulingOut &so)
	{
		stages = so.stages;
#ifdef CHECK
		SchedulingOut_create++;
#endif
		return*this;
	}
	~SchedulingOut()
	{
#ifdef CHECK
		SchedulingOut_delete++;
#endif
	}
	STAGES		stages;		//!< Vector of pipeline stages.
	Banks		banks;		//!< Memory banks.
	//! Print info.
	void info()const
	{
		for(int i = 0; i < stages.size(); i++)
		{
			cout << "Stage " << i << endl;
			stages[i].info();
		}
		cout << endl;
		banks.info();
	}
	//! Print summary info
	void summary(const CDFG*cdfg, const Cadency*cad, const Clock*clk)const
	{
		/* DH : for levinson result only*/
		int total_operator_add = 0;
		int total_operator_mult = 0;
		int total_operator_sub = 0;
		/* FIN DH : for levinson result only*/

		vector<OperatorInstance*>	operators;
		vector<int>	operatorStage;
		vector<int>	operatorNumber;
		vector<double> operatorUseRate; // number of operation / cycles
		long stage, i, j;
		for(stage = 0; stage < stages.size(); stage++)
		{
			long nops = stages[stage].instances.size();
			for(i = 0; i < nops; i++)
			{
				OperatorInstance*o = (OperatorInstance*)stages[stage].instances[i];
				bool new_op = true;
				for(j = 0; j < operators.size(); j++)
				{
					OperatorInstance*op = operators[j];
					if(op->oper()->component() == o->oper()->component()&& operatorStage[j] == stage)
					{
						operatorNumber[j]++;
						new_op = false;
						break;
					}
				}
				if(new_op == true)
				{
					operators.push_back(o);
					operatorStage.push_back(stage);
					operatorNumber.push_back(1);
				}
			}
		}


		cout << "\n------------------------------------------------------------------------------------------------------------" << endl;
		cout << "  " << "Operator" << " \t;  " << "Stage " << " \t;  " << "Allocation;  Area " << " " << endl;
		cout << "------------------------------------------------------------------------------------------------------------" << endl;
		int n;
		int total_area = 0;
		int total_operators = 0;
		for(n = 0; n < operators.size(); n++)
		{
			cout << "  " << operators[n]->oper()->component()<< " \t;  " << operatorStage[n] << " \t;  " << operatorNumber[n]
				<< " \t;  " << operators[n]->oper()->area()<< endl;
			total_area += operators[n]->oper()->area()* operatorNumber[n];
			total_operators += operatorNumber[n];
			if(operators[n]->oper()->implements("add")|| operators[n]->oper()->implements("add_fp"))
				total_operator_add += operatorNumber[n];
			if(operators[n]->oper()->implements("mul")|| operators[n]->oper()->implements("mul_fp"))
				total_operator_mult += operatorNumber[n];
			if(operators[n]->oper()->implements("sub")|| operators[n]->oper()->implements("sub_fp"))
				total_operator_sub += operatorNumber[n];
		}
		cout << "------------------------------------------------------------------------------------------------------------" << endl;
		cout << "  operator(s)total = " << total_operators << ";  stage(s) = " << stages.size()<< ";  area total = " << total_area << endl;
		/* DH : for levinson result only*/
		cout << "  operator add total = " << total_operator_add  << endl;
		cout << "  operator mul total = " << total_operator_mult << endl;
		cout << "  operator sub total = " << total_operator_sub  << endl;
		/* FIN DH : for levinson result only*/
		cout << "------------------------------------------------------------------------------------------------------------" << endl;
		cout << endl;


		operators.clear();
		operatorStage.clear();
		operatorNumber.clear();

		for(stage = 0; stage < stages.size(); stage++)
		{
			long nops = stages[stage].instances.size();
			for(i = 0; i < nops; i++)
			{
				OperatorInstance*o = (OperatorInstance*)stages[stage].instances[i];

				operators.push_back(o);
				operatorStage.push_back(stage);
				operatorNumber.push_back(1);
				long length = 0;
				long rate = 0;
				for(j = 0; j < cdfg->nodes().size(); j++)
				{
					const CDFGnode*node = cdfg->nodes().at(j);
					if(node->type()!= CDFGnode::OPERATION)continue;
					const Operation*oper = (const Operation*)node;
					if(!oper->inst())continue;
					if(oper->inst()->no() == o->no())
					{
						length = oper->cycles();
						rate++;
					}
				}
				operatorUseRate.push_back(length* rate*100/(cad->length()/clk->period()));
			}
		}
		cout << "------------------------------------------------------------------------------------------------------------" << endl;
		cout << "  " << "Operator" << " \t;  " << "Number" << " \t;  " << "Use Rate(%)"  << endl;
		cout << "------------------------------------------------------------------------------------------------------------" << endl;
		for(n = 0; n < operators.size(); n++)
		{
			cout << "  " << operators[n]->oper()->component()<< " \t;  " << operators[n]->no()<< " \t;  " << operatorUseRate.at(n)<< endl;

		}
		cout << "------------------------------------------------------------------------------------------------------------" << endl;
		cout << endl;


		operators.clear();
		operatorStage.clear();
		operatorNumber.clear();

	}


	void summary(const CDFG*cdfg, const Cadency*cad, const Clock*clk, ostream& out)const
	{
		/* DH : for levinson result only*/
		int total_operator_add = 0;
		int total_operator_mult = 0;
		int total_operator_sub = 0;
		/* FIN DH : for levinson result only*/

		vector<OperatorInstance*>	operators;
		vector<int>	operatorStage;
		vector<int>	operatorNumber;
		vector<double> operatorUseRate; // number of operation / cycles
		long stage, i, j;
		for(stage = 0; stage < stages.size(); stage++)
		{
			long nops = stages[stage].instances.size();
			for(i = 0; i < nops; i++)
			{
				OperatorInstance*o = (OperatorInstance*)stages[stage].instances[i];
				bool new_op = true;
				for(j = 0; j < operators.size(); j++)
				{
					OperatorInstance*op = operators[j];
					if(op->oper()->component() == o->oper()->component()&& operatorStage[j] == stage)
					{
						operatorNumber[j]++;
						new_op = false;
						break;
					}
				}
				if(new_op == true)
				{
					operators.push_back(o);
					operatorStage.push_back(stage);
					operatorNumber.push_back(1);
				}
			}
		}

		int n;
		int total_area = 0;
		int total_operators = 0;
		for(n = 0; n < operators.size(); n++) {
			out << operators[n]->oper()->component()<< " = " << operatorNumber[n] << endl;
			total_area += operators[n]->oper()->area()* operatorNumber[n];
			total_operators += operatorNumber[n];
			if(operators[n]->oper()->implements("add")|| operators[n]->oper()->implements("add_fp"))
				total_operator_add += operatorNumber[n];
			if(operators[n]->oper()->implements("mul")|| operators[n]->oper()->implements("mul_fp"))
				total_operator_mult += operatorNumber[n];
			if(operators[n]->oper()->implements("sub")|| operators[n]->oper()->implements("sub_fp"))
				total_operator_sub += operatorNumber[n];
		}
		out << "op_total = " << total_operators << endl;
		/* DH : for levinson result only*/
		out << "op_add_total = " << total_operator_add  << endl;
		out << "op_mul_total = " << total_operator_mult << endl;
		out << "op_sub_total = " << total_operator_sub  << endl;
		out << "op_area = " << total_area << endl;
		/* FIN DH : for levinson result only*/

		operators.clear();
		operatorStage.clear();
		operatorNumber.clear();

#if 0
		for(stage = 0; stage < stages.size(); stage++) {
			long nops = stages[stage].instances.size();
			for(i = 0; i < nops; i++) {
				OperatorInstance*o = (OperatorInstance*)stages[stage].instances[i];
				operators.push_back(o);
				operatorStage.push_back(stage);
				operatorNumber.push_back(1);
				long length = 0;
				long rate = 0;
				for(j = 0; j < cdfg->nodes().size(); j++) {
					const CDFGnode*node = cdfg->nodes().at(j);
					if(node->type()!= CDFGnode::OPERATION)continue;
					const Operation*oper = (const Operation*)node;
					if(!oper->inst())continue;
					if(oper->inst()->no() == o->no())
					{
						length = oper->cycles();
						rate++;
					}
				}
				operatorUseRate.push_back(length* rate*100/(cad->length()/clk->period()));
			}
		}
		out << "------------------------------------------------------------------------------------------------------------" << endl;
		out << "  " << "Operator" << " \t;  " << "Number" << " \t;  " << "Use Rate(%)"  << endl;
		out << "------------------------------------------------------------------------------------------------------------" << endl;
		for(n = 0; n < operators.size(); n++)
		{
			out << "  " << operators[n]->oper()->component()<< " \t;  " << operators[n]->no()<< " \t;  " << operatorUseRate.at(n)<< endl;

		}
		out << "------------------------------------------------------------------------------------------------------------" << endl;
		out << endl;
#endif

		operators.clear();
		operatorStage.clear();
		operatorNumber.clear();

	}

	//DEBUG_DH
	void debug_oper()const
	{

		long stage, i, j;
		for(stage = 0; stage < stages.size(); stage++)
		{
			long nops = stages[stage].instances.size();
			for(i = 0; i < nops; i++)
			{
				OperatorInstance*o = (OperatorInstance*)stages[stage].instances[i];
				cout << o->oper()->name()<< endl;
			}
		}
	}




	long operators()const
	{
		long s = 0;
		for(int stage=0; stage<stages.size(); stage++)
		{
			s+= stages[stage].instances.size();
		}
		return s;
	}


	long max_operator_no()const
	{
		long max = 0;
		for(int stage=0; stage<stages.size(); stage++)
		{
			long nops = stages[stage].instances.size();
			for(int i = 0; i < nops; i++)
			{
				const OperatorInstance*o = (const OperatorInstance*)stages[stage].instances[i];
				if(o->no()>max)
					max=o->no();
			}
		}
		return max;
	}


	long NumberOfMux2to1InputOperatorInstance()const
	{
		vector<const OperatorInstance*>	operators;
		vector<int>	operatorStage;
		long stage, i, j;
		for(stage = 0; stage < stages.size(); stage++)
		{
			long nops = stages[stage].instances.size();
			for(i = 0; i < nops; i++)
			{
				const OperatorInstance*o = (const OperatorInstance*)stages[stage].instances[i];
				bool new_op = true;
				for(j = 0; j < operators.size(); j++)
				{
					const OperatorInstance*op = operators[j];
					if(op->oper()->component() == o->oper()->component()&& operatorStage[j] == stage)
					{
						new_op = false;
						break;
					}
				}
				if(new_op == true)
				{
					operators.push_back(o);
					operatorStage.push_back(stage);
				}
			}
		}
		int n;
		long nbmux2to1=0;
		for(n = 0; n < operators.size(); n++)
		{
			const OperatorInstance*inst=operators[n];
			nbmux2to1+=inst->numberOfInputMuxToAllInputsOperatorInstance();
		}
		operators.clear();
		operatorStage.clear();
		return nbmux2to1;
	}
};

//! The scheduling process.
//!
class Scheduling
{
public:
	static bool			_debug;				// debug toggle
private:

	//! to stop scheduling when all hardware resources are used
	bool _no_more_hardware;
	//! to inform when hardware are allocated // EJ 06/2007
	bool _newAllocateInstance;
	//! to save first instance allocated // EJ 06/2007
	vector<Operator*> _newAllocateOperator;
	//! to inform when there is one or more loopback variable
	bool there_is_loopback_variable;
	//! Ordered list of operations for operation selection.
	//! This list is updated after each clock cycle.
	/* GL 22/06/07	: Replace lines
	   typedef priority_queue<const Operation*, vector<const Operation*>, Mobility> OPERATIONS;
	/* GL 24/05/07 : Bind at the end of cycle*/
	//typedef priority_queue<const Operation*, vector<const Operation*>, UpDownBitwidth> S_OPERATIONS;
	//! Global list of operations to bind
	//S_OPERATIONS*_toBind;				// scheduled operations
	/*Fin GL*/
	/* By*/
	typedef priority_list OPERATIONS;
	/* Fin GL*/
	//! Global list of operations to schedule
	OPERATIONS*_operations;
	//! Global list of operations to bind
	OPERATIONS*_toBind;				// scheduled operations
	//! List of operations for which scheduling failed, no need to order them and slow down it management
	vector<const Operation*> _operations_failed;
	//! local type for operations in the global scheduling list
	struct func_sched
	{
		const Function*func;	// function
		long            n;		// number of operations for the function
		bool			no_more_hardware;
	};
	//! List of functions in the scheduling list
	typedef vector<struct func_sched> FUNCTIONS;
	FUNCTIONS _functions;

	// FifoR management
	static void  add(deque<const Operation*> &oplist, const Operation*o)
	{
		for(int i = 0; i < oplist.size(); i++)if(oplist[i] == o)return; // already there
		//cout << "adding " << o->name()<< endl;
		oplist.push_back(o);								// add pointer to operation at tail
	}
	void remove(deque<const Operation*> &oplist, const Operation*o)
	{
		for(int i = 0; i < oplist.size(); i++)if(oplist[i] == o)
		{
			//cout << "removing " << o->name()<< endl;
			oplist.erase(oplist.begin()+i);					// remove pointer
			return;
		}
	}
	/* GL 24/05/07 : insert an operator instance in a sorted list*/
	void remove(deque<OperatorInstance*> &oilist, OperatorInstance*oi)
	{
		for(int i = 0; i < oilist.size(); i++)if(oilist[i] == oi)
		{
			oilist.erase(oilist.begin()+i);					// remove pointer
			return;
		}
	}
	void insert(deque<OperatorInstance*> &oilist, OperatorInstance*oi)
	{
		if(/*GL*/!/**/oilist.empty())
		{
			deque<OperatorInstance*>::iterator it = oilist.begin();
			while(it!= oilist.end())
			{
				if(oi->sumOfwidths()<(*it)->sumOfwidths())break;
				it++;
			}
			if(it == oilist.end())
				oilist.push_back(oi);
			else
				oilist.insert(it,oi);
		}
		else
			oilist.push_back(oi);
	}
	/* Fin GL*/
	/* GL 27/09/2007*/
	void newWidth(OperatorInstance*oi, const Operation*o)
	{
		for(int i = 0; i<oi->oper()->inputPorts();i++)oi->inputsBitwidth(i, o->inputsBitwidth(i));
	}
	/* Fin GL*/
	/* GL 11/09/07 : insert an operation in sorted list*/
	void insert(deque<const Operation*> &oplist, const Operation*o)
	{
		if(!oplist.empty())
		{
			deque<const Operation*>::iterator it = oplist.begin();
			while(it!= oplist.end())
			{
				if(o->sumOfwidths()<(*it)->sumOfwidths())break;
				it++;
			}
			if(it == oplist.end())
				oplist.push_back(o);
			else
				oplist.insert(it,o);
		}
		else
			oplist.push_back(o);
	}
	/* Fin GL*/
	/* GL 02/10/07*/

	// to update CDFG after scheduling
	void updateCDFG(CDFGnode*n);
	// to clear CDFG
	void clearCDFG(CDFGnode*n);

	// Specific tools
	//const Operation* selectBestCandidate(OperatorSchedulingState::OPERATIONS**operations);
	bool  tryToScheduleOperation(const Operation*o, SchedulingOut*sos);	// try to schedule an operation
	void  orderOperations();				// re-order operations
	void  scheduleOperations(SchedulingOut*sos);	// schedule re-ordered operations "now"
	bool  schedule(SchedulingOut*sos);				// a full schedule turn
	void  nextEvent(AllocationOut*allocs, SchedulingOut*sos);		// fly time until an event occurs
	/* GL 26/04/2007 :	Replace fllowing line
	   operator is no more constant
	   void  freeOperatorInstances(long s, const Operator*o, AllocationOut*allocs); // free instances of an operator
	   By*/
	void  freeOperatorInstances(long s, Operator*o, AllocationOut*allocs); // free instances of an operator
	/* Fin GL*/
	/* Caaliph 27/06/2007*/
	void  freeOperatorInstances(long s, const Operator*o); // free instances of an operator
	/* End Caaliph*/
	bool  freeOperators(const Operation*o);// release operator if operation is finished
	void  freeOperators(const Function*f);	// release operators for a function when operations are finished
	void  freeOperators();					// release all operators when operations are finished
	void  freeStage(long s, AllocationOut*allocs);				// free hardware for previous pipeline stage
	/*  KT 15/07/2007 Replace the folowing line
		void  newStage(long s);
		By*/
	void  newStage(long s, SchedulingOut*sos);					// allocate hardware for a new pipeline stage
	/*End KT*/
	/* Caaliph 27/06/2007*/
	void  freeCycle(long c, long s);				// free hardware for previous pipeline stage
	void  newCycle(long c,long s);					// allocate hardware for a new pipeline stage
	/* End Caaliph*/
	void  bind(const Operation*o, FunctionSchedulingState*fss, OperatorInstance*oi); //(operation,operator)assignment
	/*GL 24/05/07 : bind at the end of the cycle*/
	void  bind();							// all scheduled operations(operation,operator)assignment
	/* Fin GL*/
	/* GL 23/07/07 : New Function*/
	void MWBMatching();
	/* Fin GL*/
	/* 29/10/07 : add lines*/
	long chainOperationsInPattern(const Operation* o);
	/* Fin GL*/
	/* GL / 08/01/08 : add lines*/
	bool  allPredecessorsScheduled(const Operation*o)const; // detect if all predeccesors of an operation are scheduled
	bool scheduled(const CDFGnode*node)const; // detect if a CDFG node is scheduled
	/* Fin GL*/
	/* GL : add lines*/
	void  lockAllPredecessors(const Operation*o)const;
	/* Fin GL*/
	bool  allPredecessorsFinished(long l, const Operation*o)const; // detect if all predeccesors of an operation are finished
	bool  finished(long l, const CDFGnode*o)const;// detect if a CDFG node is finished
	void  setSuccessorsExecutable(long l, const CDFGnode*source);
	void  setNodeExecutable(long l, const CDFGnode*node);
	long  critical(CDFGnode*n)const;
	long  maxCriticalPredecessors(CDFGnode*n)const;
	void  protectCriticalLoop(const Data*read_var);
	void  protectCriticalLoops(const Operation*o);
	void  resetAllMaxCriticalPredecessors(void);
	string blanks(long l)const
	{
		string s;
		for(long i = 0; i < l; i++)s += " ";
		return s;
	}

	// operator instances management
	OperatorInstance* getInstanceReady(const Operation*o, SchedulingOut*sos);
	OperatorInstance* allocateInstance(const Operation*o);
	OperatorInstance* getInstanceOrWait(const Operation*o, SchedulingOut*sos, bool*delayed_by_memory_constraint);

	// DATA
	const Switches*_sw;				// user switches
	CDFG*_cdfg;				// CDFG
	FunctionRefs		_used_functions;	// used functions of the CDFG
	SelectionOutRefs*_sels;				// selection results
	AllocationOut*_allocs;			// allocation results/stage
	const Lib*_lib; // CAA for chaining
	/* Caaliph 27/06/2007*/
	AllocationOutStep*_allocs_bis;			// add by Caaliph
	/* End Caaliph*/
	const Clock*_clk;				// system clock
	const Cadency*_cadency;			// user cadency constraint
	/* Caaliph 27/06/2007*/
	long				_max_cad;			//add by Caaliph
	long                _alloc_cycle_count; // Caaliph
	/* End Caaliph*/
	long				_time;				// current time
	long				_stage;				// current pipeline stage
	long				_stages;			// number of stages(from _alloc.size())
	long				_stageTimeBoundary;	// time limit of current stage
	SchedulingOut*_sos;				// to store results
	/* GL 26/04/2007 :	Replace fllowing line
	   operator is no more constant
	   vector<vector<const Operator*> > _dynamicStages; // to store the identity of the used operators in dynamic stages
	   By*/
	vector<vector<Operator*> > _dynamicStages; // to store the identity of the used operators in dynamic stages
	/* Fin GL*/
	const Mem*_mem;				// memory model
	MEMC				_memc;				// memory mapping constraints

public:
	//! Scheduling process constructor.
	//! The object contains all the local variables necessary
	//! to run the scheduling algorithms.
	//! @param sw Input. Is a pointer to the user switches.
	//! @param cdfg Inout. Is a pointer to the CDFG to schedule. The CDFG operations
	//! will be annotated with added infomations comming frol the scheduling process.
	//! All annotations will use the "_addr" extension fiels of the CDFG's operations.
	//! @param cadency Input. is a pointer to the cadency constraint.
	//! @param clk Input. Is a pointer to the system clock.
	//! @param mem Input; Is a pointer to the memory access time.
	//! @param sels Input. Is a pointer to the result of the selection process.
	//! @param allocs Input. Is a pointer to the result of the allocation process.
	//! @param sos. Output. Is a pointer to the object that will contain the scheduling results.
	//!
	//! This algorithm has been taken from
	//! "Convention de recherche n° 90 3B 055 00 790 92 45 CNS, Repport B, Noyau de l'outil de synthèse GAUT"
	//! Eric Martin, Jean-Luc Philippe, LASTI-ENSSAT, 1992
	//!
	//!	-# Saves pointers to local copies to avoid passing constant parameters.
	//!	-# Allocate scheduling-specific temporary memory to all functions, operators and CDFG nodes.
	//!	-# Build first list of executables operations, extracted from the CDGG source.
	//!	-# Schedule CDFG. Time starts at 0. This is a continuous loop of clock cycles
	//! and scheduling of operations ready to be executed on the allocated set of operators.
	//!	-# Free temporary memory allocated for functions and operators.
	/***Scheduling(
	  const Switches*sw,					// in
	  CDFG*cdfg,					// in
	  const Cadency*cadency,				// in
	  const Clock*clk,					// in
	  const Mem*mem,						// in
	  SelectionOutRefs*sels,				// in = the output of the multi-delay selection stage
	  AllocationOut*allocs,				// in = the output of the multi-function selection stage
	  SchedulingOut*sos					// out
	  );***/
	/* Caaliph 27/06/2007*/
	Scheduling(
			   const Switches*sw,					// in
			   CDFG*cdfg,					// in
			   const Cadency*cadency,				// in
			   long  max_cad,
			   const Clock*clk,					// in
			   const Mem*mem,						// in
			   SelectionOutRefs*sels,				// in = the output of the multi-delay selection stage
			   AllocationOut*allocs,				// in = the output of the multi-function selection stage
			   AllocationOutStep*allocs_bis,				// in = the output of the multi-function selection stage
			   SchedulingOut*sos					// out
			  );
	/* End Caaliph*/
	// Caaliph 03/12/2009
	Scheduling(

			   const Switches*sw,					// in
			   CDFG*cdfg,					// in
			   const Cadency*cadency,				// in
			   const Clock*clk,					// in
			   const Mem*mem,						// in
			   SelectionOutRefs*sels,				// in = the output of the multi-delay selection stage
			   AllocationOut*allocs,				// in = the output of the multi-function selection stage
			   const Lib*lib,
			   SchedulingOut*sos					// out
			  ); // End Caaliph
	//! Scheduling process constructor.
	//! Frees the temporary memory allocated to CDFG's nodes.
	~Scheduling();

};

#endif // __SCHEDULING_H__

// end of: scheduling.h


