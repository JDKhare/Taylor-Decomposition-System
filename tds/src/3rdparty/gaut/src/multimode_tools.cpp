//	File:		mutlimode_tools.cpp
//	Purpose:	functions for multimode part
//	Author:		Caaliph ANDRIAMISAINA, LESTER, UBS
#include "clock.h"		// system clock model
#include "cdfg.h"		// CDFG model
#include "operation.h"
#include "multimode_tools.h"
#include "data.h"

using namespace std;

void propagate_mode(CDFG *dfg_second, long mode)
{
	//vector<const CDFGnode*>::const_iterator n_it;
	int n_it;
	vector<CDFGnode*> v_node = dfg_second->nodes();
	for (n_it=0; n_it < v_node.size(); n_it++)
	{
		const CDFGnode *node = v_node[n_it];
		if (node->type()==CDFGnode::OPERATION)
		{
			((Operation*)node)->mode(mode);
		}
	}
}
/* Functions to search if a node, operation or function exist in a vector of nodes, operations or functions*/
bool search_node(const CDFGnode *n, vector<const CDFGnode*> *v)
{
	for (int i=0; i<v->size(); i++)
		if (n->name() == v->at(i)->name()) return true;
	return false;
}
bool search_node(const CDFGnode *n, vector<const Operation*> *v)
{
	for (int i=0; i<v->size(); i++)
		if (((Operation*)n)->name() == v->at(i)->name()) return true;
	return false;
}
bool search_function(const CDFGnode *n, vector<const CDFGnode*> *v)
{
	for (int i=0; i<v->size(); i++)
		if (((Operation*)n)->function()->name() == ((Operation*)v->at(i))->function()->name()) return true;
	return false;
}
bool search_function(const Operation *n, vector<const Operation*> *v)
{
	for (int i=0; i<v->size(); i++)
		if (n->function()->name() == v->at(i)->function()->name()) return true;
	return false;
}
bool search_operations(const Operation *n, vector<const Operation*> *v)
{
	for (int i=0; i<v->size(); i++)
		if (n->name() == v->at(i)->name()) return true;
	return false;
}
/* Function to search if an operation implemented by an operator "X" exist in a vector of operations */
bool search_identify(const Operation *n, vector<const Operation*> *v)
{
	for (int i=0; i<v->size(); i++)
		if (n->identify() == v->at(i)->identify()) return true;
	return false;
}
bool yet_match_indexes(int k, vector<int> *match)
{
	//v->erase(v->begin(),v->end());
	int i;
	bool temp_result=false;

	for (i=0; i<match->size(); i++)
	{
		if (k==(match->at(i))) temp_result=true;
	}
	return temp_result;
}
bool yet_match_indexes(long k, vector<long> *match)
{
	//v->erase(v->begin(),v->end());
	int i;
	bool temp_result=false;

	for (i=0; i<match->size(); i++)
	{
		if (k==(match->at(i))) temp_result=true;
	}
	return temp_result;
}
/* Search if an operation, belonging to mode "x", already exists in a vector of operations*/
bool identical_operations(const Operation *n, vector<const Operation*> *v)
{
	for (int i=0; i<v->size(); i++)
		if ((n->name() == v->at(i)->name()) && (n->mode()==v->at(i)->mode())) return true;
	return false;
}
/* Get operator name */
void get_operator_name (const Operation* op, string *name)
{
	int length;
	OperatorRefs::const_iterator it;

	it = op->function()->implemented_by().begin();
	OperatorRef op_ref = (*it).second;
	const Operator *opr = op_ref;
	*name=opr->name();
	length =name->size()-3;
	name->erase(length);
	if (opr->isMultiFunction()) name->erase(0,4);
}
/* Check if two operations are compatible */
bool compatible_operations(const Operation* op1, const Operation* op2)
{
	bool temp_res = false;
	string name1, name2;
	get_operator_name(op1, &name1);
	get_operator_name(op2, &name2);
	if (name1 == name2) temp_res = true;
	return temp_res;
}
/* Check if two operations are implemented by the same operator */
bool merged_operators(const Operation* op1, const Operation* op2)
{
	bool temp_res = false;
	string name1, name2;
	get_operator_name(op1, &name1);
	get_operator_name(op2, &name2);
	if ((name1 == name2) && (op1->identify() == op2->identify())) temp_res = true;
	return temp_res;
}
/* Check if an operation is implemented by an operator in a vector of operators */
bool same_operators(const Operation* ops, vector<const Operation*> *oper)
{
	bool temp_res = false;
	string name1;
	get_operator_name(ops, &name1);
	for (int i=0; i<oper->size(); i++)
	{
		string name2;
		get_operator_name(oper->at(i), &name2);
		if ((name1 == name2) && (ops->identify() == oper->at(i)->identify()))
		{
			temp_res = true;
			i = oper->size()-1;
		}
	}
	return temp_res;
}
/* Check if two operations, in the same cycle, are compatible */
bool compatible_successors(const Operation* op1, const Operation* op2)
{
	bool temp_res = false;
	string name1, name2;
	get_operator_name(op1, &name1);
	get_operator_name(op2, &name2);
	if ((name1 == name2) && (op1->start() == op2->start())) temp_res = true;
	return temp_res;
}
/* Check if two operations are compatible */
bool compatible_successors_no_cycle(const Operation* op1, const Operation* op2)
{
	bool temp_res = false;
	string name1, name2;
	get_operator_name(op1, &name1);
	get_operator_name(op2, &name2);
	if (name1 == name2) temp_res = true;
	return temp_res;
}
/* Bind the operations of v2 to the operators of v1 */
void assigned_identify(vector<const Operation*> *v1, vector<const Operation*> *v2)
{
	for (int i=0; i<v2->size(); i++)
		((Operation*)v2->at(i))->identify(v1->at(i)->identify());
}
void assigned_identify(const Operation* v1, const Operation* v2)
{
	((Operation*)v2)->identify(v1->identify());
}
/* Get immediately operations predecessors */
void get_operations_predecessors(const CDFGnode *n, vector<const Operation*> *v)
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_src;	// a source node
	const CDFGnode *n_dst;	// a destination node
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator

	for (e_it = n->predecessors.begin(); e_it != n->predecessors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_src = e->source;	// get the source node
		if (n_src->type() == CDFGnode::OPERATION)
		{
			if (!search_node(n_src, v))
				if (!((Operation*)n_src)->function()->passThrough())
					v->push_back(((Operation*)n_src));
		}
		else
		{
			get_operations_predecessors(n_src, v);
		}
	}
}
/* Get all operations predecessors */
void get_all_operations_predecessors(const CDFGnode *n, vector<const Operation*> *v)
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_src;	// a source node
	const CDFGnode *n_dst;	// a destination node
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
	vector<const CDFGnode*> pred_temp;


	for (e_it = n->predecessors.begin(); e_it != n->predecessors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_src = e->source;	// get the source node
		if (n_src->type() == CDFGnode::OPERATION)
		{
			if (!search_node(n_src, v))
			{
				v->push_back(((Operation*)n_src));
			}
		}
		get_all_operations_predecessors(n_src, v);
	}
}
/* Get operations successors */
void get_operations_successors(const CDFGnode *n, vector<const Operation*> *v)
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_src;	// a source node
	const CDFGnode *n_dst;	// a destination node
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator

	for (e_it = n->successors.begin(); e_it != n->successors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_dst = e->target;	// get the source node
		if (n_dst->type() == CDFGnode::OPERATION)
		{
			if (!search_node(n_dst, v))
				if (!((Operation*)n_dst)->function()->passThrough())
					v->push_back(((Operation*)n_dst));
		}
		else
		{
			get_operations_successors(n_dst, v);
		}
	}
}
/* Fill the vector of bound operations */
void get_bound_operations(const Operation *n1, const Operation *n2, vector<const Operation*> *v1, vector<const Operation*> *v2)
{
	int i;
	bool yet_bound1=false;
	bool yet_bound2=false;

	for (i=0; i<v1->size(); i++)
	{
		if (n1->name()==(v1->at(i))->name()) yet_bound1=true;
	}
	if (yet_bound1==false) v1->push_back(n1);
	for (i=0; i<v2->size(); i++)
	{
		if (n2->name()==(v2->at(i))->name()) yet_bound2=true;
	}
	if (yet_bound2==false) v2->push_back(n2);
}
/* Get operations at current cycle */
void get_operations_nodes_at_start(const CDFGnode *source, vector<const Operation*> *v, long start, long cadency)
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_src;	// a source node
	const CDFGnode *n_dst;	// a destination node
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
	int i=0;

	for (e_it = source->successors.begin(); e_it != source->successors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_dst = e->target;	// get the source node
		if (n_dst->type() == CDFGnode::OPERATION && start == n_dst->start()%cadency)
		{
			if (!search_node(n_dst, v))
			{
				if (!((Operation*)n_dst)->function()->passThrough())
					v->push_back(((Operation*)n_dst));
			}

		}
		get_operations_nodes_at_start(n_dst, v, start, cadency);
	}
}
/* Get number of compatible operations */
long get_ops_comp(CDFG *cdfg, vector<const Operation*> *v)
{
	long nb_ops=0;
	int n_it;
	vector<CDFGnode*> v_node = cdfg->nodes();
	for (n_it=0; n_it < v_node.size(); n_it++)
	{
		const CDFGnode *node = v_node[n_it];
		if (node->type()==CDFGnode::OPERATION)
		{
			if (search_function(((Operation*)node), v))
			{
				nb_ops++;
				((Operation*)node)->compatible_ops(true);
			}
			else ((Operation*)node)->compatible_ops(false);
		}
	}
	return nb_ops;
}
/* Get type of operations in a graph */
long get_operation_type(CDFG *cdfg)
{
	long nb_ops=0;
	//vector<const CDFGnode*>::const_iterator n_it;
	vector<const CDFGnode*> v;
	int n_it;
	vector<CDFGnode*> v_node = cdfg->nodes();
	for (n_it=0; n_it < v_node.size(); n_it++)
	{
		const CDFGnode *node = v_node[n_it];
		if (node->type()==CDFGnode::OPERATION)
		{
			if (((Operation*)node)->function()->passThrough()) continue;
			if (!search_function(node, &v)) nb_ops++;
			v.push_back(node);
		}
	}
	return nb_ops;
}
/* Check if 2 vectors contain compatible opoerations */
bool compatibility_status(vector<const Operation*> *operations_to_bind, vector<const Operation*> *existing_FUs)
{
	bool compatible = false;
	for (int i=0; i<operations_to_bind->size(); i++)
	{
		for (int j=0; j<existing_FUs->size(); j++)
		{
			if (compatible_operations(operations_to_bind->at(i), existing_FUs->at(j)))
			{
				compatible=true;
				i=operations_to_bind->size()-1;
				j=existing_FUs->size()-1;
			}
		}
	}
	return compatible;
}
/* Get compatible operations between 2 graphs */
void get_compatibles_op(CDFG *cdfg2, CDFG *cdfg1, vector<const Operation*> *comp_op, vector<const Operation*> *non_comp_op)
{
	//vector<const CDFGnode*>::const_iterator n1_it;
	//vector<const CDFGnode*>::const_iterator n2_it;
	const Operation *o1;
	const Operation *o2;
	int n2_it, n1_it;
	vector<CDFGnode*> v_node1 = cdfg1->nodes();
	vector<CDFGnode*> v_node2 = cdfg2->nodes();

	for (n2_it=0; n2_it < v_node2.size(); n2_it++)
	{
		const CDFGnode *node2 = v_node2[n2_it];

		//for(n2_it=cdfg2->nodes().begin(); n2_it!=cdfg2->nodes().end(); n2_it++) {
		if (node2->type() == CDFGnode::OPERATION)
		{
			bool compatible = false;
			o2 = (const Operation*) node2;
			//for(n1_it=cdfg1->nodes().begin(); n1_it!=cdfg1->nodes().end(); n1_it++) {
			for (n1_it=0; n1_it < v_node1.size(); n1_it++)
			{
				const CDFGnode *node1 = v_node1[n1_it];
				if (node1->type() == CDFGnode::OPERATION)
				{
					o1 = (const Operation*) (node1);
					if (compatible_operations(o1,o2))
					{
						compatible=true;
						if (!search_function(o2,comp_op))
						{
							comp_op->push_back(o2);
						}
						//n1_it=v_node1.end()-1;
						break;
					}
				}
			}
			if (!compatible)
			{
				if (!search_function(o2,non_comp_op))
					non_comp_op->push_back(o2);
			}
		}
	}
}
/* Initialize the delay value used during the scheduling */
void delay_init (CDFG *cdfg2)
{
	//vector<const CDFGnode*>::const_iterator n2_it;
	//for(n2_it=cdfg2->nodes().begin(); n2_it!=cdfg2->nodes().end(); n2_it++) {
	vector<CDFGnode*> v_node2 = cdfg2->nodes();
	int n2_it;
	for (n2_it=0; n2_it < v_node2.size(); n2_it++)
	{
		const CDFGnode *node2 = v_node2[n2_it];
		if (node2->type() == CDFGnode::OPERATION)
			((Operation*)node2)->delay(0);
	}
}
/* Get the operations that have not been bound */
void get_not_bound_op(vector<const Operation*> *operations_to_bind, vector<const Operation*> *bound_op_temp,
                      vector<const Operation*> *not_bound_op)
{

	for (int i=0; i<operations_to_bind->size(); i++)
	{
		if (!search_operations(operations_to_bind->at(i), bound_op_temp))
			if (!search_operations(operations_to_bind->at(i), not_bound_op))
				not_bound_op->push_back(operations_to_bind->at(i));
	}
}
/* Get the functional units used to implement a set of operations */
void get_used_FUs (vector<const Operation*> *v_ops, vector<const Operation*> *v_FUs)
{
	for (int i=0; i<v_ops->size(); i++)
	{
		if (!same_operators(v_ops->at(i), v_FUs))
			v_FUs->push_back(v_ops->at(i));
	}
}
/* Copy the content of a vector of operations into an another vector of operations */
void copy_of_vector(vector<const Operation*> *v_in, vector<const Operation*> *v_out)
{
	for (int i=0; i<v_in->size(); i++)
		v_out->push_back(v_in->at(i));
}
/* Erase an element from a vector of operations */
void erase_element (vector<const Operation*> *v_in, const Operation *to_erase)
{
	vector<const Operation*>::iterator it;

	for (it = v_in->begin(); it != v_in->end(); it++)
	{
		if (*it==to_erase)
		{
			v_in->erase(it);
			it = v_in->end()-1;
		}
	}
}
/* Initialize the array of weights */
void init_array(long w[], int longueur, long init_value)
{
	for (int i=0; i<longueur; i++)
		w[i]=init_value;
}
void init_array(long w[1000][1000], long init_value)
{
	for (int i=0; i<1000; i++)
		for (int j=0; j<1000;j++)
			w[i][j]=init_value;
}
void init_array(float w[1000][1000], float init_value)
{
	for (int i=0; i<1000; i++)
		for (int j=0; j<1000;j++)
			w[i][j]=init_value;
}
/* Compute the number of predecessors and successors of an operation*/
void Pred_Succ_Computation(const Operation* existing_FU, vector<const Operation*> *predecessors1, vector<const Operation*> *successors1,vector<const Operation*> *predecessors2,
                           vector<const Operation*> *successors2, long *NbPredB, long *NbSuccC, long start, bool cases)
{
	int l,k;
	predecessors1->clear();
	successors1->clear();
	get_operations_predecessors(existing_FU, predecessors1);
	get_operations_successors(existing_FU, successors1);
	for (l = 0; l < predecessors1->size(); l++)
	{
		for (k=0; k < predecessors2->size(); k++)
		{
			if (merged_operators(predecessors1->at(l), predecessors2->at(k)) && (start!=0))
				(*NbPredB)++;
		}
	}
	for (l = 0; l < successors1->size(); l++)
	{
		for (k = 0; k < successors2->size(); k++)
		{
			if (cases==0)
			{
				if (compatible_successors(successors1->at(l), successors2->at(k)))
					(*NbSuccC)++;
			}
			else
			{
				if (compatible_successors_no_cycle(successors1->at(l), successors2->at(k)))
					(*NbSuccC)++;
			}
		}
	}
}
void Pred_Succ_Computation(const Operation* existing_FU, vector<const Operation*> *predecessors1, vector<const Operation*> *successors1,vector<const Operation*> *predecessors2,
                           vector<const Operation*> *successors2, float *NbPredB, float *NbSuccC, long start, bool cases)
{
	int l,k;
	predecessors1->clear();
	successors1->clear();
	get_operations_predecessors(existing_FU, predecessors1);
	get_operations_successors(existing_FU, successors1);
	for (l = 0; l < predecessors1->size(); l++)
	{
		for (k=0; k < predecessors2->size(); k++)
		{
			if (merged_operators(predecessors1->at(l), predecessors2->at(k)) && (start!=0))
				(*NbPredB)++;
		}
	}
	for (l = 0; l < successors1->size(); l++)
	{
		for (k = 0; k < successors2->size(); k++)
		{
			if (cases==0)
			{
				if (compatible_successors(successors1->at(l), successors2->at(k)))
					(*NbSuccC)++;
			}
			else
			{
				if (compatible_successors_no_cycle(successors1->at(l), successors2->at(k)))
					(*NbSuccC)++;
			}
		}
	}
}
void find_max(long w[],int longueur, long *max, const Operation *operation_to_bind, vector<const Operation*> *existing_FUs)
{
	int i;
	for (i=0;i<longueur;i++)
	{
		if (compatible_operations(operation_to_bind, existing_FUs->at(i)))
		{
			if (w[i]>*max)
			{
				*max=w[i];
			}
		}
	}
}
void index_computation(long w[],int longueur, int *out_l, int in_i, int *out_i, long *max,
                       const Operation *operation_to_bind, vector<const Operation*> *existing_FUs)
{
	long max_tab=-1;
	int i;
	find_max(w,longueur,&max_tab,operation_to_bind,existing_FUs);
	if (in_i==0) *max=max_tab;
	else if (max_tab<*max) *max=max_tab;
	for (i=0;i<longueur;i++)
	{
		if (compatible_operations(operation_to_bind, existing_FUs->at(i)))
		{
			if (w[i]<=*max)
			{
				*out_l=i;
				*max=w[i];
				*out_i=in_i;
			}
		}
	}
}
void echange (string *in, string *out)
{
	string in_name = *in;
	*in=*out;
	*out=in_name;
}
void echange (float *in, float *out)
{
	float in_name = *in;
	*in=*out;
	*out=in_name;
}
void echange (long *in, long *out)
{
	long in_name = *in;
	*in=*out;
	*out=in_name;
}
void echange (CDFG **in, CDFG **out)
{
	CDFG *in_name = *in;
	in=out;
	*out=in_name;
}
void tri_bulle(float tableau[], long nb_ops_comp[], long nb_ops[], CDFG *cdfg[], string cdfgfilename[], long cadencys[], long modes[], string modes_numbers[],
               string vhdlnames[], string vhdl_prefixs[], string memfiles[], int longueur)
{
	int i, inversion;
	do
	{
		inversion=0;
		for (i=0;i<longueur-1;i++)
		{
			if (tableau[i]<tableau[i+1])
			{
				echange(&tableau[i],&tableau[i+1]);
				echange(&nb_ops_comp[i],&nb_ops_comp[i+1]);
				echange(&nb_ops[i],&nb_ops[i+1]);
				CDFG *tmp=cdfg[i];
				cdfg[i]=cdfg[i+1];
				cdfg[i+1]=tmp;
				echange(&cdfgfilename[i],&cdfgfilename[i+1]);
				echange(&cadencys[i],&cadencys[i+1]);
				echange(&modes[i],&modes[i+1]);
				echange(&modes_numbers[i], &modes_numbers[i+1]);
				echange(&vhdlnames[i],&vhdlnames[i+1]);
				echange(&vhdl_prefixs[i],&vhdl_prefixs[i+1]);
				echange(&memfiles[i],&memfiles[i+1]);
				inversion=1;
			}
			else if (tableau[i]==tableau[i+1])
			{
				if (nb_ops_comp[i]<nb_ops_comp[i+1])
				{
					echange(&tableau[i],&tableau[i+1]);
					echange(&nb_ops_comp[i],&nb_ops_comp[i+1]);
					echange(&nb_ops[i],&nb_ops[i+1]);
					CDFG *tmp=cdfg[i];
					cdfg[i]=cdfg[i+1];
					cdfg[i+1]=tmp;
					echange(&cdfgfilename[i],&cdfgfilename[i+1]);
					echange(&cadencys[i],&cadencys[i+1]);
					echange(&modes[i],&modes[i+1]);
					echange(&modes_numbers[i], &modes_numbers[i+1]);
					echange(&vhdlnames[i],&vhdlnames[i+1]);
					echange(&vhdl_prefixs[i],&vhdl_prefixs[i+1]);
					echange(&memfiles[i],&memfiles[i+1]);
					inversion=1;
				}
				else if (nb_ops_comp[i]==nb_ops_comp[i+1])
				{
					if (nb_ops[i]<nb_ops[i+1])
					{
						echange(&tableau[i],&tableau[i+1]);
						echange(&nb_ops_comp[i],&nb_ops_comp[i+1]);
						echange(&nb_ops[i],&nb_ops[i+1]);
						CDFG *tmp=cdfg[i];
						cdfg[i]=cdfg[i+1];
						cdfg[i+1]=tmp;
						echange(&cdfgfilename[i],&cdfgfilename[i+1]);
						echange(&cadencys[i],&cadencys[i+1]);
						echange(&modes[i],&modes[i+1]);
						echange(&modes_numbers[i], &modes_numbers[i+1]);
						echange(&vhdlnames[i],&vhdlnames[i+1]);
						echange(&vhdl_prefixs[i],&vhdl_prefixs[i+1]);
						echange(&memfiles[i],&memfiles[i+1]);
						inversion=1;
					}
				}

			}
		}
	}
	while (inversion);
}
/* Compute the data lifetimes and check if it is necessary to duplicate data according to its lifetime */
/* Get data consumers */
void get_data_successors(const CDFGnode *n, vector<const Operation*> *v)
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_src;	// a source node
	const CDFGnode *n_dst;	// a destination node
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator

	for (e_it = n->successors.begin(); e_it != n->successors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_dst = e->target;	// get the source node
		if (n_dst->type() == CDFGnode::OPERATION)
		{
			if (!search_node(n_dst, v))
				v->push_back(((Operation*)n_dst));
		}
		else
		{
			get_operations_successors(n_dst, v);
		}
	}
}
/* Get data predecessors */
void get_data_predecessors(const CDFGnode *n, vector<const Operation*> *v)
{
	const CDFGedge *e;		// an edge
	const CDFGnode *n_src;	// a source node
	const CDFGnode *n_dst;	// a destination node
	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator

	for (e_it = n->predecessors.begin(); e_it != n->predecessors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_src = e->source;	// get the source node
		if (n_src->type() == CDFGnode::OPERATION)
		{
			if (!search_node(n_src, v))
				v->push_back(((Operation*)n_src));
		}
		else
		{
			get_operations_predecessors(n_dst, v);
		}
	}
}
/* lifetimes computing and the duplication */
/****void data_WR(CDFG *cdfg, Cadency *cadency, Clock *clk) {
	vector<CDFGnode*> v_node = cdfg->nodes();
	for(int IT = 0; IT < v_node.size(); IT++) {
		CDFGnode *node = v_node[IT];
		if(node->type()==CDFGnode::DATA) {
			Data *d = (Data *) node;
			vector<const Operation*> ops;
			if(d->type()==Data::VARIABLE || d->type()==Data::INPUT || d->type()==Data::CONSTANT) {
				get_data_successors(node, &ops);
				long max_start=-1;
				long max_length=-1;
				for(int i=0; i<ops.size(); i++) {
					if((ops[i]->start()+ops[i]->length())>(max_start+max_length)) {
						max_start=ops[i]->start();
						max_length=ops[i]->length();
					}
				}
				d->end(max_start+max_length);
				d->lifetime(d->end()-d->start());
				long end=0;
				if(d->end()%cadency->length()!=0) end=d->end()%cadency->length();
				else if(d->end()%cadency->length()==0) {
					if(d->end()>=cadency->length())
						end=cadency->length();
					else
						end=0;
				}
				if(end>d->start()%cadency->length())
					d->duplication(ceil((double)d->lifetime()/(double)cadency->length()));
				else
					d->duplication(ceil((double)d->lifetime()/(double)cadency->length())+1);
			}
			if(d->type()==Data::OUTPUT) {
				d->end(d->start());
				d->lifetime(d->end()-d->start());
			}
		}
	}
}***/
/* Computing of the lifetimes and the duplication */
void data_WR(CDFG *cdfg, Cadency *cadency, Clock *clk, Mem *mem, Banks *banks)
{
	vector<CDFGnode*> v_node = cdfg->nodes();
	for (int IT = 0; IT < v_node.size(); IT++)
	{
		CDFGnode *node = v_node[IT];
		if (node->type()==CDFGnode::DATA)
		{
			Data *d = (Data *) node;
			vector<const Operation*> ops;
			if (d->type()==Data::VARIABLE || d->type()==Data::INPUT || d->type()==Data::CONSTANT)
			{
				get_data_successors(node, &ops);
				long max_start=-1;
				long max_length=-1;
				for (int i=0; i<ops.size(); i++)
				{
					if ((ops[i]->start()+ops[i]->length())>(max_start+max_length))
					{
						max_start=ops[i]->start();
						max_length=ops[i]->length();
					}
				}
				//cout << d->name()<<"coucou " << max_start+max_length<<endl;


				if (d->readvar() && !d->aging())  //après fusion temps réel
				{
					// variables
					Banks::BANKS::const_iterator b_it;			// to scan banks
					const Bank *bank;
					if (banks->size())
					{
						for (b_it = banks->banks.begin(); b_it != banks->banks.end(); b_it++)
						{
							bank = &(*b_it).second;		// get current bank
							// variables
							Bank::USES::const_iterator u_it;			// to scan banks' accesses
							const BankUse *use;
							long time;

							// scan all writes of the bank

							for (u_it = bank->writes.begin();
							        u_it != bank->writes.end(); u_it++)
							{
								use = &(*u_it).second;	// get current bank use
								time = use->time_start();		// get time of bank use
								// update in case of WRITE
								time = (time + mem->access_time()) % cadency->length();
								const Data *data = use->address()->data();
								if (data->name() != d->name())  continue;
								if (time > max_start+max_length)
								{
									max_length = 0;
									max_start = time;
								}
							}
						}
					}
				}


				d->end(max_start+max_length);
				d->lifetime(d->end()-d->start());
				long end=0;
				if (d->end()%cadency->length()!=0) end=d->end()%cadency->length();
				else if (d->end()%cadency->length()==0)
				{
					if (d->end()>=cadency->length())
						end=cadency->length();
					else
						end=0;
				}
				if (end>d->start()%cadency->length())
					d->duplication(ceil((double)d->lifetime()/(double)cadency->length()));
				else
					d->duplication(ceil((double)d->lifetime()/(double)cadency->length())+1);
			}
			if (d->type()==Data::OUTPUT)
			{
				/* 07/04/2008 KT replace the line
				d->end(d->start());
				by*/
				d->end(d->start()+ clk->period());
				d->lifetime(d->end()-d->start() );
			}
			//cout << d->name()<<"           " << " start "<< d->start() <<" end "<< d->end() <<endl;
		}



	}
}

