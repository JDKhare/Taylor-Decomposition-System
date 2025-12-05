/*
 * =====================================================================================
 *
 *       Filename:  TedManOrder.cc
 *    Description:
 *        Created:  12/8/2008 10:27:16 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 194                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <list>

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedParents.h"


#include "DfgNode.h"
using dfg::DfgNode;
using dfg::DfgStat;

#include "util.h"
using util::Util;

namespace ted {


#ifdef FUNCTIONS_DEPRECATED
	void TedMan::getUpLowMulBound(int* upb, int* lowb) {
		unsigned int index = 0;
		wedge weight = 0;
		TedNode* pNode = NULL,* pKid = NULL;
		upb = lowb = 0;
		TedContainer:: iterator mp;
		set <TedNode*>::iterator  sp;
		set <TedNode*> sKids;
		for(mp = _container.begin(); mp != _container.end(); mp++) {
			for(sp = mp->second.second->begin();sp != mp->second.second->end(); sp ++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					//weight = _iterkids.getWeight();
					for(unsigned int i = 0; i < index; i++) {
						(*upb)++;
						if(index != 0) {
							sKids.insert(pKid);
						}
					}
				}
			}
		}
		(*lowb) = sKids.size();
	}
#endif

	/* Johnson Trotter Permutation Enumeration Algorithm:
http://www.cut-the-knot.org/Curriculum/Combinatorics/JohnsonTrotter.shtml
*/

#define DINT pair <unsigned int, bool>
	int _JT_find_largest_mobile(vector <DINT > & vdindex) {
		int i, target, largestindex = -1, largestval = -1;
		for(i = vdindex.size()-1; i >= 0; i--) {
			target = vdindex[i].second ? i-1:i+1;
			if(target < 0 || target >(vdindex.size()-1))continue;
			if(vdindex[i].first > vdindex[target].first) {
				if(largestval <(int)vdindex[i].first) {
					largestval = vdindex[i].first;
					largestindex = i;
				}
			}
		}
		return largestindex;
	}

	list <unsigned int> & _JTPermutationEnumaration(unsigned int n, list <unsigned int> & lIndex) {
		vector <DINT > vdindex;
		DINT diTemp;

		for(unsigned int j = 0; j < n; j++) {
			vdindex.push_back(DINT(j, true));
		}
		int i, target = 0;
		while((i = _JT_find_largest_mobile(vdindex))!= -1) {
			if(vdindex[i].second) {
				target = i-1;
				lIndex.push_back(target);
			} else {
				target = i+1;
				lIndex.push_back(i);
			}
			for(unsigned int j = 0; j < n; j++) {
				if(vdindex[j].first > vdindex[i].first) {
					vdindex[j].second ^= true;
				}
			}
			diTemp = vdindex[i];
			vdindex[i] = vdindex[target];
			vdindex[target] = diTemp;

		}
		return lIndex;
	}

