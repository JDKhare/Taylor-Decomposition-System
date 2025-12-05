/*
 * =====================================================================================
 *
 *       Filename:  ConvertTed2Dfg.cc
 *    Description:
 *        Created:  09/10/2009 01:18:50 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 17:43:11 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <iostream>
using namespace std;

#include "TedMan.h"
#include "TedNode.h"
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
using namespace dfg;

#include "ConvertTed2Dfg.h"
namespace convert {

	void ConvertTed2Dfg::translate(void) {
		Mark<TedNode>::newMark();
		for(TedMan::PrimaryOutputs::iterator it = _tMan->_pos.begin(); it!=_tMan->_pos.end(); it++ ) {
			if(it->second.getType()!=TedNodeRoot::PT && it->second.getType()!=TedNodeRoot::ST) {
				TedNode* tNode = it->second.Node();
				DfgNode* dNode = translate(tNode);
				int weight = it->second.getWeight();
				DfgNode* dout = NULL;
				if (weight!= 1) {
					DfgNode* constant = new DfgNode(_dMan,weight);
					dout = dNode->mul(constant);
				} else {
					dout = dNode;
				}
				TedRegister retimed = it->second.getRegister();
				if (retimed>0) {
					if (!_dMapR.has(dout)) {
						_dMapR.registrate(dout,1,dout->reg(1));
					}
					dout = _dMapR.get(dout,retimed);
					//dout = dout->reg(retimed);
				}
				_dMan->_mPos[it->first] = dout;
			}
		}
	}

	DfgNode* ConvertTed2Dfg::translate(TedNode* pNode) {
		if (pNode->visited.isMarked() || pNode==TedNode::getOne()) {
			return _tMap.get(pNode,1);
		}
		int weight = 0;
		int index = 0;
		TedNode* pKid = NULL;
		DfgNode* plhs = NULL;
		DfgNode* lhs = NULL;
		DfgNode* rhs = NULL;
		DfgNode* nrhs = NULL;
		DfgNode* currentNode = NULL;
		string piName;
		const TedVar* pVar = pNode->getVar();
		assert(pVar);
		int retimed = pNode->getRegister();
#if 1
		piName = pVar->getRootName();
#else
		if (pVar->isMember()) {
			piName = (dynamic_cast<const TedVarGroupMember*>(pVar))->getOwner()->getName();
#if 0
		} else if(pVar->isRetimed()) {
			//
			// the variable prefix @n corresponding to n registers, allows the _piMap
			// to distinguish among different degree of retimed variables. Otherwise
			// the hash for b@3, b@1, and b@123 look the same as the has for b
			//
			piName = pVar->getBase()->getName();
#endif
		} else {
			piName = pNode->getName();
		}
#endif
		enum SubPropagation {
			PREVIOUS_IS_NEG,
			FORWARD_IS_NEG,
			RESOLVE_PREVIOUS_AND_FORWARD,
			RESOLVE_FORWARD_AND_FORWARD,
			RESOLVE,
			RESOLVE_AND_FORWARD,
			NO_NEGATION
		};
		SubPropagation negativeState = NO_NEGATION;
		DfgNode* plhs_neg = NULL;
		if (!_piMap.has(piName)) {
			if (pNode->getBackptr()) {
				TedNode* term = pNode->getBackptr();
				currentNode = translate(term);
			} else {
				DfgOperator::Type op = DfgOperator::VAR;
				if(pNode->getVar()->isVarConst()) {
					op = DfgOperator::VARCONST;
				}
				currentNode = new DfgNode(_dMan,piName,op);
			}
			_piMap.registrate(piName,1,currentNode);
			/*
			if(0!=pNode->getRegister()) {
				if (!_dMapR.has(currentNode)) {
					_dMapR.registrate(currentNode,1,currentNode->reg(1));
				}
				currentNode = _dMapR.get(currentNode,pNode->getRegister());
			}*/		
		}
		FOREACH_KID_OF_NODE(pNode) {
			index = _iterkids.getIndex();
			weight = _iterkids.getWeight();
			pKid = _iterkids.Node<TedNode>();
			TedRegister kidreg = _iterkids.getRegister();
			rhs = translate(pKid);
			if (index==0) {
				if (weight<0) {
					plhs_neg = _tMap.get(pKid,weight*-1);
					if (kidreg>0) {
						if (!_dMapR.has(plhs_neg)) {
							_dMapR.registrate(plhs_neg,1,plhs_neg->reg(1));
						}
						plhs_neg = _dMapR.get(plhs_neg,kidreg);
					}
					negativeState = PREVIOUS_IS_NEG;
				}
				plhs = _tMap.get(pKid,weight);
				if (kidreg>0) {
					if (!_dMapR.has(plhs)) {
						_dMapR.registrate(plhs,1,plhs->reg(1));
					}
					plhs = _dMapR.get(plhs,kidreg);
				}
			} else {
				lhs = _piMap.get(piName,index);
				if (!_dMap.has(lhs)) {
					_dMap.registrate(lhs,_dOne,lhs);
				}
				if (retimed>0) {
					if (!_dMapR.has(lhs)) {
						_dMapR.registrate(lhs,1,lhs->reg(1));
					}
					lhs = _dMapR.get(lhs,retimed);
				}
				if (!_dMap.has(lhs)) {
					_dMap.registrate(lhs,_dOne,lhs);
				}
				//WEIGHT_TIMES_NODE_TIMES_KID
				if (rhs != _dOne) {
					if (kidreg>0) {
						if (!_dMapR.has(rhs)) {
							_dMapR.registrate(rhs,1,rhs->reg(1));
						}
						rhs = _dMapR.get(rhs,kidreg);
					}
					//rhs = rhs->reg(kidreg);
					rhs = lhs->mul(rhs);
					if (!_dMap.has(rhs)) {
						_dMap.registrate(rhs,_dOne,rhs);
					} else {
						nrhs = _dMap.get(rhs,_dOne);
						if (rhs!=nrhs) {
							delete rhs;
							rhs = nrhs;
						}
					}
				} else {
					rhs = lhs;
					assert(_dMap.has(lhs));
					DfgNode* tmp = _dMap.get(lhs,_dOne);
					if (tmp!=rhs) {
						rhs = tmp;
						std::cout << " replicated node " << tmp->getName() << std::endl;
					}
				}
#if 0
				if (!_dMap.has(lhs)) {
					_dMap.registrate(lhs,_dOne,lhs);
				} else {
					// pi exist both in pi map and dmap
					nrhs = _dMap.get(lhs,_dOne);
					if (lhs!=nrhs) {
						std::cout << "warning deleting pi: " << lhs->getName() << endl;
						delete lhs;
						lhs = nrhs;
					}
				}
#endif
				if (weight<0) {
					weight *= -1;
					switch (negativeState) {
					case PREVIOUS_IS_NEG:
						negativeState = RESOLVE_PREVIOUS_AND_FORWARD;
						plhs = plhs_neg;
						break;
					case FORWARD_IS_NEG:
						negativeState = RESOLVE_FORWARD_AND_FORWARD;
						break;
					default:
						if (plhs) {
							negativeState = RESOLVE;
						} else {
							negativeState = RESOLVE_AND_FORWARD;
						}
						break;
					}
				}
				DfgNode* dw = _tMap.get(weight);
				rhs = _dMap.get(rhs,dw);
				if (plhs) {
					switch (negativeState) {
					case RESOLVE:
						negativeState = NO_NEGATION;
						plhs = plhs->sub(rhs);
						break;
					case RESOLVE_FORWARD_AND_FORWARD:
						negativeState = FORWARD_IS_NEG;
						plhs = plhs->add(rhs);
						break;
					case FORWARD_IS_NEG:
						negativeState = NO_NEGATION;
						plhs = rhs->sub(plhs);
						break;
					case RESOLVE_PREVIOUS_AND_FORWARD:
						negativeState = FORWARD_IS_NEG;
						plhs = plhs->add(rhs);
						break;
					case PREVIOUS_IS_NEG:
						negativeState = NO_NEGATION;
						plhs = plhs_neg;
						plhs = rhs->sub(plhs);
						break;
					case NO_NEGATION:
						plhs = plhs->add(rhs);
						break;
					}
					if (!_dMap.has(plhs)) {
						_dMap.registrate(plhs,_dOne,plhs);
					} else {
						nrhs = _dMap.get(plhs,_dOne);
						if (plhs!=nrhs) {
							delete plhs;
							plhs = nrhs;
						}
					}
				} else {
					plhs = rhs;
					if (negativeState == RESOLVE_AND_FORWARD)
						negativeState = FORWARD_IS_NEG;
				}
			}
		}
		assert(negativeState == NO_NEGATION);
		//its impossible that all edges have a negative weight, by
		//construction all negative edges are factored out.
		/*
		   if (negativeState == FORWARD_IS_NEG) {
		   DfgNode* dw = _tMap.get(-1);
		   plhs = plhs->mul(dw);
		   negativeState = NO_NEGATION;
		   }
		   */
		_tMap.registrate(pNode,1,plhs);
		pNode->visited.setMark();
		return plhs;
	}

}
