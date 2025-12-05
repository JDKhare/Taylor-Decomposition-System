/*
 * =====================================================================================
 *
 *       Filename:  TedNodeParent.h
 *    Description:
 *        Created:  12/10/2008 12:19:42 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDPARENTS_H__
#define __TEDPARENTS_H__

#include "TedContainer.h"
#include "TedNode.h"
#include <map>
#include <vector>
#include <cassert>
#include <algorithm>

namespace ted {

	typedef std::vector<unsigned int> MyIndex;

	class ParentOrder {
	private:
		const TedContainer* _container;
	public:
		explicit ParentOrder(const TedContainer* c) : _container(c) { assert(_container); }
		~ParentOrder(void) {}
		bool operator()(const TedNode* lhs,const TedNode* rhs) const {
			return _container->isFirstLower(lhs,rhs);
		}
	};


	// The vector<unsigned int> could be optimized in case of linear TEDs
	// as pair<unsigned int, unsigned int> - this will be slightly faster
	class MyParent : public std::map<TedNode*,MyIndex*> {
	public:
		MyParent(void) {};
		~MyParent(void);
		void registerKey(TedNode* pNode);
		void unregisterKey(TedNode* pNode);
		void registerParent(TedNode* pNode, unsigned int edge);
		void unregisterParent(TedNode* pNode, unsigned int edge);
		size_t numParents(void) const;
	};


	// The vector<unsigned int> could be optimized in case of linear TEDs
	// as pair<unsigned int, unsigned int> - this will be slightly faster
	class MyParentOrder : public std::map<TedNode*,MyIndex*,ParentOrder> {
	public:
		MyParentOrder(const TedContainer* c) : map<TedNode*,MyIndex*,ParentOrder>(ParentOrder(c)) {};
		~MyParentOrder(void) {};

		MyParentOrder& operator= (MyParent& parent) {
			for(MyParent::iterator it = parent.begin(), end = parent.end(); it!=end; it++) {
				TedNode* pNode = it->first;
				MyIndex* pIndex = it->second;
				this->insert(std::pair<TedNode*,MyIndex*> (pNode,pIndex) );
			}
			return (*this);
		}
	};

	class TedParents : public std::map<TedNode*,MyParent*> {
		enum TYPE { LINEAR, NONLINEAR };
		TYPE _type;
	public:
		TedParents(void) : _type(LINEAR) {};
		~TedParents(void);
		void collect(TedNode* pNode, TedNode* parent, unsigned int edge);
		bool isLinear(void) const;
		void registerKey(TedNode* pNode);
		void unregisterKey(TedNode* pNode);
		void registerParent(TedNode* pNode, TedNode* parent, unsigned int edge);
		void unregisterParent(TedNode* pNode, TedNode* parent, unsigned int edge);
		bool hasParents(TedNode* pNode) const;
		size_t numParents(TedNode* pNode) const;

		bool operator== (const TedParents& other) const;
		bool operator!= (const TedParents& other) const;
		void print_parents(void) const;
	};

	// ALWAYS INLINE THE FOLLOWING FUNCTIONS
#include "TedParents.inl"

}
#endif
