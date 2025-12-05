/*
 * =====================================================================================
 *
 *       Filename:  ConvertTed2Dfg.h
 *    Description:
 *        Created:  10/01/2009 08:35:48 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 17:43:11 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __CONVERTTED2DFG_H__
#define __CONVERTTED2DFG_H__

#include <string>
#include <map>
using namespace std;

namespace ted {
	class TedMan;
	class TedNode;
}
namespace dfg {
	class DfgMan;
	class DfgNode;
}

#include "MapDfgNode.h"
#include "MapDfgPI.h"
#include "MapDfgTed.h"
namespace convert {

	using namespace dfg;
	using namespace ted;

	class ConvertTed2Dfg {
	public:
		explicit ConvertTed2Dfg(TedMan * tm, DfgMan* dm) : \
			_tMan(tm), _dMan(dm), _dOne(NULL), _piMap(dm), _tMap(dm), _dMap(dm), _dMapR(dm) { _dOne = _tMap.get(1); }
		~ConvertTed2Dfg(void) { _tMan = NULL; _dMan = NULL; _dOne = NULL; }

		void translate(void);
		DfgNode* translate(TedNode* pNode);
	private:
		TedMan* _tMan;
		DfgMan* _dMan;
		DfgNode* _dOne;
		MapDfgPI _piMap;
		MapDfgTed _tMap;
		MapDfgNode _dMap;
		MapDfgNodeR _dMapR;
	};

}
#endif

