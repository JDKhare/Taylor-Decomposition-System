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

//	File:		dot_gantt.h
//	Purpose:	.gantt file formet output for GAUT kernel
//	Author:		Emmanuel Juin, LESTER, UBS

class DotGantt;

#ifndef __DOTGANTT_H__
#define __DOTGANTT_H__

#include <fstream>
#include "parser.h"	// time Units
#include "os_tools.h"	// cadency
#include "map.h"
#include "switches.h"
#include "cadency.h"
#include "bus.h"
#include "reg.h"
#include "cdfg.h"
#include "parser.h"
using namespace std;

//! Smart pointer to DotGanttNode, based on the generic MapRef.
class DotGanttNodeRef  : public MapRef <CDFGnode>
{
public:
	DotGanttNodeRef(CDFGnode *d) : MapRef<CDFGnode>(d){}
};

//! List of smart pointers to DotGanttNode, based on the generic MapRefs.
class DotGanttNodeRefs : public MapRefs<CDFGnode, DotGanttNodeRef> {};

//! GAUT's .gantt file format.management.
class DotGantt
{
private:
public:
	static bool _debug;		// to debug
	//! .gantt file dump from GAUT kernel outputs.
	//! @param Input. sw is a pointer to the user switches.
	//! @param Input. cdfg is a pointer to the CDFG.
	//! @param Input. clk is a pointer to the system clock.
	//! @param Input. cadency is a pointer to the cadency.
	//! @param Input. reg_out is a pointer to the registers.
	//! @param Input. bout is a pointer to the bus synthesis outputs.
	DotGantt(
	    const Switches *sw,					// in
	    const CDFG *cdfg,					// in
	    const Clock *clk,					// in
	    const Cadency *cadency,				// in
	    const RegOut *reg_out,				// in
	    const BusOut *bout					// in
	);
	/* Caaliph : 27/06/2007 */
	DotGantt(
	    const Switches *sw,					// in
	    const string &_cdfg_file_name,		//in //DH passage par reference 12/11/2008 : attention au passage par veleur (recopie)
	    const CDFG *cdfg,					// in
	    const Clock *clk,					// in
	    const Cadency *cadency,				// in
	    const RegOut *reg_out,				// in
	    const BusOut *bout					// in
	);
	/* End Caaliph */
};

#endif // __DOTGANTT_H__

