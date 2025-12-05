/*
 * =====================================================================================
 *
 *       Filename:  DfgShifter.h
 *    Description:  Shifter;
 *        Created:  08/26/2009 12:28:17 PM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08)
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400(Wed, 07 Oct 2009)$: Date of last commit
 * =====================================================================================
 */

#ifndef __DFGSHIFTER_H__
#define __DFGSHIFTER_H__

#include <map>
#include "DfgNode.h"
#include "types.h"

/**
 * @namespace dfg
 * @brief The Data Flow Graph data structure representation of the network or ted.
 **/
namespace dfg {

	using namespace std;

	class DfgMan;

	class DfgShifter {
	private:
		DfgMan* _currentManager;
		string _constName;
		int _maxShiftLevel;
		bool _isCo2;
		multimap <int, DfgNode*> _shiftNodes;

		wedge computeShift(DfgNode* pNode, unsigned int& addsubCount);
		wedge computeKidValue(DfgNode* pNode, wedge value);
		int markShifters(DfgNode* pNode);
		void markShifters(void);
		void revertToConst(DfgNode* headOfChain);
		bool backtrackConstChain(DfgNode* chain, int current, int depth);
		DfgNode* getLeftOpConstChain(DfgNode* pNode);
		void remapShifterChain(DfgNode* headOfChain, DfgNode* baseLeft, DfgNode* baseRight);

		DfgNode* getShifterChain(DfgNode* pNode, DfgNode* baseLeft);
		//NOTE in remapShiftChain, we can chain getShifterChain(*,basLeft)by getShifterChain(*)
		//to change its behavior.
		DfgNode* getShifterChain(DfgNode* pNode);
	public:
		explicit DfgShifter(DfgMan* manager,int maxLevel, bool isCo2 = false);
		~DfgShifter(void);

		void remapShifter(void);
	};


#ifdef INLINE
#include "DfgShifter.inl"
#endif

}
#endif
