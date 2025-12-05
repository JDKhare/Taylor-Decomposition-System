/*
 * =====================================================================================
 *
 *       Filename:  pnode.inl
 *    Description:
 *        Created:  11/8/2008 8:29:32 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 145                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500 (Thu, 18 Nov 2010)     $: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline
void PNode::setData(void * pData) {
	_data = pData;
}

inline
void * PNode::getData(void) {
	return _data;
}

inline
const char * PNode::getName(void) const {
	return _name.c_str();
}

inline
PNode * PNode::getLeft(void) const {
	return _left;
}

inline
PNode * PNode::getRight(void) const {
	return _right;
}

inline
PNode * PNode::add (PNode * op) {
	return new PNode(PolyParser::ADD, this, op);
}

inline
PNode * PNode::sub (PNode * op) {
	return new PNode(PolyParser::SUB, this, op);
}

inline
PNode * PNode::mul (PNode * op) {
	return new PNode(PolyParser::MUL, this, op);
}

inline
PNode * PNode::div (PNode * op) {
	return new PNode(PolyParser::DIV, this, op);
}

inline
PNode * PNode::exp (PNode * op) {
	return new PNode(PolyParser::POW, this, op);
}

inline
PNode * PNode::reg (PNode * op) {
	return new PNode(PolyParser::REG, this, op);
}
