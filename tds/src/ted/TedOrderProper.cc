/*
 * =====================================================================================
 *
 *       Filename:  TedOrderProper.cc
 *    Description:
 *        Created:  29/4/2009 11:31:10 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 201                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedParents.h"
#include "TedOrderStrategy.h"
#include "TedOrderCost.h"

namespace ted {


TedMan* TedOrderProper::patchorderVars(void) {
	TedContainer& container = _pManCurrent->getContainer();
	PartialOrder var_to_patch = container.patchOrderGT();
	for(PartialOrder::iterator it = var_to_patch.begin(); it != var_to_patch.end(); it++) {
		TedVar var = it->first;
		for(set<TedVar>::iterator kit = it->second.begin(); kit != it->second.end(); kit++) {
			TedVar var_kit = *kit;
			TedSet& nodes = *(container[var_kit].second);
			bool jump_below = true;
			for (TedSet::iterator jt = nodes.begin(); jt != nodes.end(); jt++) {
				TedNode* pNode = *jt;
				FOREACH_CHILD_OF_NODE(pNode,_iterkids,_iterend) {
					TedNode* pKid = _iterkids.Node<TedNode>();
					if (container.isFirstLower(var,*pKid->getVar())) {
						jump_below = false;
						break;
					}
				}
				if (!jump_below) {
					break;
				}
			}
			if (jump_below) {
				TedMan* new_man = jumpBelow(var_kit, var);
				assert(new_man==NULL || new_man==_pManCurrent);
			}
		}
	}
	var_to_patch.clear();
	TedParents parents;
	_pManCurrent->preOrderDFS(parents, &TedParents::collect);
	var_to_patch = container.patchOrderGT();
	for(PartialOrder::iterator it = var_to_patch.begin(); it != var_to_patch.end(); it++) {
		TedVar var = it->first;
		TedSet& nodes = *(container[var].second);
		for(set<TedVar>::iterator kit = it->second.begin(); kit != it->second.end(); kit++) {
			TedVar var_kit = *kit;
			bool jump_above = true;
			for (TedSet::iterator jt = nodes.begin(); jt != nodes.end(); jt++) {
				TedNode* pNode = *jt;
				if (parents.hasParents(pNode)) {
					for(MyParent::iterator kt = parents[pNode]->begin(), end = parents[pNode]->end(); kt != end; kt++) {
						TedNode* topParent = kt->first;
						if(container.isFirstLower(*topParent->getVar(),var_kit)) {
							jump_above = false;
							break;
						}
					}
					if (!jump_above) {
						break;
					}
				}
			}
			if (jump_above) {
				TedMan* new_man = jumpAbove(var,var_kit);
				assert(new_man==NULL || new_man==_pManCurrent);
			}
		}
	}
	return _pManCurrent;
}

string TedOrderProper::fixorder(PartialOrder& var_lesser_than, bool verboseOnly) {
	TedContainer& container = _pManCurrent->getContainer();
	string instructions = "";
	multimap<int,TedVar> final_order;
	for(PartialOrder::iterator jt = var_lesser_than.begin(); jt != var_lesser_than.end(); jt++) {
		final_order.insert(pair<int,TedVar>(container.findMaxDepth(var_lesser_than,jt->first),jt->first));
	}
	for(multimap<int,TedVar>::reverse_iterator jt = final_order.rbegin(); jt != final_order.rend(); jt++) {
		TedVar var_lower = jt->second;
		set<TedVar>& lorder = var_lesser_than[var_lower];
		TedVar var_upper =*lorder.begin();
		if(lorder.size()>1) {
			//
			int count = 0;
			for(set<TedVar>::iterator kit = lorder.begin(); kit != lorder.end(); kit++) {
				if(container.findMaxDepth(var_lesser_than,*kit)>0) {
					var_upper =*kit;
					count++;
				}
			}
			// the count assertion could be removed.
			// I've added the assertion to double check the following condition(not used anywhere else)
			// the var_lesser_than runs keepMaxDepth which removes all variables > 0 but one variable
			// therefore count should be at most 1
			assert(count<=1);
		}
		// var_lower goes below var_upper
		if(verboseOnly) {
			instructions += "jumpAbove -p ";
			instructions += var_upper.getName();
			instructions += " ";
			instructions += var_lower.getName();
			instructions += "\n";
		} else {
			//TedMan* new_man = jumpBelow(var_lower,var_upper);
			TedMan* new_man = jumpAbove(var_upper, var_lower);
			assert(new_man==NULL || new_man==_pManCurrent);
		}
	}
	return instructions;
}

// retiming operations(construction and manipulation)might result in a bad placement of nodes
// i.e children are on top of parents.
// This method, forces a good ordering of retimed variables.
//
// Note: The suggested order is a solution for full retimed variables,
//       when there are registers across the edges, the suggested solution
//       might leave some un-resolved variable orders
TedMan* TedOrderProper::fixorderRetimedVars(bool verboseOnly) {
	TedContainer& container = _pManCurrent->getContainer();
	BaseVarsRetimed base_retimed = container.getExistingRetimedVarsPerBaseClass();
	string text = "";
	for(BaseVarsRetimed::iterator it = base_retimed.begin(), end=base_retimed.end(); it != end; it++) {
		set<TedVar> &retimed_vars_of_base = it->second;
		const TedVar* base_var = it->first;
		PartialOrder var_lesser_than = container.getPartialOrderLT(retimed_vars_of_base,base_var);
		text += fixorder(var_lesser_than,verboseOnly);
	}
	if(verboseOnly && !text.empty()) {
		cout << "Instructions suggested to restore ordering visualization in the graph (among retimes variables):" << endl;
		cout << text;
	}
	return _pManCurrent;
}

//
// It works, but in some cases it might generate a segmentation fault
//
//   a1-\                     /---a1--\
//       \----a2--\          2R        \
//                 \---a3---/          1R
//                                       \
//                                        \---b
//
//  a1 cannot go down because there is an edge with register
//  if a3 goes up it means that a2 should go up as well; but
//  deep in the reordering process the assumption that the container
//  is ordered is broken
//
//  see the TdsCmd.cc::cmdFixOrder
//
TedMan* TedOrderProper::fixorderVars(bool verboseOnly) {
	TedContainer& container = _pManCurrent->getContainer();
	PartialOrder var_lesser_than = container.getOrderLT();
	string text = fixorder(var_lesser_than,verboseOnly);
	if (verboseOnly && !text.empty()) {
		cout << "Instructions suggested to restore ordering visualization in the graph (among variables):" << endl;
		cout << text;
	}
	return _pManCurrent;
}

/** @brief proper up bouble up
 *  @details there is no need for the duplicate code, this is left temporarily to accomodate
 *  the shell command which require a new manager to be returned.
 **/
