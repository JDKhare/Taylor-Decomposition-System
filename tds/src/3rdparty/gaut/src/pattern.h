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

//	File:		pattern.h
//	Purpose:	Patterns.
//	Author:		Ghizlane Lebreton, LESTER, UBS

class Pattern;
class PatternRef;
class PatternRefs;

#ifndef __PATTERN_H__
#define __PATTERN_H__

#include <iostream>
#include <string>
using namespace std;
#include "map.h"

//! Smart pointer on a Pattern.
class PatternRef		: public MapRef<Pattern>
{
public:
	PatternRef(Pattern *p) : MapRef<Pattern>(p) {}
};
//! List of smart pointers to Patterns.
class PatternRefs : public MapRefs<Pattern, PatternRef>
{
};

#include "function.h"

//! Patterns.
class Pattern : public MapID
{
private:
	const long _cycle;
public:
	void *_addr;	//!< for extensions
	//! Create a new pattern.
	//! @param Input. name is the uniqu eof the Pattern.
	Pattern(const string & name, const long cycle) : MapID(name), _cycle(cycle)
	{
		_addr = 0;
	}
	~Pattern();
	//! Get Numbre of allowed cycles.
	const long cycle() const
	{
		return _cycle;
	}
	//! Print info.
	void info() const; // see pattern.cpp
	//! Serialize into a file.
	void serialize(ofstream &f) const;
};
#endif // __PATTERN_H__
