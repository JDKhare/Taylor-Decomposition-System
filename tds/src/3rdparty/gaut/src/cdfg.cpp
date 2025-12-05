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

//	File:		cdfg.cpp
//	Purpose:	CDFG
//	Author:		Pierre Bomel, LESTER, UBS

#include <list>
#include <fstream>
#include "cdfg.h"
#include "data.h"
#include "operation.h"
#include "scheduling.h"
#include "multimode_tools.h"
#include "mentor_type.h"
using namespace std;

// CDFGnode

bool CDFGnode::backTo(const CDFGnode *n, long &measure) const
{
	if (n == this)  	// found
	{
		measure = 0;
		return true;
	}
	// scan all my predecessors
	long max = 0;
	bool path = false;
	for (int i = 0; i < predecessors.size(); i++)
	{
		long local;
		const CDFGedge *e = predecessors[i];		// get my current edge
		const CDFGnode *p = e->source;				// get source node
		if (!p->backTo(n, local)) continue;			// no path to "n" from successor "p"
		path = true;								// at least a path
		if (local > max) max = local;				// keep max
		/* DH: 26/10/2009 : use only dynamic index for mem_read/mem_write operation
		   if (type() == CDFGnode::OPERATION)
		   {
		   const Operation *o = (Operation*)this;
		   cout << "operation = " << o->name() << endl;
		   const Function *f = o->function();
		   cout << "function = " << f->name() << endl;
		   if (f->name() == "mem_read" || f->name() == "mem_write")
		   break;
		   }
		   FIN DH ! 26/10/2009 */
	}
	if (path)
	{
		measure = max + length();					// add my length to the path
		return true;
	}
	// no path
	measure = -1;
	return false;
}

void CDFGnode::forceAlapBackward(const CDFGnode *n, long limit)
{
	// process node
	//cout << "Forcing ALAP backward for " << name() << " limit=" << limit << " target=" << n->name();
	limit -= length();	// adapt limit to current node
	//cout << " resized=" << limit << endl;
	if (!alap_locked())
	{
		//cout << "  never locked before, now locked to " << limit << endl;
		alap_locked(true);							// lock it now
		alap(limit);								// set new ALAP value
	}
	else  										// locked before
	{
		//cout << "  already locked to " << alap();
		if (limit < alap())
		{
			//cout << ",  stronger lock, now locked to " << limit << endl;
			alap(limit);								// set new ALAP value
		}
		else
		{
			//if (limit == alap()) cout << ",  no change";
			//else cout << ",  weaker lock";
			//cout << endl;
			return;
		}
	}

	if (n == this) return;							// stop here
	// else propagate to all my predecessors
	for (int i = 0; i < predecessors.size(); i++)
	{
		const CDFGedge *e = predecessors[i];		// get my current edge
		CDFGnode *p = e->source;					// get source node
		p->forceAlapBackward(n, limit);				// propagate
	}
}

// CDFG implementation

// extract list of used functions from a CDFG
void CDFG::extract_list_of_used_functions(
										  FunctionRefs *used_functions		// out
										 ) const
{
	used_functions->clear();									// empty list
	for (CDFG::NODES::const_iterator it = nodes().begin(); it != nodes().end(); it++)
	{
		CDFGnode *node = (*it);								// get list element
		if (node->type() != CDFGnode::OPERATION) continue;	// next
		const Operation *op = (const Operation*) node;		// dynamic link here
		const Function *f = op->function();					// get function
		string name = f->name();							// get function name
		Function *f2;										// dummy
		if (used_functions->search(name, &f2)) continue;		// already there
		used_functions->add(FunctionRef((Function*)f));		// add function ref
	}
	// simple check to avoid troubles later
	if (used_functions->size() == 0)
	{
		cerr << "Internal error in CDFG: no functions found" << endl;
		exit(1);
	}
}

// Scheduling ASAP & ALAP
//
// when a node has no inputs or outputs constrained
//		asap(x) = max(asap(y)) with y = predecessor(x)
//		alap(x) = min(alap(y)) with y = successor(x)

// ASAP = As Soon As Possible
bool CDFG::ASAP(long start)
{
	// build the evaluation order with a DFS search in the graph
	int n = numberOfNodes();
	vector<bool> inserted(n);									// to avoid double insertion
	int i;
	for (i = 0; i < n; i++) inserted[i] = false;
	vector<CDFGnode *> node_list;								// to receive ordered nodes
	orderPredecessors<CDFGnode, CDFGedge>(sink(), inserted, node_list);				// order nodes now
	// now we scan the nodes
	for (i = 0; i < node_list.size(); i++)
	{
		CDFGnode *node = node_list[i];
		if (_debug)
		{
			cout << "CDFG::ASAP looking at node(" << node->index() << ")";
			node->info();
		}
		// scan all predecessors edges
		long max = 0;
		for (vector<CDFGedge*>::const_iterator e_it = node->predecessors.begin(); e_it != node->predecessors.end(); e_it++)
		{
			CDFGnode *source_node = (*e_it)->source;			// get source node
			if (_debug)
			{
				cout << "CDFG::ASAP  looking at predecessor(" << source_node->index() << ")";
				source_node->info();
			}
			//int source_index = source_node->index();
			long end = source_node->asap() + source_node->length();	// compute current source "asap"
			/* GL 20/12/08 : Complex operators */
			if(_sw->hSchedule() == "fine_grain") {

				if(node->type() == CDFGnode::OPERATION)
					end = abs( ((Operation *)node)->input_length(source_node->isPredecessorNumber(node), _clk) - source_node->asap());

				if(node->type() == CDFGnode::DATA) {
					if(source_node->type() == CDFGnode::OPERATION) {
						end = source_node->asap() + ((Operation *)source_node)->output_length(node->isSuccessorNumber(source_node), _clk);	// compute current source "asap"
						cout << "source: " << source_node->name() << " SN : " << node->isSuccessorNumber(source_node) << " lenghth : " << ((Operation *)source_node)->output_length(node->isSuccessorNumber(source_node), _clk);
					}}
			}
			/* Fin GL*/
			if (end > max) max = end;							// check for maximum
			if (_debug) cout << "CDFG::ASAP  asap=" << source_node->asap() << " length=" << source_node->length() << " new max=" << max << endl;
		}
		if (node->alap_locked() && (max > node->alap()))  		// sanity check
		{
			cout << "Info: alap time " << node->alap() << " of '" << node->name();
			cout << "' is locked and smaller than scheduled asap " << max << endl;
			return false;
		}
		if (node->asap_locked() && (max > node->asap()))  		// sanity check
		{
			cout << "Info: asap time " << node->asap() << " of '" << node->name();
			cout << "' is locked and is smaller than scheduled asap " << max << endl;
			return false;
		}
		if (!node->asap_locked()) node->asap(max);				// set asap time of node with max
		if (_debug) cout << "CDFG::ASAP OK done, asap = " << max << endl << endl;
	}
	return true;
}

