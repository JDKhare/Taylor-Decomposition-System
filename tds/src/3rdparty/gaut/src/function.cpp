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

//	File:		function.cpp
//	Purpose:	functions
//	Author:		Pierre Bomel, LESTER, UBS

#include <string>
#include <math.h>
#include "function.h"
#include "operator.h"
using namespace std;

// Function dump
void Function::info() const
{
	cout << "Function: " << name() << endl;
	/* GL 20/11/07 */
	cout << "  Symbolic Function: " << symbolic_function() << endl;
	/* Fin GL */
	cout << "  input  ports = ";
	PortRefs::const_iterator it;
	for (it = _inputs.begin(); it != _inputs.end(); it++)
	{
		((*it).second)->info();
		cout << " ";
	}
	cout << endl;
	cout << "  output ports = ";
	for (it = _outputs.begin(); it != _outputs.end(); it++)
	{
		((*it).second)->info();
		cout << " ";
	}
	cout << endl;
	cout << "  implemented by";
	if (_implemented_by.size() == 0)
		cout << " no implementation operators" << endl;
	else for (OperatorRefs::const_iterator it = _implemented_by.begin(); it != _implemented_by.end(); it++)
		{
			OperatorRef op_ref = (*it).second;
			cout << " " << op_ref->name();
		}
	cout << endl;
}

// Function serialize
void Function::serialize(ofstream &f) const
{
	f << "Function: " << name() << "\n";
	f << "  input  ports = ";
	PortRefs::const_iterator it;
	for (it = _inputs.begin(); it != _inputs.end(); it++)
	{
		((*it).second)->serialize(f);
		cout << " ";
	}
	f << "\n";
	f << "  output ports = ";
	for (it = _outputs.begin(); it != _outputs.end(); it++)
	{
		((*it).second)->serialize(f);
		cout << " ";
	}
	f << "\n";
	f << "  implemented by";
	if (_implemented_by.size() == 0)
		f << " no implementation operators" << "\n";
	else for (OperatorRefs::const_iterator it = _implemented_by.begin(); it != _implemented_by.end(); it++)
		{
			OperatorRef op_ref = (*it).second;
			f << " " << op_ref->name();
		}
	f << "\n";
}

bool Function::passThrough() const
{
	for (OperatorRefs::const_iterator it = _implemented_by.begin(); it != _implemented_by.end(); it++)
	{
		OperatorRef op_ref = (*it).second;
		if (!op_ref->passThrough()) return false;
	}
	return true;
}

Function::~Function()
{
	PortRefs::const_iterator it;
	for (it = _inputs.begin(); it != _inputs.end(); it++) delete ((*it).second);
	for (it = _outputs.begin(); it != _outputs.end(); it++) delete ((*it).second);
#ifdef CHECK
	function_delete++;
#endif
}

// Ports management
void Function::addPort(const string &name, bool input, int pos)
{
	if (input)	_inputs.add(PortRef(new Port(name, Port::IN, pos, this)));
	else		_outputs.add(PortRef(new Port(name, Port::OUT, pos, this)));
}
const Port * Function::getInputPortByPos(int pos) const
{
	for (PortRefs::const_iterator it = _inputs.begin(); it != _inputs.end(); it++)
		if (((*it).second)->pos() == pos) return (*it).second;
	return 0;
}
const Port * Function::getOutputPortByPos(int pos) const
{
	for (PortRefs::const_iterator it = _outputs.begin(); it != _outputs.end(); it++)
		if (((*it).second)->pos() == pos) return (*it).second;
	return 0;
}
const Port * Function::getInputPortByName(const string &name) const
{
	for (PortRefs::const_iterator it = _inputs.begin(); it != _inputs.end(); it++)
		if (((*it).second)->name() == name) return (*it).second;
	return 0;
}
const Port * Function::getOutputPortByName(const string &name) const
{
	for (PortRefs::const_iterator it = _outputs.begin(); it != _outputs.end(); it++)
		if (((*it).second)->name() == name) return (*it).second;
	return 0;
}
const Port * Function::getPortByName(const string &name) const
{
	const Port * p = getInputPortByName(name);
	if (!p) p = getOutputPortByName(name);	// second chance
	return p;
}
int Function::numberOfMonoOperators() const
{
	int n = 0;
	for (OperatorRefs::const_iterator it = _implemented_by.begin(); it != _implemented_by.end(); it++)
	{
		const Operator *o = (*it).second;
		if (o->mono()) n++;
	}
	return n;
}
/* GL 25/10/07 : combinational_delay */
long Function::getCombinationalDelayMax() const
{
	long delay = 0;
	for (OperatorRefs::const_iterator it = _implemented_by.begin(); it != _implemented_by.end(); it++)
	{
		const Operator *o = (*it).second;
		if (o->getCombinationalDelay(this) >= delay) delay = o->getCombinationalDelay(this);
	}
	return delay;
}
long Function::getCombinationalDelayMin() const
{
	long delay = 0;
	for (OperatorRefs::const_iterator it = _implemented_by.begin(); it != _implemented_by.end(); it++)
	{
		const Operator *o = (*it).second;
		if (o->getCombinationalDelay(this) <= delay) delay = o->getCombinationalDelay(this);
	}
	return delay;

}
/* Fin GL */
// end of: function.cpp
