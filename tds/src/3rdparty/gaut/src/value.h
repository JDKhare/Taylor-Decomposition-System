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

//	File:		value.h
//	Purpose:	VALUE of DATA
//	Author:		Pierre Bomel, LESTER, UBS

class Value;

#ifndef __VALUE_H__
#define __VALUE_H__

#include <iostream>
#include "check.h"
using namespace std;

//! Values.
class Value
{
	// Now GAUT handles only values of the same data format: integers.
	// But this should be quickly enhanced to accomodate with
	// variable length data path and fixed point representations.
private:
	const long _value;		//!< The value itself.
public:
	void * _addr; //!< for extensions
	//! Create a Value.
	Value(long value) : _value(value)
	{
		_addr = 0;
#ifdef CHECK
		value_create++;	// debug
#endif
	}
	//! Delete a Value.
	~Value()
	{
#ifdef CHECK
		value_delete++;	// debug
#endif
	}
	//! Get value of Value.
	long value() const
	{
		return _value;
	}
};

#endif // __VALUE_H__

// end of: value.h
