/*
 * =====================================================================================
 *
 *        Filename:  TedNode.cc
 *     Description:  TedNode class
 *         Created:  04/17/2007 12:53:32 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-08 15:28:25 +0100(Fri, 08 Jan 2010)$: Date of last commit
 * =====================================================================================
 */

#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>

#include "util.h"
using util::Util;

#include "TedNode.h"
#include "TedKids.h"
#include "TedNodeIterator.h"
#include "TedParents.h"

#include "Bitwidth.h"
using data::Bitwidth;

namespace ted {
#ifndef INLINE
# 	include "TedNode.inl"
#endif

	const TedNode TedNode::_one(1);
	const TedNode* TedNode::_pOne = &TedNode::_one;

	//inline
	TedNode::dfs_iterator TedNode::dfs_begin(void) {
		return dfs_iterator(this,dfs_iterator::begin);
	}

	//inline
	TedNode::dfs_iterator TedNode::dfs_end(void) {
		return dfs_iterator(dfs_iterator::end);
	}

	//inline
	TedNode::const_dfs_iterator TedNode::const_dfs_begin(void)const{
		return const_dfs_iterator(this,const_dfs_iterator::begin);
	}

	//inline
	TedNode::const_dfs_iterator TedNode::const_dfs_end(void)const{
		return const_dfs_iterator(const_dfs_iterator::end);
	}

	/** @brief construct a TedNode from a variable name*/
	TedNode::TedNode(const string& name, bool binary): _register(0), _nRefs(0), _tVar(NULL), _tKids(NULL) {
		pair <bool, wedge> pa = Util::atoi(name.c_str());
		_nRefs = 0;
		if(pa.first == true) {
			_tVar = TedVarMan::instance().createVar(pa.second);
		} else {
			_tVar  = TedVarMan::instance().createVar(name,binary);
		}
#ifdef _DEBUG
		safe_guard = 0xbebe0000;
#endif
	}

	TedNode::~TedNode(void) {
		if(_tVar && _tVar->isConst() && _tVar!=_pOne->_tVar) {
			delete _tVar;
		}
		if(_bitw) {
			delete _bitw;
		}
		_tVar = NULL;
		_tKids = NULL;
		_bitw = NULL;
		_backptr = NULL;
#ifdef _DEBUG
		safe_guard = 0xdeadbeaf;
#endif
	}

	template<typename T>
		void TedNode::preOrderDFS(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int)) {
			Mark<TedNode>::newMark();
			_preOrderDFS_rec(obj,functor);
		}

	/** @brief starting from "this" TedNode, executes functor in a pre-order DFS traversal*/
	template<typename T>
		void TedNode::_preOrderDFS_rec(T& obj, void(T::*functor)(TedNode*,TedNode*,unsigned int)) {
			if(visited.isMarked()) {
				return;
			}
			unsigned int index=0;
			TedNode* pKid=NULL;
			wedge weight=0;
			FOREACH_KID_OF_NODE(this) {
				index = _iterkids.getIndex();
				pKid = _iterkids.Node<TedNode>();
				weight = _iterkids.getWeight();
				(obj.*functor)(pKid,this,index);
				pKid->_preOrderDFS_rec(obj,functor);
			}
			visited.setMark();
		}

	//force instantiation of TedParent - the above template does not work maybe because of recursion.
	template<>
		void TedNode::_preOrderDFS_rec<TedParents>(TedParents& obj, void(TedParents::*functor)(TedNode*,TedNode*,unsigned int)) {
			if(visited.isMarked()) {
				return;
			}
			unsigned int index=0;
			TedNode* pKid=NULL;
			FOREACH_KID_OF_NODE(this) {
				index = _iterkids.getIndex();
				pKid = _iterkids.Node<TedNode>();
				(obj.*functor)(pKid,this,index);
				pKid->_preOrderDFS_rec(obj,functor);
			}
			visited.setMark();
		}

	/** @brief collect child nodes in the order of dfs*/
	void TedNode::_collectDFS_rec(vector<TedNode*> & vNodes) {
		if(visited.isMarked()) {
			return;
		}
		TedNode* pKid=NULL;
		FOREACH_KID_OF_NODE(this) {
			pKid = _iterkids.Node<TedNode>();
			pKid->_collectDFS_rec(vNodes);
		}
		visited.setMark();
		_tVar->visited.setMark();
		vNodes.push_back(this);
	}

	void TedNode::cleanBitwidth(void) {
		if(_bitw) {
			delete _bitw;
			_bitw = NULL;
		}
		FOREACH_KID_OF_NODE(this) {
			(*_iterkids).cleanBitwidth();
		}
	}

	void TedNode::multiply(wedge weight) {
		_tKids->pushGCD(weight);
	}

	void TedNode::update_cone_TMark(void) {
		if(visited.isMarked()) {
			return;
		}
		TedNode* pKid=NULL;
		FOREACH_KID_OF_NODE(this) {
			pKid = _iterkids.Node<TedNode>();
			pKid->update_cone_TMark();
		}
		if (getKids()) {
			getKids()->visited.setMark();
		}
		visited.setMark();
		//	_tVar->visited.setMark();
	}

	void TedNode::update_cone_PMark(void) {
		if(visited.isMarked()) {
			return;
		}
		TedNode* pKid=NULL;
		FOREACH_KID_OF_NODE(this) {
			pKid = _iterkids.Node<TedNode>();
			pKid->update_cone_PMark();
		}
		visited.setMark();
		visited.setPMark();
		if (getKids()) {
			getKids()->visited.setPMark();
		}
		//_tVar->visited.setMark();
	}

}
