/*
 * =====================================================================================
 *
 *        Filename:  TEDMan.h
 *     Description:  TEDManager header
 *         Created:  04/17/2007 11:42:07 AM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDMAN_H__
#define __TEDMAN_H__

#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <climits>
//#include "DfgMan.h"
#include "TedVar.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedContainer.h"
#include "TedNodeRoot.h"
#include "TedOrderCost.h"
#include "TedOrderStrategy.h"
#include "TedDecompose.h"
#include "TedRetiming.h"

namespace dfg {
	class DfgMan;
}
using namespace dfg;

namespace convert {
	class ConvertTed2Dfg;
	class ConvertTed2DfgFactor;
	class ConvertDfg2Ted;
}

namespace data {
	class Bitwidth;
}
using data::Bitwidth;

/**
 * @namespace ted
 * @brief The Taylor Expansion Diagram(TED)data structure.
 **/
namespace ted {

	using namespace std;

	class TedNode;
	class ATedNode;
	class TedParents;
	class TedBitwidth;

	/** @brief A Map of TedNodeRoot keyed by the variable name**/
	typedef map<TedNode*,unsigned int> TedCandidates;

	/**
	 * @class TedMan
	 * @brief Implements a TED manager, where all final TEDs are stored in it.
	 **/
	class TedMan {
		friend class convert::ConvertDfg2Ted;
		friend class convert::ConvertTed2Dfg;
		friend class convert::ConvertTed2DfgFactor;
		friend class DfgMan;
		friend class TedOrder;
		friend class TedOrderStrategy;
		friend class TedDecompose;
		friend class TedBitwidth;
		friend class TedRetiming;
	public:
		typedef	map<string,TedNodeRoot> PrimaryOutputs;
	private:
		/* the constant limit, used for DFactor routine*/
		size_t _const_limit;

		TedVarMan& _vars;

		/* the map of TedNodePOs, keyed by the name*/
		PrimaryOutputs _pos;

		/* the data structure of Ted Manager the map is keyed by the TedVariable
		 * value is a pair of <unsigned int, set <TedNode*> > the unsigned int
		 * is the level of the variable the set of Ted Nodes are nodes at that level
		 */
		TedContainer _container;

		TedNode* _Duplicate_Rec(TedMan* pManNew, TedNode* pNode, \
								map <TedNode*, TedNode*> & mVisited);
		TedNode* _Linearize_Rec(TedMan* pManNew, TedNode* pNode, \
								map <TedNode*, TedNode*> & mVisited);
		TedMan* _DuplicateUnlink(set <string>* ponames = NULL);

	public:
		static void purge_all_but(TedMan*);
		static void purge_all_but(TedMan*,TedMan*);
		static void purge_all_but(TedMan*,TedMan*,TedMan*);
		void purge_transitory_container(void);

		bool check_for_dangling_nodes(void);

		pair<TedNode*, wedge>  _Normalize_Rec(TedNode* pENode);
		ATedNode* _PropogateZero_Rec(ATedNode* pENode);

		typedef const_iteratorDFS<TedMan> const_dfs_iterator;
		const_dfs_iterator const_dfs_begin(void)const;
		const_dfs_iterator const_dfs_end(void)const;
		typedef iteratorDFS<TedMan> dfs_iterator;
		dfs_iterator dfs_begin(void);
		dfs_iterator dfs_end(void);

		TedMan(void);
		~TedMan(void);

		Bitwidth* getMaxBitwidth(void);

		TedContainer& getContainer(void) { return _container; }
		PrimaryOutputs& getPOs(void) { return _pos; }

		bool isSimpleVariable(const TedNode* pNode)const;

		//Set and Get Const Limit;

		bool isEmpty(void)const;
		size_t getConstLimit(void)const;
		void setConstLimit(size_t l);
		TedNode* getNode(const TedVar &v);
		TedNode* getNode(const string &p);

		void clearVar(void);
		void registerVar(const string &, bool binary = false);
		void registerVar(const string &, wedge value);
		void registerVars(vector<string>&);
		void registerVars(list<TedVar*>&);
		bool hasVar(const string &)const;
		void getVars(vector<string>&);
		void getVars(list<TedVar*>&);
		TedVar* checkVariables(const string& pVar);

		size_t getLevel(const TedNode*v)const;
		size_t getMaxLevelWoConst(void)const;

		TedNodeRoot normalize(ATedNode*);
		void linkTedToPO(TedNode* pNode, wedge weight, const string& poname);
		void linkTedToPO(const TedNodeRoot& po, const string& poname);
		void UnLinkTedToPo(const string & poname);

		void collectDFS(vector <TedNode*> & vNodes);
		template<typename T>
			void preOrderDFS(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int));
		template<typename T>
			void preOrderDFSforPOs(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int));
		void collectParents(void);

		string dotString(const TedNode* pNode, bool bigFont, bool compact_graph)const;
		string obfuscate(const TedNode* pNode)const;
		void writeDot(ofstream&,bool bigFont,bool compact_graph);

		string dotStringDetailed(const TedNode* pNode, const TedNode* pTouch)const;
		void writeDotDetailed(void);
		void writeDotDetailed(const TedNode* pTouch);
		void writeDotDetailed(ofstream&);
		void writeDotDetailed(ofstream&, const TedNode* pTouch);

		/** @brief Duplicate a TedManager*/
		TedMan* duplicate(void);

		TedMan* relocateVariable(const string & v, unsigned int level);

#ifdef CURRENTLY_ON_DEVELOPMENT
		void reor(const TedVar& varX, const TedVar& varY);
		void forwardWeightUp(TedNode* pnode, wedge gcd, TedParents&);