bool CDFG::ALAP(long stop)
{
	// build the evaluation order with a DFS search in the graph
	int n = numberOfNodes();
	vector<bool> inserted(n);									// to avoid double insertion
	int i;
	for (i = 0; i < n; i++) inserted[i] = false;
	vector<CDFGnode *> node_list;								// to receive ordered nodes
	orderSuccessors<CDFGnode, CDFGedge>(source(), inserted, node_list);				// order nodes now
	// now we scan the nodes
	for (i = 0; i < node_list.size(); i++)
	{
		CDFGnode *node = node_list[i];
		if (_debug)
		{
			cout << "CDFG::ALAP looking at node(" << node->index() << ")";
			node->info();
		}
		// scan all successors edges
		long min = stop;
		for (vector<CDFGedge*>::const_iterator e_it = node->successors.begin(); e_it != node->successors.end(); e_it++)
		{
			CDFGnode *target_node = (*e_it)->target;			// get target node
			if (_debug)
			{
				cout << "CDFG::ALAP  looking at successor(" << target_node->index() << ")";
				target_node->info();
			}
			//int target_index = target_node->index();
			if (target_node->alap() < min) min = target_node->alap();
			if (_debug) cout << "CDFG::ALAP  alap=" << target_node->asap() << " new min=" << min << endl;
			/* GL 20/12/08 : Complex operators */
			if(_sw->hSchedule() == "fine_grain") {
				if(node->type() == CDFGnode::DATA) {
					if(target_node->type() == CDFGnode::OPERATION)
						min += ((Operation *)target_node)->input_length(node->isPredecessorNumber(target_node), _clk);
				}
				if(node->type() == CDFGnode::OPERATION) {
					if(target_node->type() == CDFGnode::DATA)
						min += node->length() - ((Operation *)node)->output_length(target_node->isSuccessorNumber(node), _clk);
				}
			}
			/* Fin GL*/
		}
		min -= node->length();								// update my ALAP
		if (node->asap_locked() && (min < node->asap()))  	// sanity check
		{
			cout << "Info: asap time " << node->asap() << " of '" << node->name();
			cout << "' is locked and is smaller then schedules alap " << min << endl;
			return false;
		}
		if (node->alap_locked() && (min < node->alap()))
		{
			cout << "Info: alap time " << node->alap() << " of '" << node->name();
			cout << "' is locked and is greater than schedules alap " << min << endl;
			return false;
		}
		if (!node->alap_locked()) node->alap(min);			// set asap time of node with min
		if (_debug) cout << "CDFG::ALAP OK done, alap = " << min << endl << endl;
	}
	return true;
}

// Reset all asap and alap times to 0, excepted the "locked" ones (they are user defined)
// CAUTION:
void CDFG::reset(const SelectionOutRefs *mdos)
{
	vector<CDFGnode*>::iterator n_it;
	CDFGnode *n;
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		n = *n_it;
		if (!n->asap_locked()) n->asap(0);
		if (!n->alap_locked()) n->alap(0);
		n->cycles(mdos,_clk ,_sw->memory_access_time());	// propagate to all nodes
	}
}

/* CONVERSION

   HYPOTHESIS
   data are 100 % correct
   data, operation and control names are unique
   a single source
   a single sink
   all targets of sink and source exits
   all read and write targets of operations exist
   all function ports exist

   1/ Conversion rules are:

   for each parsed object P

   if type of P is "source"
   create a new Control instance C, with name P._name
   create a new CDFGnode instance of type (SOURCE, C)
   endif

   if type of P is "sink"
   create a new Control instance C, with name P._name
   create a new CDFGnode instance of type (SINK, C)
   endif

   if type of P is "input", "output" or "variable"
   find existing Data or create a new Data instance D
   create a new CDFGnode instance of type (DATA, D)
   endif

   if type of P is "operation"
   find existing Function of name = (value of attribute "function" of P) F
   create a new Operation instance O
   O._function					= F
   create a new CDFGnode instance of type (OPERATION, O)
   endif

   endfor

   2/ Connect nodes via edges

*/

#include "operation.h"
#include "control.h"
#include "data.h"

void CDFG::connectIO(CDFGnodeRefs &cdfg_nodes_by_name, CDFGnode *node, const ParseObject *object, const string &io, const string &io_map, bool input)
{
	const ParseAttribute *att_target, *att_map;
	att_target = object->attribute(io);
	//att_target->info();
	for (int pos = 0; pos < att_target->_values.size(); pos++)
	{
		const Port *p, *other_p;
		string target_name	= att_target->value(pos);
		//cout << "target = " << target_name << endl;
		// value can be a Data node name
		// or an "operation_name.port_name"
		CDFGnode *target_node;
		string op_name, port_name;
		Parser::split('.', target_name, op_name, port_name);
		if (port_name.size() != 0)   // "operation_name.port_name" case
		{
			cdfg_nodes_by_name.search(op_name, &target_node);
			//other_p = target_node->operation()->function()->getPortByName(port_name);
			// dynamic link here !
			other_p = ((Operation*)target_node)->function()->getPortByName(port_name);
		}
		else   // Data node case
		{
			cdfg_nodes_by_name.search(target_name, &target_node);
			other_p = 0;
		}
		//target_node->info();
		//const Operation *o = node->operation();
		// dynamic link here
		const Operation *o = (Operation*)node;
		//cout << "operation = " << o->name() << endl;
		const Function *f = o->function();
		//cout << "function = " << f->name() << endl;
		CDFGedge *e = new CDFGedge();
		if (object->getAttribute(io_map, &att_map))   // explicit mapping by name
		{
			//cout << "with map" << endl;
			string map_name	= att_map->value(pos);
			//cout << "map name = " << map_name << endl;
			if (input)	p = f->getInputPortByName(map_name);
			else		p = f->getOutputPortByName(map_name);


			// dynamic memory EJ 04/07/2008
			if (!p && (f->name() == "mem_read" || f->name() == "mem_write"))
			{
				p = new Port(io_map, input?Port::IN:Port::OUT, pos, f);
				((Operation*)o)->port_number (pos+1);
			}
			else
				// end dynamic memory


				if (!p)
				{
					cerr << "Error in node '" << node->name() << "': no '" << map_name << "' ";
					cerr << (input ? "input" : "output") << " port in function '" << f->name() << "'" << endl;
					exit(1);
				}
		}
		else
		{
			//cout << "without map" << endl;
			if (input) p = f->getInputPortByPos(pos);
			else       p = f->getOutputPortByPos(pos);


			// dynamic memory EJ 04/07/2008
			if (!p && (f->name() == "mem_read" || f->name() == "mem_write"))
			{
				p = new Port(io_map, input?Port::IN:Port::OUT, pos, f);
				((Operation*)o)->port_number (pos+1);
			}
			else
				// end dynamic memory


				if (!p)
				{
					cerr << "Error in node '" << node->name() << "': no ";
					cerr << (input ? "input" : "output") << " port # function '" << f->name() << "'" << endl;
					exit(1);
				}
		}
		if (other_p && input)
		{
			delete e;	// remove it
			continue;	// don't process input side when both are operations to avoid double edges ...
		}
		if (input)
		{
			e->targetPort(p);
			connect(e, target_node, node);
			if (other_p) // target node is another operation
				e->sourcePort(other_p);
		}
		else
		{
			e->sourcePort(p);
			const Data* target_data= (const Data *)target_node;
			const ParseAttribute *dynamicAccess_att= (const ParseAttribute *)target_data->_addr3;
			if ((f->name() == "mem_write") && (dynamicAccess_att==NULL))
			{
				cerr << "Error in node '" << node->name() << "': no ";
				cerr << "Invalid cdfg: output of  # function '" << f->name() << "' is not a dynamic access : " <<  target_data->name() <<  endl;
				exit(1);
			}
			else
				connect(e, node, target_node);
			if (other_p) // target node is another operation
				e->targetPort(other_p);
		}
		addEdge(e);
	}
}

