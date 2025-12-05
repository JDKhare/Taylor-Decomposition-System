/*
 * =====================================================================================
 *
 *       Filename:  DfgNode.h
 *    Description:  Ander Multiplier Graph Node;
 *        Created:  04/30/2007 08:46:01 PM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 208                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __DFGNODE_H__
#define __DFGNODE_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <cassert>
using namespace std;

#include "types.h"
#include "DfgOperator.h"

#include "Mark.h"
using dtl::Mark;


namespace ted {
	class TedVar;
}
using namespace ted;

namespace convert {
	class ConvertTed2Dfg;
	class ConvertDfg2Ted;
	class ConvertDfg2TedError;
}

namespace dfg {

	class DfgMan;
	class DfgBitReduction;

	/**
	 * @class DfgNode
	 * @brief The DFG node
	 * @details
	 * - _level PIs=0, POs=path with more operators
	 **/
	class DfgNode {
		friend class DfgMan;
		friend class DfgShifter;
		friend class convert::ConvertDfg2TedError;
		friend class DfgBitReduction;
	public:
		static const char TOKEN_REG = 'R';
	private:
#if 0
		DfgOperator::Type _type;
#endif
		union {
			void* ptr;
			DfgNode* backtrack;
			char* name;
			wedge value;
		} _data;
		DfgNode* _left;
		DfgNode* _right;
		DfgMan* _pMan;
		DfgOperator* _op;
		int _level;
		wedge _value;
		Mark<DfgNode> _visited;

		//	    unsigned int _opdelay;
		unsigned int _nrefs;
		unsigned int _arrival_time;
		unsigned int _required_time;

		static  unsigned int _addDelay;
		static  unsigned int _subDelay;
		static  unsigned int _mpyDelay;
		static  unsigned int _divDelay;
		static  unsigned int _lshDelay;
		static  unsigned int _rshDelay;
		static  unsigned int _regDelay;

		void _CollectBalanceSupportAndCone(vector <DfgNode*> &vSupp, vector <DfgNode*> &vCone);
		void _UpdateSupportPhase_rec(map <DfgNode*, bool> & mPhase, unsigned int nNegs = 0);
		void _BalanceCone(DfgMan* pMan, vector <DfgNode*> &vSupp, vector <DfgNode*> &vCone);
		bool _UpdateConePhase_rec(map <DfgNode*, bool> & mPhase);

	public:
		DfgNode(DfgMan*, DfgOperator::Type op, DfgNode* left, DfgNode* right);
		DfgNode(DfgMan*, const string&, DfgOperator::Type op = DfgOperator::VAR);
		DfgNode(DfgMan*, DfgNode* prev, unsigned int regamount);
		DfgNode(DfgMan*, wedge);
		~DfgNode(void);

		bool isMarked(void) { return _visited.isMarked(); }
		void setMark(void) { return _visited.setMark(); }
		wedge getValue(void)const { return _value; }
		int getLevel(void)const { return _level; }
		DfgNode* getLeft(void)const;
		DfgNode* getRight(void)const;
		void setKids(DfgNode* left, DfgNode* right);
		void swapKid(void);

		bool isVarConst(void)const;
		TedVar* getTedVarIfExist(void)const;

		bool isConst(void)const;
		wedge getConst(void)const;

		bool isPI(void)const;
		const char* getName(void)const;

		bool isOp(void)const;
		DfgOperator::Type getOp(void)const;
		void setType(DfgOperator::Type);
		const char* getOpStr(void)const;

		char* getSymbolSave(void);

		bool isAdd(void)const;
		bool isMul(void)const;
		bool isDiv(void)const;
		bool isLSH(void)const;
		bool isSub(void)const;
		bool isAddorSub(void)const;
		bool isReg(void)const;
		bool isAssign(void)const;

		DfgNode* add(DfgNode*);
		DfgNode* sub(DfgNode*);
		DfgNode* mul(DfgNode*);
		DfgNode* div(DfgNode*) {};
		DfgNode* neg(void);
		DfgNode* reg(unsigned int value);

		unsigned int collectDFS(vector <DfgNode*  > & vNodes);
		void collectDFSonly(vector <DfgNode*  > & vNodes);
#ifdef FUNCTIONS_DEPRECATED
		unsigned int collectDFS(vector <DfgNode*  > & vNodes,
								map <DfgNode*, unsigned int>* mNodeLevels);
#endif
		bool uptoLevel(unsigned int level) { return(_level <= level); }
		void computeBitwidth(bool(DfgNode::*Functor)(unsigned int), unsigned int);
		void computeBitwidth(void);
		DfgOperator* getOperator(void) { return _op; }
		void computeSNR(void);

		void balance(DfgMan* pMan);
		void balanceAreaRecovery(set <DfgNode*> & sVisited);
		void updateDelayAndNumRefs(void);
		unsigned int numRefs(void)const;
		void incRef(void);
		unsigned int getArrivalTime(void)const;
		void setArrivalTime(unsigned int d);
		void incArrivalTime(unsigned int d);
		void decArrivalTime(unsigned int d);
		unsigned int getRequiredTime(void)const;
		void setRequiredTime(unsigned int d);
		void incRequiredTime(unsigned int d);
		void decRequiredTime(unsigned int d);

		unsigned int getOpDelay(void)const;

		static void setOpDelays(unsigned int adddelay, unsigned int subdelay, unsigned int muldelay, unsigned int lshdelay, unsigned int regdelay = 0, unsigned int divdelay = 0);
		static void setEnvOpDelays(void);
		static unsigned int delayAdd(void);
		static unsigned int delaySub(void);
		static unsigned int delayMul(void);
		static unsigned int delayDiv(void);
		static unsigned int delayLSH(void);
		static unsigned int delayReg(void);
	};

	struct _DfgNode_ltDelay {
		bool operator()(DfgNode* n1, DfgNode* n2)const {
			return(n1->getArrivalTime()< n2 ->getArrivalTime());
		}
	};

	struct _lt_slack {
		bool operator()(DfgNode* n1, DfgNode* n2)const {
			return(n1->getRequiredTime()- n1->getArrivalTime()< n2->getRequiredTime()- n2->getArrivalTime());
		}
	};


#ifdef INLINE
#include "DfgNode.inl"
#endif

}
#endif
