/*
 * =====================================================================================
 *
 *       Filename:  TedContainer.cc
 *    Description:
 *        Created:  12/8/2008 5:30:33 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 194                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include <algorithm>

#include "TedContainer.h"
#include "TedVarMan.h"
#include "ETedNode.h"

namespace ted {

#ifndef INLINE
#include "TedContainer.inl"
#endif

	struct DeleteObject {
		template <typename T> void operator()(const T* ptr)const {
			delete ptr;
		}
	};

	TedContainer::~TedContainer(void) {
		for(iterator it = begin(); it != end(); it++) {
			TedSet* ps = it->second.second;
			for (TedSet::iterator jt = ps->begin(), jt_end = ps->end(); jt!=jt_end; jt++) {
				TedNode* pNode = (*jt);
				assert(pNode);
				TedKids* pKid = pNode->getKids();
				delete pNode;
				if (pKid) {
					delete pKid;
				}
			}
			delete ps;
		}
	}

	bool TedContainer::hasNode(TedNode* pNode) const {
		if(pNode==TedNode::getOne())
			return true;
		return hasNode(*pNode->getVar(),*pNode->getKids(),pNode->getRegister());
	}

	bool TedContainer::hasNode(const TedVar& pNode_var, const TedKids& pNode_kids, const TedRegister& pNode_reg) const {
		const_iterator it = find(pNode_var);
		if(it == end())
			return false;
		for(TedSet::const_iterator jt = it->second.second->begin(); jt != it->second.second->end(); jt ++) {
			if(((*jt)->getRegister() == pNode_reg)&&(*jt)->getKids()->equal(pNode_kids)) {
				return true;
			}
		}
		return false;
	}

	bool TedContainer::check_children_are_included(void) const {
		for(const_iterator it = begin(); it != end(); it++) {
			for(TedSet::const_iterator jt = it->second.second->begin(); jt != it->second.second->end(); jt ++) {
				const TedNode* pNode = (*jt);
				assert(pNode);
				assert(pNode->getKids());
				FOREACH_KID_OF_NODE(pNode) {
					TedNode* pKid = _iterkids.Node<TedNode>();
					if(!existsOrOne(pKid)) {
						return false;
					}
				}
			}
		}
		return true;
	}

	bool TedContainer::existsOrOne(TedNode* pNode) const {
		if(pNode==TedNode::getOne())
			return true;
		const_iterator it = find(*pNode->getVar());
		if(it == end())
			return false;
		TedSet::const_iterator jt = it->second.second->find(pNode);
		if(jt == it->second.second->end())
			return false;
		return true;
	}

	bool TedContainer::eraseNode(TedNode* pNode) {
		if(pNode==TedNode::getOne())
			return false;
		iterator it = find(*pNode->getVar());
		if(it == end())
			return false;
		TedSet::iterator jt = it->second.second->find(pNode);
		if(jt == it->second.second->end())
			return false;
		it->second.second->erase(jt);
		return true;
	}

	/** @brief similar to getNode(const TedNode*)but inserts the current node into the set*/
	TedNode* TedContainer::getNode(ETedNode* pNode) {
		if(pNode==TedNode::getOne())
			return pNode;
		iterator it = find(*pNode->getVar());
		//TedNode* ptr = NULL;
		if(it == end()) {
			unsigned int keyLevel = getMaxLevel()+1;
			(*this)[*pNode->getVar()] = pair<unsigned int, TedSet*>(keyLevel, new TedSet);
			(*this)[*pNode->getVar()].second->insert(pNode);
		}	else {
			for(TedSet::iterator jt = it->second.second->begin(); jt != it->second.second->end(); jt ++) {
				if(((*jt)->getRegister() == pNode->getRegister())&&(*jt)->getKids()->equal(*pNode->getKids())) {
					return*(jt);
				}
			}
			it->second.second->insert(pNode);
		}
		return pNode;
	}


	/** @brief Creates or Retrieves a TedNode from a variable and kids
	  @detail There are two main cases:
	  1)When the variable does not exist,
	  - create the set of nodes for the variable.
	  - insert the set into the map.
	  - create the node and insert it into the map.
	  2)When the variable exist,
	  - iterate through the set find nodes with same kids.
	  - if found return the node.
	  - when not found, create the new node, and put insert it into the map, return the node.
	 *******/
	TedNode* TedContainer::getNode(const TedVar & v, const TedKids & k, const TedRegister & r) {
		assert(!v.isConst());
		assert(!k.empty());
		iterator it = find(v);
		TedNode* ptr = NULL;
		if(it == end()) {
			unsigned int keyLevel = getMaxLevel()+1;
			(*this)[v] = pair<unsigned int, TedSet*>(keyLevel, new TedSet);
			ptr = new ETedNode(v, k, r); //it was TedNode
			(*this)[v].second->insert(ptr);
		}	else {
			for(TedSet::iterator jt = it->second.second->begin(); jt != it->second.second->end(); jt ++) {
				if((*jt)->getRegister() == r &&(*jt)->getKids()->equal(k) == true) {
					return*(jt);
				}
			}
			ptr = new ETedNode(v, k, r); //it was TedNode
			it->second.second->insert(ptr);
		}
		return ptr;
	}

	/** @brief Is the first node is Lower(level)than the second node*/
	bool TedContainer::isFirstLower(const TedVar& v1, const TedVar& v2)const {
		if(v2.isConst()) {
			return false;
		} else {
			if(v1.isConst()) {
				return true;
			} else {
				TedContainer::const_iterator v1_it = find(v1);
				TedContainer::const_iterator v2_it = find(v2);
				assert(v1_it!=this->end());
				assert(v2_it!=this->end());
				unsigned int v1_level = v1_it->second.first;
				unsigned int v2_level = v2_it->second.first;
				return v1_level > v2_level;
			}
		}
	}

	/** @brief Is the first node is Higher(level)than the second node*/
	bool TedContainer::isFirstHigher(const TedVar& v1, const TedVar& v2)const {
		if(v1.isConst()) {
			return false;
		} else {
			if(v2.isConst()) {
				return true;
			} else {
				return(find(v1)->second.first < find(v2)->second.first);
			}
		}
	}

	/** @brief Does the first node have the same height(level)as the second node*/
	bool TedContainer::isSameHight(const TedVar& v1, const TedVar& v2)const {
		if(v1.isConst()&& v2.isConst()) {
			return true;
		}
		if(v1.isConst()|| v2.isConst()) {
			return false;
		}
		return(find(v1)->second.first == find(v2)->second.first);
	}

	void TedContainer::registerVar(const TedVar& var) {
		if(find(var) == end()) {
			unsigned int keyLevel = getMaxLevel()+1;
			(*this)[var] = pair<unsigned int,TedSet*>(keyLevel, new TedSet);
		}
	}

	void TedContainer::registerVarAtTop(const TedVar& var) {
		if(find(var) == end()) {
			for(iterator it = begin(); it != end(); it++) {
				it->second.first++;
			}
			(*this)[var] = pair<unsigned int, TedSet*>(1, new TedSet);
		}
	}

	void TedContainer::registerRetimedVarAt(const TedVar& var, const TedVar& atVar) {
		if (getLevel(atVar)==0) {
			assert(&var==&atVar || var.getBase()==&atVar);
			if (find(var)==end()) {
				(*this)[var] = pair<unsigned int, TedSet*>(getMaxLevel()+1, new TedSet);
			}
		} else {
			unsigned int level = getMaxLevel()+1-getLevel(atVar);
			if(0==level) {
				throw string("04008. Reference variable does not exist");
			} else {
				if(find(var) == end()) {
					for(iterator it = begin(); it != end(); it++) {
						if(it->second.first > level) {
							it->second.first++;
						}
					}
					(*this)[var] = pair<unsigned int, TedSet*>(level+1, new TedSet);
				}
			}
		}
	}

	void TedContainer::registerRetimedVar(const TedVar& var) {
		const TedVar* base = var.getBase();
		assert(base);
		if(find(var) == end()) {
			BaseVarsRetimed retime_map = getExistingRetimedVarsPerBaseClass();
			TedVar candidate =*base;
			if(retime_map.find(base)!=retime_map.end()) {
				set<TedVar>& retime_set = retime_map[base];
				for(set<TedVar>::iterator it = retime_set.begin(), end=retime_set.end(); it != end; it++) {
					TedVar current =*it;
					//retime_map[base] contains base node with register 0, so at least one case will hold true
					if(current.getRegister()< var.getRegister()) {
						if(current.getRegister()> candidate.getRegister()) {
							candidate = current;
						}
					}
				}
			}
			registerRetimedVarAt(var,candidate);
		}
	}

	/**
	 * @brief  returns a reference to the variable with the given level
	 *         If there is not such a level, throws an exception.
	 **/
	const TedVar& TedContainer::getVarAtLevel(unsigned int level) {
		unsigned int pos = getMaxLevel()+1-level;
		for(const_iterator it = begin(); it != end(); it++) {
			if(pos == it->second.first) {
				return  it->first;
			}
		}
		throw string("04009. The requested level does not exist");
	}

	void TedContainer::rotateLevels(const TedVar& higher, const TedVar& lower) {
		iterator fit = find(higher);
		iterator lit = find(lower);
		assert(fit!=end());
		assert(lit!=end());
		unsigned int fint = fit->second.first;
		unsigned int lint = lit->second.first;
		assert(fint > lint);
		//TedContainer is ordered by TedVar, not by its position, so we have to
		//traverse the entire container updating wherever needed.
		for(iterator it = begin(); it != end(); it++) {
			if(lint <= it->second.first && it->second.first < fint) {
				it->second.first++;
			}
		}
		fit->second.first = lint;
	}

	void TedContainer::removeLevel(const TedVar& var) {
		iterator rit = find(var);
		assert(rit!=end());
		assert(rit->second.second->empty());
		for(iterator it = begin(); it!= end(); it++) {
			if(it->second.first > rit->second.first) {
				it->second.first--;
			}
		}
		delete rit->second.second;
		erase(rit);
	}

	TedContainerOrder TedContainer::order(void) {
		TedContainerOrder ordered;
		for(iterator it = begin(); it!= end(); it++) {
			ordered[it->second.first] = pair<TedVar*,TedSet*>((TedVar*)&it->first, it->second.second);
		}
		return ordered;
	}

	//
	// find all retimed variables corresponding to a base variable
	//
	// base_var ---> set of retimed variables of base_var
	//
	BaseVarsRetimed TedContainer::getExistingRetimedVarsPerBaseClass(void) {
		BaseVarsRetimed retimed_var_map;
		for(iterator it = begin(); it != end(); it++) {
			TedVar var = it->first;
			if(!var.isRetimed())
				continue;
			const TedVar* base = var.getBase();
			if(retimed_var_map.find(base) ==retimed_var_map.end()) {
				set<TedVar> start_set;
				start_set.insert(var);
				start_set.insert(*base);
				retimed_var_map[base] = start_set;
			} else {
				retimed_var_map[base].insert(var);
			}
		}
		return retimed_var_map;
	}

	//
	// get the correct order sequence for each retimed variable
	// in a decreasing order LT = LESS THAN
	//
	// a@2 < a@1
	// a@3 < a@2,a@1
	// a@4 < a@3,a@2
	// a@5 < a@3,a@1
	// a@6 < a@2
	// a@7 < a@3
	//
	// the input is the existing retimed vars of base and the base var itself
	//
	// NOTE: this could be optimized to
	// a@4,a@5,a@7 < a@3, < a@2 < a@1
	//               a@6
	//
	// NOTE: if the base var is present int the container, it is inserted into the retimed_vars_of_base
	//
	// NOTE: the following example produces conflicting ordering
	// poly N@3*[{a^2*(a)@2}@2]+a@4*(a@2)^4
	//
	// the above polynomial is constructed as
	//({(N,3,@),[{(a,2,^),(a,2,@),*},2,@],*},[(a,4,@),{(a,2,@),4,^},*],+)
	//       1         2       3                   7        8
	//              ------------------                   ------------
	//                       4                                9
	//                       -------------       ----------------------
	//                             5                       10
	//   -------------------------------------
	//                       6
	//                                           11
	//
	// an the Partial order obtained is:
	//  a@2 < a@4,
	//  a@4 < a@2,
	// which is a conflicting order
	//
	// the reason for this behavior lies during the construction of operation 5
	// at operation 4 we have defined
	//        a^2 --------> a@2
	PartialOrder TedContainer::derivePartialOrderLT(set<TedVar>& retimed_vars_of_base, const TedVar* base_var) {
		PartialOrder var_lesser_than;
		//the base var might exist in the container
		bool base_var_in_container = (find(*base_var)!= end());
#if 0
		if(base_var_in_container) {
			retimed_vars_of_base.insert(*base_var);
		}
#endif
		for(set<TedVar>::iterator jit = retimed_vars_of_base.begin(), jend = retimed_vars_of_base.end(); jit != jend; jit++) {
			TedVar var =*jit;
			const TedVar* base = var.getBase();
			if(base_var)
				assert((base && base==base_var)|| \
					   (base_var_in_container && this->isSameHight(var,*base_var)));
			TedSet& nodes =*(*this)[var].second;
			for(TedSet::iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				TedNode* node =*kit;
				FOREACH_KID_OF_NODE(node) {
					TedNode* kidnode = _iterkids.Node<TedNode>();
					if((base && kidnode->getVar()->getBase() == base)|| \
					   (base_var_in_container && this->isSameHight(*kidnode->getVar(),*base_var))) {
						if(var_lesser_than.find(*kidnode->getVar()) ==var_lesser_than.end()) {
							set<TedVar> lthan;
							lthan.insert(var);
							var_lesser_than.insert(pair< TedVar,set<TedVar> >(*kidnode->getVar(),lthan));
						} else {
							var_lesser_than[*kidnode->getVar()].insert(var);
						}
					}
				}
			}
		}
#define _DEBUG_TED_ORDER
#ifdef _DEBUG_TED_ORDER
		for(PartialOrder::iterator it = var_lesser_than.begin(), end = var_lesser_than.end(); it!=end; it++) {
			set<TedVar>& s = it->second;
			cout <<(it->first).getName()<< " < ";
			for(set<TedVar>::iterator jit = s.begin(), jend =s.end(); jit != jend; jit++) {
				cout <<(*jit).getName()<< ", ";
			}
			cout << endl;
		}
#endif
		return var_lesser_than;
	}

	PartialOrder TedContainer::deriveOrderLT(void) {
		PartialOrder var_lesser_than;
		for(iterator it = begin(); it != end(); it++) {
			const TedVar& var = it->first;
			TedSet& nodes =*(*this)[var].second;
			for(TedSet::iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				TedNode* node =*kit;
				assert(node);
				FOREACH_KID_OF_NODE(node) {
					TedNode* kidnode = _iterkids.Node<TedNode>();
					if (kidnode == TedNode::getOne()) {
						continue;
					}
					if(var_lesser_than.find(*kidnode->getVar()) ==var_lesser_than.end()) {
						set<TedVar> lthan;
						lthan.insert(var);
						var_lesser_than.insert(pair< TedVar,set<TedVar> >(*kidnode->getVar(),lthan));
					} else {
						var_lesser_than[*kidnode->getVar()].insert(var);
					}
				}
			}
		}
		return var_lesser_than;
	}

	PartialOrder TedContainer::patchOrderLT(void) {
		PartialOrder var_lesser_than;
		for(iterator it = begin(); it != end(); it++) {
			const TedVar& var = it->first;
			TedSet& nodes =*(*this)[var].second;
			for(TedSet::iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				TedNode* node =*kit;
				assert(node);
				FOREACH_KID_OF_NODE(node) {
					TedNode* kidnode = _iterkids.Node<TedNode>();
					if (kidnode == TedNode::getOne()) {
						continue;
					}
					if ( this->isFirstLower(var,*kidnode->getVar()) ) {
						if(var_lesser_than.find(*kidnode->getVar()) ==var_lesser_than.end()) {
							set<TedVar> lthan;
							lthan.insert(var);
							var_lesser_than.insert(pair< TedVar,set<TedVar> >(*kidnode->getVar(),lthan));
						} else {
							var_lesser_than[*kidnode->getVar()].insert(var);
						}
					}
				}
			}
		}
		return var_lesser_than;
	}

	PartialOrder TedContainer::patchOrderGT(void) {
		PartialOrder var_gt;
		for(iterator it = begin(); it != end(); it++) {
			const TedVar& var = it->first;
			TedSet& nodes =*(*this)[var].second;
			for(TedSet::iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				TedNode* node =*kit;
				assert(node);
				FOREACH_KID_OF_NODE(node) {
					TedNode* kidnode = _iterkids.Node<TedNode>();
					if (kidnode == TedNode::getOne()) {
						continue;
					}
					if ( this->isFirstLower(var,*kidnode->getVar()) ) {
						if(var_gt.find(*kidnode->getVar()) ==var_gt.end()) {
							set<TedVar> gthan;
							gthan.insert(*kidnode->getVar());
							var_gt.insert(pair< TedVar,set<TedVar> >(var,gthan));
						} else {
							var_gt[var].insert(*kidnode->getVar());
						}
					}
				}
			}
		}
		return var_gt;
	}

	//
	// get the correct order sequence for each retimed variable
	// in a incrasing order GT = GREATER THAN
	//
	// a@3 > a@5,a@4,a@7
	// a@1 > a@2,a@3,a@5
	// a@2 > a@4,a@3,a@6
	//
	// the input is the existing retimed vars of base and the base var itself
	//
	// NOTE: this could be optimized to
	// a@1 > a@2 > a@6,
	//             a@3  > a@4,a@5,a@7
	//
	PartialOrder TedContainer::derivePartialOrderGT(set<TedVar>& retimed_vars_of_base,const TedVar* base_var) {
		PartialOrder var_greater_than;
		for(set<TedVar>::iterator jit = retimed_vars_of_base.begin(), jend = retimed_vars_of_base.end(); jit != jend; jit++) {
			TedVar var =*jit;
			const TedVar* base = var.getBase();
			if(base_var)assert(base==base_var);
			TedSet& nodes =*(*this)[var].second;
			set<TedVar> hthan;
			for(TedSet::iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				TedNode* node =*kit;
				FOREACH_KID_OF_NODE(node) {
					TedNode* kidnode = _iterkids.Node<TedNode>();
					if(kidnode->getVar()->getBase() == base) {
						hthan.insert(*kidnode->getVar());
					}
				}
			}
			if(!hthan.empty()) {
				var_greater_than[var] = hthan;
			}
		}
		return var_greater_than;
	}

	// NOTE: assumes there are no loops
	// i.e.
	//       a1--\               /--->a2
	//       	  \             /
	//       	   --> b1   b2--
	//
	int TedContainer::findMaxDepth(PartialOrder& guide, TedVar var) {
		int max = 0;
		if(guide.find(var)!= guide.end()) {
			for(set<TedVar>::iterator it = guide[var].begin(); it != guide[var].end(); it++) {
				TedVar var_next =*it;
				//
				// avoid infinite recursion
				// Under some convination of retime and reordering
				// we can end up with:
				//
				//  A---\ 2R    /---->A
				//       \     / R
				//        -->A@1
				//            \
				//             \
				//              -->B
				//
				// note that A@1 has node A both as a parent and as a child
				//
				if(isSameHight(var_next,var))
					continue;
				int max_next = findMaxDepth(guide,var_next)+1;
				if(max_next > max)
					max = max_next;
			}
		}
		return max;
	}

	void TedContainer::keepMaxDepth(PartialOrder& guide, TedVar var) {
		int depth = findMaxDepth(guide,var)- 1;
		if(depth==0)
			return;
		set<TedVar>& workset = guide[var];
		if(workset.size() ==1)
			return;
		set<TedVar> copy = workset;
		for(set<TedVar>::iterator it = copy.begin(); it != copy.end(); it++) {
			int test = findMaxDepth(guide,*it);
			if(test!=0 && test<depth) {
				workset.erase(*it);
			}
		}
	}

	// it takes the derived partial order
	//
	// a@2 < a@1
	// a@3 < a@2,a@1
	// a@4 < a@3,a@2
	// a@5 < a@3,a@1
	// a@6 < a@2
	// a@7 < a@3
	//
	// and it optimizes it to:
	//
	// a@2 < a@1
	// a@3 < a@2
	// a@4 < a@3
	// a@5 < a@3
	// a@6 < a@2
	// a@7 < a@3
	//
	void TedContainer::optimizePartialOrderLT(PartialOrder& guide) {
		for(PartialOrder::iterator it = guide.begin(); it != guide.end(); it++) {
			keepMaxDepth(guide,it->first);
		}
	}


	PartialOrder TedContainer::getOrderLT(void) {
		PartialOrder ret = deriveOrderLT();
		optimizePartialOrderLT(ret);
		return ret;
	}

	PartialOrder TedContainer::getPartialOrderLT(set<TedVar>& retimed_vars_of_base, const TedVar* base_var) {
		PartialOrder ret = derivePartialOrderLT(retimed_vars_of_base,base_var);
		optimizePartialOrderLT(ret);
		return ret;
	}


