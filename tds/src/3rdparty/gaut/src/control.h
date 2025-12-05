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

//	File:		Control.h
//	Purpose:	Control nodes in CDFG
//	Author:		Pierre Bomel, LESTER, UBS

class Control;
class ControlRef;
class ControlRefs;

#ifndef __Control_H__
#define __Control_H__

#include <iostream>
#include <string>
using namespace std;
#include "map.h"
#include "cdfg.h"
#include "check.h"

//! A smart pointer on a Control node.
class ControlRef  : public MapRef <Control>
{
public:
	ControlRef(Control *c) : MapRef<Control>(c){}
};
//! A list of smart pointers to Control nodes.
class ControlRefs : public MapRefs<Control, ControlRef> {};

//! CDFG Control nodes.
class Control : public CDFGnode
{
public:
	// Local type to distinguish Control nodes.
	enum Type {SOURCE, SINK};
private:
	const Type	_type;	//!< Node type.
	// scheduling data
	long _asap;			//!< asap time.
	bool _asap_locked;	//!< asap locked status.
	long _alap;			//!< alap time.
	bool _alap_locked;	//!< alap locked status.
	long _start;		//!< start time.
public:
	//! Create a new Control node
	//! @param Input. name is the unique node name.
	//! @param Input. type is the Control node type.
	Control(const string &name, Control::Type type) : CDFGnode(name, CONTROL), _type(type)
	{
		_asap = _alap = 0;
		_asap_locked = _alap_locked = false;
		_start = -1;
		//_addr = 0;
#ifdef CHECK
		control_create++;	// debug
#endif
	}
	~Control()
	{
#ifdef CHECK
		control_delete++;	// debug
#endif
	}
	Control::Type type() const
	{
		return _type;
	}
	// set data
	//! Set start time.
	void start(long start)
	{
		_start = start;
	}
	//! Set asap time.
	void asap(long asap)
	{
		_asap = asap;
	}
	//! Set alap time.
	void alap(long alap)
	{
		_alap = alap;
	}
	//! Set asap locked status.
	void asap_locked(bool b)
	{
		_asap_locked = b;
	}
	//! Set alap locked status.
	void alap_locked(bool b)
	{
		_alap_locked = b;
	}
	//! Set cycles, not implemented.
	void cycles(const SelectionOutRefs *mdos,long clk_period,long memory_access_time) {} // do nothing
	// get data
	//! Get start time.
	long start() const
	{
		return _start;
	}
	//! Get asap time.
	long asap() const
	{
		return _asap;
	}
	//! Get alap time.
	long alap() const
	{
		return _alap;
	}
	//! Get length.
	//! Now the only control nodes do not correspond to any processing.
	//! They are just here to implement a block.
	//! They have no duration: length = cycles = 0
	long length() const
	{
		return 0;
	}
	//! Get number of cycles.
	//! Now the only control nodes do not correspond to any processing.
	//! They are just here to implement a block.
	//! They have no duration: length = cycles = 0
	long cycles() const
	{
		return 0;
	}
	//! Get asap locked status.
	bool asap_locked() const
	{
		return _asap_locked;
	}
	//! Get alap locked status.
	bool alap_locked() const
	{
		return _alap_locked;
	}
	//! Print info.
	void info() const
	{
		cout << "  Control " << name();
		cout << "(type=";
		switch (_type)
		{
		case SOURCE:
			cout << "source";
			break;
		case SINK:
			cout << "sink";
			break;
		}
		cout << ",asap=";
		if (asap_locked()) cout << "[";
		cout << _asap;
		if (asap_locked()) cout << "]";
		cout << ",alap=";
		if (alap_locked()) cout << "[";
		cout << _alap;
		if (alap_locked()) cout << "]";
		cout << ",length=" << length();
		cout << ",start=" << start();
		cout << ")" << endl;
	}

	//! Serialize.
	void serialize(ofstream &f) const
	{
		int i;
		switch (type())
		{
		case SOURCE:
			f << "source(" << name() << ") {" << "\n";
			f << "  targets ";
			for (i = 0; i < successors.size(); i++)
			{
				if (i) f << ", ";
				f << successors[i]->target->name();
			}
			f << ";" << "\n";
			break;
		case SINK:
			f << "sink(" << name() << ") {" << "\n";
			f << "  targets ";
			for (i = 0; i < predecessors.size(); i++)
			{
				if (i) f << ", ";
				f << predecessors[i]->source->name();
			}
			f << ";" << "\n";
			break;
		}
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  start " << start() << ";" << "\n";
		f << "}" << endl;
	}

	/* Caaliph 03/07/2007 */
	//! Serialize.
	void serialize_STAR(ofstream &f, Cadency *cadency) const
	{
		int i;
		switch (type())
		{
		case SOURCE:
			f << "source(" << name() << ") {" << "\n";
			f << "  targets ";
			for (i = 0; i < successors.size(); i++)
			{
				if (i) f << ", ";
				if (successors[i]->target->type()==DATA)
				{
					if (((Data*)(successors[i]->target))->duplication()>1)
					{
						f << successors[i]->target->name()<<"_"<<0;
					}
					else
						f << successors[i]->target->name();
				}
				else f << successors[i]->target->name();
			}
			f << ";" << "\n";
			break;
		case SINK:
			f << "sink(" << name() << ") {" << "\n";
			f << "  targets ";
			for (i = 0; i < predecessors.size(); i++)
			{
				if (i) f << ", ";
				f << predecessors[i]->source->name();
			}
			f << ";" << "\n";
			break;
		}
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  real_start " << start() << ";" << "\n";
		f << "  start " << start()%cadency->length() << ";" << "\n";
		f << "}" << endl;
	}
	/* End Caaliph */


	//! Returns chaining information
	long isChained() const
	{
		return 0;
	}

	//! Get length when chaining is performed in time units.
	long chainingLength() const
	{
		return 0;
	}

};

#endif // __Control_H__
