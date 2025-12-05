#include "multimode_binding.h"
#include "multimode_tools.h"
//#include "cdfg.h"
#include "operation.h"
#include <limits>

using namespace std;

void rrt_construction(CDFG*cdfg_prim, AllocationOutStep*alloc, Cadency*cadency, Clock*clk, Switches*sw)
{
	long S, C, IT;
		for(C = 0; C < cadency->length()/clk->period(); C++)
		{
			AllocationOutCycle AOC;
				alloc->allocationOutCycle().push_back(AOC);
				AllocationOutCycle*AOC_bis = &alloc->allocationOutCycle()[C];

				vector<CDFGnode*> v_node = cdfg_prim->nodes();
				for(IT = 0; IT < v_node.size(); IT++)
				{
					CDFGnode*node = v_node[IT];
						if(node->type()!= CDFGnode::OPERATION)continue;
							Operation*o = (Operation*)node;
								if(o->function()->passThrough())continue;
									if((node->start()%sw->cadency_prim_val()) == C* clk->period())
									{
										OperatorRefs::const_iterator it;
											for(it = o->function()->implemented_by().begin(); it!= o->function()->implemented_by().end(); it++)
											{
												OperatorRef op_ref = (*it).second;
													const Operator*op = op_ref;
													if(op->isMultiFunction())continue;
														vector<const Operator*>*AOC_op_v = &AOC_bis->operators();
															AOC_op_v->push_back(op);
											}
									}
				}
		}
}

int operation_nodes(CDFG*cdfg_prim)
{
	long IT;
	int nb_ops = 0;
	vector<CDFGnode*> v_node = cdfg_prim->nodes();
	for(IT = 0; IT < v_node.size(); IT++)
	{
		CDFGnode*node = v_node[IT];
		if(node->type()!= CDFGnode::OPERATION)continue;
		nb_ops ++;
	}
	return nb_ops;
}