#if 0
	void TedContainer::printVars(void)const {
		for(TedContainer::const_iterator it = begin(); it != end(); it++) {
			cout << it->second.first << ": " << it->first.getName()<< endl;
		}
	}
#endif

	void TedContainer::update_mark(void(Mark<TedNode>::*functor)(void)) {
		for(iterator it = begin(); it != end(); it++) {
			const TedVar& var = it->first;
			TedSet& nodes =*(*this)[var].second;
			for(TedSet::iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				TedNode* node =*kit;
				assert(node);
				(node->visited.*functor)();
				assert(node->getKids());
				node->getKids()->visited.setMark();
			}
		}
	}


	void TedContainer::print_container(void) const {
		for(const_iterator it = begin(); it != end(); it++) {
			const TedVar& var = it->first;
			cout << "------- " << var.getName() << " index=" << this->at(var).first << endl;
			const TedSet& nodes =*this->at(var).second;
			for(TedSet::const_iterator kit = nodes.begin(), kend=nodes.end(); kit!=kend; kit++) {
				const TedNode* node =*kit;
				cout << node << " ";
			}
			cout << endl;
		}
	}

	bool TedContainer::check_all_nodes_are_marked_at_var(const TedVar& var) const {
		for(TedSet::const_iterator it = this->at(var).second->begin(), end = this->at(var).second->end(); it!=end; it++) {
			if (!(*it)->visited.isMarked()) {
				return false;
			}
		}
		return true;
	}
}
