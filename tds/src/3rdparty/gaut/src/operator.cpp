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

//	File:		operator.cpp
//	Purpose:	operators
//	Author:		Pierre Bomel, LESTER, UBS

#include "operator.h"
using namespace std;

// ImplementedFunction class ---------------------------------------------------------

void ImplementedFunction::info() const
{
	cout << "  implementing function " << _function->name() << endl;
	cout << "    propagation time = " << _propagation_time << endl;
	cout << "    power            = " << _power << endl;
}

void ImplementedFunction::serialize(ofstream &f) const
{
	f << "  implementing function " << _function->name() << "\n";
	f << "    propagation time = " << _propagation_time << "\n";
	f << "    power            = " << _power << "\n";
}

void ImplementedFunctionRefs::info() const
{
	for (const_iterator it = begin(); it != end(); it++) (*it)->info();
}

void ImplementedFunctionRefs::serialize(ofstream &f) const
{
	for (const_iterator it = begin(); it != end(); it++) (*it)->serialize(f);
}

// Operator class ---------------------------------------------------------

long Operator::propagation_time(const Function * f) const
{
	int n = _functions.size();
	if (n!=0)
	{
		if (n == 1) // mono-function operator
			return _functions[0]->propagation_time();
		// else multi-function operators
		for (int i = 0; i < n; i++) if (_functions[i]->function() == f) // only when operator implements f !
				return _functions[i]->propagation_time();
		// else error
	}
	cerr << "Internal error: operator '" << name() << "' does not implement function '" << f->name() << "'" << endl;
	exit(1);
}

bool Operator::compatible(const Operator *o, const Function *f, const Clock *clk) const
{
	//cout << "me=" << name() << " him=" << o->name() << " function=" << f->name() << " period=" << clk->period() << endl;
	//if (o->isMonoFunction()) return false;	// o must be a multi-function operator
	const ImplementedFunction *imf;
	if (!o->implements(f, &imf)) return false;	// o must implement the same function
	//cout << "OK implement same function " << f->name() << endl;
	if (o->pipelineStages() != pipelineStages()) return false; // must have the same # of pipeline stages
	//cout << "OK pipelineStages=" << o->pipelineStages() << endl;
	// for combinatorial ops, pipelineStages are both equal to 0
	long mycycles = clockCycles(f, this, clk);
	//cout << "my clock cycles=" << mycycles << endl;
	long hiscycles = clockCycles(f, o, clk);
	//cout << "his clock cycles=" << hiscycles << endl;
	if (mycycles != hiscycles) return false; // must have the same time
	//cout << "OK" << endl;
	// all conditions are met now: OK !
	return true;
}

//! Delete of an operator. Contains an explicit delete of all its ports.
Operator::~Operator()
{
	// free ports explicitely

	//DH trace debug cout << "Operator detruction" << endl;
	int i;
	for (i = 0; i < _in_ports.size(); i++) delete _in_ports[i];
	for (i = 0; i < _out_ports.size(); i++) delete _out_ports[i];
#ifdef CHECK
	operator_delete++;	// debug
#endif
}

void Operator::addPort(long i, const Port *p)
{
	if (p->way() == Port::IN)
	{
		if ((i < 0) || (i >= _in_ports.size()))
		{
			cerr << "Internal error: out of bound access to input ports" << endl;
			exit(1);
		}
		_in_ports[i] = p;
	}
	else
	{
		if ((i < 0) || (i >= _out_ports.size()))
		{
			cerr << "Internal error: out of bound access to output ports" << endl;
			exit(1);
		}
		_out_ports[i] = p;
	}
}

