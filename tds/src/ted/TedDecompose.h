/*
 * =====================================================================================
 *
 *        Filename:  TedDecompose.h
 *     Description:  TED Decompose header
 *         Created:  07/16/2009 11:42:07 AM EDT
 *          Author:  Daniel Gomez-Prado
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __TEDDECOMPOSE_H__
#define __TEDDECOMPOSE_H__

#include <list>
#include <vector>
#include <deque>

#include "BinaryThreadedTree.h"
#include "TedParents.h"
#include "TedNodeRoot.h"

#include "LinkedSet.h"
using dtl::LinkedSet;

namespace ted {

	using namespace dtl;
	using namespace std;

	class TedMan;
	class TedNode;

	//typedef vector<TedNode*> chainOP;
	typedef deque<TedNode*> chainOP;
	typedef set<TedNode*> Term;

	/** @brief   Decomposition algorithm
	  @details _bottomBFS,_allTerms,oneTerm,_parents is shared across functions, so most of them
	  must be clear when the functions are invoked, and must finish clean at function exit. The
	  reason behind this, is to reuse storage allocated by the containers
	 **/
	class TedDecompose {
	private:
		TedMan* _currentManager;
		LinkedSet<TedNode*> _bottomBFS;
		//list<TedNode*> _bottomBFS;
		vector< chainOP > _allTerms;
		chainOP _oneTerm;
		TedParents _parents;
		bool _onlyPOs;

		void findProductSupport(void);
		void extractProductSupport(BinaryThreadedTree<int,chainOP>&);

		void findSumSupport(void);
		void extractSumSupport(BinaryThreadedTree<int,chainOP>&);
		TedNode* updateReplicatedTermReference(chainOP& currentTerm, unsigned int index);
		void updateFutureChainOP(TedNode*, const TedVar&, \
								 BinaryThreadedTree<int,chainOP>::inorder_iterator, \
								 BinaryThreadedTree<int,chainOP>::inorder_iterator );

		void removeParents(chainOP& currentTerm);

		void getParents(void);
		void removeTermFromContainer(chainOP& currentTerm);
		void updateReferences(TedNode* oldTop, TedNode* newTop);
		void updateParentReferences(TedNode* oldTop, TedNode* newTop);
		void updatePoReferences(TedNode* oldTop, TedNode* newTop);
		void mergeReplicatedTerm(TedNodeRoot::Type type, TedNode* oldTop, TedNode* newTop, TedKids& kids);
		void annotateBackptr(TedNode*,TedNode*);

	public:
		TedDecompose(TedMan* manager, bool onlyPOs = true);
		~TedDecompose(void);

		bool isIrreducible(void);

		bool productTerm(void);
		bool sumTerm(void);
		bool allTermsWithoutReorder(void);
		list<string> getOrder(void);

		static unsigned int _st_count;
		static unsigned int _pt_count;
	};

}
#endif

