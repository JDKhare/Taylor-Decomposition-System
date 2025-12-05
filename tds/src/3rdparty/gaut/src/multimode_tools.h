//	File:		multimode_tools.h
//	Purpose:	functions for multimode part
//	Author:		Caaliph ANDRIAMISAINA, LESTER, UBS

#include "bank.h"
//#include "mem.h"

void propagate_mode(CDFG *dfg_second, long mode);
bool search_node(const CDFGnode *n, vector<const CDFGnode*> *v);
bool search_node(const CDFGnode *n, vector<const Operation*> *v);
bool search_function(const CDFGnode *n, vector<const CDFGnode*> *v);
bool search_function(const Operation *n, vector<const Operation*> *v);
bool search_operations(const Operation *n, vector<const Operation*> *v);
bool yet_match_indexes(int k, vector<int> *match);
bool yet_match_indexes(long k, vector<long> *match);
bool identical_operations(const Operation *n, vector<const Operation*> *v);
extern void get_operator_name (const Operation* op, string *name);
bool compatibility_status(vector<const Operation*> *operations_to_bind, vector<const Operation*> *existing_FUs);
bool compatible_operations(const Operation* op1, const Operation* op2);
bool merged_operators(const Operation* op1, const Operation* op2);
bool same_operators(const Operation* ops, vector<const Operation*> *oper);
bool compatible_successors(const Operation* op1, const Operation* op2);
bool compatible_successors_no_cycle(const Operation* op1, const Operation* op2);
void assigned_identify(vector<const Operation*> *v1, vector<const Operation*> *v2);
void assigned_identify(const Operation* v1, const Operation* v2);
void get_operations_predecessors(const CDFGnode *n, vector<const Operation*> *v);
void get_all_operations_predecessors(const CDFGnode *n, vector<const Operation*> *v);
void get_operations_successors(const CDFGnode *n, vector<const Operation*> *v);
void get_bound_operations(const Operation *n1, const Operation *n2, vector<const Operation*> *v1, vector<const Operation*> *v2);
void get_operations_nodes_at_start(const CDFGnode *source, vector<const Operation*> *v, long start, long cadency);
long get_operation_type(CDFG *cdfg);
void get_compatibles_op(CDFG *cdfg2, CDFG *cdfg1, vector<const Operation*> *comp_op, vector<const Operation*> *non_comp_op);
void get_not_bound_op(vector<const Operation*> *operations_to_bind, vector<const Operation*> *bound_op_temp,
                      vector<const Operation*> *not_bound_op);
void get_operations_at_start(vector<const CDFGnode*> *v_in, vector<const CDFGnode*> *v_out, long start);
void get_pred_at_start_clk(CDFG *cdfg2, CDFG *cdfg1, Cadency *cadency, Clock *clk, vector<const CDFGnode*> *v_in, vector<const CDFGnode*> *can_used_op, const CDFGnode *at_start_op, long start);
void get_pred_at_start(CDFG *cdfg2, CDFG *cdfg1, Cadency *cadency, Clock *clk, vector<const CDFGnode*> *v_in, vector<const CDFGnode*> *can_used_op, const CDFGnode *at_start_op, long start);
void get_used_FUs (vector<const Operation*> *v_ops, vector<const Operation*> *v_FUs);
void copy_of_vector(vector<const Operation*> *v_in, vector<const Operation*> *v_out);
void erase_element (vector<const Operation*> *v_in, const Operation *to_erase);
long get_ops_comp(CDFG *cdfg, vector<const Operation*> *v);
void delay_init (CDFG *cdfg2);
void init_array(long w[1000][1000], long init_value);
void init_array(float w[1000][1000], float init_value);
void find_max(long w[],int longueur, long *max, const Operation *operation_to_bind, vector<const Operation*> *existing_FUs);
void index_computation(long w[],int longueur, int *out_l, int in_i, int *out_i, long *max,
                       const Operation *operation_to_bind, vector<const Operation*> *existing_FUs);
void Pred_Succ_Computation(const Operation* existing_FU, vector<const Operation*> *predecessors1, vector<const Operation*> *successors1,vector<const Operation*> *predecessors2,
                           vector<const Operation*> *successors2, long *NbPredB, long *NbSuccC, long start, bool cases);
void Pred_Succ_Computation(const Operation* existing_FU, vector<const Operation*> *predecessors1, vector<const Operation*> *successors1,vector<const Operation*> *predecessors2,
                           vector<const Operation*> *successors2, float *NbPredB, float *NbSuccC, long start, bool cases);
void echange (string *in, string *out);
void echange (float *in, float *out);
void echange (long *in, long *out);
void echange (CDFG **in, CDFG **out);
void tri_bulle(float tableau[], long nb_ops_comp[], long nb_ops[], CDFG *cdfg[], string cdfgfilename[], long cadencys[], long modes[], string modes_numbers[],
               string vhdlnames[], string vhdl_prefixs[], string memfiles[], int longueur);
extern void get_data_successors(const CDFGnode *n, vector<const Operation*> *v);
extern void get_data_predecessors(const CDFGnode *n, vector<const Operation*> *v);
extern void data_WR(CDFG *cdfg, Cadency *cadency, Clock *clk,Mem *mem, Banks *banks);
void MWBM_MOD(const Operation *operation_to_bind, vector<const Operation*> *existing_FUs, long *num_id, Cadency *cadency);


