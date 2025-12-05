/*
 * =====================================================================================
 *
 *       Filename:  Node.inl
 *    Description:
 *        Created:  02/11/2010 10:40:09 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 194                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-06-14 22:41:02 -0400 (Thu, 14 Jun 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline
void   Node::setName(const char * name) {
	_name = name;
}

inline
NODE_TYPE Node::getType(void) {
	return _type;
}

inline
void   Node::setType(NODE_TYPE type) {
	_type = type;
}

inline
bool Node::isConst(void) {
	return (_type == CONST);
}

inline
bool Node::isConstVar(void) {
	return (_type == CONSTVAR);
}

inline
bool Node::isPI(void) {
	return (_type == PI || _type == CONST || _type == CONSTVAR);
}

inline
bool Node::isPO(void) {
	return _type == PO;
}

inline
bool Node::isTemporary(void) {
	return (_type == TEMPORARY);
}

inline
bool Node::isVar(void) {
	return (_type == VAR);
}

inline
bool Node::isOperator(void) {
	return (_type == OPERATOR);
}

inline
bool Node::isInterface(void) {
	return (_type == TED);
}

inline
const char * Node::func(void) {
	return _func.c_str();
}

inline
void Node::setFunc(const char * func) {
	_func = func;
}

inline
bool Node::isAdd(void) {
	return _func == FUNC_ADD;
}

inline
bool Node::isSub(void) {
	return _func == FUNC_SUB;
}

inline
bool Node::isMul(void) {
	return _func == FUNC_MUL;
}

inline
bool Node::isReg(void) {
	return _func == FUNC_REG;
}

inline
bool Node::isSll(void) {
	return _func == FUNC_SLL;
}

inline
bool Node::isArithmetic(void) {
	return isAdd()||isSub()||isMul();
}

inline
bool Node::isAssign(void) {
	return _func == FUNC_ASSIGN;
}
inline
void Node::addFanin (Node * fin ) {
	_fanins.push_back(fin);
}

inline
void Node::addFanout(Node * fout) {
	_fanouts.push_back(fout);
}

inline
Node * Node::getFanin (unsigned i) {
	assert (_fanins.size()  > i);
	return _fanins[i];
}

inline
size_t Node::numFanins(void) {
	return _fanins.size();
}

inline
Node * Node::getFanout(unsigned i) {
	assert (_fanouts.size() > i);
	return _fanouts[i];
}

inline
size_t Node::numFanouts(void) {
	return _fanouts.size();
}

inline
void Node::clearFanin(void) {
	_fanins.clear();
}

inline
void Node::clearFanout(void) {
	_fanouts.clear();
}

inline
void * Node::getData(void) {
	return _pData;
}

inline
void   Node::setData(void * pData) {
	_pData = pData;
}

inline
void Node::setData(const CDFGData& data) {
	_data = data;
}

inline
int Node::getArrTime(void) {
	return _arrTime;
}

inline
int Node::getReqTime(void) {
	return _reqTime;
}

inline
void Node::setReqTime(int t) {
	_reqTime = t;
}

inline
void Node::clearArrReqTime(void) {
	_arrTime = -1;
	_reqTime = -1;
}

inline
bool Node::isCleanArrReqTime(void) {
	return (_arrTime == -1 && _reqTime == -1);
}

inline
int Node::NumRefs(void) {
	return _nRefs;
}

inline
void Node::incRef(void) {
	_nRefs++;
}

inline
void Node::decRef(void) {
	_nRefs--;
}
