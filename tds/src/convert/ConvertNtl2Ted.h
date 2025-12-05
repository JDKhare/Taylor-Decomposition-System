/*
 * =====================================================================================
 *
 *       Filename:  ConvertNtl2Ted.h
 *    Description:
 *        Created:  02/10/2010 12:22:21 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 205                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-05 17:08:07 -0400 (Thu, 05 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */
#ifndef __CONVERTNTL2TED_H__
#define __CONVERTNTL2TED_H__

#include <string>
#include <map>
using namespace std;

namespace ted {
	class TedMan;
	class TedNode;
	class ATedNode;
}
namespace network {
	class Netlist;
	class Node;
}

namespace convert {

	using namespace network;
	using namespace ted;

	class ConvertNtl2Ted {
	public:
		explicit ConvertNtl2Ted(Netlist * nm, TedMan * tm) : _nMan(nm), _nManNew(NULL), _tMan(tm), _evaluateConstantNodes(false) {};
		~ConvertNtl2Ted(void) { _nMan = NULL; _nManNew = NULL; _tMan = NULL; primaryInputs.clear(); primaryConstants.clear(); }

		void translate(bool);
		void translate(void);
		ATedNode* translate(Node* pNode);
		Netlist* getNewNetlist(void) { return _nManNew; }
	private:
		Netlist* _nMan;
		Netlist* _nManNew;
		TedMan* _tMan;
		bool _evaluateConstantNodes;
		set<Node*> primaryInputs;
		set<Node*> primaryConstants;

		void obtainPIs(Node * pNode);

		Node* pseudo_output(ATedNode* pET, Node* pFanin, set<Node*> & sFanins);

		template<typename T>
			Node * collapseOtherNodes(Node * pNode);
		template<typename T>
			ATedNode * collapseArithmeticNodes(Node * pNode,  set<Node*> & sFanins);
		template<typename T>
			ATedNode * collapseRegisterNodes(Node * pNode,  set<Node*> & sFanins, int&);
		template<typename T>
			ATedNode * collapseSllNodes(Node * pNode,  set<Node*> & sFanins);

		template<typename T>
		ATedNode* retimeCone(ATedNode* pNode,int retime);
	};

}

#endif

