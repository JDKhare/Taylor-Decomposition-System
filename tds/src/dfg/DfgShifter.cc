/*
 * =====================================================================================
 *
 *       Filename:  DfgShifter.cc
 *    Description:  DfgShifter class
 *        Created:  08/27/2009 12:27:36 PM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08)
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */
#include <cmath>
#include <cstring>

#include "util.h"
using util::Util;

#include "Environment.h"
using tds::Environment;

#include "DfgShifter.h"
#include "DfgMan.h"
namespace dfg {

#ifndef INLINE
#include "DfgShifter.inl"
#endif

	DfgShifter::DfgShifter(DfgMan* manager, int maxLevel, bool isCo2):_currentManager(manager), _maxShiftLevel(maxLevel), _isCo2(isCo2) {
		assert(_currentManager);
		_constName = Environment::getStr("const_prefix");
		_constName += "2";

	}

	DfgShifter::~DfgShifter(void) {
		_currentManager = NULL;
		_shiftNodes.clear();
	}

	/**
	 * @brief DfgShifter::markShifters Mark and collect the nodes that can be replaced by shifters. See DfgNode::markShifters
	 * @param vNodes Container that receives the marked selection of multipliers and node "Co2".
	 */
	void DfgShifter::markShifters(void) {
		_shiftNodes.clear();
		Mark<DfgNode>::newMark();
		for(map <string, DfgNode*>::iterator mp = _currentManager->_mPos.begin(); mp!= _currentManager->_mPos.end(); mp++) {
			markShifters(mp->second);
		}
	}

	/**
	 * @brief recursively traverse the nodes in the DFG marking at its backtrack
	 * 		  the nodes involved in the creation of shifters. It also propagates up the node ptr, of the
	 * 		  left child of the shifter.
	 *
	 * @param vNodes Container that stores the nodes that can be implemented by shifters.
	 *
	 * @return The shift level of the DfgNode. Used it the recursive traversal, and saved in _level.
	 *
	 * @details  The nodes are marked while backtracking.
	 * 				- Level 0 is node "Co2".
	 * 				- Level 1 are all nodes which connect directly to "Co2"(at least one edge).
	 * 				- Level 2 any operand which connects to a node Level 1, and the other is level >= 1.
	 * 				  All operands Level 1 and 2 contribute to the chain of shifters.
	 * 				- Level 3 any operand which connects to a node Level 2, and the other is level >= 2.
	 *                Multipliers Level 3, might contribute if they have a predecessor Level 2 which is an adder.
	 * 				  Adders Level 3 are not part of the chain.
	 * 				- Level 4 are operands that no longer belong to the shifter.
	 * 				- Level 5 or more are normal variables and constants.(for all purposes this could be 4 as well),
	 * 				  so operands relying on those increment its level by 1.
	 *
	 * @warning The case that both levels connect to a level 1 node, is not considered. So we stop collecting
	 * 			at level 2. We strongly rely in the assumption that at level 2, only one of the children
	 *          connect to a level 1 node.
	 * @note 	We can not use data._value instead of _level, because we must be able to mark VAR and CONST, and these
	 *          nodes use data._value to store their name and coeficient, respectively.
	 **/
	int DfgShifter::markShifters(DfgNode* pNode) {
		if(pNode->_visited.isMarked())
			return pNode->_level;
		switch(pNode->getOp()) {
		case DfgOperator::ADD:
		case DfgOperator::SUB:
		case DfgOperator::MUL:
			{
				int left_level = markShifters(pNode->getLeft());
				int right_level = markShifters(pNode->getRight());
				// From both left and right subgraphs, choose the min level.
				pNode->_level = MIN(left_level, right_level)+1;
				if(pNode->_level<5 &&(left_level>=5 || right_level>=5))
					pNode->_level = 4;
				// propagate up the left child of the shifter(the node to be shifted).
				// i.e:
				// in a*8 that becomes a<<3, a is the left child
				// in(a*b)*8 that becomes(a*b)<<3,* is the left child \
				// in a*(b*8)that becomes a*(b<<3), b is the left child  > depends on DFG structure
				// in(a*8)*b that becomes(a<<3)*b, a is the left child /
				break;
			}
		case DfgOperator::LSH:
		case DfgOperator::RSH:
			throw(string("01011. The DFG already contains shifters"));
			break;
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			{
				if(!strcmp(pNode->_data.name,_constName.c_str())) {
					//Variable "co2" has level 0
					pNode->_level = 0;
				} else  {
					//Other variables do not increment the DFG level.
					pNode->_level = 5;
				}
				break;
			}
		case DfgOperator::CONST:
			//Constant values do not increment the DFG level.
			pNode->_level = 0;
			break;
		default:
			throw(string("01015. UNKOWN DFG node type."));
		}
		// pair "this" node with current "level".
		//allow marking adders as head of the shifter chain
		if(pNode->_level < 5)
			_shiftNodes.insert(pair<int, DfgNode*>(pNode->_level,pNode));
		// mark this path as traversed
		pNode->_visited.setMark();
		return pNode->_level;
	}

