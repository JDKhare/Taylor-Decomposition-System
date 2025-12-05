/*
 * =====================================================================================
 *
 *       Filename:  ConvertDfg2TedError.cc
 *    Description:
 *        Created:  01/12/2010 09:13:36 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 122                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-11 21:46:11 +0100 (Mon, 11 Jan 2010)     $: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <iostream>
#include <map>
using namespace std;

#include "TedMan.h"
#include "TedNode.h"
#include "ATedNode.h"
#include "ETedNode.h"
#include "TedNodeRoot.h"
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
#include "DfgOperator.h"
using namespace dfg;

#include "util.h"
#include "csd.h"
using util::Util;
using util::Csd;

#include "ConvertDfg2TedError.h"
namespace convert {

	void ConvertDfg2TedError::translate(void) {
		assert(ATedNode::checkContainer(_tMan->getContainer()));
		Mark<DfgNode>::newMark();
		//map <DfgNode *, ATedNode *> mVisited;
		for (DfgMan::PrimaryOutputs::iterator p = _dMan->_mPos.begin(); p != _dMan->_mPos.end(); p++) {
			Value val = translate(p->second);
			TedNodeRoot pinO(_tMan->normalize(val.first));
			_tMan->linkTedToPO(pinO.Node(), pinO.getWeight(), p->first);
			TedNodeRoot pinE(_tMan->normalize(val.second));
			_tMan->linkTedToPO(pinE.Node(), pinE.getWeight(), "E_"+p->first);
		}
	}

	ConvertDfg2TedError::Value ConvertDfg2TedError::translate(DfgNode * pNode) {
		assert(pNode->getOperator()->getBitwidth());
		ATedNode * pANode=NULL, *pALeft=NULL, *pELeft=NULL, *pARight=NULL, *pERight=NULL, *pErr=NULL, *pErrNode=NULL;
		Value vLeft, vRight;
		if (pNode->_visited.isMarked()) {
			return _map[pNode];
		}
		string varErrName("E_");
		varErrName += Util::itoa(pNode->getOperator()->getID());
		if(pNode->isOp()) {
			vLeft = translate(pNode->getLeft());
			pALeft = vLeft.first;
			pELeft = vLeft.second;
			vRight= translate(pNode->getRight());
			pARight = vRight.first;
			pERight = vRight.second;
			pErr = new ETedNode(varErrName);
			TedVar* pVarErr = _tMan->checkVariables(varErrName);
			pVarErr->setBitwidth(pNode->getOperator()->getBitwidth());
		}
		switch(pNode->getOp()) {
		case DfgOperator::CONST:
			{
				pANode = new ETedNode(pNode->getConst());
				pErrNode = NULL;
				break;
			}
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			{
				pANode = new ETedNode(pNode->getName());
				varErrName = "E_";
				varErrName += pNode->getName();
				pErrNode = new ETedNode(varErrName);
				TedVar* pvarErr = _tMan->checkVariables(varErrName);
				TedVar* pVarNode = _tMan->checkVariables(pNode->getName());
				pvarErr->setBitwidth(pVarNode->getBitwidth());
				break;
			}
		case DfgOperator::ADD:
			{
				pANode = pALeft->duplicateRecursive()->add(pARight)->duplicateRecursive();
				if(!pELeft) {
					pELeft = new ETedNode(0);
				}
				if(!pERight) {
					pERight = new ETedNode(0);
				}
				pErrNode = pELeft->duplicateRecursive()->add(pERight->duplicateRecursive());
				pErrNode = pErrNode->add(pErr);
				break;
			}
		case DfgOperator::SUB:
			{
				pANode = pALeft->duplicateRecursive()->sub(pARight->duplicateRecursive());
				if(!pELeft) {
					pELeft = new ETedNode(0);
				}
				if(!pERight) {
					pERight = new ETedNode(0);
				}
				pErrNode = pELeft->duplicateRecursive()->sub(pERight->duplicateRecursive());
				pErrNode = pErrNode->add(pErr);
				break;
			}
		case DfgOperator::MUL:
			{
				pANode = pALeft->duplicateRecursive()->mul(pARight->duplicateRecursive());
				if(!pELeft && !pERight) {
					pErrNode = new ETedNode(0);
				} else if(!pELeft) {
					pErrNode = pERight->duplicateRecursive()->mul(pALeft->duplicateRecursive());
				} else if(!pERight) {
					pErrNode = pELeft->duplicateRecursive()->mul(pARight->duplicateRecursive());
				} else {
					pELeft = pELeft->duplicateRecursive()->mul(pARight->duplicateRecursive());
					pERight = pERight->duplicateRecursive()->mul(pALeft->duplicateRecursive());
					pErrNode = pELeft->duplicateRecursive()->add(pERight->duplicateRecursive());
				}
				pErrNode = pErrNode->add(pErr);
				break;
			}
		case DfgOperator::LSH:
			{
				assert(pARight->isConst());
				int val = 2;
				int exponent = pARight->getConst();
				for (int i = 1; i < exponent; i++) {
					val *= 2;
				}
				pARight->setConst(val);
				pANode = pALeft->duplicateRecursive()->mul(pARight->duplicateRecursive());
				assert(pELeft);
				pErrNode = pELeft->duplicateRecursive()->mul(pARight->duplicateRecursive());
				break;
			}
		case DfgOperator::RSH:
		default:
			throw(string("Error. DFG to TED operation not implemented."));
		}
		pNode->_visited.setMark();
		Value ret(pANode,pErrNode);
		_map.insert(pair<DfgNode*,Value>(pNode,ret));
		return ret;
	}

}