void CDFG::checkBank(const ParseObject *object, Data *d)
{
	const ParseAttribute *a;
	if (object->getAttribute("bank", &a) && ((_mapping_constraints == true) || /*DH : 11/10/2008 */ (d->dynamic_address()!=-1) || (d->_addr3)))   // found a alap constraint
	{
		long bank = Parser::atol(a->_values[0]);
		long address = Parser::atol(object->attribute("address")->_values[0]);
		d->setBank(bank, address);
	}
}

/* GL 18/09/08 : add lines */
void CDFG::checkPort (const ParseObject *object, Data *d)
{
	const ParseAttribute *a;
	if (object->getAttribute("port", &a) && _io_constraints_detected == true)   // found a IO constraints
	{
		long port = Parser::atol(a->_values[0]);
		d->port(port);
	}
}
/* End GL */


void CDFG::setFunction(CDFGnodeRefs &cdfg_nodes_by_name, CDFGnode *node, const ParseObject *object, const FunctionRefs &functions) const
{
	Operation *o = (Operation*)node;
	if (_bitwidth_Aware)
	{
		string function_name = o->function_name();
		const ParseAttribute *att_target;
		att_target = object->attribute("read");
		int pos;
		for (pos = 0; pos < att_target->_values.size(); pos++)
		{
			const Port *p, *other_p;
			string target_name	= att_target->value(pos);
			CDFGnode *target_node;
			string op_name, port_name;
			Parser::split('.', target_name, op_name, port_name);
			if (port_name.size() != 0)   // "operation_name.port_name" case
			{
				cdfg_nodes_by_name.search(op_name, &target_node);
				other_p = ((Operation*)target_node)->function()->getPortByName(port_name);
			}
			else   // Data node case
			{
				cdfg_nodes_by_name.search(target_name, &target_node);
				other_p = 0;
			}
			if (((Data*)target_node)->getSigned() == Data::SIGNED || ((Data*)target_node)->getSigned() == Data::SFIXED) function_name += "$s";
			else if (((Data*)target_node)->getSigned() == Data::UNSIGNED || ((Data*)target_node)->getSigned() == Data::UFIXED) function_name += "$u";
			if ((_clusteringMode == 1) || (_clusteringMode == 0))
			{
				ostringstream bitwidth;
				bitwidth << ((Data*)target_node)->bitwidth();
				function_name += bitwidth.str();
			}
		}
		att_target = object->attribute("write");
		for (pos = 0; pos < att_target->_values.size(); pos++)
		{
			const Port *p, *other_p;
			string target_name	= att_target->value(pos);
			CDFGnode *target_node;
			string op_name, port_name;
			Parser::split('.', target_name, op_name, port_name);
			if (port_name.size() != 0)   // "operation_name.port_name" case
			{
				cdfg_nodes_by_name.search(op_name, &target_node);
				other_p = ((Operation*)target_node)->function()->getPortByName(port_name);
			}
			else   // Data node case
			{
				cdfg_nodes_by_name.search(target_name, &target_node);
				other_p = 0;
			}
			if (((Data*)target_node)->getSigned() == Data::SIGNED || ((Data*)target_node)->getSigned() == Data::SFIXED) function_name += "$s";
			else if (((Data*)target_node)->getSigned() == Data::UNSIGNED || ((Data*)target_node)->getSigned() == Data::UFIXED) function_name += "$u";
			if ((_clusteringMode == 1) || (_clusteringMode == 0))
			{
				ostringstream bitwidth;
				bitwidth << ((Data*)target_node)->bitwidth();
				function_name += bitwidth.str();
			}
		}
		// search function with same functionnality and nearest bitwidth
		long last_bitwidth = 0, bitwidth;
		bool smallest = true, first = true;
		if (_clusteringMode == 2)
		{
			for (FunctionRefs::const_iterator f_it = functions.begin(); f_it!=functions.end(); f_it++)
			{
				bool OK = true;
				Function *f = (*f_it).second;
				if (f->symbolic_function() != o->function_name()) continue; //next

				const ParseAttribute *att_target;

				att_target = object->attribute("read");
				int pos;
				bitwidth = 0;
				for (pos = 0; ((pos < att_target->_values.size()) && (OK==true)); pos++)
				{
					const Port *p, *other_p;
					string target_name	= att_target->value(pos);
					CDFGnode *target_node;
					string op_name, port_name;
					Parser::split('.', target_name, op_name, port_name);
					if (port_name.size() != 0)   // "operation_name.port_name" case
					{
						cdfg_nodes_by_name.search(op_name, &target_node);
						other_p = ((Operation*)target_node)->function()->getPortByName(port_name);
					}
					else   // Data node case
					{
						cdfg_nodes_by_name.search(target_name, &target_node);
						other_p = 0;
					}

					// EJ 07/07/2008 : levinson
					if (f->mem_access())
					{
						OK =  (((Data*)target_node)->bitwidth() <= f->getInputPortByPos(0)->bitwidth());
						bitwidth += f->getInputPortByPos(0)->bitwidth();
					}
					else
					{
						OK =  (((Data*)target_node)->bitwidth() <= f->getInputPortByPos(pos)->bitwidth());
						bitwidth += f->getInputPortByPos(pos)->bitwidth();
					}
				}

				if (OK)
				{
					if (!first)
						smallest = (bitwidth < last_bitwidth);
					else
					{
						smallest = true;
						first = false;
					}
				}

				att_target = object->attribute("write");

				for (pos = 0; ((pos < att_target->_values.size()) && (OK==true)); pos++)
				{
					const Port *p, *other_p;
					string target_name	= att_target->value(pos);
					CDFGnode *target_node;
					string op_name, port_name;
					Parser::split('.', target_name, op_name, port_name);
					if (port_name.size() != 0)   // "operation_name.port_name" case
					{
						cdfg_nodes_by_name.search(op_name, &target_node);
						other_p = ((Operation*)target_node)->function()->getPortByName(port_name);
					}
					else   // Data node case
					{
						cdfg_nodes_by_name.search(target_name, &target_node);
						other_p = 0;
					}
					OK =  (((Data*)target_node)->bitwidth() <= f->getOutputPortByPos(pos)->bitwidth());
				}
				if ((OK) && (smallest))
				{
					function_name = f->name();
					last_bitwidth = bitwidth;
				}
			}
		}
		Function *f;
		if (functions.search(function_name, &f) == false)
		{
			functions.info();
			cerr << "Fatal error: function " << function_name << " doesn't exist" << endl;
			exit(1);
		}
		o->function(f);

	}
	else
	{
		Function *f;

		if (functions.search(o->function_name(), &f) == false)
		{
			cerr << "Fatal error: function " << o->function_name() << " doesn't exist" << endl;
			exit(1);
		}
		o->function(f);
	}
}

// EJ 14/02/2008 : dynamic memory
bool dynamic_element_sort (const Data* a, const Data* b)
{
	/*  check mapping
		if (a->bank() != -1 && a->bank() == b->bank() &&
	/*  a->address() == b->address()) ||
	a->dynamic_address() == b->dynamic_address())
	{
	cout << "fatal error : check memory mapping" << endl;
	exit(1);
	} */
	// return sort
	return a->dynamic_address() < b->dynamic_address();
}

