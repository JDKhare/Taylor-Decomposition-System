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

//	File:		mem.h
//	Purpose:	memory
//	Author:		Pierre Bomel, LESTER, UBS

// forward refs
class Mem;

#ifndef __MEM_H__
#define __MEM_H__

#include "check.h"
using namespace std;

//! Memory model.
class Mem
{
private:
	const long _access_time;	//!< Access time. It means a R/W can occur only every _access_time tu.
public:
	//! Create a new mamory model.
	Mem(long access_time) : _access_time(access_time)
	{
#ifdef CHECK
		mem_create++;	// debug
#endif
	}
	//! Delete a memory model
	~Mem()
	{
#ifdef CHECK
		mem_delete++;	// debug
#endif
	}
	//! Get memory access time in tu.
	long access_time() const
	{
		return _access_time;
	}
	//! Print info.
	void info()
	{
		cout << "memory access time = " << _access_time << endl;
	}
};

#endif // __MEM_H__
