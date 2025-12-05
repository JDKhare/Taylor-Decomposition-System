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

//	File:		b_graph.h
//	Purpose:	bipartite graph
//	Author:		Ghizlane Lhairech-Lebreton, LESTER, UBS

class Connexion;
class Connexions;
class BGnode;
class BGedge;
class BG;

#ifndef __B_GRAPH_H__
#define __B_GRAPH_H__

#include <iostream>
using namespace std;
#include "map.h"


#include "graph.h"
#include "cdfg.h"


//! Smart pointer on an operator instance connexion
class Connexion  : public MapRef <long>
{
public:
	Connexion(long *nb) : MapRef<long>(nb){}
};
//! List of smart pointers on Operations
class Connexions : public MapRefs<long, Connexion> {};


//! BG nodes.
class BGnode : public Node<BGedge>  	// specific graph elements
{
public:
	//! Enumerated type to distinguish nodes.
	enum Type {OPERATION, INSTANCE};
private:
	const Type	_type;		//!< Node type.
	const Operation *_op;	//!< pointer to operation if any
	OperatorInstance *_oi;	//!< pointer to instance if any
public:
	void *_addr;			//!< Generic pointer to extend easily.
	void *_addr2;			//!< Generic pointer to extend easily.
	//! Build a new BG node.
	//! @param Input. name is the unique name of the node.
	//! @param Input. type is the nod etype.
	BGnode(Type type, const Operation *op, OperatorInstance *oi) : _type(type)
	{
		_op = op;
		_oi = oi;
		_addr = 0;
		_addr2 = 0;
	}
	~BGnode()
	{
	}
	//! Get type.
	Type type() const
	{
		return _type;
	}
	//! Get CDFGnode

	const Operation *operation()
	{
		return _op;
	}
	OperatorInstance *instance()
	{
		return _oi;
	}
};

//! BG edges.
class BGedge : public Edge<BGnode>
{
private:
	long _weight;
public:
	//! Build a new edge
	BGedge()
	{
		_weight = 0;
	}
	//! Build a new edge
	BGedge(long weight)
	{
		_weight = weight;
	}
	//! Delete an edge.
	~BGedge()
	{
	}
	//! Set weight.
	void weight(long weight)
	{
		_weight = weight;
	}
	//! Get weight.
	long weight()
	{
		return _weight;
	}
	//! an edge cost
	long edge_cost ();
};

#include "operation.h"
#include "scheduling.h"

//! GAUT's BG. Is a subclass of Graph with specific BGnode and CDGGedge.
class BG : public Graph<BGnode, BGedge>
{
public:
	typedef vector<BGnode *> NODES;
private:
	NODES _operations;
	NODES _instances;

	// ! remove node from sublists operations and instances
	void remove(BGnode *n) ;

public:
	//! Create a new BG.
	//! @param Input. set of operation nodes.
	//! @param Input. set of operator instance nodes.
	BG(
	    FunctionSchedulingState::OPERATIONS &ops,// vector<const Operation *> &ops,
	    FunctionSchedulingState::A_OPERATORS &opr);//vector<OperatorInstance *> &opr)
	//! Delete a BG.
	~BG()
	{
		// delete all nodes: Graph will do it !
		BGnode *n;
	}

	//! compute an edge weight
	void computeWeight( BGedge *s_e );

	//! update all weights
	void updateWeights ();

	//! find edge with minmum weight matching
	BGedge* minMatching();

	//! find edge with maximum weight matching
	BGedge* maxMatching();

	long parents( BGedge *s_e );
	/* GL 22/02/08 : New function */
	long successors( BGedge *s_e );
	/* Fin GL */
	long distance( BGedge *s_e );
	long connexions( BGedge *s_e );

	//! remove edge from list
	void removeEdge(BGedge *e) ;

	//! remove node from list
	void removeNode(BGnode *n) ;

	//! width resulted from instance fitting
	long newWidth (OperatorInstance *oi, const Operation *o);

	//! Info
	void info();
	//
	void munkres(const Function *f);

	//! compute weight for (oi,o)
	double computeWeight(OperatorInstance *oi, const Operation *o);
	long successors(OperatorInstance *oi, const Operation *o);
	long distance(OperatorInstance *oi, const Operation *o);
	long connexions(OperatorInstance *oi, const Operation *o);
};


#endif // __B_GRAPH_H__
