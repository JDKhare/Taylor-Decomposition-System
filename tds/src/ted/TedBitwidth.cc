/*
 * =====================================================================================
 *
 *       Filename:  TedBitwidth.cc
 *    Description:
 *        Created:  09/18/2009 08:39:38 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-11 21:46:11 +0100(Mon, 11 Jan 2010)$: Date of last commit
 * =====================================================================================
 */

#include <vector>
#include <map>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "Bitwidth.h"
using namespace data;

#include "TedBitwidth.h"
#include "TedNode.h"
#include "TedMan.h"
#include "TedParents.h"
namespace ted {

	bool _BitwidthOrder::operator()(Bitwidth* p1, Bitwidth* p2)const {
		return(*p1).isLessThan(*p2);
	}

	TedBitwidth::TedBitwidth(TedMan* manager): _currentManager(manager), _type(NONE) {
		assert(_currentManager);
		Bitwidth* choose = TedNode::getOne()->getBitwidth();
		if(!choose) {
			_currentManager = NULL;
			throw(string("04038. No bitwidth set yet"));
		} else {
			Integer* prec = dynamic_cast<Integer*>(choose);
			if(prec) {
				_type = INTEGER;
			} else {
				_type = FIXEDPOINT;
			}
		}
		switch(_type) {
		case INTEGER:
			_newPre = new Integer(0);
			_addOne = new Integer(1);
			break;
		case FIXEDPOINT:
			_newPre = new FixedPoint(0,0);
			_addOne = new FixedPoint(1,1);
			break;
		case NONE:
		default:
			throw(string("04037. No bitwidth set yet"));
			break;
		}
	}

	TedBitwidth::~TedBitwidth(void) {
		_currentManager = NULL;
		delete _newPre;
		delete _addOne;
	}

	void TedBitwidth::compute(void) {
		_currentManager->preOrderDFS(_parents,&TedParents::collect);
		Mark<TedNode>::newMark();
		for(TedMan::PrimaryOutputs::iterator it = _currentManager->_pos.begin(); it != _currentManager->_pos.end(); it++) {
			TedNodeRoot& root = it->second;
			if(root.getType() ==TedNodeRoot::PO) {
				TedNode* atRoot = root.Node();
				Bitwidth* prec = computeRec(atRoot);
				if(root.getBitwidth()) {
					delete root.getBitwidth();
				}
#if 0
				unsigned int weight = abs(root.getWeight());
#else
				wedge weight = abs(root.getWeight());
#endif
				if(1!=weight) {
					double bits_needed = log((double)weight)/log((double)2);
					double bits_upbound = ceil(bits_needed);
					unsigned int bits = (unsigned int)bits_upbound;
					if(bits_upbound == bits_needed) {
						//i.e. 16 decimal do not require 4 bits but 5.
						bits++;
					}
					Bitwidth* bitw = _newPre->clone();
					bitw->set(bits);
					prec->mpy(bitw);
					delete bitw;
				}
				root.setBitwidth(prec);
			}
		}
	}

