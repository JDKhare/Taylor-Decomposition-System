/*
 * =====================================================================================
 *
 *       Filename:  ConvertDfg2TedError.h
 *    Description:
 *        Created:  01/12/2010 09:13:21 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 122                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-11 21:46:11 +0100 (Mon, 11 Jan 2010)     $: Date of last commit
 * =====================================================================================
 */
#ifndef __CONVERTDFG2TEDERROR_H__
#define __CONVERTDFG2TEDERROR_H__

#include "ConvertDfg2Ted.h"

#include <map>

namespace convert {

	class ConvertDfg2TedError : public ConvertDfg2Ted {
	protected:
		typedef std::pair<ATedNode*,ATedNode*> Value;
		typedef std::map<DfgNode*,Value> ValueMap;

		ValueMap _map;
	public:
		explicit ConvertDfg2TedError(DfgMan* dm, TedMan * tm) : ConvertDfg2Ted(dm,tm) {};
		~ConvertDfg2TedError(void) {};

		void translate(void);
		Value translate(DfgNode* pNode);
	};

}
#endif

