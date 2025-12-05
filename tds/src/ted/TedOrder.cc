/*
 * =====================================================================================
 *
 *       Filename:  TedOrder.cc
 *    Description:
 *        Created:  07/14/2011 08:59:38 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */


#include <bitset>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <list>

using namespace std;

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ATedNode.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedParents.h"
#include "TedOrder.h"

namespace ted {

	void TedOrder::updateCurrentManager(TedMan* pman) {
		_pManCurrent = pman;
		ATedNode::setContainer(_pManCurrent->getContainer());
	}


	TedMan* TedOrder::duplicateManager(TedMan* pMan) {
		return pMan->duplicate();
	}

	void TedOrder::deleteManager(TedMan* pMan) {
		assert(pMan);
		delete pMan;
	}


	TedMan* TedOrder::toLocation(const TedVar& var, unsigned int location) {
		unsigned int level = _pManCurrent->getContainer().getLevel(var);
		if(level<location) {
			return pushUp(var,location-level);
		} else if(level>location) {
			return pushDown(var,level-location);
		} else {
			//TOFIX: this is really poor.
			//this class should be in charge of placing a TedMan into the history
			//look at code bottomdcse(608)for instance, there is an assumption
			//that the TedMan* returned is always different than the TedMan calling.
#if 0
			return _pManCurrent->duplicate();
#else
			return _pManCurrent;
#endif
		}
	}

	// var1 will be located above var2
	TedMan* TedOrder::jumpAbove(const TedVar& var1, const TedVar& var2) {
		if(_pManCurrent->getContainer().isSameHight(var1,var2))
			return _pManCurrent;
		int jump = _pManCurrent->getContainer().getLevel(var2)- _pManCurrent->getContainer().getLevel(var1);
		if(jump==-1) {
			// it is already in the desired location
			return _pManCurrent;
		}
		return(jump>0) ? pushUp(var1,jump): pushDown(var1,-(++jump));
	}

	// var1 will be located below var2
	TedMan* TedOrder::jumpBelow(const TedVar& var1, const TedVar& var2) {
		if(_pManCurrent->getContainer().isSameHight(var1,var2))
			return _pManCurrent;
		int jump = _pManCurrent->getContainer().getLevel(var2)- _pManCurrent->getContainer().getLevel(var1);
		if(jump==1) {
			// it is already in the desired location
			return _pManCurrent;
		}
		return(jump>0) ? pushUp(var1,(--jump)): pushDown(var1,-jump);
	}

	TedMan* TedOrder::exchange(const TedVar& var1, const TedVar& var2) {
		if(_pManCurrent->getContainer().isSameHight(var1,var2))
			return _pManCurrent;
		int jump = _pManCurrent->getContainer().getLevel(var1)- _pManCurrent->getContainer().getLevel(var2);
		TedMan* last = NULL;
		if(jump<0) {
			jump*= -1;
			if(jump>1) {
				jump--;
				last = pushUp(var1,jump);
				//upper jumps the same size below
				jump++;
				last = pushDown(var2,jump);
			} else {
				last = pushUp(var1);
			}
		} else {
			if(jump>1) {
				//jump below upper
				jump--;
				last = pushUp(var2,jump);
				//upper jumps the same size below
				jump++;
				last = pushDown(var1,jump);
			} else {
				last = pushUp(var2);
			}
		}
		return last;
	}

	TedMan* TedOrder::bblUp(const TedVar& var) {
		unsigned int level = _pManCurrent->getContainer().getLevel(var);
		if(level == _pManCurrent->getContainer().getMaxLevel()) {
			return NULL;
		} else {
			return pushUp(var);
		}
	}

	TedMan* TedOrder::bblUp(const TedVarGroupOwner& owner) {
		map<unsigned int, TedVar*> varOrder;
		for(unsigned int i=1; i <= owner.getSize(); i++) {
			TedVarGroupMember* member = owner.getMember(i);
			unsigned int memberlevel = _pManCurrent->getContainer().getLevel(*member);
			varOrder.insert(pair<unsigned int, TedVar*>(memberlevel,member));
		}
		unsigned int maxvarlevel = varOrder.rbegin()->first;
		//unsigned int minvarlevel = varOrder.begin()->first;
		unsigned int level = maxvarlevel;
		if(level == _pManCurrent->getContainer().getMaxLevel())
			return NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManNext = NULL;
		int distance = 1;
		const TedVar downVar(pMan->getContainer().getVarAtLevel(level+1).getName());
		TedVar* pdown = _pManCurrent->_vars.getIfExist(downVar.getName());
		if(pdown && pdown->isMember()) {
			distance = (dynamic_cast<TedVarGroupMember*>(pdown))->getOwner()->getSize();
		}
		updateCurrentManager(pMan);
		for(map<unsigned int,TedVar*>::const_reverse_iterator it = varOrder.rbegin(), end = varOrder.rend(); it != end; it++) {
			TedVar* upward = it->second;
			pManNext = pushUp(*upward,distance);
			this->deleteManager(pMan);
			pMan = pManNext;
			updateCurrentManager(pMan);
		}
		return pMan;
	}

	/** @brief Bubble down a variable*/
	TedMan* TedOrder::bblDown(const TedVar& var) {
		int level = _pManCurrent->getContainer().getLevel(var);
		if(level == 1) {
			return NULL;
		} else {
			return pushDown(var);
		}
	}

