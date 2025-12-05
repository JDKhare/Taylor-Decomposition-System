/*
 * =====================================================================================
 *
 *       Filename:  MapDfgTed.cc
 *    Description:
 *        Created:  10/02/2009 12:20:42 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <cassert>

#include "TedNode.h"
using namespace ted;

#include "DfgNode.h"
using namespace dfg;

#include "MapDfgTed.h"
namespace convert {

	MapDfgTed::MapDfgTed(PManagerType ptr) : ConvertMap<TedNode*,int,DfgMan*,DfgNode>(ptr) {
		NodeType* dOne = new NodeType(_pMan,1);
		registrate(TedNode::getOne(),1,dOne);
	}

	void MapDfgTed::registrate(OuterKey tNode, InnerKey index, NodeType* dnode) {
		Database::iterator found = _data.find(tNode);
		if (found == _data.end()) {
			Data datum;
			datum.insert(pair<InnerKey,NodeType*>(index,dnode));
			_data.insert(pair<OuterKey,Data>(tNode,datum));
		} else {
			throw(string("08001. Registering an existing node"));
		}
	}

	MapDfgTed::NodeType* MapDfgTed::get(InnerKey weight) {
		//PRE: TedNode::getOne() has already been registered
		Data& datum = _data[TedNode::getOne()];
		assert(!datum.empty());
		NodeType * ret = datum[weight];
		if (!ret) {
			ret = new NodeType(_pMan,weight);
			datum[weight] = ret;
		}
		return ret;
	}

	MapDfgTed::NodeType* MapDfgTed::get(OuterKey tNode, InnerKey weight) {
		//PRE: tNode has already been registered
		if (TedNode::getOne() ==tNode) {
			return get(weight);
		}
		Data& datum = _data[tNode];
		assert(!datum.empty());
		NodeType * ret = datum[weight];
		if (!ret) {
			NodeType * rhs = datum[1];
			NodeType * lhs = get(weight);
			//ret = new NodeType(_pMan,NodeType::MUL,lhs,rhs);
			ret = lhs->mul(rhs);
			datum[weight] = ret;
		}
		return ret;
	}

}