void find_conflict_operations(CDFG*cdfg_prim, int NB_DFG, Cadency*cadency, long cadency_prim, Clock*clk, long start, const Operation*at_start_op,
							  vector<const Operation*>*v_in, vector<const Operation*>*non_comp_ops, vector<const Operation*>*conflict_ops, long cases)
{
	int i,j;
		long time_step_sec, time_step_prim, time_start, conflict_limit;
		long limit;
		//vector<const CDFGnode*>::const_iterator v_it;
		int v_it;
		vector<const Operation*>::const_iterator n_it, out_it;
		const Operation*op;
		bool can_used = true;

		time_step_sec = start%cadency->length();
		time_step_prim = start%cadency_prim;
		conflict_limit = at_start_op->length()- clk->period();
		for(j=time_step_sec+conflict_limit;j>=time_step_sec-conflict_limit;j=j-clk->period())
		{
			for(i=0; i<v_in->size();i++)
			{
				if(j==v_in->at(i)->start()%cadency->length())
				{
					conflict_ops->push_back(v_in->at(i));
				}
			}
		}
	// Find the can_used_op in the cdfg1(main CDFG)except the operations that have
	// the same "operator number(identification)" as the operations that cannot be used(cannot_used_op)
	limit=time_step_prim;
		j= time_step_prim+conflict_limit;
		vector<CDFGnode*> v_node = cdfg_prim->nodes();
		//for(v_it=cdfg_prim->nodes().begin();v_it!=cdfg_prim->nodes().end();v_it++) {
		for(v_it=0; v_it < v_node.size(); v_it++)
		{
			const CDFGnode* node = v_node[v_it];
				//if((*v_it)->type() == CDFGnode::OPERATION) {
				if(node->type() == CDFGnode::OPERATION)
				{
					//op = (const Operation*)(*v_it);
					op = (const Operation*)node;
						if(op->function()->passThrough())continue;
							if(((cases==10)||(cases==12)||(cases==20)||(cases==22))&&(NB_DFG<1))
							{
								if(((op->start()%cadency_prim==j)||(op->start()%cadency_prim==limit))&&(compatible_operations(at_start_op, op)))
								{
									conflict_ops->push_back(op);
								}
							}
				}
		}
		}

		void find_conflict_operations_one_dfg(Cadency*cadency, Clock*clk, long start, const Operation*at_start_op,
											  vector<const Operation*>*v_in, vector<const Operation*>*conflict_ops)
		{
			int i,j;
				long time_step_sec, time_step_prim, time_start, conflict_limit;
				long limit;
				//vector<const CDFGnode*>::const_iterator v_it;
				int v_it;
				vector<const Operation*>::const_iterator n_it, out_it;
				const Operation*op;
				bool can_used = true;

				time_step_sec = start%cadency->length();
				conflict_limit = at_start_op->length()- clk->period();
				for(j=time_step_sec+conflict_limit;j>=time_step_sec-conflict_limit;j=j-clk->period())
				{
					for(i=0; i<v_in->size();i++)
					{
						if(j==v_in->at(i)->start()%cadency->length())
						{
							conflict_ops->push_back(v_in->at(i));
						}
					}
				}
		}

		void get_can_used_op_one_dfg(vector<const Operation*>*bound_operations, const Operation* operation_to_bind, vector<const Operation*>*can_used_op,
									 Clock*clk, Cadency*cadency, long start)
		{
			int i,j, v_it;
				long time_step, time_start, conflict_limit;
				//vector<const CDFGnode*>::const_iterator v_it;
				vector<const Operation*> temp_used_op;
				vector<const Operation*>::const_iterator n_it, out_it;
				vector<const Operation*> conflict_ops;
				const Operation*op;
				bool can_used = true;

				find_conflict_operations_one_dfg(cadency, clk, start, operation_to_bind, bound_operations, &conflict_ops);
				// Find the can_used_op in the bound_op(specifically in the secondary)
				for(n_it=bound_operations->begin();n_it!=bound_operations->end();n_it++)
				{
					for(out_it=conflict_ops.begin();out_it!=conflict_ops.end();out_it++)
					{
						if(merged_operators(*out_it,*n_it))
						{
							can_used=false;
								out_it = conflict_ops.end()-1;
						}
						else can_used = true;
					}
					if(can_used && compatible_operations(operation_to_bind,*n_it))
						can_used_op->push_back(*n_it);
				}
		}

		void get_can_used_op(CDFG*cdfg_prim, int NB_DFG, vector<const Operation*>*bound_operations,
							 const Operation* operation_to_bind, vector<const Operation*>*can_used_op, vector<const Operation*>*non_comp_ops,
							 Clock*clk, Cadency*cadency, long cadency_prim, long start, long cases)
		{
			int i,j, v_it;
				long time_step, time_start, conflict_limit;
				//vector<const CDFGnode*>::const_iterator v_it;
				vector<const Operation*> temp_used_op;
				vector<const Operation*>::const_iterator n_it, out_it;
				vector<const Operation*> conflict_ops;
				const Operation*op;
				bool can_used = true;

				find_conflict_operations(cdfg_prim, NB_DFG, cadency,cadency_prim,clk,start,operation_to_bind,bound_operations, non_comp_ops, &conflict_ops, cases);
				// Find the can_used_op in the bound_op(specifically in the secondary)
				for(n_it=bound_operations->begin();n_it!=bound_operations->end();n_it++)
				{
					for(out_it=conflict_ops.begin();out_it!=conflict_ops.end();out_it++)
					{
						if(merged_operators(*out_it,*n_it))
						{
							can_used=false;
								out_it = conflict_ops.end()-1;
						}
						else can_used = true;
					}
					if(can_used && compatible_operations(operation_to_bind,*n_it)&& !identical_operations(*n_it, can_used_op))
						can_used_op->push_back(*n_it);
				}
			// Find the can_used_op in the cdfg1(main CDFG)except the operations that have
			// the same "operator number(identification)" as the operations that cannot be used(cannot_used_op)
			vector<CDFGnode*> v_node = cdfg_prim->nodes();
				for(v_it=0; v_it < v_node.size(); v_it++)
				{
					const CDFGnode* node = v_node[v_it];
						if(node->type() == CDFGnode::OPERATION)
						{
							op = (const Operation*)node;
								if(op->function()->passThrough())continue;
									for(out_it=conflict_ops.begin();out_it!=conflict_ops.end();out_it++)
									{
										if(merged_operators(*out_it, op))
										{
											can_used=false;
												out_it = conflict_ops.end()-1;
										}
										else can_used = true;
									}
							if(can_used && compatible_operations(operation_to_bind, op)&& !identical_operations(op, can_used_op))
								can_used_op->push_back(op);
						}
				}
		}
	void MWBM_cycles(vector<const Operation*>*operations_to_bind, vector<const Operation*>*existing_FUs,
					 vector<const Operation*>*bound_operations, vector<const Operation*>*found_FUs, vector<const Operation*>*bound_op_temp, vector<const Operation*>*not_bound_op,
					 Cadency*cadency, long start, long cases)
	{
		int ob, efu;
			int l, k;
			int count_matching, nb_op_to_bind;
			int bound=0;
			int found_FU=0;
			int row, col;
			long NbPredB, NbSuccC;
			bool yet_used_op = false;
			bool yet_used_index = false;
			bool equal_weight;
			vector<const Operation*> predecessors1, successors1, predecessors2, successors2;
			vector<const Operation*> can_used_op;

			if(!operations_to_bind->empty()&& !existing_FUs->empty())//if
			{
				int nrows = operations_to_bind->size();
					int ncols = existing_FUs->size();
					Matrix<double> matrix(nrows, ncols);

					for(row = 0; row < nrows; row++)
					{
						predecessors2.clear();
							successors2.clear();
							get_operations_predecessors(operations_to_bind->at(row), &predecessors2);
							get_operations_successors(operations_to_bind->at(row), &successors2);
							for(col = 0; col < ncols; col++)
							{
								NbPredB = 0;
									NbSuccC = 0;
									if(compatible_operations(operations_to_bind->at(row), existing_FUs->at(col)))
									{
										Pred_Succ_Computation(existing_FUs->at(col),&predecessors1,&successors1,&predecessors2,&successors2,
															  &NbPredB,&NbSuccC,start,0);
											matrix(row,col) = NbPredB + NbSuccC;
									}
									else
									{
										matrix(row,col) = std::numeric_limits< double >::max();
									}
							}
					}
				Munkres m;
					m.solve(matrix);
					for(row = 0; row < nrows; row++)
					{
						for(col = 0; col < ncols; col++)
						{
							if(matrix(row,col) == 0)
							{
								if(!existing_FUs->empty())
								{
									if(compatible_operations(operations_to_bind->at(row), existing_FUs->at(col)))
									{
										assigned_identify(existing_FUs->at(col), operations_to_bind->at(row));
											bound_op_temp->push_back(operations_to_bind->at(row));
											get_bound_operations(existing_FUs->at(col),operations_to_bind->at(row), found_FUs,
																 bound_operations);
									}
								}
							}
						}
					}
			}
	}

		void MLEA_cycles(vector<const Operation*>*operations_to_bind, vector<const Operation*>*existing_FUs,
						 vector<const Operation*>*bound_operations, vector<const Operation*>*found_FUs, vector<const Operation*>*bound_op_temp, vector<const Operation*>*not_bound_op,
						 Cadency*cadency, long start, long cases)
		{
			int ob, efu;
				int l, k;
				int count_matching, nb_op_to_bind;
				int bound=0;
				int found_FU=0;
				int row, col;
				long NbPredB, NbSuccC;
				bool yet_used_op = false;
				bool yet_used_index = false;
				bool equal_weight;
				vector<const Operation*> predecessors1, successors1, predecessors2, successors2;
				vector<const Operation*> can_used_op;

				if(!operations_to_bind->empty()&& !existing_FUs->empty())//if
				{
					nb_op_to_bind = operations_to_bind->size();
						for(count_matching=0; count_matching<nb_op_to_bind; count_matching++)
						{
							long w_temp=-1;
								if(!compatibility_status(operations_to_bind, existing_FUs))
								{
									count_matching = nb_op_to_bind-1;
										continue;
								}
							long weight_tmp=-1;
								for(ob=0; ob<operations_to_bind->size(); ob++)
								{
									equal_weight=true;
										long w_ob_efu[1000][1000];
										init_array(w_ob_efu, -1);
										predecessors2.clear();
										successors2.clear();
										get_operations_predecessors(operations_to_bind->at(ob), &predecessors2);
										get_operations_successors(operations_to_bind->at(ob), &successors2);
										for(efu=0; efu<existing_FUs->size(); efu++)
										{
											NbPredB = 0;
												NbSuccC = 0;
												if(compatible_operations(operations_to_bind->at(ob), existing_FUs->at(efu)))
												{
													Pred_Succ_Computation(existing_FUs->at(efu),&predecessors1,&successors1,&predecessors2,&successors2,
																		  &NbPredB,&NbSuccC,start,0);
														w_ob_efu[ob][efu] = NbPredB + NbSuccC;
														if(cases==20||cases==22)
															weight_tmp = w_ob_efu[ob][efu];
												}
										}
									if(cases==20||cases==22)
									{
										for(l=0; l<existing_FUs->size(); l++)
										{
											if((w_ob_efu[ob][l]!=-1)&&(w_ob_efu[ob][l]!=weight_tmp))
											{
												equal_weight=false;
													l=existing_FUs->size()-1;
											}
										}
										if(equal_weight && search_operations(operations_to_bind->at(ob), not_bound_op))
											not_bound_op->push_back(operations_to_bind->at(ob));
									}
									for(l=0;l<existing_FUs->size();l++)
									{
										if(compatible_operations(operations_to_bind->at(ob), existing_FUs->at(l)))
										{
											if(w_ob_efu[ob][l]>w_temp)
											{
												w_temp = w_ob_efu[ob][l];
													bound = ob;
													found_FU = l;
											}
										}
									}
								}
							if(!existing_FUs->empty())
							{
								if(compatible_operations(operations_to_bind->at(bound), existing_FUs->at(found_FU)))
								{
									assigned_identify(existing_FUs->at(found_FU), operations_to_bind->at(bound));
										bound_op_temp->push_back(operations_to_bind->at(bound));
										get_bound_operations(existing_FUs->at(found_FU),operations_to_bind->at(bound),found_FUs,
															 bound_operations);
										erase_element(operations_to_bind, operations_to_bind->at(bound));
										erase_element(existing_FUs, existing_FUs->at(found_FU));
								}
							}
						}
				}
		}


		void MWBM_global(CDFG*cdfg_prim[], int NB_DFG, vector<const Operation*>*operations_to_bind, vector<const Operation*>*bound_op_temp,
						 vector<const Operation*>*bound_operations, vector<const Operation*>*non_comp_ops, vector<const Operation*>*not_bound_op,
						 vector<long>*match, Clock*clk, Cadency*cadency, long cadency_prim[], long start, long*m_temp, long*nb_mux, long cases)
		{

				vector<const Operation*> can_used_op2, v_FUs, found_FUs, bound_op, copy_of_unbound_op;
				vector<const Operation*> predecessors1, successors1, predecessors2, successors2;
				vector<long> match_identifier;
				bool can_used = true;
				bool share_res = false;
				long identifier, temp_ident;
				float NbPredB, NbSuccC, cout_temp, temp;
				int i,j,k,l,m, col, row;
				int bound, found_FU, count_matching;

				if(cases==10)
					get_not_bound_op(operations_to_bind, bound_op_temp, not_bound_op);
				else copy_of_vector(operations_to_bind, not_bound_op);
					if(!not_bound_op->empty())(*nb_mux)++;
						for(int nb_dfgs=0; nb_dfgs<NB_DFG; nb_dfgs++)
						{
							/* m_temp*/
								for(i=0; i<bound_operations->size();i++)
								{
									temp_ident = ((Operation*)bound_operations->at(i))->identify();
										if(!yet_match_indexes(temp_ident, match))match->push_back(temp_ident);
								}
							for(i=0; i<match->size();i++)
							{
								if(match->at(i)>(*m_temp))(*m_temp) =match->at(i);
							}
							/* end*/
								for(i=0; i<not_bound_op->size(); i++)
								{
									vector<const Operation*> can_used_op;
										get_can_used_op(cdfg_prim[nb_dfgs], nb_dfgs, bound_operations,not_bound_op->at(i),&can_used_op2, non_comp_ops, clk,cadency,cadency_prim[nb_dfgs],start,cases);
								}
							get_used_FUs(&can_used_op2, &v_FUs);

								int nrows = not_bound_op->size();
								int ncols = v_FUs.size();
								Matrix<double> matrix(nrows, ncols);

								for(row = 0; row < nrows; row++)
								{
									predecessors2.clear();
										successors2.clear();
										get_operations_predecessors(not_bound_op->at(row), &predecessors2);
										get_operations_successors(not_bound_op->at(row), &successors2);
										for(col = 0; col < ncols; col++)
										{
											if(compatible_operations(not_bound_op->at(row), v_FUs.at(col)))
											{
												temp=0;
													for(k=0; k<can_used_op2.size(); k++)
													{
														NbPredB=0;
															NbSuccC=0;
															cout_temp=0;
															if(merged_operators(v_FUs[col], can_used_op2[k]))
															{
																Pred_Succ_Computation(can_used_op2[k],&predecessors1,&successors1,&predecessors2,&successors2,
																					  &NbPredB,&NbSuccC,start,1);
																	cout_temp=NbPredB+NbSuccC;
																	temp = temp + cout_temp;
															}
													}
												matrix(row,col) = temp;
											}
											else
											{
												matrix(row,col) = std::numeric_limits< double >::max();
											}
										}
								}
							Munkres m;
								m.solve(matrix);
								for(row = 0; row < nrows; row++)
								{
									for(col = 0; col < ncols; col++)
									{
										if(matrix(row,col) == 0)
										{
											if(!v_FUs.empty())
											{
												if(compatible_operations(not_bound_op->at(row), v_FUs.at(col)))
												{
													assigned_identify(v_FUs.at(col), not_bound_op->at(row));
														get_bound_operations(v_FUs.at(col),not_bound_op->at(row),&found_FUs,
																			 bound_operations);
														bound_op.push_back(not_bound_op->at(row));
												}
											}
										}
									}
								}
						}
			get_not_bound_op(not_bound_op, &bound_op, &copy_of_unbound_op);
				for(i=0; i<copy_of_unbound_op.size(); i++)
				{
					(*m_temp)++;
						((Operation*)copy_of_unbound_op.at(i))->identify((*m_temp));
						bound_operations->push_back(copy_of_unbound_op.at(i));
				}

		}

		void MLEA_global(CDFG*cdfg_prim[], int NB_DFG, vector<const Operation*>*operations_to_bind, vector<const Operation*>*bound_op_temp,
						 vector<const Operation*>*bound_operations, vector<const Operation*>*non_comp_ops, vector<const Operation*>*not_bound_op,
						 vector<long>*match, Clock*clk, Cadency*cadency, long cadency_prim[], long start, long*m_temp, long*nb_mux, long cases)
		{

				vector<const Operation*> can_used_op2, v_FUs, found_FUs;
				vector<const Operation*> predecessors1, successors1, predecessors2, successors2;
				vector<long> match_identifier;
				bool can_used = true;
				bool share_res = false;
				long identifier, temp_ident;
				float NbPredB, NbSuccC, cout_temp, temp;
				int i,j,k,l,m;
				int bound, found_FU, count_matching;

				if(cases==10||cases==12||cases==20||cases==22)
					get_not_bound_op(operations_to_bind, bound_op_temp, not_bound_op);
				else copy_of_vector(operations_to_bind, not_bound_op);
					if(!not_bound_op->empty())(*nb_mux)++;
						for(int nb_dfgs=0; nb_dfgs<NB_DFG; nb_dfgs++)
						{
							/* m_temp*/
								for(i=0; i<bound_operations->size();i++)
								{
									temp_ident = ((Operation*)bound_operations->at(i))->identify();
										if(!yet_match_indexes(temp_ident, match))match->push_back(temp_ident);
								}
							for(i=0; i<match->size();i++)
							{
								if(match->at(i)>(*m_temp))(*m_temp) =match->at(i);
							}
							/* end*/
								int nombre_op = not_bound_op->size();
								for(i=0; i<not_bound_op->size(); i++)
								{
									vector<const Operation*> can_used_op;
										get_can_used_op(cdfg_prim[nb_dfgs], nb_dfgs, bound_operations,not_bound_op->at(i),&can_used_op2, non_comp_ops, clk,cadency,cadency_prim[nb_dfgs],start,cases);
								}
							get_used_FUs(&can_used_op2, &v_FUs);
								for(count_matching=0; count_matching<nombre_op; count_matching++)
								{
									float w_temp=-1;
										if(!compatibility_status(not_bound_op, &v_FUs))
										{
											count_matching = nombre_op-1;
												continue;
										}
									for(i=0; i<not_bound_op->size(); i++)
									{
										float w[1000][1000];
											init_array(w, -1);
											predecessors2.clear();
											successors2.clear();
											get_operations_predecessors(not_bound_op->at(i), &predecessors2);
											get_operations_successors(not_bound_op->at(i), &successors2);
											for(j=0; j<v_FUs.size(); j++)
											{
												temp=0;
													for(k=0; k<can_used_op2.size(); k++)
													{
														NbPredB=0;
															NbSuccC=0;
															cout_temp=0;
															if(merged_operators(v_FUs[j], can_used_op2[k]))
															{
																Pred_Succ_Computation(can_used_op2[k],&predecessors1,&successors1,&predecessors2,&successors2,
																					  &NbPredB,&NbSuccC,start,1);
																	if(cases==4)
																	{
																		if((can_used_op2[k]->start()%cadency->length()) == (not_bound_op->at(i)->start()%cadency->length()))
																			cout_temp= 0;
																		else cout_temp=NbPredB+NbSuccC;
																	}
																	else cout_temp=NbPredB+NbSuccC;
																		temp = temp + cout_temp;
															}
													}
												if(cases==12||cases==22||cases==3)
												{
													if(w[i][j]==0)w[i][j]=0;
													else w[i][j]=1/temp;
												}
												else w[i][j] = temp;
											}
										for(l=0; l<v_FUs.size(); l++)
										{
											if(compatible_operations(not_bound_op->at(i), v_FUs[l]))
											{
												if(w[i][l]>w_temp)
												{
													w_temp = w[i][l];
														bound = i;
														found_FU = l;
												}
											}
										}
									}
									if(!v_FUs.empty())
									{
										if(compatible_operations(not_bound_op->at(bound), v_FUs[found_FU]))
										{
											assigned_identify(v_FUs[found_FU], not_bound_op->at(bound));
												get_bound_operations(v_FUs[found_FU],not_bound_op->at(bound),&found_FUs,
																	 bound_operations);
												erase_element(not_bound_op, not_bound_op->at(bound));
												erase_element(&v_FUs, v_FUs[found_FU]);
										}
									}

								}
						}
			for(i=0; i<not_bound_op->size(); i++)
			{
				(*m_temp)++;
					((Operation*)not_bound_op->at(i))->identify((*m_temp));
					bound_operations->push_back(not_bound_op->at(i));
			}
		}

		void MWBM_ONE_DFG(vector<const Operation*>*operations_to_bind, vector<const Operation*>*bound_operations, vector<long>*match, Clock*clk,
						  Cadency*cadency, long start, long*m_temp, long*nb_mux)
		{

				vector<const Operation*> can_used_op2, v_FUs, found_FUs, bound_op, copy_of_unbound_op;
				vector<const Operation*> predecessors1, successors1, predecessors2, successors2;
				vector<long> match_identifier;
				bool can_used = true;
				bool share_res = false;
				long identifier, temp_ident;
				float NbPredB, NbSuccC, cout_temp, temp;
				int i,j,k,l,row, col;
				int bound, found_FU, count_matching;


				for(i=0; i<bound_operations->size();i++)
				{
					temp_ident = ((Operation*)bound_operations->at(i))->identify();
						if(!yet_match_indexes(temp_ident, match))match->push_back(temp_ident);
				}
			for(i=0; i<match->size();i++)
			{
				if(match->at(i)>(*m_temp))(*m_temp) =match->at(i);
			}
			int nombre_op = operations_to_bind->size();
				for(i=0; i<operations_to_bind->size(); i++)
				{
					vector<const Operation*> can_used_op;
						get_can_used_op_one_dfg(bound_operations, operations_to_bind->at(i),&can_used_op2, clk, cadency, start);
				}
			get_used_FUs(&can_used_op2, &v_FUs);
				int nrows = operations_to_bind->size();
				int ncols = v_FUs.size();
				Matrix<double> matrix(nrows, ncols);

				for(row = 0; row < nrows; row++)
				{
					predecessors2.clear();
						successors2.clear();
						get_operations_predecessors(operations_to_bind->at(row), &predecessors2);
						get_operations_successors(operations_to_bind->at(row), &successors2);
						for(col = 0; col < ncols; col++)
						{
							///cout <<"Operation a assigner "<< not_bound_op->at(row)->function_name()<<endl;
							////cout <<"Operateurs existants "<< v_FUs.at(col)->function_name()<<endl;
							if(compatible_operations(operations_to_bind->at(row), v_FUs.at(col)))
							{
								NbPredB=0;
									NbSuccC=0;
									Pred_Succ_Computation(v_FUs[col],&predecessors1,&successors1,&predecessors2,&successors2,
														  &NbPredB,&NbSuccC,start,1);
									matrix(row,col) = NbPredB+NbSuccC;

							}
							else
							{
								matrix(row,col) = std::numeric_limits< double >::max();
							}
						}
				}
			Munkres m;
				m.solve(matrix);
				for(row = 0; row < nrows; row++)
				{
					for(col = 0; col < ncols; col++)
					{
						if(matrix(row,col) == 0)
						{
							if(!v_FUs.empty())
							{
								if(compatible_operations(operations_to_bind->at(row), v_FUs.at(col)))
								{
									assigned_identify(v_FUs.at(col), operations_to_bind->at(row));
										get_bound_operations(v_FUs.at(col),operations_to_bind->at(row),&found_FUs,
															 bound_operations);
										bound_op.push_back(operations_to_bind->at(row));
								}
							}
						}
					}
				}
			get_not_bound_op(operations_to_bind, &bound_op, &copy_of_unbound_op);
				for(i=0; i<copy_of_unbound_op.size(); i++)
				{
					(*m_temp)++;
						((Operation*)copy_of_unbound_op.at(i))->identify((*m_temp));
						bound_operations->push_back(copy_of_unbound_op.at(i));
				}

		}


		void MLEA_ONE_DFG(vector<const Operation*>*operations_to_bind, vector<const Operation*>*bound_operations, vector<long>*match, Clock*clk,
						  Cadency*cadency, long start, long*m_temp, long*nb_mux)
		{

				vector<const Operation*> can_used_op2, v_FUs, found_FUs;
				vector<const Operation*> predecessors1, successors1, predecessors2, successors2;
				vector<long> match_identifier;
				bool can_used = true;
				bool share_res = false;
				long identifier, temp_ident;
				float NbPredB, NbSuccC;
				int i,j,k,l,m;
				int bound, found_FU, count_matching;


				for(i=0; i<bound_operations->size();i++)
				{
					temp_ident = ((Operation*)bound_operations->at(i))->identify();
						if(!yet_match_indexes(temp_ident, match))match->push_back(temp_ident);
				}
			for(i=0; i<match->size();i++)
			{
				if(match->at(i)>(*m_temp))(*m_temp) =match->at(i);
			}
			int nombre_op = operations_to_bind->size();
				for(i=0; i<operations_to_bind->size(); i++)
				{
					vector<const Operation*> can_used_op;
						get_can_used_op_one_dfg(bound_operations, operations_to_bind->at(i),&can_used_op2, clk, cadency, start);
				}
			get_used_FUs(&can_used_op2, &v_FUs);
				for(count_matching=0; count_matching<nombre_op; count_matching++)
				{
					float w_temp=-1;
						if(!compatibility_status(operations_to_bind, &v_FUs))
						{
							count_matching = nombre_op-1;
								continue;
						}
					for(i=0; i<operations_to_bind->size(); i++)
					{
						float w[1000][1000];
							init_array(w, -1);
							predecessors2.clear();
							successors2.clear();
							get_operations_predecessors(operations_to_bind->at(i), &predecessors2);
							get_operations_successors(operations_to_bind->at(i), &successors2);
							for(j=0; j<v_FUs.size(); j++)
							{
								NbPredB=0;
									NbSuccC=0;
									Pred_Succ_Computation(v_FUs[j],&predecessors1,&successors1,&predecessors2,&successors2,
														  &NbPredB,&NbSuccC,start,1);

									w[i][j]=NbPredB+NbSuccC;
							}
						for(l=0; l<v_FUs.size(); l++)
						{
							if(compatible_operations(operations_to_bind->at(i), v_FUs[l]))
							{
								if(w[i][l]>w_temp)
								{
									w_temp = w[i][l];
										bound = i;
										found_FU = l;
								}
							}
						}
					}
					if(!v_FUs.empty())
					{
						if(compatible_operations(operations_to_bind->at(bound), v_FUs[found_FU]))
						{
							assigned_identify(v_FUs[found_FU], operations_to_bind->at(bound));
								get_bound_operations(v_FUs[found_FU],operations_to_bind->at(bound),&found_FUs,
													 bound_operations);
								erase_element(operations_to_bind, operations_to_bind->at(bound));
								erase_element(&v_FUs, v_FUs[found_FU]);
						}
					}

				}
			for(i=0; i<operations_to_bind->size(); i++)
			{
				(*m_temp)++;
					((Operation*)operations_to_bind->at(i))->identify((*m_temp));
					bound_operations->push_back(operations_to_bind->at(i));
			}
		}

		void one_dfg_binding(CDFG*cdfg_sec, Clock*clk, Cadency*cadency, long nb_stages, string cases, vector<const Operation*>*bound_operations)
		{
			long start;
				int i,j,k,l=0;
				/////const CDFGnode*source_prim = cdfg_prim[0]->source();//->source();
				const CDFGnode*source_sec = cdfg_sec->source(); // à modifier
			vector<const Operation*> existing_FUs;
				vector<const Operation*> comp_ops, non_comp_ops;
				vector<long> match;
				long m_temp=0;
				long nb_MUXes=0;
				long nb_op_at_start=0;
				long Nb_REG=0;
				bool yet_used_op = false;
				bool yet_used_index = false;

				for(start = 0; start < cadency->length(); start += clk->period())
				{

						vector<const Operation*> operations_to_bind_at_start;
						vector<const Operation*> copy_op_to_bind, bound_op_temp, not_bound_op;
						vector<const Operation*>::iterator it;


						nb_op_at_start=bound_operations->size();
						int nb_reg=0;
						for(i=0; i<nb_op_at_start; i++)
						{
							if(bound_operations->at(i)->function()->passThrough())nb_reg=nb_reg+1;
							else nb_reg=nb_reg+2;
						}
					if(Nb_REG<nb_reg)Nb_REG=nb_reg;
						get_operations_nodes_at_start(source_sec, &operations_to_bind_at_start, start, cadency->length());
							nb_op_at_start=operations_to_bind_at_start.size();
							nb_reg=0;
							for(i=0; i<nb_op_at_start; i++)
							{
								if(operations_to_bind_at_start[i]->function()->passThrough())nb_reg=nb_reg+1;
								else nb_reg=nb_reg+2;
							}
					if(Nb_REG<nb_reg)Nb_REG=nb_reg;
						if(operations_to_bind_at_start.empty())continue;
							copy_of_vector(&operations_to_bind_at_start, &copy_op_to_bind);
								if((cases == "mwbm_cycles")||(cases == "mwbm_global"))// MWBM binding is used
									MWBM_ONE_DFG(&copy_op_to_bind, bound_operations, &match, clk, cadency, start, &m_temp, &nb_MUXes);
								else
								{
									if((cases == "mlea_cycles")||(cases == "mlea_global"))// MLEA binding is used
										MLEA_ONE_DFG(&copy_op_to_bind, bound_operations, &match, clk, cadency, start, &m_temp, &nb_MUXes);
									else
										MWBM_ONE_DFG(&copy_op_to_bind, bound_operations, &match, clk, cadency, start, &m_temp, &nb_MUXes);
								}
				}
		}
	// Binding: First, bind the operations to functional units using the Maximum Bipartite weighted matching algorithm(MWBM)
	// Binding cont: afterwards, bind the non-bound operations(binding_cont)
	// Weights are the predecessors and succussors number
	//cdfg2: the secondary dfg
	//cdfg1: the main dfg
	//bound_operations2: the operations bounded to the operations in the main DFG
	void dfgs_binding(CDFG*cdfg_sec, CDFG*cdfg_prim[], int Nb_graphes, Clock*clk, Cadency*cadency, long cadency_prim[], long nb_stages, string cases, vector<const Operation*>*bound_operations)
	{
		long start;
			int i,j,k,l=0;
			const CDFGnode*source_prim = cdfg_prim[0]->source();//->source();
		const CDFGnode*source_sec = cdfg_sec->source(); // à modifier
		vector<const Operation*> existing_FUs;
			vector<const Operation*> comp_ops, non_comp_ops;
			vector<long> match;
			long m_temp=0;
			long nb_MUXes=0;
			long nb_op_at_start=0;
			long Nb_REG=0;
			bool yet_used_op = false;
			bool yet_used_index = false;

			for(start = 0; start < cadency->length(); start += clk->period())
			{

					vector<const Operation*> existing_FUs_at_start, operations_to_bind_at_start;
					vector<const Operation*> copy_op_to_bind, bound_op_temp, not_bound_op;
					vector<const Operation*>::iterator it;

					get_operations_nodes_at_start(source_prim, &existing_FUs_at_start, start, cadency_prim[0]);
					nb_op_at_start=existing_FUs_at_start.size();
					int nb_reg=0;
					for(i=0; i<nb_op_at_start; i++)
					{
						if(existing_FUs_at_start[i]->function()->passThrough())nb_reg=nb_reg+1;
						else nb_reg=nb_reg+2;
					}
				if(Nb_REG<nb_reg)Nb_REG=nb_reg;
					get_operations_nodes_at_start(source_sec, &operations_to_bind_at_start, start, cadency->length());
						nb_op_at_start=operations_to_bind_at_start.size();
						nb_reg=0;
						for(i=0; i<nb_op_at_start; i++)
						{
							if(operations_to_bind_at_start[i]->function()->passThrough())nb_reg=nb_reg+1;
							else nb_reg=nb_reg+2;
						}
				if(Nb_REG<nb_reg)Nb_REG=nb_reg;
					if(operations_to_bind_at_start.empty())continue;
						copy_of_vector(&operations_to_bind_at_start, &copy_op_to_bind);
							// En cas d'égalité de poids, on fait un choix aléatoire
							if(cases == "mwbm_cycles")// Inter-cycle(fréquence des motifs)
							{
								MWBM_cycles(&operations_to_bind_at_start,&existing_FUs_at_start,bound_operations,&existing_FUs,&bound_op_temp,&not_bound_op,cadency,start,10);
									MWBM_global(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 10);
							}
							else
							{
								if(cases == "mlea_cycles")// Inter-cycle(fréquence des motifs)
								{
									MWBM_cycles(&operations_to_bind_at_start,&existing_FUs_at_start,bound_operations,&existing_FUs,&bound_op_temp,&not_bound_op,cadency,start,10);
										MWBM_global(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 10);
								}
								/***case 12: // Inter-cycle(inverse de la fréquence des motifs)
								  MWBM(&operations_to_bind_at_start,&existing_FUs_at_start,bound_operations,&existing_FUs,&bound_op_temp,&not_bound_op,cadency,start,12);
								  binding_cont(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 12);
								  break;
								// En cas d'égalité de poids, on garde les opérations isolées et on les assigne d'une manière globale
								case 20: // Inter-cycle(fréquence des motifs)
								MWBM(&operations_to_bind_at_start,&existing_FUs_at_start,bound_operations,&existing_FUs,&bound_op_temp,&not_bound_op,cadency,start,20);
								binding_cont(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 20);
								break;
								case 22: // Inter-cycle(inverse de la fréquence des motifs)
								MWBM(&operations_to_bind_at_start,&existing_FUs_at_start,bound_operations,&existing_FUs,&bound_op_temp,&not_bound_op,cadency,start,22);
								binding_cont(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 22);
								break;***/
								else
								{
									// Considération des opérateurs globaux
									if(cases == "mwbm_global")// fréquence des motifs
										MWBM_global(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 1);
									else
										if(cases == "mlea_global")// fréquence des motifs
											MWBM_global(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 1);
												/***case 3: // inverse de la fréquence des motifs
												  binding_cont(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 3);
												  break;
												  case 4: // opérateurs inter-cycle non prioritaires
												  binding_cont(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 4);
												  break;****/
												// En cas d'égalité de poids, on fait un choix aléatoire
										else // Inter-cycle(fréquence des motifs)
										{
											MWBM_cycles(&operations_to_bind_at_start,&existing_FUs_at_start,bound_operations,&existing_FUs,&bound_op_temp,&not_bound_op,cadency,start, 10);
												MWBM_global(cdfg_prim, Nb_graphes, &copy_op_to_bind,&bound_op_temp,bound_operations,&non_comp_ops,&not_bound_op,&match,clk,cadency,cadency_prim,start,&m_temp, &nb_MUXes, 10);
										}
								}
							}
			}
		/**	cout <<"Nombre de REG MIN "<<Nb_REG<<endl;
		  cout <<"Nombre de MUXes a rajouter dans le controleur "<<nb_MUXes<<endl;
		  cout << bound_operations->size()<< endl;
		  cout << existing_FUs.size()<< endl;***/
	}

		// Binding end : Bind the non bound operations in the secondary CDFG
		// Partially_bound_op : operations that have been bound during the first binding
		// bound_op : the output which contains the bound operations
		void spact_mr_binding(CDFG*cdfg2, CDFG*cdfg1, Clock*clk, Cadency*cadency, long nb_stages, vector<const CDFGnode*>*partially_bound_op,
							  vector<const CDFGnode*>*bound_op)
		{
			//vector<const CDFGnode*>::const_iterator n_it;
			int n_it;
				vector<const CDFGnode*>::const_iterator v_it;
				vector<const CDFGnode*> not_bound_op; // Vector of non bound operations
			vector<long> match_identifier, match;
				const CDFGnode*n;
				const Operation*op;
				long identifier,temp_ident;
				long m_temp=0;
				long start;
				long nb_ops;
				bool bound=false;
				bool share_res=false;
				int i=0;

				for(i=0; i<partially_bound_op->size();i++)bound_op->push_back(partially_bound_op->at(i));
					// Fill the vector of non bound operations
					vector<CDFGnode*> v_node = cdfg2->nodes();
						for(n_it=0; n_it < v_node.size(); n_it++)
						{
							const CDFGnode* node = v_node[n_it];
								//for(n_it = cdfg2->nodes().begin(); n_it != cdfg2->nodes().end(); n_it++) {
								if(node->type() == CDFGnode::OPERATION)
								{
									op = (const Operation*)node;
										for(v_it=partially_bound_op->begin();v_it!=partially_bound_op->end();v_it++)
										{
											if((*v_it)->name() ==op->name())
											{
												bound=true;
													v_it = partially_bound_op->end()-1;
											}
											else bound = false;
										}
									if(!bound)
									{
										not_bound_op.push_back(node);
									}
								}
								else continue;
						}
						for(start = 0; start < nb_stages*cadency->length(); start += clk->period())
						{
							vector<const CDFGnode*> not_bound_start, cannot_used_op, can_used_op, can_used_op_clk;
								// All these vectors are used only for the operations with length>clk->period
								//not_bound_start : vector of non bound operations at c-step=start
								//cannot_used_op : vector of operations that are not compatible with not_bound_start
								//can_used_op : vector of operations that are compatible with not_bound_start
								bool can_used = true;
								for(i=0; i<bound_op->size();i++)
								{
									temp_ident = ((Operation*)bound_op->at(i))->identify();
										if(!yet_match_indexes(temp_ident, &match))match.push_back(temp_ident);
								}
							for(i=0; i<match.size();i++)
							{
								if(match[i]>m_temp)m_temp=match[i];
							}
							get_operations_at_start(&not_bound_op,&not_bound_start,start);
								if(not_bound_start.empty())continue;
									match_identifier.clear();
										for(i=0; i<not_bound_start.size(); i++)
										{
											can_used_op.clear();
												can_used_op_clk.clear();
												if(not_bound_start[i]->length()<=clk->period())
												{
													get_pred_at_start_clk(cdfg2,cdfg1,cadency,clk,bound_op,&can_used_op_clk,not_bound_start[i],start);
														for(int j=0; j<can_used_op_clk.size();j++)
														{
															if((((Operation*)(can_used_op_clk[j]))->function()->name()
																== ((Operation*)(not_bound_start[i]))->function()->name())&&
															   (!yet_match_indexes(((Operation*)(can_used_op_clk[j]))->identify(),&match_identifier)))
															{
																share_res=true;
																	match_identifier.push_back(((Operation*)can_used_op_clk[j])->identify());
																	identifier= ((Operation*)can_used_op_clk[j])->identify();
																	j=can_used_op_clk.size()-1;
															}
															else share_res=false;
														}
												}
												else
												{
													get_pred_at_start(cdfg2,cdfg1,cadency,clk,bound_op,&can_used_op,not_bound_start[i],start);
														if(can_used_op.size() ==0)share_res=false;
															for(int j=0; j<can_used_op.size();j++)
															{
																if((((Operation*)(can_used_op[j]))->function()->name()
																	== ((Operation*)(not_bound_start[i]))->function()->name())&&
																   (!yet_match_indexes(((Operation*)(can_used_op[j]))->identify(),&match_identifier)))
																{
																	share_res=true;
																		match_identifier.push_back(((Operation*)can_used_op[j])->identify());
																		identifier= ((Operation*)can_used_op[j])->identify();
																		j=can_used_op.size()-1;
																}
																else share_res=false;
															}
												}
											if(share_res)
											{
												((Operation*)not_bound_start[i])->identify(identifier);
													bound_op->push_back(not_bound_start[i]);
											}
											else
											{
												m_temp++;
													((Operation*)not_bound_start[i])->identify(m_temp);
													bound_op->push_back(not_bound_start[i]);
											}
										}
						}
						}
			void fill_scheduling_out(CDFG*cdfg, SchedulingOut*sched, Cadency*cadency, Clock*clk)
			{
				vector<const Operation*> ops;
					long Nbops = 0;
					//long stages = 1;
					long sink_delay = (cdfg->sink()->start()+clk->period())/clk->period();
					///for(int C = 0; C < sink_delay; C++) {
					vector<CDFGnode*> v_node = cdfg->nodes();
					for(int IT = 0; IT < v_node.size(); IT++)
					{
						CDFGnode*node = v_node[IT];
							if(node->type()!= CDFGnode::OPERATION)continue;
								Operation*o = (Operation*)node;
									if(o->function()->passThrough())continue;
										((OperatorInstance*)o->inst())->change_no(o->identify());
											//if((node->start()%sw->cadency_prim_val()) == C* clk->period()) {
											OperatorRefs::const_iterator it;
											for(it = o->function()->implemented_by().begin(); it!= o->function()->implemented_by().end(); it++)
											{
												OperatorRef op_ref = (*it).second;
													Operator*op = op_ref;
													if(op->isMultiFunction())continue;
														if(!same_operators(o, &ops))
														{
															OperatorInstance*oi = new OperatorInstance(op, cadency, clk);
																if(op->name()!="assign_op")Nbops++;
																	oi->change_no(o->identify());
																		sched->stages[0].instances.push_back(oi);
																		oi->bind(o->function());
																		ops.push_back(o);
														}
											}
					}
					////	}
				cout<<"operators = "<<Nbops<<", stages = "<<ceil((double)sink_delay/(double)(cadency->length()/clk->period()));
			}
