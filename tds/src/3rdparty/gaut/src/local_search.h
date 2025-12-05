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

//	File:		local_search.h
//	Purpose:	local_search
//	Author:		Kods Trabelsi, LESTER, UBS




#ifndef __LOCAL_SEARCH_H__
#define __LOCAL_SEARCH_H__

#include <math.h>
#include "function.h"
#include "operator.h"
#include "switches.h"
#include "cdfg.h"
#include "scheduling.h"
#include "reg.h"
#include "graph.h"
#include "operation.h"
#include "port.h"
#include "resizing.h"
using namespace std;








struct Dico_port
{
	string name;
	int oper_index;
};
typedef vector<struct Dico_port> DICO_PORTS;


class Type
{

public:

	const Function*function;
	vector <Operation*> operations;
	bool checked;						//!< true if all operations are checked	on the local_search step
	int pos_source;
	int pos_destination;
};


class Cycle
{

public:

	int id;
	vector <Type*> types;
	bool checked;						//!< true if all operations are checked	on the local_search step

};


//! Cycles is a vector of Cycle. A lists of operation for each cycle
class Cycles : public vector<Cycle*> {};



class Solution
{

private:

	long _operators_cost;				//!< Cost of operators units.
	long _registers_cost;				//!< Cost of registers.
	long _interconections_cost;		//!< Cost of interconnections units.
	long _total_cost;					//!< Cost of architecture.

	const Switches*_sw;				//!< Local copy of sw.
	const Cadency*_cadency;			//!< Local copy of cadency.
	const Clock*_clk;					//!< Local copy of clk.
	RegOut*_regs;						//!< Local copy of regs.

public:

	vector< Data*> datas;
	vector< Operation*> _operations;
	short**_datas_compatibility;
	short**_ops_compatibility;

	OI_Lists _oi_lists;
	DICO_PORTS _dico_ports;
	short**_port_reading_the_var;
	short**_oper_writing_the_var;
	short**_score_array_port_var;
	short**_score_array_oper_var;
	int _mux_list[200];

	bool all_reg_cheked;
	bool all_cycles_cheked;
	bool all_operations_cheked;
	int pos;
	static bool _debug;
	bool delete_reg;
	bool nothing_changed;

	Reg* reg_source;
	Reg* reg_destination;
	Data* d;
	Operation* o1;
	Operation* o2;
	OperatorInstance*inst1;
	OperatorInstance*inst2;


	// Getter
	const Switches*sw()
	{
		return _sw;
	};
	const Cadency*cadency()
	{
		return _cadency;
	};
	const Clock*clk()
	{
		return _clk;
	};
	RegOut*regs()const
	{
		return _regs;
	};
	long total_cost()
	{
		return _total_cost;
	}
	long operators_cost()
	{
		return _operators_cost;
	};
	long registers_cost()
	{
		return _registers_cost;
	};
	long interconections_cost()
	{
		return _interconections_cost;
	};

	// Setter
	void interconections_cost(long interconections_cost)
	{
		_interconections_cost = interconections_cost;
	};
	void registers_cost(long registers_cost)
	{
		_registers_cost = registers_cost;
	};
	void operators_cost(long operators_cost)
	{
		_operators_cost=operators_cost;
	};
	void total_cost(long total_cost)
	{
		_total_cost = total_cost;
	};
	void regs(RegOut*regs)
	{
		_regs = regs;
	};

	// Costing the solution
	void costing();
	void compute_interconnections_cost();

	//! Constructor of the object representing the costing process.
	//! Cost of architecture(computing units + registers + interconnection units)
	Solution(
			 const Switches*sw,
			 const Cadency*cadency,
			 const Clock*clk,
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
			 short**ops_compatibility);

	~Solution()
	{
#ifdef CHECK
		Solution_delete++;
#endif
		delete _port_reading_the_var;
		delete	_oper_writing_the_var;
		delete	_score_array_port_var;
		delete	_score_array_oper_var;
		delete	_datas_compatibility;
		delete	_ops_compatibility;
		//delete	_regs;
		//delete	_oi_lists;
		//delete _dico_ports;
	}
};






class Local_search
{

private:

	Solution*_initial_sol;
	Solution*_sol_ref;
	Solution*_sol_neighbour;

	const Switches*_sw;				//!< Local copy of sw.
	const Cadency*_cadency;			//!< Local copy of cadency.
	const Clock*_clk;					//!< Local copy of clk.
	const SchedulingOut*_sched;		//!< Local copy of sched.
	RegOut*_regs;						//!< Local copy of regs.
	CDFG*_cdfg;						//!< Local copy of cdfg.
	OI_Lists _oi_lists;


public:

	Cycles cycles;
	vector< Data*> datas;
	vector< Operation*> operations;

	//!Constructor of the object: local search process.
	Local_search(const Switches*sw,			// in
				 const Cadency*cadency,				// in
				 const Clock*clk,					// in
				 const SchedulingOut*sched, // in
				 RegOut*regs,
				 CDFG*cdfg);

	//Getter
	Solution*initial_solution()
	{
		return _initial_sol;
	};
	Solution*sol_ref()
	{
		return _sol_ref;
	};
	Solution*sol_neighbour()
	{
		return _sol_neighbour;
	};

	//Setter
	void initial_solution(Solution*sol)
	{
		_initial_sol= sol;
	};
	void sol_ref(Solution*sol)
	{
		_sol_ref = sol;
	};
	void sol_neighbour(Solution*sol)
	{
		_sol_neighbour = sol;
	};

	static bool _debug;
	bool no_more_move;

	DICO_PORTS dico_ports;
	short**datas_compatibility;
	short**ops_compatibility;
	short**port_reading_the_var;
	short**oper_writing_the_var;

	short**score_array_port_var;
	short**score_array_oper_var;

	void initialize(CDFG*cdfg, OI_Lists*oi_lists);



};

#endif
// __LOCAL_SEARCH_H__



