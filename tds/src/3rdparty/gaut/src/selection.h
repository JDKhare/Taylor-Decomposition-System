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

//	File:		selection.h
//	Purpose:	Selection process
//	Author:		Pierre Bomel, LESTER, UBS

class Selection;
class SelectionOut;
class SelectionOutRef;
class SelectionOutRefs;

#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "switches.h"
#include "cdfg.h"
#include "function.h"
#include "parser.h"
#include "clock.h"
#include "lib.h"
#include "map.h"
#include "cadency.h"
#include "mem.h"
using namespace std;

//! A smart pointer to a SelectionOut.
class SelectionOutRef  : public MapRef <SelectionOut>
{
public:
	SelectionOutRef(SelectionOut *d) : MapRef<SelectionOut>(d) {}
};

//! A list of smart pointers to SelectionOut.
class SelectionOutRefs : public MapRefs<SelectionOut, SelectionOutRef>
{
private:
	long _cadency;			//!< User cadency.
	long _pipelineStages;	//!< Number of pipeline stages.
	long _cost;				//!< Area cost.
	//! Critical loop.
	//!	A critical loop is the maximum time between a read and a write of a loopback variable.
	long _criticalLoop;
	bool _valid;			//!< Tells if the mapping is valid or not.
	static bool _debug;		//!< Class debug toggle.
public:
	//! Duplicate SelectionOut items.
	//! @param res is the destination SelectionOutRefs object
	void duplicate(SelectionOutRefs &res) const;
	//! Delete selectionOut items.
	void del();
	//! Set cadency.
	void cadency(long cadency)
	{
		_cadency = cadency;
	}
	//! Set area cost.
	void cost(long cost)
	{
		_cost = cost;
	}
	//! Set number of pipeline stages.
	void pipelineStages(int stages);
	//! Set critical loop.
	void criticalLoop(long criticalLoop)
	{
		_criticalLoop = criticalLoop;
	}
	//! Unvalid me.
	void isNotValid()
	{
		_valid = false;
	};
	

	//! Unvalid me.
	void isValid()
	{
		_valid = true;
	};

	//! Get cadency.
	//! @result the cadency.
	long cadency() const
	{
		return _cadency;
	}
	//! Get area cost.
	//! @result the area cost.
	long cost() const
	{
		return _cost;
	}
	//! Get number of pipeline stages.
	//! @result the number of pipeline stages.
	long pipelineStages() const
	{
		return _pipelineStages;
	}
	//! Get critical loop.
	long criticalLoop() const
	{
		return _criticalLoop;
	}
	//! Check validity.
	// @result validity status.
	bool valid() const
	{
		return _valid;
	}
	//! Search a function.
	//! @param Input f is a pointer to the function searched.
	//! @param Output. m is a double pointer on the found SelectionOut (if any).
	//! @result true if SelectionOut found with matching function, else fales.
	bool searchFunction(const Function *f, const SelectionOut **m) const;
	//! @param Input o is a pointer to the operator searched.
	//! @param Output. m is a double pointer on the found SelectionOut (if any).
	//! @result true if SelectionOut found with matching operator, else fales.
	bool searchOperator(const Operator *o, const SelectionOut **m) const;
	//! Prints contents of the list at console in a human readable way. For debug.
	void info() const;
	//! Prints contents of the list at console in a human readable way. For debug.
	//! @param Input. only_valid when true tells "info" to print only valid selections.
	void info(bool only_valid) const;
	//! Serializes into a file.
	//! @param Output. f is an output stream.
	//! @param Input. index is a numeric ID that identifies the selection list.
	void serialize(ostream &f, int index) const;
	//! Serializes into a file.
	//! @param Output. f is an output stream.
	//! @param Output. name is the name of the file into which to serialize.
	//! @param Input. index is a numeric ID that identifies the selection list.
	void serialize(const string &name, int index) const;
};

#include "function.h"
#include "operator.h"