	/**
	 * @brief computes the amount of bits to shift to the left.
	 *
	 * @return The amount of bits to shift to the left. Used in the recursive traversal, and saved in _level.
	 */
	wedge DfgShifter::computeShift(DfgNode* pNode, unsigned int& addsubCount) {
		if(pNode->_visited.isMarked())
			return pNode->_value;
		wedge left_value = 0;
		wedge right_value = 0;
		switch(pNode->getOp()) {
		case DfgOperator::DIV:
			throw(string("01030. Right shift has not been implemented yet."));
			break;
		case DfgOperator::MUL:
			left_value = !pNode->getLeft()->isAddorSub() ? computeShift(pNode->getLeft(),addsubCount): 0;
			right_value = !pNode->getRight()->isAddorSub() ? computeShift(pNode->getRight(),addsubCount): 0;
			// From both left and right subgraphs, aggregate the shift amount
			pNode->_value = left_value +right_value;
			break;
		case DfgOperator::ADD:
		case DfgOperator::SUB:
			pNode->_arrival_time = ++addsubCount;
			if((pNode->getLeft()->getLevel()<5)&&(!pNode->getLeft()->_visited.isWaterMarked()|| pNode->getLeft()->isAddorSub()))
				left_value = computeShift(pNode->getLeft(),addsubCount);
			if((pNode->getRight()->getLevel()<5)&&(!pNode->getRight()->_visited.isWaterMarked()|| pNode->getRight()->isAddorSub()))
				right_value = computeShift(pNode->getRight(),addsubCount);
			pNode->_required_time = addsubCount;
			left_value = computeKidValue(pNode->getLeft(),left_value);
			right_value = computeKidValue(pNode->getRight(),right_value);
			// Traverse both subgraphs, to guarantee a chain of shifters has only one head
			// That is, force marking all elements of a chain
			if(pNode->isAdd())
				pNode->_value = left_value+right_value;
			else
				pNode->_value = left_value-right_value;
			break;
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			if(!strcmp(pNode->_data.name,_constName.c_str())) {
				// Variables and Constant values do not increment the DFG level
				pNode->_value = 1;
			} else {
				pNode->_value = 0;
			}
			break;
		case DfgOperator::CONST:
			pNode->_value = pNode->_data.value;
			break;
		case DfgOperator::LSH:
		case DfgOperator::RSH:
			throw(string("01029. Attempting to compute the shift of a shift."));
			break;
		default:
			throw(string("01019. UNKNOWN DFG node type."));
			break;
		}
		pNode->_visited.setMark();
		return pNode->_value;
	}

	wedge DfgShifter::computeKidValue(DfgNode* pNode, wedge value) {
		switch(pNode->getOp()) {
		case DfgOperator::MUL:
			if((pNode->getLevel()<5)&& !pNode->_visited.isWaterMarked())
				value = (int)pow((long double)2, (long double)value);  // is a MPY in the chain, NOT a head of chain MPY
			else
				value = 0; // is the head of chain MPY
			break;
		case DfgOperator::ADD:
		case DfgOperator::SUB:
			value = pNode->getValue();	// is an already computed kid value
			break;
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			if(!strcmp(pNode->getName(),_constName.c_str()))
				value = 2;  //is the variable "co2"
			else
				value = 0;	//is any other variable
			break;
		case DfgOperator::CONST:
			value = pNode->getConst(); // is a constant
			break;
		case DfgOperator::DIV:
		case DfgOperator::LSH:
		case DfgOperator::RSH:
		default:
			value = 0;
			break;
		}
		return value;
	}

