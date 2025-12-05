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

//	File:		resizing.h
//	Purpose:	Resizing Operator's instances A posteriori according to bitwidths
//	Author:		Ghizlane Lhairech, LESTER, UBS
//	Date:		06/04 /07

class Resizing;


#ifndef __RESIZING_H__
#define __RESIZING_H__

#include "scheduling.h"
#include "cdfg.h"
#include "graph.h"
#include "operation.h"
#include "port.h"
#include "data.h"
#include "control.h"
using namespace std;


//! Operator instance list. To sort operations according to instances.

class OI_List
{

public:

	vector<const Operation *>	Operations;			//!< The list operations having the same instance
	OperatorInstance *					Op;			//!< The correspondant operator
	vector<int>						  Nops;			//!< Number of operations for each instance
	/* KT 16/04/2008 */
	bool checked;									//!< true if all operations are checked	on the local_search step
	long index;										//!< the index of the operation will be chechek on the next iteration of the local search
	/* End KT */
	
	OI_List() 
	{
		Operations.clear();
		Nops.clear();
		Op=NULL;		  
	};
	
};

//! Operator instance lists is a vector of OI_List. A list for each instance

class OI_Lists : public vector<OI_List> {};

//! The resizing process.
//! The resizing process consists in computing bitwidths of operator instances
//! each instance has a list of multi-bitwidths operations bound on it
//! This process resizes operators according to inputs/outputs widths
class Resizing
{

private:

	//! Local debug toggle
	static bool _debug;
	/* GL 07/09/2007 : combinatorial area */
	long _area;
	/* Fin GL */
	/* GL 30/09/2007 : Number of bits */
	long _nbBits;
	/* Fin GL */
	const Switches *_sw;							//!< Local copy of sw.
	const SchedulingOut	*_sched;					//!< Local copy of sched
	CDFG *_cdfg;								//!< Local copy of cdfg.

public:

	void info() const;
	string newOperatorName(Operator *op) const;
	string newComponentName(Operator *op) const;
	void updateParamPort(Operator *op);
	void operatorSizing(void);
	/* GL 07/09/2007 : combinatorial area */
	long area()
	{
		return _area;
	}
	/* Fin GL */
	//! Constructor of the object representing the resizing process.
	//! @param Inout. sched is a pointer to the scheduling result.
	//! @param Inout. cdfg is a pointer to the CDFG.
	//! Operators are resized according to biwidhts informations
	Resizing(
	    const Switches *sw,
	    const SchedulingOut	*sched,
	    CDFG *cdfg							// in-out
	);
};

#endif // __RESIZING_H__

// end of: resizing.h

