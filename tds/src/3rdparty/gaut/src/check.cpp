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

#ifdef CHECK

#include <iostream>
using namespace std;

int cadency_create = 0;
int cadency_delete = 0;
int cdfg_create = 0;
int cdfg_delete = 0;
int cdfgedge_create = 0;
int cdfgedge_delete = 0;
int cdfgnode_create = 0;
int cdfgnode_delete = 0;
int clock_create = 0;
int clock_delete = 0;
int control_create = 0;
int control_delete = 0;
int data_create = 0;
int data_delete = 0;
int function_create = 0;
int function_delete = 0;
int node_create = 0;
int node_delete = 0;
int edge_create = 0;
int edge_delete = 0;
int graph_create = 0;
int graph_delete = 0;
int io_constraints_create = 0;
int io_constraints_delete = 0;
int lib_create = 0;
int lib_delete = 0;
int mapid_create = 0;
int mapid_delete = 0;
int mem_create = 0;
int mem_delete = 0;
int SelectionOut_create = 0;
int SelectionOut_delete = 0;
int operation_create = 0;
int operation_delete = 0;
int operator_create = 0;
int operator_delete = 0;
int OperatorInstance_create = 0;
int OperatorInstance_delete = 0;
int OperatorSchedulingState_create = 0;
int OperatorSchedulingState_delete = 0;
int OperationSchedulingState_create;
int OperationSchedulingState_delete;
int FunctionSchedulingState_create = 0;
int FunctionSchedulingState_delete = 0;
int Mobility_create = 0;
int Mobility_delete = 0;
/* GL 24/05/07 : for scheduling
/* and add lines */
int Priority_create = 0;
int Priority_delete = 0;
/* Fin GL */
int SchedulingStage_create = 0;
int SchedulingStage_delete = 0;
int SchedulingOut_create = 0;
int SchedulingOut_delete = 0;
int Scheduling_create = 0;
int Scheduling_delete = 0;
int tools_create = 0;
int tools_delete = 0;
int parser_create = 0;
int parser_delete = 0;
int port_create = 0;
int port_delete = 0;
int switches_create = 0;
int switches_delete = 0;
int value_create = 0;
int value_delete = 0;

void checkDump()
{
	cout << "---------------------------" << endl;
	cout << "Dynamic behavior statistics" << endl;
	cout <<  "cadency+" << " = " << cadency_create << endl;
	cout <<  "cadency-" << " = " << cadency_delete << endl;
	cout <<  "cdfg+"    << " = " << cdfg_create << endl;
	cout <<  "cdfg-"    << " = " << cdfg_delete << endl;
	cout <<  "cdfgedge+"<< " = " << cdfgedge_create << endl;
	cout <<  "cdfgedge-"<< " = " << cdfgedge_delete << endl;
	cout <<  "cdfgnode+"<< " = " << cdfgnode_create << endl;
	cout <<  "cdfgnode-"<< " = " << cdfgnode_delete << endl;
	cout <<  "clock+"   << " = " << clock_create << endl;
	cout <<  "clock-"   << " = " << clock_delete << endl;
	cout <<  "control+" << " = " << control_create << endl;
	cout <<  "control-" << " = " << control_delete << endl;
	cout <<  "data+"    << " = " << data_create << endl;
	cout <<  "data-"    << " = " << data_delete << endl;
	cout <<  "function+"<< " = " << function_create << endl;
	cout <<  "function-"<< " = " << function_delete << endl;
	cout <<  "node+"    << " = " << node_create << endl;
	cout <<  "node-"    << " = " << node_delete << endl;
	cout <<  "edge+"    << " = " << edge_create << endl;
	cout <<  "edge-"    << " = " << edge_delete << endl;
	cout <<  "graph+"   << " = " << graph_create << endl;
	cout <<  "graph-"   << " = " << graph_delete << endl;
	cout <<  "io_constraints+" << " = " << io_constraints_create << endl;
	cout <<  "io_constraints-" << " = " << io_constraints_delete << endl;
	cout <<  "lib+"     << " = " << lib_create << endl;
	cout <<  "lib-"     << " = " << lib_delete << endl;
	cout <<  "mapid+"   << " = " << mapid_create << endl;
	cout <<  "mapid-"   << " = " << mapid_delete << endl;
	cout <<  "mem+"     << " = " << mem_create << endl;
	cout <<  "mem-"     << " = " << mem_delete << endl;
	cout <<  "SelectionOut+" << " = " << SelectionOut_create << endl;
	cout <<  "SelectionOut-" << " = " << SelectionOut_delete << endl;
	cout <<  "operation+" << " = " << operation_create << endl;
	cout <<  "operation-" << " = " << operation_delete << endl;
	cout <<  "operator+"<< " = " << operator_create << endl;
	cout <<  "operator-"<< " = " << operator_delete << endl;
	cout <<  "OperatorInstance+"<< OperatorInstance_create << endl;
	cout <<  "OperatorInstance-"<< OperatorInstance_delete << endl;
	cout <<  "OperatorSchedulingState+"<< OperatorSchedulingState_create << endl;
	cout <<  "OperatorSchedulingState-"<< OperatorSchedulingState_delete << endl;
	cout <<  "FunctionSchedulingState+"<< FunctionSchedulingState_create << endl;
	cout <<  "FunctionSchedulingState-"<< FunctionSchedulingState_delete << endl;
	cout <<  "Mobility+"<< Mobility_create << endl;
	cout <<  "Mobility-"<< Mobility_delete << endl;
	/* GL 21/06/07 : remove lines
	/* and add lines */
	cout <<  "Priority+"<< Priority_create << endl;
	cout <<  "Priority-"<< Priority_delete << endl;
	/* Fin GL */
	cout <<  "SchedulingStage+"<< SchedulingStage_create << endl;
	cout <<  "SchedulingStage-"<< SchedulingStage_delete << endl;
	cout <<  "SchedulingOut+"<< SchedulingOut_create << endl;
	cout <<  "SchedulingOut-"<< SchedulingOut_delete << endl;
	cout <<  "Scheduling+"<< Scheduling_create << endl;
	cout <<  "Scheduling-"<< Scheduling_delete << endl;
	cout <<  "tools+"   << " = " << tools_create << endl;
	cout <<  "tools-"   << " = " << tools_delete << endl;
	cout <<  "parser+"  << " = " << parser_create << endl;
	cout <<  "parser-"  << " = " << parser_delete << endl;
	cout <<  "port+"    << " = " << port_create << endl;
	cout <<  "port-"    << " = " << port_delete << endl;
	cout <<  "switches+"<< " = " << switches_create << endl;
	cout <<  "switches-"<< " = " << switches_delete << endl;
	cout <<  "value+"   << " = " << value_create << endl;
	cout <<  "value-"   << " = " << value_delete << endl;
	cout << "---------------------------" << endl;
}
#else
void checkDump() {}
#endif
