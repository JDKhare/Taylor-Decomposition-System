/*
 * =====================================================================================
 *
 *       Filename:  TedManOrder.cc
 *    Description:
 *        Created:  29/4/2009 10:27:16 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-11 21:46:11 +0100(Mon, 11 Jan 2010)$: Date of last commit
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

namespace ted {

	namespace {
		int negateTedNodesRec(TedNode* pNode, set <TedNode*> & sNodes) {
			unsigned int index=0;
			TedNode* pKid=NULL;
			int weight=0;
			bool bHighest=true;
			int nPhaseAll=1, nPhaseThis=0;

			FOREACH_KID_OF_NODE_REV(pNode) {
				index  = _iterkids.getIndex();
				pKid   = _iterkids.Node<TedNode>();
				weight = _iterkids.getWeight();
				nPhaseThis = negateTedNodesRec(pKid, sNodes);
				nPhaseThis*= sNodes.find(pKid) == sNodes.end() ? 1:-1;
				if(bHighest && nPhaseThis == -1) {
					nPhaseAll = -1;
					continue;
				}
				pNode->setKidWeight(index, weight* nPhaseAll* nPhaseThis);
				if(bHighest)bHighest = false;
			}
			return nPhaseAll;
		}

		void negateTedNodes(TedMan::PrimaryOutputs&_mPos, set <TedNode*> & sNodes) {
			TedMan::PrimaryOutputs::iterator mp;
			int i;
			for(mp = _mPos.begin(); mp != _mPos.end(); mp++) {
				i = negateTedNodesRec(mp->second.Node(), sNodes);
				mp->second.setWeight(mp->second.getWeight()*i);
			}
		}
	}

	TedMan* TedOrderSwap::pushUp(const TedVar& var, unsigned int distance) {
		assert(distance>0);
		TedMan* pMan = _pManCurrent->duplicate();
		updateCurrentManager(pMan);
		for(unsigned int jump = 1; jump <= distance; jump++) {
			TedMan* pManNext = pushUp(var);
			if (pManNext) {
				delete pMan;
				pMan = pManNext;
			}
		}
		return pMan;
	}

	TedMan* TedOrderSwap::pushUp(const TedVar& var) {
		TedMan* pMan = NULL;
		if(!_pManCurrent->getContainer().isAtTop(var)) {
			unsigned int level = _pManCurrent->getContainer().getLevel(var)+1;
			const TedVar& varUp = _pManCurrent->getContainer().getVarAtLevel(level);
			pMan = swapVariable(varUp, var);
			updateCurrentManager(pMan);
		}
		return pMan;
	}

	TedMan* TedOrderSwap::pushDown(const TedVar& var) {
		TedMan* pMan = NULL;
		if(!_pManCurrent->getContainer().isAtBottom(var)) {
			unsigned int level = _pManCurrent->getContainer().getLevel(var)-1;
			const TedVar& varDown = _pManCurrent->getContainer().getVarAtLevel(level);
			pMan = swapVariable(var, varDown);
			updateCurrentManager(pMan);
		}
		return pMan;
	}

	TedMan* TedOrderSwap::pushDown(const TedVar& var, unsigned int distance) {
		assert(distance>0);
		TedMan* pMan = _pManCurrent->duplicate();
		updateCurrentManager(pMan);
		for(unsigned int jump = 1; jump <= distance; jump++) {
			TedMan* pManNext = pushDown(var);
			if (pManNext) {
				delete pMan;
				pMan = pManNext;
			}
		}
		return pMan;
	}

	TedMan* TedOrderSwap::swapVariable(const TedVar& a, const TedVar& b) {
		unsigned int level = _pManCurrent->getContainer().getLevel(a);
		if(level == 1) {
			return NULL;
		}
		map <TedNode*, ATedNode*> mNodes;
		set <TedNode*>::iterator p;
		TedMan* pManNew = NULL;
		TedMan* pManTemp = new TedMan();
		buildATed(mNodes, a, b, pManTemp);
		normalizeATed(mNodes, pManTemp);
		pManNew = duplicateSwapping(a, b, pManTemp);
		delete pManTemp;
		return pManNew;
	}


	/** @brief used for normalization of ETed nodes during reordering*/
	void TedOrderSwap::buildATed(Ted2ATed & mNodesA, const TedVar& va, const TedVar& vb, TedMan* pManTemp) {
		assert(ATedNode::checkContainer(pManTemp->getContainer()));
		set <TedNode*>* sNodes;
		set <TedNode*>::iterator sp;
		unsigned int index=0;
		TedNode* pNode=NULL,*pKid=NULL;
		int weight=0;
		ATedNode* pANodeKid=NULL,* pANode=NULL;
		Ted2ATed mNodesB;

		/* Register Variables*/
		pManTemp->getContainer().registerVar(vb);
		pManTemp->getContainer().registerVar(va);

		/*for variable b which is lower than a*/
		sNodes = _pManCurrent->getContainer()[vb].second;

		for(sp = sNodes->begin(); sp != sNodes->end(); sp ++) {
			pNode =*sp;
			pANode = new ETedNode(0);
			FOREACH_KID_OF_NODE(pNode) {
				index = _iterkids.getIndex();
				pKid = _iterkids.Node<TedNode>();
				weight = _iterkids.getWeight();
				pANodeKid = new ETedNode(Util::itoa((long)pKid).c_str());
				pANodeKid = pANodeKid->mul(new ETedNode(weight));

				for(unsigned int i= 0; i < index; i++) {
					pANodeKid = pANodeKid->mul(new ETedNode(pNode->getName()));
				}
				pANode = pANode->add(pANodeKid);
			}
			mNodesB[pNode] = pANode;
		}

		/*for variable a which is higher than b*/
		sNodes = _pManCurrent->getContainer()[va].second;

		for(sp = sNodes->begin(); sp != sNodes->end(); sp ++) {
			pNode =*sp;
			pANode = new ETedNode(0);
			FOREACH_KID_OF_NODE(pNode) {
				index = _iterkids.getIndex();
				pKid = _iterkids.Node<TedNode>();
				weight = _iterkids.getWeight();
				//ok, iterator used for checking existance
				Ted2ATed::iterator mp = mNodesB.find(pKid);
				if(mp == mNodesB.end()) {
					pANodeKid = new ETedNode(Util::itoa((long)pKid).c_str());
				} else {
					pANodeKid = mp->second->duplicateRecursive();
				}
				ATedNode* pTemp = new ETedNode(weight);
				pANodeKid = pANodeKid->mul(pTemp);
				for(unsigned int i = 0; i < index; i++) {
					pANodeKid = pANodeKid->mul(new ETedNode(pNode->getName()));
				}
				pANode = pANode->add(pANodeKid);
			}
			mNodesA[pNode] = pANode;
		}
	}

	TedNode* TedOrderSwap::normalizeATedNode(TedNode* pOrigNode, ATedNode* pANode, TedMan* pManTemp) {
		pANode = pManTemp->_PropogateZero_Rec(pANode);
		pair<TedNode*, int> pe = pManTemp->_Normalize_Rec(pANode);
		TedNode* pNode = pe.first;
		assert(pe.second == 1 || pe.second == -1);
		pManTemp->linkTedToPO(pe.first, pe.second, Util::itoa((long)pOrigNode));
		return pNode;
	}

	void TedOrderSwap::normalizeATed(Ted2ATed & mNodes,  TedMan* pManTemp) {
		for(Ted2ATed::iterator p = mNodes.begin(); p!= mNodes.end(); p++) {
			normalizeATedNode(p->first, p->second, pManTemp);
		}
	}


	TedNode* TedOrderSwap::reorderVariableRec(TedNode* pNodeTemp, const TedVar & va, const TedVar &  vb, set <TedNode*>* sA, set <TedNode*>* sB, map <TedNode*, TedNode*> & mVisited) {
		TedNode* pNode=NULL,* pKid=NULL,* pKidTemp=NULL;
		TedKids tKids;
		unsigned int index=0;
		int weight=0;
		pair<bool, long> tt;
		map <TedNode*, TedNode*>::iterator mp;
		set <TedNode*>::iterator sp;

		if((mp = mVisited.find(pNodeTemp))!= mVisited.end()) {
			return mp->second;
		}
		tt = Util::atoi(pNodeTemp->getName().c_str());
		if(tt.first == true) {
			return(TedNode*)tt.second;
		}
		FOREACH_KID_OF_NODE(pNodeTemp) {
			index = _iterkids.getIndex();
			pKidTemp = _iterkids.Node<TedNode>();
			weight = _iterkids.getWeight();
			pKid = reorderVariableRec(pKidTemp, va, vb, sA, sB, mVisited);
			tKids.insert(index, pKid, weight,TedRegister(0));
		}
		pNode = NULL;
		if(pNodeTemp->getVar()->equal(va)) {
			for(sp = sA->begin(); sp != sA->end(); sp++) {
				if((*sp)->getKids()->equal(tKids)) {
					pNode =*sp;
					break;
				}
			}
			if(pNode == NULL) {
				//pNode = new TedNode(*pNodeTemp->getVar(), tKids);
				pNode = new ETedNode(*pNodeTemp->getVar(), tKids, pNodeTemp->getRegister());
				sA->insert(pNode);
			}
		} else if(pNodeTemp->getVar()->equal(vb)) {
			for(sp = sB->begin(); sp != sB->end(); sp++) {
				if((*sp)->getKids()->equal(tKids)) {
					pNode =*sp;
					break;
				}
			}
			if(pNode == NULL) {
				//pNode = new TedNode(*pNodeTemp->getVar(), tKids);
				pNode = new ETedNode(*pNodeTemp->getVar(), tKids, pNodeTemp->getRegister());
				sB->insert(pNode);
			}
		} else {
			assert(0);
		}
		mVisited[pNodeTemp] = pNode;
		return pNode;
	}

	TedMan* TedOrderSwap::reorderVariable(const TedVar & va, const TedVar & vb, TedMan* pManTemp) {
		TedContainer::iterator msp;
		set <TedNode*>  sNegatedNodes;
		set <TedNode*>* sNodesA,* sNodesB;
		set <TedNode*>* sNewA,* sNewB;
		set <TedNode*> sNodesTemp2;
		set <TedNode*>::iterator sp;
		unsigned int    LevelA, LevelB;
		TedNode* pNode,* pNodeNew;
		pair<bool, long> tt;
		map <TedNode*, TedNode*> mNodesRev, mVisited;
		map <TedNode*, TedNode*>::iterator mp;
		TedKids tKids;
		map <TedNode*, TedNode*> mSecondLevelNodes;
		msp = _pManCurrent->getContainer().find(va);
		LevelA  = msp->second.first;
		sNodesA = msp->second.second;
		_pManCurrent->getContainer().erase(msp);
		msp = _pManCurrent->getContainer().find(vb);
		LevelB  = msp->second.first;
		sNodesB = msp->second.second;
		_pManCurrent->getContainer().erase(msp);
		sNewA = new set <TedNode*>;
		sNewB = new set <TedNode*>;
		for(sp = sNodesA->begin(); sp != sNodesA->end(); sp++) {
			sNewA->insert(*sp);
		}
		for(sp = sNodesB->begin(); sp != sNodesB->end(); sp++) {
			sNewB->insert(*sp);
		}
		for(TedMan::PrimaryOutputs::iterator mp = pManTemp->getPOs().begin(); mp!= pManTemp->getPOs().end(); mp++) {
			pNode = reorderVariableRec(mp->second.Node(), va, vb, sNewA, sNewB, mVisited);
			pNodeNew = (TedNode*)(mp->first.c_str());
			tt = Util::atoi(mp->first.c_str());
			assert(tt.first);
			if((sp = sNodesA->find((TedNode*)(tt.second)))!= sNodesA->end()) {
				(*sp)->setVar(*pNode->getVar());
				//(*sp)->_tKids = NULL;
				(*sp)->setKids(*pNode->getKids());
				sNewA->erase(pNode);
				sNewA->insert(*sp);
			} else if((sp = sNodesB->find((TedNode*)(tt.second)))!= sNodesB->end()) {
				(*sp)->setVar(*pNode->getVar());
				//(*sp)->_tKids = NULL;
				(*sp)->setKids(*pNode->getKids());
				sNewB->erase(pNode);
				sNewB->insert(*sp);
			} else {
				assert(0);
			}
			if(mp->second.getWeight() == -1) {
				sNegatedNodes.insert(*sp);
			}
		}
		_pManCurrent->getContainer()[vb] = pair <unsigned int, set <TedNode*>*>(LevelA, sNewB);
		_pManCurrent->getContainer()[va] = pair <unsigned int, set <TedNode*>*>(LevelB, sNewA);
		delete sNodesA;
		delete sNodesB;
		negateTedNodes(_pManCurrent->getPOs(), sNegatedNodes);
		return _pManCurrent->duplicate();
	}

	TedNodeRoot TedOrderSwap::duplicateSwappingRec(TedNode* pNode, const TedVar & va, const TedVar & vb, TedMan* pManNew, TedMan* pManTemp, int iState, map <TedNode*, TedNodeRoot> & mVisited) {
		unsigned int index = 0;
		int weight = 0;
		int nPhaseThisNode = 1;
		int nPhaseThisKid = 0;
		int nPhaseAllKid = 1;
		TedNode*pNodeNew = NULL,* pKid = NULL;
		TedKids tKids;
		pair<bool, long> tt;
		set <TedNode*>* sNodes = NULL;
		bool bHighest = true;
		TedNodeRoot pin;
		map <TedNode*, TedNodeRoot>::iterator mp;
		TedMan::PrimaryOutputs::iterator mpPo;
		if(iState == 0) {
			if((mpPo = pManTemp->getPOs().find(Util::itoa((long)(pNode))))!= pManTemp->getPOs().end()) {
				pNode = mpPo->second.Node();
				iState = 1;
				nPhaseThisNode = mpPo->second.getWeight();
			}
		} else if(iState == 1) {
			tt = Util::atoi(pNode->getName().c_str());
			if(tt.first) {
				pNode = (TedNode*)tt.second;
				iState = 2;
			}
		}
		if((mp=mVisited.find(pNode))!= mVisited.end())return mp->second;
		if(pNode == TedNode::getOne())
			return  TedNodeRoot(pNode, 1);
		FOREACH_KID_OF_NODE_REV(pNode) {
			index  = _iterkids.getIndex();
			pKid   = _iterkids.Node<TedNode>();
			weight = _iterkids.getWeight();
			pin = duplicateSwappingRec(pKid, va, vb, pManNew, pManTemp, iState, mVisited);
			nPhaseThisKid = pin.getWeight();
			if(bHighest && nPhaseThisKid == -1) {
				nPhaseAllKid = -1;
				nPhaseThisKid = 1;
			} else {
				weight = weight* nPhaseThisKid* nPhaseAllKid;
			}
			tKids.insert(index, pin.Node(), weight,TedRegister(0));
			bHighest = false;
		}
		sNodes = pManNew->getContainer()[*pNode->getVar()].second;
		if(pNode->getVar()->equal(va)||(pNode->getVar()->equal(vb))) {
			pNodeNew = NULL;
			for(set <TedNode*>::iterator sp = sNodes->begin(); sp != sNodes->end(); sp++) {
				if((*sp)->getKids()->equal(tKids)) {
					pNodeNew =*sp;
					break;
				}
			}
			if(pNodeNew == NULL) {
				//pNodeNew  = new TedNode(*pNode->getVar(), tKids);
				pNodeNew  = new ETedNode(*pNode->getVar(), tKids, pNode->getRegister());
				sNodes->insert(pNodeNew);
			}
		} else {
			//pNodeNew  = new TedNode(*pNode->getVar(), tKids);
			pNodeNew  = new ETedNode(*pNode->getVar(), tKids, pNode->getRegister());
			sNodes->insert(pNodeNew);
		}
		pin = TedNodeRoot(pNodeNew, nPhaseAllKid* nPhaseThisNode);
		mVisited[pNode] = pin;
		return pin;
	}

	TedMan* TedOrderSwap::duplicateSwapping(const TedVar& tva, const TedVar& tvb, TedMan* pManTemp) {
		TedMan* pManNew = NULL;
		int istate = 0;
		TedContainer::iterator msp;
		map <TedNode*, TedNodeRoot> mVisited;
		TedNodeRoot pin;

		pManNew = new TedMan();
		for(msp = _pManCurrent->getContainer().begin(); msp != _pManCurrent->getContainer().end(); msp++) {
			pManNew->getContainer()[msp->first] =
				pair<unsigned int, set <TedNode*>*>(msp->second.first, new set <TedNode*>);
		}
		pManNew->getContainer()[tva].first ++;
		pManNew->getContainer()[tvb].first --;
		for(TedMan::PrimaryOutputs::iterator mp = _pManCurrent->getPOs().begin(); mp != _pManCurrent->getPOs().end(); mp++) {
			istate = 0;
			pin = duplicateSwappingRec(mp->second.Node(), tva, tvb, pManNew, pManTemp, istate, mVisited);
			pin.setType(mp->second.getType());
			pin.setWeight(pin.getWeight()*mp->second.getWeight());
			pin.setRegister(mp->second.getRegister());
			pManNew->linkTedToPO(pin, mp->first);
			//pManNew->linkTedToPO(pin.Node(), pin.getWeight()* mp->second.getWeight(), mp->first.c_str());
		}
		return pManNew;
	}

}
