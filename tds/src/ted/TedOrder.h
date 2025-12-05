/*
 * =====================================================================================
 *
 *       Filename:  TedOrder.h
 *    Description:
 *        Created:  07/14/2011 08:56:05 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __TEDORDER_H__
#define __TEDORDER_H__ 1

#include <set>
#include <map>
#include <vector>
#include <cassert>

#include "types.h"

namespace ted {

	using namespace std;

	class Cost;
	class TedMan;
	class TedCompareCost;
	class TedVar;
	class TedVarGroupOwner;
	class TedNode;
	class TedNodeRoot;
	class ATedNode;
	class TedParents;
	typedef map<TedNode*,ATedNode*>	Ted2ATed;


	class TedOrder {
	protected:
		TedMan* _pManSteady;
		TedMan* _pManCurrent;
		bool _progressBar;
		double _stop_criteria;

		virtual TedMan* pushUp(const TedVar& v, unsigned int distance) = 0;
		virtual TedMan* pushUp(const TedVar& v) = 0;
		virtual TedMan* pushDown(const TedVar& v, unsigned int distance) = 0;
		virtual TedMan* pushDown(const TedVar& v) = 0;

		virtual TedMan* duplicateManager(TedMan* pMan);
		virtual void deleteManager(TedMan* pMan);

	public:
		typedef pair <unsigned int, bool> DINT;
		TedOrder(TedMan* pman): _pManSteady(pman), _pManCurrent(pman), _progressBar(true), _stop_criteria(0.0) {};
		virtual ~TedOrder(void) {};

		void updateCurrentManager(TedMan* pman);

		TedMan* jumpAbove(const TedVar& v1, const TedVar& v2);
		TedMan* jumpBelow(const TedVar& v1, const TedVar& v2);
		TedMan* exchange(const TedVar& v1, const TedVar& v2);
		TedMan* toLocation(const TedVar& v, unsigned int location);
		TedMan* bblUp(const TedVar& v);
		TedMan* bblUp(const TedVarGroupOwner& v);
		TedMan* bblDown(const TedVar& v);
		TedMan* bblDown(const TedVarGroupOwner& v);
		TedMan* toTop(const TedVar& v);
		TedMan* toTop(const TedVarGroupOwner& v);
		TedMan* toBottom(const TedVar& v);
		TedMan* toBottom(const TedVarGroupOwner& v);
		TedMan* flip(const TedVarGroupOwner& var);

		double stop_criteria(void) const { return _stop_criteria; };
		void stop_criteria(double v) { _stop_criteria = v; }
	};


}
#endif