/* Maximum weighted matching algorithm for the "assign" operation binding */
void MWBM_MOD(const Operation *operation_to_bind, vector<const Operation*> *existing_FUs, long *num_id, Cadency *cadency)
{
	int efu;
	int j, l, k;
	int count_matching, nb_op_to_bind;
	int bound=0;
	int found_FU=0;
	int nb_FUs;
	long NbPredB;
	long max_id=-1;
	vector<const Operation*> predecessors1, predecessors2, cannot_used_FUs, can_used_FUs;

	nb_FUs = existing_FUs->size();
//	for(j=0; j<nb_FUs; j++)
//		cout<<existing_FUs->at(j)->name()<<"_"<<existing_FUs->at(j)->identify()<<endl;
	for (j=0; j<nb_FUs; j++)
	{
		if (operation_to_bind->start()%cadency->length()==existing_FUs->at(j)->start()%cadency->length())
			cannot_used_FUs.push_back(existing_FUs->at(j));
	}
	for (j=0; j<cannot_used_FUs.size(); j++)
	{
//		cout<<cannot_used_FUs[j]->name()<<"_"<<cannot_used_FUs[j]->identify()<<endl;
		if (max_id<cannot_used_FUs[j]->identify())
			max_id = cannot_used_FUs[j]->identify();
	}
	for (j=0; j<nb_FUs; j++)
	{
		if (!search_identify(existing_FUs->at(j), &cannot_used_FUs))
			can_used_FUs.push_back(existing_FUs->at(j));
	}
	if (!can_used_FUs.empty())
	{
		long w_temp=-1;
		long w_ob_efu[1000];
		bool comp = false;
		init_array(w_ob_efu, 1000, -1);
		predecessors2.clear();
		get_operations_predecessors(operation_to_bind, &predecessors2);
		for (efu=0; efu<can_used_FUs.size(); efu++)
		{
			NbPredB = 0;
			if (compatible_operations(operation_to_bind, can_used_FUs[efu]))
			{
				predecessors1.clear();
				get_operations_predecessors(can_used_FUs[efu], &predecessors1);
				for (l = 0; l < predecessors1.size(); l++)
				{
					for (k=0; k < predecessors2.size(); k++)
					{
						if (merged_operators(predecessors1[l], predecessors2[k]))
							(NbPredB)++;
					}
				}
				w_ob_efu[efu] = NbPredB ;
			}
		}
		for (l=0;l<can_used_FUs.size();l++)
		{
			if (compatible_operations(operation_to_bind, can_used_FUs[l]))
			{
				if (w_ob_efu[l]>w_temp)
				{
					w_temp = w_ob_efu[l];
					found_FU = l;
				}
			}
		}
		if (!existing_FUs->empty())
		{
			if (compatible_operations(operation_to_bind, can_used_FUs[found_FU]))
			{
				assigned_identify(can_used_FUs[found_FU], operation_to_bind);
				existing_FUs->push_back(operation_to_bind);
				*num_id = can_used_FUs[found_FU]->identify();
			}
		}
	}
	if (nb_FUs==existing_FUs->size())
	{
		*num_id = max_id + 1;
		((Operation*)operation_to_bind)->identify(*num_id);
		existing_FUs->push_back(operation_to_bind);
	}
}

