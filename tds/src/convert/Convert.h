/*
 * =====================================================================================
 *
 *       Filename:  Convert.h
 *    Description:  Convertion among TED, DFG and NTL data structures
 *        Created:  09/10/2009 12:52:41 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <list>
#include <string>

namespace ted {
	class TedVar;
	class TedMan;
}
namespace dfg {
	class DfgMan;
}
namespace network {
	class Netlist;
	class Node;
}

/**
 * @namespace network
 * @brief A network data structure to hold structural elements of an GAUT CDFG file
 **/
namespace convert {

	using namespace ted;
	using namespace dfg;
	using namespace network;

	class Convert {
	public:
		explicit Convert(void) : _pDfgMan(NULL), _pTedMan(NULL), _pNetlist(NULL) {};
		explicit Convert(DfgMan* ptr) : _pDfgMan(ptr), _pTedMan(NULL), _pNetlist(NULL) {};
		explicit Convert(TedMan* ptr) : _pDfgMan(NULL), _pTedMan(ptr), _pNetlist(NULL) {};
		explicit Convert(Netlist* ptr) : _pDfgMan(NULL), _pTedMan(NULL), _pNetlist(ptr) {};
		explicit Convert(DfgMan* ptrd, Netlist* ptrn) : _pDfgMan(ptrd), _pTedMan(NULL), _pNetlist(ptrn) {};
		~Convert(void) {};

		DfgMan * ted2dfg(void);
		DfgMan * ted2dfgFactor(bool showDfgFactor = false);

		TedMan * dfg2ted(bool flatten = false);
		TedMan * dfg2ted(std::list<TedVar*>& );
		TedMan * dfg2tedError(void);

		TedMan * ntl2ted(bool);
		DfgMan * ntl2dfg(void);

		Netlist * dfg2ntl(bool evaluateConstants = false);

		Netlist * getNewNetlist(void) { return _newNetlist; }

		//reuse Convert object for another convertion
		void operator()(DfgMan* ptr) { _pDfgMan = ptr; _pTedMan = NULL; _pNetlist = NULL; }
		void operator()(TedMan* ptr) { _pTedMan = ptr; _pDfgMan = NULL; _pNetlist = NULL; }
		void operator()(Netlist* ptr) { _pNetlist = ptr; _pTedMan = NULL; _pDfgMan = NULL; }
	private:
		DfgMan * _pDfgMan;
		TedMan * _pTedMan;
		Netlist * _pNetlist;
		Netlist * _newNetlist;

	};

}
#endif

