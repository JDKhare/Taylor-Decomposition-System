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

//	File:		port.cpp
//	Purpose:	functions ports
//	Author:		Pierre Bomel, LESTER, UBS

#include <iostream>
#include "port.h"
#include "operator.h"
#include "scheduling.h"
using namespace std;

//! Create a new symbolic port.
//! @param Input. name is the unique port name.
//! @param Input. way is the port way.
//! @param Input. pos is the declaration position.
//! @param Input. function is a backward pointer to the containing function.
Port::Port(const string & name, Way way, int pos, const Function *f)
		: _type(SYMBOLIC_PORT), _way(way), _name(name), _pos(pos)
{
	_oper = 0;
	_function = (Function *) f;
	_addr = 0;
	_signed = STD_LOGIC_VECTOR;
	_bitwidth = 16;
	/* GL 05/10/07 : fixed point */
	_fixedPoint = -999;
	/* Fin GL */
#ifdef CHECK
	port_create++;	// debug
#endif
}

//! Create a new operator port.
//! @param Input. name is the unique port name.
//! @param Input. way is the port way.
//! @param Input. pos is the declaration position.
//! @param Input. op is a backward pointer to the containing operator.
Port::Port(const string & name, Way way, int pos, const Function *f, const Operator *op)
		: _type(PORT), _way(way), _name(name), _pos(pos)
{
	//cout << "Create port " << _name << " way=" << way << " pos=" << pos << " operator=" << op->name() << endl;
	_oper = (Operator *) op;
	_function = (Function *) f;
	_addr = 0;
	_signed = STD_LOGIC_VECTOR;
	_bitwidth = 16;
	/* GL 05/10/07 : fixed point */
	_fixedPoint = -999;
	/* Fin GL */
	/* GL 26/04/09 : Timing behavior */
	_var_time = false;
	/* Fin GL */
#ifdef CHECK
	port_create++;	// debug
#endif
}

void Port::info() const
{
	if (_type == SYMBOLIC_PORT)
	{
		cout << _name << "/" << "(" << _pos << ")";
		/* GL 23/07/07 : print bitwidth */
		cout << "/" << "(" << _bitwidth << "bits)";
		/* Fin GL */
		/* GL 09/11/07 : print signe info */
		cout << "/" << getStringSigned();
		/* Fin GL */
	}
	else
	{
		const Port *p = _way == IN ? _function->getInputPortByPos(_pos) : _function->getOutputPortByPos(_pos);
		cout << _name << "/" << _function->name() << "." << p->name() << "(" << _pos << ")";
		/* GL 23/07/07 : print bitwidth */
		cout << "/" << "(" << p->bitwidth() << " bits)";
		/* Fin GL */
		/* GL 09/11/07 : print signe info */
		cout << "/" << p->getStringSigned();
		/* Fin GL */
	}
}

void Port::serialize(ofstream &f) const
{
	if (_type == SYMBOLIC_PORT)
	{
		f << _name << "/" << "(" << _pos << ")";
	}
	else
	{
		const Port *p = _way == IN ? _function->getInputPortByPos(_pos) : _function->getOutputPortByPos(_pos);
		f << _name << "/" << _function->name() << "." << p->name() << "(" << _pos << ")";
	}
}


//! Build a signal name from an instance and a port.
//! @param Input. inst is a pointer to the instance.
//! @result a string
string Port::signal_name(const OperatorInstance *inst,const Switches *_sw) const
{
	string sig_name = inst->instance_name(_sw);
	sig_name += "_";
	sig_name += name();
	return sig_name;
}

void Port::entity_port_vhdl_declatation(ofstream &f, const string &tabul, const Switches *_sw,const OperatorInstance *inst,const string &way) const
{
	if (_sw->bitwidth_aware())
	{
		f << tabul << signal_name(inst,_sw) << " : " <<	way << getStringSigned();
		if (fixedPoint() == -999)
			f << "(" << bitwidth()-1 <<" downto 0);" << "\n";
		else
			f << "(" << fixedPoint()- 1 << " downto -" << bitwidth()- fixedPoint() <<");" << "\n";
	}
	else
		f << tabul << signal_name(inst, _sw) << " : " << way  << "word;" << "\n";
}


void Port::signal_port_vhdl_declatation(ofstream &f, const string &tabul, const Switches *_sw,const OperatorInstance *inst) const
{
	if (_sw->bitwidth_aware())
	{
		f << tabul << "signal " << signal_name(inst,_sw) << " : " << getStringSigned();
		if (fixedPoint() == -999)
			f << "(" << bitwidth()-1 <<" downto 0);" << "\n";
		else
			f << "(" << fixedPoint()- 1 << " downto -" << bitwidth()- fixedPoint() <<");" << "\n";
	}
	else
		f << tabul << "signal " << signal_name(inst, _sw) << " : " << "word;" << "\n";
}

// end of: port.cpp




