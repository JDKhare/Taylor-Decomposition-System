/*
 * =====================================================================================
 *
 *       Filename:  DfgNode.inl
 *    Description:
 *        Created:  11/8/2008 8:43:40 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 208                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline
DfgNode* DfgNode::getLeft(void)const { return _left; }

inline
DfgNode* DfgNode::getRight(void)const { return _right; }

inline
bool DfgNode::isVarConst(void)const { assert(_op); return(_op->getOp() == DfgOperator::VARCONST); }

inline
bool DfgNode::isConst(void)const { assert(_op); return(_op->getOp() == DfgOperator::CONST); }

inline
wedge DfgNode::getConst(void)const { assert(isConst()); return _data.value; }

inline
bool DfgNode::isPI(void)const { assert(_op); return(_op->getOp() == DfgOperator::VAR); }

inline
const char* DfgNode::getName(void)const { assert(!isConst()); return _data.name; }

inline
bool DfgNode::isAssign(void)const { assert(_op); return(_op->getOp() == DfgOperator::EQ); }

inline
bool DfgNode::isOp(void)const { return(isAdd()|| isSub()|| isMul()|| isLSH()|| isReg()|| isAssign()); }

inline
void DfgNode::setType(DfgOperator::Type type) { assert(_op); _op->setType(type); }

inline
DfgOperator::Type DfgNode::getOp(void)const { assert(_op); return _op->getOp(); }

inline
bool DfgNode::isAdd(void)const { assert(_op); return( _op->getOp() == DfgOperator::ADD); }

inline
bool DfgNode::isSub(void)const { assert(_op); return(_op->getOp() == DfgOperator::SUB); }

inline
bool DfgNode::isLSH(void)const { assert(_op); return(_op->getOp() == DfgOperator::LSH); }

inline
bool DfgNode::isMul(void)const { assert(_op); return(_op->getOp() == DfgOperator::MUL); }

inline
bool DfgNode::isReg(void)const { assert(_op); return( _op->getOp() == DfgOperator::REG); }

inline
bool DfgNode::isAddorSub(void)const { return isAdd()||isSub(); }

inline
void DfgNode::setOpDelays(unsigned int adddelay, unsigned int subdelay, unsigned int muldelay, unsigned int lshdelay, unsigned int regdelay, unsigned int divdelay) {
	_addDelay = adddelay;
	_subDelay = subdelay;
	_mpyDelay = muldelay;
	_lshDelay = lshdelay;
	_divDelay = divdelay;
	_regDelay = regdelay;
}

inline
unsigned int DfgNode::delayAdd(void) { return _addDelay; }

inline
unsigned int DfgNode::delaySub(void) { return _subDelay; }

inline
unsigned int DfgNode::delayMul(void) { return _mpyDelay; }

inline
unsigned int DfgNode::delayDiv(void) { return _divDelay; }

inline
unsigned int DfgNode::delayLSH(void) { return _lshDelay; }

inline
unsigned int DfgNode::delayReg(void) { return _regDelay; }

inline
void DfgNode::swapKid(void) {
	DfgNode* bTemp;
	bTemp = _left;
	_left = _right;
	_right = bTemp;
}

inline
unsigned int DfgNode::numRefs(void)const { return _nrefs; }

inline
void DfgNode::incRef(void) { _nrefs ++; }

inline
unsigned int DfgNode::getArrivalTime(void)const { return _arrival_time; }

inline
void DfgNode::setArrivalTime(unsigned int d) { _arrival_time = d; }

inline
void DfgNode::incArrivalTime(unsigned int d) { _arrival_time += d; }

inline
void DfgNode::decArrivalTime(unsigned int d) { _arrival_time -= d; }

inline
unsigned int DfgNode::getRequiredTime(void)const { return _required_time; }

inline
void DfgNode::setRequiredTime(unsigned int d) { _required_time  = d; }

inline
void DfgNode::incRequiredTime(unsigned int d) { _required_time += d; }

inline
void DfgNode::decRequiredTime(unsigned int d) { _required_time -= d; }