#endif

		TedMan* fixorderRetimedVars(bool verboseOnly);
		TedMan* fixorderVars(bool verboseOnly);
		TedMan* patchorderVars();

		TedMan* jumpAboveVariable(const string& v1, const string& v2, TedOrderStrategy* order);
		TedMan* jumpBelowVariable(const string& v1, const string& v2, TedOrderStrategy* order);
		TedMan* exchangeVariable(const string& v1, const string& v2, TedOrderStrategy* order);
		TedMan* bblUpVariable(const string & v, TedOrderStrategy* order, unsigned int count);
		TedMan* bblDownVariable(const string &v, TedOrderStrategy* order, unsigned int count);
		TedMan* bblUpVariable(const string & v, TedOrderStrategy* order);
		TedMan* bblDownVariable(const string &v, TedOrderStrategy* order);
		TedMan* topVariable(const string & v, TedOrderStrategy* order);
		TedMan* bottomVariable(const string & v, TedOrderStrategy* order);
		TedMan* siftAll(TedOrderStrategy* order, TedCompareCost* less);
		TedMan* siftGroupedAll(TedOrderStrategy* order, TedCompareCost* less);
		TedMan* siftVar(const string &v, TedOrderStrategy* order, TedCompareCost* less);
		TedMan* flipVar(const string & pVar, TedOrderStrategy* order);
		TedMan* permuteOrdering(TedOrderStrategy* order, TedCompareCost* less, bool break_into_strides);
		TedMan* annealing(TedOrderStrategy* order, TedCompareCost* less, bool break_into_strides, bool stride_backtrack);

		void forwardWeightUp(TedNode* pNode, wedge gcd, TedParents& parents);
		void forwardWeightUp_rec(TedNode* pNode, wedge gcd, TedParents& parents, TedSet& tmp_key, map<TedNode*,TedNode*> & no_touch);
		void updateTopParents(TedNode* oldNode, TedNode* newNode, TedParents& parents);
		void updateBottomParents(TedNode* oldNode, TedNode* newNode, TedParents& parents);
		void updateAllParents(TedNode* oldNode, TedNode* newNode, TedParents& parents);
		void replaceNodeBy(TedNode* pNode, TedNode* newNode, TedParents& parents);

		void eval(const string& varname, int value);
		void eval(const TedVar& varX, int value);

		TedNodeRoot* getPO(TedNode*);
		bool isPO(TedNode*);
		TedMan* unlinkPO(set<string> s);
		TedMan* extractPO(set<string> s);
		TedMan* linearize(void);

		void computeBitwidth(void);
		bool decompose(bool);
		TedMan* decomposeAll(bool);
		TedMan* decomposeAll(TedCompareCost*,bool);
		void decomposeST(bool);
		void decomposePT(bool);
		void moveSupportTo(TedMan* destiny);
		void cost(void);
		void costNaive(unsigned int&, unsigned int&);
		void deriveSchedule(void);

		// For dynamic factorization
		unsigned int getNumOfCandidates(void);
		TedNode* getFirstCandidate(vector <pair<TedNode*, unsigned int> >* vParients);
		TedMan* dynFactorize(void);
		void printCandidates(void);
		void getCandidateList(TedCandidates& cand_list);
		void searchFactorizableNode(vector<pair<TedNode*, TedNode*> > & cand_list);
		void searchDecompNode(vector<pair<TedNode*, TedNode*> > & cand_list);
		void searchmpyclass(vector<pair<TedNode*, TedNode*> > & cand_list);
		void searchadditiveNode(vector<TedNode*> & add_candidate_list);
		void getAssociatedNodes(TedNode* parentnode, vector<string> &AssociatedNodes);
		void getParentNodes(TedNode* candidate, vector<TedNode*> &vparentnodes);

		TedMan* dcse(void);
		TedMan* lcse(void);
		TedMan* bottomdcse(void);
		TedMan* maxdcse(void);
		TedMan* scse(void);
		TedMan* bottomscse(void);
		TedMan* ecse(void);

		string write(const TedNode* pNode);
		void write(string filename);
		void read(string filename);
		static unsigned int _factor_counter;

		void retime(const string& v, bool allRelatedVars, bool forward, TedRegister uptoR=INT_MAX);
		void retime(bool forward,TedRegister uptoR=INT_MAX);
		void retime_down_forced(const string& v);

		bool regressionTest(stringstream& message);
		bool verify(const string& output1, const string& output2);

#ifdef FUNCTIONS_DEPRECATED
		static TedMan* buildTedFromDFG(TedMan* pTedMan, DfgMan* pDfgMan);
		void getUpLowMulBound(int* upb, int* lowb);
#endif

		void foreach(void(TedNode::*functor)(void));
		void foreach(void(TedVar::*functor)(void)const);
		void listOutputs(void);

		void recoverRegisterVars(void);
		//Debugging facilities
#ifndef NDEBUG
		void debugDFStraversal(void);
#endif
	};


	/** @brief for all TedNodePOs of type PO, executes functor in a pre-order DFS traversal*/
	template<typename T>
		void TedMan::preOrderDFSforPOs(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int)) {
			Mark<TedNode>::newMark();
			for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
				if(p->second.getType() ==TedNodeRoot::PO) {
					p->second.Node()->_preOrderDFS_rec(obj,functor);
				}
			}
		}

	/** @brief for all TedNodePOs, executes functor in a pre-order DFS traversal*/
	template<typename T>
		void TedMan::preOrderDFS(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int)) {
			Mark<TedNode>::newMark();
			for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
				p->second.Node()->_preOrderDFS_rec(obj,functor);
			}
		}

#ifdef INLINE
#include "TedMan.inl"
#endif

}
#endif
