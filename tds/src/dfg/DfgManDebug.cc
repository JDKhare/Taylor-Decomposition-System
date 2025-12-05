/*
 * =====================================================================================
 *
 *       Filename:  DfgManDebug.cc
 *    Description:
 *        Created:  05/16/2009 11:59:37 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef NDEBUG

#include <iostream>
using namespace std;

#include "DfgMan.h"
#include "DfgNode.h"

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
using namespace ted;

namespace dfg {
	void DfgMan::debugTed2DfgMapping(const Ted2Dfg& mapping) const {
		Ted2Dfg::const_iterator iter = mapping.begin();
		Ted2Dfg::const_iterator iter_end = mapping.end();
		cerr << "=======================================" << endl;
		cerr << "Debugging Ted2Dfg mapping: " << &mapping << endl;
		//the internal operation does not depend on the traversal order, therefore
		//we can use any order, i.e the order of the map
		for (; iter != iter_end; iter++) {
			cerr << "TedNode " << iter->first << " " << iter->first->getName();
			cerr << " -> with DFG " << iter->second << " " << iter->second->getSymbolSave();
			cerr << endl;
		}
	}
}

#else
namespace {
	bool Avoid_empty_translation_unit = true;
}
#endif

