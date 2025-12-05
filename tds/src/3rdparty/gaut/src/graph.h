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

//	File:		graph.h
//	Purpose:	GRAPH generics
//	Author:		Pierre Bomel, LESTER, UBS

template <class Node>				class Edge;
template <class Edge>				class Node;
template <class Node, class Edge>	class Graph;

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <vector>
#include <string>
using namespace std;

extern int Node_created;
extern int Node_deleted;
extern int Edge_created;
extern int Edge_deleted;
extern int Graph_created;
extern int Graph_deleted;
extern void GraphStat();

//! Node-Edge model.
//! All nodes and edges share a minimum commun set: an index.
class NE_Model
{
private:
	int _index;	//!< index into _nodes array
public:
	void index(int i)
	{
		_index = i;    //!< Set the index.
	}
	int index() const
	{
		return _index;    //!< Get the index.
	}
};

//! Node template.
//! The Node class has a template Edge describing the Edge class.
template <class Edge>
class Node : public NE_Model
{
private:
public:
	//! Create a new node.
	Node()
	{
#ifdef CHECK
		node_create++;	// debug
#endif
	}
	//! delete a node.
	virtual ~Node()
	{
#ifdef CHECK
		node_delete++;	// debug
#endif
	}
	vector<Edge*>	successors,		//!< Set of successor edges.
	predecessors;	//!< Set of predecessor edges.
	//! Check if a node is a successor of the current node.
	//! @param Input. n is a pointer to the successor candidate.
	//! @result a boolean, true = is a successor, false otherwise
	bool isASuccessorOf(const Node *n) const
	{
		for (int i = 0; i < n->successors.size(); i++)
		{
			const Edge *e = n->successors[i];
			const Node *t = e->target;
			if (t == this) return true;
		}
		return false;
	}
	//! Check if a node is a predecessor of the current node.
	//! @param Input. n is a pointer to the predecessor candidate.
	//! @result a boolean, true = is a predecessor, false otherwise
	bool isAPredecessorOf(const Node *n) const
	{
		for (int i = 0; i < n->predecessors.size(); i++)
		{
			const Edge *e = n->predecessors[i];
			const Node *s = e->source;
			if (s == this) return true;
		}
		return false;
	}
	/* GL 20/12/08 : Complex operators */
	//! Find the position ofa node in the successor set of the current node.
	//! @param Input. n is a pointer to the successor candidate.
	//! @result a long, -1 = if is not a successor.
	long isSuccessorNumber(const Node *n) const
	{
		for (int i = 0; i < n->successors.size(); i++)
		{
			const Edge *e = n->successors[i];
			const Node *t = e->target;
			if (t == this) return i;
		}
		return -1;
	}
	//! Find the position ofa node in the predecessor set of the current node.
	//! @param Input. n is a pointer to the predecessor candidate.
	//! @result a long, -1 = if is not a predecessor.
	long isPredecessorNumber(const Node *n) const
	{
		for (int i = 0; i < n->predecessors.size(); i++)
		{
			const Edge *e = n->predecessors[i];
			const Node *s = e->source;
			if (s == this) return i;
		}
		return -1;
	}
	/* Fin GL */

};

//! Recursive function template to order predecessors of a node.
//! Node and Edge are node and edge template classes.
//! @param Input. start is a pointer to the starting node.
//! @param Inout. inserted is a vector of booleans telling if a node has already been processed and avoid infinite recursive looping.
//! @param Output. an ordered vector of nodes.
template<class Node, class Edge>
void orderPredecessors(Node *start, vector<bool> &inserted, vector<Node *> &node_list)
{
	//cout << "looking at node(" << index() << ")"; info();
	if (inserted[start->index()]) return;					// no double insertion
	//cout << "ordering node(" << start->index() << ")"; info();
	// insert my predeccessors
	for (typename vector<Edge*>::const_iterator e_it = start->predecessors.begin(); e_it != start->predecessors.end(); e_it++)
		orderPredecessors<Node, Edge>((*e_it)->source, inserted, node_list);
	// insert me
	node_list.push_back(start);
	inserted[start->index()] = true;
	//cout << "ordered node(" << start->index() << ")"; info();
}

//! Recursive function template to order successors of a node.
//! Node and Edge are node and edge template classes.
//! @param Input. start is a pointer to the starting node.
//! @param Inout. inserted is a vector of booleans telling if a node has already been processed and avoid infinite recursive looping.
//! @param Output. an ordered vector of nodes.
template<class Node, class Edge>
void orderSuccessors(Node *start, vector<bool> &inserted, vector<Node *> &node_list)
{
	if (inserted[start->index()]) return;					// no double insertion
	// insert my successors
	for (typename vector<Edge*>::const_iterator e_it = start->successors.begin(); e_it != start->successors.end(); e_it++)
		orderSuccessors<Node, Edge>((*e_it)->target, inserted, node_list);
	// insert me
	node_list.push_back(start);
	inserted[start->index()] = true;
}