TedMan* TedOrderProper::pushUp(const TedVar& var, unsigned int distance) {
	if (distance>0) {
		updateCurrentManager(_pManCurrent);
		for(unsigned int jump = 1; jump <= distance; jump++) {
			pushUp(var);
		}
	}
	return _pManCurrent;
}

TedMan* TedOrderProper::pushUp(const TedVar& var) {
	if(!_pManCurrent->getContainer().isAtTop(var)) {
		unsigned int level = _pManCurrent->getContainer().getLevel(var)+1;
		const TedVar& varUp = _pManCurrent->getContainer().getVarAtLevel(level);
		updateCurrentManager(_pManCurrent);
		reor(varUp, var);
	}
	return _pManCurrent;
}

TedMan* TedOrderProper::pushDown(const TedVar& var, unsigned int distance) {
	if (distance>0) {
		updateCurrentManager(_pManCurrent);
		for(unsigned int jump = 1; jump <= distance; jump++) {
			pushDown(var);
		}
	}
	return _pManCurrent;
}

TedMan* TedOrderProper::pushDown(const TedVar& var) {
	if(!_pManCurrent->getContainer().isAtBottom(var)) {
		unsigned int level = _pManCurrent->getContainer().getLevel(var)-1;
		const TedVar& varDown = _pManCurrent->getContainer().getVarAtLevel(level);
		updateCurrentManager(_pManCurrent);
		reor(var, varDown);
	}
	return _pManCurrent;
}

