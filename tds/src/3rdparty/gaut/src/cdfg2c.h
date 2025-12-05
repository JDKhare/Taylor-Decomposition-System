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

//	File:		cdfg2c.h
//	Purpose:	.c file format output for debug cdfg
//	Author:		Dominique Heller, Lab_Sticc, UBS

#ifndef __CDFG2C_H__
#define __CDFG2C_H__
#include <fstream>
#include "parser.h"	// time Units
#include "os_tools.h"	// cadency
#include "map.h"
#include "switches.h"
#include "lib.h"
#include "cadency.h"
#include "bus.h"
#include "reg.h"
#include "cdfg.h"
#include "ic.h"
#include "parser.h"
using namespace std;

class Cdfg2c
{

public:
	static string get_predefined_operation(const string &operation_name);
	static void process_loopback(ofstream &f,const CDFG *cdfg);
	static void process_aging(ofstream &f,const CDFG *cdfg);


	//! @param Input. cdfg is a pointer to the CDFG.
	Cdfg2c(
	    const CDFG *cdfg					// in
	);
};

#endif // __CDFG2C_H__