void CDFG::convert(const ParseObjects *data, const FunctionRefs &functions)
{

	// temporary data bases for data consistency check
	CDFGnodeRefs cdfg_nodes_by_name;

	// 1rst phase
	for (ParseObjects::const_iterator it = data->begin(); it != data->end(); it++)
	{
		const ParseObject *object = (*it).second;
		CDFGnode *node;
		if (object->_type == "constant")
		{
			Data *d = new Data(object->_name, Data::CONSTANT);
			//cout << "constant " << d->name() << endl;
			// get value
			long value;

			/* EJ 19/12/2007 : read ac_fixed attribute */
			const ParseAttribute *fixedpoint, *quantization, *overflow, *bitwidth, *signed_mode;
			if (object->getAttribute("fixedpoint", &fixedpoint) && fixedpoint->value(0) != "-999")   // found fixedpoint attribute
			{
				quantization = object->attribute("quantization");
				overflow = object->attribute("overflow");
				bitwidth = object->attribute("bitwidth");
				signed_mode = object->attribute("signed");
				// mentor_type
				BIT_VECTOR bv;
				SIGNED_MODE signedMode;
				QUANTIZATION_MODE quantizationMode;
				OVERFLOW_MODE overflowMode;

				if (signed_mode->value(0) == "1")
					signedMode = SIGNED_M;
				else
					signedMode = UNSIGNED_M;

				quantizationMode = Mentor_type::getQuantizationMode((char*)quantization->value(0).c_str());
				overflowMode = Mentor_type::getOverflowMode((char*)overflow->value(0).c_str());

				bv = Mentor_type::atobv((char*)(object->attribute("value")->_values[0]).c_str(),
										Parser::atol(bitwidth->value(0)),
										signedMode,
										Parser::atol(fixedpoint->value(0)),
										quantizationMode,
										overflowMode);

				// convert fixed value to long value
				value = Mentor_type::bvtoi(bv,Parser::atol(bitwidth->value(0)),signedMode);

				/* End EJ */
			}
			else
			{
				value = Parser::atol(object->attribute("value")->_values[0]);

			}
			/* GL 05/10/07 : Fixed precision */
			d->fixedPoint(-999);
			/* Fin GL */
			d->value(value);

			//cout << "value " << d->value() << endl;
			//addNode(node = new CDFGnode(d));

			// EJ 26/10/2007 : hardwire constant
			bool hardwire = false; // by default memory constant
			const ParseAttribute *h;
			if (object->getAttribute("hardwire", &h) )   // found a hardwire attribut
			{
				hardwire = object->attribute("hardwire")->_values[0] == "1";
			}

			// EJ 14/02/2008 : dynamic memory
			const ParseAttribute *dynamicAccess_att;
			if (object->getAttribute("dynamicAccess", &dynamicAccess_att))
			{
				d->_addr3 = (void*)dynamicAccess_att;
				const ParseAttribute *sizeDynamicAccess_att;
				if (object->getAttribute("size", &sizeDynamicAccess_att))
				{
					d->dynamic_access(d);
					d->new_dynamic_elements(Parser::atol(sizeDynamicAccess_att->value(0)));
				}
			}
			const ParseAttribute *dynamicElement_att;
			if (object->getAttribute("dynamicElement", &dynamicElement_att))
			{
				d->_addr3 = (void*)dynamicElement_att;
				const ParseAttribute *dynamicAddress_att;
				dynamicAddress_att = object->attribute("dynamicAddress");
				d->dynamic_address(Parser::atol(dynamicAddress_att->value(0)));
				if (hardwire)
				{
					cout << "fatal error : a constant is hardwire and it has a dynamic memory access" << endl;
					exit(1);
				}
			}
			// End EJ : Dynamic access

			// check bank, if any and if no hardwire
			if (!hardwire) checkBank(object, d);
			else d->hardwire(true);

			// dynamic link here
			addNode(node = d);
		}
		else if (object->_type == "variable" || object->_type == "temporary")
		{
			const ParseAttribute *resetvalueattr;
			Data *d = new Data(object->_name, Data::VARIABLE);
			// get reset value
			long resetvalue=0;
			if (object->getAttribute("value",&resetvalueattr))
			{
				/* EJ 19/12/2007 : read ac_fixed attribute */
				const ParseAttribute *fixedpoint, *quantization, *overflow, *bitwidth, *signed_mode;
				if (object->getAttribute("fixedpoint", &fixedpoint) && fixedpoint->value(0) != "-999")   // found fixedpoint attribute
				{
					quantization = object->attribute("quantization");
					overflow = object->attribute("overflow");
					bitwidth = object->attribute("bitwidth");
					signed_mode = object->attribute("signed");
					// mentor_type
					BIT_VECTOR bv;
					SIGNED_MODE signedMode;
					QUANTIZATION_MODE quantizationMode;
					OVERFLOW_MODE overflowMode;
					if (signed_mode->value(0) == "1")
						signedMode = SIGNED_M;
					else
						signedMode = UNSIGNED_M;
					quantizationMode = Mentor_type::getQuantizationMode((char*)quantization->value(0).c_str());
					overflowMode = Mentor_type::getOverflowMode((char*)overflow->value(0).c_str());
					bv = Mentor_type::atobv((char*)(object->attribute("value")->_values[0]).c_str(),
											Parser::atol(bitwidth->value(0)),
											signedMode,
											Parser::atol(fixedpoint->value(0)),
											quantizationMode,
											overflowMode);
					// convert fixed value to long value
					resetvalue = Mentor_type::bvtoi(bv,Parser::atol(bitwidth->value(0)),signedMode);
					/* End EJ */
				}
				else
				{
					resetvalue = Parser::atol(object->attribute("value")->_values[0]);
				}
			}
			//cout << "variable " << d->name() << endl;
			// check for loopback
			const ParseAttribute *loopback_att;
			if (object->getAttribute("loopback", &loopback_att))
			{
				// keep a temporary pointer to the attribute !
				//cout << "with loopback" << endl;
				d->_addr2 = (void*)loopback_att;
			}
			const ParseAttribute *aging_att;
			if (object->getAttribute("aging", &aging_att))
			{
				// keep a temporary pointer to the attribute !
				//cout << "with aging" << endl;
				//d->aging((Data *)aging_att);
				d->_addr2 = (void*)aging_att;
				d->_aging = true;
			}

			// EJ 14/02/2008 : dynamic memory
			const ParseAttribute *dynamicAccess_att;
			if (object->getAttribute("dynamicAccess", &dynamicAccess_att))
			{
				d->_addr3 = (void*)dynamicAccess_att;
				const ParseAttribute *sizeDynamicAccess_att;
				if (object->getAttribute("size", &sizeDynamicAccess_att))
				{
					d->dynamic_access(d);
					d->new_dynamic_elements(Parser::atol(sizeDynamicAccess_att->value(0)));
				}
			}
			const ParseAttribute *dynamicElement_att;
			if (object->getAttribute("dynamicElement", &dynamicElement_att))
			{
				d->_addr3 = (void*)dynamicElement_att;
				const ParseAttribute *dynamicAddress_att;
				dynamicAddress_att = object->attribute("dynamicAddress");
				d->dynamic_address(Parser::atol(dynamicAddress_att->value(0)));
			}
			// End EJ : Dynamic access
			d->resetvalue(resetvalue);
			/* GL 05/10/07 : Fixed precision */
			d->fixedPoint(-999);
			/* Fin GL */
			//addNode(node = new CDFGnode(d));
			// check bank, if any
			checkBank(object, d);
			// dynamic link here
			addNode(node = d);
		}
		else if ((object->_type == "input")||(object->_type == "output"))
		{
			Data *d;
			if (object->_type == "input")			d = new Data(object->_name, Data::INPUT);
			else if (object->_type == "output")		d = new Data(object->_name, Data::OUTPUT);
			//cout << "IO " << d->name() << endl;
			/* GL 05/10/07 : Fixed precision */
			d->fixedPoint(-999);
			/* Fin GL */
			/* GL 08/09/08 : add lines */
			checkPort(object, d); //TO_VERIFY
			//addNode(node = new CDFGnode(d));
			// dynamic link here
			addNode(node = d);
		}
		else if (object->_type == "operation")
		{
			Operation *o = new Operation(object->_name, object->attribute("function")->_values[0]);





			/* Caaliph 27/06/2007 */
			const ParseAttribute *att_start;
			object->getAttribute("start", &att_start);
			if (att_start!=NULL)
			{
				o->start(Parser::atol(att_start->value(0)));
			}
			const ParseAttribute *att_operator;
			object->getAttribute("identify", &att_operator);
			if (att_operator!=NULL)
			{
				o->identify(Parser::atol(att_operator->value(0)));
			}
			const ParseAttribute *name_operator;
			object->getAttribute("operator", &name_operator);
			if (name_operator!=NULL)
			{
				o->name_oper(name_operator->_name);
			}
			/* End Caaliph */
			//addNode(node = new CDFGnode(o));
			// dynamic link here
			addNode(node = o);
		}
		else if ((object->_type == "source")||(object->_type == "sink"))
		{
			Control::Type type;
			if (object->_type == "source")			type = Control::SOURCE;
			else if (object->_type == "sink")		type = Control::SINK;
			Control *c = new Control(object->_name, type);
			//addNode(node = new CDFGnode(c));
			c->_addr2 = (void*)object->attribute("targets"); // EJ 05/03/2008
			// dynamic link here
			addNode(node = c);
			switch (type)
			{
			case Control::SOURCE:
				_source = node;
				break;
			case Control::SINK:
				_sink = node;
				break;
			}
			/* Caaliph 27/06/2007 */
		}
		else if (object->_type == "mode")
		{
			;
			/* End Caaliph */
		}
		else
		{
			cout << "Warning: type '" << object->_type << "'" << endl;
			continue;//exit(1);
		}

		// keep in the node a temporary pointer to the parse object
		// it will be used in a second phase to connect nodes via edges
		node->_addr = (void *)object;
		/* Caaliph 27/06/2007 */
		if (object->_type!="mode")
			/* End Caaliph */
			cdfg_nodes_by_name.add(node);

		const ParseAttribute *a;
		if (_io_constraints_detected)
		{
			// check time constraint (IO nodes only now)
			if (object->getAttribute("time", &a))   // found a time constraint
			{
				long time = Parser::atol(a->value(0));
				// Caution: for Data nodes only; in the CDFGnode asap = alap = data.time !
				// dynamic link here
				((Data*)node)->time(time);		 // node asap & alap = data time
				//_io_constraints_detected = true;	// remember this for later ASAP/ALAP
				if (object->getAttribute("length", &a))
				{
					//long length = Parser::atol(a->value(0));
					//((Data*)node)->alap_force(time + length);
				}
			}
		}

		// check asap/alap constraints on operations and control nodes
		if (object->getAttribute("asap", &a))   // found a asap constraint
		{
			long asap = Parser::atol(a->value(0));
			// Caution: for Data nodes only; in the CDFGnode asap = alap = data.time !
			node->asap(asap);		 // node asap & alap = data time
			node->asap_locked(true);
		}
		if (object->getAttribute("alap", &a))   // found a alap constraint
		{
			long alap = Parser::atol(a->value(0));
			// Caution: for Data nodes only; in the CDFGnode asap = alap = data.time !
			node->alap(alap);		 // node asap & alap = data time
			node->alap_locked(true);
		}

		// check bitwidth/signed on input, output, constant, variable nodes
		if (object->getAttribute("bitwith", &a) || object->getAttribute("bitwidth", &a))
		{
			// get bitwidth
			long _bitwidth = Parser::atol(a->_values[0]);
			((Data*)node)->bitwidth(_bitwidth);
		}
		/* GL 05/10/07 : Fixed precision */
		// check fixed precision attributes
		if (object->getAttribute("fixedpoint", &a))
		{
			_fixed_detected = true;
			//get fixedpoint
			long _fixedpoint = Parser::atol(a->_values[0]);
			((Data*)node)->fixedPoint(_fixedpoint);

			// get quantization mode
			if (object->getAttribute("quantization", &a))
			{
				string _quantization  = a->_values[0];
				((Data*)node)->quantization(_quantization);
			}

			// get overflow mode
			if (object->getAttribute("overflow", &a))
			{
				string _overflow   = a->_values[0];
				((Data*)node)->overflow (_overflow);
			}
		}
		/* Fin GL */
		if (object->getAttribute("signed", &a))
		{
			// get signed
			string _signed = a->value(0);
			if (_signed=="s" || _signed=="1")
			{
				/* GL 05/10/07 : Fixed precision */
				if (((Data*)node)->fixedPoint() != -999)
					((Data*)node)->setSigned(Data::SFIXED);
				else
					/* Fin GL */
					((Data*)node)->setSigned(Data::SIGNED);
			}
			else if (_signed=="u"|| _signed=="0")
			{
				/* GL 05/10/07 : Fixed precision */
				if (((Data*)node)->fixedPoint()!=-999)
					((Data*)node)->setSigned(Data::UFIXED);
				else
					/* Fin GL */
					((Data*)node)->setSigned(Data::UNSIGNED);
			}
			else
			{
				cerr << "Error: unknown value '" << _signed << "' in attribute '" << a->_name << "'" << endl;
				exit(1);
			}
		}
	}

	// now all nodes exist !

	// 2nd phase:
	//		replace loopback and aging by the right pointer
	//		add variable which must store in memory unit to source & sink node
	int i;
	for (i = 0; i < nodes().size(); i++)
	{
		int pos;
		Data *d;
		Control *c;
		// get node
		CDFGnode *node = nodes()[i];
		// get parse object which generated the node
		const ParseObject *object = (const ParseObject *) node->_addr;
		const ParseAttribute *att;
		// look at node type
		//cout << "node " << node->name() << endl;
		switch (node->type())
		{
		case CDFGnode::DATA:
			d = (Data*) node; // dynamic link here
			switch (d->type())
			{
			case Data::VARIABLE:

				//att = (const ParseAttribute *)d->writevar();
				att = (const ParseAttribute*) d->_addr2;
				if (att)
				{
					if (d->aging() == true)
					{
						const CDFGnode *aging_target = searchNode(att->_values[0]);
						if (!aging_target) break;
						Data *aging_target_data = (Data*)aging_target; // dynamic link
						//d->aging(aging_target_data);
						d->loopback(aging_target_data);
						//
						// ADDED by DANIEL
						// make sure the _writevar property has its aging feature properly stored
						//aging_target_data->_aging = true;
						//
						//cout << "node " << node->name() << " has aging " << d->loopback()->name() << endl;
						break;
					}
					else
					{
						const CDFGnode *loopback_target = searchNode(att->_values[0]);
						if (!loopback_target) break;
						Data *loopback_target_data = (Data*)loopback_target; // dynamic link
						d->loopback(loopback_target_data);
						//cout << "node " << node->name() << " has loopback " << d->loopback()->name() << endl;
						break;
					}
				}
				/* Done by compiler
				// EJ : 05/03/2008 ajout des variables dans les noeuds source et sink si la variable
				// doit être en mémoire (attribut bank)
				if (d->bank() != -1 || d->dynamic_address() >=0)
				{
				att = (const ParseAttribute*) _source->_addr2;
				((ParseAttribute*)att)->add(object->_name);
				att = (const ParseAttribute*) _sink->_addr2;
				((ParseAttribute*)att)->add(object->_name);
				cout << "variable " << object->_name << " add in source/sink node" << endl;
				}
				// End EJ
				*/
				// EJ 14/02/2008 : dynamic memory
				//case Data::CONSTANT:
				att = (const ParseAttribute*) d->_addr3;
				if (att)
				{
					// this node is a dynamic access or a dynamic element
					const CDFGnode *dynamic_target = searchNode(att->_values[0]);
					if (!dynamic_target)
					{
						cout << "fatal error : node " << att->_values[0] << "doesn't find" << endl;
						exit(1);
					}
					Data *dynamic_target_data = (Data*)dynamic_target; // dynamic link
					// this node is a dynamic access
					d->dynamic_access(dynamic_target_data);
					if (d->dynamic_address() >=0)
					{
						d->dynamic_elements(d);
						sort(d->dynamic_access()->dynamic_elements()->begin(),
							 d->dynamic_access()->dynamic_elements()->end(), dynamic_element_sort);
						if (_debug)
							cout << "node " << node->name() << " has dynamic element " << dynamic_target_data->name() << endl;
					}
					else
					{
						if (_debug)
							cout << "node " << node->name() << " has dynamic access " << dynamic_target_data->name() << endl;
					}
					break;
				}
				// End EJ

				break;
			}
			break;
		case CDFGnode::OPERATION:
			setFunction(cdfg_nodes_by_name, node, object, functions);
			connectIO(cdfg_nodes_by_name, node, object, "read", "read_map", true);		// process all inputs
			connectIO(cdfg_nodes_by_name, node, object, "write", "write_map", false);	// process all outputs
			break;
		case CDFGnode::CONTROL:
			// nothing here
			break;
		}
	}

	// 3e phase:
	//		connect nodes via edges
	for (i = 0; i < nodes().size(); i++)
	{
		int pos;
		Data *d;
		Control *c;
		// get node
		CDFGnode *node = nodes()[i];
		// get parse object which generated the node
		const ParseObject *object = (const ParseObject *) node->_addr;
		const ParseAttribute *att;
		// look at node type
		//cout << "node " << node->name() << endl;
		if (node->type() == CDFGnode::CONTROL)
		{
			c = (Control*) node; // dynamic link
			c->_addr2 = 0; //clear dynamic link
			const ParseAttribute *att_target = object->attribute("targets");
			for (pos = 0; pos < att_target->_values.size(); pos++)
			{
				string target_name	= att_target->value(pos);
				CDFGnode *target_node;
				cdfg_nodes_by_name.search(target_name, &target_node);
				CDFGedge *e = new CDFGedge();
				switch (c->type())
				{
				case Control::SOURCE:
					connect(e, node, target_node);
					break;
				case Control::SINK:
					connect(e, target_node, node);
					break;
				}
				addEdge(e);
			}
		} else if (node->type() == CDFGnode::DATA && ((Data*)node)->type() ==Data::VARIABLE) {
			// ------------------------------------------------------------------
			// ADDED by Daniel to be able to insert registers after an operation
			// ------------------------------------------------------------------
			// the register is added by entering the CDFG in the following format:
			// temporary(op_dummy_out) {
			//		bitwidth 32;
			// }
			// variable(op_real_out) {
			//		aging op_dummy_out;
			//		bitwidth 32;
			// }
			// operation(op) {
			//		function any_valid_op;
			//		read list_of_inputs;
			//		write op_dummy_out;
			// }
			//
			// NOTE: the variable op_real_out MUST not be declare in the source list of operations
			Data* d = (Data*) node;
			if (d->_addr2 && d->aging()) {
				const ParseAttribute* part = (const ParseAttribute*)d->_addr2;
				CDFGnode *target_node = const_cast<CDFGnode*>(searchNode(part->_values[0]));
				if (target_node) {
					CDFGedge* e = new CDFGedge();
					connect(e,target_node,node);
					addEdge(e);
				}
			}
		}
	}
}





