/**@brief reorders two adjacent variables on a multiple output TED,
  @detail The two nodes have to be adjacent to be swapped. The method assumes
  that varX is on top of VarY in the graph. If you are not sure in
  which var is on top use the method reorder instead.

  First we Split varX nodes into UxWithY and UxWithoutY
  Then as nodes in UxWithoutY do not need to be swapped they are inserted into the final Ux
  */
void TedOrderProper::reor(const TedVar& varX, const TedVar& varY) {
	static char progress[4] = {'|','/','-','\\'};
	static bool show_progress = true;
	if(show_progress) {
		std::cout << progress[0];
		std::flush(std::cout);
	}
	TedContainer& currentContainer = _pManCurrent->getContainer();
	size_t yInitialLevel = currentContainer.getLevel(varY);
	size_t xInitialLevel = currentContainer.getLevel(varX);
	assert(xInitialLevel = yInitialLevel + 1);

	TedParents parents;
	//_pManCurrent->preOrderDFSforPOs(parents, &TedParents::collect);
	_pManCurrent->preOrderDFS(parents, &TedParents::collect);

	TedSet pathWith_X_Y;
	TedSet Utmp;
	TedSet::iterator px_it, py, pOk;
	TedNode* pNode;
	ETedNode* pAdder;
	Mark<TedNode>::newMark();

	// leave in currentContainer[varX] <= only varX nodes with no kid varY
	// place all varX nodes with kid varY into pathWith_X_Y
	TedSet pathWith_X =*currentContainer[varX].second;
	for(TedSet::iterator p = pathWith_X.begin(), pend = pathWith_X.end(); p!=pend; p++) {
		pNode = (*p);
		bool XconnectToY = false;
		bool register_stop = false;
		bool loop_stop = false;
		FOREACH_CHILD_OF_NODE(pNode,_iterkids,_iterend) {
			TedNode* pKid = _iterkids.Node<TedNode>();
			if(currentContainer.getLevel(*pKid->getVar()) == yInitialLevel) {
				XconnectToY = true;
				register_stop = (_iterkids.getRegister()> 0);
				pKid->visited.setWaterMark();
			}
			//added to detect loops created by retimed variables
			FOREACH_CHILD_OF_NODE(pKid,_iter,_iterend) {
				TedNode* pKid2 = _iter.Node<TedNode>();
				if(currentContainer.getLevel(*pKid2->getVar()) == xInitialLevel) {
					loop_stop = true;
					break;
				}
			}
		}
		if(XconnectToY) {
			if(register_stop || loop_stop) {
				for(TedSet::iterator pi = pathWith_X_Y.begin(), pi_end = pathWith_X_Y.end(); pi !=pi_end; pi++) {
					currentContainer[varX].second->insert(*pi);
				}
				pathWith_X_Y.clear();
				cerr << '\b' << "Info: Cannot reorder variables across edges with registers. " << endl;
				if(loop_stop)
					cerr     << "      Retimed variable has both a parent and a child of the same variable" << endl;
				return;
			} else {
				pathWith_X_Y.insert(pNode);
				currentContainer[varX].second->erase(pNode);
			}
		} else {
			pNode->visited.setMark();
		}
	}
	pathWith_X.clear();

	if(show_progress) {
		cout << '\b' << progress[1];
		std::flush(std::cout);
	}

	// leave in getContainer()[varY] <= only varY nodes that have parents other than varX
	TedSet pathWith_Y =*currentContainer[varY].second;
	for(TedSet::iterator p = pathWith_Y.begin(), pend = pathWith_Y.end(); p!=pend; p++) {
		pNode = (*p);
		if(pNode->visited.isWaterMarked()) {
			bool all_parents_are_varX = true;
			for(MyParent::iterator it = parents[pNode]->begin(), end = parents[pNode]->end(); it != end; it++) {
				TedNode* topParent = it->first;
				if(currentContainer.getLevel(*topParent->getVar())!=xInitialLevel) {
					all_parents_are_varX = false;
					break;
				}
			}
			if(_pManCurrent->isPO(pNode)) {
				all_parents_are_varX = false;
			}
			if(all_parents_are_varX) {
				currentContainer[varY].second->erase(pNode);
			} else {
				pNode->visited.setMark();
			}
		} else {
			pNode->visited.setMark();
		}
	}
	pathWith_Y.clear();

	// change the index of varX and varY
	// starts with varX on top of varY
	size_t yFinalLevel = xInitialLevel;
	size_t xFinalLevel = yInitialLevel;
	currentContainer[varY].first = currentContainer.getMaxLevel()+1-yFinalLevel;
	currentContainer[varX].first = currentContainer.getMaxLevel()+1-xFinalLevel;
	// ends with varX below of varY

#if _DEBUG
	//verify that all nodes remaining in the container at varX and varY are marked
	assert(currentContainer.check_all_nodes_are_marked_at_var(varY));
	assert(currentContainer.check_all_nodes_are_marked_at_var(varX));
#endif

	if(show_progress) {
		cout << '\b' << progress[2];
		std::flush(std::cout);
	}

#ifdef _DEBUG
	int path_size_debug = pathWith_X_Y.size();
	int iteration_debug = 0;
#endif
	// pathWith_X_Y
	for(TedSet::iterator px_it = pathWith_X_Y.begin(), px_end = pathWith_X_Y.end(); px_it != px_end; px_it++) {
#ifdef _DEBUG
		iteration_debug++;
#endif
		TedNode* px = (*px_it);
		ATedNode* pPath = NULL;
		TedKids oldKids_X;
		ETedNode* edge0_X = NULL;
		wedge weight0_X = 0;
		TedRegister edge0_R = 0;
		ETedNode* edge_X = NULL;
		TedKids::iterator xt_kid = px->getKids()->begin(), xt_end = px->getKids()->end();
		for(;xt_kid != xt_end; xt_kid++) {
			TedNode* pKid_x = xt_kid.Node<TedNode>();
			int weight_x = xt_kid.getWeight();
			int index_x = xt_kid.getIndex();
			TedRegister regKid_x = xt_kid.getRegister();
			const TedVar* pKid_x_var = pKid_x->getVar();
			const TedVar& pKid_var =*pKid_x_var;
			if(varY != pKid_var) {
				// collect all kids of px different than varY
				if(0==index_x) {
					if(pKid_x!=TedNode::getOne()) {
						TedKids newkids = pKid_x->getKids()->duplicate();
						newkids.pushGCD(weight_x);
						edge0_X = (ETedNode*)currentContainer.getNode(*pKid_x->getVar(),newkids,pKid_x->getRegister());
					} else {
						edge0_X = (ETedNode*)pKid_x;
						weight0_X = weight_x;
					}
					edge0_R = regKid_x;
				} else {
					oldKids_X.insert(index_x, pKid_x, weight_x, regKid_x);
				}
			} else {
				// swap the order for kids of varX connecting to varY
				assert(regKid_x==TedRegister(0));
				pAdder = NULL;
				if(0==index_x) {
					TedKids newkids = pKid_x->getKids()->duplicate();
					newkids.pushGCD(weight_x);
					pAdder = (ETedNode*)currentContainer.getNode(*pKid_x->getVar(),newkids,pKid_x->getRegister());
				} else {
					TedKids::iterator yt_kid = pKid_x->getKids()->begin(), yt_end = pKid_x->getKids()->end();
					for(;yt_kid != yt_end; yt_kid++) {
						int weight_y = yt_kid.getWeight();
						TedNode* pKid_y = yt_kid.Node<TedNode>();
						int index_y = yt_kid.getIndex();
						TedRegister regKid_y = yt_kid.getRegister();
						int propagateW = weight_x*weight_y;
						TedKids newKidsX;
						TedKids newKidsY;
						newKidsX.insert(index_x,pKid_y,1,regKid_y);
						TedNode* pNewVarX = currentContainer.getNode(varX, newKidsX,varX.getRegister());
						assert(!pNewVarX->isConst());
						newKidsY.insert(index_y, pNewVarX, propagateW, regKid_x);
						TedNode* pNewVarY = currentContainer.getNode(varY, newKidsY, varY.getRegister());
						newKidsX.clear();
						newKidsY.clear();
						// we have asserted that there can not be a retimed edge in the set pathWith_X_Y
						pAdder = (ETedNode*)((ETedNode*)pNewVarY)->add_with_content(pAdder,TedRegister(0));
					}
				}
				assert(pAdder);
				// We have just created this new connection(after reordering)so it is safe
				// to assume that there are no other elements pointing to these nodes
				// PS: if there are, their gcd has already been extracted and the code below won't
				// modify it a bit.
				//
				// At this moment we have generated pAdder, now we need to ensure that the
				// weight on each edge of node at the top has the gcd of the child node.
				//
				// gdc_map has been inserted because a child node might be referenced by different
				// the same parent through multiple edges, therefore the GCD previously extracted
				// must be saved
				map<TedNode*,int> gcd_map;
				FOREACH_KID_OF_NODE(pAdder) {
					TedNode* kidnode = (*_iterkids).Node();
					if(kidnode!= TedNode::getOne()) {
						int gcdKid = 1;
						if(gcd_map.find(kidnode) ==gcd_map.end()) {
							gcdKid = kidnode->getKids()->extractGCD();
							gcd_map.insert(pair<TedNode*,int>(kidnode,gcdKid));
						} else {
							gcdKid = gcd_map[kidnode];
						}
						gcdKid*= (*_iterkids).getWeight();
						(*_iterkids).setWeight(gcdKid);
					}
				}
				gcd_map.clear();
				// we have asserted that there can not be a retimed edge in the set pathWith_X_Y
				pPath = pAdder->add_with_content(pPath,TedRegister(0));
			}
		}
		if(oldKids_X.size()>0) {
			//
			// oldKids and pPath have different set of edges, therefore edge_X will not be
			// modified
			//
			edge_X = (ETedNode*)currentContainer.getNode(varX, oldKids_X, px->getRegister());
			//edge_X = (ETedNode*)pnedge_X->clone();

		}
		// pPath node should have the new connection Y_X; as we have been iterating
		// through nodes from pathWith_X_Y, which had a connection X_Y.
		// Therefore node pPath must always exist.
		assert(pPath);
		if(edge0_X==TedNode::getOne()) {
#if 0
			ETedNode* dummyNode = (ETedNode*)(((ETedNode*)(pPath))->clone());
			dummyNode->setKids(TedKids::create_kids());
			dummyNode->setKid(0,edge0_X,weight0_X,0);
			pPath = ((ETedNode*)pPath)->add_with_content(dummyNode, edge0_R);
#else
			pPath = ((ETedNode*)pPath)->add_with_content(weight0_X);
#endif
		} else {
			// edge0_X was connected through a retimed edge edge0_R
			// therefore we need to stitch the edge0_R
			pPath = ((ETedNode*)pPath)->add_with_content(edge0_X, edge0_R);
			if(((ETedNode*)pPath)->isErasable(edge0_X)) {
				currentContainer.eraseNode(edge0_X);
				delete edge0_X;
				edge0_X = NULL;
			}
		}
		// edge_X is a constructed node, with the remaining children not in the pathWith_X_X set
		// therefore the node edge_X is not retimed afterwards
		pPath = ((ETedNode*)pPath)->add_with_content(edge_X,TedRegister(0));
		bool already_existed = currentContainer.hasNode(pPath);
		ETedNode* pPathContainer = (ETedNode*)currentContainer.getNode(*pPath->getVar(),*pPath->getKids(),pPath->getRegister());
		if(pPath!=pPathContainer) {
			delete pPath;
			pPath = NULL;
		}
		TedKids pPathKids = pPathContainer->getKids()->duplicate();
		wedge gcd = pPathKids.extractGCD();
		ETedNode* pPathTotal = (ETedNode*)pPathContainer;
		bool in_container = true;
		if(gcd!=1) {
			//
			// regardless of the final node
			// the parent edges of the current node being swapped have to be updated
			_pManCurrent->forwardWeightUp(px, gcd, parents);
			//
			if(!already_existed) {
				currentContainer[*pPathContainer->getVar()].second->erase(pPathContainer);
			}
			ETedNode* ptmp = new ETedNode(*pPathContainer->getVar(),pPathKids,pPathContainer->getRegister());
			bool existed = currentContainer.hasNode(ptmp);
			if(existed) {
				delete ptmp;
				ptmp = NULL;
				pPathTotal = (ETedNode*)currentContainer.getNode(*pPathContainer->getVar(),pPathKids,pPathContainer->getRegister());
			} else {
				pPathTotal = ptmp;
				in_container = false;
			}
			if(!already_existed) {
				delete pPathContainer;
				pPathContainer = NULL;
			}
			already_existed = existed;
		}
		assert(pPathTotal);
		FOREACH_KID_OF_NODE(pPathTotal) {
			TedNode* kidnode = _iterkids.Node<TedNode>();
			if(currentContainer.isSameHight(*kidnode->getVar(), varX)) {
				kidnode->visited.setMark();
			}
		}
		if(pathWith_X_Y.find(pPathTotal)!=pathWith_X_Y.end()) {
			assert(in_container);
			assert(pPathTotal->visited.isMarked());
			//px should be redirected to pPathTotal
			//traverse the parent nodes and redirect all px pointers to pPathTotal
			//
			//NOTE: could this generate more merging?
			_pManCurrent->updateAllParents(px,pPathTotal,parents);
			delete px;
		} else {
#if 0
			if(in_container) {
				currentContainer[*pPathTotal->getVar()].second->erase(pPathTotal);
			}
			px->setKids(pPathTotal->getKids()->clone());
			px->setVar(*pPathTotal->getVar());
			px->setRegister(pPathTotal->getRegister());
			TedNode* tmp = currentContainer.getNode((ETedNode*)px);
			if (tmp!=px) {
				//delete px;
				px->visited.cleanMark();
				assert(tmp->visited.isMarked());
			} else {
				px->visited.setMark();
			}
			// All Paths finish, reattach parents of px to pPathTotal
			delete pPathTotal;
			pPathTotal = NULL;
#elif 0
			assert(currentContainer.hasNode(pPathTotal));
#elif 1
			// update parents if it already existed
			bool removed = false;
			if(in_container) {
				currentContainer[*pPathTotal->getVar()].second->erase(pPathTotal);
				removed = true;
			}
			assert(!currentContainer.hasNode(pPathTotal));
			px->setKids(pPathTotal->getKids()->clone());
			px->setVar(*pPathTotal->getVar());
			px->setRegister(pPathTotal->getRegister());
			TedNode* new_px = currentContainer.getNode((ETedNode*)px);
			assert(new_px==px);
			px->visited.setMark();
			delete pPathTotal;
			pPathTotal = NULL;
			//if (removed) {
			//	_pManCurrent->updateAllParents(pPathTotal,px,parents);
			//	assert(px->visited.isMarked());	
			//}
#endif
		}
	}
	pathWith_X_Y.clear();

	if(show_progress) {
		cout << '\b' << progress[3];
		std::flush(std::cout);
	}

	// cleaning unused nodes with VarY
	pathWith_Y =*currentContainer[varY].second;
	for(TedSet::iterator it = pathWith_Y.begin(), end = pathWith_Y.end();	it!=end; it++) {
		if(!(*it)->visited.isMarked()) {
			currentContainer[varY].second->erase(*it);
		} else {
			(*it)->visited.cleanMark();
		}
	}
	pathWith_Y.clear();

	// cleaning unused nodes with VarX
	pathWith_X =*currentContainer[varX].second;
	for(TedSet::iterator it = pathWith_X.begin(), end = pathWith_X.end();	it!=end; it++) {
		if(!(*it)->visited.isMarked()) {
			currentContainer[varX].second->erase(*it);
		} else {
			(*it)->visited.cleanMark();
		}
	}
	pathWith_X.clear();
#ifdef _DEBUG
	assert(currentContainer.check_children_are_included());
	assert(_pManCurrent->check_for_dangling_nodes());
#if 0
	//
	// we could speed up the purge time of un-marked nodes
	// if we maintain the TedParents table up to date
	//
	TedParents parents_check;
	_pManCurrent->preOrderDFSforPOs(parents_check, &TedParents::collect);
	if (parents!=parents_check) {
		cout << " -------------------------------- " << endl;
		parents.print_parents();
		cout << " -------------------------------- " << endl;
		parents_check.print_parents();
		cout << " -------------------------------- " << endl;
	}
#endif
#endif
	if(show_progress) {
		cout << '\b';
	}
}


