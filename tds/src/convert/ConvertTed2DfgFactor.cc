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
 *  $Revision:: 122                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-11 21:46:11 +0100 (Mon, 11 Jan 2010)     $: Date of last commit
 * =====================================================================================
 */


#include <cassert>
#include <map>
#include <string>
using namespace std;

#include "DfgMan.h"
#include "DfgNode.h"
using namespace dfg;

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
using namespace ted;

#include "util.h"
using util::Util;

#include "ConvertTed2DfgFactor.h"
namespace convert {

	DfgNode * ConvertTed2DfgFactor::getDfgNode(const char * var, DfgOperator::Type op) {
		map<string, DfgNode *>::iterator p;
		if ( (p = _mapDfg.find(var)) != _mapDfg.end()) {
			return p->second;
		} else {
			DfgNode* pNode = new DfgNode (_dMan, var, op);
			_mapDfg.insert(pair<string, DfgNode *>(var, pNode));
			return pNode;
		}
	}

	DfgNode * ConvertTed2DfgFactor::getDfgNode(long var) {
		map<long, DfgNode *>::iterator p;
		if ( (p = _mapDfgConst.find(var)) != _mapDfgConst.end()) {
			return p->second;
		} else {
			DfgNode* pNode = new DfgNode (_dMan, var);
			_mapDfgConst.insert(pair<long, DfgNode *>(var, pNode));
			return pNode;
		}
	}

#if 0
	void ConvertTed2DfgFactor::factorTedNode ( const TedNode * pNode, ListMapDfgTed& mFactors) {
		DfgNode * bTemp;
		TedNode * pKid = NULL;
		int weight = 0;
		unsigned int index = 0;
		FOREACH_KID_OF_NODE(pNode) {
			index = _iterkids.getIndex();
			pKid =_iterkids.Node<TedNode>();
			weight =_iterkids.getWeight();
			bTemp = getDfgNode(weight);
			if (index == 0) {
				assert(_mapDfgFactors.find(pKid) != _mapDfgFactors.end());
				mFactors = _mapDfgFactors[pKid];
				if (mFactors.size() == 0) {
					assert (pKid == TedNode::getOne());
					mFactors.insert(pair<const TedNode *, DfgNode *>(TedNode::getOne(),bTemp));
				} else {
					//The operation in the container does not depend in the order of the iteration
					//so we can use any iteration here, even ordered by ptr location (see bug 106)
					for (ListMapDfgTed::iterator p = mFactors.begin(); p != mFactors.end(); p++) {
						p->second = p->second->mul(bTemp);
					}
				}
				continue;
			}
			const char* var_name;
			TedVar* var_node = TedVarMan::instance().getIfExist(pNode->getName());
			DfgOperator::Type op = DfgOperator::VAR;
			if (var_node->isVarConst()) {
				op = DfgOperator::VARCONST;
			}
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
				bTemp = bTemp->mul(getDfgNode(var_name,op));
				//bTemp = bTemp->mul(getDfgNode(pNode->getName()));
			}
			//the iterator is only used to check existence, so its safe to use it.
			ListMapDfgTed::iterator q = mFactors.find(pKid);
			if (q!=mFactors.end()) {
				//q->second = (q->second)->add(bTemp);
				q->second = bTemp->add(q->second);
			} else {
				mFactors.insert(pair<const TedNode *, DfgNode *>(pKid, bTemp ));
			}
		}
		_mapDfgFactors[pNode] = mFactors;
	}

	/** @brief The function to construct Dfg manager from a Ted Manager */
	void ConvertTed2DfgFactor::translate(void) {
		assert(_tMan);
		ListMapDfgTed mExpressions;
		ListMapDfgTed mCoFactors;
		_mapDfgFactors[TedNode::getOne()] = mCoFactors;
		mExpressions.insert(pair<const TedNode*, DfgNode* >(TedNode::getOne(), new DfgNode(_dMan, 1)));
		long test = 0;
		for (TedMan::dfs_iterator it = _tMan->dfs_begin(), end = _tMan->dfs_end(); it != end; it++, test++) {
			const TedNode* pNode = *it;
			//_tMan->write(pNode);
			if (pNode  == TedNode::getOne())
				continue;
			mCoFactors.clear();
			factorTedNode(pNode, mCoFactors);
			DfgNode* expr = NULL;
			for (ListMapDfgTed::list_reverse_iterator q = mCoFactors.list_rbegin(); q!= mCoFactors.list_rend(); q++) {
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
			if (test==3743)
				cout << test << endl;
			/* 		if (test>3743)
					throw(string("stop"));
					*/
		}
		for (TedMan::PrimaryOutputs::iterator p = _tMan->_pos.begin(); p != _tMan->_pos.end();p++) {
			DfgNode* dNode = getDfgNode(p->second.getWeight());
			_dMan->_mPos[p->first] = mExpressions[p->second.Node()]->mul(dNode);
		}
	}

#else

	void ConvertTed2DfgFactor::factorTedNode ( const TedNode * pNode) {
		if (_mapDfgFactors.find(pNode)!=_mapDfgFactors.end()) {
			return;
		}
		DfgNode * bTemp;
		TedNode * pKid = NULL;
		int weight = 0;
		unsigned int index = 0;
		ListMapDfgTed mFactors;
		FOREACH_KID_OF_NODE(pNode) {
			index = _iterkids.getIndex();
			pKid =_iterkids.Node<TedNode>();
			weight =_iterkids.getWeight();
			bTemp = getDfgNode(weight);
			if (index == 0) {
				assert(_mapDfgFactors.find(pKid) != _mapDfgFactors.end());
				mFactors = _mapDfgFactors[pKid];
				if (mFactors.size() == 0) {
					assert (pKid == TedNode::getOne());
					mFactors.insert(pair<const TedNode *, DfgNode *>(TedNode::getOne(),bTemp));
				} else {
					//The operation in the container does not depend in the order of the iteration
					//so we can use any iteration here, even ordered by ptr location (see bug 106)
					for (ListMapDfgTed::iterator p = mFactors.begin(); p != mFactors.end(); p++) {
						p->second = p->second->mul(bTemp);
					}
				}
				continue;
			}
			const char* var_name;
			TedVar* var_node = TedVarMan::instance().getIfExist(pNode->getName());
			DfgOperator::Type op = DfgOperator::VAR;
			if (var_node->isVarConst()) {
				op = DfgOperator::VARCONST;
			}
#ifndef NDEBUG
			//a valid assumption is that the TedManager is built and stable,
			//therefore all pNodes have the real TedVar.
			assert(var_node==pNode->getVar());
#endif
			assert(NULL!=var_node);
#if 1
			var_name = var_node->getRootName().c_str();
#else
			if (var_node->isMember()) {
				const string& var_name_str = (dynamic_cast<TedVarGroupMember*>(var_node))->getOwner()->getName();
				var_name = var_name_str.c_str();
			} else {
				const string& var_name_str = var_node->getName();
				var_name = var_name_str.c_str();
			}
#endif
			for (unsigned int j = 0; j < index; j++) {
				bTemp = bTemp->mul(getDfgNode(var_name,op));
			}
			//the iterator is only used to check existence, so its safe to use it.
			ListMapDfgTed::iterator q = mFactors.find(pKid);
			if (q!=mFactors.end()) {
				//q->second = (q->second)->add(bTemp);
				q->second = bTemp->add(q->second);
			} else {
				mFactors.insert(pair<const TedNode *, DfgNode *>(pKid, bTemp ));
			}
		}
		_mapDfgFactors[pNode] = mFactors;
	}

	/** @brief The function to construct Dfg manager from a Ted Manager */
	void ConvertTed2DfgFactor::translate(void) {
		assert(_tMan);
		ListMapDfgTed mExpressions;
		_mapDfgFactors.insert(pair<const TedNode*,ListMapDfgTed>(TedNode::getOne(), ListMapDfgTed()));
		mExpressions.insert(pair<const TedNode*, DfgNode* >(TedNode::getOne(), new DfgNode(_dMan, 1)));
		long test = 0;
		for (TedMan::dfs_iterator it = _tMan->dfs_begin(), end = _tMan->dfs_end(); it != end; it++, test++) {
			const TedNode* pNode = *it;
			//_tMan->write(pNode);
			if (pNode  == TedNode::getOne())
				continue;
			factorTedNode(pNode);
			DfgNode* expr = NULL;
			ListMapDfgTed& mCoFactors = _mapDfgFactors[pNode];
			for (ListMapDfgTed::list_reverse_iterator q = mCoFactors.list_rbegin(); q!= mCoFactors.list_rend(); q++) {
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
			/* 		if (test>3743)
					throw(string("stop"));
					*/
		}
		for (TedMan::PrimaryOutputs::iterator p = _tMan->_pos.begin(); p != _tMan->_pos.end();p++) {
			DfgNode* dNode = getDfgNode(p->second.getWeight());
			_dMan->_mPos[p->first] = mExpressions[p->second.Node()]->mul(dNode);
		}
	}

#endif

}