void Operator::info() const
{
	long i;
	cout << "Operator " << name() << endl;
	cout << "  area " << _area << endl;
	cout << "  type " << (mono() ? "mono-function" : "multi-function") << endl;
	cout << "  " << (_synchronous ? "synchronous" : "combinatorial") << endl;
	if (_synchronous)
	{
		cout << "  cadency " << _cadency << endl;
		cout << "  latency " << _latency << endl;
		cout << "  pipeline stages " << _pipelineStages << endl;
		cout << "  reset polarity " << (_polarity == HIGH ? "high" : "low") << endl;
		cout << "  reset port name " << _rst_name << endl;
		cout << "  clock port name " << _clk_name << endl;
	}
	if (_functions.size() > 1)
	{
		cout << "  control port name " << _ctrl_name << endl;
		cout << "  codes = ";
		for (i = 0; i < _functions.size(); i++) cout << getCtrlCode(i) << " ";
		cout << endl;
	}
	cout << "  input ports: ";
	for (i = 0; i < _in_ports.size(); i++)
	{
		_in_ports[i]->info();
		cout << " ";
	}
	cout << endl;
	cout << "  output ports: ";
	for (i = 0; i < _out_ports.size(); i++)
	{
		_out_ports[i]->info();
		cout << " ";
	}
	cout << endl;
	_functions.info();
}

void Operator::serialize(ofstream &f) const
{
	long i;
	f << "Operator " << name() << "\n";
	f << "  area " << _area << "\n";
	f << "  type " << (mono() ? "mono-function" : "multi-function") << "\n";
	f << "  " << (_synchronous ? "synchronous" : "combinatorial") << "\n";
	if (_synchronous)
	{
		f << "  cadency " << _cadency << "\n";
		f << "  latency " << _latency << "\n";
		f << "  pipeline stages " << _pipelineStages << "\n";
		f << "  reset polarity " << (_polarity == HIGH ? "high" : "low") << "\n";
		f << "  reset port name " << _rst_name << "\n";
		f << "  clock port name " << _clk_name << "\n";
	}
	if (_functions.size() > 1)
	{
		f << "  control port name " << _ctrl_name << "\n";
		f << "  codes = ";
		for (i = 0; i < _functions.size(); i++) cout << getCtrlCode(i) << " ";
		f << "\n";
	}
	f << "  input ports: ";
	for (i = 0; i < _in_ports.size(); i++)
	{
		_in_ports[i]->serialize(f);
		cout << " ";
	}
	f << "\n";
	f << "  output ports: ";
	for (i = 0; i < _out_ports.size(); i++)
	{
		_out_ports[i]->serialize(f);
		cout << " ";
	}
	f << "\n";
	_functions.serialize(f);
}
/* GL 27/04/2007 :	Add fllowing lines */
//! Copy of operator items with newlocation and new name
Operator * Operator::copyOf(string name, string compName)
{
	Operator *o = new Operator(name, compName, this);
	// copy ports in another location
	int i;
	for (i = 0; i<this->inputPorts(); i++)
	{
		const Port *p = new Port(
		    this->getInputPort(i)->name(),
		    this->getInputPort(i)->way(),
		    i,
		    this->getInputPort(i)->function(),
		    o
		);
		o->_in_ports[i] = p;
	}
	for (i = 0; i<this->outputPorts(); i++)
	{
		const Port *p = new Port(
		    this->getOutputPort(i)->name(),
		    this->getOutputPort(i)->way(),
		    i,
		    this->getOutputPort(i)->function(),
		    o
		);
		o->_out_ports[i] = p;
	}
	return o;
}
/* Fin GL */

/* GL 25/10/07 : combinational_delay */
//! Get function combinational delay.
long Operator::getCombinationalDelay(const Function *f) const
{
	string function_name = f->symbolic_function();
	int i = 0;
	while ((function_name != getFunctionList(i)) && (i < _functionList.size()))
		i++;
	if (i != _functionList.size()) return _combinational_delay[i];
	else return -1;
	return -1;
}
/* Fin GL */

	//! Searches if an operator implements a function defined by is name
	bool Operator::implements(const string &function_name) const
	{
		for (ImplementedFunctionRefs::const_iterator it = _functions.begin(); it != _functions.end(); it++)
		{
			const ImplementedFunction *imf = *it;
			const Function *f=imf->function();
			if (f->symbolic_function() == function_name)
			{
				return true;
			}
		}
		return false;
	}

// end of: operator.cpp


