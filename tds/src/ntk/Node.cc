/*
 * =====================================================================================
 *
 *       Filename:  Node.c
 *    Description:  Node Class
 *        Created:  05/07/2007 10:39:58 AM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include <iostream>
#include <sstream>
#include <cstring>

#include "Node.h"

#include "util.h"
using util::Util;

#include "Tds.h"
#include "Environment.h"
using namespace tds;

namespace network {
#ifndef INLINE
#include "Node.inl"
#endif

	Node::Node(const char* name) {
		_name = name;
		_type = OPERATOR;
		_level = 0;
		_pData= NULL;
		_nRefs = 0;
		clearArrReqTime();
	}

	Node::Node(const char* name, NODE_TYPE type) {
		_name = name;
		_type = type;
		_level = 0;
		_pData= NULL;
		_nRefs = 0;
		clearArrReqTime();
	}

	Node::Node(wedge constValue) {
		_name = Environment::getStr("const_prefix");
#if 1
		if(constValue<0) {
			constValue*= -1;
			_name += Environment::getStr("negative_prefix");
		}
		_name += Util::itoa(constValue);
		_type = CONST;
#else
		_name += Util::itoa(constValue);
		_type = PI;
#endif
		_level = 0;
		_pData= NULL;
		_nRefs = 0;
		clearArrReqTime();
	}

	const char* Node::getName(void) {
#if 0
		if(_bAddingConstPrefix && Util::atoi(_name.c_str()).first) {
			string constname = Environment::getStr("const_prefix");
			constname += _name;
			return constname.c_str();
		}
#endif
		return _name.c_str();
	}

	void   Node::dotString(string & ss, bool bVerbose) {
		ostringstream  str;
		string s, name;
		unsigned int i;

		if(isPO()) {
			s = "invtriangle, color = red";
		} else if(isConst()) {
			s = "triangle, color = blue";
		} else if(isPI()||  _level == 0) {
			s = "triangle, color = red";
		} else if(!isArithmetic()) {
			s = "box, color = lightgray, style = filled";
		} else if(!isMul()) {
			s = "box";
		} else {
			s = "ellipse";
		}
		str << "\t{\n";
		str << "\t\t rank = same;\n";
		if(isPO()) {
			str << "\t\t LevelPO;\n";
		} else {
			str << "\t\t Level" << _level << ";\n";
		}
		name = getName();
		//Util::strSubstitute(name, "LPAREN", "(");
		//Util::strSubstitute(name, "RPAREN", ")");
		if(isPO()|| isPI()|| _level == 0 || strcmp(func(), "$ted") == 0) {
			// remove prefix const_moins_
			//               const_
			string const_prefix = Environment::getStr("const_prefix");
			if(name.length()> const_prefix.length()&& !name.substr(0,const_prefix.length()).compare(const_prefix)) {
				name.erase(0,const_prefix.length());
				string neg_prefix = Environment::getStr("negative_prefix");
				if(name.length()> neg_prefix.length()&& !name.substr(0,neg_prefix.length()).compare(neg_prefix)) {
					name.erase(0,neg_prefix.length());
					name.insert(0,"-");
				}
			}
			str << "\t\t Node_" << this <<" [label = \"" << name;
		} else  {
			str << "\t\t Node_" << this <<" [label = \"" <<func();
			if (bVerbose)
				str << " " << name;
		}
		if(bVerbose)
			str << " \\[" <<getArrTime()<<"/"<<getReqTime()<<"\\]";
		str <<"\", shape=" << s;
		str <<", fontsize=26 ";
		str << "];\n";
		str << "\t}\n";
		for(i = 0; i < numFanins(); i++) {
			str << "\tNode_" << getFanin(i)<<" -> Node_"<< this << "[style = solid, ";
			if(isSub()&& i == 1) {
				str << "color  = red];\n";
			} else {
				str << "];\n";
			}

		}
		str <<"\n";
		ss = str.str();
	}

	//Node* Node::MergeFanin(unsigned int i) {
	//    TEDNode* pt,* ptkid;
	//    string expr, exprkid, expr1, exprnew;
	//    Node* pFanin = getFanin(i);
	//    Node* pNewNode;
	//
	//
	//    if((ptkid = pFanin->GetTed()) == NULL ||(pt=GetTed()) == NULL) {
	//	return this;
	//    }
	//
	//    expr    = TEDNode::getExpression_rec(pt);
	//    exprkid = TEDNode::getExpression_rec(ptkid);
	//    expr1   = TEDNode::getVarFromIndex(i);
	//
	//    exprnew = Util::strsubstitute(expr, expr1.c_str(), string("("+exprkid+")").c_str());
	//
	//    pNewNode = new Node(Name());
	//    pNewNode->SetType(getType());
	//    pNewNode->_pTNode = new TEDNode(exprnew);
	//
	//    return pNewNode;
	//}


	bool Node::_bAddingConstPrefix = false;
	void Node::startAddingCostantPrefix(void) {
		_bAddingConstPrefix = true;
	}
	void Node::stopAddingCostantPrefix(void) {
		_bAddingConstPrefix = false;
	}

	long Node::getConst(void) {
		assert(isConst());
		if(_data.value!=0) {
			return _data.value;
		} else {
			pair <bool, long> tt;
			string constprefix = Environment::getStr("const_prefix");
			tt = Util::atoi(getName()+ strlen(constprefix.c_str()));
			assert(tt.first);
			return tt.second;
		}
	}

	void Node::setArrTime(int t) {
		if(isPO()|| strcmp(func(), "aging") ==0) {
			_arrTime = t-1;
		} else {
			_arrTime = t;
		}
	}

	Node* Node::duplicate(void) {
		Node* pNode;
		pNode = new Node(_name.c_str(), _type);
		pNode->_func = _func;
		pNode->_pData = _pData;
		pNode->_level = _level;
		pNode->_arrTime = _arrTime;
		pNode->_reqTime = _reqTime;
		return pNode;
	}


	struct _Node_ltDelay {
		bool operator()(Node* n1, Node* n2)const {
			return(n1->getArrTime()< n2 ->getArrTime());
		}
	};

	//void _BalanceCone(Node* pNode, vector <Node*> &vSupp, vector <Node*> &vCone) {
	void Node::balanceSupport(vector <Node*> &vSupp) {
		multiset <Node*, _Node_ltDelay> sNodes;
		multiset <Node*, _Node_ltDelay>::iterator p;
		map <Node*, bool> mNodePositivePhase;
		Node*pFanin,* thisNew,* left,* right;
		bool bPhase, bPhaseLeft, bPhaseRight;
		int max;
		static long counter= 0;
		assert(this->numFanins() == 2);
		for(uint i = 0; i < vSupp.size(); i++) {
			sNodes.insert(vSupp[i]);
			mNodePositivePhase[vSupp[i]] = true;
		}
		if(this->isAdd()|| this->isSub()) {
			this->updateSupportPhase_rec(mNodePositivePhase);
		}
		while(sNodes.size()> 1) {
			p = sNodes.begin();
			left  =*p;
			sNodes.erase(p);
			p = sNodes.begin();
			right =*p;
			sNodes.erase(p);
			string name = "V_"+Util::itoa(counter++);
			if(this->isMul()) {
				thisNew = new Node(name.c_str(), this->getType());
				thisNew->setFunc("*");
				thisNew->addFanin(left);
				thisNew->addFanin(right);
				max = left->getArrTime();
				if(max < right->getArrTime()) {
					max = right->getArrTime();
				}
				sNodes.insert(thisNew);
			} else if(this->isAdd()|| this->isSub()) {
				bPhaseLeft = mNodePositivePhase[left];
				bPhaseRight = mNodePositivePhase[right];
				if(bPhaseLeft && bPhaseRight) {
					thisNew = new Node(name.c_str(), this->getType());
					thisNew->setFunc("+");
					thisNew->addFanin(left);
					thisNew->addFanin(right);
					bPhase = true;
				} else if(bPhaseLeft && !bPhaseRight) {
					thisNew = new Node(name.c_str(), this->getType());
					thisNew->setFunc("-");
					thisNew->addFanin(left);
					thisNew->addFanin(right);
					bPhase = true;
				} else if(!bPhaseLeft && bPhaseRight) {
					thisNew = new Node(name.c_str(), this->getType());
					thisNew->setFunc("-");
					thisNew->addFanin(right);
					thisNew->addFanin(left);
					bPhase = true;
				} else {
					thisNew = new Node(name.c_str(), this->getType());
					thisNew->setFunc("+");
					thisNew->addFanin(left);
					thisNew->addFanin(right);
					bPhase = false;
				}
				max = left->getArrTime();
				if(max < right->getArrTime()) {
					max = right->getArrTime();
				}
				thisNew->setArrTime(max+1);
				sNodes.insert(thisNew);
				mNodePositivePhase[thisNew] = bPhase;
			} else {
				assert(0);
			}
		}
		p = sNodes.begin();
		thisNew =*p;
		sNodes.erase(p);
		/*
		   if(this->isAdd()|| this->isSub()) {
		   assert(thisNew->isAdd()|| this->isSub());
		   bPhase = thisNew->updateConePhase_rec(mNodePositivePhase);
		   assert(bPhase == true);
		   if(bPhase == false) {
		   this = this->Mul(new DfgNode(pMan, -1));
		   }
		   }
		   */
		this->_type = thisNew->_type;
		this->clearFanin();
		unsigned int i;
		max = -1;
		FOREACH_FANIN_OF_NODE(thisNew, i, pFanin) {
			this->addFanin(pFanin);
			if(max < pFanin->getArrTime()) {
				max = pFanin->getArrTime();
			}
		}
		this->setArrTime(max+1);
		//delete this;
		//for(uint i = 0; i < vCone.size(); i++) {
		//    if(vCone[i] == this)continue;
		//    delete vCone[i];
		//}
	}

	void Node::updateSupportPhase_rec(map <Node*, bool> & mPhase, unsigned int nNegs) {
		map <Node*, bool>::iterator p;
		if((p = mPhase.find(this))!= mPhase.end()) {
			if((nNegs%2) == 0) {
				p->second = true;
			} else {
				p->second = false;
			}
			return;
		}
		if(this->isPI())
			return;
		if(this->numFanins()!= 2) {
			cerr << "Error: Node " << this->getName()<< " has " << this->numFanins()<< " fanins." << endl;
		}
		assert(this->numFanins() == 2);
		this->getFanin(0)->updateSupportPhase_rec(mPhase, nNegs);
		if(this->isSub()) {
			this->getFanin(1)->updateSupportPhase_rec(mPhase, nNegs + 1);
		} else {
			this->getFanin(1)->updateSupportPhase_rec(mPhase, nNegs);
		}
	}


	bool _SameMathFunc(Node* n1, Node* n2) {
		if(strncmp(n1->func(), "*", 1) == 0 && strncmp(n2->func(), "*", 1) ==0) {
			return true;
		} else if((strncmp(n1->func(), "+", 1) == 0 || strncmp(n1->func(), "-", 1) == 0)
				  &&(strncmp(n2->func(), "+", 1) == 0 || strncmp(n2->func(), "-", 1) == 0)) {
			return true;
		}
		return false;
	}

	void Node::collectBalanceSupportAndCone(vector <Node*> & vSupp, vector <Node*> & vCone) {
		unsigned int i;
		Node* pFanin;
		FOREACH_FANIN_OF_NODE(this, i, pFanin) {
			if(pFanin->NumRefs() == 1 && _SameMathFunc(this, pFanin)) {
				pFanin->collectBalanceSupportAndCone(vSupp, vCone);
			} else {
				vSupp.push_back(pFanin);
			}
		}
		vCone.push_back(this);
	}

	void Node::balance_rec(void) {
		unsigned int i;
		int arr;
		if(this->visited.isMarked())
			return;
		this->visited.setMark();
		Node* pFanin;
		if(!this->isAdd()&& !this->isSub()&& !this->isMul()) {
			arr = -1;
			FOREACH_FANIN_OF_NODE(this, i, pFanin) {
				pFanin->balance_rec();
				if(arr < pFanin->getArrTime()) {
					arr = pFanin->getArrTime();
				}
			}
			this->setArrTime(arr + 1);
		} else {
			vector <Node*> vSupp;
			vector <Node*> vCone;
			this->collectBalanceSupportAndCone(vSupp, vCone);
			arr = 0;
			for(i = 0; i < vSupp.size(); i++) {
				vSupp[i]->balance_rec();
			}
			//_BalanceCone(n, vSupp, vCone);
			this->balanceSupport(vSupp);
		}
	}

	void Node::updateRequiredTime(int req) {
		assert(req>=0);
		unsigned int i;
		Node* pFanin;
		if(this->getReqTime() ==-1) {
			//uninitialized
			this->setReqTime(req);
		} else if(this->getReqTime()> req) {
			//a tighter constraint was found
			this->setReqTime(req);
		} else if(this->getReqTime() == req) {
			//this path has the same requirement, nothing to do
			//cerr << "Node " << getName()<< " already has req = " << req << " no need to update its path" << endl;
			return;
		}
		//cerr << "Traversing Node " << getName()<< " req = " << req << endl;
		req = this->getReqTime();
		FOREACH_FANIN_OF_NODE(this, i,  pFanin) {
			if(this->isPO()|| strcmp(this->func(), "DFF") == 0) {
				pFanin->updateRequiredTime(req);
			} else {
				int nreq = (req>1) ? req-1 : 0;
				pFanin->updateRequiredTime(nreq);
			}
		}
	}


	bool Node::updateConePhase_rec(map <Node*, bool> & mPhase) {
		map <Node*, bool>::iterator p;
		bool bRet = false;

		if((p = mPhase.find(this))!= mPhase.end()) {
			return p->second;
		}
		bool bLeft  = this->getFanin(0)->updateConePhase_rec(mPhase);
		bool bRight = this->getFanin(1)->updateConePhase_rec(mPhase);

		unsigned int c = 0;
		if(bLeft)c ^= 0x01;
		if(bRight)c ^= 0x02;

		switch(c) {
		case 0x0: //both negative
			bRet = false;
			break;
		case 0x2: //left  negative
			{
				//            SwapKid();  //than follow  case 1
				Node* n1 = this->getFanin(0);
				Node* n2 = this->getFanin(1);
				this->clearFanin();
				this->addFanin(n2);
				this->addFanin(n1);
			}
		case 0x1: //right negative
			this->isSub();
			bRet = true;
			break;
		case 0x3: //both positive
			bRet = true;
			break;
		}
		return bRet;
	}


}

