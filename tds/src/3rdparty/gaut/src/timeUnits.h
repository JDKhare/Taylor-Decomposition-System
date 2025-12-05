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

//	File:		timeUnits.h
//	Purpose:	time units definition
//	Author:		Pierre Bomel, LESTER, UBS

// forward refs
class TimeUnit;

#ifndef __TIME_UNITS_H__
#define __TIME_UNITS_H__

#include <iostream>
#include <string>
using namespace std;

//! Time units (tu) types.
enum TimeUnits
{
	S,		// second
	MS,		// milli-second
	US,		// micro-seconds
	NS,		// nano-seconds
	PS,		// pico-seconds
	FS,		// femto-second
	ERRS	// error
};

//! Time Unit (tu) class.
class TimeUnit
{
private:
	TimeUnits _unit;	//!< The time unit type.
public:
	//! Create a new tu.
	TimeUnit(TimeUnits tu = ERRS)
	{
		_unit = tu;
	}
	//! Copy a tu.
	TimeUnit(const string &s)
	{
		if      (s == "s")  _unit= S;
		else if (s == "ms") _unit= MS;
		else if (s == "us") _unit= US;
		else if (s == "ns") _unit= NS;
		else if (s == "ps") _unit= PS;
		else if (s == "fs") _unit= FS;
		else _unit= ERRS;
	}
	//! Operator () for tu.
	TimeUnit & operator() (const string &s)
	{
		if      (s == "s")  _unit= S;
		else if (s == "ms") _unit= MS;
		else if (s == "us") _unit= US;
		else if (s == "ns") _unit= NS;
		else if (s == "ps") _unit= PS;
		else if (s == "fs") _unit= FS;
		else _unit= ERRS;
		return *this;
	}
	//! Convert a tu to a string.
	string timeUnitStr() const
	{
		if (_unit == S)    return "s";
		if (_unit == MS)   return "ms";
		if (_unit == US)   return "us";
		if (_unit == NS)   return "ns";
		if (_unit == PS)   return "ps";
		if (_unit == FS)   return "fs";
		return "??";
	}
	//! Error for tu.
	bool error() const
	{
		return(_unit==ERRS);
	}
	//! Print info.
	void info()
	{
		cout << "time unit = " << timeUnitStr() << endl;
	}
};

#endif // __TIME_UNITS_H__
