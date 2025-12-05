/*
 * =====================================================================================
 *
 *       Filename:  DfgOperator.cc
 *    Description:
 *        Created:  11/22/2009 09:54:13 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 110                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-11-20 19:54:30 -0500 (Fri, 20 Nov 2009)     $: Date of last commit
 * =====================================================================================
 */

#include "DfgNode.h"
#include "DfgOperator.h"

namespace dfg {

#ifndef INLINE
#include "DfgNode.inl"
#endif

	unsigned int DfgOperator::idcount = 0;
	map<unsigned int,DfgOperator*> DfgOperator::manager;

	DfgOperator::DfgOperator(Bitwidth* pre, Type type): _bitw(pre), _rounding(DN), _id(++idcount), _count(0), _type(type), _error(NULL) {
		manager.insert(pair<unsigned int,DfgOperator*>(_id,this));
	}
	DfgOperator::DfgOperator(Type type): _bitw(NULL), _rounding(DN), _id(++idcount), _count(0), _type(type), _error(NULL) {
		manager.insert(pair<unsigned int,DfgOperator*>(_id,this));
	}
	DfgOperator::~DfgOperator(void) {
		if(_bitw)
			delete _bitw;
		manager.erase(_id);
	};

}