//! A function/operator selection.
class SelectionOut
{
private:
	// generated by the selection process
	Function *_func;				//!< Is a pointer to the function to implement.
	const Operator *_oper;				//!< Is a pointer to selected operator implementing the function.
	long		_cycles;			//!< Is the number of cycles for the selected operator.
	long		_length;			//!< Is the length in time units for the selected operator.
	// generated by the mapping cost computation process
	vector<long>	_operations;		//!< Is the number of operations per pipeline stage.
	vector<double>	_parallelismRate;	//!< Is the parallelism rate per pipeline stage.
	vector<long>	_operators;			//!< Is the number of operators per pipeline stage.
	vector<long>	_area;				//!< Is the area of operators per pipeline stage.
public:
	SelectionOut(Function *func, const Operator *oper, const Clock *clk);			// for generated mappings
	SelectionOut(const SelectionOut &src);
	SelectionOut & operator=(const SelectionOut & src)
	{
		cerr << "Internal error: copying a Mapping" << endl;
		exit(1);
	}
	~SelectionOut()
	{
#ifdef CHECK
		SelectionOut_delete++;
		//cout << "Mapping delete" << endl; info();
#endif
	}
	//! Get Function.
	//! @result a pointer to the function to implement.
	Function *func() const
	{
		return _func;
	}
	//! Get operator.
	//! @result a pointer to the operator selected to implement the function.
	const Operator *oper() const
	{
		return _oper;
	}
	//! Get name.
	//! @result a unique string for the function/operator selection item.
	string name() const;
	//! Print info.
	//! Prints contents of the object at console in a human readable way. For debug.
	void info() const;
	//! Get cycles.
	//! @result number of cycles for the implementing operator.
	long cycles() const
	{
		return _cycles;
	}
	//! Get length.
	//! @result length.
	long length() const
	{
		return _length;
	}
	//! Clear pipeline data.
	// Clear vectors of operations, operators, parallelism rates and area costs.
	void clear()
	{
		_operations.clear();
		_operators.clear();
		_parallelismRate.clear();
		_area.clear();
	}
	//! Get operations.
	//! @result a vector of operations per stage.
	vector<long> & operations()
	{
		return _operations;
	}
	//! Get operators.
	//! @result a vector of operators per stage.
	vector<long> & operators()
	{
		return _operators;
	}
	//! Get parallelism rate.
	//! @result a vector of parallelism rates per stage.
	vector<double> & parallelismRate()
	{
		return _parallelismRate;
	}
	//! Get parallelism rate.
	//! @result a constant vector of parallelism rates. Each index represents a stage.
	const vector<double> & parallelismRate() const
	{
		return _parallelismRate;
	}
	//! Get area cost.
	//! @result a vector of area costs.
	vector<long> & area()
	{
		return _area;
	}
	//! Serializes into a file.
	//! @param Output. f is an output stream.
	//! @param Input. global_no is the global number of the SelectionOut.
	//! @param Input. local_no.is the local number of the SelectionOut.
	void serialize(ostream &f, int global_no, long local_no) const;
};

//! The selection process.
//! The selection process consists in selecting operators in the library to implement
//! the functions used in the CDFG. This selection is based on a rough ASAP/ALAP
//! scheduling of the operations in order to compute average parallelism rates per
//! pipeline stages. Then operators are selected. The best selection is the one
//! that satisfies the cadency constraints and minimizes the operator area cost.
class Selection
{
private:

	//! Local debug toggle
	static bool _debug;

	//! Build a SelectionOutRefs from parsed data.
	//! @param Input. objs is a pointer to parsed objects.
	//! @param Output. mappings is a pointer to the vector of SelectionOutRefs to build.
	void convert_to_SelectionOuts(
	    const ParseObjects *objs,					// in (parsed data)
	    vector<SelectionOutRefs> *mappings	// out
	);

	//! Run an ASAP followed by an ALAP on the CDFG.
	//! @param Input. mdos is a pointer to a selection result.
	//! @result Returns true if scheduling possible, false otherwise.
	//! Due to user constraints, the scheduling might be impossible. If
	//! this is the case GAUT stops and prints an error message telling
	//! the user what constraint caused the scheduling to fail.
	bool ASAP_ALAP(
	    const SelectionOutRefs *mdos				// in
	);

