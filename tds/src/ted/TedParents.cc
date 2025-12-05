/*
 * =====================================================================================
 *
 *       Filename:  TedParents.cc
 *    Description:
 *        Created:  08/21/2011 11:38:24 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include <algorithm>

using namespace std;

#include "TedParents.h"
#ifdef USING_TED_NAMESPACE
using namespace ted;
#endif

void TedParents::print_parents(void) const {
	for(TedParents::const_iterator it = this->begin(), it_end = this->end(); it != it_end; it++) {
		TedNode* node = it->first;
		MyParent* parents = it->second;
		cout << " ===== " << node << " = " << parents->size() << " ==== " <<endl;
		for(MyParent::const_iterator pit = parents->begin(), pit_end = parents->end(); pit != pit_end; pit++) {
			TedNode* parent = pit->first;
			MyIndex* index = pit->second;
			cout << parent << " (" << index->size() << ") ";
			for(MyIndex::const_iterator jt = index->begin(), jt_end = index->end(); jt!=jt_end; jt++) {
				cout <<*jt << ",";
			}
			cout << endl;
		}
	}
}
