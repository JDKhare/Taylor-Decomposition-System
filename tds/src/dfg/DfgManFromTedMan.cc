/*
 * =====================================================================================
 *
 *       Filename:  DfgMan.c
 *    Description:  DfgMan class
 *        Created:  05/01/2007 04:30:18 AM EDT
 *         Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 17:43:11 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */


#include <cassert>

#include "DfgMan.h"
#include "DfgNode.h"

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
using namespace ted;

#include "util.h"
using util::Util;

namespace dfg {
#if 0
	namespace {
		DfgNode * _GetDfgNode (DfgMan * pMan, const char * var, DfgOperator::Type op = DfgOperator::VAR, bool bReset = false) {
			static map <string, DfgNode *> mDfgNodes;
			DfgNode * pNode;
			map<string, DfgNode *>::iterator p;
			if (bReset) {
				mDfgNodes.clear();
				return NULL;
			}
			if ( (p = mDfgNodes.find(var)) != mDfgNodes.end()) {
				return p->second;
			} else {
				pNode = new DfgNode (pMan, var, op);
				mDfgNodes.insert(pair<string, DfgNode *>(var, pNode));
				return pNode;
			}
		}

		DfgNode * _GetDfgNodeConst (DfgMan * pMan, long var, bool bReset = false) {
			static map <long, DfgNode *> mDfgNodes;
			DfgNode * pNode;
			map<long, DfgNode *>::iterator p;
			if (bReset) {
				mDfgNodes.clear();
				return NULL;
			}
			if ( (p = mDfgNodes.find(var)) != mDfgNodes.end()) {
				return p->second;
			} else {
				pNode = new DfgNode (pMan, var);
				mDfgNodes.insert(pair<long, DfgNode *>(var, pNode));
				return pNode;
			}
		}

		void _factorTedNode (
							 DfgMan * pDfgMan,
							 const TedNode * pNode,
							 map <const TedNode *, Ted2Dfg > & mTedMFactors,
							 Ted2Dfg& mFactors) {
			DfgNode * bTemp;
			TedNode * pKid = NULL;
			int weight = 0;
			unsigned int index = 0;
			FOREACH_KID_OF_NODE(pNode) {
				index = _iterkids.getIndex();
				pKid =_iterkids.Node<TedNode>();
				weight =_iterkids.getWeight();
				//bTemp = new DfgNode(pDfgMan, weight);
				bTemp = _GetDfgNodeConst(pDfgMan, weight);
				if (index == 0) {
					assert(mTedMFactors.find(pKid) != mTedMFactors.end());
					mFactors = mTedMFactors[pKid];
					if (mFactors.size() == 0) {
						assert (pKid == TedNode::getOne());
						mFactors.insert(pair<const TedNode *, DfgNode *>(TedNode::getOne(),bTemp));
					} else {
						//The operation in the container does not depend in the order of the iteration
						//so we can use any iteration here, even ordered by ptr location (see bug 106)
						for (Ted2Dfg::iterator p = mFactors.begin(); p != mFactors.end(); p++) {
							p->second = p->second->mul(bTemp);
						}
					}
					continue;
				}
				const char* var_name;
				TedVar* var_node = TedVarMan::instance().getIfExist(pNode->getName());
				DfgOperator::Type op = DfgOperator::VAR;
				if (var_node->isVarConst())
					op = DfgOperator::VARCONST;
#ifndef NDEBUG
				//a valid assumption is that the TedManager is built and stable,
				//therefore all pNodes have the real TedVar.
				assert(var_node==pNode->getVar());
#endif
				assert(NULL!=var_node);
				if (var_node->isMember()) {
					const string& var_name_str = (dynamic_cast<TedVarGroupMember*>(var_node))->getOwner()->getName();
					var_name = var_name_str.c_str();
				} else {
					const string& var_name_str = var_node->getName();
					var_name = var_name_str.c_str();
				}
				for (unsigned int j = 0; j < index; j++) {
					bTemp = bTemp->mul(_GetDfgNode(pDfgMan, var_name,op));
					//bTemp = bTemp->mul(_GetDfgNode(pDfgMan, pNode->getName()));
				}
				//the iterator is only used to check existence, so its safe to use it.
				Ted2Dfg::iterator q = mFactors.find(pKid);
				if (q!=mFactors.end()) {
					//q->second = (q->second)->add(bTemp);
					q->second = bTemp->add(q->second);
				} else {
					mFactors.insert(pair<const TedNode *, DfgNode *>(pKid, bTemp ));
				}
			}
			mTedMFactors[pNode] = mFactors;
			//return mFactors;
		}

	}
#endif

