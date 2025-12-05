/*
 * =====================================================================================
 *
 *       Filename:  TedOrderStrategy.h
 *    Description:
 *        Created:  4/29/2009 12:11:50 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 195                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400(Wed, 07 Oct 2009)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDORDERSTRATEGY_H__
#define __TEDORDERSTRATEGY_H__

#include <list>
#include <string>
#include <ostream>
using std::list;
using std::string;
using std::ostream;

#include "TedOrder.h"
#include "RNGenerator.h"
using namespace dtl;

#ifdef _DEBUG
#define COST_CHECK_OLD \
	assert(bestCost==newCost); \
Cost* newCostTest = cmpFunc->compute_cost(**pMan); \
cmpFunc->restart(); \
Cost* newCostBest = cmpFunc->compute_cost(**pManBest); \
cmpFunc->restart(); \
if((*newCostTest != newCost)||(bestCost !=*newCostBest)) { \
	cerr << endl; \
	cerr << "Warning: Cost function \"" << cmpFunc->name()<< "\" varies over multiple iterations" << endl; \
	cerr << "         This usually happens in the MUX variable reported by GAUT" << endl; \
} \
delete newCostTest; \
delete newCostBest;
#else
#define COST_CHECK_OLD
#endif


namespace ted {

	class TedOrderStrategy : public TedOrder {
	protected:
		list<string> history;
		Cost* best_history;
		int index_history;

		void COST_CHECK(TedCompareCost* cmpFunc, TedMan** pManBest, const Cost& bestCost);
		void COST_CHECK(TedCompareCost* cmpFunc, TedMan** pMan, const Cost& newCost, TedMan** pManBest, const Cost& bestCost);

		bool _unstopable;
		unsigned int _minLevel;
		unsigned int _maxLevel;

		static const unsigned long STOP_ANNEALING = 1;

		vector< set<TedVar> > getStrides(void);
		vector< set<TedVar> > getStrides(bool);
		void getStrides(const TedNode*, map<const TedNode*,set<TedVar> >&);
		bool stopAnnealing(const Cost& c1,const Cost& c2,const Cost& c3,const Cost& c4, long temperature);

		void bestManager(TedCompareCost* compare, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost);
		virtual void updateBestManagerAnnealing(TedCompareCost* compare, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost, RNG& rng, double temperature, Cost& start_cost);
		virtual void updateBestManager(TedCompareCost* compare, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost);

		virtual TedMan* duplicateBestManager(TedMan* pMan) { return pMan; }
		virtual void deleteBestManager(TedMan* pMan) { assert(pMan); }

		string recordOrder(TedMan* pMan);
		void recordHistory(TedMan* pMan, Cost* cost, TedCompareCost* cmpFunc);

	public:
		TedOrderStrategy(TedMan* pman): TedOrder(pman), best_history(NULL), index_history(-1), _unstopable(false), _minLevel(0), _maxLevel(0) {
			defaultWindow();
		}
		~TedOrderStrategy(void);

		TedMan* siftAll(TedCompareCost* compare);
		TedMan* siftGroupedAll(TedCompareCost* compare);
		TedMan* sift(const TedVar& v, TedCompareCost* compare);
		TedMan* sift(const TedVarGroupOwner& var, TedCompareCost* compare);
		TedMan* permute(TedCompareCost* compare, bool break_into_strides);
		TedMan* permute(TedCompareCost* compare, set<TedVar>* sVars);
		TedMan* annealing(TedCompareCost* compare, bool break_into_strides,bool always_backtrack);
		TedMan* annealing(TedCompareCost* compare, set<TedVar>* sVars, bool enable_backtrack);

		void unstopable(void) { _unstopable = true; }
		void printHistory(ostream& out);
		void printHistory(void);

		void defineWindow(string lower_var, string upper_var);
		void defaultWindow(void);
	};

	class TedOrderSwap : public TedOrderStrategy {
	private:
		void buildATed(Ted2ATed  & mNodesA, \
					   const TedVar& va, const TedVar& vb, TedMan* pManTemp);
		void normalizeATed(Ted2ATed & mNodes,  TedMan* pManTemp);
		TedNode* normalizeATedNode(TedNode* pOrigNode,  ATedNode* pENode, TedMan* pManTemp);
		TedMan* reorderVariable(const TedVar & va, const TedVar & vb, TedMan* pManTemp);
		TedNode* reorderVariableRec(TedNode* pNodeTemp, \
									const TedVar & va, const TedVar &  vb, \
									set <TedNode*>* sA, set <TedNode*>* sB, \
									map <TedNode*, TedNode*>& mVisited);
		TedNodeRoot duplicateSwappingRec(TedNode* pNode, \
										 const TedVar & va, const TedVar & vb, \
										 TedMan* pManNew, TedMan* pManTemp, int iState, \
										 map <TedNode*, TedNodeRoot> & mVisited);
		TedMan* duplicateSwapping(const TedVar& va, const TedVar& vb, TedMan* pManTemp);
		TedMan* swapVariable(const TedVar& a, const TedVar& b);
	protected:
		TedMan* pushUp(const TedVar& v, unsigned int d);
		TedMan* pushUp(const TedVar& v);
		TedMan* pushDown(const TedVar& v, unsigned int d);
		TedMan* pushDown(const TedVar& v);
	public:
		TedOrderSwap(TedMan* pman): TedOrderStrategy(pman) {};
		~TedOrderSwap(void) {};
	};

	class TedOrderReloc : public TedOrderStrategy {
	private:
		TedMan* relocateVariable(const TedVar& v, unsigned int l);
	protected:
		TedMan* pushUp(const TedVar& v, unsigned int d);
		TedMan* pushUp(const TedVar& v);
		TedMan* pushDown(const TedVar& v, unsigned int d);
		TedMan* pushDown(const TedVar& v);
	public:
		TedOrderReloc(TedMan* pman): TedOrderStrategy(pman) {};
		~TedOrderReloc(void) {};
	};

	class TedOrderProper : public TedOrderStrategy {
	private:
		void reor(const TedVar& varX, const TedVar& varY);
#if 0
		void forwardWeightUp(TedNode* pNode, wedge gcd, TedParents& parents);
#endif
	protected:
		TedMan* pushUp(const TedVar& v, unsigned int d);
		TedMan* pushUp(const TedVar& v);
		TedMan* pushDown(const TedVar& v, unsigned int d);
		TedMan* pushDown(const TedVar& v);

		void updateBestManagerAnnealing(TedCompareCost* compare, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost, RNG& rng, double temperature,Cost& start_cost);
		void updateBestManager(TedCompareCost* compare, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost);

		TedMan* duplicateManager(TedMan* pMan) { return pMan; }
		void deleteManager(TedMan* pMan) { assert(pMan==_pManCurrent); }

		TedMan* duplicateBestManager(TedMan* pMan);
		void deleteBestManager(TedMan* pMan);

	public:
		TedOrderProper(TedMan* pman): TedOrderStrategy(pman) {};
		~TedOrderProper(void) {};

		void reorder(const TedVar& varX, const TedVar& varY) { reor(varX,varY); }
		void bottom(const TedVar& varX);

		// Re-stablish the order of each fully retimed variables by base variable (upward or downward)
		// i.e multiple retimes variables of a base var A are reordered (only A vars)
		TedMan* fixorderRetimedVars(bool verboseOnly=true);
		// Re-stablish the order among different variables which are presummible all downward retimed
		// no registers in the edges
		TedMan* fixorderVars(bool verboseOnly=true);
		string fixorder(PartialOrder& var_lesser_than, bool verboseOnly);
		TedMan* patchorderVars(void);
	};

}
#endif

