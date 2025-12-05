/*
 * =====================================================================================
 *
 *       Filename:  TedManDebug.cc
 *    Description:
 *        Created:  05/16/2009 10:26:04 AM
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

#include "TedMan.h"
namespace ted {

	void TedMan::debugDFStraversal(void) {
		cerr << "========================================" << endl;
		cerr << "Debugging TedMan DFS traversal: " << this << endl;
		for (TedMan::dfs_iterator it = this->dfs_begin(), end = this->dfs_end(); it != end; it++) {
			const TedNode* pNode = *it;
			cerr << pNode << ":" << pNode->getName() << "  ";
		}
		cerr << endl;
	}

}
#else
namespace {
	bool Avoid_empty_translation_unit = true;
}
#endif

