/*
 * =====================================================================================
 *
 *        Filename:  pnode.h
 *     Description:  Poly Node for binary operation
 *         Created:  04/19/2007 01:02:33 PM EDT
 *          Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) (R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __PNODE_H__
#define __PNODE_H__

#include <string>
#include <vector>
using namespace std;

#include "poly.h"

namespace polyparser {

	/**
	 * @class PNode
	 * @brief Implements the tree node and its operations for the parsed polynomial.
	 **/
	class PNode {
	private:
		string _name;
		PNode * _left;
		PNode * _right;
		void * _data;
	public:
		enum TYPE {ADD, SUB, MUL, DIV, EXP, LSH, VAR, REG};
		PNode (void);
		PNode (const char *);
		PNode (string);

		PNode (const char name, PNode * left, PNode * right);
#if 0
		PNode (string & name, PNode * left, PNode * right);
		PNode (string   name, PNode * left, PNode * right);
#endif
		const char * getName(void) const;
		PNode * getLeft(void) const;
		PNode * getRight(void) const;

		PNode * add(PNode *);
		PNode * sub(PNode *);
		PNode * mul(PNode *);
		PNode * div(PNode *);
		PNode * exp(PNode *);
		PNode * reg(PNode *);

		TYPE getType(void) const;
		void collectDFS(vector <PNode *> &);
		void setData(void *);
		void * getData(void);
	};

#ifdef INLINE
#include "pnode.inl"
#endif

}
#endif

