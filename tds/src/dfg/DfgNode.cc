/*
 * =====================================================================================
 *
 *       Filename:  TedDecompose.c
 *    Description:  TedDecompose class
 *        Created:  04/30/2007 08:59:36 PM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 208                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <climits>
#include <cmath>

#ifndef UINT_MAX
# define UINT_MAX 0xFFFF
#endif

#include "util.h"
using util::Util;

#include "Environment.h"
using tds::Environment;


#include "DfgNode.h"
#include "DfgMan.h"
#include "DfgOperator.h"

#include "Bitwidth.h"
using data::Bitwidth;

#include "TedVarMan.h"
using ted::TedVarMan;

namespace dfg {
#ifndef INLINE
#include "DfgNode.inl"
#endif

	DfgNode::DfgNode(DfgMan* pMan, DfgOperator::Type op, DfgNode* left, DfgNode* right):
		//	_type(op),
		_left(left), _right(right), _pMan(pMan), _op(NULL), _level(0), _value(0) {
			_op = new DfgOperator(op);
			_data.ptr = NULL;
			_pMan->registerNode(this);
			_nrefs = 0;
			if(_left && _right) {
				setArrivalTime((_left->getArrivalTime()> _right->getArrivalTime() ? _left->getArrivalTime(): _right->getArrivalTime())+ getOpDelay());
			} else {
				_arrival_time= 0;
			}
			_required_time = UINT_MAX;
		}

	DfgNode::DfgNode(DfgMan* pMan, const string& varname, DfgOperator::Type op) {
		//	_type = op;
		_op = new DfgOperator(op);
		_data.name = Util::strsav(varname.c_str());
		_left  = NULL;
		_right = NULL;
		_pMan  = pMan;
		_pMan->registerNode(this);

		_level = 0;
		_value = 0;
		_nrefs = 0;
		_arrival_time = 0;
		_required_time = UINT_MAX;
	}

	DfgNode::DfgNode(DfgMan* pMan, DfgNode* prev, unsigned int regamount):
		//	_type(DfgOperator::REG),
		_left(prev), _right(prev), _pMan(pMan), _op(NULL), _level(0), _value(0) {
			_op = new DfgOperator(DfgOperator::REG);
			setArrivalTime(prev->getArrivalTime()+ getOpDelay());
			string name = Util::itoa(regamount)+ TOKEN_REG;
			_data.name = Util::strsav(name.c_str());
			_required_time = UINT_MAX;
			_nrefs = 0;
			_pMan->registerNode(this);
		}

	DfgNode::DfgNode(DfgMan* pMan, wedge constvalue) {
		//	_type = DfgOperator::CONST;
		_op = new DfgOperator(DfgOperator::CONST);
		_data.value = constvalue;
		_left  = NULL;
		_right = NULL;
		_pMan  = pMan;
		_pMan->registerNode(this);

		_level = 0;
		_value = 0;
		_nrefs = 0;
		_arrival_time = 0;
		_required_time= UINT_MAX;
	}

	DfgNode::~DfgNode(void) {
		_pMan->unregisterNode(this);
		if(isPI()) {
			free(_data.name);
		}
	}

	void DfgNode::setKids(DfgNode* left, DfgNode* right) {
		_left = left;
		_right = right;
		if(_left && _right)
			setArrivalTime((_left->getArrivalTime()> _right->getArrivalTime() ? _left->getArrivalTime(): _right->getArrivalTime())+ getOpDelay());
	}


	const char* DfgNode::getOpStr(void)const {
		switch(getOp()) {
		case DfgOperator::ADD: return "+";
		case DfgOperator::MUL: return "*";
		case DfgOperator::DIV: return "/";
		case DfgOperator::SUB: return "-";
		case DfgOperator::LSH: return "<<";
		case DfgOperator::RSH: return ">>";
		case DfgOperator::REG: return _data.name;
		case DfgOperator::EQ: return "assign";
		default: return "??";
		}
	}
	static DfgNode* _RemoveNegPhase_rec(DfgMan* pMan, DfgNode* bNode, bool & bNegs) {
		DfgNode* left,* right;

		if(bNode->isConst()&& bNode->getConst() == -1) {
			bNegs ^= true;
			return NULL;
		} else if(!bNode->isMul()) {
			return bNode;
		}

		left  = _RemoveNegPhase_rec(pMan, bNode->getLeft(),  bNegs);
		right = _RemoveNegPhase_rec(pMan, bNode->getRight(),  bNegs);

		if(left != NULL && right != NULL) {
			bNode->setKids(left, right);
			return bNode;
		} else if(left == NULL && right != NULL) {
			return right;
		} else if(right == NULL && left != NULL) {
			return left;
		} else {
			return new DfgNode(pMan, 1);
		}
	}

	DfgNode* DfgNode::add(DfgNode* t) {
		DfgNode* tt;
		bool bLeftNeg, bRightNeg;
		DfgNode* left,* right;
		bLeftNeg = bRightNeg = false;

		if(isConst()&& t->isConst()) {
			return new DfgNode(_pMan, getConst()+ t->getConst());
		}

		if(isConst()) {
			if(getConst() == 0) {
				return t;
			} else if(getConst()< 0) {
				return t->sub(new DfgNode(_pMan, getConst()*-1));
			}
		}

		if(t->isConst()) {
			if(t->getConst() == 0) {
				return this;
			} else if(t->getConst()< 0) {
				return this->sub(new DfgNode(_pMan, t->getConst()* -1));
			}
		}


		if(this->isMul()) {
			left = _RemoveNegPhase_rec(_pMan, this, bLeftNeg);
		}  else {
			left = this;
		}

		if(t->isMul()) {
			right = _RemoveNegPhase_rec(_pMan, t, bRightNeg);
		}  else {
			right = t;
		}

		if(bLeftNeg == bRightNeg) {
			tt = new DfgNode(_pMan, DfgOperator::ADD, left, right);
			if(bLeftNeg) {
				tt = tt->mul(new DfgNode(_pMan, -1));
			}
		} else {
			if(bLeftNeg) {
				//tt = new DfgNode(_pMan, DfgOperator::SUB, right, left);
				tt = right->sub(left);
			} else {
				//tt = new DfgNode(_pMan, DfgOperator::SUB, left, right);
				tt = left->sub(right);
			}
		}
		return tt;
	}

	DfgNode* DfgNode::sub(DfgNode* t) {
		DfgNode* pTemp;
		if(isConst()&& t->isConst()) {
			return new DfgNode(_pMan, getConst()- t->getConst());
		}
		if(t->isConst()&& t->getConst() == 0) {
			return this;
		}

		/*
		   pTemp = t->neg();
		   if(pTemp != NULL) {
		   return new DfgNode(_pMan, DfgOperator::ADD, this, pTemp);
		   } else {
		   return  new DfgNode(_pMan, DfgOperator::SUB, this, t);
		   }
		   */
		pTemp = new DfgNode(_pMan, DfgOperator::SUB, this, t);
		return pTemp;
	}

	DfgNode* DfgNode::reg(unsigned int value) {
		return(value==0) ? this : new DfgNode(_pMan,this,value);
	}

	DfgNode* _NegateAddOrSubNode(DfgNode* t) {
		DfgNode* l,* r;

		if(!t->isAddorSub()) {
			return NULL;
		}

		if(t->isSub()) {
			t->setKids(t->getRight(), t->getLeft());
			return t;
		} else if(t->isAdd()) {
			l = _NegateAddOrSubNode(t->getLeft());
			r = _NegateAddOrSubNode(t->getRight());

			if(l != NULL && r != NULL) {
				t->setKids(l, r);
			} else {
				return NULL;
			}
		}
		return NULL;
	}


	DfgNode* DfgNode::mul(DfgNode* t) {
		DfgNode* pLeft,* pRight,* pLeftNeg;

		if(isConst()&& t->isConst()) {
			return new DfgNode(_pMan, getConst()* t->getConst());
		}

		if(isConst()) {
			pLeft = t;
			pRight = this;
		} else {
			pLeft = this;
			pRight = t;
		}

		if(pRight->isConst()&& pRight->getConst() == 1) {
			return pLeft;
		}
		if(pRight->isConst()&& pRight->getConst()< 0) {
			pLeftNeg = pLeft->neg();
			if(pLeftNeg != 0) {
				return pLeftNeg->mul(new DfgNode(_pMan, pRight->getConst()*-1));
			}
		}
		return  new DfgNode(_pMan, DfgOperator::MUL, pLeft, pRight);
	}

	/**
	 * @brief Recursive traversal of DFG nodes, storing all nodes into
	 *        vNodes, and annotating in each node its level
	 *
	 * @pre   a new mark should have been started
	 * @see   Mark::startNewMark
	 * @param vNodes Container for all DFG Nodes.
	 * @param vNodes Maps each node to a particular level.
	 *
	 * @return The maximum number of levels in the DFG graph, or subgraph.
	 **/
	unsigned int DfgNode::collectDFS(vector <DfgNode*> & vNodes) {
		if(_visited.isMarked())
			return _level;
		if(isOp()) {
			int left_level = getLeft()->collectDFS(vNodes);
			int right_level = getRight()->collectDFS(vNodes);
			// From both left and right subgraphs, choose the max level
			_level = MAX(left_level, right_level)+1;
		} else {
			// Variables and Constant values do not increment the DFG level
			_level = 0;
		}
		// pair "this" node with current "level"
		vNodes.push_back(this);
		_visited.setMark();
		return _level;
	}

	void DfgNode::collectDFSonly(vector <DfgNode*> & vNodes) {
		if(_visited.isMarked())
			return;
		if(isOp()) {
			getLeft()->collectDFSonly(vNodes);
			getRight()->collectDFSonly(vNodes);
			// From both left and right subgraphs, choose the max level
		}
		// pair "this" node with current "level"
		vNodes.push_back(this);
		_visited.setMark();
	}

	void DfgNode::computeSNR(void) {
		if(_visited.isMarked())
			return;
		DfgOperator* left = NULL;
		DfgOperator* right = NULL;
		if(isOp()) {
			left = getLeft()->_op;
			right = getRight()->_op;
			if(!left || !right) {
				throw(string("01031. Error."));
			}
			getLeft()->computeSNR();
			getRight()->computeSNR();
		}
		if(!_op) {
			throw(string("01032. Error."));
		} else {
			if(_op->getError()) {
				_op->clearError();
			}
		}
		switch(getOp()) {
		case DfgOperator::SUB:
		case DfgOperator::ADD:
			{
				assert(left->getError());
				assert(right->getError());
				Error* err = left->getError()->add(right->getError());
				err->insert(_op->getBitwidth()->getSize(),_op->getBitwidth()->getError());
				_op->set(err);
				break;
			}
		case DfgOperator::MUL:
			{
				assert(left->getError());
				assert(right->getError());
				Error* err = left->getError()->mpy(right->getError());
				err->insert(_op->getBitwidth()->getSize(),_op->getBitwidth()->getError());
				_op->set(err);
				break;
			}
		case DfgOperator::LSH:
		case DfgOperator::RSH:
			{
				throw(string("01033. DfgOperator::LSH, DfgOperator::RSH not implemented."));
				break;
			}
		case DfgOperator::CONST:
			{
				Error* err = new Error((long double)_data.value,(long double)_data.value,0.0);
				_op->set(err);
				break;
			}
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			{
				Error* err = TedVarMan::instance().getIfExist(getName())->getError();
				if(err) {
					err = new Error(err);
					_op->set(err);
				} else {
					err = new Error(1,1,0);
					_op->set(err);
					//throw(string("01018. No bitwidth defined for variable \"")+getName()+string("\"."));
				}
				break;
			}
		default:
			throw(string("01034. Unknow DfgNode type."));
			break;
		}
		_visited.setMark();
	}

	/** @brief compute the bitwith of the node by traversing its cone up to the operators at level(default PIs=0)*/
	void DfgNode::computeBitwidth(bool(DfgNode::*Functor)(unsigned int), unsigned int level) {
		if(Functor &&(this->*Functor)(level))
			return;
		if(_visited.isMarked())
			return;
		if(isOp()&& !isReg()) {
			getLeft()->computeBitwidth(Functor,level);
			getRight()->computeBitwidth(Functor,level);
		}
		computeBitwidth();
		_visited.setMark();
	}

	void DfgNode::computeBitwidth(void) {
		DfgOperator* left = NULL;
		DfgOperator* right = NULL;
		if(isOp()&& !isReg()) {
			left = getLeft()->getOperator();
			right = getRight()->getOperator();
			if(!left || !right) {
				throw(string("01020. No bitwidth set yet."));
			}
		}
		if(!_op) {
			_op = new DfgOperator(getOp());
		} else {
			if(_op->getBitwidth()) {
				_op->clearBitwidth();
			}
		}
		switch(getOp()) {
		case DfgOperator::SUB:
		case DfgOperator::ADD:
			{
				Bitwidth* pre = left->getBitwidth()->clone();
				pre->add(right->getBitwidth());
				_op->set(pre);
				break;
			}
		case DfgOperator::MUL:
			{
				Bitwidth* pre = left->getBitwidth()->clone();
				pre->mpy(right->getBitwidth());
				_op->set(pre);
				break;
			}
		case DfgOperator::LSH:
			{
				unsigned int amount = getRight()->getValue();
				assert(amount>0);
				Bitwidth* pre = left->getBitwidth()->clone();
				pre->lsh(amount);
				_op->set(pre);
				break;
			}
		case DfgOperator::RSH:
			{
				unsigned int amount = getRight()->getValue();
				assert(amount>0);
				Bitwidth* pre = left->getBitwidth()->clone();
				pre->rsh(amount);
				_op->set(pre);
				break;
			}
		case DfgOperator::CONST:
			{
				long double weight = _data.value;
				if(weight <1)
					weight*= -1;
				double bits_needed = log((double)weight)/log((double)2);
				double bits_upbound = ceil(bits_needed);
				unsigned int bits = (unsigned int)bits_upbound;
				if(bits_upbound == bits_needed) {
					//i.e. 16 decimal do not require 4 bits but 5.
					bits++;
				}
				Bitwidth* pre = new Integer(bits);
				_op->set(pre);
				break;
			}
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			{
				Bitwidth* pre = TedVarMan::instance().getIfExist(getName())->getBitwidth();
				if(pre) {
					pre = pre->clone();
					_op->set(pre);
				} else {
					delete _op;
					_op = NULL;
					throw(string("01018. No bitwidth defined for variable \"")+getName()+string("\"."));
				}
				break;
			}
		case DfgOperator::REG:
			throw(string("01036. Bitwidth computation does not handle registers yet."));
		default:
			throw(string("01017. Unknow DfgNode type."));
			break;
		}
	}

