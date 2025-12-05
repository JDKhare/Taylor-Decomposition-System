/*
 * =====================================================================================
 *
 *       Filename:  Convert.cc
 *    Description:
 *        Created:  09/10/2009 01:18:50 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 17:43:11 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <iostream>
#include <list>
using namespace std;

#include "Convert.h"
#include "ConvertTed2Dfg.h"
#include "ConvertTed2DfgFactor.h"
#include "ConvertDfg2Ted.h"
#include "ConvertDfg2TedError.h"
#include "ConvertNtl2Ted.h"
#include "ConvertDfg2Ntl.h"

#include "TedMan.h"
#include "TedNode.h"
#include "ATedNode.h"
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
using namespace dfg;

#include "Ntk.h"
#include "Node.h"
using namespace network;

namespace convert {
	DfgMan * Convert::ted2dfg(void) {
		assert(_pTedMan);
		assert(!_pDfgMan);
		_pDfgMan = new DfgMan();
		ConvertTed2Dfg t2d(_pTedMan,_pDfgMan);
		t2d.translate();
		DfgNode::setEnvOpDelays();
		_pDfgMan->updateDelayAndNumRefs();
		_pDfgMan->updateRequiredTime();
		_pDfgMan->showRegisters();
		assert(_pDfgMan);
		return _pDfgMan;
	}

	DfgMan * Convert::ted2dfgFactor(bool showDfgFactors) {
		assert(_pTedMan);
		assert(!_pDfgMan);
		_pDfgMan = new DfgMan();
		ConvertTed2DfgFactor t2df(_pTedMan,_pDfgMan);
		t2df.translate();
		if (!showDfgFactors) {
			_pDfgMan->relink_pseudo_factors();
			_pDfgMan->strash();
		}
		DfgNode::setEnvOpDelays();
		_pDfgMan->updateDelayAndNumRefs();
		_pDfgMan->updateRequiredTime();
		assert(_pDfgMan);
		return _pDfgMan;
	}


	TedMan * Convert::dfg2ted(list<TedVar*>& vars) {
		assert(_pDfgMan);
		assert(!_pTedMan);
		_pTedMan = new TedMan();
		_pTedMan->registerVars(vars);
		ConvertDfg2Ted d2t(_pDfgMan,_pTedMan);
		d2t.translate();
		assert(_pTedMan);
		//ATedNode::purge_transitory_container();
		return _pTedMan;
	}

	TedMan * Convert::dfg2ted(bool flatten) {
		assert(_pDfgMan);
		assert(!_pTedMan);
		_pTedMan = new TedMan();
		ConvertDfg2Ted d2t(_pDfgMan,_pTedMan);
		if (flatten) {
			d2t.flatten();
		} else {
			d2t.translate();
		}
		assert(_pTedMan);
		//ATedNode::purge_transitory_container();
		return _pTedMan;
	}

	TedMan * Convert::dfg2tedError(void) {
		assert(_pDfgMan);
		assert(!_pTedMan);
		_pTedMan = new TedMan();
		ConvertDfg2TedError d2t(_pDfgMan,_pTedMan);
		d2t.translate();
		assert(_pTedMan);
		//ATedNode::purge_transitory_container();
		return _pTedMan;
	}


	TedMan * Convert::ntl2ted(bool flag = false) {
		assert(_pNetlist);
		assert(!_pTedMan);
		_pTedMan = new TedMan();
		ConvertNtl2Ted n2t(_pNetlist,_pTedMan);
		n2t.translate(flag);
		_newNetlist = n2t.getNewNetlist();
#if 0
		Mark<TedNode>::newPMark();
		_pTedMan->getContainer().update_PMark();
		TedKids::garbage_collect_unmarked(&Mark<TedKids>::isPMarked);
		ATedNode::garbage_collect_unmarked(&Mark<TedNode>::isPMarked);
#endif
		assert(_pTedMan);
		return _pTedMan;
	}

	DfgMan * Convert::ntl2dfg(void) {
		/*	assert(_pNetlist);
			assert(!_pDfgMan);
			_pDfgMan = new DfgMan();
			ConvertNtl2Dfg n2d(_pNetlist,_pDfgMan);
			n2d.translate();
			assert(_pDfgMan);
			return _pDfgMan; */
		return NULL;
	}


	Netlist* Convert::dfg2ntl(bool evaluateConstants) {
		assert(_pDfgMan);
		ConvertDfg2Ntl d2n(_pDfgMan,_pNetlist,evaluateConstants);
		_newNetlist = d2n.translate();
		assert(_newNetlist);
		return _newNetlist;
	}
}
