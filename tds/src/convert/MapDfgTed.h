/*
 * =====================================================================================
 *
 *       Filename:  MapDfgTed.h
 *    Description:
 *        Created:  10/01/2009 09:40:12 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __MAPDFGTED_H__
#define __MAPDFGTED_H__

namespace ted {
	class TedMan;
	class TedNode;
}
namespace dfg {
	class DfgMan;
	class DfgNode;
}

#include "ConvertMap.h"

namespace convert {

	using namespace dfg;
	using namespace ted;

	class MapDfgTed : public ConvertMap<TedNode*,int,DfgMan*,DfgNode> {
	public:
		explicit MapDfgTed(PManagerType);
		~MapDfgTed(void) {};

		DfgNode* get(InnerKey);
		void registrate(OuterKey, InnerKey, NodeType *);
		NodeType * get(OuterKey, InnerKey);
	};

}
#endif

