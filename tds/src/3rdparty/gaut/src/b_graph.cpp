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

//	File:		b_graph.cpp
//	Purpose:	bipartite graph
//	Author:		Ghizlane Lhairech-Lebreton, LESTER, UBS

#include <list>
#include "operation.h"
#include "scheduling.h"
#include "b_graph.h"
#include "graph.h"
#include "multimode_tools.h"
#include "connection.h"
#include <limits>

using namespace std;


//BGedge : edge of a bipartite graph

//! an edge cost
long BGedge::edge_cost ()
{
	long cost = 0;
	for (int i = 0; i<target->instance()->oper()->inputPorts();i++) if (source->operation()->inputsBitwidth(i)>target->instance()->inputsBitwidth(i))
			cost += (source->operation()->inputsBitwidth(i) - target->instance()->inputsBitwidth(i));
	return cost;
}


//BG : Bipartite graph

// ! remove node from sublists operations and instances and all successors, predecesors
void BG::remove(BGnode *n)
{
	int i;
	int j;
	switch (n->type())
	{
	case BGnode::OPERATION :
		for (i = 0; i < _operations.size(); i++) if (_operations[i] == n)
			{
				for (j = 0; j < _operations[i]->successors.size(); j++) removeEdge(_operations[i]->successors[j]);
				_operations.erase(_operations.begin()+i);			// remove pointer
				return;
			}
	case BGnode::INSTANCE :
		for (i = 0; i < _instances.size(); i++) if (_instances[i] == n)
			{
				for (j = 0; j < _instances[i]->predecessors.size(); j++) removeEdge(_instances[i]->predecessors[j]);
				_instances.erase(_instances.begin()+i);				// remove pointer
				return;
			}
	}
}

//! Create a new BG.
//! @param Input. set of operation nodes.
//! @param Input. set of operator instance nodes.
BG::BG(
    FunctionSchedulingState::OPERATIONS &ops,// vector<const Operation *> &ops,
    FunctionSchedulingState::A_OPERATORS &opr)//vector<OperatorInstance *> &opr)
{
	// build graph
	BGnode *node;
	int i;

	for (i = 0; i<opr.size(); i++)
	{
		OperatorInstance *oi = opr[i];
		addNode(node=new BGnode(BGnode::INSTANCE, NULL, oi));
		_instances.push_back(node);
	}

	for (i = 0; i<ops.size(); i++)
	{
		const Operation *op = ops[i];
		addNode(node=new BGnode(BGnode::OPERATION, op, NULL));
		_operations.push_back(node);

		// connect nodes via edges
		for (int j = 0; j<_instances.size(); j++)
		{
			BGnode *n = _instances[j];
			OperatorInstance *oi = n->instance();
			BGedge *e = new BGedge();
			addEdge(e);
			connect(e,node,n);
		}
	}
}

