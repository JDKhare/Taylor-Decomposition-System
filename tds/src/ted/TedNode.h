/*
 * =====================================================================================
 *
 *        Filename:  TedNode.h
 *     Description:  TedNode class header
 *         Created:  04/17/2007 12:13:04 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-01-12 14:44:42 +0100(Tue, 12 Jan 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDNODE_H__
#define __TEDNODE_H__

#include <vector>
#include <set>
#include <map>

#include "TedVar.h"
#include "TedKids.h"
#include "TedNodeIterator.h"
#include "TedRegister.h"
#include "TedVarMan.h"

#include "Mark.h"
using dtl::Mark;

namespace data {
	class Bitwidth;
}
using namespace data;

namespace ted {

	using namespace std;
	class TedMan;

	/**
	 * @class TedNode
	 * @brief Implement the nodes of the TED
	 **/
	class TedNode {
		friend class TedMan;
	protected:
		static const TedNode _one;
		static const TedNode* _pOne;
		TedRegister _register;
		/* Number of references to a TedNode*/
		unsigned int _nRefs;
		/* the variable*/
		TedVar* _tVar;
		/* the kids*/
		TedKids* _tKids;
		Bitwidth* _bitw;
		TedNode* _backptr;
#ifdef _DEBUG
		int safe_guard;
#endif

		void _collectDFS_rec(vector <TedNode*> & vNodes);
		template<typename T>
			void _preOrderDFS_rec(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int));

	public:
		mutable Mark<TedNode> visited;

		typedef const_iteratorDFS<TedNode> const_dfs_iterator;
		const_dfs_iterator const_dfs_begin(void)const;
		const_dfs_iterator const_dfs_end(void)const;
		typedef iteratorDFS<TedNode> dfs_iterator;
		dfs_iterator dfs_begin(void);
		dfs_iterator dfs_end(void);

		static TedNode* getOne(void);
		explicit TedNode(void);
		explicit TedNode(const string &, bool binary=false);
		explicit TedNode(wedge constvalue);
		explicit TedNode(const TedVar & t, const TedKids & k, const TedRegister & r);
		explicit TedNode(const TedVar & t, const TedKids & k);
		explicit TedNode(const TedVar&);
		explicit TedNode(const TedNode&);

		virtual ~TedNode(void);

		const TedVar* getVar(void)const;
		void setVar(const TedVar &);
		TedKids* getKids(void)const;
		void setKids(TedKids &);
		void setKids(TedKids*);

		Bitwidth* getBitwidth(void)const;
		void setBitwidth(Bitwidth*);

		TedNode* getKidNode(unsigned int index)const;
		wedge getKidWeight(unsigned int index)const;
		void setKidWeight(unsigned int index, wedge weight);
		TedRegister getKidRegister(unsigned int index) const;
		void multiply(wedge weight);

		void replaceKid(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg = 0);
		void addKid(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg = 0);
		void setKid(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg = 0);
		void removeKid(unsigned int index);
		unsigned int numKids(void)const;
		bool hasKid(unsigned int index)const;

		void incRegister(const TedRegister& val) { _register += val; }
		void setRegister(const TedRegister& val) { _register = val; }
		TedRegister getRegister(void)const { return _register; }

		bool isConst(void)const;
		wedge getConst(void)const;
		void setConst(wedge);

		template<typename T>
			void preOrderDFS(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int));
		void collectDFS(vector <TedNode*> & vNodes);
		void update_cone_TMark(void);
		void update_cone_PMark(void);
#if 0
		virtual TedNode* duplicate(void);
#endif
		const string& getName(void)const;
		const string& getRootName(void) const;

		bool isBasic(void)const;

		void setBackptr(TedNode*);
		TedNode* getBackptr(void)const;

		void cleanBitwidth(void);
	};

#ifdef INLINE
# 	include "TedNode.inl"
#endif

}
#endif

