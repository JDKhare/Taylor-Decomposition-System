#include "cdfg.h"
#include "operation.h"
#include "allocation.h"
#include "switches.h"
#include "scheduling.h"


void rrt_construction(CDFG *cdfg_prim, AllocationOutStep *alloc, Cadency *cadency, Clock *clk, Switches *sw);

void find_conflict_operations (CDFG *cdfg_prim, int NB_DFG, Cadency *cadency, long cadency_prim, Clock *clk, long start, const Operation *at_start_op, vector<const Operation*> *v_in, vector<const Operation*> *non_comp_ops,
                               vector<const Operation*> *conflict_ops, long cases);
void get_can_used_op(CDFG *cdfg_prim, int NB_DFG, vector<const Operation*> *bound_operations, const Operation* operation_to_bind, vector<const Operation*> *can_used_op, vector<const Operation*> *non_comp_ops,
                     Clock *clk, Cadency *cadency, long cadency_prim, long start, long cases);
void MWBM_cycles(vector<const Operation*> *operations_to_bind, vector<const Operation*> *existing_FUs, vector<const Operation*> *bound_operations, vector<const Operation*> *found_FUs,
          vector<const Operation*> *bound_op_temp, vector<const Operation*> *not_bound_op, Cadency *cadency, long start, long cases);
void MWBM_global(CDFG *cdfg_prim[], int NB_DFG, vector<const Operation*> *operations_to_bind, vector<const Operation*> *bound_op_temp, vector<const Operation*> *bound_operations,
                  vector<const Operation*> *non_comp_ops, vector<const Operation*> *not_bound_op, vector<long> *match,  Clock *clk, Cadency *cadency, long cadency_prim[], long start, long *m_temp, long *Nb_MUXes, long cases);
void MLEA_cycles(vector<const Operation*> *operations_to_bind, vector<const Operation*> *existing_FUs,
          vector<const Operation*> *bound_operations, vector<const Operation*> *found_FUs, vector<const Operation*> *bound_op_temp, vector<const Operation*> *not_bound_op,
          Cadency *cadency, long start, long cases);
void MLEA_global(CDFG *cdfg_prim[], int NB_DFG, vector<const Operation*> *operations_to_bind, vector<const Operation*> *bound_op_temp, vector<const Operation*> *bound_operations,
                  vector<const Operation*> *non_comp_ops, vector<const Operation*> *not_bound_op, vector<long> *match,  Clock *clk, Cadency *cadency, long cadency_prim[], long start, long *m_temp, long *Nb_MUXes, long cases);
// First binding: Bind only the compatible operations using the Maximum Bipartite weighted matching algorithm
// Weights are the predecessors and succussors number
//cdfg2: the secondary dfg
//cdfg1: the main dfg
//bound_operations2: the operations bounded to the operations in the main DFG
void dfgs_binding(CDFG *cdfg2, CDFG *cdfg_prim[], int Nb_graphes, Clock *clk, Cadency *cadency, long cadency_prim[], long nb_stages, string cases, vector<const Operation*> *bound_operations2);
void fill_scheduling_out (CDFG *cdfg, SchedulingOut *sched, Cadency *cadency, Clock *clk);

void spact_mr_binding(CDFG *cdfg2, CDFG *cdfg1, Clock *clk, Cadency *cadency, long nb_stages, vector<const CDFGnode*> *partially_bound_op,
                      vector<const CDFGnode*> *bound_op);

/// One DFG assignment
void find_conflict_operations_one_dfg (Cadency *cadency, Clock *clk, long start, const Operation *at_start_op,
                                       vector<const Operation*> *v_in, vector<const Operation*> *conflict_ops);
void get_can_used_op_one_dfg(vector<const Operation*> *bound_operations, const Operation* operation_to_bind, vector<const Operation*> *can_used_op,
                             Clock *clk, Cadency *cadency, long start);
void MWBM_ONE_DFG(vector<const Operation*> *operations_to_bind, vector<const Operation*> *bound_operations, vector<long> *match, Clock *clk,
                  Cadency *cadency, long start, long *m_temp, long *nb_mux);
void MLEA_ONE_DFG(vector<const Operation*> *operations_to_bind, vector<const Operation*> *bound_operations, vector<long> *match, Clock *clk,
                  Cadency *cadency, long start, long *m_temp, long *nb_mux);
void one_dfg_binding(CDFG *cdfg_sec, Clock *clk, Cadency *cadency, long nb_stages, string cases, vector<const Operation*> *bound_operations);

int operation_nodes(CDFG *cdfg_prim);

extern void multimode_union(int argc, char **argv, Switches sw, Clock clk, TimeUnit tu, Cadency cadency, Mem mem, Lib lib);
extern void multimode_incremental(int argc, char **argv, Switches sw, Clock clk, TimeUnit tu, Cadency cadency, Mem mem, Lib lib);
