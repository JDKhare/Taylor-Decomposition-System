/*
 * =====================================================================================
 *
 *       Filename:  ConvertTed2DfgFactor.h
 *    Description:
 *        Created:  01/27/2010 08:16:56 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 107                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-11-19 12:36:29 -0500 (Thu, 19 Nov 2009)     $: Date of last commit
 * =====================================================================================
 */
#ifndef __CONVERTTED2DFGFACTOR_H__
#define __CONVERTTED2DFGFACTOR_H__

#include <map>
#include <string>

#include "LinkedMap.h"
using dtl::LinkedMap;

namespace ted {
	class TedMan;
	class ATedNode;
}
namespace dfg {
	class DfgMan;
	class DfgNode;
}

namespace convert {

	using dfg::DfgMan;
	using dfg::DfgNode;
	using ted::TedMan;
	using ted::TedNode;

	class ConvertTed2DfgFactor {
	protected:
		TedMan* _tMan;
		DfgMan* _dMan;

		typedef LinkedMap<const TedNode*, DfgNode*> ListMapDfgTed;
		std::map<const TedNode*, ListMapDfgTed > _mapDfgFactors;
		std::map<std::string,DfgNode*> _mapDfg;
		std::map<long,DfgNode*> _mapDfgConst;

		DfgNode * getDfgNode(const char * var, DfgOperator::Type op = DfgOperator::VAR);
		DfgNode * getDfgNode(long);
#if 0
		void factorTedNode(const TedNode * pNode, ListMapDfgTed& mFactors);
#else
		void factorTedNode(const TedNode * pNode);
#endif

	public:
		explicit ConvertTed2DfgFactor(TedMan * tm, DfgMan* dm, bool keep = false) : _tMan(tm), _dMan(dm) { }
		~ConvertTed2DfgFactor(void) { _tMan = NULL; _dMan = NULL; }

		void translate(void);
	};

}

#endif