#ifdef FUNCTIONS_DEPRECATED
	unsigned int DfgNode::collectDFS(vector <DfgNode*> & vNodes,
									 map <DfgNode*, unsigned int>*  mNodeLevels) {
		map <DfgNode*, unsigned int>::iterator mp;
		unsigned int left_level, right_level, level;
		assert(mNodeLevels);
		if((mp = mNodeLevels->find(this))!= mNodeLevels->end())
			return mp->second;
		if(isOp()) {
			left_level = getLeft()->collectDFS(vNodes, mNodeLevels);
			right_level = getRight()->collectDFS(vNodes, mNodeLevels);
			// From both left and right subgraphs, choose the max level
			level = MAX(left_level, right_level)+1;
		} else {
			// Variables and Constant values do not increment the DFG level
			level = 0;
		}
		// pair "this" node with current "level"
		(*mNodeLevels)[this] = level;
		vNodes.push_back(this);
		return level;
	}
#endif

	void DfgNode::_CollectBalanceSupportAndCone(vector <DfgNode*> &vSupp, vector <DfgNode*> &vCone) {

		if(this->getLeft()) {
			if(this->getLeft()->numRefs() == 1 &&((this->getLeft()->isMul()&& this->isMul())||(this->getLeft()->isAddorSub()&&  this->isAddorSub()))) {
				this->getLeft()->_CollectBalanceSupportAndCone(vSupp, vCone);
			} else {
				vSupp.push_back(this->getLeft());
			}
		}
		if(this->getRight()) {
			if(this->getRight()->numRefs() == 1 &&((this->getRight()->isMul()&& this->isMul())||(this->getRight()->isAddorSub()&&  this->isAddorSub()))) {
				this->getRight()->_CollectBalanceSupportAndCone(vSupp, vCone);
			} else {
				vSupp.push_back(this->getRight());
			}
		}
		vCone.push_back(this);
	}

	void DfgNode::_UpdateSupportPhase_rec(map <DfgNode*, bool> & mPhase, unsigned int nNegs) {
		map <DfgNode*, bool>::iterator p;

		if((p = mPhase.find(this))!= mPhase.end()) {
			if((nNegs%2) == 0) {
				p->second = true;
			} else {
				p->second = false;
			}
			return;
		}
		getLeft()->_UpdateSupportPhase_rec(mPhase, nNegs);
		if(this->isSub()) {
			getRight()->_UpdateSupportPhase_rec(mPhase, nNegs + 1);
		} else {
			getRight()->_UpdateSupportPhase_rec(mPhase, nNegs);
		}
	}

	void DfgNode::balanceAreaRecovery(set <DfgNode*> & sVisited) {
		DfgNode* pLeft,* pRight,* pTemp;

		if(sVisited.find(this)!= sVisited.end())return;
		if((pLeft = getLeft())!= NULL)pLeft->balanceAreaRecovery(sVisited);
		if((pRight= getRight())!= NULL)pRight->balanceAreaRecovery(sVisited);

		if(isOp()&& getRequiredTime()> getArrivalTime()
		   && pLeft != NULL && pLeft->isOp()&& pLeft->getRequiredTime()> pLeft->getArrivalTime()
		   && pRight != NULL && pRight->isOp()&& pRight->getRequiredTime()> pRight->getArrivalTime()) {
			bool bFlag = true;
			if(isAddorSub()) {
				if(pRight->isAddorSub()) {
					;
				} else if(pLeft->isAddorSub()) {
					pTemp = pRight;
					pRight = pLeft;
					pLeft = pTemp;
				} else {
					bFlag = false;

				}
			}
			if(isMul()) {
				if(pRight->isMul()) {
					;
				} else if(pLeft->isMul()) {
					pTemp = pRight;
					pRight = pLeft;
					pLeft = pTemp;
				} else {
					bFlag = false;
				}
			}

			if(bFlag && pRight->getRight()!= NULL && pRight->getLeft()!= NULL && pRight->numRefs() == 1) {
				pTemp = pRight->getRight();
				pRight->setKids(pLeft, pRight->getLeft());
				setKids(pRight, pTemp);
			} else if(bFlag && pLeft->getRight()!= NULL && pLeft->getLeft()!= NULL && pLeft->numRefs() == 1) {
				pTemp = pLeft->getRight();
				pLeft->setKids(pLeft->getLeft(), pRight);
				setKids(pTemp, pLeft);
			}
		}
		sVisited.insert(this);
	}

	void DfgNode::balance(DfgMan* pMan) {

		vector <DfgNode*> vSupp;
		vector <DfgNode*> vCone;

		this->_CollectBalanceSupportAndCone(vSupp, vCone);
		if(vSupp.size() == 0) {
			return;
		}
		for(unsigned int i = 0; i < vSupp.size(); i++) {
			vSupp[i]->balance(pMan);
		}
		_BalanceCone(pMan, vSupp, vCone);
	}


	void DfgNode::_BalanceCone(DfgMan* pMan, vector <DfgNode*> &vSupp, vector <DfgNode*> &vCone) {
		//QREN
		//return;

		multiset <DfgNode*, _DfgNode_ltDelay> sNodes;
		multiset <DfgNode*, _DfgNode_ltDelay>::iterator p;
		map <DfgNode*, bool> mNodePositivePhase;
		DfgNode* left,* right,* pNode;
		bool bPhase, bPhaseLeft, bPhaseRight;

		for(unsigned int i = 0; i < vSupp.size(); i++) {
			sNodes.insert(vSupp[i]);
			mNodePositivePhase[vSupp[i]] = true;
		}

		if(this->isAddorSub()) {
			_UpdateSupportPhase_rec(mNodePositivePhase);
		}

		while(sNodes.size()> 1) {
			p = sNodes.begin();
			left  =*p;
			sNodes.erase(p);
			p = sNodes.begin();
			right =*p;
			sNodes.erase(p);
			if(this->isMul()) {
				// pNode = new DfgNode(this->var);
				pNode = new DfgNode(pMan, DfgOperator::MUL, left, right);
				sNodes.insert(pNode);
			} else if(this->isAddorSub()) {
				bPhaseLeft = mNodePositivePhase[left];
				bPhaseRight = mNodePositivePhase[right];

				if(bPhaseLeft && bPhaseRight) {
					pNode = new DfgNode(pMan, DfgOperator::ADD, left, right);
					bPhase = true;
				} else if(bPhaseLeft && !bPhaseRight) {
					pNode = new DfgNode(pMan, DfgOperator::SUB, left, right);
					bPhase = true;
				} else if(!bPhaseLeft && bPhaseRight) {
					pNode = new DfgNode(pMan, DfgOperator::SUB, right, left);
					bPhase = true;
				} else {
					pNode = new DfgNode(pMan, DfgOperator::ADD, left, right);
					bPhase = false;
				}
				sNodes.insert(pNode);
				mNodePositivePhase[pNode] = bPhase;
			} else if(this->isLSH()) {
				pNode = new DfgNode(pMan, DfgOperator::LSH, left, right);
				sNodes.insert(pNode);
			} else if(this->isReg()) {
				throw string("01035. Balance does not handle registers yet.");
				//pNode = new DfgNode(pMan,DfgOperator::REG,left,right);
			} else {
				assert(0);
			}
		}

		p = sNodes.begin();
		pNode =*p;
		sNodes.erase(p);

		if(this->isAddorSub()) {
			assert(pNode->isAddorSub());
			bPhase = pNode->_UpdateConePhase_rec(mNodePositivePhase);
			if(bPhase == false) {
				pNode = pNode->mul(new DfgNode(pMan, -1));
			}
		}

		this->setType(pNode->getOp());
		this->setKids(pNode->getLeft(), pNode->getRight());

		//delete pNode;
		for(unsigned int i = 0; i < vCone.size(); i++) {
			if(vCone[i] == this)continue;
			delete vCone[i];
		}
	}

	/*
	   void DfgNode::UpdateDelayAndNumRefs(void) {
	   vector <DfgNode*> vNodes;
	   unsigned int dLeft, dRight;
	   collectDFS(vNodes);

	   for(unsigned int i = 0; i < vNodes.size(); i++) {
	   vNodes[i]->_nrefs = 0;
	   if(vNodes[i]->getLeft())vNodes[i]->getLeft()->incRef();
	   if(vNodes[i]->getRight())vNodes[i]->getRight()->incRef();

	   dLeft = dRight = 0;
	   if(vNodes[i]->getLeft())dLeft  = vNodes[i]->getLeft()->getArrivalTime();
	   if(vNodes[i]->getRight())dRight = vNodes[i]->getRight()->getArrivalTime();
	   vNodes[i]->setArrivalTime((dLeft>dRight?dLeft:dRight)+ vNodes[i]->getOpDelay());
	   }
	   }
	   */

	bool DfgNode::_UpdateConePhase_rec(map <DfgNode*, bool> & mPhase) {
		map <DfgNode*, bool>::iterator p;
		bool bRet = false;

		if((p = mPhase.find(this))!= mPhase.end()) {
			return p->second;
		}
		bool bLeft  = getLeft()->_UpdateConePhase_rec(mPhase);
		bool bRight = getRight()->_UpdateConePhase_rec(mPhase);

		unsigned int c = 0;
		if(bLeft)c ^= 0x01;
		if(bRight)c ^= 0x02;

		switch(c) {
		case 0x0: //both negative
			bRet = false;
			break;
		case 0x2: //getLeft  negative
			swapKid();  //than follow  case 1
		case 0x1: //getRight negative
			//this->_type == '-';
			this->setType(DfgOperator::SUB);
			bRet = true;
			break;
		case 0x3: //both positive
			bRet = true;
			break;
		}

		return bRet;
	}

	unsigned int DfgNode::_addDelay=0, DfgNode::_subDelay=0, DfgNode::_mpyDelay=0;
	unsigned int DfgNode::_divDelay=0, DfgNode::_lshDelay=0, DfgNode::_rshDelay=0;
	unsigned int DfgNode::_regDelay=0;

	unsigned int DfgNode::getOpDelay(void)const {
		if(this->isMul()) {
			return delayMul();
		} else if(this->isAdd()) {
			return delayAdd();
		} else if(this->isSub()) {
			return delaySub();
		} else if(this->isLSH()) {
			return delayLSH();
		} else if(this->isReg()) {
			return delayReg();
		} else if(this->isAssign()) {
			return 1;
		} else {
			return 0;
		}
	}

	char* DfgNode::getSymbolSave(void) {
		char* p;
		string s;
		switch(getOp()) {
		case DfgOperator::CONST:
			s = Util::itoa(getConst());
			break;
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			s = getName();
			break;
		case DfgOperator::REG:
		case DfgOperator::ADD:
		case DfgOperator::SUB:
		case DfgOperator::MUL:
		case DfgOperator::DIV:
		case DfgOperator::LSH:
		case DfgOperator::RSH:
		case DfgOperator::EQ:
			s = getOpStr();
			break;
		default:
			throw(string("01024. UNKOWN DFG node type"));
		}
		return(p = (char*)Util::strsav(s.c_str()));
	}

	DfgNode* DfgNode::neg(void) {
		DfgNode* pLeft,* pRight;
		switch(getOp()) {
		case DfgOperator::SUB:
			pLeft= getLeft()->neg();
			pRight = getRight()->neg();
			/*
			   if(pLeft != NULL && pRight != NULL) {
			   return(pLeft->sub(pRight));
			   } else if(pLeft == NULL && pRight != NULL) {
			   return(getLeft()->add(pRight));
			   } else if(pLeft != NULL && pRight == NULL) {
			   return(pLeft->add(getRight()));
			   } else {
			   return getRight()->sub(getLeft());
			   }
			   */
			return getRight()->sub(getLeft());
		case DfgOperator::ADD:
			pLeft= getLeft()->neg();
			pRight = getRight()->neg();
			if(pLeft != NULL && pRight != NULL) {
				return getLeft()->neg()->add(getRight()->neg());
			} else {
				return NULL;
			}
		case DfgOperator::MUL:
			pLeft= getLeft()->neg();
			pRight = getRight()->neg();
			if(pLeft == NULL && pRight == NULL) {
				return NULL;
			} else if(pLeft != NULL) {
				return pLeft->mul(getRight());
			} else {
				return pRight->mul(getLeft());
			}
			break;
		case DfgOperator::VAR:
		case DfgOperator::VARCONST:
		case DfgOperator::CONST:
		default:
			return NULL;
		}
	}

	void DfgNode::setEnvOpDelays(void) {
		_addDelay = Environment::getInt("delayADD");
		_subDelay = Environment::getInt("delaySUB");
		_mpyDelay = Environment::getInt("delayMPY");
		_divDelay = 0; //Environment::getInt("delayDfgOperator::DIV");
		_lshDelay = Environment::getInt("delayLSH");
		_rshDelay = Environment::getInt("delayRSH");
		_regDelay = Environment::getInt("delayREG");
	}

	TedVar* DfgNode::getTedVarIfExist(void)const {
		return TedVarMan::instance().getIfExist(getName());
	}

}
