/*
 * =====================================================================================
 *
 *       Filename:  TedManEval.cc
 *    Description:
 *        Created:  05/03/11 07:48:11
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include "TedMan.h"
#include "ETedNode.h"

namespace ted {

	/**@brief evaluates a variables on a multiple output TED,
	  @detail Assumes there are no register on the edges.(All registers are backward retimed to the PIs)
	  */
	void TedMan::eval(const TedVar& varX, int value) {
		//reduce to a simple case
		TedOrderProper force_bottom(this);
		force_bottom.bottom(varX);

		//size_t xInitialLevel = _container.getLevel(varX);
		//assert(xInitialLevel==1);

		TedParents parents;
		preOrderDFSforPOs(parents, &TedParents::collect);

		TedSet pathWith_X_Y;
		TedSet Utmp;
		TedSet::iterator px_it, py, pOk;
		//TedNode* pNode;
		//ETedNode* pAdder;
		Mark<TedNode>::newMark();

		// leave in currentContainer[varX] <= only varX nodes with no kid varY
		// place all varX nodes with kid varY into pathWith_X_Y
		TedSet& pathWith_X =*_container[varX].second;
		for(TedSet::iterator px_it = pathWith_X.begin(), px_end = pathWith_X.end(); px_it != px_end; px_it++) {
			TedNode* px = (*px_it);
			//ATedNode* pPath = NULL;
			TedKids oldKids_X;
			TedKids::iterator xt_kid = px->getKids()->begin(), xt_end = px->getKids()->end();
			int constant = 0;
			for(;xt_kid != xt_end; xt_kid++) {
				TedNode* pKid_x = xt_kid.Node<TedNode>();
				assert(pKid_x==TedNode::getOne());
				int weight_x = xt_kid.getWeight();
				int index_x = xt_kid.getIndex();
				constant += ((int)pow((double)value,index_x))*weight_x;
			}
			if(constant!=1) {
				forwardWeightUp(px, constant, parents);
			}
			//losses canonicity
			replaceNodeBy(px,TedNode::getOne(),parents);
		}
		pathWith_X.clear();
		_container.removeLevel(varX);
		_vars.remove(varX.getName());
	}

	// see updateTopParents
	void TedMan::replaceNodeBy(TedNode* pNode, TedNode* newNode, TedParents& parents) {
		if(parents.find(pNode) == parents.end()) {
			throw(string("04040. Node has no parent, it is a PO"));
			return;
		}
		for(MyParent::iterator it = parents[pNode]->begin(), end = parents[pNode]->end(); it != end; it++) {
			TedNode* topParent = it->first;
			MyIndex* topIndices = it->second;
			for(int i=0; i != topIndices->size(); i++) {
				int index = topIndices->at(i);
				wedge weight = topParent->getKidWeight(index);
				TedRegister reg = topParent->getKids()->getRegister(index);
				topParent->getKids()->replace(index,newNode,weight,reg);
			}
		}
	}

#if 0
	void TedMan::forwardWeightUp(TedNode* pNode, wedge gcd, TedParents& parents) {
		bool found = false;
		//PO edges are not listed as parents but internal nodes can also be POs
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			TedNodeRoot& root = p->second;
			if(root.Node() == pNode) {
				found = true;
				root.setWeight(root.getWeight()*gcd);
			}
		}
		if(parents.find(pNode) == parents.end()) {
			//if pNode has no parents it should have haven at least one PO
			//otherwise we are dealing with a dangling node
			assert(found);
			return;
		}
		set<TedNode*> tmp_key;
		map<TedNode*,TedNode*> tobe_updated;
		MyParent* parents_of_pnode = parents[pNode];
		assert(parents_of_pnode);
#if 1
		typedef MyParent MyParentType;
		MyParentType fixed_parents =*parents_of_pnode;
#else
		typedef MyParentOrder MyParentType;
		MyParentType fixed_parents(&this->getContainer());
		fixed_parents =*parents_of_pnode;
