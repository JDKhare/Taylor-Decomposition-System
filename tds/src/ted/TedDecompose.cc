/*
 * =====================================================================================
 *
 *       Filename:  TedDecompose.cc
 *    Description:
 *        Created:  16/7/2009 11:31:10 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include "TedNode.h"
#include "TedDecompose.h"
#include "TedMan.h"

namespace ted {

	unsigned int TedDecompose::_st_count = 0;
	unsigned int TedDecompose::_pt_count = 0;

	TedDecompose::TedDecompose(TedMan* manager, bool onlyPOs):_currentManager(manager), _onlyPOs(onlyPOs) {
		assert(_currentManager);
	}

	TedDecompose::~TedDecompose(void) {
		_currentManager = NULL;
		_parents.clear();
	}

	void TedDecompose::getParents(void) {
		assert(_parents.empty());
		if (_onlyPOs) {
			_currentManager->preOrderDFSforPOs(_parents,&TedParents::collect);
		} else {
			_currentManager->preOrderDFS(_parents,&TedParents::collect);
		}
	}

	bool TedDecompose::isIrreducible(void) {
		for(TedMan::PrimaryOutputs::iterator it = _currentManager->_pos.begin(); it != _currentManager->_pos.end(); it++) {
			if(it->second.getType() ==TedNodeRoot::PO) {
				TedNode* atRoot = it->second.Node();
				if(atRoot->numKids()> 1) {
					return false;
				} else {
					FOREACH_KID_OF_NODE(atRoot) {
						TedNode* kid = _iterkids.Node<TedNode>();
						if(kid!=TedNode::getOne())
							return false;
					}
				}
			}
		}
		return true;
	}

	//TODO: be able to extract
	//      case 1: a*(b+3)*c
	//      case 2: a*(b+5)*(a+5)*b
	// at the moment these are irreducible
	void TedDecompose::findProductSupport(void) {
		assert(_bottomBFS.empty());
		assert(_oneTerm.empty());
		assert(_allTerms.empty());
		//Start at node ONE => initialize the traversal with the parents of ONE
		Mark<TedNode>::newMark();
		for(MyParent::iterator it = _parents[TedNode::getOne()]->begin(), end = _parents[TedNode::getOne()]->end(); it != end; it++) {
			_bottomBFS.insert(it->first);
			it->first->visited.setWaterMark();
		}
		while(!_bottomBFS.empty()) {
			TedNode* current = _bottomBFS.front();
			TedNode* next = current;
			_bottomBFS.pop_front();
			do {
				_oneTerm.push_front(next);
				if((next->numKids() ==1)&& \
				   (_parents.numParents(next) ==1)&& \
				   (_parents[next]->begin()->second->front() == 1) &&\
				   !_currentManager->isPO(next)) {
					next = _parents[next]->begin()->first;
				} else {
					next = NULL;
				}
			} while(next);
			if(_oneTerm.size()> 1) {
				bool alreadyExtracted = false;
				TedNode* topOfChain = _oneTerm.front();
				if(!_parents.hasParents(topOfChain)) {
					for(TedMan::PrimaryOutputs::iterator it = _currentManager->_pos.begin(); it != _currentManager->_pos.end(); it++) {
						if((it->second.Node() == topOfChain)&& \
						   ((it->second.getType() ==TedNodeRoot::ST)||(it->second.getType() ==TedNodeRoot::PT))) {
							alreadyExtracted = true;
							break;
						}
					}
					//only do PO nodes, do not do any support PT,ST
				}
				if(!alreadyExtracted) {
					//do not visit this node as they will be extracted
					for(chainOP::iterator it = _oneTerm.begin(), end = _oneTerm.end(); it!= end; it++) {
						(*it)->visited.setMark();
					}
					//save the chain of nodes to be extracted: front = bottom node, back = top node
					_allTerms.push_back(_oneTerm);
				}
			} else {
				//do not visit this node again on the BFS, we have already tried it
				current->visited.setMark();
				if(_parents.hasParents(current)) {
					//If it is not a primary output, annotate the parent reachable from here(BFS)
					for(MyParent::iterator it = _parents[current]->begin(), end = _parents[current]->end(); it!= end; it++) {
						TedNode* pNode = it->first;
						if(!pNode->visited.isMarked()&& !pNode->visited.isWaterMarked())
							_bottomBFS.insert(pNode);
					}
				}
				//oneProductTerm.clear();
			}
			_oneTerm.clear();
		}
	}

	void TedDecompose::extractProductSupport(BinaryThreadedTree<int,chainOP>& bt) {
		BinaryThreadedTree<int,chainOP>::inorder_iterator iot = bt.begin();
		BinaryThreadedTree<int,chainOP>::inorder_iterator iot_end = bt.end();
		Mark<TedNode>::newMark();
		//extract the support found
		for(;iot!=iot_end;++iot) {
			chainOP oneProductTerm((*iot)->getData());
			if(1==oneProductTerm.size()&& oneProductTerm.front()->getBackptr()) {
				continue;
			}
			assert(oneProductTerm.size()>1);

			TedNode* bottomOfChain = oneProductTerm.back();
			TedNode* topOfChain = oneProductTerm.front();
			oneProductTerm.pop_front();
			TedNode* chainFromTop = oneProductTerm.front();
			oneProductTerm.push_front(topOfChain);
			if(!topOfChain->visited.isMarked()) {
				removeTermFromContainer(oneProductTerm);
			}
			TedKids kidsPT;
			unsigned topIndex = 0;
			TedNode* topKid = NULL;
			wedge topWeight = 0;
			list<unsigned int> index2erase;
			FOREACH_KID_OF_NODE(topOfChain) {
				topIndex = _iterkids.getIndex();
				topKid = _iterkids.Node<TedNode>();
				topWeight = _iterkids.getWeight();
				//mark for cut the upper edges
				if(topKid!=chainFromTop) {
					kidsPT.insert(topIndex,topKid,topWeight,TedRegister(0));
					index2erase.push_back(topIndex);
				}
			}
			while(!index2erase.empty()) {
				topOfChain->removeKid(index2erase.back());
				index2erase.pop_back();
			}
			assert(kidsPT.has(1) ==false);
			assert(bottomOfChain->numKids() ==1 && bottomOfChain->hasKid(1));
			kidsPT.insert(1,bottomOfChain->getKidNode(1),1,TedRegister(0));
			bottomOfChain->replaceKid(1,TedNode::getOne(),bottomOfChain->getKidWeight(1));
			// generate the new Node Product Term #.
			if(!topOfChain->visited.isMarked()) {
				TedNode* newTop = updateReplicatedTermReference(oneProductTerm,1);
				if(!newTop->visited.isMarked()) {
					newTop->visited.setMark();
					string nameVarPT="PT" + Util::itoa(++_pt_count);
					assert(_currentManager->_vars.getIfExist(nameVarPT) ==NULL);
					TedVar* varPT = _currentManager->_vars.createVar(nameVarPT);
					TedNode* nodePT = _currentManager->_container.getNode(*varPT,kidsPT);
					updateReferences(topOfChain,nodePT);
					//require normalization
					TedNodeRoot po(newTop,1,TedNodeRoot::PT);
					po.setLink(nodePT);
					annotateBackptr(nodePT,newTop);
					_currentManager->_container.rotateLevels(*varPT,*(newTop->getVar()));
					_currentManager->linkTedToPO(po,nameVarPT);
					if(_parents.hasParents(newTop)) {
						updateFutureChainOP(newTop,*varPT,iot,iot_end);
					}
				} else {
					mergeReplicatedTerm(TedNodeRoot::PT,topOfChain,newTop,kidsPT);
				}
				bottomOfChain->visited.setMark();
				removeParents(oneProductTerm);
				while(!oneProductTerm.empty()) {
					TedNode* toErase = oneProductTerm.back();
					oneProductTerm.pop_back();
					if (!toErase->visited.isMarked()) {
						delete toErase;
					}
				}
			} else {
				mergeReplicatedTerm(TedNodeRoot::PT,topOfChain,topOfChain,kidsPT);
			}
		}
	}

	void TedDecompose::updateFutureChainOP(TedNode* newTop, const TedVar& var, \
										   BinaryThreadedTree<int,chainOP>::inorder_iterator iot, \
										   BinaryThreadedTree<int,chainOP>::inorder_iterator iot_end) {
		TedNode* nodeVarPT = _currentManager->getNode(var);
		updateParentReferences(newTop,nodeVarPT);
		annotateBackptr(nodeVarPT,newTop);
		//look in the remaining chainOp to see if we need to update the Chain
		++iot;
		for(;iot!=iot_end;++iot) {
			chainOP& nextProductTerm = (*iot)->obtainData();
			chainOP::iterator it = nextProductTerm.begin();
			chainOP::iterator it_end = nextProductTerm.end();
			for(;it!=it_end;++it) {
				if(*it==newTop)
					break;
			}
			if(it!= it_end) {
				it;
				nextProductTerm.erase(it,nextProductTerm.end());
				nextProductTerm.push_back(nodeVarPT);
				//if(1==nextProductTerm.size()) {
				//	throw(string("04XXX. Updated decomposition chain has only one member"));
				//}
			}
		}
	}

	void TedDecompose::annotateBackptr(TedNode* pNode, TedNode* backptr) {
		pNode->setBackptr(backptr);
	}

	/** @brief extract product terms, return 1 if there were any extraction, 0 otherwise*/
	bool TedDecompose::productTerm(void) {
		unsigned int start= _pt_count;
		getParents();
		if(!_parents.isLinear()) {
			throw(string("04015. Linearize first"));
		}
		findProductSupport();
		//reorder the support found to keep the parent information of successive extractions valid
		//allProductTerms should be arranged having its chains ordered from top to bottom. One can
		//determine the order of a chain by looking at its top variable.
		BinaryThreadedTree<int,chainOP> bt;
		while(!_allTerms.empty()) {
			chainOP oneProductTerm = _allTerms.back();
			_allTerms.pop_back();
			TedNode* topOfChain = oneProductTerm.front();
			int level = _currentManager->getLevel(topOfChain);
			bt.insert(level,oneProductTerm);
		}
		extractProductSupport(bt);
		_parents.clear();
		return(start<_pt_count);
	}

	void TedDecompose::findSumSupport(void) {
		assert(_bottomBFS.empty());
		assert(_oneTerm.empty());
		assert(_allTerms.empty());
		_bottomBFS.insert(TedNode::getOne());
		Mark<TedNode>::newMark();
		while(!_bottomBFS.empty()) {
			TedNode* current = _bottomBFS.front();
			_bottomBFS.pop_front();
			if(_parents.numParents(current)<= 1 || _currentManager->isPO(current)) {
				//do not visit this node again on the BFS, we have already tried it
				current->visited.setMark();
				if(_parents.hasParents(current) && !_currentManager->isPO(current)) {
					//If it is not a primary output, annotate the parent reachable from here(BFS)
					for(MyParent::iterator it=_parents[current]->begin(), end=_parents[current]->end(); it!=end; it++) {
						_bottomBFS.insert(it->first);
					}
				}
			} else {
				map<unsigned int, Term> stClique;
				Term stCandidates;	//used as a marker only
				Term stBreakOnTop;	//used as a marker only
				//There is no need to insert the base "current" node, from which we traverse its
				//parents searching for a possible adder chain, as to be part of the chain
				//stClique[_currentManager->getLevel(current)].insert(current);
				//stCandidates.insert(current);
				for(MyParent::iterator it=_parents[current]->begin(), end=_parents[current]->end(); it!=end; it++) {
					TedNode* candidate = it->first;
					if((candidate->hasKid(1))&&(candidate->getKidNode(1) == current)) {
						stClique[_currentManager->getLevel(candidate)].insert(candidate);
						stCandidates.insert(candidate);
						if(_parents.hasParents(candidate)&&(_parents.numParents(candidate)!= 1) || _currentManager->isPO(candidate)) {
							//if this node belongs to an adder chain, it must become the top of a chain
							stBreakOnTop.insert(candidate);
						}
					} else {
						_bottomBFS.insert(candidate);
					}
				}
				for(map<unsigned int, Term >::reverse_iterator it = stClique.rbegin(), \
					end=stClique.rend(); it!=end; it++) {
					for(Term::iterator jt = it->second.begin(), endjt = it->second.end(); jt!=endjt; jt++) {
						TedNode* next =*jt;
						// as we are beginning an adder chain, it does not matter if next is in stBreakOnTop,
						// because the "next" TedNode is already the top of this adder chain.
						do {
							if(!next->visited.isMarked()) {
								next->visited.setMark();
								_oneTerm.push_back(next);
								if(next->hasKid(0)) {
									Term::iterator kt=stCandidates.find(next->getKidNode(0));
									if(kt!=stCandidates.end()) {
										Term::iterator lt=stBreakOnTop.find(next->getKidNode(0));
										if(lt!=stBreakOnTop.end()) {
											if(_oneTerm.size()> 1) {
												_allTerms.push_back(_oneTerm);
											} else  if(_oneTerm.size() == 1) {
												TedNode* recover = _oneTerm.front();
												_bottomBFS.insert(recover);
											}
											_oneTerm.clear();
										}
										next= (*kt);
									} else {
										next = NULL;
									}
								} else {
									next = NULL;
								}
							} else {
								next = NULL;
							}
						} while(next);
						if(_oneTerm.size()> 1) {
							_allTerms.push_back(_oneTerm);
						} else if(_oneTerm.size() == 1) {
							TedNode* recover = _oneTerm.front();
							_bottomBFS.insert(recover);
						}
						_oneTerm.clear();
					}
				}
			}
		}
		assert(_bottomBFS.empty());
		assert(_oneTerm.empty());
	}


	void TedDecompose::removeTermFromContainer(chainOP& currentTerm) {
		for(chainOP::iterator it = currentTerm.begin(), end=currentTerm.end(); it!=end; it++) {
			TedNode* pNode =*it;
			assert(pNode);
			const TedVar* pVar = pNode->getVar();
			assert(pVar);
			Term* toremove = _currentManager->_container[*pVar].second;
			if (!pNode->visited.isMarked()) {
				if(toremove->find(pNode)!= toremove->end())
					toremove->erase(pNode);
				else
					throw(string("04036. memory leak, node is no longer in the set"));
			}
		}
	}

	void TedDecompose::removeParents(chainOP& currentTerm) {
		for(chainOP::iterator it = currentTerm.begin(), end=currentTerm.end(); it!=end; it++) {
			_parents.unregisterKey(*it);
		}
	}

	/** @brief
	 * @details Assumes ChainOp is ordered
	 - the front corresponds to the top node
	 - the back corresponds to the bottom node
	 **/
	TedNode* TedDecompose::updateReplicatedTermReference(chainOP& oneTerm, unsigned int index) {
		TedNode* pNode = NULL;
		TedNode* pCurrent = NULL;
		TedNode* pNext = NULL;
		chainOP::reverse_iterator rit = oneTerm.rbegin();
		chainOP::reverse_iterator rend = oneTerm.rend();
		chainOP::reverse_iterator next = rit;
		--rend; // don't iterate through the first node
		++next; // ahead of rit by one, gets to the first node
		for(; rit!=rend; rit++, next++) {
			pCurrent =*rit;
			pNext =*next;
			pNode = _currentManager->_container.getNode(*(pCurrent)->getVar(),*(pCurrent)->getKids());
			annotateBackptr(pNode,pCurrent->getBackptr());
			pNode->visited.setMark();
			(*next)->replaceKid(index,pNode,pNext->getKidWeight(index),pNext->getKids()->getRegister(index));
		}
		pCurrent =*(oneTerm.begin());
		assert(pNext==pCurrent);
		pNode = _currentManager->_container.getNode(*(pNext)->getVar(),*(pNext)->getKids());
		annotateBackptr(pNode,pCurrent->getBackptr());
		return pNode;
	}

	void TedDecompose::extractSumSupport(BinaryThreadedTree<int,chainOP>& bt) {
		BinaryThreadedTree<int,chainOP>::inorder_iterator iot = bt.begin();
		BinaryThreadedTree<int,chainOP>::inorder_iterator iot_end = bt.end();
		Mark<TedNode>::newMark();
		//extract the support found
		for(;iot!=iot_end;++iot) {
			//strong assumption, correct only for linear TEDs
			//We are assuming all nodes have at most two children, edge-0 and edge-1
			chainOP oneSumTerm((*iot)->getData());

			if(1==oneSumTerm.size()&& oneSumTerm.front()->getBackptr()) {
				continue;
			}
			assert(oneSumTerm.size()>1);

			TedNode* topOfChain = oneSumTerm.front();
			TedNode* bottomOfChain= oneSumTerm.back();
			if(!topOfChain->visited.isMarked()) {
				removeTermFromContainer(oneSumTerm);
			}
			assert(topOfChain->hasKid(1));
			TedNode* root = topOfChain->getKidNode(1);
			TedKids kidsST;
			kidsST.insert(1,root,1,TedRegister(0));
			if(bottomOfChain->hasKid(0)) {
				wedge additive_weight = bottomOfChain->getKidWeight(0);
				if(bottomOfChain->getKidNode(0)!= root) {
					kidsST.insert(0,bottomOfChain->getKidNode(0),additive_weight,TedRegister(0));
					bottomOfChain->removeKid(0);
				} else {
					bottomOfChain->replaceKid(0,TedNode::getOne(),additive_weight,TedRegister(0));
				}
			}
			for(chainOP::iterator it= oneSumTerm.begin(), end=oneSumTerm.end(); it!=end; it++) {
				wedge toRoot_weight = (*it)->getKidWeight(1);
				(*it)->replaceKid(1,TedNode::getOne(),toRoot_weight);
			}
			//generate the new node Sum Term S#
			if(!topOfChain->visited.isMarked()) {
				TedNode* newTop = updateReplicatedTermReference(oneSumTerm,0);
				if(!newTop->visited.isMarked()) {
					newTop->visited.setMark();
					string nameVarST="ST" + Util::itoa(++_st_count);
					assert(_currentManager->_vars.getIfExist(nameVarST) ==NULL);
					TedVar* varST = _currentManager->_vars.createVar(nameVarST);
					// register the new Node in the _container.
					TedNode* nodeST = _currentManager->_container.getNode(*varST,kidsST);
					updateReferences(topOfChain,nodeST);
					//require normalization
					TedNodeRoot po(newTop,1,TedNodeRoot::ST);
					po.setLink(nodeST);
					annotateBackptr(nodeST,newTop);
					_currentManager->_container.rotateLevels(*varST,*(newTop->getVar()));
					_currentManager->linkTedToPO(po,nameVarST);
					if(_parents.hasParents(newTop)) {
						updateFutureChainOP(newTop,*varST,iot,iot_end);
					}
				} else {
					mergeReplicatedTerm(TedNodeRoot::ST,topOfChain,newTop,kidsST);
				}
				bottomOfChain->visited.setMark();
				removeParents(oneSumTerm);
				while(!oneSumTerm.empty()) {
					TedNode* toErase = oneSumTerm.back();
					oneSumTerm.pop_back();
					if (!toErase->visited.isMarked()) {
						delete toErase;
					}
				}
			} else {
				mergeReplicatedTerm(TedNodeRoot::ST,topOfChain,topOfChain,kidsST);
			}
		}
	}

	/** @brief extract sum terms, return 1 if there were any extraction, 0 otherwise*/
	bool TedDecompose::sumTerm(void) {
		unsigned int start = _st_count;
		getParents();
		findSumSupport();
		//reorder the support found to keep the parent information of successive extractions valid
		//allProductTerms should be arranged having its chains ordered from top to bottom. One can
		//determine the order of a chain by looking at its top variable.
		BinaryThreadedTree<int,chainOP> bt;
		while(!_allTerms.empty()) {
			chainOP oneSumTerm = _allTerms.back();
			_allTerms.pop_back();
			//TedNode* topOfChain = oneSumTerm.back();
			TedNode* topOfChain = oneSumTerm.front();
			int level = _currentManager->getLevel(topOfChain);
			bt.insert(level,oneSumTerm);
		}
		//extract the support found
		extractSumSupport(bt);
		_parents.clear();
		return(start<_st_count);
	}

	/** @brief Replace the current chain Term(topOfChain=oldTop), by an existing extracted Term(topOfChain=newTop).*/
	void TedDecompose::mergeReplicatedTerm(TedNodeRoot::Type type, TedNode* oldTop, TedNode* newTop, TedKids& kids) {
		for(TedMan::PrimaryOutputs::iterator it = _currentManager->_pos.begin(), end =_currentManager->_pos.end();  it != end; it++) {
			if(it->second.getType() ==type && it->second.Node() ==newTop) {
				TedVar* varPT = _currentManager->_vars.getIfExist(it->first);
				TedNode* nodePT = _currentManager->_container.getNode(*varPT,kids);
				updateReferences(oldTop,nodePT);
				annotateBackptr(nodePT,newTop);
				break;
			}
		}
	}

	/** @brief Replace all references to oldTop(from their parents)by newTop.
	 * @details If oldTop is a PO, we replace the root node referred by the TedNodeRoot, by newTop which is the new root.
	 **/
	void TedDecompose::updateReferences(TedNode* oldTop, TedNode* newTop) {
		if(_parents.hasParents(oldTop)) {
			//not a PO
			updateParentReferences(oldTop,newTop);
		}
		if (_currentManager->isPO(oldTop)) {
			//is a PO
			updatePoReferences(oldTop,newTop);
		}
	}

	void TedDecompose::updateParentReferences(TedNode* oldTop, TedNode* newTop) {
		for(MyParent::iterator it = _parents[oldTop]->begin(), end=_parents[oldTop]->end(); it!=end; it++) {
			TedNode* topParent = it->first;
			MyIndex* edgeConnection = it->second;
			for(unsigned int index = 0, last = edgeConnection->size(); index < last; index++) {
				int kidIndex = (*edgeConnection)[index];
				wedge weight = topParent->getKidWeight(kidIndex);
				topParent->replaceKid(kidIndex,newTop,weight);
			}
		}
	}

	void TedDecompose::updatePoReferences(TedNode* oldTop, TedNode* newTop) {
		bool exceptionFound = true;
		for(TedMan::PrimaryOutputs::iterator it = _currentManager->_pos.begin(); it != _currentManager->_pos.end(); it++) {
			if(it->second.Node() == oldTop) {
				if(_onlyPOs && it->second.getType()!=TedNodeRoot::PO) {
					throw(string("04016,04018. Top parent is not a PO"));
				}
				TedNodeRoot replacePO(newTop,it->second.getWeight(),it->second.getType());
				replacePO.setLink(it->second.getLink());
				replacePO.setRegister(it->second.getRegister());
				it->second = replacePO;
				exceptionFound = false;
				break;
			}
		}
		if(exceptionFound)
			throw(string("04017,04019. Unable to find the top parent in POs"));
	}


	/** @brief Decomposes the TED into its NFF
	 * @return the logic for continuing is: PT,ST = 1 when there was an extraction, 0 otherwise.
	 *  PT ST | continue
	 *  0  0  | 0(no)
	 *  0  1  | 1(yes)
	 *  1  0  | 0(no)
	 *  1  1  | 1(yes)
	 * therefore the equation is equal to ST
	 **/
	bool TedDecompose::allTermsWithoutReorder(void) {
		bool doOnceMore = false;
		//TedDecompose decompose(this);
		do {
			productTerm();
			doOnceMore = sumTerm();
		} while(doOnceMore);
		//check if the TED has been completely decomposed.
		//there are many cases in which the remaining TED can no longer be reduced unless the order of the
		//remaining variables is changed.
		return isIrreducible();
	}

}
