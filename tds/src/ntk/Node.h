/*
 * =====================================================================================
 *
 *       Filename:  Node.h
 *    Description:  Node class
 *        Created:  05/07/2007 10:30:42 AM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */


#ifndef __NETWORK_NODE_H__
#define __NETWORK_NODE_H__

#include <string>
#include <map>
#include <set>
#include <vector>
#include <cassert>

#include "Mark.h"
using dtl::Mark;

#include "types.h"

namespace network {

	using namespace std;

#define FOREACH_FANIN_OF_NODE(n, i, fin)for(i = 0;i < n->numFanins(); i++)if(fin = n->getFanin(i))
#define FOREACH_FANOUT_OF_NODE(n, i, fout)for(i = 0;i < n->numFanouts(); i++)if(fout = n->getFanout(i))

#define FUNC_ADD "+"
#define FUNC_SUB "-"
#define FUNC_MUL "*"
#define FUNC_SLL "<<"
#define FUNC_REG "DFF"
#define FUNC_TED "$ted"
#define FUNC_ERROR "[?]"
#define FUNC_ASSIGN "assign"
#define FUNC_LOOPBACK "loopback"

	//enum FUNC_TYPE {LATCH,MPY,SUB,ADD,DIV,SRA,SLA,SRL,SLL,ROR,ROL,EQ,NE,LT,LE,GT,GE,EQMUX,NEMUX,LTMUX,LEMUX,GEMUX,GTMUX,OR,AND,XOR,NAND,NOR,ABS,ASSIGN};
	enum NODE_TYPE {DFF,CONST,CONSTVAR,PI,PO,VAR,TEMPORARY,OPERATOR,TED};  //STR: structural nodes

	enum ROUNDING {AC_RND, AC_TRN};
	enum OVERFLOW_ {AC_WRAP, AC_SAT};

	class CDFGData {
	public:
		int bitwidth;
		int sign;
		int bank;
		int address;
		wedge value;
		int hardwire;
		int time;
		int port;
		int age;
		int fixedpoint;
		string aging;
		string loopback;
		ROUNDING rtype;
		OVERFLOW_ otype;
		CDFGData(void): bitwidth(32), sign(0), bank(0), address(0), value(0), \
						hardwire(0), time(0), port(0), age(0), fixedpoint(0), \
									aging(""), loopback(""), rtype(AC_RND), otype(AC_WRAP) {};
		~CDFGData(void) {};
	};

	/**
	 * @class Node
	 * @brief The nodes in the netlist.
	 **/
	class Node {
		friend class Netlist;
	private:
		string _name;
		string _func;  //functional type
		unsigned int _level;
		void* _pData;
		vector <Node*> _fanouts;
		vector <Node*> _fanins;

		int _arrTime;
		int _reqTime;
		unsigned int _nRefs;

		static bool _bAddingConstPrefix;
	public:
		NODE_TYPE _type;  //structural type
		CDFGData _data;
		Mark<Node> visited;

		Node(const char* name);
		Node(const char* name, NODE_TYPE type);
		Node(wedge constValue);

		const char* getName(void);
		void setName(const char* name);
		NODE_TYPE getType(void);
		void setType(NODE_TYPE type);

		bool isPI(void);
		bool isConst(void);
		bool isConstVar(void);
		bool isVar(void);
		bool isTemporary(void);
		bool isPO(void);
		bool isOperator(void);
		bool isInterface(void);

		const char* func(void);
		void setFunc(const char*);
		bool isAdd(void);
		bool isSub(void);
		bool isMul(void);
		bool isArithmetic(void);
		bool isReg(void);
		bool isSll(void);
		bool isAssign(void);

		void* getData(void);
		void setData(void* pData);
		void setData(const CDFGData&);

		void addFanin(Node* fin);
		void addFanout(Node* fout);
		Node* getFanin(unsigned i);
		Node* getFanout(unsigned i);
		size_t numFanins(void);
		size_t numFanouts(void);
		void clearFanin(void);
		void clearFanout(void);

		int getArrTime(void);
		int getReqTime(void);
		void setArrTime(int t);
		void setReqTime(int t);

		int NumRefs(void);
		void incRef(void);
		void decRef(void);

		void clearArrReqTime(void);
		bool isCleanArrReqTime(void);
		Node* duplicate(void);

		static void startAddingCostantPrefix(void);
		static void stopAddingCostantPrefix(void);

		void dotString(string &, bool bVerbose = false);
		//	    Node* MergeFanin(unsigned int);

		//bool isConst(void);
		long getConst(void);

	private:

		void balanceSupport(vector <Node*> &vSupp);
		void updateSupportPhase_rec(map <Node*, bool> & mPhase, unsigned int nNegs = 0);
		void collectBalanceSupportAndCone(vector <Node*> & vSupp, vector <Node*> & vCone);
		void balance_rec(void);
		void updateRequiredTime(int req);
		bool updateConePhase_rec(map <Node*, bool> & mPhase);
	};

#ifdef INLINE
#include "Node.inl"
#endif

}

#endif

