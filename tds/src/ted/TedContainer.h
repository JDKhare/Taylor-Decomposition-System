/*
 * =====================================================================================
 *
 *       Filename:  TedContainer.h
 *    Description:
 *        Created:  12/8/2008 1:34:31 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDCONTAINER_H
#define __TEDCONTAINER_H

#include <map>
#include <set>
#include <vector>
#include "DfgMan.h"
#include "TedVar.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedNode.h"
#include "TedKids.h"

namespace ted {
	class ETedNode;

	typedef set<TedNode*> TedSet;
	typedef map<unsigned int,pair<TedVar*,TedSet*> > TedContainerOrder;
	typedef map<TedVar,set<TedVar> > PartialOrder;
	typedef map<const TedVar*,set<TedVar> > BaseVarsRetimed;


	/**
	 * @brief Groups set of nodes by their TED variable
	 * @details The key element used(TedVar)does not distinguish among simple TedVar, grouped, or owner.
	 *          All this information is lost when casting to TedVar(the key used), if that information needs
	 * 			to be retrieved, one can used base_varVariables and pass the name of the variable.
	 * @author Daniel Gomez-Prado
	 **/
	class TedContainer : public map<TedVar, pair<unsigned int,TedSet*> > {
	public:
		TedContainer(void) {};
		~TedContainer(void);

		TedContainerOrder order(void);
		bool isFirstLower(const TedNode* v1, const TedNode* v2)const;
		bool isFirstLower(const TedVar& v1, const TedVar& v2)const;
		bool isFirstHigher(const TedNode* v1, const TedNode* v2)const;
		bool isFirstHigher(const TedVar& v1, const TedVar& v2)const;
		bool isSameHight(const TedNode* v1, const TedNode* v2)const;
		bool isSameHight(const TedVar& v1, const TedVar& v2)const;
		size_t getLevel(const TedVar& v)const;
		size_t nodeCount(void)const;
		//TedNode* getNode(const TedNode* pnode);
		TedNode* getNode(ETedNode* pnode);
		TedNode* getNode(const TedVar& v, const TedKids& k, const TedRegister& r = 0);
		size_t getMaxLevel(void)const;
		void registerVar(const TedVar& var);
		void registerVarAtTop(const TedVar& var);
		void registerRetimedVar(const TedVar& var);
		void registerRetimedVarAt(const TedVar& var, const TedVar& atVar);
		const TedVar& getVarAtLevel(unsigned int);
		void rotateLevels(const TedVar&, const TedVar&);
		void removeLevel(const TedVar&);
		bool isEmpty(void)const;

		bool isAtTop(const TedVar& var) const;
		bool isAtBottom(const TedVar& var) const;

		bool hasNode(TedNode* pNode) const;
		bool hasNode(const TedVar& pNode_var, const TedKids& pNode_kids, const TedRegister& pNode_reg) const;
		bool eraseNode(TedNode* pNode);
		bool existsOrOne(TedNode* pNode) const;
		bool check_children_are_included(void) const;
		void update_mark(void(Mark<TedNode>::*functor)(void));
		void update_TMark(void);
		void update_PMark(void);
		void print_container(void) const;

		map< const TedVar*,set<TedVar> > getExistingRetimedVarsPerBaseClass(void);
		PartialOrder getPartialOrderLT(set<TedVar>& var_set,const TedVar* base_var = NULL);
		PartialOrder derivePartialOrderLT(set<TedVar>& var_set,const TedVar* base_var = NULL);
		PartialOrder derivePartialOrderGT(set<TedVar>& var_set,const TedVar* base_var = NULL);
		void optimizePartialOrderLT(PartialOrder& guide);
		int findMaxDepth(PartialOrder&, TedVar var);
		void keepMaxDepth(PartialOrder& guide, TedVar var);

		PartialOrder deriveOrderLT(void);
		PartialOrder getOrderLT(void);
		PartialOrder patchOrderLT(void);
		PartialOrder patchOrderGT(void);

		bool check_all_nodes_are_marked_at_var(const TedVar& var) const;
	};

#ifdef INLINE
#include "TedContainer.inl"
#endif

}

#endif

