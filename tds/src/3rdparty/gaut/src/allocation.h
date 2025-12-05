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

//	File:		allocation.h
//	Purpose:	GAUT optimisation of selection process with multi-function operators
//	Author:		Pierre Bomel, LESTER, UBS

class Allocation;

#ifndef __ALLOCATION_H__
#define __ALLOCATION_H__

#include <climits>
#include <iostream>
#include "cadency.h"
#include "cdfg.h"
#include "function.h"
#include "operator.h"
#include "switches.h"
#include "lib.h"
#include "selection.h"
using namespace std;

//! To represent functions with allocated operators.
class AllocFunction
{
public:
	const Function*		function;		//!< Identity of the function.
	Operator*		_oper;			//!< Mapped operator.
	double					rem;			//!< Parallelism remainder.
	bool					processed;		//!< Boolean toggle to tell when function is processed.
	bool					valid;			//!< To allocate only the good ones. Can be marked unvalid.
	AllocFunction(const Function*f, double r): function(f), _oper(0), rem(r), processed(false), valid(true) {}
	//! Get name of allocation.
	string name()const
	{
		return function->name();
	}
	//! Print info.
	void info()const
	{
		cout << "function " << name()<< endl;
		cout << "  remainder=" << rem << endl;
		cout << "  processed=" << processed << endl;
	}
	//! Set operator
	void oper(const Operator*o)
	{
		_oper = (Operator*)o;
	}
	//! Get operator
	const Operator* oper()const
	{
		return _oper;
	}
};

//! Smart pointer to AllocFunction.
class AllocFunctionRef  : public MapRef <AllocFunction>
{
public:
	AllocFunctionRef(AllocFunction*d): MapRef<AllocFunction>(d) {}
};

//! List of smart pointers to AllocFunction.
class AllocFunctionRefs : public MapRefs<AllocFunction, AllocFunctionRef>
{
public:
	//! Raw creation to empty list.
	AllocFunctionRefs() {}
	//! Copy build. This is an explicit duplication of all items.
	AllocFunctionRefs(const AllocFunctionRefs &src)
	{
		for(AllocFunctionRefs::const_iterator it = src.begin(); it != src.end(); it++)
		{
			AllocFunction*mf = (*it).second;
			AllocFunction*copy = new AllocFunction(*mf);	// real copy
			add(AllocFunctionRef(copy));
		}
	}
	//! Assign build. This is an explicit duplication of all items.
	AllocFunctionRefs & operator= (const AllocFunctionRefs &src)
	{
		clear();
		for(AllocFunctionRefs::const_iterator it = src.begin(); it != src.end(); it++)
		{
			AllocFunction*mf = (*it).second;
			AllocFunction*copy = new AllocFunction(*mf);	// real copy
			add(AllocFunctionRef(copy));
		}
		return*this;
	}
	//! Free. This is an explicit free of all items.
	~AllocFunctionRefs()
	{
		for(AllocFunctionRefs::iterator it = begin(); it != end(); it++)
		{
			AllocFunction*mf = (*it).second;
			delete mf;
		}
	}
	//! Get info.
	void info()const
	{
		for(AllocFunctionRefs::const_iterator it = begin(); it != end(); it++)
		{
			const AllocFunction*mf = (*it).second;
			mf->info();
		}
	}
};
/* Caaliph: utile to store the RRT 27/06/2007*/
class AllocationOutCycle
{
private:
	vector<const Operator*> _operators;		//!< For each allocated operator gives its identity.
	vector<const Operator*> _copy_operators;		//!< For each allocated operator gives its identity.
	//vector<const Function*> _functions;

public:
	//! Get vector of allocated operators.
	vector<const Operator*> & operators()
	{
		return _operators;
	}
	/*void operators(vector<const Operator*> opr) {
	  for(int i=0; i<opr.size(); i++) {
	  _operators; }*/
	vector<const Operator*> & copy_operators()
	{
		return _copy_operators;
	}
	//! Get number of operators
	long size()const
	{
		return _operators.size();
	}
	long copy_size()const
	{
		return _copy_operators.size();
	}
	//! Bind a function to an operator instance.
	//! @param f Input. Is a pointer to the function to bind.
	////////void assign(const Function* f) {_functions = f; }
	//! Get function.
	///vector<const Function*> & functions() {return _functions; }

};
class AllocationOutStep
{
private:
	//ALLOCMODIF
	vector<AllocationOutCycle> _allocationOutCycle;
public:
	//! Get fff
	vector<AllocationOutCycle> & allocationOutCycle()
	{
		return _allocationOutCycle;
	}
};
/*End Caaliph*/