	/**
	 * @brief DfgShifter::remapShifter converts the multipliers by "Co2" constants into shifters.
	 * @details  First mark and group the the nodes to shift, see markShifter.
	 *			 Then computes the shift amount(see DfgNode::computeShift)marking the nodes that at the end need to be deleted.
	 *			 The nodes that are left unmarked are replaced by shifters.
	 * @warning It assumes the shift operator is a variable with name Environment.getStr("const_prefix")+"2".
	 * @todo    In the CSD all constants are passed to "Co2" notation, independently if they are used to multiply
	 *          or to add. In this algorithm we are assuming that at the top of a Co2 is always a* operation,
	 *          and never a +. If we do something like a*b+8; and 8 is passed to "Co2"; the current algorithm
	 *          will create the shifter for Co2, and then delete it. This needs to be fixed, here or at the CSD.
	 */
	void DfgShifter::remapShifter(void) {
		markShifters();
		Mark<DfgNode>::newMark();
		for(multimap<int,DfgNode*>::reverse_iterator it = _shiftNodes.rbegin(),
			end = _shiftNodes.rend(); it != end; it++) {
			DfgNode* pNode = it->second;
			if(pNode->_visited.isMarked()|| pNode->isPI()|| pNode->isVarConst())
				continue;
			unsigned int addsubCount = 0;
			computeShift(pNode,addsubCount);
			// Un-marks the head of the shifter. As computeShift runs recursively, all components belonging
			// to this shifter have been marked, therefore this for loop will not hit those cases again.
			//it->second->_visited.cleanMark();
			pNode->_visited.setWaterMark();
		}
		// replace the nodes by shifters
		for(multimap<int, DfgNode*>::reverse_iterator it = _shiftNodes.rbegin(); it != _shiftNodes.rend(); it++) {
			DfgNode* headOfChain = it->second;
			if(headOfChain->_visited.isWaterMarked()) {
				// Unmarked nodes are the head of the shift chain.
				// "left operand" << "right operand"
				if(headOfChain->isAddorSub()) {
					// There is no point in implementing a constant as a chain of shifters
					// recompute its constant value and revert to it
					revertToConst(headOfChain);
				} else if(headOfChain->isMul()) {
					// Members of the shifter chain are marked,
					// therefore the "left operand" of the shifter
					// should be the member that is not marked
					bool leftOpInChain = headOfChain->getLeft()->_visited.isMarked()|| headOfChain->getLeft()->_visited.isWaterMarked();
					DfgNode* baseLeft = NULL;
					DfgNode* baseRight = NULL;
					if(leftOpInChain) {
						baseLeft = headOfChain->getRight();
						baseRight = headOfChain->getLeft();
					} else {
						baseLeft = headOfChain->getLeft();
						baseRight = headOfChain->getRight();
					}
					if(0==headOfChain->getValue()&& baseRight->getRequiredTime()+1>_maxShiftLevel) {
						//the form is X<<6+x<<4-X<<2, but the number of shifters required exceed
						//the maximum allowed, therefore, it is implemented as X*(constant value)
						DfgNode* opRight = new DfgNode(_currentManager,baseRight->getValue());
						headOfChain->setKids(baseLeft,opRight);
						baseRight->_visited.cleanMark();
					} else if(baseRight->isAddorSub()) {
						//the form is X<<6+X<<4-x<<2
						remapShifterChain(headOfChain,baseLeft,baseRight);
					} else {
						//the form is X<<2,
						//we can change the type of headOfChain, from* to <<
						DfgNode* opRight = new DfgNode(_currentManager,headOfChain->getValue());
						headOfChain->setKids(baseLeft,opRight);
						headOfChain->setType(DfgOperator::LSH);
					}
				}
				headOfChain->_level = 6;
				// else {
				//  marked nodes, are part of a chain of shifters but no longer needed.
				//  These nodes are deleted at the end, not here; as we might generate
				//  dangling pointers. We use them in getLeft(), getRight().
				// }
			}
		}
		// delete no longer needed nodes.
		_currentManager->cleanUp();
		_currentManager->updateDelayAndNumRefs();
	}

	void DfgShifter::revertToConst(DfgNode* headOfChain) {
		assert(headOfChain->isAddorSub());
		DfgNode* chain = headOfChain;
		unsigned int depth = headOfChain->getRequiredTime();
		unsigned int current = headOfChain->getArrivalTime();
		int value = headOfChain->getValue();
		bool negate = (value>0) ? false : true;
		while(current<depth) {
			current++;
			if(backtrackConstChain(chain->getLeft(),current,depth)) {
				chain = chain->getLeft();
			} else if(backtrackConstChain(chain->getRight(),current,depth)) {
				chain = chain->getRight();
			} else {
				throw(string("01013. Can't revert to constant, lost while backtracking chain of ADD/SUB"));
			}
		}
		DfgNode* left_operand = getLeftOpConstChain(chain);
		if(!left_operand && chain!=headOfChain)
			left_operand = getLeftOpConstChain(headOfChain);
		if(!left_operand)
			throw(string("01014. Can't revert to constant, lost site of the last left operand"));
		if(negate) {
			value*= -1;
			headOfChain->setType(DfgOperator::SUB);
		}  else {
			headOfChain->setType(DfgOperator::ADD);
		}
		DfgNode* right_operand = new DfgNode(_currentManager,value);
		headOfChain->setKids(left_operand,right_operand);
		headOfChain->_visited.cleanMark();
	}