#endif
		//NOTE:
		// the pass iterators of fixed_parents might be destroyed
		for(MyParentType::iterator it = fixed_parents.begin(), end = fixed_parents.end(); it != end; it++) {
			TedNode* topParent = it->first;
			MyIndex* topIndices = it->second;
			TedKids newKids = topParent->getKids()->duplicate();
			for(int i=0; i != topIndices->size(); i++) {
				int index = topIndices->at(i);
				wedge weight = newKids.getWeight(index);
				newKids.setWeight(index,weight*gcd);
			}
			wedge gcdUp = newKids.extractGCD();
			if(1 != gcdUp) {
				forwardWeightUp(topParent,gcdUp,parents);
			}
			TedNode* myParent = NULL;
			if(_container.hasNode(*topParent->getVar(),newKids,topParent->getRegister())) {
				TedNode* newParent = _container.getNode(*topParent->getVar(),newKids,topParent->getRegister());
				if (newParent == topParent) {
					// the top parent node 'X6' had only one edge which received a gcd of -1
					// this creates a new modified '-X6' but after extracting its gcd revert to 'X6'
					// In this case, the new parent and the top parent are the same node
					// nothing to update
					assert(gcdUp==-1);
					assert(topParent->getKids()->size() ==1);
				} else {
					myParent = newParent;
					// the modification made in topParent reflects another node already existing
					// so we have to move all connections to topParent into the newParent
					if (parents_of_pnode->find(newParent)!=parents_of_pnode->end()) {
						// we are about to update parent P1 with another parent P2
						// P2 might have not being traversed yet, so node which is also a parent
						myParent = new ETedNode(*topParent->getVar(),newKids,topParent->getRegister());
						// myParent is not in the container nor it is referred anywhere else
						// so it is a safe temporary holder.
						tmp_key.insert(myParent);
					}
					// topParent becomes a danlging node which will be recovered at the very end
					assert(!parents.hasParents(myParent));
					updateAllParents(topParent,myParent,parents);
					//tobe_updated.insert(pair<TedNode*,TedNode*>(topParent,myParent));
				}
			} else {
				TedKids::destroy_kids(topParent->_tKids);
				topParent->setKids(newKids);
			}
		}
		fixed_parents.clear();
		for (set<TedNode*>::iterator it = tmp_key.begin(), end = tmp_key.end(); it!=end; it++) {
			// myParent could have already being reattached to topParent in recursive call of forwardWeightUp/updateAllParents
			TedNode* myParent =*it;
			TedNode* topParent = _container.getNode(*myParent->getVar(),*myParent->getKids(),myParent->getRegister());
			assert(topParent!=myParent);
			if(parents.hasParents(myParent) || isPO(myParent)) {
				updateAllParents(myParent,topParent,parents);
			}
			delete myParent;
		}
		tmp_key.clear();
	}
#endif

	void TedMan::forwardWeightUp(TedNode* pNode, wedge gcd, TedParents& parents) {
		map<TedNode*,TedNode*> no_touch;
		TedSet tmp_key;
		forwardWeightUp_rec(pNode,gcd,parents,tmp_key,no_touch);
		for (set<TedNode*>::iterator it = tmp_key.begin(), end = tmp_key.end(); it!=end; it++) {
			// myParent could have already being reattached to topParent in recursive call of forwardWeightUp/updateAllParents
			TedNode* myParent =*it;
			TedNode* topParent = _container.getNode(*myParent->getVar(),*myParent->getKids(),myParent->getRegister());
			assert(topParent!=myParent);
			if(parents.hasParents(myParent) || isPO(myParent)) {
				updateAllParents(myParent,topParent,parents);
			}
			delete myParent;
		}
		tmp_key.clear();
		no_touch.clear();

	}

	void TedMan::forwardWeightUp_rec(TedNode* pNode, wedge gcd, TedParents& parents, TedSet& tmp_key, map<TedNode*,TedNode*> & no_touch) {
		bool found = false;
		//PO edges are not listed as parents but internal nodes can also be POs
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			TedNodeRoot& root = p->second;
			if(root.Node() == pNode) {
				found = true;
				root.setWeight(root.getWeight()*gcd);
			}
		}
		if(parents.find(pNode) == parents.end()) {
			// if pNode has no parents it should have haven at least one PO
			// otherwise we are dealing with a dangling node
			assert(found);
			return;
		}
		map<TedNode*,TedNode*> tobe_updated;
		MyParent* parents_of_pnode = parents[pNode];
		assert(parents_of_pnode);
#if 1
		typedef MyParent MyParentType;
		MyParentType fixed_parents =*parents_of_pnode;
#else
		typedef MyParentOrder MyParentType;
		MyParentType fixed_parents(&this->getContainer());
		fixed_parents =*parents_of_pnode;