/* Caaliph: 27/06/2007 Modified serialize function: utile to generate ucom files for STAR */
/*
   void CDFG::serialize(const string &file_name) const {
   vector<CDFGnode*>::const_iterator n_it;
   const CDFGnode *n;
// open
ofstream f(file_name.c_str(), ios::out);
for(n_it = nodes().begin(); n_it != nodes().end(); n_it++) {
(*n_it)->serialize(f);	// dynamic link here works fine :-)

}
// close
f.close();
}
*/
void CDFG::serialize(const string &file_name) const
{
	vector<CDFGnode*>::const_iterator n_it;
	const CDFGnode *n;
	long identification;
	const Operation *op;
	const Switches *sw;
	int j=0;
	ofstream f(file_name.c_str(), ios::out);
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			if (op->function()->passThrough())
			{
				(*n_it)->serialize(f);
				f<<"}"<<"\n";
				continue;
			}
			string name;
			(*n_it)->serialize(f);	// dynamic link here works fine :-)
			identification = op->inst()->no();
			get_operator_name(op, &name);
			f<< "  identify "<<identification<<";"<< "\n";
			f<< "  operator comp_"<<identification<<"_"<<name<<";"<<"\n";
			f << "}" << "\n";
		}
		else (*n_it)->serialize(f);
	}
	// close
	f.close();
}

