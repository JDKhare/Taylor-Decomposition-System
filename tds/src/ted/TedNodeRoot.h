/*
 * =====================================================================================
 *
 *       Filename:  TedNodeRoot.h
 *    Description:
 *        Created:  12/10/2008 12:11:50 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-05-17 12:55:05 -0400(Mon, 17 May 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDNODEROOT_H__
#define __TEDNODEROOT_H__

#include "TedNode.h"

namespace ted {


	/*
	 * @class TedNodeRoot
	 * @brief The Function output of TEDs has two data members:
	 *  - _pNode is the TedNode the pin points to
	 *  - _weight is the weight of the node
	 */
	class TedNodeRoot {
	public:
		// PO Primary Output
		// PT Product Term
		// ST Sum Term
		// TC Top Cut
		// BC Bottom Cut
		// ET Error TED
		// EV Expected Value
		// PS Primary Support
		enum Type { PO, PT, ST, TC, BC, ET, EV, PS };
	private:
		TedNode* _pNode;
		wedge       _weight;
		Type			_type;
		TedRegister _register;
		TedNode* _link;
		Bitwidth* _bitw;
		double _error;
	public:
		/** @brief Construct an empty TedNodeRoot*/
		TedNodeRoot(void): _pNode(NULL), _weight(0), _type(PO), _register(0), _link(NULL), _bitw(NULL), _error(0.0) {};
		/** @brief Construct a TedNodeRoot with Node, weight and default type PO*/
		TedNodeRoot(TedNode* p, wedge n): _pNode(p), _weight(n), _type(PO), _register(0), _link(NULL), _bitw(NULL), _error(0.0) {};
		/** @brief Construct a TedNodeRoot with Node, weight and type*/
		TedNodeRoot(TedNode* p, wedge n, Type type): _pNode(p), _weight(n), _type(type), _register(0), _link(NULL), _bitw(NULL), _error(0.0) {};
		TedNodeRoot(const TedNodeRoot& other) : _pNode(other._pNode), _weight(other._weight), _type(other._type), _register(other._register), _link(other._link), _bitw(other._bitw), _error(other._error) {}
		~TedNodeRoot(void) { _pNode = NULL; _link=NULL; _bitw=NULL; }
		void Node(TedNode*);
		TedNode* Node(void)const;
		wedge getWeight(void)const;
		Type getType(void)const;
		TedRegister getRegister(void)const;
		TedNode* getLink(void);
		void setLink(TedNode* pnode);
		void setWeight(wedge n);
		void setType(Type type);
		void setRegister(const TedRegister& val);
		Bitwidth* getBitwidth(void)const;
		void setBitwidth(Bitwidth*);
		void setError(double);
		double getError(void)const;
	};

	// ALWAYS INLINE THE FOLLOWING FUNCTIONS
#include "TedNodeRoot.inl"

}
#endif
