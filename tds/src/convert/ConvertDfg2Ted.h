/*
 * =====================================================================================
 *
 *       Filename:  ConvertDfg2Ted.h
 *    Description:
 *        Created:  01/11/2010 02:26:33 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */
#ifndef __CONVERTDFG2TED_H__
#define __CONVERTDFG2TED_H__

#include <map>
using namespace std;

namespace ted {
	class TedMan;
	class ATedNode;
}
using ted::TedMan;
using ted::TedNode;

namespace dfg {
	class DfgMan;
	class DfgNode;
}
using dfg::DfgMan;
using dfg::DfgNode;

namespace convert {

	class ConvertDfg2Ted {
	public:
		explicit ConvertDfg2Ted(DfgMan* dm, TedMan * tm) : _dMan(dm), _tMan(tm) {};
		~ConvertDfg2Ted(void) { _tMan = NULL; _dMan = NULL; }

		virtual void translate(void);
		ATedNode* translate(DfgNode* pNode);

		virtual void flatten(void);
		ATedNode* flatten(map<string,int>& hash_count, map<string,ATedNode*>& hash_name, DfgParents& parents, DfgNode* pNode);
	protected:
		DfgMan* _dMan;
		TedMan* _tMan;
		map <DfgNode *, ATedNode *> mVisited;
	};

}

#endif