void CDFG::serialize(const string &file_name, const string &mode) const
{
	vector<CDFGnode*>::const_iterator n_it;
	const CDFGnode *n;
	long identification;
	const Operation *op;
	const Switches *sw;
	int j=0;
	ofstream f(file_name.c_str(), ios::out);
	f<<"mode("<<mode<<")"<<"\n";
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			if (op->function()->passThrough())
			{
				(*n_it)->serialize(f);
				f<<"}"<<"\n";
				continue;
			}
			string name;
			(*n_it)->serialize(f);	// dynamic link here works fine :-)
			identification = op->inst()->no();
			get_operator_name(op, &name);
			f<< "  identify "<<identification<<";"<<"\n";
			f<< "  operator comp_"<<identification<<"_"<<name<<";"<<"\n";
			f << "}" << "\n";
		}
		else (*n_it)->serialize(f);
	}
	// close
	f.close();
}

void CDFG::serialize(const string &file_name, vector<const CDFGnode*> *v,const string &mode) const
{
	vector<CDFGnode*>::const_iterator n_it;
	vector<const CDFGnode*>::const_iterator v_it;
	const CDFGnode *n;
	long identifier;
	int i=0;
	int j=0;
	const Operation *op;
	bool bounded=false;
	ofstream f(file_name.c_str(), ios::out);
	f<<"mode("<<mode<<")"<<"\n";
	//f<<"mode "<<mode<<";"<<endl;
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			if (op->function()->passThrough())
			{
				(*n_it)->serialize(f);
				f<<"}"<<"\n";
				continue;
			}
			string name;
			(*n_it)->serialize(f);	// dynamic link here works fine :-)
			for (v_it=v->begin();v_it!=v->end();v_it++)
			{
				if ((*v_it)->name() ==op->name())
				{
					identifier = ((Operation*)(*v_it))->identify();
					bounded=true;
					v_it = v->end()-1;
				}
				else bounded = false;
			}
			get_operator_name(op, &name);
			if (bounded)
			{
				f<< "  identify "<<identifier<<";"<<"\n";
				f<< "  operator comp_"<<identifier<<"_"<<name<<";"<<"\n";
			}
			f << "}" << "\n";
		}
		else (*n_it)->serialize(f);
	}
	// close
	f.close();
}

