/*
 * =====================================================================================
 *
 *       Filename:  TedRetiming.cc
 *    Description:
 *        Created:  05/18/2010 11:43:33 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "TedRetiming.h"
#include "TedNode.h"
#include "ETedNode.h"
#include "TedMan.h"
#include "TedParents.h"
namespace ted {


	TedRetiming::TedRetiming(TedMan* manager): _currentManager(manager) {
		assert(_currentManager);
		getParents();
		Mark<TedNode>::newMark();
	}

	TedRetiming::~TedRetiming(void) {
		_currentManager = NULL;
		clearParents();
	}

	void TedRetiming::getParents(void) {
		assert(_parents.empty());
		_currentManager->preOrderDFSforPOs(_parents, &TedParents::collect);
	}

	void TedRetiming::clearParents(void) {
		_parents.clear();
	}

	void TedRetiming::reorder(void) {
		TedOrderProper order(_currentManager);
		order.fixorderRetimedVars();
		clearParents();
	}

	bool TedRetiming::upMinCR(TedVar& var, TedRegister uptoR) {
		if(_parents.empty()) {
			getParents();
			Mark<TedNode>::newMark();
		}
		const TedVar* baseVar = var.getBase();
		TedContainer& container = _currentManager->getContainer();
		TedSet* nodes = container[var].second;
		TedSet copies =*nodes;
		map<TedNode*,TedNode*> old2new;
		bool retime_operation_executed = false;
		for(TedSet::iterator sp = copies.begin(); sp != copies.end(); sp++) {
			TedNode* node =*sp;
			TedRegister nodeReg = (var.isConst() || var.isVarConst()) ? uptoR : node->getRegister();
			if(node->visited.isMarked())
				continue;
			node->visited.setMark();
			if((var.isVar()&& nodeReg==0))
				continue;
			TedRegister kidsReg = node->getKids()->minRegister();
			if(kidsReg==0) {
				// minReg returns INT_MAX if there is only one edge connected to TedNode::getOne()
				//        returns 0, if there is an edge other than TedNode::getOne() with no register
				continue;
			}
			if(kidsReg>uptoR) {
				kidsReg=uptoR;
			}
			retime_operation_executed = true;

			TedRegister minReg = (nodeReg > kidsReg) ? kidsReg : nodeReg;
			if(minReg==INT_MAX) {
				continue;
			}
			nodes->erase(node);
			assert(minReg!=0);
			kidsReg -= minReg;
			nodeReg -= minReg;

			if (!var.isConst() && !var.isVarConst()) {
				assert(baseVar);
				const TedVar* newVar = (nodeReg!=0) ? TedVarMan::instance().createRetimedVar(*baseVar, nodeReg): baseVar;
				container.registerRetimedVarAt(*newVar,*baseVar);
				node->setVar(*newVar);
				node->setRegister(nodeReg);
			}
			node->getKids()->retimeUp(minReg);
			TedNode* newNode = container.getNode((ETedNode*)node);
			//
			// update parent registers
			//        PrimaryOutputs register count
			//
			if(_currentManager->isPO(node)) {
				TedNodeRoot* root = _currentManager->getPO(node);
				TedRegister rootReg = root->getRegister()+ minReg;
				root->setRegister(rootReg);
				root->Node(newNode);
			} else {
				for(MyParent::iterator it = _parents[node]->begin(); it != _parents[node]->end(); it++) {
					TedNode* topParent = it->first;
					if(old2new.find(topParent)!=old2new.end()) {
						topParent = old2new[topParent];
					}
					MyIndex* topIndices = it->second;
					for(int i=0; i != topIndices->size(); i++) {
						int index = topIndices->at(i);
						TedRegister kidReg = topParent->getKids()->getRegister(index);
						kidReg+=minReg;
						int weight = topParent->getKids()->getWeight(index);
						topParent->getKids()->replace(index, newNode, weight, kidReg);
					}
				}
			}
			if(newNode!=node) {
				old2new[node] = newNode;
				//delete node;
			}
		}
		for(map<TedNode*,TedNode*>::iterator it = old2new.begin(); it != old2new.end(); it++) {
			TedNode* old = it->first;
			delete old;
		}
		old2new.clear();
		return retime_operation_executed;
	}


	bool TedRetiming::up(TedRegister uptoR) {
		TedContainerOrder tnodesOrdered = _currentManager->getContainer().order();
		// from bottom register to top register
		bool retimed = false;
		for(TedContainerOrder::reverse_iterator it = tnodesOrdered.rbegin(); it!= tnodesOrdered.rend(); it++) {
			TedVar* pvar  = _currentManager->checkVariables(it->second.first->getName());
			retimed |= upMinCR(*pvar,uptoR);
		}
		if(retimed) {
			reorder();
		}
		return retimed;
	}


	bool TedRetiming::upAllRelatedTo(TedVar& var, TedRegister uptoR) {
		TedContainerOrder tnodesOrdered = _currentManager->getContainer().order();
		// from bottom register to top register
		bool retimed = false;
		const TedVar* base = var.getBase();
		if(!base) {
			//if var is not a retimed variable
			//it could be a base variable
			base = &var;
		}
		BaseVarsRetimed retimed_map = _currentManager->getContainer().getExistingRetimedVarsPerBaseClass();
		if(retimed_map.find(base)!= retimed_map.end()) {
			// if it is a base variable
			// retime up all variables related to var
			for(TedContainerOrder::reverse_iterator it = tnodesOrdered.rbegin(); it!= tnodesOrdered.rend(); it++) {
				TedVar* pvar  = _currentManager->checkVariables(it->second.first->getName());
				if(retimed_map[base].find(*pvar)!= retimed_map[base].end()) {
					retimed |= upMinCR(*pvar,uptoR);
				}
			}
		} else {
			retimed |= upMinCR(var,uptoR);
		}
		if(retimed) {
			reorder();
		}
		return retimed;
	}


	bool TedRetiming::down(TedRegister uptoR) {
		TedContainerOrder tnodesOrdered = _currentManager->getContainer().order();
		// from bottom register to top register
		bool retimed = false;
		for(TedContainerOrder::iterator it = tnodesOrdered.begin(); it!= tnodesOrdered.end(); it++) {
			TedVar* pvar  = _currentManager->checkVariables(it->second.first->getName());
			retimed |= downMinCR(*pvar,uptoR);
		}
		if(retimed) {
			reorder();
		}
		return retimed;
	}

	bool TedRetiming::downAllRelatedTo(TedVar& var, TedRegister uptoR) {
		TedContainerOrder tnodesOrdered = _currentManager->getContainer().order();
		// from bottom register to top register
		bool retimed = false;
		const TedVar* base = var.getBase();
		if(!base) {
			//if var is not a retimed variable
			//it could be a base variable
			base = &var;
		}
		BaseVarsRetimed retimed_map = _currentManager->getContainer().getExistingRetimedVarsPerBaseClass();
		if(retimed_map.find(base)!= retimed_map.end()) {
			// if it is a base variable
			// retime up all variables related to var
			for(TedContainerOrder::iterator it = tnodesOrdered.begin(); it!= tnodesOrdered.end(); it++) {
				TedVar* pvar  = _currentManager->checkVariables(it->second.first->getName());
				if(retimed_map[base].find(*pvar)!= retimed_map[base].end()) {
					retimed |= downMinCR(*pvar,uptoR);
				}
			}
		} else {
			retimed |= downMinCR(var,uptoR);
		}
		if(retimed) {
			reorder();
		}
		return retimed;
	}


	bool TedRetiming::has_retimed_child_same_var(TedNode* node) {
		size_t node_level = _currentManager->getContainer().getLevel(*node->getVar());
		for(Kids::iterator _iterkids = node->getKids()->begin(); _iterkids != node->getKids()->end(); _iterkids++) {
			TedNode* child = _iterkids->second.Node();
			size_t child_level = _currentManager->getContainer().getLevel(*child->getVar());
			if(child_level==node_level) {
				return true;
			}
		}
		return false;
	}

	bool TedRetiming::has_retimed_parent_same_var(TedNode* node) {
		size_t node_level = _currentManager->getContainer().getLevel(*node->getVar());
		for(MyParent::iterator it = _parents[node]->begin(); it != _parents[node]->end(); it++) {
			TedNode* parent = it->first;
			size_t parent_level = _currentManager->getContainer().getLevel(*parent->getVar());
			if(parent_level==node_level) {
				return true;
			}
		}
		return false;
	}

	list<TedNode*> TedRetiming::get_retimed_child_same_var(TedNode* node) {
		size_t node_level = _currentManager->getContainer().getLevel(*node->getVar());
		list<TedNode*> same_child;
		for(Kids::iterator _iterkids = node->getKids()->begin(); _iterkids != node->getKids()->end(); _iterkids++) {
			TedNode* child = _iterkids->second.Node();
			size_t child_level = _currentManager->getContainer().getLevel(*child->getVar());
			if(child_level==node_level) {
				same_child.push_back(child);
			}
		}
		return same_child;
	}

	list<TedNode*> TedRetiming::get_retimed_parent_same_var(TedNode* node) {
		size_t node_level = _currentManager->getContainer().getLevel(*node->getVar());
		list<TedNode*> same_parent;
		if(_parents.find(node)!=_parents.end()) {
			for(MyParent::iterator it = _parents[node]->begin(); it != _parents[node]->end(); it++) {
				TedNode* parent = it->first;
				size_t parent_level = _currentManager->getContainer().getLevel(*parent->getVar());
				if(parent_level==node_level) {
					same_parent.push_back(parent);
				}
			}
		}
		return same_parent;
	}

	// In theory a node might be as
	//
	//   N----parent_1----- ^2 2R -------\       /------------ R ------grand_child_1-----X
	//                \---- ^3 R ---------child_1------ ^2 2R ---------grand_child_2-----Y
	//   M----parent_2-------- R ----------/ /   \------ ^3 4R --------/
	//   P----parent_3-------- ^4 3R -------/
	//
	//                                       |-- {a*(a*X)|R+a^2*(a*Y)|2R+a^3*(a*Y)|4R} --|
	//                                                replaced by A
	//
	//   where parent_x, child_1 and grand_child_x are of the same type of variable "a"
	//
	//   N*[a^2*(A|2R)+a^3*(A|R)] + M*a*(A|R)| P*a^4*(A|3R)
	//
	list<TedNode*> TedRetiming::order_retimed_same_var_set(const TedVar& var) {
		list<TedNode*> retimed_order;
		Mark<TedNode>::newMark();
		const TedContainer& container = _currentManager->getContainer();
		//size_t var_level = container.getLevel(var);
		const TedSet* nodes = container.at(var).second;
		map<TedNode*,int> track_order;
		int highest_order = 0;
		const int regular_order = 0;
		for(TedSet::const_iterator sp = nodes->begin(); sp != nodes->end(); sp++) {
			TedNode* node =*sp;
			track_order.insert(pair<TedNode*,int>(node,regular_order));
		}
		for(TedSet::const_iterator sp = nodes->begin(); sp != nodes->end(); sp++) {
			TedNode* node =*sp;
			if(node->visited.isMarked())
				continue;
			//node->visited.setMark();
			TedNode* parent = NULL;
			list<TedNode*> root_parents;
			list<TedNode*> parents = get_retimed_parent_same_var(node);
			//retrieve the root parents
			//do not mark any node yet.
			while(!parents.empty()) {
				parent = parents.front();
				parents.pop_front();
				//
				//if a parent is marked, then the node should have been
				//traversed. therefore we should have stop at node->visited.isMarked()
				//
				assert(!parent->visited.isMarked());
				list<TedNode*> new_parents = get_retimed_parent_same_var(parent);
				if(new_parents.empty()) {
					if(find(root_parents.begin(),root_parents.end(),parent) ==root_parents.end()) {
						assert(track_order[parent]==regular_order);
						root_parents.push_back(parent);
						track_order[parent] = 1;
					}
					// else
					// it is already included in root_parents
				} else {
					// Warning:
					// this might include duplicated items in parents
					//   a given parent might have been traversed and removed from the parents list
					//   and then a test==parent is re-inserted.
					for(list<TedNode*>::iterator it = new_parents.begin(), end = new_parents.end(); it != end; it++) {
						TedNode* test = (*it);
						if(find(parents.begin(),parents.end(),test) ==parents.end()) {
							parents.push_back(test);
						}
					}
				}
			}
			if(root_parents.empty()) {
				assert(track_order[node]==regular_order);
				if(has_retimed_child_same_var(node)) {
					// node is a root parent
					// it has no retimed parent with the same variable
					// it has retimed child with the same variable
					root_parents.push_back(node);
					track_order[node] = 1;
				} else {
					// node is not a root parent
					// it has no retimed parent with the same variable
					// it has no retimed child with the same variable
					node->visited.setMark();
				}
			}
			if(!root_parents.empty()) {
				// node has either retimed parents with the same variable or it is a root parent itself
				while(!root_parents.empty()) {
					parent = root_parents.front();
					root_parents.pop_front();
					//if(parent->visited.isMarked())
					//	continue;
					parent->visited.setMark();
					int next_level = track_order[parent]+1;
					if(has_retimed_child_same_var(parent)) {
						list<TedNode*> children = get_retimed_child_same_var(parent);
						while(!children.empty()) {
							TedNode* child_candidate = children.front();
							children.pop_front();
							if(track_order[child_candidate] < next_level) {
								track_order[child_candidate] = next_level;
							}
							if(find(root_parents.begin(), root_parents.end(), child_candidate) == root_parents.end()) {
								root_parents.push_back(child_candidate);
							}
						}
					} else {
						next_level--;
						if(next_level>highest_order) {
							highest_order = next_level;
						}
					}
				}
			}
		}
		multimap<int,TedNode*> final_order;
		for(map<TedNode*,int>::iterator it = track_order.begin(), end = track_order.end(); it != end; it++) {
			final_order.insert(pair<int,TedNode*>(it->second,it->first));
		}
		for(multimap<int,TedNode*>::iterator it = final_order.begin(), end = final_order.end(); it != end; it++) {
			retimed_order.push_back(it->second);
		}
		return retimed_order;
	}


	bool TedRetiming::downMaxR(TedVar& var) {
		if(_parents.empty()) {
			getParents();
			Mark<TedNode>::newMark();
		}
		const TedVar* baseVar = var.getBase();
		if(baseVar==NULL)
			baseVar = &var;
		TedContainer& container = _currentManager->getContainer();
		TedSet copies =*container[var].second;
		Mark<TedNode>::newMark();

		//maps the new variable after retiming to the old variable before retiming
#if 0
		map<TedNode*,map<TedRegister,TedNode*> > old2new;
#endif
		list<TedNode*> oldnodes;
		for(TedSet::iterator sp = copies.begin(); sp != copies.end(); sp++) {
			TedNode* node =*sp;
			if(node->visited.isMarked())
				continue;
			node->visited.setMark();
			//compute the highest children
			bool left_in_container = false;
			// for each parent node
			for(MyParent::iterator it = _parents[node]->begin(); it != _parents[node]->end(); it++) {
				TedNode* topParent = it->first;
				MyIndex* topIndices = it->second;
				// for each parent-edge in a parent node
				for(int i=0; i != topIndices->size(); i++) {
					int index = topIndices->at(i);
					TedRegister kidReg = topParent->getKids()->getRegister(index);
					int kidWeight = topParent->getKids()->getWeight(index);
					if(kidReg==0) {
						left_in_container = true;
						continue;
					}
#if 0
					map<TedRegister,TedNode*> regNew;
					if(old2new.find(topParent)!=old2new.end()) {
						regNew = old2new[topParent];
						if(regNew.find(kidReg)!=regNew.end()) {
							topParent = regNew[kidReg];
						}
					}
#endif
					//retime down
					TedKids newKids = node->getKids()->duplicate();
					newKids.retimeDown(kidReg);
					TedRegister nodeReg = kidReg + node->getRegister();
					const TedVar* newVar = TedVarMan::instance().createRetimedVar(*baseVar, nodeReg);

					container.registerRetimedVarAt(*newVar,*baseVar);
					TedNode* newNode = container.getNode(*newVar, newKids, nodeReg);
					//update the _parents[node] children's register count
					topParent->replaceKid(index, newNode, kidWeight);
#if 0
					if(newNode!=node) {
						if(regNew.find(kidReg) ==regNew.end()) {
							regNew[kidReg] = newNode;
							old2new[node] = regNew;
						}
					}
#endif
				}
			}
			if(!left_in_container) {
				oldnodes.push_back(node);
				//	nodes->erase(node);
				//	delete node;
			}
		}
#if 0
		for(map<TedNode*,map<TedRegister,TedNode*> >::iterator it = old2new.begin(); it != old2new.end(); it++) {
			TedNode* old = it->first;
			delete old;
		}
		old2new.clear();
#else
		for(list<TedNode*>::iterator it = oldnodes.begin(); it != oldnodes.end(); it++) {
			TedNode* old = *it;
			delete old;
		}
#endif

		clearParents();
		return true;
	}


	bool TedRetiming::downMinCR(TedVar& var, TedRegister uptoR) {
		if(_parents.empty()) {
			getParents();
			Mark<TedNode>::newMark();
		}
		const TedVar* baseVar = var.getBase();
		if(baseVar==NULL)
			baseVar = &var;
		TedContainer& container = _currentManager->getContainer();
		//
		//pre-process the nodes so that:
		//   node_a@X ---R---> node_a@Z ---2R---> node_a@Y
		//are traversed in the same order a@X,a@Z,a@Y
		//
		list<TedNode*> preorder = order_retimed_same_var_set(var);

#if _RETIMING_DOWN_ORDER_DEBUG
		for(list<TedNode*>::iterator it = preorder.begin(); it != preorder.end(); it++) {
			cout << "ptr=" <<(*it)<< " node=" <<(*it)->getVar()->getName()<< endl;
		}
#endif

		bool retime_operation_executed = false;
		Mark<TedNode>::newMark();

		//maps the new variable after retiming to the old variable before retiming
		//

		TedSet* nodes = container[var].second;

		//map<TedNode*,TedNode*> old2new;
		for(list<TedNode*>::iterator it = preorder.begin(); it != preorder.end(); it++) {
			TedNode* node =*it;
			if(node->visited.isMarked())
				continue;
			node->visited.setMark();
			TedRegister minReg = 0;
			bool start = true;

			if(_currentManager->isPO(node)) {
				TedNodeRoot* root = _currentManager->getPO(node);
				minReg = root->getRegister();
			} else {
				assert(_parents.find(node)!=_parents.end());
				for(MyParent::iterator jt = _parents[node]->begin(); jt != _parents[node]->end(); jt++) {
					TedNode* topParent = jt->first;
					MyIndex* topIndices = jt->second;
					//		if(old2new.find(topParent)!=old2new.end()) {
					//			topParent = old2new[topParent];
					//		}
					for(int i=0; i != topIndices->size(); i++) {
						int index = topIndices->at(i);
						TedRegister kidReg = topParent->getKids()->getRegister(index);
						if(start && i == 0) {
							start = false;
							minReg = kidReg;
							continue;
						}
						if(kidReg < minReg)
							minReg = kidReg;
					}
					if(minReg==0)
						break;
				}
			}
			if(minReg==0)
				continue;
			if(minReg > uptoR) {
				minReg = uptoR;
			}
			retime_operation_executed = true;
			//
			// remove node with variable node->getVar()from the container. so that
			// the node ptr can be reused in the container with a new variable newVar
			//
			// Because we are re-using the node ptr, then newNode should be equal to node
			// and the _parents container mantains its correct information
			//
			nodes->erase(node);
			TedRegister nodeReg = node->getRegister();
			if ( !var.isConst() && !var.isVarConst()) {
				nodeReg += minReg;
				const TedVar* newVar = TedVarMan::instance().createRetimedVar(*baseVar, nodeReg);
				container.registerRetimedVarAt(*newVar,*baseVar);
				node->setVar(*newVar);
				node->setRegister(nodeReg);
			}
			node->getKids()->retimeDown(minReg);
			TedNode* newNode = container.getNode((ETedNode*)node);
			assert(newNode==node);
			//
			// update _parents[node] children's register count
			//        PrimaryOutputs register count
			//
			if(_currentManager->isPO(node)) {
				TedNodeRoot* root = _currentManager->getPO(node);
				TedRegister rootReg = root->getRegister()- minReg;
				root->setRegister(rootReg);
			} else {
				for(MyParent::iterator it = _parents[node]->begin(); it != _parents[node]->end(); it++) {
					TedNode* topParent = it->first;
					MyIndex* topIndices = it->second;
					for(int i=0; i != topIndices->size(); i++) {
						int index = topIndices->at(i);
						TedRegister kidReg = topParent->getKids()->getRegister(index);
						//topParent->getKids()->setRegister(index, kidReg-minReg);
						int kidWeight = topParent->getKids()->getWeight(index);
						topParent->replaceKid(index, newNode, kidWeight,kidReg-minReg);
					}
				}
			}
#if 0
			if(newNode!=node)
				delete node;
#endif
		}
		//for(map<TedNode*,TedNode*>::iterator it = old2new.begin(); it != old2new.end(); it++) {
		//	TedNode* old = it->first;
		//	delete old;
		//}
		clearParents();
		//old2new.clear();
		return retime_operation_executed;
	}

}
