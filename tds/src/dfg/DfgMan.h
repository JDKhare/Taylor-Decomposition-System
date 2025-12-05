/*
 * =====================================================================================
 *
 *       Filename:  DfgMan.h
 *    Description:  Ander Multiplier gragh manager;
 *        Created:  05/01/2007 03:12:17 AM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __DFGMAN_H__
#define __DFGMAN_H__

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>

#include "LinkedMap.h"
using dtl::LinkedMap;

#include "types.h"
#include "DfgStat.h"

namespace ted {
	class TedMan;
	class TedNode;
	class TedFifo;
}
using namespace ted;

namespace convert {
	class ConvertTed2Dfg;
	class ConvertTed2DfgFactor;
	class ConvertDfg2Ted;
	class ConvertDfg2TedError;
};

/**
 * @namespace dfg
 * @brief The Data Flow Graph data structure representation of the network or ted.
 **/
namespace dfg {

	using namespace std;
	class DfgNode;
	class DfgShifter;
	class DfgOperator;
	class DfgBitReduction;

	typedef LinkedMap<const TedNode*, DfgNode*> Ted2Dfg;
	typedef map<DfgNode*, vector<DfgNode*> > DfgParents;

	/**
	 * @class DfgMan
	 * @brief The DFG manager.
	 **/
	class DfgMan {
		friend class TedMan;
		friend class DfgShifter;
		friend class convert::ConvertTed2Dfg;
		friend class convert::ConvertTed2DfgFactor;
		friend class convert::ConvertDfg2Ted;
		friend class convert::ConvertDfg2TedError;
		friend class DfgBitReduction;
	public:
		typedef map<string,DfgNode*> PrimaryOutputs;
	private:
		PrimaryOutputs _mPos;
		set <DfgNode*> _sNodes;
		bool _showRegisters;

		DfgNode* _UseShift_Rec(DfgMan*, DfgNode*, const string& L);
		DfgNode* _ConstToCsd(DfgMan* pManNew, wedge v, const string& L);
		void _relink_dfactored_node_rec(DfgNode* pNode, set <string> & sPseudoPos);
		void _UpdateRequiredTime_Rec(DfgNode* pNode, unsigned int _req);

	public:
		void evaluateConstants(void);
		void protectConstants(void) {};

		void info_hierarchy(void);
		int  hierarchy(map<DfgNode*,pair<int,int> >& level, DfgNode* poNode, DfgParents&);
		void hierarchy(multimap<int,pair<string,int> >& level);
		void relink_pseudo_factors(void);
		void collectParents(DfgParents& container);
		void collectParents(DfgParents& container, vector<DfgNode*>& dfsNodes);
		explicit DfgMan(void) : _showRegisters(false) {}
		//DfgMan(TedMan*, bool showDfgFactors = false);
		~DfgMan(void);

		void showRegisters(void) { _showRegisters = true; }

		PrimaryOutputs* getPos(void);
		void registerNode(DfgNode*);
		void unregisterNode(DfgNode*);
		void cleanUp(void);
		void strash(void);

		unsigned int collectDFS(vector <DfgNode* > & vNodes);
		void collectDFSonly(vector <DfgNode* > & vNodes);

		void computeBitwidth(bool(DfgNode::*Functor)(unsigned int), unsigned int);
		void computeBitwidth(void);
		void computeSNR(void);
#if 0
		void optimize(double maxerror);
#endif
		void writeDot(ofstream&, bool bUseArrivalTime = true, bool bVerbose = true, bool show_ids = false);
		string dotString(DfgNode* pNode, unsigned int level, bool bVerbose, bool show_ids);

		void writeDotDetailed(void);
		void writeDotDetailed(ofstream&);
		string dotStringDetailed(DfgNode* pNode);

		void writeCDFGname(ofstream&,DfgNode*);
		const char* getNameIfRegisters(DfgNode* pNode);
		string writeCDFGop(DfgNode* pNode);

		void getNumOperators(DfgStat & t);
		void getExpression(map<string, string> & mExprs, map<string, string>* mCSEs=NULL, bool patchFactors=false);

		void linkDFGToPO(DfgNode*, string);

		//replace const multipliers by shifters using 'canonical signed digits' format
		DfgMan* useShifter(const string& L);
		void updateDelayAndNumRefs(void);
		void updateRequiredTime(void);

		/* Functions used to balance the DfgManger in terms of delay*/
		//void SetPiArrivalTime(void);

		void balance(void);
		void balanceAreaRecovery(void);
		void schedule(unsigned int nMul, unsigned nAdd, unsigned nSub, bool pipeline = false);
		void scheduleStat(DfgStat& stat);

		int maxArrivalTime(void);

		void remapShifter(unsigned int maxShiftsPerChain);

		void writeCDFG(const char*);

#ifdef FUNCTIONS_DEPRECATED
		void unlinearize(char* v);
		unsigned int collectDFS(vector <DfgNode* > & vNodes,
								map <DfgNode*, unsigned int >* mNodeLevels);
#endif

		void writeGappa(string filename);

		string write(DfgNode*);
		void write(string filename);
		void read(string filename);

		bool regressionTest(stringstream& message);
		void getInfoSNR(void);

#ifndef NDEBUG
		void debugTed2DfgMapping(const Ted2Dfg&)const;
#endif
	};

#ifdef INLINE
#include "DfgMan.inl"
#endif

}
#endif

