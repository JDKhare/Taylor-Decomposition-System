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

//	File:		operation.cpp
//	Purpose:	CDFG Operations
//	Author:		Pierre Bomel, LESTER, UBS

#include "function.h"
#include "operation.h"
/* GL 07/01/07	: add lines */
#include "scheduling.h"
/* Fin GL */
using namespace std;

void Operation::info() const
{
	cout << "  Operation " << name();
	cout << "(function=" << _function->name();
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
	/*GL 11/09/07 : add bitwidth info */
	cout << ",bitwidths";
	for (int i =0; i<_inputsBitwidth.size(); i++) cout << "_" << inputsBitwidth(i);
	/*Fin GL */
	cout << ")" << endl;
}
/* GL 07/01/07	: add lines */
//! compute an operation overcost
void Operation::computeOvercost()
{
	Function *f = _function;
	FunctionSchedulingState *f_ss = (FunctionSchedulingState *) f->_addr;
	OperatorInstance *oi;
	for (OperatorRefs::const_iterator o_it = f_ss->allocated.begin(); o_it != f_ss->allocated.end(); o_it++)
	{
		Operator *oper = (*o_it).second;
		OperatorSchedulingState *o_ss = (OperatorSchedulingState*) oper->_addr;
		if (!o_ss->E.empty()) oi = o_ss->E.at(0);
		/* GL 17/02/08 : Replace lines
		else oi = o_ss->D.at(0);
		/* By */
		oi = 0;
		/* Fin GL */
	}
	/* GL 17/02/08 : Add lines */
	if ( oi ==0)
	{
		_overcost = sumOfwidths();
		return;
	}
	/* Fin GL */
	if (_inputsBitwidth.size()!= 2)
	{
		_overcost = abs(oi->sumOfwidths() - sumOfwidths()) ;
		return;
	}
	long gradient1 = abs(oi->inputsBitwidth(0) - _inputsBitwidth[0])/oi->inputsBitwidth(0) + abs(oi->inputsBitwidth(1) - _inputsBitwidth[1])/oi->inputsBitwidth(1);
	long gradient2 = abs(oi->inputsBitwidth(0) - _inputsBitwidth[1])/oi->inputsBitwidth(0) + abs(oi->inputsBitwidth(1) - _inputsBitwidth[0])/oi->inputsBitwidth(1);
	if (gradient1<gradient2)
	{
		_overcost = gradient1;
		return;
	}
	_overcost = gradient2;
}
/* Fin GL */

// end of: operation.h


