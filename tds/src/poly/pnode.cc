/*
 * =====================================================================================
 *
 *        Filename:  pnode.cc
 *     Description:  pnode class
 *         Created:  04/19/2007 01:06:28 PM EDT
 *          Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) (R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#include "pnode.h"

namespace polyparser {

#ifndef INLINE
#include "pnode.inl"
#endif

	PNode::PNode (void) {
		_name = "";
		_left = NULL;
		_right = NULL;
		_data  = NULL;
	}

	PNode::PNode (string n) {
		_name = n;
		_left = NULL;
		_right = NULL;
		_data  = NULL;
	}

#if 0
	PNode::PNode (string & n) {
		_name = n;
		_left = NULL;
		_right = NULL;
		_data  = NULL;
	}
#endif

	PNode::PNode (const char * n) {
		_name = n;
		_left = NULL;
		_right = NULL;
		_data  = NULL;
	}

#if 0
	PNode::PNode (string n, PNode * left, PNode * right) {
		_name = n;
		_left = left;
		_right = right;
		_data  = NULL;
	}

	PNode::PNode (string & n, PNode * left, PNode * right) {
		_name = n;
		_left = left;
		_right = right;
		_data  = NULL;
	}
#endif

	PNode::PNode (const char n, PNode * left, PNode * right) {
		_name = n;
		_left = left;
		_right = right;
		_data  = NULL;
	}


	PNode::TYPE PNode::getType(void) const {
		switch (_name[0]) {
		case PolyParser::ADD: return ADD;
		case PolyParser::SUB: return SUB;
		case PolyParser::MUL: return MUL;
		case PolyParser::DIV: return DIV;
		case PolyParser::POW: return EXP;
		case PolyParser::LSH: return LSH;
		case PolyParser::REG: return REG;
		default: return VAR;
		}
	}

	void PNode::collectDFS(vector <PNode *> & vNodes) {
		if (_left  != NULL) _left->collectDFS(vNodes);
		if (_right != NULL) _right->collectDFS(vNodes);
		vNodes.push_back(this);
	}

}