//! Edge template.
//! The Edge class has a template Node describing the Node class.
template <class Node>
class Edge : public NE_Model
{
public:
	//! Create a new Edge.
	Edge()
	{
#ifdef CHECK
		edge_create++;	// debug
#endif
	}
	//! delate an edge.
	virtual ~Edge()
	{
#ifdef CHECK
		edge_delete++;	// debug
#endif
	}
	Node	*source,	//!< Pointer to the edge source node.
	*target;	//!< Pointer to the edge target node.
};

//! Function template to connect two nodes via an edge.
//! Node and Edge are node and edge template classes.
//! @param Inout. e is a pointer to the edge connecting the nodes.
//! @param Inout. s is a pointer to the source node.
//! @param Inout. t is a pointer to the target node.
template<class Node, class Edge>
void connect(Edge *e, Node *s, Node *t)
{
	e->source = s;
	e->target = t;
	s->successors.push_back(e);
	t->predecessors.push_back(e);
}

//! Graph template.
//! The Graph class has a template Node describing the Node class and a template Edge describing the Edge class.
template <class Node, class Edge>
class Graph
{
public:
	typedef vector<Node*> NODES;	//!< Local type to describe Node vectors.
	typedef vector<Edge*> EDGES;	//!< Local type to describe Edge vectors.
private:
	NODES _nodes;					//!< The Node vector.
	EDGES _edges;					//!< The Edge vector.
public:
	//! Create a new Graph.
	Graph()
	{
#ifdef CHECK
		graph_create++;	// debug
#endif
	}
	//! Delete a Graph.
	//! Contains an explicit free of all nodes and edges.
	~Graph()
	{
		int i;
		for (i = 0; i < _nodes.size(); i++) delete _nodes[i];
		for (i = 0; i < _edges.size(); i++) delete _edges[i];
#ifdef CHECK
		graph_delete++;	// debug
#endif
	}
	//! Add a node
	//! @param Input. n is a pointer to the node to add.
	void addNode(Node *n)
	{
		_nodes.push_back(n);
		n->index(_nodes.size()-1);
	}
	//! Add an edge
	//! @param Input. n is a pointer to the edge to add.
	void addEdge(Edge *e)
	{
		_edges.push_back(e);
		e->index(_edges.size()-1);
	}
	//! Get access to the nodes.
	NODES & nodes()
	{
		return _nodes;
	}
	//! get access to the nodes with a "const" protection.
	const NODES & nodes() const
	{
		return _nodes;
	}
	//! get access to the edges.
	EDGES & edges()
	{
		return _edges;
	}
	//! Get access to the edges with a "const" protection.
	const EDGES & edges() const
	{
		return _edges;
	}
	//! Print info.
	void info() const
	{
		int i;
		cout << "Nodes:" << endl;
		for (i = 0; i < _nodes.size(); i++)
		{
			const Node *n = _nodes[i];
			n->info();
			/*cout << "    Predecessors:" << endl;
			for(int p = 0; p < n->predecessors.size(); p++) {
				cout << "      "; n->predecessors[p]->info();
			}
			cout << "    Successors:" << endl;
			for(int s = 0; s < n->successors.size(); s++) {
				cout << "     "; n->successors[s]->info();
			}*/
		}
		cout << "Edges:" << endl;
		for (i = 0; i < _edges.size(); i++)
		{
			_edges[i]->info();
		}
	}
	//! Get number of nodes.
	int numberOfNodes() const
	{
		return _nodes.size();
	}
	//! Get number of edges.
	int numberOfEdges() const
	{
		return _edges.size();
	}
	//! Search a node by its name.
	//! @param INput. name is the node name.
	//! @result a node pointer if found or NULL otherwise.
	const Node * searchNode(const string &name) const   // sequential search, to be used "lightly"
	{
		for (int i = 0; i < _nodes.size(); i++)
			if (name == _nodes[i]->name())
				return _nodes[i];
		return 0;
	}
	//! Scan all nodes. This may modify the Graph.
	//! @param Input. f is function to apply to each node.
	void scan_nodes(void f(Node *))
	{
		for (int i = 0; i < _nodes.size(); i++) f(_nodes[i]);
	}
	//! Scan all nodes protected by a "const". This will never modify the scanned Graph.
	//! @param Input. f is function to apply to each node.
	void scan_nodes(void f(const Node *)) const
	{
		for (int i = 0; i < _nodes.size(); i++) f(_nodes[i]);    // const version
	}
};

#endif // __GRAPH_H__
