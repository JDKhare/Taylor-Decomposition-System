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

//	File:		port.h
//	Purpose:	Functions/operators ports.
//	Author:		Pierre Bomel, LESTER, UBS

class Port;
class PortRef;
class PortRefs;
class OperatorInstance;

#ifndef __PORT_H__
#define __PORT_H__

#include <iostream>
#include <string>
using namespace std;
#include "map.h"
#include "check.h"
#include "parser.h"
#include "switches.h"

//! Smart pointer to a symbolic Port.
class PortRef  : public MapRef <Port>
{
public:
	PortRef(Port *r) : MapRef<Port>(r){}
};
//! List of smart pointers to symbolic Port.
class PortRefs : public MapRefs<Port, PortRef> {};

#include "function.h"

//! Ports
class Port
{
public:
	//! Local type to distinguish between ports
	enum Type
	{
		SYMBOLIC_PORT,			//!< Function port
		PORT					//!< Operator port
	};
	//! local type to distinguish ports in/out
	enum Way
	{
		IN,						//!< Input port
		OUT						//!< Output port
	};
	//! local type to distinguish ports (s)igned/(u)nsigned
	enum Signed
	{
		SIGNED,					//!< Signed port
		UNSIGNED,				//!< Unsigned port
		/* GL 05/10/07 : fixed point */
		SFIXED,
		UFIXED,
		/* Fin GL */
		STD_LOGIC_VECTOR		//!< std_logic_vector port (default)
	};
private:
	const Type		_type;		//!< port type
	const Way		_way;		//!< port way
	const string	_name;		//!< unique port name.
	const int		_pos;		//!< position in function declaration (0..N-1)
	Function	*_function;	//!< backward pointer to function
	long			_bitwidth;	//!< port bitwidth
	/* GL 05/10/07 : fixed point */
	long _fixedPoint;
	/* Fin GL */
	Signed			_signed;	//!< port type
	/* GL 26/04/09 : Timing behavior */
	bool			_var_time;	//!< port timing behavior
	/* Fin GL */
	Operator	*_oper;		//!< backward pointer to operator
public:
	void *_addr; //!< for extensions
	Port(const string & name, Way way, int pos, const Function *f);
	Port(const string & name, Way way, int pos, const Function *f, const Operator *op);
	~Port()
	{
#ifdef CHECK
		port_delete++;	// debug
#endif
	}
	//! Get way.
	Way way() const
	{
		return _way;
	}
	//! Get type.
	Type type() const
	{
		return _type;
	}
	//! Get name.
	string name() const
	{
		return _name;
	}
	//! Get bitwidth.
	long bitwidth() const
	{
		return _bitwidth;
	}
	//! Set bitwidth
	void bitwidth(long bitwidth)
	{
		_bitwidth = bitwidth;
	}
	/* GL 05/10/07 : fixed point */
	//! Get fixedPoint.
	long fixedPoint() const
	{
		return _fixedPoint;
	}
	//! Set fixedPoint
	void fixedPoint(long fixedPoint)
	{
		_fixedPoint = fixedPoint;
	}
	/* Fin GL */
	//! Get Signed
	Signed getSigned() const
	{
		return _signed;
	}
	//! Get string signed
	string getStringSigned() const
	{
		if (_signed == SIGNED) return "signed";
		else if (_signed == UNSIGNED) return "unsigned";
		/* GL 05/10/07 : fixed point */
		else if (_signed == SFIXED) return "sfixed";
		else if (_signed == UFIXED) return "ufixed";
		/* Fin GL */
		else return "std_logic_vector";
	}
	//! Set Signed
	void setSigned(Signed s)
	{
		_signed = s;
	}
	//! Get declaration position.
	int pos() const
	{
		return _pos;
	}
	/* GL 26/04/09 : Timing behavior */
	bool hasVariableTime() const
	{
		return _var_time;
	}
	void hasVariableTime(bool vt)
	{
		_var_time = vt;
	}
	/* Fin GL */
	//! Get function.
	const Function * function() const
	{
		return _function;
	}
	//! Get operator
	const Operator * oper() const
	{
		return _oper;
	}
	//! Print info.
	void info() const;
	//! Serialize into a file.
	void serialize(ofstream &f) const;

	void entity_port_vhdl_declatation(ofstream &f, const string &tabul, const Switches *_sw,const OperatorInstance *inst,const string &way) const;

	void signal_port_vhdl_declatation(ofstream &f, const string &tabul, const Switches *_sw,const OperatorInstance *inst) const;

	string signal_name(const OperatorInstance *inst,const Switches *_sw) const;


};

#endif // __PORT_H__


