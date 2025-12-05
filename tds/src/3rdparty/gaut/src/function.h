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

//	File:		function.h
//	Purpose:	Functions.
//	Author:		Pierre Bomel, LESTER, UBS

class Function;
class FunctionRef;
class FunctionRefs;

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <iostream>
#include <string>
using namespace std;
#include "map.h"

//! Smart pointer on a Function.
class FunctionRef		: public MapRef<Function>
{
public:
	FunctionRef(Function *f) : MapRef<Function>(f) {}
};
//! List of smart pointers to Functions.
class FunctionRefs : public MapRefs<Function, FunctionRef>
{
};

#include "operator.h"
#include "port.h"
#include "os_tools.h"
#include "check.h"

//! Functions.
class Function : public MapID
{
private:
	PortRefs		_inputs;				//!< each function has input ports
	PortRefs		_outputs;				//!< each function has output ports
	OperatorRefs	_implemented_by;		//!< each function is implemented by a set of operators
	/* GL 03/05/07	: Add a symbolic function */
	/*const*/
	string			_symbolic_function;		//!< each function has a symbolic function describing its functionnality
	/* Fin GL */
	// EJ 07/07/2008 : levinson
	bool			_mem_access;				//!< if true, operation is mem_read or mem_write (dynamic memory)
	// End EJ

public:
	void *_addr;	//!< for extensions
	//! Create a new function.
	//! @param Input. name is the uniqu eof the Function.
	Function(const string & name) : MapID(name)
	{
		_addr = 0;
		_mem_access = false; // EJ 07/07/2008 : levinson
		_symbolic_function = ""; // EJ 07/07/2008
#ifdef CHECK
		function_create++;	// debug
#endif
	}
	~Function();

	//! Add a symbolic port to a function.
	//! @param Input. name is the unique name of the port.
	//! @param Input. input is a boolean, true = input port, false = output port.
	//! @param Input. pos is the position of the port (0..N-1).
	void addPort(const string &name, bool input, int pos);

	// EJ 07/07/2008 : levinson
	//! if true, operation is mem_read or mem_write (dynamic memory)
	void mem_access(bool b)
	{
		_mem_access = b;
	}
	bool mem_access() const
	{
		return _mem_access;
	}
	// End EJ

	// get info
	//! Get number of inputs.
	int inputs() const
	{
		return _inputs.size();
	}
	//! Get number of outputs.
	int outputs() const
	{
		return _outputs.size();
	}
	//! Get number of operands = get number of inputs.
	int operands() const
	{
		return inputs();
	}
	//! Get number of mono-function operators.
	int numberOfMonoOperators() const;
	//! Get number of multi-functions operators.
	int numberOfMultiOperators() const
	{
		return numberOfImplementingOperators()-numberOfMonoOperators();
	}
	//! Get total number of implementing operators.
	int numberOfImplementingOperators() const
	{
		return _implemented_by.size();
	}
	//! Get input ports.
	PortRefs & inputPorts()
	{
		return _inputs;
	}
	//! Get output ports.
	PortRefs & outputPorts()
	{
		return _outputs;
	}
	//! Get input ports by position.
	//! @param Input. pos is the position of the port (0..N-1).
	const Port * getInputPortByPos(int pos) const;
	//! Get output ports by position.
	//! @param Input. pos is the position of the port (0..N-1).
	const Port * getOutputPortByPos(int pos) const;
	//! Get input port by name.
	//! @param Input. name is the unique name of the port.
	const Port * getInputPortByName(const string &name) const;
	//! Get output port by name.
	//! @param Input. name is the unique name of the port.
	const Port * getOutputPortByName(const string &name) const;
	//! Get port by name.
	//! @param Input. name is the unique name of the port.
	const Port * getPortByName(const string &name) const;
	//! Add an operator to the implementation list.
	//! Remember: one can only add operators in the set, never delete any.
	//! @param Input. o is a smart pointer to an Operator.
	void implemented_by(const OperatorRef o)
	{
		_implemented_by.add(o);
	}
	//! Get set of implementing operators.
	//! To avoid the caller to modify the operator set, the returned value is "const"
	OperatorRefs & implemented_by()
	{
		return _implemented_by;
	}
	//! Get list of smart pointers to Operators.
	const OperatorRefs & implemented_by() const
	{
		return _implemented_by;
	}
	//! Print info.
	void info() const; // see function.cpp
	//! Serialize into a file.
	void serialize(ofstream &f) const;
	//! Check if the function is a "pass-through" one.
	//! The function is a "pass-through" one if all its
	//! implementing operators are "pass-through" ones.
	bool passThrough() const;
	/* GL 03/05/07	: Add a symbolic function */
	//!< Get symbolic function name
	string symbolic_function() const
	{
		return _symbolic_function;
	}
	void symbolic_function(string name)
	{
		_symbolic_function = name;
	}
	/* Fin GL */
	/* GL 25/10/07 : combinational_delay */
	//!< Get maximun combinatorial delay
	long getCombinationalDelayMax() const;
	//!< Get minimum combinatorial delay
	long getCombinationalDelayMin() const;
	/* Fin GL */
};
#endif // __FUNCTION_H__
