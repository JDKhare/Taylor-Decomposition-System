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

//	File:		io_constraints.cpp
//	Purpose:	Specific processing for IO constraints
//	Author:		Pierre Bomel, LESTER, UBS

class IOC;

#ifndef __IOC_H__
#define __IOC_H__

#include "switches.h"
#include "data.h"
#include "cdfg.h"
#include "check.h"
using namespace std;

//! IO constraints.
class IOC
{
private:
	static bool _debug;			// debug
	static CDFG *_cdfg;			//!< copy of a local parameter
	static Clock *_clk;			//!< copy of a local parameter
	static Cadency *_cadency;	//!< copy of a local parameter
public:
	IOC()
	{
#ifdef CHECK
		io_constraints_create++;	// debug
#endif
	}
	~IOC()
	{
#ifdef CHECK
		io_constraints_delete++;	// debug
#endif
	}

	//! ASAP shift of output data nodes.
	//! The output nodes without asap locked can be switched
	//! to a sooner delay. This delay is the time when they
	//! are produced. No need to wait for the end of
	//! the algorithm to output all the data in a single clock
	//! cycle. This would lead to an unecessary high number
	//! of buses.
	//! @param Input. cdfg is a pointer to the CDFGG to update.
	//! @param Input. scheduled is a boolean telling if the CDFG has been scheduled or not. Default value = false.
	static void asap_shift(CDFG *cdfg, const Clock *clk, const Cadency *cadency, bool scheduled = false);
	//! Get min starting time of all successors of a node.
	//! @param Input. n is a pointer to the node.
	//!
	//! When scheduled is true start times are updated, else asap times are updated.
	static void set_min_start(CDFGnode *n, bool scheduled);
	//! ALAP shift of input data nodes.
	//! The input nodes without alap locked can be delayed
	//! to a later delay. This delay is the first time when they
	//! are consumed. No need to read them all at the start of
	//! the algorithm in a single clock
	//! cycle. This would lead to an unecessary high number
	//! of buses.
	//! @param Input. cdfg is a pointer to the CDFGG to update.
	//! @param Input. scheduled is a boolean telling if the CDFG has been scheduled or not. Default value = false.
	static void alap_shift(CDFG *cdfg, bool scheduled = false);
	//! Get max ending time of all predecessors of a node.
	//! @param Input. n is a pointer to the node.
	//!
	//! When scheduled is true max is computed with (start+length) times, with (alap+length) times otherwise.
	static long max_end(CDFGnode *n, bool scheduled);

};

#endif // __IOC_H__

// end of: io_constraints.h