//! Allocated operators for a pipeline stage.
//! This is a set of two vectors: allocated operators and use rates/operator.
class AllocationOutStage
{
private:
	//! List of functions to process.
	//! This data member is a working area, cleared after allocation.
	AllocFunctionRefs		_functions;
public:
	// allocated operators are stored in a vector
	// only this part is the result of the allocation process
	vector<double>			_use_rates;		//!< For each allocated operator gives its use rate.
	vector<const Operator*> _operators;		//!< For each allocated operator gives its identity.
	/* Caaliph: 27/06/2007*/
	vector<AllocationOutCycle> _allocationOutCycle;

public:
	//! Get AllocationOutCycle
	vector<AllocationOutCycle> & allocationOutCycle()
	{
		return _allocationOutCycle;
	}
	/* End Caaliph*/
	//! Get vector of allocated operators.
	vector<const Operator*> & operators()
	{
		return _operators;
	}
	//! Get vector of use rates of allocated operators.
	vector<double> & use_rates()
	{
		return _use_rates;
	}
	//! Get smart list of functions still to process.
	AllocFunctionRefs & functions()
	{
		return _functions;
	}
	//! Compute cost of solution.
	long cost()const
	{
		long c = 0;
		if(_operators.size() == 0)return LONG_MAX;
		for(int i = 0; i < _operators.size(); i++)c += _operators[i]->area();
		return c;
	}

	//! Search if there is at least a non-processed function with a non-null remainder.
	bool parallelismRateRemainder()const
	{
		for(AllocFunctionRefs::const_iterator it = _functions.begin(); it != _functions.end(); it++)
		{
			const AllocFunction*mf = (*it).second;
			if(!mf->processed &&(mf->rem != 0.0))return true;
		}
		return false;
	}
	//! Print info.
	void info()const
	{
		int i;
		for(i = 0; i < _use_rates.size(); i++)
		{
			cout << "  instance #" << i;
			cout << "(operator=" << _operators[i]->name();
			cout << ",use=" << _use_rates[i] << ")" << endl;
		}
		//_functions.info();	// for debug only
	}
	//! Get number of operators
	long size()const
	{
		return _operators.size();
	}

};

//! Output of the allocation process.
//! This a vector of AllocationOutStage. Each index is the id of a pipeline stage.
class AllocationOut : public vector<AllocationOutStage>
{
public:
	//! Get number of operators
	long operators_noPassThrough()const
	{
		long s = 0;
		for(vector<AllocationOutStage>::const_iterator it = begin(); it != end(); it++)
		{
			const AllocationOutStage*stage = &(*it);
			for(int i=0; i<((AllocationOutStage*)stage)->operators().size(); i++)
			{
				const Operator*oper = ((AllocationOutStage*)stage)->operators()[i];
				if(!oper->passThrough())s++;
			}
		}
		return s;
	}
	//! Get number of operators
	long operators()const
	{
		long s = 0;
		for(vector<AllocationOutStage>::const_iterator it = begin(); it != end(); it++)
		{
			const AllocationOutStage*stage = &(*it);

			s += stage->size();
		}
		return s;
	}
};

//! The allocation process.
//! The selection process estimated a number of operators per
//! function and per pipeline stages. The allocation process
//! reuses this estimation and tries to place parallelism
//! remainders on multi-function operators. If there are no
//! multi-function operators, ceil(parallelism rates)is
//! the number of operators that will be allocated for
//! each function and for each pipeline stage.
class Allocation
{
private:
	static bool _debug;							//!< Local debug toggle.
	// inputs
	const SelectionOutRefs*	_sos;			//!< Local copy of sos.
	const Clock*				_clk;			//!< Local copy of sysclk.
	// working data
	int							_stages;		//!< Number of pipeline stages.
	int							_stage;			//!< Current processing stage.
	// outputs
	AllocationOut*				_best;			//!< Local copy of alloc.
	long						_level;			//!< Recursion level for debug.

	//! Optimization of a pipeline stage.
	//! @param Input. level is an indentation level for debug trace.
	//! @param Input. in is the current AllocationOutStage to optimize.
	void stageOptimize(int level, AllocationOutStage &in);
	//! Optimization of a function.
	//! @param Input. level is an indentation level for debug trace.
	//! @param Input. in is the current AllocationOutStage to optimize.
	//! @param Input. function is a pointer to the function to optimize.
	void functionOptimize(int level, AllocationOutStage &in, const Function*function);
	//! Try an operator.
	//! @param Input. level is an indentation level for debug trace.
	//! @param Input. in is the current AllocationOutStage to optimize.
	//! @param Input. function is a pointer to the function to optimize.
	//! @param Input. op is a pointer to the operator to try.
	void tryop(int level, AllocationOutStage &in, const Function*function, const Operator*op, AllocFunction*mf);
	//! Manual allocation
	void manualAllocation(AllocationOut*alloc, const Switches*sw);
	//! Print Summary
	void summary(AllocationOut*alloc);

public:

	//! Constructor of the allocation Process.
	//
	//! @param Input. sw is a pointer to the user switches.
	//! @param Input. sysclk is a pointer to the system clock.
	//! @param Input. lib is a pointer to the library.
	//! @param Input. cdfg is a pointer to the CDFG.
	//! @param Input. sos is a pointer to the selection results.
	//! @param Output. alloc is a pointer to the allocation produced.
	Allocation(
			   const Switches*sw,							// in
			   const Clock*sysclk,						// in
			   const Lib*lib,								// in
			   const CDFG*cdfg,							// in
			   const SelectionOutRefs*sos,				// in
			   AllocationOut*alloc					// out
			  );

	//! Get number of pipeline stages.
	long stages()const
	{
		return _stages;
	}
};

#endif // __ALLOCATION_H__