/**** For spact-mr only and will need some modifications *****/
//Function to fill the not_bound_start vector
void get_operations_at_start(vector<const CDFGnode*> *v_in, vector<const CDFGnode*> *v_out, long start)
{
	int i;

	for (i=0; i<v_in->size();i++)
	{
		if (start==v_in->at(i)->start())
		{
			v_out->push_back(v_in->at(i));
		}
	}
}
// Function to fill the cannot_used_op vector
void get_pred_at_start_clk(CDFG *cdfg2, CDFG *cdfg1, Cadency *cadency, Clock *clk, vector<const CDFGnode*> *v_in,
                           vector<const CDFGnode*> *can_used_op, const CDFGnode *at_start_op, long start)
{
	int i,j;
	long time_step, time_start;
	vector<const CDFGnode*>::const_iterator n_it, out_it;
	vector<const CDFGnode*> v_out;
	const Operation *op;
	bool can_used = true;

	time_step = start%cadency->length();
	for (i=0; i<v_in->size();i++)
	{
		if (v_in->at(i)->start()==time_step)
			v_out.push_back(v_in->at(i));
	}
	// Find the can_used_op in the bound_op (specifically in the secondary)
	for (n_it=v_in->begin();n_it!=v_in->end();n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			for (out_it=v_out.begin();out_it!=v_out.end();out_it++)
			{
				if (((*out_it)->name()==op->name())||(((Operation*)(*out_it))->identify()==op->identify()))
				{
					can_used=false;
					out_it = v_out.end()-1;
				}
				else if (((Operation*)(at_start_op))->function()->name()==op->function()->name()) can_used = true;
				else can_used =false;
			}
			if (can_used)
				can_used_op->push_back(*n_it);
		}
	}
	// Find the can_used_op in the cdfg1 (main CDFG) except the operations that have
	// the same "operator number (identification)" as the operations that cannot be used (cannot_used_op)
	vector<CDFGnode*> v_node = cdfg1->nodes();
	for (int IT=0; IT < v_node.size(); IT++)
	{
		const CDFGnode *node = v_node[IT];
		//for(n_it=cdfg1->nodes().begin();n_it!=cdfg1->nodes().end();n_it++){
		if (node->type() == CDFGnode::OPERATION)
		{
			op =(const Operation*) node;
			for (out_it=v_out.begin();out_it!=v_out.end();out_it++)
			{
				if ((((Operation*)(*out_it))->function()->name()==op->function()->name())&&
				        (((Operation*)(*out_it))->identify()==op->identify()))
				{
					can_used=false;
					out_it = v_out.end()-1;
				}
				else if (((Operation*)(at_start_op))->function()->name()==op->function()->name()) can_used = true;
				else can_used =false;
			}
			if (can_used)
				can_used_op->push_back(node);
		}
	}
}
// Function to fill the cannot_used_op vector
void get_pred_at_start(CDFG *cdfg2, CDFG *cdfg1, Cadency *cadency, Clock *clk, vector<const CDFGnode*> *v_in,
                       vector<const CDFGnode*> *can_used_op, const CDFGnode *at_start_op, long start)
{
	int i,j;
	long time_step, time_start, conflict_limit;
	vector<const CDFGnode*>::const_iterator n_it, out_it;
	vector<const CDFGnode*> v_out;
	const Operation *op;
	bool can_used = true;

	time_step = start%cadency->length();
	conflict_limit = at_start_op->length() - clk->period();
	for (j=time_step+conflict_limit;j>=time_step-conflict_limit;j=j-clk->period())
	{
		for (i=0; i<v_in->size();i++)
		{
			if (j==v_in->at(i)->start()%cadency->length())
			{
				v_out.push_back(v_in->at(i));
			}
		}
	}
	for (i=0; i<v_in->size();i++)
	{
		if (((v_in->at(i)->start()==time_step)&&(start >= cadency->length()))
		        &&(!search_node(v_in->at(i), &v_out)))
			v_out.push_back(v_in->at(i));
	}
	// Find the can_used_op in the bound_op (specifically in the secondary)
	for (n_it=v_in->begin();n_it!=v_in->end();n_it++)
	{
		if ((*n_it)->type() == CDFGnode::OPERATION)
		{
			op = (const Operation*) (*n_it);
			for (out_it=v_out.begin();out_it!=v_out.end();out_it++)
			{
				if (((*out_it)->name()==op->name())||(((Operation*)(*out_it))->identify()==op->identify()))
				{
					can_used=false;
					out_it = v_out.end()-1;
				}
				else if (((Operation*)(at_start_op))->function()->name()==op->function()->name()) can_used = true;
				else can_used =false;
			}
			if (can_used) can_used_op->push_back(*n_it);
		}
	}
	// Find the can_used_op in the cdfg1 (main CDFG) except the operations that have
	// the same "operator number (identification)" as the operations that cannot be used (cannot_used_op)
	vector<CDFGnode*> v_node = cdfg1->nodes();
	for (int IT=0; IT < v_node.size(); IT++)
	{
		const CDFGnode *node = v_node[IT];
		if (node->type() == CDFGnode::OPERATION)
		{
			op =(const Operation*) node;
			for (out_it=v_out.begin();out_it!=v_out.end();out_it++)
			{
				if ((((Operation*)(*out_it))->function()->name()==op->function()->name())&&
				        (((Operation*)(*out_it))->identify()==op->identify()))
				{
					can_used=false;
					out_it = v_out.end()-1;
				}
				else if (((Operation*)(at_start_op))->function()->name()==op->function()->name()) can_used = true;
				else can_used =false;
			}
			if (can_used)
				can_used_op->push_back(node);
		}
	}
}
