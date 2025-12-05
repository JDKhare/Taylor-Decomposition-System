/*
 * =====================================================================================
 *
 *       Filename:  ConvertDfg2Ted.cc
 *    Description:
 *        Created:  01/11/2010 02:31:41 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 208                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-09 04:42:44 -0400 (Mon, 09 Jul 2012)     $: Date of last commit
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
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
using namespace dfg;

#include "util.h"
using namespace util;

#include "ConvertDfg2Ted.h"
namespace convert {

#if 0
	void ConvertDfg2Ted::translate(void) {
		assert(ATedNode::checkContainer(_tMan->getContainer()));
		Mark<DfgNode>::newMark();
		for (DfgMan::PrimaryOutputs::iterator p = _dMan->_mPos.begin(); p != _dMan->_mPos.end(); p++) {
			ATedNode * pANode = translate(p->second);
			TedNodeRoot pin(_tMan->normalize(pANode));
			_tMan->linkTedToPO(pin.Node(), pin.getWeight(), p->first);
		}
	}

	ATedNode * ConvertDfg2Ted::translate(DfgNode * pNode) {
		//	cout << "ID=" << pNode->getOperator()->getID() << ",";
		if (pNode->isMarked()) {
			return mVisited[pNode]->duplicateRecursive();
			//		return mVisited[pNode];
		}
		ATedNode* pANode = NULL;
		if (pNode->isConst()) {
			pANode = new ETedNode(pNode->getConst());
		} else if (pNode->isPI() || pNode->isVarConst()) {
			pANode = new ETedNode(pNode->getName());
		} else {
			assert(pNode->isOp());
			ATedNode* pELeft = translate(pNode->getLeft());
			ATedNode* pERight= translate(pNode->getRight());
			if (pNode->isAdd()) {
				pANode = pELeft->add(pERight);
			} else if (pNode->isSub()) {
				pANode = pELeft->sub(pERight);
			} else if (pNode->isMul()) {
				pANode = pELeft->mul(pERight);
			} else if (pNode->isLSH()) {
				assert(pERight->isConst());
				wedge val = 2;
				wedge exponent = pERight->getConst();
				for (int i = 1; i < exponent; i++) {
					val *= 2;
				}
				pERight->setConst(val);
				pANode = pELeft->mul(pERight);
			}
		}
		pNode->setMark();
		mVisited[pNode] = pANode;
		return pANode;
	}
#else
	void ConvertDfg2Ted::translate(void) {
		multimap<int,pair<string,int> > level;
		_dMan->hierarchy(level);
		assert(ATedNode::checkContainer(_tMan->getContainer()));
#if 1
		Mark<TedNode>::newPMark();
		_tMan->getContainer().update_PMark();
#endif
		vector<DfgNode*> dfs_nodes;
		DfgParents parents;
		_dMan->collectParents(parents,dfs_nodes);
		unsigned long long count = 0;
		for(vector<DfgNode*>::iterator it = dfs_nodes.begin(); it != dfs_nodes.end(); it++) {
			DfgNode* pNode =*it;
			ATedNode* pANode = NULL;
			if (pNode->isConst()) {
				pANode = new ETedNode(pNode->getConst());
			} else if (pNode->isPI() || pNode->isVarConst()) {
				string name = pNode->getName();
				TedVar* pvar = pNode->getTedVarIfExist();
				if(pvar && pvar->isRetimed())
					name = pvar->getBase()->getName();				
				pANode = new ETedNode(name);
			} else {
				assert(pNode->isOp());
				ATedNode* pELeft = mVisited[pNode->getLeft()];
				ATedNode* pERight = mVisited[pNode->getRight()];
				assert(parents.find(pNode->getLeft())!=parents.end());
				assert(parents.find(pNode->getRight())!=parents.end());
				if (pNode->isReg()) {
					assert(pELeft==pERight);
					string name = pNode->getName();
					Util::strSubstitute(name,"R");
					pair<bool,wedge> test = Util::atoi(name.c_str());
					assert(test.first);
					TedRegister retime = test.second;
					pANode = ETedNode::BuildRecursive(pELeft,retime);
				} else {
					if (parents[pNode->getLeft()].size()>1) {
						pELeft = pELeft->duplicateRecursive();
					}
					if (parents[pNode->getRight()].size()>1) {
						pERight = pERight->duplicateRecursive();
					}
					if (pNode->isAdd()) {
						pANode = pELeft->add(pERight);
					} else if (pNode->isSub()) {
						pANode = pELeft->sub(pERight);
					} else if (pNode->isMul()) {
						pANode = pELeft->mul(pERight);
					} else if (pNode->isLSH()) {
						assert(pERight->isConst());
						wedge val = 2;
						wedge exponent = pERight->getConst();
						for (int i = 1; i < exponent; i++) {
							val *= 2;
						}
						pERight->setConst(val);
						pANode = pELeft->mul(pERight);
					} 
				}
			}
#if 1
			Mark<TedNode>::newMark();
			pANode->update_cone_PMark();
			TedKids::garbage_collect_unmarked(&Mark<TedKids>::isPMarked);
			ATedNode::garbage_collect_unmarked(&Mark<TedNode>::isPMarked);
#endif
			mVisited[pNode] = pANode;
		}
		for (DfgMan::PrimaryOutputs::iterator p = _dMan->_mPos.begin(); p != _dMan->_mPos.end(); p++) {
			ATedNode * pANode = mVisited[p->second]->duplicateRecursive();
			TedNodeRoot pin(_tMan->normalize(pANode));
			_tMan->linkTedToPO(pin.Node(), pin.getWeight(), p->first);
		}
	}

	ATedNode * ConvertDfg2Ted::translate(DfgNode * pNode) {
		return NULL;
	}
#endif

	void ConvertDfg2Ted::flatten(void) {
		assert(ATedNode::checkContainer(_tMan->getContainer()));
		multimap<int,pair<string,int> > h;
		_dMan->hierarchy(h);
#if 0
		Mark<TedNode>::newPMark();
		_tMan->getContainer().update_PMark();
#endif
		map<string,ATedNode*> hash_name;
		map<string,int> hash_count;
		DfgParents parents;
		_dMan->collectParents(parents);
		Mark<DfgNode>::newMark();
		mVisited.clear();
		for (multimap<int,pair<string,int> >::iterator jt = h.begin(), jtend=h.end(); jt!=jtend; jt++) {
			string po_name = jt->second.first;
			DfgNode* po_node = _dMan->_mPos[po_name];
			ATedNode* pANode = flatten(hash_count,hash_name,parents,po_node);
			hash_name[po_name] = pANode;
			hash_count[po_name] = jt->second.second;
#if 0
			Mark<TedNode>::newMark();
			pANode->update_cone_PMark();
			TedKids::garbage_collect_unmarked(&Mark<TedKids>::isPMarked);
			ATedNode::garbage_collect_unmarked(&Mark<TedNode>::isPMarked);
			cout << "po_name="<<po_name<<endl;
#endif
		}
		for (multimap<int,pair<string,int> >::iterator jt = h.begin(), jtend=h.end(); jt!=jtend; jt++) {
			if (jt->second.second==0) {
				ATedNode * pANode = hash_name[jt->second.first];
				TedNodeRoot pin(_tMan->normalize(pANode));
				_tMan->linkTedToPO(pin.Node(), pin.getWeight(), jt->second.first);
			}
		}
	}

	ATedNode* ConvertDfg2Ted::flatten(map<string,int>& hash_count, map<string,ATedNode*>& hash_name, DfgParents& parents, DfgNode* pNode) {
		if (mVisited.find(pNode)!=mVisited.end()) {
			return mVisited[pNode];
		}
		ATedNode* pANode = NULL;
		if (pNode->isConst()) {
			pANode = new ETedNode(pNode->getConst());
		} else if (pNode->isPI() || pNode->isVarConst()) {
			string name = pNode->getName();
			map<string,ATedNode*>::iterator it = hash_name.find(name);
			if (it!=hash_name.end()) {
				pANode = hash_name[name];
				//			if (hash_count[name]>1) {
				//				pANode = pANode->duplicateRecursive();
				//				hash_count[name]--;
				//			}
			} else {
				pANode = new ETedNode(pNode->getName());
			}
		} else {
			assert(pNode->isOp());
			ATedNode* pELeft = flatten(hash_count,hash_name,parents,pNode->getLeft());
			ATedNode* pERight = flatten(hash_count,hash_name,parents,pNode->getRight());
			assert(parents.find(pNode->getLeft())!=parents.end());
			if (parents[pNode->getLeft()].size()>1) {
				pELeft = pELeft->duplicateRecursive();
			}
			assert(parents.find(pNode->getRight())!=parents.end());
			if (parents[pNode->getRight()].size()>1) {
				pERight = pERight->duplicateRecursive();
			}
			if (pNode->isAdd()) {
				pANode = pELeft->add(pERight);
			} else if (pNode->isSub()) {
				pANode = pELeft->sub(pERight);
			} else if (pNode->isMul()) {
				pANode = pELeft->mul(pERight);
			} else if (pNode->isLSH()) {
				assert(pERight->isConst());
				wedge val = 2;
				wedge exponent = pERight->getConst();
				for (int i = 1; i < exponent; i++) {
					val *= 2;
				}
				pERight->setConst(val);
				pANode = pELeft->mul(pERight);
			}
		}
		mVisited[pNode] = pANode;
		return pANode;
	}

}
