/*
 * =====================================================================================
 *
 *        Filename:  ETedNode.cc
 *     Description:  ETedNode class
 *         Created:  04/17/2007 03:32:35 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 201                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <list>
#include <bitset>

#include "ETedNode.h"
#include "TedNode.h"
#include "TedContainer.h"
#include "TedKids.h"
#include "TedVar.h"

namespace ted {

#ifndef INLINE
#include "ETedNode.inl"
#endif

	/** @brief Construct a ETed from a variable*/
	ETedNode::ETedNode(const string& var,bool binary): ATedNode() {
		_tVar = TedVarMan::instance().createVar(var,binary);
		_tKids = TedKids::create_kids();
		_tKids->insert(1, new ETedNode(1), 1,TedRegister(0));
		_permanentContainer->registerVar(*(this->getVar()));
	}

	ETedNode::ETedNode(const string& var, const string& range): ATedNode() {
		_tVar = TedVarMan::instance().createVar(var,range);
		_tKids = TedKids::create_kids();
		_tKids->insert(1, new ETedNode(1), 1,TedRegister(0));
		_permanentContainer->registerVar(*(this->getVar()));
	}

	ETedNode::ETedNode(const string& constvar, wedge value): ATedNode() {
		_tVar = TedVarMan::instance().createVar(constvar,value);
		_tKids = TedKids::create_kids();
		_tKids->insert(1, new ETedNode(1), 1,TedRegister(0));
		_permanentContainer->registerVar(*(this->getVar()));
	}

	/*@brief Construct a constant ETed from value*/
	ETedNode::ETedNode(wedge constvalue): ATedNode(constvalue) {
	}

	/** @brief ETed Addition operation*/
	ATedNode* ETedNode::add(ATedNode* etNode) {
		ATedNode* pKid1 = NULL,*pKid2 = NULL;
		if(etNode == NULL) {
			return this;
		}
#if 0
		this->visited.cleanPMark();
		etNode->visited.cleanPMark();
#endif
		if(this == etNode) {
			for(TedNode::dfs_iterator it = this->dfs_begin(); it != this->dfs_end(); it++) {
				if(it->isConst()) {
					it->setConst(it->getConst()*2);
				}
			}
			return this;
		}
		if(isConst()&& etNode->isConst()) {
			return new ETedNode(getConst()+ etNode->getConst());
		}
		if(_permanentContainer->isFirstLower(this, etNode)) {
			return etNode->add(this);
		}
		if(_permanentContainer->isFirstHigher(this, etNode)) {
			pKid1 = (ATedNode*)this->getKidNode(0);
			this ->setKid(0, etNode->add(pKid1), 1);
			return this;
		}
		assert(_permanentContainer->isSameHight(this, etNode));
		unsigned int index = 0;
		//int weight;
		set <unsigned int> sKidVisited;
		FOREACH_KID_OF_NODE(this) {
			index = _iterkids.getIndex();
			pKid1 = _iterkids.Node<ATedNode>();
			pKid2 = (ATedNode*)etNode->getKidNode(index);
			this->replaceKid(index, pKid1->add(pKid2), 1);
			sKidVisited.insert(index);
		}
		FOREACH_KID_OF_NODE(etNode) {
			index = _iterkids.getIndex();
			pKid2 = _iterkids.Node<ATedNode>();
			if(sKidVisited.find(index) == sKidVisited.end())
			{
				this->setKid(index, pKid2, 1);
			}
		}
		return this;
	}

	/** @brief ETed Multiplication operation*/
	ATedNode* ETedNode::mul(ATedNode* etNode) {
		if(etNode == NULL) {
			return NULL;
		}
#if 0
		this->visited.cleanPMark();
		etNode->visited.cleanPMark();
#endif
		if(isConst()|| etNode->isConst()) {
			ATedNode* t1 = NULL,* t2 = NULL;
			if(isConst()) {
				t1 = etNode;
				t2 = this;
			} else if(etNode->isConst()) {
				t1 = this;
				t2 = etNode;
			}
			wedge t2val = t2->getConst();
			for(TedNode::dfs_iterator it = t1->dfs_begin(), end = t1->dfs_end(); it != end; it++) {
				if(it->isConst()) {
					it->setConst(it->getConst()*t2val);
				}
			}
			return t1;
		}
		if(_permanentContainer->isFirstLower(this, etNode)) {
			return etNode->mul(this);
		}
		unsigned int index1 = 0, index2 = 0;
		ATedNode* pKid1 = NULL,* pKid2 = NULL;
		if(_permanentContainer->isFirstHigher(this, etNode)) {
			ATedNode* pDup = NULL;
			FOREACH_KID_OF_NODE(this) {
				pKid1 = _iterkids.Node<ATedNode>();
				index1 = _iterkids.getIndex();
				pDup = etNode->duplicateRecursive();
				this->replaceKid(index1, pKid1->mul(pDup), 1);
			}
			return this;
		}
		assert(_permanentContainer->isSameHight(this, etNode));
		map <unsigned int, ATedNode*> mKids;
		map <unsigned int, ATedNode*>::iterator p;
		list<unsigned int> index2erase;
		if((this->getKids())!=NULL) {
			if(this->getVar()->isBinary()) {
				// (pkid1 + node*pkid3)*(pkid2 + node*pkid4)
				// pkid1*pkid2 + node*(pkid3*pkid2 + pkid4*pkid1 + pkid3*pkid4)
				assert(getKids()->size()<= 2);
				assert(etNode->getKids()->size()<= 2);
				//
				// 0-edge
				//
				pKid1 = dynamic_cast<ATedNode*>(etNode->getKids()->getNode(0));
				pKid2 = dynamic_cast<ATedNode*>(getKids()->getNode(0));
				if(pKid1 && pKid2) {
					mKids[0] = pKid1->duplicateRecursive()->mul(pKid2->duplicateRecursive());
				} else {
					index2erase.push_back(0);
				}
				//
				// 1-edge
				//
				ATedNode* pKid3 = dynamic_cast<ATedNode*>(etNode->getKids()->getNode(1));
				ATedNode* pKid4 = dynamic_cast<ATedNode*>(getKids()->getNode(1));
				ATedNode* t = NULL;
				if(pKid3) {
					if(pKid4) {
						t = pKid3->duplicateRecursive()->mul(pKid4->duplicateRecursive());
						if(pKid2) {
							ATedNode* tx = pKid3->duplicateRecursive()->mul(pKid2->duplicateRecursive());
							t = t->add(tx);
						}
						if(pKid1) {
							ATedNode* tx = pKid4->duplicateRecursive()->mul(pKid1->duplicateRecursive());
							t = t->add(tx);
						}
					} else if(pKid2) {
						t = pKid3->duplicateRecursive()->mul(pKid2->duplicateRecursive());
					}
					mKids[1] = t;
				} else if(pKid1 && pKid4) {
					t = pKid4->duplicateRecursive()->mul(pKid1->duplicateRecursive());
					mKids[1] = t;
				} else {
					assert(false);
				}
			} else {
				for(TedKids::iterator _iterkids1 = getKids()->begin(), _iterend1 = getKids()->end(); \
					(_iterkids1 != _iterend1); _iterkids1++) {
					index1 = _iterkids1.getIndex();
					pKid1 = _iterkids1.Node<ATedNode>();
					if((etNode->getKids())!=NULL) {
						for(TedKids::iterator _iterkids2 = etNode->getKids()->begin(), \
							_iterend2 = etNode->getKids()->end(); \
							(_iterkids2 != _iterend2); _iterkids2++) {
							index2 = _iterkids2.getIndex();
							pKid2 = _iterkids2.Node<ATedNode>();
							ATedNode* t = pKid1->duplicateRecursive()->mul(pKid2->duplicateRecursive());
							if((p = mKids.find(index1+index2))!= mKids.end()) {
								p->second = p->second->add(t);
							} else {
								mKids[index1+index2] = t;
							}
						}
					}
					index2erase.push_back(index1);
				}
			}
		}
		while(!index2erase.empty()) {
			this->removeKid(index2erase.back());
			index2erase.pop_back();
		}
		for(p = mKids.begin(); p != mKids.end(); p++) {
			this->setKid(p->first, p->second, 1);
		}
		return this;
	}

	/** @brief ETed subtraction operation*/
	ATedNode* ETedNode::sub(ATedNode* etNode) {
		if(etNode == NULL)
			return this;
		ATedNode* t = new ETedNode(-1);
		ATedNode* pENode = this->add(etNode->mul(t));
		return pENode;
	}

	/** @brief ETed power operation*/
	ATedNode* ETedNode::exp(ATedNode* etNode) {
		ATedNode* t= new ETedNode(1);
		if(etNode->isConst()) {
			if(etNode->getConst()>= 0) {
				for(int i = 0; i < etNode->getConst(); i++) {
					ETedNode* op1 = (ETedNode*)t->duplicateRecursive();
					ETedNode* op2 = (ETedNode*)this->duplicateRecursive();
					t = op1->mul(op2);
				}
			} else {
				for(int i = 0; i < etNode->getConst(); i++) {
					ETedNode* op1 = (ETedNode*)t->duplicateRecursive();
					ETedNode* op2 = (ETedNode*)this->duplicateRecursive();
					t = op1->div(op2);
				}
			}
		} else {
			throw(string("04005. Can't build TED with variable \"")+ string(etNode->getName())+ string("\" as exponent"));
		}
		return t;
	}

	// Resulting node have not extracted GCD nor checked into the container
	//
	// NOTE: adding nodes could have only an additive edge with no solid edge
	//
	/** @brief ETed Addition operation*/
	ATedNode* ETedNode::add_with_content(TedNode* etNode, TedRegister reg) {
		if(etNode == NULL) {
			assert(reg==0);
			return this;
		}
		// the only constant that should exist on a TED is ONE
		assert(this->isConst() ?(this==TedNode::getOne()): true);
		assert(etNode->isConst() ?(etNode==TedNode::getOne()): true);
		if(_permanentContainer->isFirstLower(this, etNode)) {
			assert(reg==TedRegister(0));
			return((ETedNode*)etNode)->add_with_content(this,reg);
		}
		if(_permanentContainer->isFirstHigher(this, etNode)) {
			ETedNode* sum = (ETedNode*)this->clone();
			sum->setKids(this->getKids()->clone());
			TedNode* pKid1 = sum->getKidNode(0);
			if(pKid1) {
				//no register allowed on edges
				TedRegister sum_kidreg0 = sum->getKids()->getRegister(0);
				ETedNode* intermidiate = NULL;
				if(sum->getKidWeight(0)!=1) {
					TedKids newKids = pKid1->getKids()->duplicate();
					newKids.pushGCD(sum->getKidWeight(0));
					bool tmp_existed = _permanentContainer->hasNode(*pKid1->getVar(),newKids,pKid1->getRegister());
					ETedNode* tmp = (ETedNode*)_permanentContainer->getNode(*pKid1->getVar(),newKids,pKid1->getRegister());
					intermidiate = (ETedNode*)((ETedNode*)etNode)->add_with_content(tmp,sum_kidreg0);
					if(!tmp_existed && intermidiate->isErasable(tmp)) {
						_permanentContainer->eraseNode(tmp);
						delete tmp;
					}
				} else {
					intermidiate = (ETedNode*)((ETedNode*)etNode)->add_with_content(pKid1,sum_kidreg0);
				}
				assert(intermidiate);
				wedge gcd = intermidiate->getKids()->extractGCD();
				ETedNode* check = (ETedNode*)_permanentContainer->getNode(*intermidiate->getVar(),*intermidiate->getKids(),intermidiate->getRegister());
				if(check != intermidiate) {
					delete intermidiate;
					intermidiate = check;
				}
				sum->setKid(0, intermidiate, gcd, reg);
			} else {
				wedge gcd = 1;
				ETedNode* check = (ETedNode*)etNode;
				if(etNode!=TedNode::getOne()) {
					TedKids newkids = etNode->getKids()->duplicate();
					gcd = newkids.extractGCD();
					if(gcd!=1) {
						check = (ETedNode*)_permanentContainer->getNode(*etNode->getVar(),newkids,etNode->getRegister());
					}
				}
				sum->setKid(0,check,gcd,reg);
			}
			return sum;
		}
		assert(_permanentContainer->isSameHight(this, etNode));
		assert(reg==0);
		unsigned int index = 0;
		set <unsigned int> sKidVisited;
		ETedNode* result = (ETedNode*)this->clone();
		result->_tKids = TedKids::create_kids();
		//ADD all edges of this with edges of etNode
		FOREACH_KID_OF_NODE(this) {
			index = _iterkids.getIndex();
			TedNode* pKid1 = _iterkids.Node<TedNode>();
			// pKid1 MUST exist
			assert(pKid1);
			TedNode* pKid2 = etNode->getKidNode(index);
			TedNode* final = NULL;
			wedge pkid1_weight = _iterkids.getWeight();
			wedge newWeight = 1;
			TedRegister regfinal = _iterkids.getRegister();
			if(!pKid2) {
				//pKid2 does not exist
				final = pKid1;
				newWeight = pkid1_weight;
			} else {
				//pKid2 exists
				wedge pkid2_weight = etNode->getKidWeight(index);
				assert(etNode->getKids()->getRegister(index) ==regfinal);
				ETedNode* intermidiate = NULL;
				if(pKid1 == TedNode::getOne()) {
					//pKid1 is ONE
					// registers in edges connecting to ONE can be reduced to TedRegister(0)
					if(pKid2 == TedNode::getOne()) {
						//pKid2 is ONE
						final = pKid1;
						newWeight = pkid1_weight+pkid2_weight;
					} else {
						//pKid2 exists and it is different than ONE
						if(pkid2_weight != 1) {
							intermidiate = (ETedNode*)((ETedNode*)pKid2)->clone();
							TedKids* newkids =  pKid2->getKids()->clone();
							newkids->pushGCD(pkid2_weight);
							intermidiate->setKids(newkids);
						} else {
							intermidiate = (ETedNode*)pKid2;
						}
						intermidiate = (ETedNode*)intermidiate->add_with_content(pkid1_weight);
						newWeight = final->getKids()->extractGCD();
						final = _permanentContainer->getNode(*intermidiate->getVar(),*intermidiate->getKids(),intermidiate->getRegister());
						if(final != intermidiate) {
							delete intermidiate;
						}
					}
				} else {
					//pKid1 exists and it is different than ONE
					if(pkid1_weight!=1) {
						intermidiate = (ETedNode*)((ETedNode*)pKid1)->clone();
						TedKids* newkids =  pKid1->getKids()->clone();
						newkids->pushGCD(pkid1_weight);
						intermidiate->setKids(newkids);
					} else {
						intermidiate = (ETedNode*)(pKid1);
					}
					if(pKid2 == TedNode::getOne()) {
						// registers in edges connecting to ONE can be reduced to TedRegister(0)
						ETedNode* tmp = (ETedNode*)(intermidiate->add_with_content(pkid2_weight));
						if(intermidiate!=pKid1 && tmp->isErasable(intermidiate)) {
							_permanentContainer->eraseNode(intermidiate);
							delete intermidiate;
						}
						intermidiate = tmp;
					} else {
						//pKid2 exists and it is different than ONE
						ETedNode* intermidiate2 = NULL;
						if(pkid2_weight!=1) {
							TedKids newkids =pKid2->getKids()->duplicate();
							newkids.pushGCD(pkid2_weight);
							intermidiate2 = (ETedNode*)_permanentContainer->getNode(*pKid2->getVar(),newkids,pKid2->getRegister());
						} else {
							intermidiate2 = (ETedNode*)pKid2;
						}
						ETedNode* tmp = (ETedNode*)(intermidiate->add_with_content(intermidiate2,TedRegister(0)));
						if(intermidiate!=pKid1 && tmp->isErasable(intermidiate)) {
							_permanentContainer->eraseNode(intermidiate);
							delete intermidiate;
						}
						if(intermidiate2!=pKid2 && tmp->isErasable(intermidiate2)) {
							_permanentContainer->eraseNode(intermidiate2);
							delete intermidiate2;
						}
						intermidiate = tmp;
					}
					newWeight = intermidiate->getKids()->extractGCD();
					final = _permanentContainer->getNode(*intermidiate->getVar(),*intermidiate->getKids(),intermidiate->getRegister());
					if(final != intermidiate) {
						delete intermidiate;
					}
				}
			}
			assert(final);
			result->addKid(index, final, newWeight,regfinal);
			sKidVisited.insert(index);
		}
		//ADD the remaining edges of etNode
		FOREACH_KID_OF_NODE(etNode) {
			index = _iterkids.getIndex();
			TedNode* pKid2 = _iterkids.Node<TedNode>();
			if(sKidVisited.find(index) == sKidVisited.end()) {
				result->addKid(index, pKid2, _iterkids.getWeight(), _iterkids.getRegister());
			}
		}
		return result;
	}

	bool ETedNode::isErasable(TedNode* ptr) {
		if(!ptr || this==ptr) {
			return false;
		}
		list<TedNode*> sac;
		sac.push_back(this);
		while(!sac.empty()) {
			TedNode* p = sac.front();
			sac.pop_front();
			FOREACH_KID_OF_NODE(p) {
				TedNode* pkid = _iterkids.Node<TedNode>();
				if(pkid==ptr) {
					return false;
				}
				if (pkid!=TedNode::getOne()) {
					sac.push_back(pkid);
				}
			}
		}
		return true;
	}

	ATedNode* ETedNode::add_with_content(wedge constvalue) {
#if 1
		// grab the variable and create a additive edge to TedNode::one
		ETedNode* dummyNode = (ETedNode*)(this->clone());
		dummyNode->setKids(TedKids::create_kids());
		dummyNode->setKid(0,TedNode::getOne(),constvalue,0);
		ATedNode* result = this->add_with_content(dummyNode,0);
		return result;
#else
		// This was an optimization made, but the code was never finished
		// for instance the following example results in an incorrect
		// result
		//
		// [1] + (x1*2+1)*4  == x1*2*x4+(x4+1)
		//
		// but results in     (x1*2+1)*(x4+1)
		//
		// there is no node replication
		//        
		//NOTE: we collect the adder_chain being traversed while
		//      push down the weight in the additive edges, until either:
		//      1) there is no additive edge OR
		//      2) the additive edge connects to TedNode::getOne()
		//
		list<TedNode*> adder_chain;
		ETedNode* intermidiate = (ETedNode*)this;
		//bool duplicated = false;
		while(intermidiate->hasKid(0)&& intermidiate->getKidNode(0)!=TedNode::getOne()) {
			ETedNode* inter_clone = (ETedNode*)intermidiate->clone();
			inter_clone->_tKids = TedKids::create_kids();
			inter_clone->_tKids = intermidiate->_tKids->clone();
			inter_clone->removeKid(0);
			TedRegister reg_count = intermidiate->getKids()->getRegister(0);
			wedge weight = intermidiate->getKids()->getWeight(0);
			TedKids newkids =intermidiate->getKidNode(0)->getKids()->duplicate();
			newkids.pushGCD(weight);
			intermidiate = (ETedNode*)_permanentContainer->getNode(*intermidiate->getKidNode(0)->getVar(),newkids,intermidiate->getKidNode(0)->getRegister());
			assert(intermidiate);
			inter_clone->addKid(0,intermidiate,1,reg_count);
			adder_chain.push_back(inter_clone);
			//duplicated = true;
		}
		//NOTE: at the end of the adder chain we perform the addition
		//      inter_clone  is the chain of additions with the additive weight pushed down
		//      intermidiate is the node following the end of the adder chain.
		assert(intermidiate!=NULL && intermidiate!=TedNode::getOne());
		// make sure we are not tinkering with an existing node in the container
//		if(!duplicated) {
		TedKids* newkids = intermidiate->getKids()->clone();
		intermidiate = (ETedNode*)intermidiate->clone();
		intermidiate->setKids(newkids);
//		}
		// add the constvalue to the intermidiate node
		if(intermidiate->hasKid(0)) {
			assert(intermidiate->getKidNode(0) ==TedNode::getOne());
			intermidiate->setKidWeight(0,intermidiate->getKidWeight(0)+constvalue);
		} else {
			intermidiate->addKid(0,TedNode::getOne(),constvalue,TedRegister(0));
		}
		wedge weight = intermidiate->getKids()->extractGCD();
		ETedNode* back_track = (ETedNode*)_permanentContainer->getNode(*intermidiate->getVar(),*intermidiate->getKids(),intermidiate->getRegister());
		if(back_track != intermidiate) {
			delete intermidiate;
			intermidiate = NULL;
		}
		//NOTE: we update the GCD of the adder_chain and extract any new GCD
		while(!adder_chain.empty()) {
			ETedNode* inter_clone = (ETedNode*)adder_chain.back();
			adder_chain.pop_back();
			if(weight!=1) {
				for(TedKids::iterator iter1 = inter_clone->getKids()->begin(), iter1end = inter_clone->getKids()->end(); iter1 != iter1end; iter1++) {
					TedNode* pKid   = iter1.Node<TedNode>();
					if(pKid==back_track) {
						(*iter1).setWeight((*iter1).getWeight()*weight);
						(*iter1).setNode(back_track);
					}
				}
			}
			weight = inter_clone->getKids()->extractGCD();
			intermidiate = inter_clone;
			back_track = (ETedNode*)_permanentContainer->getNode(*intermidiate->getVar(),*intermidiate->getKids(),intermidiate->getRegister());
			if(back_track != intermidiate) {
				delete intermidiate;
				intermidiate = NULL;
			}
		}
		return back_track;
#endif
	}

	ATedNode* ETedNode::reg(ATedNode* atNode) {
		if(atNode == NULL || isConst()) {
			return this;
		}
		if(!atNode->isConst()) {
			throw(string("04004. attempting to assign a variable number of registers"));
		}
		TedRegister regVal = atNode->getConst();
		if(regVal==0) {
			return this;
		}
		ATedNode* ret = BuildRecursive(this,regVal);
		return ret;
	}

}
