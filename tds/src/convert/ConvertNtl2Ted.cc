/*
 * =====================================================================================
 *
 *       Filename:  ConvertNtl2Ted.cc
 *    Description:
 *        Created:  02/10/2010 12:22:27 PM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 205                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <iostream>
using namespace std;

#include "TedMan.h"
#include "TedNode.h"
#include "ATedNode.h"
#include "ETedNode.h"
#include "TedNodeRoot.h"
#include "TedVarMan.h"
using namespace ted;

#include "Ntk.h"
#include "Node.h"
using namespace network;

#include "ConvertNtl2Ted.h"

#include "Environment.h"
using namespace tds;


namespace convert {

	void ConvertNtl2Ted::translate(bool flag) {
		_evaluateConstantNodes=flag;
		translate();
	}
	void ConvertNtl2Ted::translate(void) {
		vector <Node*> vPos;
		size_t i;
		primaryInputs.clear();
		primaryConstants.clear();
		set <Node*>::iterator p;
		_nManNew = new Netlist((string(_nMan->getName())+ "_2ted").c_str());
		_nMan->getPoPis(&vPos);
		Mark<Node>::newMark();
		for(i = 0; i < vPos.size(); i++) {
			obtainPIs(vPos[i]);
		}
		
		if(!_evaluateConstantNodes) {
			for(p = primaryConstants.begin(); p != primaryConstants.end(); p++) {
				Node* node = (*p);
				_tMan->registerVar(node->getName(),node->_data.value);
			}
		}
		for(p = primaryInputs.begin(); p != primaryInputs.end(); p++) {
			_tMan->registerVar((*p)->getName());
		}
		
		for(i = 0; i < vPos.size(); i++) {
			assert(vPos[i]->isPO());
			collapseOtherNodes<ETedNode>(vPos[i]);
		}
		_tMan->recoverRegisterVars();
	}

	void ConvertNtl2Ted::obtainPIs(Node* pNode) {
		size_t i = 0;
		Node* pFanin = NULL;
		FOREACH_FANIN_OF_NODE(pNode, i, pFanin) {
			if(pFanin->visited.isMarked())
				continue;
			pFanin->visited.setMark();
			obtainPIs(pFanin);
			if(pNode->isArithmetic()&&(!pFanin->isArithmetic()&& !pFanin->isReg() && !pFanin->isSll())) {
				if(pFanin->isConst()) {
					primaryConstants.insert(pFanin);
				} else {
					primaryInputs.insert(pFanin);
				}
			}
		}
	}

	template<typename T>
		ATedNode* ConvertNtl2Ted::collapseRegisterNodes(Node* pNode,  set <Node*> & sFanins, int& amount) {
			assert(ATedNode::checkContainer(_tMan->getContainer()));
			assert(pNode->isReg());
			assert(pNode->numFanins() ==1);
			Node* pFanin = pNode->getFanin(0);
			ATedNode* pET = NULL;
			amount++;
			if(pFanin->isReg()) {
				pET = collapseRegisterNodes<T>(pFanin,sFanins,amount);
			} else if(pFanin->isArithmetic()) {
				pET = collapseArithmeticNodes<T>(pFanin,sFanins);
			} else if(pFanin->isSll()) {
				pET = collapseSllNodes<T>(pFanin,sFanins);
			} else {
				sFanins.insert(collapseOtherNodes<T>(pFanin));
				if(_evaluateConstantNodes && pFanin->isConst() ) {
					//if(_evaluateConstantNodes && pFanin->isConst()&& pFanin->_data.value != 0 && pFanin->_data.value != 1 && pFanin->_data.value != -1)
					pET = new T(pFanin->_data.value);
				} else {
					pET = new T(pFanin->getName());
				}
			}
			ATedNode* retimed = retimeCone<T>(pET,1);
			return retimed;
		}

	template<typename T>
		ATedNode* ConvertNtl2Ted::collapseSllNodes(Node* pNode,  set <Node*> & sFanins) {
			assert(ATedNode::checkContainer(_tMan->getContainer()));
			assert(pNode->isSll());
			assert(pNode->numFanins() ==2);
			Node* pFanin = pNode->getFanin(0);
			Node* pConst = pNode->getFanin(1);
			ATedNode* pET[2];
			if(pFanin->isReg()) {
				int amount = 0;
				ATedNode* retimed = collapseRegisterNodes<T>(pFanin,sFanins,amount);
				assert(amount!=0);
				pET[0]=retimed;
				//TedRegister retime = amount;
				//pET[0] = new T(*retimed->getVar(),*retimed->getKids(),retime);
			} else if(pFanin->isArithmetic()) {
				pET[0] = collapseArithmeticNodes<T>(pFanin,sFanins);
			} else if(pFanin->isSll()) {
				pET[0] = collapseSllNodes<T>(pFanin,sFanins);
			} else {
				sFanins.insert(collapseOtherNodes<T>(pFanin));
				if(_evaluateConstantNodes && pFanin->isConst() ) {
					pET[0] = new T(pFanin->_data.value);
				} else {
					pET[0] = new T(pFanin->getName());
				}
			}
			if (pConst->isConst()) {
				pET[1] = new T(pConst->_data.value);
			} else if(pConst->isSll()) {
				pET[1] = collapseSllNodes<T>(pConst,sFanins);
			} else if(pConst->isArithmetic()) {
				pET[1] = collapseArithmeticNodes<T>(pConst,sFanins);
			} else {
				//right hand operator of SLL is a non numeric value.
				assert(false);
			}
			assert(pET[1]->isConst());
			ATedNode* pow2 = new T(2);
			ATedNode* value = pow2->exp(pET[1]);
			ATedNode* pTNode = pET[0]->duplicateRecursive()->mul(value);
			return pTNode;
		}

	template<typename T>
		ATedNode* ConvertNtl2Ted::retimeCone(ATedNode* pNode,int retime) {
			if (pNode->isConst())
				return pNode;
			const TedVar* baseVar = NULL;
			int retime_amount = retime;
			if (pNode->getVar()->isRetimed()) {
				retime_amount += pNode->getVar()->getRegister();
				baseVar = pNode->getVar()->getBase();
			} else {
				baseVar = pNode->getVar();
			}
			const TedVar* retimedVar = TedVarMan::instance().createRetimedVar(*baseVar,retime_amount);
			_tMan->getContainer().registerRetimedVarAt(*retimedVar,*baseVar);
			TedKids* newKids = TedKids::create_kids();
			unsigned int index=0;
			ATedNode* pKid=NULL;
			wedge weight=0;
			int register_exists = 0;
			FOREACH_KID_OF_NODE(pNode){
				index = _iterkids.getIndex();
				pKid = _iterkids.Node<ATedNode>();
				weight = _iterkids.getWeight();
				register_exists = _iterkids.getRegister();
				assert(register_exists==0);
				ATedNode* newKidNode = retimeCone<T>(pKid,retime);
				newKids->insert(index,newKidNode,weight,register_exists);
			}
			ATedNode* newNode = new T(*retimedVar,*newKids,retime_amount);
			//delete newKids;
			return newNode;
		}

	template<typename T>
		ATedNode* ConvertNtl2Ted::collapseArithmeticNodes(Node* pNode,  set <Node*> & sFanins) {
			assert(ATedNode::checkContainer(_tMan->getContainer()));
			assert(pNode->isArithmetic());
			size_t i = 0;
			Node* pFanin=NULL;
			ATedNode* pET[2];
			int amount[2];
			pair <bool, long> tt;
			FOREACH_FANIN_OF_NODE(pNode, i, pFanin) {
				if(pFanin->isArithmetic()) {
					pET[i] = collapseArithmeticNodes<T>(pFanin, sFanins);
				} else if(pFanin->isReg()) {
					amount[i] = 0;
					ATedNode* retimed = collapseRegisterNodes<T>(pFanin, sFanins, amount[i]);
					assert(amount[i]!=0);
					TedRegister retime = amount[i];
#if 0
					pET[i] = retimeCone<T>(retimed,retime);
#else
					pET[i] = retimed;
#endif
				} else if(pFanin->isSll()) {
					pET[i] = collapseSllNodes<T>(pFanin,sFanins);
				} else {
					sFanins.insert(collapseOtherNodes<T>(pFanin));
					//cheby does not translate +/- 1
					if(_evaluateConstantNodes && pFanin->isConst() ) {
						pET[i] = new T(pFanin->_data.value);
					} else {
						pET[i] = new T(pFanin->getName());
					}
				}
			}
			ATedNode* pTNode = NULL;
			if(pNode->isAdd()) {
				pTNode = pET[0]->duplicateRecursive()->add(pET[1]->duplicateRecursive());
			} else if(pNode->isSub()) {
				pTNode = pET[0]->duplicateRecursive()->sub(pET[1]->duplicateRecursive());
			} else if(pNode->isMul()) {
				pTNode = pET[0]->duplicateRecursive()->mul(pET[1]->duplicateRecursive());
			} else if(pNode->isReg()) {
				//this should be resolved without reaching the PIs
				assert(0);
			} else {
				assert(0);
			}
			return pTNode;
		}

	template<typename T>
		Node* ConvertNtl2Ted::collapseOtherNodes(Node* pNode) {
			assert(ATedNode::checkContainer(_tMan->getContainer()));
			assert(!pNode->isArithmetic()&& !pNode->isReg());
			Node* pNewNode= _nManNew->get(pNode->getName());
			if(pNewNode != NULL) {
				return pNewNode;
			}
			pNewNode = new Node(pNode->getName(), pNode->getType());
			pNewNode->setFunc(pNode->func());
			set <Node*> sFanins;
			size_t i;
			Node* pFanin, *pNewFanin;
			ATedNode* pET;
			FOREACH_FANIN_OF_NODE(pNode, i, pFanin) {
				if(pFanin->isArithmetic()) {
					sFanins.clear();
					pET = collapseArithmeticNodes<T>(pFanin, sFanins);
					pNewFanin = pseudo_output(pET,pFanin,sFanins);
				} else if(pFanin->isReg()) {
					sFanins.clear();
					int amount = 0;
					ATedNode* retimed = collapseRegisterNodes<T>(pFanin, sFanins, amount);
					assert(amount!=0);
					pET = retimed;
					//TedRegister retime = amount;
					//pET = new T(*retimed->getVar(),*retimed->getKids(),retime);
					pNewFanin = pseudo_output(pET,pFanin,sFanins);
				} else if(pFanin->isSll()) {
					sFanins.clear();
					pET = collapseSllNodes<T>(pFanin,sFanins);
					pNewFanin = pseudo_output(pET,pFanin,sFanins);
				} else {
					pNewFanin = collapseOtherNodes<T>(pFanin);
				}
				pNewNode->addFanin(pNewFanin);
			}
			_nManNew->add(pNewNode);
			return pNewNode;
		}


	Node* ConvertNtl2Ted::pseudo_output(ATedNode* pET, Node* pFanin, set<Node*> & sFanins) {
		string newFaninName = pFanin->getName();
		Node* pNewFanin = new Node(newFaninName.c_str(), pFanin->getType());
		for(set <Node*>::iterator p = sFanins.begin(); p != sFanins.end(); p++) {
			pNewFanin->addFanin(_nManNew->get((*p)->getName()));
		}
		TedNodeRoot pin = _tMan->normalize(pET);
		_tMan->linkTedToPO(pin, pNewFanin->getName());
		pNewFanin->setData((void*)(pin.Node()));
		pNewFanin->setFunc(FUNC_TED);
		_nManNew->add(pNewFanin);
		return pNewFanin;
	}

}