	DfgNode* DfgShifter::getLeftOpConstChain(DfgNode* chain) {
		if((chain->getLeft()->isOp()&& chain->getLeft()->getLevel()>=5)||
		   ((chain->getLeft()->isPI()|| chain->getLeft()->isVarConst())&& strcmp(chain->getLeft()->getName(),_constName.c_str()))) {
			return chain->getLeft();
		} else if((chain->getRight()->isOp()&& chain->getRight()->getLevel()>=5)||
				  ((chain->getRight()->isPI()|| chain->getRight()->isVarConst())&& strcmp(chain->getRight()->getName(),_constName.c_str()))) {
			return chain->getRight();
		}
		return NULL;
	}

	void DfgShifter::remapShifterChain(DfgNode* headOfChain, DfgNode* baseLeft, DfgNode* baseRight) {
		assert(headOfChain->isMul());
		DfgNode* chain = headOfChain;
		DfgNode* nextChain = baseRight;
		DfgNode* nnextChain = NULL;
		DfgNode* shiftChain = NULL;
		while(chain) {
			if(nextChain) {
				DfgNode* nextLeft = nextChain->getLeft();
				DfgNode* nextRight = nextChain->getRight();
				if(nextLeft->isAddorSub()&& nextRight->isAddorSub()) {
					throw(string("01001. Both subgraphs of the ADD/SUB are chain of shifters"));
				} else if(nextLeft->isAddorSub()) {
					shiftChain = getShifterChain(nextRight,baseLeft);
					chain->setType(nextChain->getOp());
					//next chain comes at left(ADD/SUB)
					//shiftChain comes at right, computed from right leave
					chain->setKids(nextChain,shiftChain);
					nnextChain = nextLeft;
				} else if(nextRight->isAddorSub()) {
					shiftChain = getShifterChain(nextLeft,baseLeft);
					chain->setType(nextChain->getOp());
					//next chain comes at right(ADD/SUB)
					//shiftChain comes at left, computed from left leave
					chain->setKids(shiftChain,nextChain);
					nnextChain = nextRight;
				} else {
					// is the last ADD/SUB in the chain,
					// take the left element,
					// next iteration will take the right element
					shiftChain = getShifterChain(nextLeft,baseLeft);
					chain->setType(nextChain->getOp());
					//shiftChain comes at left, computed from left leave
					//at right will come the last shifter "nextChain"
					chain->setKids(shiftChain,nextChain);
					nnextChain = NULL;
				}
			} else {
				// take the right element
				// the last shifter in the chain
				//if(_isCo2) {
				//	chain->_data.value = pow((long double)2,(int)chain->getRight()->getValue());
				//	chain->setType(DfgOperator::CONST);
				//	chain->setKids(NULL,NULL);
				//} else {
				DfgNode* opRight = new DfgNode(_currentManager,chain->getRight()->getValue());
				chain->setType(DfgOperator::LSH);
				chain->setKids(baseLeft,opRight);
				//}
			}
			chain->_visited.cleanMark();
			chain = nextChain;
			nextChain = nnextChain;
		}
	}


	DfgNode* DfgShifter::getShifterChain(DfgNode* pNode, DfgNode* baseLeft) {
		DfgNode* opRight = new DfgNode(_currentManager,pNode->getValue());
		return new DfgNode(_currentManager,DfgOperator::LSH,baseLeft,opRight);
	}

	DfgNode* DfgShifter::getShifterChain(DfgNode* pNode) {
		return new DfgNode(_currentManager,pow((long double)2,(long double) pNode->getValue()));
	}

	bool DfgShifter::backtrackConstChain(DfgNode* pNode, int current, int depth) {
		return pNode->isAddorSub()&&(pNode->getLevel()< 5)&&
			(pNode->getArrivalTime() ==current)&&(pNode->getRequiredTime() ==depth);
	}

}
