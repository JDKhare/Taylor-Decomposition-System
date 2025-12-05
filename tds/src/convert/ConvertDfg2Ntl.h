/*
 * =====================================================================================
 *
 *       Filename:  ConvertDfg2Ntl.h
 *    Description:
 *        Created:  11/10/2010 09:46:02 AM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __CONVERTDFG2NTL_H__
#define __CONVERTDFG2NTL_H__

#include <string>
#include <map>
using namespace std;

namespace network {
	class Node;
	class Netlist;
}

namespace dfg {
	class DfgMan;
	class DfgNode;
}

namespace convert {
	using network::Node;
	using network::Netlist;
	using dfg::DfgMan;
	using dfg::DfgNode;

	class ConvertDfg2Ntl {
	public:
		typedef map<string, DfgNode*> MapNames2Dfg;
		typedef map<string, Node*> MapNames2Ntl;
		typedef map<DfgNode*, Node*> MapDfg2Ntl;
		typedef map<DfgNode*, string> MapDfg2Name;

		explicit ConvertDfg2Ntl(DfgMan* dm, Netlist* nm, bool condition) :
			_dMan(dm), _nMan(nm), _evalConst(condition) {};
		~ConvertDfg2Ntl(void) { _nMan = NULL; _dMan = NULL; }

		Netlist* translate(void);
		void decomposedNodes(Netlist* newNtl, MapNames2Dfg& mpPoDfgNode, MapNames2Ntl& mDecomposedNodes);
	protected:
		DfgMan* _dMan;
		Netlist* _nMan;
		bool _evalConst;
	};

}

#endif