	Bitwidth* TedBitwidth::computeRec(TedNode* pNode) {
		assert(NULL!=pNode);
		if(pNode->visited.isMarked()) {
			return pNode->getBitwidth()->clone();
		}
		if(TedNode::getOne() ==pNode) {
			return _newPre->clone();
		}
		pNode->visited.setMark();
		Bitwidth* varPre = pNode->getVar()->getBitwidth();
		if(pNode->getBackptr()&& !varPre) {
			varPre = computeRec(pNode->getBackptr());
			TedVar* var2set = _currentManager->checkVariables(pNode->getName());
			assert(var2set);
			assert(varPre);
			var2set->setBitwidth(varPre);
		}
		Bitwidth* iterPre = NULL;
		//populate the Bitwidth on each path annotate it on each edge path
		FOREACH_KID_OF_NODE(pNode) {
			unsigned int index = _iterkids.getIndex();
#if 0
			unsigned int weight = abs(_iterkids.getWeight());
#else
			wedge weight = abs(_iterkids.getWeight());
#endif
			TedNode* kid = _iterkids.Node<TedNode>();
			iterPre = computeRec(kid);
			if(0!=index) {
				if(1==index) {
					iterPre->mpy(varPre);
				} else {
					Bitwidth* kidPreCmp = varPre->clone();
					kidPreCmp->power(index);
					iterPre->mpy(kidPreCmp);
					delete kidPreCmp;
				}
			}
			if(1!=weight) {
				double bits_needed = log((double)weight)/log((double)2);
				double bits_upbound = ceil(bits_needed);
				unsigned int bits = (unsigned int)bits_upbound;
				if(bits_upbound == bits_needed) {
					//i.e. 16 decimal do not require 4 bits but 5.
					bits++;
				}
				Bitwidth* bitw = _newPre->clone();
				bitw->set(bits);
				iterPre->mpy(bitw);
				delete bitw;
			}
			if((*_iterkids).getBitwidth()) {
				delete(*_iterkids).getBitwidth();
			}
			(*_iterkids).setBitwidth(iterPre);
		}
		if(pNode->getBitwidth()) {
			delete pNode->getBitwidth();
		}
		Bitwidth* currentPre = compute(pNode);
		return currentPre->clone();
	}

	bool TedBitwidth::belongsToChain(TedNode* pNode) {
		if(1==_parents.numParents(pNode)&& \
		   0== (*(*_parents[pNode]->begin()).second->begin())&& \
		   1== (*_parents[pNode]->begin()).first->getKidWeight(0)) {
			return true;
		}
		return false;
	}

	// The chain of adders is propagated to the top, so a
	// informed decision could be made on the implementation
	void TedBitwidth::addToChain(TedNode* head, multimap<Bitwidth*,TedNode*,_BitwidthOrder> & toAdd) {
		TedNode* pinChain =  head;
		while(pinChain && belongsToChain(pinChain)) {
			if(pinChain->hasKid(0)) {
				Bitwidth* pre = (*pinChain->getKids())[0].getBitwidth();
				pinChain = pinChain->getKidNode(0);
				toAdd.insert(pair<Bitwidth*,TedNode*>(pre,pinChain));
			} else {
				pinChain = NULL;
			}
		}

	}

	Bitwidth* TedBitwidth::compute(TedNode* pNode) {
		unsigned int sizeis = pNode->numKids();
		Bitwidth* currentPre = NULL;
		if(1!=sizeis) {
			bool inChain = belongsToChain(pNode);
			multimap<Bitwidth*,TedNode*,_BitwidthOrder> toAdd;
			vector<Bitwidth*> toDelete;
			Bitwidth* iterPre = NULL;
			FOREACH_KID_OF_NODE(pNode) {
				unsigned int index = _iterkids.getIndex();
				//wedge weight = _iterkids.getWeight();
				TedNode* pkid = _iterkids.Node<TedNode>();
				if(0==index) {
					if(inChain) {
						continue;
					} else {
						addToChain(pkid,toAdd);
					}
				}
				iterPre = (*_iterkids).getBitwidth();
				toAdd.insert(pair<Bitwidth*,TedNode*>(iterPre,pkid));
			}
			Bitwidth* left = NULL;
			Bitwidth* right = NULL;
			while(!toAdd.empty()) {
				left = (*toAdd.begin()).first;
				toAdd.erase(toAdd.begin());
				if(!toAdd.empty()) {
					right = (*toAdd.begin()).first;
					toAdd.erase(toAdd.begin());
					Bitwidth* tmp = left->clone();
					tmp->add(right);
					toAdd.insert(pair<Bitwidth*,TedNode*>(tmp,(TedNode*) NULL));
					toDelete.push_back(tmp);
				}
			}
			currentPre = left->clone();
			while(!toDelete.empty()) {
				Bitwidth* todel = toDelete.back();
				delete todel;
				toDelete.pop_back();
			}
		} else {
			assert(1==sizeis);
			Bitwidth* uniquePre = pNode->getKids()->begin()->second.getBitwidth();
			currentPre = uniquePre->clone();
		}
		pNode->setBitwidth(currentPre);
		return currentPre;
	}

}