void CDFG::serialize_STAR(const string &file_name, const string &prefix_name, Cadency *cadency) const
{
	vector<CDFGnode*>::const_iterator n_it;
	const CDFGnode *n;
	long identification;
	const Operation *op;
	vector<const Operation*> existing_FUs;
	const Switches *sw;
	int j=0;
	int count_op=0;
	long k=-1;
	ofstream f(file_name.c_str(), ios::out);
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			if (op->function()->passThrough())
			{
				((Operation*)(*n_it))->serialize_STAR(f, cadency);
				MWBM_MOD(op, &existing_FUs, &k, cadency);
				f<< "  identify "<<k<<";"<<"\n";
				f<<"  operator comp_"<<k<<"_assign;"<<"\n";
				f<<"}"<<"\n";
				continue;
			}
			string name;
			((Operation*)(*n_it))->serialize_STAR(f, cadency);	// dynamic link here works fine :-)
			identification = op->inst()->no();
			get_operator_name(op, &name);
			f<< "  identify "<<identification<<";"<<"\n";
			f<< "  operator comp_"<<identification<<"_"<</*op->function()->name()*/name<<";"<<"\n";
			f << "}" << "\n";
		}
		else if ((*n_it)->type() ==CDFGnode::DATA) ((Data*)(*n_it))->serialize(f, cadency, prefix_name, &j, &count_op);
		else ((Control*)(*n_it))->serialize_STAR(f, cadency);
	}
	// close
	f.close();
}

void CDFG::serialize_STAR(const string &file_name, const string &prefix_name, Cadency *cadency, const string &mode) const
{
	vector<CDFGnode*>::const_iterator n_it;
	const CDFGnode *n;
	long identification;
	const Operation *op;
	vector<const Operation*> existing_FUs;
	const Switches *sw;
	int j=0;
	int count_op=0;
	long k=-1;
	ofstream f(file_name.c_str(), ios::out);
	f<<"mode("<<mode<<")"<<"\n";
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			if (op->function()->passThrough())
			{
				((Operation*)(*n_it))->serialize_STAR(f, cadency);
				MWBM_MOD(op, &existing_FUs, &k, cadency);
				f<< "  identify "<<k<<";"<<"\n";
				f<<"  operator comp_"<<k<<"_assign;"<<"\n";
				f<<"}"<<"\n";
				continue;
			}
			string name;
			((Operation*)(*n_it))->serialize_STAR(f, cadency);	// dynamic link here works fine :-)
			identification = op->inst()->no();
			get_operator_name(op, &name);
			f<< "  identify "<<identification<<";"<<"\n";
			f<< "  operator comp_"<<identification<<"_"<<name<<";"<<"\n";
			f << "}" << "\n";
		}
		//else (*n_it)->serialize(f);
		else if ((*n_it)->type() ==CDFGnode::DATA) ((Data*)(*n_it))->serialize(f, cadency, prefix_name, &j, &count_op);
		else ((Control*)(*n_it))->serialize_STAR(f, cadency);
	}
	// close
	f.close();
}

void CDFG::serialize_STAR(const string &file_name, const string &prefix_name, Cadency *cadency, vector<const CDFGnode*> *v,const string &mode) const
{
	vector<CDFGnode*>::const_iterator n_it;
	vector<const CDFGnode*>::const_iterator v_it;
	const CDFGnode *n;
	long identifier;
	int i=0;
	int j=0;
	int count_op=0;
	long k=-1;
	const Operation *op;
	vector<const Operation*> existing_FUs;
	bool bounded=false;
	ofstream f(file_name.c_str(), ios::out);
	f<<"mode("<<mode<<")"<<"\n";
	//f<<"mode "<<mode<<";"<<"\n";
	for (n_it = nodes().begin(); n_it != nodes().end(); n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			if (op->function()->passThrough())
			{
				((Operation*)(*n_it))->serialize_STAR(f, cadency);
				MWBM_MOD(op, &existing_FUs, &k, cadency);
				f<< "  identify "<<k<<";"<<"\n";
				f<<"  operator comp_"<<k<<"_assign;"<<"\n";
				f<<"}"<<"\n";
				continue;
			}
			string name;
			((Operation*)(*n_it))->serialize_STAR(f, cadency);	// dynamic link here works fine :-)
			for (v_it=v->begin();v_it!=v->end();v_it++)
			{
				if ((*v_it)->name() ==op->name())
				{
					identifier = ((Operation*)(*v_it))->identify();
					bounded=true;
					v_it = v->end()-1;
				}
				else bounded = false;
			}
			get_operator_name(op, &name);
			if (bounded)
			{
				f<< "  identify "<<identifier<<";"<<"\n";
				f<< "  operator comp_"<<identifier<<"_"<<name<<";"<<"\n";
			}
			f << "}" << "\n";
		}
		//else (*n_it)->serialize(f);
		else if ((*n_it)->type() ==CDFGnode::DATA) ((Data*)(*n_it))->serialize(f, cadency, prefix_name, &j, &count_op);
		else ((Control*)(*n_it))->serialize_STAR(f, cadency);
	}
	// close
	f.close();
}
/* End Caaliph */

long CDFG::criticalLoop(const Data **critical) const
{
	// The critical loop is the computing time between a variable and its new value
	// If the critical loop is longer than the (cadency-mem_access_time), the CDFG is not schedulable.

	long loop = -1;		// no critical data
	*critical = 0; // no critical data
	for (int i = 0; i < nodes().size(); i++)
	{
		const CDFGnode *n = nodes()[i];			// get current CDFG node
		if (n->type() != CDFGnode::DATA) continue;		// search for DATA only
		const Data *Rv = (const Data*)n;		// read variable : dynamic link here
		if (Rv->type() != Data::VARIABLE) continue;	// search for VARIABLEs only
		if (!Rv->writevar() || Rv->aging() == true) continue;	// write variable : search for VARIABLEs with loopback only
		const Data *Wv = Rv->writevar();		// get loopback variable
		//long distance = Wv->asap() - Rv->alap();	// distance
		long distance;
		/*if (!Wv->backTo(Rv, distance)) {		// get path from Wv to Rv
		  cerr << "Internal error: no path from " << Wv->name() << " to " << Rv->name() << endl;
		  exit(1);
		  }*/
		if (!Wv->backTo(Rv, distance))  		// get path from Wv to Rv
		{
			//cout << "Internal error: no path from " << Wv->name() << " to " << Rv->name() << endl;
			cout << "WARNING: no path from " << Wv->name() << " to " << Rv->name() << endl;
			//exit(1);
			distance = Wv->asap() - Rv->alap();
		}
		// else OK, keep max path
		if (distance > loop)
		{
			*critical = Rv;
			loop = distance;
		}
	}
	return loop;
}

