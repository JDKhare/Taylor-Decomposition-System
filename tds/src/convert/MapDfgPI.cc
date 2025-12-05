/*
 * =====================================================================================
 *
 *       Filename:  MapDfgPI.cc
 *    Description:
 *        Created:  10/02/2009 12:20:38 AM
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
#include <cmath>

#include "TedNode.h"
using namespace ted;

#include "DfgNode.h"
using namespace dfg;

#include "MapDfgPI.h"
namespace convert {

	void MapDfgPI::registrate(OuterKey varname,InnerKey index, NodeType* dnode) {
		// PRE varname is not yet on the database
		assert(!has(varname));
		Data datum;
		datum.insert(pair<InnerKey,NodeType*>(index,dnode));
		_data.insert(pair<OuterKey,Data>(varname,datum));
	}

	MapDfgPI::NodeType* MapDfgPI::get(OuterKey varname, InnerKey index) {
		//PRE: varname has been registered on the map
		assert(has(varname));
		Data& datum = _data[varname];
		NodeType* ret = datum[index];
		if (!ret) {
			ret = generate(datum,index);
		}
		return ret;
	}

	MapDfgPI::NodeType* MapDfgPI::generate(Data& datum, InnerKey index) {
		assert(index!=1);
		double val = ((double)index)/2;
		double n = ceil(val);
		double m = floor(val);
		int index_n = (int) n;
		int index_m = (int) m;
		NodeType * dnode_n = datum[index_n];
		if( !dnode_n ) {
			dnode_n = generate(datum,index_n);
		}
		NodeType * dnode_m = datum[index_m];
		if( !dnode_m ) {
			dnode_m = generate(datum,index_m);
		}
		NodeType* dnode_index = new NodeType(_pMan,DfgOperator::MUL,dnode_n,dnode_m);
		datum[index] = dnode_index;
		assert(dnode_index);
		return dnode_index;
	}

}