void TedOrderProper::bottom(const TedVar& var) {
	unsigned int init_level = _pManCurrent->getContainer().getLevel(var);
	unsigned int location = 1;
	unsigned int distance = init_level-location;
	for(unsigned int jump = 1; jump <= distance; jump++) {
		unsigned int level = _pManCurrent->getContainer().getLevel(var)-1;
		const TedVar& varDown = _pManCurrent->getContainer().getVarAtLevel(level);
		reor(var, varDown);
	}
}

void TedOrderProper::updateBestManager(TedCompareCost* cmpFunc, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost) {
	assert(*pMan==*pManNext);
	Cost* pnewCost = cmpFunc->compute_cost(**pMan);
	recordHistory(*pMan,pnewCost,cmpFunc);
	cmpFunc->restart();
	Cost& newCost =*pnewCost;
	if(cmpFunc->compare(newCost,bestCost)) {
		bestCost = newCost;
		assert(*pMan!=*pManBest);
		delete*pManBest;
		*pManBest = (*pMan)->duplicate();
		COST_CHECK(cmpFunc,pMan,newCost,pManBest,bestCost);
	}
	delete pnewCost;
}

void TedOrderProper::updateBestManagerAnnealing(TedCompareCost* cmpFunc, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost, RNG& rng, double temperature,Cost& start_cost) {
	assert(*pManNext==*pMan);
	Cost* pnewCost = cmpFunc->compute_cost(**pMan);
	recordHistory(*pMan,pnewCost,cmpFunc);
	cmpFunc->restart();
	Cost& newCost =*pnewCost;
	bool accept_cost = false;
	if(cmpFunc->compare(newCost,bestCost)) {
		accept_cost = true;
	} else if ( newCost !=  bestCost) {
#if 0
		double ratio = temperature*(1+cmpFunc->get_cost(newCost) - cmpFunc->get_cost(bestCost)) / (1 + cmpFunc->get_cost(start_cost) - cmpFunc->get_cost(bestCost));
		double threshold = exp(-cmpFunc->get_cost(bestCost)/ratio);
		double random = rng.randomProb();
		accept_cost = (random < threshold);
#else
		double threshold = exp(-cmpFunc->get_cost(bestCost)/temperature);
		//double ratio = (1+cmpFunc->get_cost(newCost) - cmpFunc->get_cost(*best_history)) / (1 + cmpFunc->get_cost(start_cost) - cmpFunc->get_cost(*best_history));
		//double threshold = exp(-1*ratio*cmpFunc->get_cost(*best_history)/(temperature));
		//		cout << endl << "ratio=" << ratio << " history=" << cmpFunc->get_cost(*best_history) << " best=" << cmpFunc->get_cost(bestCost) << " new=" << cmpFunc->get_cost(newCost) << " temp="<<temperature<<"   threshold="<<threshold;
		double random = rng.randomProb();
		accept_cost = (random < threshold);
#endif
	}
	if(accept_cost && (newCost!=bestCost)) {
		bestCost = newCost;
		assert(*pMan!=*pManBest);
		delete*pManBest;
		*pManBest = (*pMan)->duplicate();
		COST_CHECK(cmpFunc,pMan,newCost,pManBest,bestCost);
	}
	delete pnewCost;
}

TedMan* TedOrderProper::duplicateBestManager(TedMan* pMan) {
	return pMan->duplicate();
}

void TedOrderProper::deleteBestManager(TedMan* pMan) {
	assert(pMan);
	delete pMan;
}

}
