/*
 * =====================================================================================
 *
 *       Filename:  MapDfgPI.h
 *    Description:
 *        Created:  10/01/2009 08:35:48 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __MAPDFGPI_H__
#define __MAPDFGPI_H__

#include <string>
using namespace std;

namespace dfg {
	class DfgMan;
	class DfgNode;
}

#include "ConvertMap.h"

namespace convert {

	using namespace dfg;

	class MapDfgPI : public ConvertMap<string,unsigned int,DfgMan*,DfgNode> {
	public:
		explicit MapDfgPI(PManagerType ptr) : ConvertMap<string,unsigned int,DfgMan*,DfgNode>(ptr) {};
		~MapDfgPI(void) {};

		void registrate(OuterKey, InnerKey, NodeType*);
		NodeType* get(OuterKey, InnerKey);

	private:
		NodeType* generate(Data&, InnerKey);
	};

}
#endif

