/*
 * =====================================================================================
 *
 *       Filename:  TedOrderReloc.cc
 *    Description:
 *        Created:  29/4/2009 10:27:16 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
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

	TedMan * TedOrderReloc::pushUp(const TedVar& var, unsigned int distance) {
		unsigned int currentLevel = _pManCurrent->getContainer().getLevel(var);
		unsigned int toLevel = currentLevel + distance;
		TedMan* pMan = relocateVariable(var,  toLevel);
		updateCurrentManager(pMan);
		return pMan;
	}

	TedMan * TedOrderReloc::pushUp(const TedVar& var) {
		unsigned int toLevel = _pManCurrent->getContainer().getLevel(var) + 1;
		TedMan* pMan = relocateVariable(var,  toLevel);
		updateCurrentManager(pMan);
		return pMan;
	}

	TedMan * TedOrderReloc::pushDown(const TedVar& var) {
		unsigned int toLevel = _pManCurrent->getContainer().getLevel(var) - 1;
		TedMan* pMan = relocateVariable(var,  toLevel);
		updateCurrentManager(pMan);
		return pMan;
	}

	TedMan * TedOrderReloc::pushDown(const TedVar& var, unsigned int distance) {
		unsigned int currentLevel = _pManCurrent->getContainer().getLevel(var);
		unsigned int toLevel = currentLevel - distance;
		TedMan* pMan = relocateVariable(var,  toLevel);
		updateCurrentManager(pMan);
		return pMan;
	}

	/** @brief Relocate a variable to a new level */
	TedMan * TedOrderReloc::relocateVariable(const TedVar& v, unsigned int l) {
		TedContainer::iterator msp = _pManCurrent->getContainer().find(v);
		if (msp == _pManCurrent->getContainer().end())
			return NULL;
		TedMan * pManNew = new TedMan();
		unsigned int level = _pManCurrent->getContainer().size() - l + 1;
		unsigned int m, k;
		bool bOldOnTop = true;
		if (level == 0 || level > _pManCurrent->getContainer().getMaxLevel())
			return NULL;
		unsigned int level_old = msp->second.first;
		if (level == level_old)
			return _pManCurrent->duplicate();
		unsigned int min = MIN(level, level_old);
		unsigned int max = MAX(level, level_old);
		if (level_old > level)
			bOldOnTop = false;
		for (msp = _pManCurrent->getContainer().begin(); msp!= _pManCurrent->getContainer().end(); msp++) {
			m = msp->second.first;
			if (m < min) {
				k = m;
			} else if (m == min) {
				k = bOldOnTop ?(max):(m+1);
			} else if (m < max) {
				k = m + (bOldOnTop?(-1):(1));
			} else if (m == max) {
				k = bOldOnTop?(m-1):(min);
			} else {
				k = m;
			}
			TedSet * sNodes = new TedSet;
			pManNew->getContainer()[TedVar(msp->first)] = pair<unsigned int, set <TedNode *> *>(k, sNodes);
		}
		assert(ATedNode::checkContainer(pManNew->getContainer()));
		for (TedMan::PrimaryOutputs::iterator mp = _pManCurrent->getPOs().begin(); mp!= _pManCurrent->getPOs().end(); mp++) {
			ATedNode* pANode = ETedNode::BuildRecursive(mp->second.Node());
			TedNodeRoot pin(pManNew->normalize(pANode));
			pin.setRegister(mp->second.getRegister());
			pin.setWeight(pin.getWeight()*mp->second.getWeight());
			pin.setType(mp->second.getType());
			pManNew->linkTedToPO(pin,mp->first);
			//pManNew->linkTedToPO(pin.Node(), pin.getWeight()*mp->second.getWeight(), mp->first.c_str());
		}
		return pManNew;
	}

}