void CDFGnode::propagateAlapLock(long alap) const
{
	const CDFGnode *node = this;
	for (int i = 0; i < node->predecessors.size(); i++)
	{
		const CDFGedge *e = node->predecessors[i];
		const CDFGnode *p = e->source;
		switch (p->type())
		{
			Data *d;
			Operation *o;
			Control *c;
		case CDFGnode::DATA :
			d = (Data*)p;
			if (d->alap_locked() && d->alap() <= alap - d->length()) continue;

			d->alap(alap - d->length());
			d->alap_locked(true);
			break;
		case CDFGnode::OPERATION :
			o = (Operation*)p;
			o->alap(alap - o->length());
			o->alap_locked(true);
			break;
		case CDFGnode::CONTROL :
			c = (Control*)p;
			c->alap(alap - c->length());
			c->alap_locked(true);
			break;
		}
		p->propagateAlapLock(p->alap());
	}
}

/* EJ 26/06/2007 */
void CDFG::propagateLocks() const
{
	for (int i = 0; i < sink()->predecessors.size(); i++)
	{
		const CDFGedge *e = sink()->predecessors[i];
		const CDFGnode *p = e->source;
		if (p->alap_locked()) p->propagateAlapLock(p->alap());
	}
}

/* EJ 26/06/2007 */
void CDFG::lockedSinkNode(long cadency, long clk, long stages, Switches::Scheduling_strategy sty) const
{
	long output_time_limit;
	for (int i = 0; i < sink()->predecessors.size(); i++)
	{
		const CDFGedge *e = sink()->predecessors[i];
		CDFGnode *p = (sink()->predecessors[i])->source;
		switch (sty)
		{
		case Switches::FORCE_NO_PIPELINE :
		case Switches::NO_PIPELINE_MIN_FSM :
			output_time_limit = cadency - clk /* DH: 2/12/200* to avoid bus collision between output and next cadence input  -clk FIN DH 2/12/2008 */;
			break;
		case Switches::FORCE_NO_MOBILITY :
			output_time_limit = p->alap();
			break;
		case Switches::NO_MORE_STAGE :
			output_time_limit = cadency*stages - clk /* DH: 2/12/200* to avoid bus collision between output and next cadence input -clk FIN DH 2/12/2008 */;
			break;
		default :
			return;
		}
		if (p->alap() > output_time_limit)  	// stop
		{
			cerr << "too strong constraints" << endl;
			exit(1);
		}
		else if (p->alap_locked()) continue;	// already locked
		p->alap(output_time_limit);
		p->alap_locked(true);					// locked
	}
}
/* GL 11/06/07 : propagate bitwidth info to operation node */
void CDFG::propagateBitwidth() const
{
	for (int i = 0; i < nodes().size(); i++)
	{
		const CDFGnode *n = nodes()[i];			// get current CDFG node
		if (n->type() != CDFGnode::OPERATION) continue;		// search for OPERATION only
		Operation *o = (Operation*)n;
		// init widths
		o->initWidths();
		int j;
		for (j = 0; j<o->predecessors.size();j++)
		{
			o->inputsBitwidth(j, ((Data*) o->predecessors[j]->source)->bitwidth());
			o->inputsSigned(j, (((Data*) o->predecessors[j]->source)->getStringSigned() =="signed"));
		}
		for (j = 0; j<o->successors.size();j++)
		{
			o->outputsBitwidth(j, ((Data*) o->successors[j]->target)->bitwidth());
			o->outputsSigned(j, (((Data*) o->successors[j]->target)->getStringSigned() =="signed"));
		}
	}
}


// -----------------------------------------------------
// Added by Daniel to visualize the internal CDFG while debugging
// -----------------------------------------------------
void CDFG::show(void) const{
	ofstream ofile;
		string filename = "debugCDFG.dot";
		ofile.open(filename.c_str());
		if(ofile.is_open()) {
			ofile << "Digraph G {\n\tsize=\"10, 7.5\"; center = \"true\";\n";
			for (int i = 0; i < nodes().size(); i++) {
				const CDFGnode *node = nodes()[i];
				ofile << "Node_" << node << " [shape=ellipse, label=\"[" << i << ":";
				switch(node->type()) {
				case CDFGnode::DATA: {
										 ofile << "DATA:";
										 const Data* data = reinterpret_cast<const Data*>(node);
										 switch(data->type()) {
										 case Data::INPUT: ofile << "in]\\n"; break;
										 case Data::OUTPUT: ofile << "out]\\n"; break;
										 case Data::VARIABLE: ofile << "var]\\n"; break;
										 case Data::CONSTANT: ofile << "const]\\n"; break;
										 }
										 ofile << data->name();
										 break;
									 }
				case CDFGnode::OPERATION:
									 {
										 ofile << "OP]:";
										 const Operation* op = reinterpret_cast<const Operation*>(node);
										 ofile << op->function_name() << "\\n" << op->name();
										 break;
									 }
				case CDFGnode::CONTROL:
									 {
										 ofile << "CTRL:";
										 const Control* ctrl = reinterpret_cast<const Control*>(node);
										 ofile << ((ctrl->type() ==Control::SINK) ? "sink]:" : "source]\\n") << ctrl->name();
										 break;
									 }
				default:
									 break;
				}
				ofile << "\\n" << node << "\"];\n";
			}
			for (int i = 0; i < edges().size(); i++) {
				const CDFGedge *edge = edges()[i];
				ofile << "Node_" << edge->source << " -> " << "Node_" << edge->target << " [label=" <<i << "];\n";
			}
			ofile << "}";
			ofile.close();
			string cmd = "dot -Tps " + filename + " -o " + filename + ".ps";
			if(-1 == launchProc(cmd.c_str(),true))
				return;
					cmd = "gsview32.exe ";
					cmd += filename + ".ps";
					launchProc(cmd.c_str(),false);
		}
}

#ifdef _WIN32

#include <process.h>
#include <windows.h>
#include <conio.h>

int CDFG::launchProc(const char* cmd, bool wait) const {
	int argc, ret;
		char** argv;
		std::string strcmd;
		argv = (char**)malloc(255);
		memset(argv, 0, 255);
		strcmd = std::string(cmd);
		splitCmdLineToArgcArgv(strcmd, argc, argv);
		if(wait)
			ret = _spawnvp(_P_WAIT,argv[0], argv);
		else
			ret = _spawnvp(_P_NOWAIT,argv[0], argv);
				free(argv);
				return ret;
}
#else
int CDFG::launchProc(const char* cmd, bool wait) const {
}
#endif

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

void CDFG::splitCmdLineToArgcArgv(std::string& line, int& argc, char ** &argv) const {
	bool bLastSpace = true;
		bool bThisSpace;
		argc = 0;
		char * p = (char *) line.c_str();

		for (unsigned int i = 0; i < line.size(); i ++) {
			bThisSpace = (isspace(line[i])!=0);
				if (bThisSpace == true && bLastSpace == false) {
					line[i] = 0;
				}
			if (bLastSpace == true && bThisSpace == false) {
				argv[argc] = p + i;
					argc++;
			}
			bLastSpace = bThisSpace;
		}
}
/* Fin GL */
// end of: cdfg.cpp


