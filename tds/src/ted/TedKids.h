/*
 * =====================================================================================
 *
 *        Filename:  TedKids.h
 *     Description:  TedKids
 *         Created:  04/17/2007 01:01:26 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDKIDS_H__
#define __TEDKIDS_H__

#include <map>
#include <string>
#include <cassert>
#include "TedKid.h"
#include "TedKidsIterator.h"

#include "types.h"

namespace ted {
	using namespace std;
	class TedNode;

#define FOREACH_KID_OF_NODE_REV(pNode)\
	FOREACH_KID_OF_KIDS_REV(pNode->getKids())

#define FOREACH_KID_OF_KIDS_REV(kids)\
	if((kids)!=NULL)\
	for(TedKids::reverse_iterator _iterkids = (kids)->rbegin(), _iterend = (kids)->rend(); \
		_iterkids != _iterend; _iterkids++)

#define FOREACH_CHILD_OF_NODE(pNode,iter,iterend)\
	FOREACH_CHILD_OF_KIDS(pNode->getKids(),iter,iterend)

#define FOREACH_CHILD_OF_KIDS(kids,iter,iterend)\
	if((kids)!=NULL)\
	for(TedKids::iterator iter = (kids)->begin(), iterend = (kids)->end(); \
		iter != iterend; iter++)

#define FOREACH_KID_OF_NODE(pNode)\
	FOREACH_KID_OF_KIDS(pNode->getKids())

#define FOREACH_KID_OF_KIDS(kids)\
	if((kids)!=NULL)\
	for(TedKids::iterator _iterkids = (kids)->begin(), _iterend = (kids)->end(); \
		_iterkids != _iterend; _iterkids++)

	// index  = _iterkids.getIndex();
	// pKid   = _iterkids.Node<TedNode>();
	// weight = _iterkids.getWeight();

	//the ith kid(first)going to TedNode(second)
	typedef map<unsigned int, TedKid> Kids;

	/**
	 * @class TedKids
	 * @brief Maps a TED node edge-n child with its n TedKid child.
	 **/
	class TedKids : public Kids{
	private:
		wedge _gcd(wedge a, wedge b)const;
#ifdef _DEBUG
		int safe_guard;
#endif

		static set<TedKids*> _transitoryContainer;
	public:
		Mark<TedKids> visited;

		static TedKids* create_kids(void);
		static void destroy_kids(TedKids*);
		static void garbage_collect_unmarked(bool (Mark<TedKids>::*functor)(void) const);
		static void purge_transitory_container(void);
		static void print_transitory_container(void);
		TedKids* clone(void) const;

		TedKids(void);
		~TedKids(void);

		void replace(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg);
		void insert(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg);
		unsigned int highestOrder(void)const;
		bool has(unsigned int index)const;
		bool equal(const TedKids &)const;

		typedef iteratorKids< Kids::iterator > iterator;
		typedef iteratorKids< Kids::const_iterator > const_iterator;
		typedef iteratorKids< Kids::reverse_iterator > reverse_iterator;

		TedNode* getNode(unsigned int index);
		wedge getWeight(unsigned int index)const;
		void setWeight(unsigned int index, wedge weight);
		wedge extractGCD(void);
		void pushGCD(wedge weight);
		TedRegister minRegister(void);
		void retimeUp(TedRegister&);
		void retimeDown(TedRegister&);
		void setRegister(unsigned int index, TedRegister reg);
		TedRegister getRegister(unsigned int index)const;

		TedKids duplicate(void) const;
	};

#ifdef INLINE
#include "TedKids.inl"
#endif

}
#endif

