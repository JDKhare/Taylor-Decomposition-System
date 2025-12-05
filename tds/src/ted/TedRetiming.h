/*
 * =====================================================================================
 *
 *       Filename:  TedRetiming.h
 *    Description:
 *        Created:  05/18/2010 11:43:17 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __TEDRETIMING_H__
#define __TEDRETIMING_H__ 1

#include "TedParents.h"

#include <list>
#include <climits>
using std::list;

namespace ted {

	class TedMan;
	class TedNode;
	class TedVar;

	class TedRetiming {
	public:
		enum Type {ALL,AVERAGE};
	private:
		TedMan* _currentManager;
		TedParents _parents;

		bool has_retimed_child_same_var(TedNode* node);
		bool has_retimed_parent_same_var(TedNode* node);
		list<TedNode*> get_retimed_child_same_var(TedNode* node);
		list<TedNode*> get_retimed_parent_same_var(TedNode* node);

		void getParents(void);
		void clearParents(void);
	public:
		TedRetiming(TedMan*);
		~TedRetiming(void);

		//MCR Maximum Common Register
		bool up(TedRegister uptoR = INT_MAX);
		bool up(TedVar& var, TedRegister uptoR = INT_MAX) { return upMinCR(var,uptoR); }
		bool upAllRelatedTo(TedVar& var, TedRegister uptoR = INT_MAX);
		bool upMinCR(TedVar&,TedRegister uptoR = INT_MAX);
		bool down(TedRegister uptoR = INT_MAX);
		bool down(TedVar& var,TedRegister uptoR = INT_MAX) { return downMinCR(var,uptoR); }
		bool downAllRelatedTo(TedVar& var,TedRegister uptoR = INT_MAX);
		bool downMinCR(TedVar&,TedRegister uptoR = INT_MAX);
		bool downMaxR(TedVar&);

		void reorder(void);

		list<TedNode*> order_retimed_same_var_set(const TedVar& var);
	};

}
#endif