	/** @brief Bubble down a grouped variable*/
	TedMan* TedOrder::bblDown(const TedVarGroupOwner& owner) {
		map<unsigned int, TedVar*> varOrder;
		for(unsigned int i=1; i <= owner.getSize(); i++) {
			TedVarGroupMember* member = owner.getMember(i);
			unsigned int memberlevel = _pManCurrent->getContainer().getLevel(*member);
			varOrder.insert(pair<unsigned int, TedVar*>(memberlevel,member));
		}
		//unsigned int maxvarlevel = varOrder.rbegin()->first;
		unsigned int minvarlevel = varOrder.begin()->first;
		unsigned int level = minvarlevel;
		if(level == 1)
			return NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManNext = NULL;
		int distance = 1;
		const TedVar upVar(pMan->getContainer().getVarAtLevel(level-1).getName());
		TedVar* pup = _pManCurrent->_vars.getIfExist(upVar.getName());
		if(pup && pup->isMember()) {
			distance = (dynamic_cast<TedVarGroupMember*>(pup))->getOwner()->getSize();
		}
		updateCurrentManager(pMan);
		for(map<unsigned int,TedVar*>::const_iterator it = varOrder.begin(); it != varOrder.end(); it++) {
			TedVar* downward = it->second;
			pManNext = pushDown(*downward,distance);
			this->deleteManager(pMan);
			pMan = pManNext;
			updateCurrentManager(pMan);
		}
		return pMan;
	}

	/** @brief Push the variable to top*/
	TedMan* TedOrder::toTop(const TedVar& var) {
		return toLocation(var, _pManCurrent->getContainer().getMaxLevel());
	}

	/** @brief Push the grouped variable to top*/
	TedMan* TedOrder::toTop(const TedVarGroupOwner& owner) {
		map<unsigned int, TedVar*> varOrder;
		for(unsigned int i=1; i <= owner.getSize(); i++) {
			TedVarGroupMember* member = owner.getMember(i);
			unsigned int memberlevel = _pManCurrent->getContainer().getLevel(*member);
			varOrder.insert(pair<unsigned int, TedVar*>(memberlevel,member));
		}
		unsigned maxvarlevel = varOrder.rbegin()->first;
		//unsigned minvarlevel = varOrder.begin()->first;
		unsigned level = maxvarlevel;
		if(level == _pManCurrent->getContainer().getMaxLevel())
			return NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManNext = NULL;
		int distance = pMan->getContainer().getMaxLevel()-level;
		updateCurrentManager(pMan);
		for(map<unsigned int,TedVar*>::const_reverse_iterator it = varOrder.rbegin(), end = varOrder.rend(); it != end; it++) {
			TedVar* upward = it->second;
			pManNext = pushUp(*upward,distance);
			this->deleteManager(pMan);
			pMan = pManNext;
			updateCurrentManager(pMan);
		}
		return pMan;
	}

	/** @brief Push the variable to Bottom*/
	TedMan* TedOrder::toBottom(const TedVar& var) {
		return toLocation(var, 1);
	}

	/** @brief Push the grouped variable to Bottom*/
	TedMan* TedOrder::toBottom(const TedVarGroupOwner& owner) {
		map<unsigned int, TedVar*> varOrder;
		for(unsigned int i=1; i <= owner.getSize(); i++) {
			TedVarGroupMember* member = owner.getMember(i);
			unsigned int memberlevel = _pManCurrent->getContainer().getLevel(*member);
			varOrder.insert(pair<unsigned int, TedVar*>(memberlevel,member));
		}
		//unsigned int maxvarlevel = varOrder.rbegin()->first;
		unsigned int minvarlevel = varOrder.begin()->first;
		unsigned int level = minvarlevel;
		if(level == 1)
			return NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManNext = NULL;
		int distance = level-1;
		updateCurrentManager(pMan);
		for(map<unsigned int,TedVar*>::const_iterator it = varOrder.begin(); it != varOrder.end(); it++) {
			TedVar* downward = it->second;
			pManNext = pushDown(*downward,distance);
			this->deleteManager(pMan);
			pMan = pManNext;
			updateCurrentManager(pMan);
		}
		return pMan;
	}


	TedMan* TedOrder::flip(const TedVarGroupOwner& owner) {
		map<unsigned int, TedVar*> varOrder;
		for(unsigned int i=1; i <= owner.getSize(); i++) {
			TedVarGroupMember* member = owner.getMember(i);
			unsigned int memberlevel = _pManCurrent->getContainer().getLevel(*member);
			varOrder.insert(pair<unsigned int, TedVar*>(memberlevel,member));
		}
		//unsigned int maxvarlevel = varOrder.rbegin()->first;
		//unsigned int minvarlevel = varOrder.begin()->first;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManNext = NULL;
		unsigned int distance = varOrder.size()-1;
		map<unsigned int, TedVar*>::const_iterator it_memberBelow = varOrder.begin();
		updateCurrentManager(pMan);
		for(; distance > 0; distance--, it_memberBelow++) {
			TedVar* goingUp = it_memberBelow->second;
			pManNext = pushUp(*goingUp,distance);
			this->deleteManager(pMan);
			pMan = pManNext;
		}
		updateCurrentManager(pMan);
		return pMan;
	}

}