	void DfgMan::_relink_dfactored_node_rec(DfgNode * pNode, set <string> & sPseudoPos) {
		DfgNode * pFanins[2], * pNewFanins[2];
		map <string, DfgNode *>::iterator p;
		if (!pNode->isOp() || pNode->_visited.isMarked()) {
			return;
		}
		pFanins[0] = pNode->getLeft();
		pFanins[1] = pNode->getRight();
		assert(pFanins[0]);
		assert(pFanins[1]);
		for (unsigned int i = 0; i < 2; i++) {
			pNewFanins[i] = pFanins[i];
			if (pFanins[i]->isPI()) {
				p = _mPos.find(pFanins[i]->getName());
				if (p != _mPos.end()) {
					pNewFanins[i] = p->second;
					sPseudoPos.insert(p->first);
					_relink_dfactored_node_rec(pNewFanins[i], sPseudoPos);
				}
			} else {
				_relink_dfactored_node_rec(pFanins[i], sPseudoPos);
			}
		}
		pNode->setKids(pNewFanins[0], pNewFanins[1]);
		pNode->_visited.setMark();
	}

	void DfgMan::relink_pseudo_factors(void) {
		map <string, DfgNode *>::iterator p,k;
		set <string>::iterator p2;
		//do {
		set <string> sPseudoPos;
		Mark<DfgNode>::newMark();
		for (p = _mPos.begin(); p!= _mPos.end(); p++) {
			DfgNode* candidate = p->second;
			if (candidate->isOp()) {
				_relink_dfactored_node_rec(candidate, sPseudoPos);
			} else if (candidate->isPI() || candidate->isVarConst()) {
				k = _mPos.find(candidate->getName());
				if (k != _mPos.end() ) {
					sPseudoPos.insert(k->first);
				}
			} else {
				assert(candidate->isConst());
			}
		}
		for (p2 = sPseudoPos.begin(); p2 != sPseudoPos.end(); p2++) {
			if ((p = _mPos.find(*p2))!= _mPos.end()) {
				_mPos.erase(p);
			}
		}
		//} while (sPseudoPos.size() != 0);
		//these do loop can be used as precaution to iterate through the network
		//in case the recursive function does not find all Pis and Pos with the same name
		//, which is called PseudoPos
	}

#if 0
	/** @brief The function to construct Dfg manager from a Ted Manager */
	DfgMan::DfgMan(TedMan * pTedMan, bool showDfgFactors) {
		assert(pTedMan);
		Ted2Dfg mExpressions;
		Ted2Dfg mCoFactors;
		DfgNode * expr;
		map<const TedNode*, Ted2Dfg > mTedMFactors;
		_GetDfgNode(this, "", DfgOperator::VAR, true);
		_GetDfgNodeConst(this, 0, true);
		mTedMFactors[TedNode::getOne()] = mCoFactors;
		mExpressions.insert(pair<const TedNode*, DfgNode* >(TedNode::getOne(), new DfgNode(this, 1)));
		for (TedMan::dfs_iterator it = pTedMan->dfs_begin(), end = pTedMan->dfs_end(); it != end; it++) {
			const TedNode* pNode = *it;
			pTedMan->write(pNode);
			if (pNode  == TedNode::getOne())
				continue;
			mCoFactors.clear();
			_factorTedNode(this, pNode, mTedMFactors, mCoFactors);
			expr = NULL;
			for (Ted2Dfg::list_reverse_iterator q = mCoFactors.list_rbegin(); q!= mCoFactors.list_rend(); q++) {
				const TedNode* tnode = *q;
				DfgNode* right_dnode = mExpressions[tnode];
				DfgNode* left_dnode = mCoFactors[tnode];
				if (expr == NULL) {
					expr = left_dnode->mul(right_dnode);
				} else {
					expr = left_dnode->mul(right_dnode)->add(expr);
				}
			}
			mExpressions.insert(pair<const TedNode *, DfgNode * >(pNode, expr));

		}
		for (TedMan::PrimaryOutputs::iterator p = pTedMan->_pos.begin(); p != pTedMan->_pos.end();p++) {
			_mPos[p->first] = mExpressions[p->second.Node()]->mul(_GetDfgNodeConst(this, p->second.getWeight()));
		}
		if (!showDfgFactors) {
			_relink_dfactored_nodes_from_ted_dfactor();
			strash();
		}
		DfgNode::setEnvOpDelays();
		updateDelayAndNumRefs();
		updateRequiredTime();
	}
#endif

}
