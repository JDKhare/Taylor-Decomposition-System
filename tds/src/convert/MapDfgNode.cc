/*
 * =====================================================================================
 *
 *       Filename:  MapDfgNode.cc
 *    Description:
 *        Created:  10/02/2009 12:20:42 AM
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

#include "TedNode.h"
using namespace ted;

#include "DfgNode.h"
using namespace dfg;

#include "MapDfgNode.h"
namespace convert {

	void MapDfgNode::registrate(OuterKey dNode, InnerKey dw, NodeType* dnode) {
		Database::iterator found = _data.find(dNode);
		if (found == _data.end()) {
			Data datum;
			datum.insert(pair<InnerKey,NodeType*>(dw,dnode));
			_data.insert(pair<OuterKey,Data>(dNode,datum));
		} else {
			throw(string("08002. Registering an existing node"));
		}
	}

	MapDfgNode::NodeType* MapDfgNode::get(OuterKey dNode, InnerKey dw) {
		//PRE: dNode has already been registered
		assert(_data.find(dNode) != _data.end());
		Data& datum = _data[dNode];
		assert(!datum.empty());
#if 1
		Data::iterator found = datum.find(dw);
		NodeType* ret = NULL;
		if (found==datum.end()) {
			ret = dNode->mul(dw);
			datum[dw] = ret;
		} else {
			ret = found->second;
		}
#else
		NodeType * ret = datum[dw];
		if (!ret) {
			ret = dNode->mul(dw);
			datum[dw] = ret;
		}
#endif
		return ret;
	}

	void MapDfgNodeR::registrate(OuterKey dNode, InnerKey dw, NodeType* dnode) {
		Database::iterator found = _data.find(dNode);
		if (found == _data.end() && 1==dw) {
			Data datum;
			datum.insert(pair<InnerKey,NodeType*>(dw,dnode));
			_data.insert(pair<OuterKey,Data>(dNode,datum));
		} else {
			throw(string("08003. Bad initial retimed node registration"));
		}
	}

	MapDfgNodeR::NodeType* MapDfgNodeR::get(OuterKey dNode, InnerKey dw) {
		//PRE: dNode has already been registered
		if (0==dw)
			return dNode;
		assert(_data.find(dNode) != _data.end());
		Data& datum = _data[dNode];
		assert(!datum.empty());
		Data::iterator found = datum.find(dw);
		NodeType* ret = NULL;
		if (found==datum.end()) {
			assert(!datum.empty());
			ret = datum[datum.size()];
			for (int index = datum.size()+1; index <= dw; index++) {
				ret = ret->reg(1);
				datum[index] = ret;
			}
		} else {
			ret = found->second;
		}
		return ret;
	}

}