//! compute an edge weight
long BG::parents( BGedge *s_e )
{
	int efu, op, NbPredB;
	vector<const Operation *> s_predecessors;

	// selected nodes
	BGnode *s_oi = s_e->target;
	BGnode *s_op = s_e->source;
	s_predecessors.clear();
	CDFGnode *n = (CDFGnode*)s_op->operation();
	get_all_operations_predecessors(n, &s_predecessors);
	OperatorInstance *oi = s_oi->instance();
	NbPredB = 0;
	for (op = 0; op<s_predecessors.size(); op++)
	{
		OperationSchedulingState *s = (OperationSchedulingState *) s_predecessors.at(op)->_addr;// get operation's state
		if (s->instance == oi) NbPredB++;
	}
	return NbPredB;
}
/* GL 22/02/08 : New function */
//! compute an edge weight
long BG::successors( BGedge *s_e )
{
	int efu, op, NbSucB;
	vector<const Operation *> s_successors;

	// selected nodes
	BGnode *s_oi = s_e->target;
	BGnode *s_op = s_e->source;
	s_successors.clear();
	CDFGnode *n = (CDFGnode*)s_op->operation();
	get_operations_successors(n, &s_successors);
	OperatorInstance *oi = s_oi->instance();
	NbSucB = 0;
	for (op = 0; op<s_successors.size(); op++)
	{
		OperationSchedulingState *s = (OperationSchedulingState *) s_successors.at(op)->_addr;// get operation's state
		if (s->instance == oi) NbSucB++;
	}
	return NbSucB;
}
/* Fin GL */
//! compute an edge weight
long BG::distance( BGedge *s_e )
{
	// selected nodes
	BGnode *s_oi = s_e->target;
	BGnode *s_op = s_e->source;
	long d_s = ((s_op->operation()->sumOfwidths() < s_oi->instance()->sumOfwidths())? 0: (s_op->operation()->sumOfwidths() - s_oi->instance()->sumOfwidths()));
	return d_s;
}
//! compute an edge weight
long BG::connexions( BGedge *s_e )
{
	int efu, op, NbConn, ops;
	vector<const Operation *> s_predecessors;
	vector<const Operation *> s_successors;

	// selected nodes
	BGnode *s_oi = s_e->target;
	BGnode *s_op = s_e->source;
	s_predecessors.clear();
	s_successors.clear();
	CDFGnode *n = (CDFGnode*)s_op->operation();
	get_operations_predecessors(n, &s_predecessors);
	get_operations_successors(n, &s_successors);
	OperatorInstance *oi = s_oi->instance();
	NbConn = -1;
	for (op = 0; op<s_predecessors.size(); op++)
	{
		OperationSchedulingState *s = (OperationSchedulingState *) s_predecessors.at(op)->_addr;// get operation's state
		long c = oi->NumberOfConnection(s->instance);
		for (ops = 0; ops<s_successors.size(); ops++)
		{
			if ( s->instance->oper()->implements(s_successors.at(ops)->function()) ) c++;
		}
		NbConn = (NbConn > c) ? NbConn : c;
	}

	return NbConn;
}

//! compute an edge weight
long BG::distance( OperatorInstance *oi,const Operation *o )
{
	long d_s = ((o->sumOfwidths() < oi->sumOfwidths())? 0: (o->sumOfwidths() - oi->sumOfwidths()));
	return d_s ;
}

//! compute an edge weight
long BG::successors( OperatorInstance *oi,const Operation *o)
{
	int p, NbSucB, op;
	vector<const Operation *> s_successors;

	// selected nodes
	s_successors.clear();
	get_operations_successors(o, &s_successors);
	NbSucB = 0;
	for (op = 0; op<s_successors.size(); op++)
	{
		OperationSchedulingState *s = (OperationSchedulingState *) s_successors.at(op)->_addr;// get operation's state
		if (s->instance == oi) NbSucB++;
	}
	return NbSucB;
}

//! compute an edge weight
long BG::connexions( OperatorInstance *oi, const Operation *o )
{
	int op, NbConn, ops;
	vector<const Operation *> s_predecessors;
	vector<const Operation *> s_successors;

	// selected nodes
	s_predecessors.clear();
	s_successors.clear();
	get_operations_predecessors(o, &s_predecessors);
	get_operations_successors(o, &s_successors);
	NbConn = -1;
	for (op = 0; op<s_predecessors.size(); op++)
	{
		OperationSchedulingState *s = (OperationSchedulingState *) s_predecessors.at(op)->_addr;// get operation's state
		long c = oi->NumberOfConnection(s->instance);
		for (ops = 0; ops<s_successors.size(); ops++)
		{
			if ( s->instance->oper()->implements(s_successors.at(ops)->function()) ) c++;
		}
		NbConn = (NbConn > c) ? NbConn : c;
	}

	return NbConn;
}


//! compute weight for (oi,o)
double BG::computeWeight(OperatorInstance *oi, const Operation *o)
{
	double alpha = 0.8;
	double beta = 0.6;
	//parents (s_e);
	long d = distance(oi,o);
	double w = (d!=0)? 1/d:10;
	w = beta * alpha * connexions(oi,o) + beta * (1 - alpha) * successors(oi,o) + (1 - beta) * w;
	return w;
}


//! compute an edge weight
void BG::computeWeight( BGedge *s_e )
{
	double alpha = 0.8;
	double beta = 0.6;
	//parents (s_e);
	long d = distance(s_e);
	double w = (d!=0)? 1/d:10;
	w = beta * alpha * connexions(s_e) + beta * (1 - alpha) * successors(s_e) + (1 - beta) * w;
	s_e->weight(w);
}


