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

//	File:		check.h
//	Purpose:	optional check of dynamic behavior of GAUT
//				Used to detect memory leaks
//	Author:		Pierre Bomel, LESTER, UBS

#ifndef __CHECK_H__
#define __CHECK_H__

#ifdef CHECK
extern int cadency_create;
extern int cadency_delete;
extern int cdfg_create;
extern int cdfg_delete;
extern int cdfgedge_create;
extern int cdfgedge_delete;
extern int cdfgnode_create;
extern int cdfgnode_delete;
extern int clock_create;
extern int clock_delete;
extern int control_create;
extern int control_delete;
extern int data_create;
extern int data_delete;
extern int function_create;
extern int function_delete;
extern int node_create;
extern int node_delete;
extern int edge_create;
extern int edge_delete;
extern int graph_create;
extern int graph_delete;
extern int io_constraints_create;
extern int io_constraints_delete;
extern int lib_create;
extern int lib_delete;
extern int mapid_create;
extern int mapid_delete;
extern int SelectionOut_create;
extern int SelectionOut_delete;
extern int mem_create;
extern int mem_delete;
extern int operation_create;
extern int operation_delete;
extern int operator_create;
extern int operator_delete;
extern int OperatorInstance_create;
extern int OperatorInstance_delete;
extern int OperatorSchedulingState_create;
extern int OperatorSchedulingState_delete;
extern int OperationSchedulingState_create;
extern int OperationSchedulingState_delete;
extern int FunctionSchedulingState_create;
extern int FunctionSchedulingState_delete;
extern int Mobility_create;
extern int Mobility_delete;
/* GL 21/06/07 : remove lines
/* and add lines */
extern int Priority_create;
extern int Priority_delete;
/* Fin GL */
extern int SchedulingStage_create;
extern int SchedulingStage_delete;
extern int SchedulingOut_create;
extern int SchedulingOut_delete;
extern int Scheduling_create;
extern int Scheduling_delete;
extern int tools_create;
extern int tools_delete;
extern int parser_create;
extern int parser_delete;
extern int port_create;
extern int port_delete;
extern int switches_create;
extern int switches_delete;
extern int value_create;
extern int value_delete;
/* KT 28/05/2008 */
extern int Solution_delete;
/* End KT */

#endif

extern void checkDump();

#endif // __CHECK_H__