#ifdef CURRENTLY_ON_DEVELOPMENT

	/**@brief reorders two adjacent variables on a multiple output TED,
	  @detail The two nodes have to be adjacent to be swapped. The method assumes
	  that varX is on top of VarY in the graph. If you are not sure in
	  which var is on top use the method reorder instead.

	  First we Split varX nodes into UxWithY and UxWithoutY
	  Then as nodes in UxWithoutY do not need to be swapped they are inserted into the final Ux
	  */
	void TedMan::reor(const TedVar& varX, const TedVar& varY) {
		size_t yInitialLevel = _container.getLevel(varY);
		size_t xInitialLevel = _container.getLevel(varX);
		assert(xInitialLevel = yInitialLevel + 1);

		TedParents parents;
		preOrderDFSforPOs(parents,&TedParents::collect);

		TedSet pathWith_X_Y;
		TedSet Utmp;
		TedSet::iterator px_it, py, pOk;
		TedNode* pNode;
		ETedNode* pAdder;
		Mark<TedNode>::newMark();

		// leave in _container[varX] <= only varX nodes with no kid varY
		// place all varX nodes with kid varY into pathWith_X_Y
		TedSet pathWith_X =*_container[varX].second;
		for(TedSet::iterator p = pathWith_X.begin(), pend = pathWith_X.end(); p!=pend; p++) {
			pNode = (*p);
			bool XconnectToY = false;
			FOREACH_KID_OF_NODE(pNode) {
				TedNode* pKid = _iterkids.Node<TedNode>();
				if(_container.getLevel(*pKid->getVar()) == yInitialLevel) {
					XconnectToY = true;
					pKid->visited.setWaterMark();
				}
			}
			if(XconnectToY) {
				pathWith_X_Y.insert(pNode);
				_container[varX].second->erase(pNode);
			} else {
				pNode->visited.setMark();
			}
		}
		pathWith_X.clear();

		// leave in _container[varY] <= only varY nodes that have parents other than varX
		TedSet pathWith_Y =*_container[varY].second;
		for(TedSet::iterator p = pathWith_Y.begin(), pend = pathWith_Y.end(); p!=pend; p++) {
			pNode = (*p);
			if(pNode->visited.isWaterMarked()) {
				bool all_parents_are_varX = true;
				for(MyParent::iterator it = parents[pNode]->begin(), end = parents[pNode]->end(); it != end; it++) {
					TedNode* topParent = it->first;
					if(_container.getLevel(*topParent->getVar())!=xInitialLevel) {
						all_parents_are_varX = false;
						break;
					}
				}
				if(all_parents_are_varX) {
					_container[varY].second->erase(pNode);
				} else {
					pNode->visited.setMark();
				}
			} else {
				pNode->visited.setMark();
			}
		}
		pathWith_Y.clear();

		// change the index of varX and varY
		// starts with varX on top of varY
		size_t yFinalLevel = xInitialLevel;
		size_t xFinalLevel = yInitialLevel;
		_container[varY].first = _container.getMaxLevel()+1-yFinalLevel;
		_container[varX].first = _container.getMaxLevel()+1-xFinalLevel;
		// ends with varX below of varY

#if _DEBUG
		//verify that all nodes remaining in the container at varX and varY are marked
		for(TedSet::iterator it = _container[varY].second->begin(), end = _container[varY].second->end();
			it!=end; it++) {
			assert((*it)->visited.isMarked());
		}
		for(TedSet::iterator it = _container[varX].second->begin(), end = _container[varX].second->end();
			it!=end; it++) {
			assert((*it)->visited.isMarked());
		}
#endif

		// pathWith_X_Y
		for(TedSet::iterator px_it = pathWith_X_Y.begin(), px_end = pathWith_X_Y.end(); px_it != px_end; px_it++) {
			TedNode* px = (*px_it);
			ATedNode* pPath = NULL;
			TedKids oldKids_X;
			ETedNode* edge0_X = NULL;
			ETedNode* edge_X = NULL;
			TedKids::iterator xt_kid = px->getKids()->begin(), xt_end = px->getKids()->end();
			for(;xt_kid != xt_end; xt_kid++) {
				TedNode* pKid_x = xt_kid.Node<TedNode>();
				wedge weight_x = xt_kid.getWeight();
				int index_x = xt_kid.getIndex();
				TedRegister regKid_x = xt_kid.getRegister();
				assert(regKid_x==TedRegister(0));
				if((*pKid_x->getVar())!= varY) {
					// collect all kids of px different than varY
					if(0==index_x) {
						edge0_X = new ETedNode(*pKid_x);
						TedKids* newKids = TedKids::create_kids();
						FOREACH_KID_OF_NODE(edge0_X) {
							newKids->insert(_iterkids.getIndex(),(*_iterkids).Node(),(*_iterkids).getWeight()*weight_x,(*_iterkids).getRegister());
						}
						edge0_X->_tKids = newKids;
						//edge0_X = edge0_X->mul(new ETedNode(weight_x));
					} else {
						oldKids_X.insert(index_x,pKid_x,weight_x,TedRegister(0));
					}
				} else {
					// swap the order for kids of varX connecting to varY
					pAdder = NULL;
					if(0==index_x) {
						pAdder = new ETedNode(*pKid_x);
						TedKids* newKids = TedKids::create_kids();
						// need to duplicate tedKids even if weight_x == 1,
						// because pKid_x might be referred by another parent.
						FOREACH_KID_OF_NODE(pAdder) {
							newKids->insert(_iterkids.getIndex(),(*_iterkids).Node(),(*_iterkids).getWeight()*weight_x,(*_iterkids).getRegister());
						}
						pAdder->_tKids = newKids;
					} else {
						TedKids::iterator yt_kid = pKid_x->getKids()->begin(), yt_end = pKid_x->getKids()->end();
						for(;yt_kid != yt_end; yt_kid++) {
							wedge weight_y = yt_kid.getWeight();
							TedNode* pKid_y = yt_kid.Node<TedNode>();
							int index_y = yt_kid.getIndex();
							TedRegister regKid_y = yt_kid.getRegister();
							assert(regKid_y==TedRegister(0));
							wedge propagateW = weight_x*weight_y;
							TedKids newKidsX;
							newKidsX.insert(index_x,pKid_y,propagateW,TedRegister(0));
							TedNode* pNewVarX = _container.getNode(varX,newKidsX);
							newKidsX.clear();
							assert(!pNewVarX->isConst());
							TedKids newKidsY;
							newKidsY.insert(index_y,pNewVarX,1,TedRegister(0));
							TedNode* pNewVarY = _container.getNode(varY,newKidsY,TedRegister(0));
							newKidsY.clear();
							pAdder = (ETedNode*)((ETedNode*)pNewVarY)->add_with_content(pAdder);
						}
					}
					assert(pAdder);
					// At this moment we have generated pAdder, now we need to ensure that the
					// weight on each edge of node at the top has the gcd of the child node.
					FOREACH_KID_OF_NODE(pAdder)if((*_iterkids).Node()!= TedNode::getOne()) {
						wedge gcdKid = (*_iterkids).Node()->getKids()->extractGCD();
						gcdKid*= (*_iterkids).getWeight();
						(*_iterkids).setWeight(gcdKid);
					}
					pPath = pAdder->add_with_content(pPath);
				}
			}
			if(oldKids_X.size()>0) {
				TedNode* pnedge_X = _container.getNode(varX,oldKids_X,px->getRegister());
				edge_X = new ETedNode(*pnedge_X);
			}
			// pPath node should have the new connection Y_X; as we have been iterating
			// through nodes from pathWith_X_Y, which had a connection X_Y.
			// Therefore node pPath must always exist.
			assert(pPath);
			pPath = ((ETedNode*)pPath)->add_with_content(edge0_X);
			pPath = ((ETedNode*)pPath)->add_with_content(edge_X);
			pPath = (ATedNode*)_container.getNode((ETedNode*)pPath);
			wedge gcd = pPath->getKids()->extractGCD();
			ETedNode* pPathTotal = (ETedNode*)pPath;
			FOREACH_KID_OF_NODE(pPathTotal) {
				TedNode* kidnode = _iterkids.Node<TedNode>();
				if(_container.isSameHight(*kidnode->getVar(),varX)) {
					kidnode->visited.setMark();
				}
			}
			_container[*pPathTotal->getVar()].second->erase(pPathTotal);
			px->_tKids = pPathTotal->_tKids;
			px->_tVar = pPathTotal->_tVar;
			px->_register = pPathTotal->_register;
			pPathTotal->_tKids = NULL;
			pPathTotal->_tVar = NULL;
			TedNode* tmp = _container.getNode((ETedNode*)px);
			assert(tmp==px);
			px->visited.setMark();
			// All Paths finish, reattach parents of px to pPathTotal
			assert(gcd);
			if(1!=gcd) {
				forwardWeightUp(px,gcd,parents);
				//assert(false);
				//			throw(string("04025. Need to populate up the gcd after reordering"));
			}
		}
		pathWith_X_Y.clear();

		// cleaning unused nodes with VarY
		pathWith_Y =*_container[varY].second;
		for(TedSet::iterator it = pathWith_Y.begin(), end = pathWith_Y.end();	it!=end; it++) {
			if(!(*it)->visited.isMarked()) {
				_container[varY].second->erase(*it);
			} else {
				(*it)->visited.cleanMark();
			}
		}
		pathWith_Y.clear();

		// cleaning unused nodes with VarX
		pathWith_X =*_container[varX].second;
		for(TedSet::iterator it = pathWith_X.begin(), end = pathWith_X.end();	it!=end; it++) {
			if(!(*it)->visited.isMarked()) {
				_container[varX].second->erase(*it);
			} else {
				(*it)->visited.cleanMark();
			}
		}
		pathWith_X.clear();
	}

	void TedMan::forwardWeightUp(TedNode* pNode, wedge gcd, TedParents& parents) {
		if(parents.find(pNode) == parents.end()) {
			throw(string("04024. Populate weight to PO"));
			return;
		}
		for(MyParent::iterator it = parents[pNode]->begin(), end = parents[pNode]->end(); it != end; it++) {
			TedNode* topParent = it->first;
			MyIndex* topIndices = it->second;
			for(int i=0; i != topIndices->size(); i++) {
				int index = topIndices->at(i);
				wedge weight = topParent->getKidWeight(index);
				topParent->setKidWeight(index, weight*gcd);
			}
			wedge gcdUp = topParent->getKids()->extractGCD();
			assert(gcdUp);
			if(1 != gcdUp) {
				forwardWeightUp(topParent,gcdUp,parents);
			}
		}
	}
#endif

}