//! width resulted from instance fitting
long BG::newWidth (OperatorInstance *oi, const Operation *o)
{
	long width = 0;
	for (int i = 0; i<oi->oper()->inputPorts();i++) if (o->inputsBitwidth(i)>oi->inputsBitwidth(i))
			width += o->inputsBitwidth(i);
	return width;
}

//! update all weights
void BG::updateWeights ()
{
	for (int i = 0; i< edges().size(); i++)
	{
		BGedge *e = edges()[i];
		computeWeight(e);
		if (Scheduling::_debug) cout << "BG::updateWeights  " << e->index() << " ( " << e->source->operation()->name() << "_" << e->target->instance()->no() << ") : " << e->weight() << endl;
	}
}

//! find edge with minmum weight matching
BGedge* BG::minMatching()
{
	long min_w = 10000;
	long e_index = 0;
	for (int i=0; i<edges().size(); i++)
	{
		BGedge *e = edges()[i];
		if (e->weight() < min_w)
		{
			min_w = e->weight();
			e_index = i;
		}
	}
	return (edges()[e_index]);
}
//! find edge with maximum weight matching
BGedge* BG::maxMatching()
{
	long max_w = -1;
	long e_index = 0;
	for (int i=0; i<edges().size(); i++)
	{
		BGedge *e = edges()[i];
		if (e->weight() > max_w)
		{
			max_w = e->weight();
			e_index = i;
		}
	}
	return (edges()[e_index]);
}


//! remove edge from list
void BG::removeEdge(BGedge *e)
{
	for (int i = 0; i < edges().size(); i++) if (edges()[i] == e)
		{
			edges().erase(edges().begin()+i);					// remove pointer
			return;
		}
}

//! remove node from list
void BG::removeNode(BGnode *n)
{
	remove(n);
	for (int i = 0; i < nodes().size(); i++) if (nodes()[i] == n)
		{
			nodes().erase(nodes().begin()+i);					// remove pointer
			return;
		}
}

//! Info
void BG::info()
{
	int i;
	cout << "Bipartite graph " << endl;
	cout << "edges = " << numberOfEdges() << endl;
	cout << "operation nodes = " << _operations.size() << endl;
	for (i = 0; i<_operations.size(); i++) cout << _operations[i]->operation()->name() << "_" << _operations[i]->operation()->sumOfwidths() << endl;
	cout << "instance nodes = " << _instances.size() << endl;
	for (i = 0; i<_instances.size(); i++) cout << "opr" << _instances[i]->instance()->no() << "_" << _instances[i]->instance()->sumOfwidths() << endl;
}


void BG::munkres(const Function *f)
{
	OperationSchedulingState *s;
	int row, col;
	const Operation *o;
	OperatorInstance *oi;

	//create matrix row=operation,column =operator_instance;
	int nrows=_operations.size();
	int ncols=_instances.size();
	Matrix<double> matrix(nrows,ncols);
	// Initialize matrix with weigth
	for ( row = 0 ; row < nrows ; row++ )
	{
		for ( col = 0 ; col < ncols ; col++ )
		{
			o=_operations[row]->operation();
			oi=_instances[col]->instance();
			double cost = computeWeight(oi,o);
			matrix(row,col) = std::numeric_limits< double >::max()-cost;
		}
	}
	//matrix.info();
	// Apply Munkres algorithm to matrix.
	Munkres m;
	m.solve(matrix);
	for ( row = 0 ; row < nrows ; row++ )
	{
		for (  col = 0 ; col < ncols ; col++ ) 
		{
			if ( matrix(row,col) == 0 )
			{
				o=_operations[row]->operation();	
				oi=_instances[col]->instance();
				if (Scheduling::_debug) cout <<"Scheduling::MWBMatching  Matching = " << o->name() << " opr" << oi->no() << endl;
				s = (OperationSchedulingState *) o->_addr;// get operation's state
				s->instance = oi;					// assign instance to operation
				if (oi)
				{
					oi->UpdateConnection (o);
					oi->bind(f);				// store binding information into operator instance
					if (!oi->fitsOperation(o)) oi->updateWidth(o);
				}
			}
		}
	}
}

// end of : b_graph.cpp