#endif
		//NOTE:
		// the pass iterators of fixed_parents might be destroyed
		for(MyParentType::iterator it = fixed_parents.begin(), end = fixed_parents.end(); it != end; it++) {
			TedNode* topParent = it->first;
			if (parents.find(topParent) ==parents.end() && NULL==getPO(topParent)) {
				assert(no_touch.find(topParent)!=no_touch.end());
				topParent = no_touch[topParent];
			}
			MyIndex* topIndices = it->second;
			TedKids newKids = topParent->getKids()->duplicate();
			for(int i=0; i != topIndices->size(); i++) {
				int index = topIndices->at(i);
				wedge weight = newKids.getWeight(index);
				newKids.setWeight(index,weight*gcd);
			}
			wedge gcdUp = newKids.extractGCD();
			if(1 != gcdUp) {
				forwardWeightUp_rec(topParent,gcdUp,parents,tmp_key, no_touch);
			}
			TedNode* myParent = NULL;
			if(_container.hasNode(*topParent->getVar(),newKids,topParent->getRegister())) {
				TedNode* newParent = _container.getNode(*topParent->getVar(),newKids,topParent->getRegister());
				if (newParent == topParent) {
					// the top parent node 'X6' had only one edge which received a gcd of -1
					// this creates a new modified '-X6' but after extracting its gcd revert to 'X6'
					// In this case, the new parent and the top parent are the same node
					// nothing to update
					assert(gcdUp==-1);
					assert(topParent->getKids()->size() ==1);
				} else {
					myParent = newParent;
					// the modification made in topParent reflects another node already existing
					// so we have to move all connections to topParent into the newParent
					if (parents_of_pnode->find(newParent)!=parents_of_pnode->end() || no_touch.find(newParent)!=no_touch.end() ) {
						// we are about to update parent P1 with another parent P2
						// P2 might have not being traversed yet, so node which is also a parent
						myParent = new ETedNode(*topParent->getVar(),newKids,topParent->getRegister());
						// myParent is not in the container nor it is referred anywhere else
						// so it is a safe temporary holder.
						tmp_key.insert(myParent);
						if (no_touch.find(topParent) ==no_touch.end()) {
							no_touch.insert(pair<TedNode*,TedNode*>(topParent,myParent));
						}
					}
					// topParent becomes a danlging node which will be recovered at the very end
					assert(!parents.hasParents(myParent));
					updateAllParents(topParent,myParent,parents);
					//tobe_updated.insert(pair<TedNode*,TedNode*>(topParent,myParent));
				}
			} else {
				TedKids::destroy_kids(topParent->_tKids);
				topParent->setKids(newKids);
			}
		}
		fixed_parents.clear();
	}

	void TedMan::updateTopParents(TedNode* oldNode, TedNode* newNode, TedParents& parents) {
		assert(oldNode!=newNode);
		bool found = false;
		TedNodeRoot* root = getPO(oldNode);
		if (root) {
			found = true;
			root->Node(newNode);
		}
		if(parents.find(oldNode) == parents.end()) {
			assert(found);
			return;
		}
		// there might be duplicated nodes
		// NOTE: newNode might or might not be in the container
		parents.registerKey(newNode);
		MyParent* oldNodeParents = parents[oldNode];
		MyParent::iterator it = oldNodeParents->begin(), it_end = oldNodeParents->end();
		multimap<TedNode*,TedNode*> tobeupdated;
		for(; it != it_end; it++) {
			TedNode* topParent = it->first;
			MyIndex* topIndices = it->second;
			assert(topParent);
			TedKids newKids = topParent->getKids()->duplicate();
			for(int i=0; i != topIndices->size(); i++) {
				int index = topIndices->at(i);
				wedge weight = newKids.getWeight(index);
				TedRegister reg = topParent->getKids()->getRegister(index);
				newKids.replace(index,newNode,weight,reg);
				parents.registerParent(newNode,topParent,index);
			}
			wedge gcdUp = newKids.extractGCD();
			assert(gcdUp==1);
			if(_container.hasNode(*topParent->getVar(),newKids,topParent->getRegister())) {
				TedNode* newParent = _container.getNode(*topParent->getVar(),newKids,topParent->getRegister());
				tobeupdated.insert(pair<TedNode*,TedNode*>(topParent,newParent));
				//updateAllParents(topParent,newParent,parents);
			} else {
				TedKids::destroy_kids(topParent->_tKids);
				topParent->setKids(newKids);
			}
		}
		TedSet toberemoved;
		for (multimap<TedNode*,TedNode*>::iterator jt =tobeupdated.begin(), jt_end = tobeupdated.end(); jt!=jt_end;jt++) {
			TedNode* old1 = jt->first;
			TedNode* new1 = jt->second;
			updateTopParents(old1,new1,parents);
			updateBottomParents(old1,new1,parents);
			toberemoved.insert(old1);
			assert(parents.hasParents(new1) || isPO(new1));
		}
		for (TedSet::iterator kt = toberemoved.begin(), kt_end = toberemoved.end(); kt!=kt_end; kt++) {
			TedNode* old2 = (*kt);
			parents.unregisterKey(old2);
			assert(!parents.hasParents(old2));
		}
		//after the parents of oldNode have been updated
		//oldNode has no parent connected to it, so its
		//key MUST be removed
		//parents.unregisterKey(oldNode);
	}

	void TedMan::updateBottomParents(TedNode* oldNode, TedNode* newNode, TedParents& parents) {
		TedSet childVisited;
		FOREACH_KID_OF_NODE(oldNode) {
			TedNode* child = _iterkids.Node<TedNode>();
			if (child == TedNode::getOne())
				continue;
			if (childVisited.find(child)!=childVisited.end())
				continue;
			childVisited.insert(child);
			MyParent* mp = parents[child];
			MyParent::iterator jt = mp->find(oldNode);
			assert(jt!=mp->end());
			MyIndex* mi = jt->second;
			mp->erase(jt);
			mp->insert(pair<TedNode*,MyIndex*>(newNode,mi));
		}
		childVisited.clear();
	}

	void TedMan::updateAllParents(TedNode* oldNode, TedNode* newNode, TedParents& parents) {
		updateTopParents(oldNode,newNode,parents);
		updateBottomParents(oldNode,newNode,parents);
		//after the parents of oldNode have been updated
		//oldNode has no parent connected to it, so its
		//key MUST be removed
		parents.unregisterKey(oldNode);
		assert(!parents.hasParents(oldNode));
		assert(parents.hasParents(newNode) || isPO(newNode));
	}

}