	//! Parse a file to generate all selection alternatives.
	//! @param Input. name is the name of the file containing the selection(s).
	//! @param Output. mappings is a pointer to the vector of SelectionOutRefs to build.
	//! @see convert_to_SelectionOuts
	void generate_alternatives(
	    const string &name,							// in
	    vector<SelectionOutRefs> *mappings	// out
	);

	//! Generate all alternatives from the CDFG.
	//! @param Input. used_functions is the list of the functions used in the CDFG.
	//! @param Output. mappings is a pointer to the vector of SelectionOutRefs to build.
	void generate_alternatives(
	    const FunctionRefs *used_functions,			// in
	    vector<SelectionOutRefs> *mappings	// out
	);

	//! Compute the cost of one alternative.
	//! @param Inout. mapping is a pointer to a SelectionOutRefs.
	//! It updates the criticalLoop, the cadency, the pipelineStages, the area cost of
	//! the selection but also the asap and alap dates of the CDFG.
	//! @result true if alternative is valid, else mark it unvalid and return false.
	bool compute_cost_of_alternative(
	    SelectionOutRefs *mapping				// in-out
	);

	//! Compute the cost of all alternatives.
	//! @param Input. mappings is a pointer to a vector of SelectionOutrefs.
	void compute_cost_of_alternatives		(
	    vector<SelectionOutRefs> *mappings	// in-out
	);

	//! Select the best alternative amoung all computed ones and update CDFG consequently.
	//! @param Input. mappings is a pointer to a vector of lists of smart pointers to SelectionOut.
	//! @param Output. map is a pointer to the result: a list of smart pointers to SelectionOut.
	//! @result the index of the list selected in the vector.
	int select_best_alternative_and_update_cdfg(
	    const vector<SelectionOutRefs> *mappings,	// in
	    SelectionOutRefs *map					// out
	);

	//! Get number of operators per pipeline stages.
	//! @param Input. mdos is a pointer to a SelectionOutRefs.
	//! @param Input. Op is a pointer to the operator to search for.
	//! @param Output. operations is a pointer to a vector of numbers of operators.
	//! Each element of the array has an index which is the stage number, and
	//! each element value is the number of operators "Op" in the stage.
	void numberOfOperatorsPerPipelineStage(
	    const SelectionOutRefs *mdos,				// in
	    const Operator *Op,							// in
	    vector<long> *operations				// out
	);

	const Switches *_sw;							//!< Local copy of sw.
	const Cadency *_cadency;						//!< Local copy of cadency.
	const Mem *_mem;								//!< Local copy of mem.
	const Clock *_sysclk;							//!< Local copy of sysclk.
	const Lib *_lib;								//!< Local copy of lib.
	CDFG *_cdfg;								//!< Local copy of cdfg.

public:

	//! Constructor of the object representing the selection process.
	//! @param Input. sw is a pointer to the user switches.
	//! @param Input. cadency is a pointer to the cadency.
	//! @param Input. mem is a pointer to the memory access time.
	//! @param Input. sysclk is a pointer to the system clock.
	//! @param Input. lib is a pointer to the library.
	//! @param Inout. cdfg is a pointer to the CDFG.
	//! .
	//! The CDFG is modified because the selection process relies on a ASAP_ALAP
	//! scheduling of its operations. It is used as a temporary variable. Only
	//! asap and alap dates are modified. Final scheduling of the CDFG
	//! will be accomplished by the scheduling-binding process.
	//! @param Ouput. map is a pointer to the result object that will contains
	//! the best selection.
	Selection(
	    const Switches *sw,							// in
	    const Cadency *cadency,						// in
	    const Mem *mem,								// in
	    const Clock *sysclk,						// in
	    const Lib *lib,								// in
	    CDFG *cdfg,							// in-out
	    SelectionOutRefs *map					// out
	);
};

#endif // __SELECTION_H__

// end of: selection.h

