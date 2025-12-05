/*
 * =====================================================================================
 *
 *       Filename:  TedBitwidth.h
 *    Description:
 *        Created:  09/18/2009 08:35:19 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDBITWIDTH_H__
#define __TEDBITWIDTH_H__

#include "TedParents.h"

namespace data {
	class Bitwidth;
}
using namespace data;

namespace ted {

	class TedMan;
	class TedNode;
	struct _BitwidthOrder {
		bool operator() (Bitwidth* p1, Bitwidth* p2) const;
	};

	class TedBitwidth {
	private:
		enum Type { NONE, INTEGER, FIXEDPOINT };
		TedMan* _currentManager;
		Type _type;
		Bitwidth* _newPre;
		Bitwidth* _addOne;
		TedParents _parents;
		Bitwidth* computeRec(TedNode*);
		Bitwidth* compute(TedNode*);
		bool belongsToChain(TedNode*);
		void addToChain(TedNode*, multimap<Bitwidth*,TedNode*,_BitwidthOrder>&);
	public:
		TedBitwidth(TedMan*);
		~TedBitwidth(void);
		void compute(void);
	};

}
#endif

