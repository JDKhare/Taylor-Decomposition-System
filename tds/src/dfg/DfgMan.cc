/*
 * =====================================================================================
 *
 *       Filename:  DfgMan.c
 *    Description:  DfgMan class
 *        Created:  05/01/2007 04:30:18 AM EDT
 *         Author:  Daniel Gomez-Prado(from jan 2008), Qian Ren(up to dec 2007)
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
#include <iostream>
#include <sstream>
#include <fstream>
#include <climits>
#include <cmath>
using namespace std;

#include "TedOrderCost.h"
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
#include "DfgShifter.h"
#include "DfgOperator.h"

#include "util.h"
#include "csd.h"
using util::Util;
using util::Csd;

#include "Environment.h"
using tds::Environment;

namespace dfg {
#ifndef INLINE
#include "DfgMan.inl"
#endif

#ifndef UINT_MAX
# define UINT_MAX 0xFFFF
#endif

	DfgMan::~DfgMan(void) {
		vector <DfgNode*> vNodes;
		for(set <DfgNode*>::iterator p = _sNodes.begin(); p != _sNodes.end(); p++) {
			vNodes.push_back(*p);
		}
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			delete vNodes[i];
		}
		_sNodes.clear();
		_showRegisters = false;
	}

	/** @brief DfgMan::cleanUp Properly cleans the set of DFG nodes*/
	void DfgMan::cleanUp(void) {
		vector<DfgNode*> vNodes;
		//TOFIX: Definitely need efficiency improvement to not involve copy of set and delete with key value //
		set<DfgNode*> sCopy;
		sCopy = _sNodes;
		collectDFS(vNodes);
		for(set <DfgNode*>::iterator p = sCopy.begin(); p != sCopy.end(); p++) {
			if(!(*p)->_visited.isMarked()) {
				_sNodes.erase(*p);
				delete*p;
			}
		}
	}

	unsigned int DfgMan::collectDFS(vector <DfgNode*> & vNodes) {
		unsigned int l = 0;
		vNodes.clear();
		Mark<DfgNode>::newMark();
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp!= _mPos.end(); mp++) {
			l = MAX(l, mp->second->collectDFS(vNodes));
		}
		return l;
	}

	void DfgMan::collectDFSonly(vector <DfgNode*> & vNodes) {
		vNodes.clear();
		Mark<DfgNode>::newMark();
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp!= _mPos.end(); mp++) {
			mp->second->collectDFSonly(vNodes);
		}
	}

	void DfgMan::collectParents(DfgParents& container) {
		vector<DfgNode*> vnodes;
		collectParents(container,vnodes);
	}

	void DfgMan::collectParents(DfgParents& container, vector<DfgNode*>& vnodes) {
		container.clear();
		vnodes.clear();
		collectDFSonly(vnodes);
		for(vector<DfgNode*>::iterator it = vnodes.begin(); it != vnodes.end(); it++) {
			DfgNode* node =*it;
			DfgNode* left = node->getLeft();
			if(container.find(left) ==container.end()) {
				vector<DfgNode*> start;
				start.push_back(node);
				container[left] = start;
			} else {
				container[left].push_back(node);
			}
			DfgNode* right = node->getRight();
			if(container.find(right) ==container.end()) {
				vector<DfgNode*> start;
				start.push_back(node);
				container[right] = start;
			} else {
				container[right].push_back(node);
			}
		}
	}

	void DfgMan::hierarchy(multimap<int,pair<string,int> >& h) {
		h.clear();
		DfgParents parents;
		collectParents(parents);
		Mark<DfgNode>::newMark();
		map< DfgNode*,pair<int,int>  > level;
		for(PrimaryOutputs::iterator it = _mPos.begin(), end=_mPos.end(); it != end; it++) {
			string poname = it->first;
			DfgNode* poNode = it->second;
			hierarchy(level, poNode, parents);
		}
		for(PrimaryOutputs::iterator it = _mPos.begin(), end=_mPos.end(); it != end; it++) {
			string poname = it->first;
			DfgNode* poNode = it->second;
			pair<string,int> pe(poname,level[poNode].second);
			h.insert(pair<int,pair<string,int> >(level[poNode].first,pe));
		}
	}
	void DfgMan::info_hierarchy(void) {
		multimap<int,pair<string,int> > h;
		hierarchy(h);
		for (multimap<int,pair<string,int> >::iterator jt = h.begin(), jtend=h.end(); jt!=jtend; jt++) {
			cout << "Level=" << jt->first << " PO-name=" << jt->second.first << " Referenced-by=" << jt->second.second << endl;
		}
		cout << "----------------"<<endl;
		cout << "Note: POs with Referenced-by equal to 0, are never" << endl;
		cout << "      re-used internally and are \"true\" outputs." << endl;
	}

	int DfgMan::hierarchy(map<DfgNode*,pair<int,int> >& level, DfgNode* pNode, DfgParents& parents) {
		assert(pNode);
		if (!pNode->isMarked()) {
			pNode->setMark();
			level[pNode] = pair<int,int>(0,parents[pNode].size());
			switch (pNode->getOp()) {
			case DfgOperator::VAR:
			case DfgOperator::VARCONST:
				{
					PrimaryOutputs::iterator it = _mPos.find(pNode->getName());
					if (it!=_mPos.end()) {
						DfgNode* pNext = it->second;
						level[pNode].first = hierarchy(level,pNext, parents)+1;
						level[pNext].second = parents[pNode].size();
					}
					break;
				}
			case DfgOperator::CONST:
				{
					return 0;
				}
			default:
				{
					int left = hierarchy(level,pNode->getLeft(), parents);
					int right = hierarchy(level,pNode->getRight(), parents);
					level[pNode].first = Util::max(left,right)+1;
					break;
				}
			}
		}
		return level[pNode].first;
	}

	/** @brief computes the bitwidth required for exact computation in the entire DFG graph**/
	void DfgMan::computeBitwidth(bool(DfgNode::*Functor)(unsigned int), unsigned int level) {
		Mark<DfgNode>::newMark();
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp!= _mPos.end(); mp++) {
			mp->second->computeBitwidth(Functor,level);
		}
	}

	/** @brief computes the bitwidth required for exact computation in the entire DFG graph**/
	void DfgMan::computeBitwidth(void) {
		return computeBitwidth(NULL,0);
	}

	void DfgMan::computeSNR(void) {
		Mark<DfgNode>::newMark();
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp!= _mPos.end(); mp++) {
			mp->second->computeSNR();
		}
	}

	void DfgMan::getInfoSNR(void) {
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp!= _mPos.end(); mp++) {
			cout << mp->first << " -> ";
			flush(cout);
			cout << mp->second->_op->getError()->getE()<< endl;
		}
	}

#if 0
	void DfgMan::optimize(double maxerror) {
		//just a proof of concept, no cleverness at all.
		//Goes left, right - top,bottom, decrementing the bitwidth by 1.
		vector <DfgNode* > vNodes;
		int latency = collectDFS(vNodes);
		map<int, set<DfgNode* >* > wave;
		for(int j=0; j <= latency; j++) {
			wave[j] = new set<DfgNode*>;
		}
		for(unsigned int k = 0; k < vNodes.size(); k++) {
			DfgNode* node = vNodes[k];
			wave[node->_level]->insert(node);
		}
		DfgNode* lastnode;
		MinGappaBound gappa;
		OrderCost cost;
		for(unsigned int index = 1; index <= latency; index++) {
			set<DfgNode*>* tochange = wave[index];
			set<DfgNode*> avoid;
			double error = 0.0;
			bool forceFinish = false;
			while((error < maxerror)&& !forceFinish) {
				int i = 1;
				for(set<DfgNode*>::iterator it = tochange->begin(); it != tochange->end(); it++,i++) {
					cout << "[" << index << "/" << latency << "]";
					cout << " " << i << "/" << tochange->size();
					lastnode = (*it);
					if(avoid.find(*it) == avoid.end()) {
						if(lastnode->_op->getBitwidth()->isReducible()) {
							lastnode->_op->getBitwidth()->reduce();
							computeBitwidth(&DfgNode::uptoLevel,index);
							error  = gappa(*this).bitwidth;
							cout << " error=" << error;
							cout << " op=" << lastnode->getOpStr();
							cout << " id=" << lastnode->_op->getID();
							cout << " bitwidth=" << lastnode->_op->getBitwidth()->at();
							if(error > maxerror) {
								lastnode->_op->getBitwidth()->increment();
								computeBitwidth(&DfgNode::uptoLevel,index);
								error = gappa(*this).bitwidth;
								avoid.insert(lastnode);
								cout << " BACKTRACKING prev error=" << error << endl;
							}
						} else {
							avoid.insert(lastnode);
						}
					}
					flush(cout);
					cout << '\xd';
				}
				forceFinish = (avoid.size() == tochange->size());
			}
		}
		for(unsigned int index = 1; index <= latency; index++) {
			delete wave[index];
		}
	}
#endif

	/**
	 * @brief DfgMan::showDot generates the DFG dot file
	 *
	 * @param bUseArrivalTime
	 * @param bVerbose
	 **/
	void DfgMan::writeDot(ofstream& ofile, bool bUseArrivalTime, bool bVerbose, bool show_ids) {
		unsigned int nMul = 0;
		unsigned int nAdd = 0;
		vector <DfgNode* > vNodes;
		ofile << "Digraph G {\n\tsize=\"10, 7.5\"; center = \"true\"; \n";
		ofile << "\t{\n"
			<< "\t\tnode [shape = plaintext];\n"
			<< "\t\tedge [style = invis];\n"
			<< "\t\tLevelPO [label=\"\"];\n";
		unsigned int maxLevel = collectDFS(vNodes);
		if(bUseArrivalTime) {
			maxLevel = maxArrivalTime();
		}
		for(unsigned int i = 0; i <= maxLevel; i++) {
			ofile << "\t\tLevel" << i << " [label=\"";
			if(Environment::getBool("show_level"))
				ofile << i;
			ofile << "\"];\n";
		}
		ofile << "\t\tLevelComments [label=\"\"];\n";
		for(unsigned int i = 0; i <= maxLevel; i++) {
			ofile << "Level" << i << " -> ";
		}
		ofile << "\t\tLevelPO";
		ofile << "-> LevelComments";
		ofile << "\n\t}\n";
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			ofile << "\t{rank = same; LevelPO; Node_" << p->first;
			ofile << " [label=\""<<p->first<<"\" shape=triangle, fontsize=26]; }\n";
			ofile << "\tNode_"<<p->second << " -> Node_" <<p->first << "\n";
		}
		for(unsigned int i = 0; i < vNodes.size(); i ++) {
			if(bUseArrivalTime) {
				ofile << dotString(vNodes[i], vNodes[i]->getArrivalTime(), bVerbose, show_ids);
			} else {
				ofile << dotString(vNodes[i], vNodes[i]->getLevel(), bVerbose, show_ids);
			}
			if(vNodes[i]->isMul())nMul++;
			if(vNodes[i]->isAdd()|| vNodes[i]->isSub())nAdd++;
		}
		ofile << "\t{rank=same;LevelComments;text [shape=plaintext,font=40, label=\"";
		ofile << "ADD(s) = " << nAdd << " MPY(s) = " << nMul <<" Latency = "<< maxLevel;
		ofile << "\"]}\n";
		ofile << "}";
	}

	const char* DfgMan::getNameIfRegisters(DfgNode* pNode) {
		if (_showRegisters) {
			TedVar* pvar = pNode->getTedVarIfExist();
//			if(pvar && pvar->isRetimed())
	//			return pvar->getBase()->getName().c_str();
			if (pvar)
				return pvar->getRootName().c_str();
			else
				return pNode->getName();
		} else {
			return pNode->getName();
		}
	}


	string DfgMan::dotString(DfgNode* pNode, unsigned int level, bool bVerbose, bool show_ids) {
		ostringstream oss;
		if(pNode->_op && pNode->_op->getBitwidth()) {
			oss << "\n\tsubgraph bitw_" << pNode << "{";
			oss << "\n\trank = same; Level" << level <<"; node_bitw_" << pNode << "[label=\"";
			oss << "b=" << pNode->_op->getBitwidth()->at();
			oss << "\", shape = plaintext]";
			oss << "\n\tNode_" << pNode << " -> node_bitw_" << pNode << "[arrowhead=inv, minlen=0]";
		}
		oss << "\n\t{rank = same; Level"<<level<<"; Node_"<<pNode<<" [label=\"";
		switch(pNode->getOp()) {
		case DfgOperator::CONST:
			oss << pNode->getConst();
			break;
		case DfgOperator::VAR:
			{
				oss << pNode->getName() << " ";
				//oss << getNameIfRegisters(pNode);
				break;
			}
		case DfgOperator::VARCONST:
			{
				TedVar* pvar = pNode->getTedVarIfExist();
				if(pvar && pvar->hasRange())
					oss << pvar->getRange();
				else
					oss << pNode->getName();
				break;
			}
		default:
			{
				oss << pNode->getOpStr();
			}
		}
		if (show_ids) {
			oss << "[" << pNode->getOperator()->getID() << "]";
		}
		if(bVerbose) {
			oss << " \\["<<pNode->getArrivalTime()<<"/" <<pNode->getRequiredTime()<<"\\]";
		}
		if(pNode->isOp()) {
			if(pNode->isAddorSub()) {
				oss <<"\" shape= box fontsize= 26]; }\n";
			} else if(pNode->isReg() || pNode->isAssign()) {
				oss << "\" shape=box height=0.25 fontsize=18]; }\n";
			} else {
				oss <<"\" shape=ellipse fontsize= 26]; }\n";
			}
		} else  {
			oss <<"\" shape=invtriangle fontsize= 26]; }\n";
		}
		if(pNode->_op && pNode->_op->getBitwidth()) {
			oss << "\n\t}";
		}
		if(pNode->isOp()) {
			oss <<"\tNode_"<<pNode->getLeft()<<" -> Node_"<<pNode;
			oss <<" [style=solid];\n";
			if(!pNode->isReg() && !pNode->isAssign()) {
				oss <<"\tNode_"<<pNode->getRight()<<" -> Node_"<<pNode;
				oss <<" [style="<<((pNode->isSub()||pNode->isLSH()) ?"dashed":"solid")<<"];\n";
			}
		}
		return oss.str();
	}

	void DfgMan::writeDotDetailed(void) {
		static unsigned int index = 0;
		ofstream ofile;
		string filename = Environment::getStr("show_directory")+ "/debugDfg" + Util::itoa(index++)+ ".dot";
		ofile.open(filename.c_str());
		if(!ofile.is_open()) {
			throw(string("01012. Cannot open temporary file for writing.\n"));
		}
		writeDotDetailed(ofile);
		ofile.close();
		Util::showDotFile(filename.c_str());
	}

	void DfgMan::writeDotDetailed(ofstream& ofile) {
		unsigned int nMul = 0;
		unsigned int nAdd = 0;
		vector <DfgNode* > vNodes;
		ofile << "Digraph G {\n\tsize=\"10, 7.5\"; center = \"true\"; \n";
		ofile << "\t{\n"
			<< "\t\tnode [shape = plaintext];\n"
			<< "\t\tedge [style = invis];\n"
			<< "\t\tLevelPO [label=\"\"];\n";
		//collectDFSonly(vNodes);
		int maxLevel = maxArrivalTime();
		for(int i = maxLevel; i >= 0; i--) {
			ofile << "\t\tLevel" << i << " [label=\""<< i <<"\"];\n";
		}
		ofile << "\t\tLevelComments [label=\"\"];\n";
		for(int i = 0; i <= maxLevel; i++) {
			ofile << "Level" << i << " -> ";
		}
		ofile << "\t\tLevelPO";
		ofile << "-> LevelComments";
		ofile << "\n\t}\n";
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			ofile << "\t{rank = same; LevelPO; Node_" << p->first;
			ofile << " [label=\""<<p->first<<"\" shape=triangle, fontsize=26]; }\n";
			ofile << "\tNode_"<<p->second << " -> Node_" <<p->first << "\n";
		}
		for(set<DfgNode*>::const_iterator it = _sNodes.begin(), end = _sNodes.end(); it != end; it++) {
			//	if((*it)->_visited.isMarked()) {
			ofile << dotStringDetailed(*it);
			if((*it)->isMul())nMul++;
			if((*it)->isAdd()||(*it)->isSub())nAdd++;
			//	}
		}
		ofile << "\t{rank=same;LevelComments;text [shape=plaintext,font=40, label=\"";
		ofile << "ADD(s) = " << nAdd << " MPY(s) = " << nMul <<" Latency = "<< maxLevel;
		ofile << "\"]}\n";
		ofile << "}";
	}

	string DfgMan::dotStringDetailed(DfgNode* pNode) {
		ostringstream oss;
		oss << "\n\t{Node_"<<pNode<<" [label=\"";
		switch(pNode->getOp()) {
		case DfgOperator::CONST:
			oss << pNode->getConst();
			break;
		case DfgOperator::VAR:
			oss << pNode->getName();
			break;
		case DfgOperator::VARCONST:
			{
				TedVar* pvar = pNode->getTedVarIfExist();
				if(pvar && pvar->hasRange())
					oss << pvar->getRange();
				else
					oss << pNode->getName();
				break;
			}
		case DfgOperator::REG:
		case DfgOperator::ADD:
		case DfgOperator::SUB:
		case DfgOperator::MUL:
		case DfgOperator::DIV:
		case DfgOperator::LSH:
		case DfgOperator::RSH:
		case DfgOperator::EQ:
			oss << pNode->getOpStr();
			if(pNode->_data.ptr)
				oss << "(ptr=" << pNode->_data.ptr << ")";
			oss << " [" << pNode->getOperator()->getID() << "]";
			break;
		default:
			throw(string("01022. UNKOWN DFG node type."));
			break;
		}
		oss << " \\n" << pNode;
		oss << " \\n\\["<<pNode->getArrivalTime()<<"/" <<pNode->getRequiredTime()<<"\\]";
		oss << " \\n l=" << pNode->_level << " v=" << pNode->_value;
		oss <<(pNode->_visited.isMarked() ?" M":" U")<< pNode->_visited.getMark();
		//oss << " \\n b=" <<(((pNode->_op)&& pNode->_op->getBitwidth()) ? pNode->_op->getBitwidth()->at(): "X");
		oss << " \\n b=" <<((pNode->_op && pNode->_op->getBitwidth()) ? pNode->_op->getBitwidth()->at(): "X");
		if(pNode->isOp()) {
			if(pNode->isAddorSub()) {
				oss <<"\" shape=box fontsize= 26]; }\n";
			} else if(pNode->isReg()) {
				oss << "\" shape=box height=0.25 fontsize=18]; }\n";
			} else {
				oss <<"\" shape= ellipse fontsize= 26]; }\n";
			}
		} else  {
			oss <<"\" shape=invtriangle fontsize= 26]; }\n";
		}
		if(pNode->isOp()) {
			oss <<"\tNode_"<<pNode->getRight()<<" -> Node_"<<pNode;
			oss <<" [style="<<((pNode->isSub()||pNode->isLSH()) ?"dashed":"solid")<<", label=\"R\"];\n";
			if(!pNode->isReg()) {
				oss <<"\tNode_"<<pNode->getLeft()<<" -> Node_"<<pNode;
				oss <<" [style=solid, label=\"L\"];\n";
			}
		}
		return oss.str();
	}

	/**
	 * @brief DfgMan::getNumOperators get the number of operators
	 *
	 * @param t
	 **/
	void DfgMan::getNumOperators(DfgStat & t) {
		vector <DfgNode* > vNodes;
		DfgNode*pNode;
		strash();
		t.latency = collectDFS(vNodes);
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			pNode = vNodes[i];
			if(pNode->isMul()) {
				if(pNode->getLeft()->isConst()|| pNode->getRight()->isConst()) {
					t.nMulConst ++;
				} else {
					t.nMul++;
				}
			} else if(pNode->isAdd()) {
				if(pNode->getLeft()->isConst()|| pNode->getRight()->isConst()) {
					t.nAddConst ++;
				} else {
					t.nAdd++;
				}
			} else if(pNode->isSub()) {
				if(pNode->getLeft()->isConst()|| pNode->getRight()->isConst()) {
					t.nSubConst ++;
				} else {
					t.nSub++;
				}
			}
		}
	}

	/**
	 * @brief DfgMan::getExpression get the expression
	 *
	 * @param mEs
	 * @param mCSEs
	 * @param patchFactors
	 **/
	void DfgMan::getExpression(map<string, string> & mEs, map<string, string>* mCSEs, bool patchFactors) {
		vector <DfgNode* > vNodes;
		string expr;
		map <DfgNode*, string> mExprs;
		DfgNode* pNode;
		bool bLeftParenthesis,  bRightParenthesis;
		map <DfgNode*, string>::iterator mp;
		map <DfgNode*, unsigned int> mNodeRefs;
		unsigned int counter;
		collectDFS(vNodes);
		counter = 0;
		if(mCSEs != NULL) {
			mCSEs->clear();
			for(unsigned int i = 0; i < vNodes.size(); i++) {
				pNode = vNodes[i];
				mNodeRefs[pNode] = 0;
				if(pNode->isOp()) {
					mNodeRefs[pNode->getLeft()] ++;
					mNodeRefs[pNode->getRight()] ++;
					if(pNode->getLeft()->isOp()&& mNodeRefs[pNode->getLeft()] == 2) {
						mExprs[pNode->getLeft()] = "S"+Util::itoa(counter++);
					}
					if(pNode->getRight()->isOp()&& mNodeRefs[pNode->getRight()] == 2) {
						mExprs[pNode->getRight()] = "S"+Util::itoa(counter++);
					}
				}
			}
		}
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			pNode = vNodes[i];
			bLeftParenthesis = bRightParenthesis = false;
			switch(pNode ->getOp()) {
			case DfgOperator::ADD:
			case DfgOperator::SUB:
			case DfgOperator::MUL:
			case DfgOperator::DIV:
			case DfgOperator::LSH:
			case DfgOperator::RSH:
				{
					if(pNode->isSub()&&(pNode->getRight()->isAdd()|| pNode->getRight()->isSub())) {
						bRightParenthesis = true;
					} else if(pNode->isMul()) {
						if(pNode->getLeft()->isAdd()|| pNode->getLeft()->isSub()) {
							bLeftParenthesis = true;
						}
						if(pNode->getLeft()->isConst()&& pNode->getLeft()->getConst()<0) {
							bLeftParenthesis = true;
						}
						if(pNode->getRight()->isAdd()|| pNode->getRight()->isSub()) {
							bRightParenthesis = true;
						}
						if(pNode->getRight()->isConst()&& pNode->getRight()->getConst()<0) {
							bRightParenthesis= true;
						}
					}
					expr = (bLeftParenthesis ? "(":"");
					expr+=mExprs[pNode->getLeft()];
					expr+= (bLeftParenthesis ? ")":"");
					expr+=pNode->getOpStr();
					expr+= (bRightParenthesis? "(":"");
					expr+=mExprs[pNode->getRight()];
					expr+= (bRightParenthesis? ")":"");
					if( (mCSEs != NULL) && ((mp = mExprs.find(pNode))!= mExprs.end()) ) {
						(*mCSEs)[mp->second] = expr;
					} else {
						mExprs[pNode] = expr;
					}
					break;
				}
			case DfgOperator::CONST:
				mExprs[pNode] = Util::itoa(pNode->getConst());
				break;
			case DfgOperator::VARCONST:
			case DfgOperator::VAR:
				mExprs[pNode] = pNode->getName();
				break;
			default:
				throw(string("01021. UNKOWN DFG node type."));
			}
		}
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp != _mPos.end(); mp++) {
			assert(mExprs.find(mp->second)!= mExprs.end());
			mEs[mp->first] = mExprs[mp->second];
		}
		// Patch mExprs and any new mCSEs found
		// as to use the DF factorized variables
		// that exist on the TED.
		if(patchFactors) {
			map<string,string> patchExpr;
			for(map<string, string>::iterator p = mEs.begin(); p != mEs.end(); p++) {
				if(p->first[0] == 'D' && p->first[1] == 'F') {
					patchExpr[p->first] = p->second;
				}
			}
			if(mCSEs!=NULL) {
				for(map<string, string>::iterator p = mCSEs->begin(); p != mCSEs->end(); p++) {
					if(p->first[0] == 'D' && p->first[1] == 'F') {
						patchExpr[p->first] = p->second;
					}
				}
			}
			int c = 0;
			for(map<string, string>::iterator patch = patchExpr.begin(); patch != patchExpr.end(); patch++) {
				for(map<string, string>::iterator p = mEs.begin(); p != mEs.end(); p++) {
					if(patchExpr.find(p->first) == patchExpr.end()) {
						while((c = p->second.find(patch->second))!= string::npos) {
							p->second.replace(c, patch->second.size(), patch->first);
						}
					}
				}
				if(mCSEs!=NULL) {
					for(map<string, string>::iterator p = mCSEs->begin(); p != mCSEs->end(); p++) {
						if(patchExpr.find(p->first) == patchExpr.end()) {
							while((c = p->second.find(patch->second))!= string::npos) {
								p->second.replace(c, patch->second.size(), patch->first);
							}
						}
					}
				}
			}
		}
		// end of patch for using the DF variables
	}

	DfgMan* DfgMan::useShifter(const string& shiftName) {
		DfgNode* pNodeNew;
		DfgMan* pManNew = new DfgMan();
		for(DfgMan::PrimaryOutputs::iterator mp=_mPos.begin(), end=_mPos.end(); mp!=end; mp++) {
			pNodeNew = _UseShift_Rec(pManNew, mp->second, shiftName);
			pManNew->linkDFGToPO(pNodeNew, mp->first);
		}
		return pManNew;
	}

	/**
	 * @brief DfgMan::_ConstToCsd transform a constant value into its Canonical Signed Digit
	 * @param pManNew
	 * @param v
	 * @param L
	 * @return DfgNode
	 **/
	DfgNode* DfgMan::_ConstToCsd(DfgMan* pManNew, wedge v, const string& shiftName) {
		assert(v != 0);
		DfgNode* pNode;
		Csd c(v);
		if(c[0] == 1) {
			pNode = new DfgNode(pManNew, 1);
		} else if(c[0] == -1) {
			pNode = new DfgNode(pManNew, -1);
		} else {
			pNode = new DfgNode(pManNew,(long)0);
		}
		DfgNode* pVar = new DfgNode(pManNew, shiftName);
		for(size_t i = 1; i < c.size(); i++) {
			if(c[i] == 1 || c[i] == -1) {
				DfgNode* pTemp = new DfgNode(pManNew, 1);
				for(size_t j = 0; j < i; j++) {
					pTemp = pTemp->mul(pVar);
				}
				if(c[i] == 1) {
					pNode = pNode->add(pTemp);
				} else {
					pNode = pNode->sub(pTemp);
				}
			}
		}
		return pNode;
	}

	DfgNode* DfgMan::_UseShift_Rec(DfgMan* pManNew, DfgNode* pNode, const string& shiftName) {
		switch(pNode->getOp()) {
		case DfgOperator::CONST:
			return _ConstToCsd(pManNew, pNode->getConst(), shiftName);
		case DfgOperator::VAR:
			{
				return new DfgNode(pManNew, pNode->getName());
			}
		case DfgOperator::ADD:
		case DfgOperator::SUB:
		case DfgOperator::MUL:
		case DfgOperator::DIV:
		case DfgOperator::LSH:
		case DfgOperator::RSH:
			{
				DfgNode* leftnode = _UseShift_Rec(pManNew, pNode->getLeft(), shiftName);
				DfgNode* rightnode = _UseShift_Rec(pManNew, pNode->getRight(), shiftName);
				return new DfgNode(pManNew, pNode->getOp(), leftnode, rightnode);
			}
		default:
			throw(string("01023. UNKOWN DFG node type."));
		}
	}


	struct _lt_strash {
		bool operator()(DfgNode* n1, DfgNode* n2)const {
			char* p1,* p2;
			int i;
			//	bool ret;

			p1 = n1->getSymbolSave();
			p2 = n2->getSymbolSave();
			if((i = strcmp(p1, p2))!=0) {
				free(p1);
				free(p2);
				return i<0;
			}
			free(p1);
			free(p2);


			if(n1->getLeft() == n2->getLeft()) {
				return n1->getRight()< n2->getRight();
			} else {
				return n1->getLeft()< n2->getLeft();
			}

			/*
			   if(n1->getLeft()!= NULL && n2->getLeft()!= NULL && n1->getLeft()!= n2->getLeft()) {
			   return(n1->getLeft()< n2->getLeft());
			   }
			   if(n1->getRight()!= NULL && n2->getRight()!= NULL && n1->getRight()!= n2->getRight()) {
			   return(n1->getRight()< n2->getRight());
			   }
			   return false;
			   */
		}
	};

	void DfgMan::strash(void) {
		vector <DfgNode*> vNodes;
		unsigned int i;
		DfgNode* pLeft = NULL;
		DfgNode* pRight = NULL;
		set <DfgNode*, _lt_strash> sNodes;
		set <DfgNode*, _lt_strash>::iterator p;
		collectDFS(vNodes);
		for(i= 0; i < vNodes.size(); i++) {
			if(!vNodes[i]->isOp()) {
				continue;
			}
			pLeft  = vNodes[i]->getLeft();
			pRight = vNodes[i]->getRight();
			p = sNodes.find(pLeft);
			if(p == sNodes.end()) {
				sNodes.insert(pLeft);
			} else {
				pLeft =*p;
			}
			p = sNodes.find(pRight);
			if(p == sNodes.end()) {
				sNodes.insert(pRight);
			} else {
				pRight =*p;
			}
			vNodes[i]->setKids(pLeft, pRight);
		}
		for(DfgMan::PrimaryOutputs::iterator q = _mPos.begin(); q != _mPos.end(); q++) {
			if(q->second) {
				if((p=sNodes.find(q->second))!= sNodes.end()) {
					q->second =*p;
				} else {
					sNodes.insert(q->second);
				}
			}
		}
	}

	void DfgMan::_UpdateRequiredTime_Rec(DfgNode* pNode, unsigned int _req) {
		if(pNode->_visited.isMarked()&& _req >= pNode->getRequiredTime()) {
			return;
		}
		if(_req < pNode->getRequiredTime()) {
			pNode->setRequiredTime(_req);
		}
		_req = pNode->getRequiredTime();
		if(pNode->getLeft()) {
			_UpdateRequiredTime_Rec(pNode->getLeft(), _req-pNode->getOpDelay());
		}
		if(pNode->getRight()) {
			_UpdateRequiredTime_Rec(pNode->getRight(), _req-pNode->getOpDelay());
		}
		pNode->_visited.setMark();
	}

	void DfgMan::updateRequiredTime(void) {
		unsigned int req = 0;
		Mark<DfgNode>::newMark();
		for(DfgMan::PrimaryOutputs::iterator q = _mPos.begin(); q != _mPos.end(); q++) {
			if(req < q->second->getArrivalTime()) {
				req = q->second->getArrivalTime();
			}
		}
		for(DfgMan::PrimaryOutputs::iterator q = _mPos.begin(); q != _mPos.end(); q++) {
			_UpdateRequiredTime_Rec(q->second, req);
		}
	}

	void DfgMan::balance(void) {
		updateDelayAndNumRefs();
		DfgNode* pNode;
		set <DfgNode*> sVisited;
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			pNode = (p->second);
			if(pNode && sVisited.find(pNode) == sVisited.end()) {
				pNode->balance(this);
				sVisited.insert(pNode);
			}
			// else it is a dangling node;
		}
		strash();
		cleanUp();
		updateRequiredTime();
	}

	void DfgMan::schedule(unsigned int nMul, unsigned int nAdd, unsigned int nSub, bool pipeline) {
		vector <DfgNode*> vNodes;
		collectDFS(vNodes);
		DfgNode* pNode;
		//bool unlimited_resources = (nMul==0 && nAdd==0 && nSub==0);
		map <DfgOperator::Type, unsigned int> mBuckets;
		map <DfgOperator::Type, unsigned int> mTemp;
		mBuckets[DfgOperator::MUL] = (nMul == 0) ?UINT_MAX:nMul;
		mBuckets[DfgOperator::ADD] = (nAdd == 0) ?UINT_MAX:nAdd;
		mBuckets[DfgOperator::SUB] = (nSub == 0) ?UINT_MAX:nSub;
		map <unsigned int, multiset<DfgNode*, _lt_slack> > arrival_table;
		map <unsigned int, multiset<DfgNode*, _lt_slack> >::iterator  arrival_it;
		multiset <DfgNode*, _lt_slack>::iterator slack_it;
		for(size_t i = 0; i < vNodes.size(); i++) {
			arrival_table[vNodes[i]->getArrivalTime()].insert(vNodes[i]);
		}
		for(arrival_it = arrival_table.begin(); arrival_it != arrival_table.end(); arrival_it++) {
			mTemp = mBuckets;
			for(slack_it = arrival_it->second.begin(); slack_it != arrival_it->second.end(); slack_it++) {
				pNode =*slack_it;
				if(!pNode->isOp()) {
					continue;
				}
				unsigned int newArrivalTime = Util::max(pNode->getLeft()->getArrivalTime(), pNode->getRight()->getArrivalTime());
				newArrivalTime += pNode->getOpDelay();
				if(pNode->getArrivalTime()< newArrivalTime) {
					pNode->setArrivalTime(newArrivalTime);
					pNode->setRequiredTime(Util::max(pNode->getArrivalTime(), pNode->getRequiredTime()));
					arrival_table[pNode->getArrivalTime()].insert(pNode);
					continue;
				}
				if(mTemp[pNode->getOp()] != 0) {
					mTemp[pNode->getOp()] --;
				} else {
					pNode->incArrivalTime(pipeline ? 1 : pNode->getOpDelay());
					pNode->setRequiredTime(Util::max(pNode->getArrivalTime(), pNode->getRequiredTime()));
					arrival_table[pNode->getArrivalTime()].insert(pNode);
				}
			}
		}
	}

	void DfgMan::scheduleStat(DfgStat& stat) {
		vector <DfgNode*> vNodes;
		//
		// collectDFG returns the latency assuming all operands have a delay of 1
		//
		collectDFS(vNodes);
		//
		// correct latency
		//
		int latency = maxArrivalTime();
		//
		// initialize the resources used (none at the beginning)
		//
		stat.rADD = 0;
		stat.rMPY = 0;
		stat.nAdd = 0;
		stat.nMul = 0;
		stat.nSub = 0;
		vector<unsigned int> adders(latency+1);
		vector<unsigned int> mult(latency+1);
		for(int index = 0; index <= latency; index++) {
			adders[index] = 0;
			mult[index] = 0;
		}
		for(size_t i = 0; i < vNodes.size(); i++) {
			unsigned int level = vNodes[i]->getArrivalTime();
			if(level>latency) {
				latency=level;
				cout << "latency has increased" << endl;
			}
			switch(vNodes[i]->getOp()) {
			case DfgOperator::MUL:
				mult[level]++;
				if(!vNodes[i]->getLeft()->isConst()&& !vNodes[i]->getRight()->isConst())
					stat.nMul++;
				else
					stat.nMulConst++;
				break;
			case DfgOperator::ADD:
				adders[level]++;
				stat.nAdd++;
				break;
			case DfgOperator::SUB:
				adders[level]++;
				stat.nSub++;
				break;
			default:
				break;
			}
		}
		for(int index = 0; index<=latency; index++) {
			if(adders[index]>stat.rADD) {
				stat.rADD = adders[index];
			}
			if(mult[index]>stat.rMPY) {
				stat.rMPY = mult[index];
			}
		}
		stat.latency = latency;
	}

	void DfgMan::balanceAreaRecovery(void) {
		updateDelayAndNumRefs();
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			set <DfgNode*> sVisited;
			if(p->second)p->second->balanceAreaRecovery(sVisited);
		}
		updateRequiredTime();
	}

	void DfgMan::updateDelayAndNumRefs(void) {
		vector <DfgNode*> vNodes;
		unsigned int dLeft, dRight;
		collectDFS(vNodes);
		DfgNode* pNode;
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			pNode = vNodes[i];
			vNodes[i]->_nrefs = 0;
			if(vNodes[i]->getLeft()) {
				vNodes[i]->getLeft()->incRef();
			}
			if(vNodes[i]->getRight()) {
				vNodes[i]->getRight()->incRef();
			}
			dLeft = dRight = 0;
			if(vNodes[i]->getLeft()) {
				dLeft  = vNodes[i]->getLeft()->getArrivalTime();
			}
			if(vNodes[i]->getRight()) {
				dRight = vNodes[i]->getRight()->getArrivalTime();
			}
			vNodes[i]->setArrivalTime((dLeft>dRight?dLeft:dRight)+ vNodes[i]->getOpDelay());
		}
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			if(p->second) {
				p->second->incRef();
			}
		}
	}

	int DfgMan::maxArrivalTime(void) {
		unsigned int max_arrival = 0;
		for(DfgMan::PrimaryOutputs::iterator q = _mPos.begin(); q != _mPos.end(); q++) {
			max_arrival = Util::max(max_arrival, q->second->getArrivalTime());
		}
		return max_arrival;
	}

	void DfgMan::remapShifter(unsigned int maxShiftLevel) {
		DfgShifter shift(this,maxShiftLevel);
		shift.remapShifter();
	}

	/**
	 * @brief DfgMan::writeCDFG writes the DFG graph in the CDFG format required by GAUT.
	 * @param file the name of the output file.
	 */
	void DfgMan::writeCDFG(const char* file) {
		ofstream ofs(file);
		vector <DfgNode*> vNodes, vConsts, vInputs, vOutputs, vVariables, vAgings;
		collectDFS(vNodes);
		ofs << "#VLSICAD-UMASS, 2012"<<endl;
		ofs << "#Date : " <<endl;
		ofs << "#Entry : poly2dfg" <<endl;
		ofs << "#  DFG output name -> DFG node pointer"<<endl;
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			ofs << "#\tNode_"<<p->first << " -> Node_" <<p->second << "\n";
		}
		ofs << "#"<<endl;
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			if(vNodes[i]->isPI()) {
				vInputs.push_back(vNodes[i]);
			} else if(vNodes[i]->isConst() || vNodes[i]->isVarConst()) {
				vConsts.push_back(vNodes[i]);
			} else {
				vVariables.push_back(vNodes[i]);
			}
		}
		ofs << "source(poly2dfg_start) {"<<endl;
		ofs << "\ttargets ";
		for(unsigned int i = 0; i < vConsts.size(); i++) {
			ofs <<(i==0?" ":",\n\t")<< Environment::getStr("const_prefix")<< Util::conformName(vConsts[i]->getConst());
		}
		if(!vConsts.empty()&& !vInputs.empty())
			ofs << ",\n\t";
		for(unsigned int i = 0; i < vInputs.size(); i++) {
			ofs <<(i==0?" ":",\n\t")<< vInputs[i]->getName();
		}
		ofs << ";\n}\n";
		ofs << "#Constant declaration\n";
		for(unsigned int i = 0; i < vConsts.size(); i++) {
			ofs << "constant(" << Environment::getStr("const_prefix")<<Util::conformName(vConsts[i]->getConst())<<") {\n";
			ofs << "\tbitwidth 32;\n";
			ofs << "\tsigned 1;\n";
			ofs << "\tvalue "<<Util::conformName(vConsts[i]->getConst())<<";\n";
			ofs << "\tbank 0;\n";
			ofs << "\taddress "<< i <<";\n";
			ofs << "\thardwire 1;\n";
			ofs << "}\n";
		}
		ofs << "#Input declaration\n";
		for(unsigned int i = 0; i < vInputs.size(); i++) {
			ofs << "input("<< getNameIfRegisters(vInputs[i]) <<") {\n";
			ofs << "\tbitwidth 32;\n";
			ofs << "\tsigned 1;\n";
			ofs << "}\n";
		}
		ofs << "#Output declaration\n";
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			ofs << "output(" << p->first << ") {\n";
			ofs << "\tbitwidth 32;\n";
			ofs << "\tsigned 1;\n";
			ofs << "}\n";
		}
		ofs << "#Variable declaration\n";
		for(unsigned int i = 0; i < vVariables.size(); i++) {
			if (vVariables[i]->getOp() == DfgOperator::REG) {
				ofs << "# Registers #\n";
				string retime_value = vVariables[i]->_data.name;
				retime_value.erase(retime_value.find_first_of(DfgNode::TOKEN_REG));
				std::pair<bool,wedge> ret = Util::atoi(retime_value.c_str());
				assert(ret.first);
				assert(vVariables[i]->getLeft() ==vVariables[i]->getRight());
				wedge start = ret.second;
				for (;ret.second>0;ret.second--) {
					ofs << "variable("<< writeCDFGop(vVariables[i]);
					if (ret.second!=start) {
						ofs << "R" << ret.second;
					}
					ofs << ") {\n";
					if (ret.second!=1) {
						ofs << "\taging " << writeCDFGop(vVariables[i]) << "R" << ret.second-1 << ";\n";
					} else {
						ofs << "\taging ";
						writeCDFGname(ofs,vVariables[i]->getLeft());
						ofs << ";\n";
					}
					ofs << "\tbitwidth 32;\n";
					ofs << "\tsigned 1;\n";
					ofs << "}\n";
				}
			} else {
				ofs << "variable("<<writeCDFGop(vVariables[i])<<") {\n";
				ofs << "\tbitwidth 32;\n";
				ofs << "\tsigned 1;\n";
				ofs << "}\n";
			}
		}
		ofs <<"#Operations\n";
		for(unsigned int i = 0; i < vVariables.size(); i++) {
			string func;
			switch(vVariables[i]->getOp()) {
			case DfgOperator::REG:
				continue;
			case DfgOperator::MUL:
				func = "mul";
				break;
			case DfgOperator::ADD:
				func = "add";
				break;
			case DfgOperator::SUB:
				func = "sub";
				break;
			case DfgOperator::LSH:
				func = "sll";
				break;
			case DfgOperator::EQ:
				func = "assign";
				break;
			default:
				func = "unknown";
				break;
			}
			ofs << "operation(op"<<i<<") {\n";
			ofs << "\tfunction " << func <<";\n";
			ofs << "\tread ";
			writeCDFGname(ofs,vVariables[i]->getLeft());
			ofs << ",";
			writeCDFGname(ofs,vVariables[i]->getRight());
			ofs << ";\n";
			ofs << "\twrite "<< writeCDFGop(vVariables[i])<<";\n";
			ofs << "}\n";
		}
		ofs << "#assign to outputs" << endl;
		unsigned int j = vVariables.size();
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			ofs << "operation(op"<<j++<<") {\n";
			ofs << "\tfunction assign;" << endl;
			ofs << "\tread " << writeCDFGop(p->second) << ";\n";
			ofs << "\twrite " << p->first << ";\n";
			ofs << "}\n";
		}
		ofs <<"sink(poly2dfg_end) {\n";
		ofs <<"\ttargets ";
		for(DfgMan::PrimaryOutputs::iterator p = _mPos.begin(); p != _mPos.end(); p++) {
			if(p == _mPos.begin())
				ofs << "\t" << p->first;
			else
				ofs << ",\n\t" << p->first;
		}
		ofs <<";\n";
		ofs <<"}";
		ofs.close();
	}

	void DfgMan::writeCDFGname(ofstream& ofs, DfgNode* pNode) {
		switch(pNode->getOp()) {
		case DfgOperator::CONST:
			ofs << Environment::getStr("const_prefix")<< Util::conformName(pNode->getConst());
			break;
		case DfgOperator::VARCONST:
		case DfgOperator::VAR:
			ofs << getNameIfRegisters(pNode);
			break;
		default:
			ofs << writeCDFGop(pNode);
			break;
		}
	}

	string DfgMan::writeCDFGop(DfgNode* pNode) {
		string name = "id";
		name += Util::itoa(pNode->getOperator()->getID());
		return name;
	}

	bool DfgMan::regressionTest(stringstream& message) {
		bool ret = true;
		int passed = 0;
		int failed = 0;
		stringstream annotateFail;
		stringstream annotatePass;
		for(DfgMan::PrimaryOutputs::iterator it = _mPos.begin(); it != _mPos.end(); it++) {
			if((it->first).size()>5 &&(it->first).compare((it->first).size()-5,5,"_diff") ==0) {
				assert((it->second)->isSub());
				assert((it->second)->getLeft()->isPI());
				assert((it->second)->getRight()->isPI());
				DfgNode* pLeft = _mPos[(it->second)->getLeft()->getName()];
				DfgNode* pRight = _mPos[(it->second)->getRight()->getName()];
				if((pLeft->getLevel()!= pRight->getLevel())||
				   (pLeft->getOp()!= pRight->getOp())) {
					failed++;
					ret = false;
					annotateFail << ":" << it->first;
				} else {
					passed++;
					annotatePass << ":" << it->first;
				}
			}
		}
		int total = passed+failed;
		if(total==passed) {
			message << " all(" << passed << "/" << total << ")";
		} else if(total==failed) {
			message << " all(" << failed << "/" << total << ")";
		} else {
			message << " " << passed << "/" << total << " " << annotatePass.str()<< " ";
			message << " " << failed << "/" << total << " " << annotateFail.str()<< " ";
		}
		return ret;
	}

	void DfgMan::evaluateConstants(void) {
		vector<DfgNode*> vNodes;
		collectDFS(vNodes);
		string const_prefix = Environment::getStr("const_prefix");
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			if((vNodes[i]->isPI()|| vNodes[i]->isVarConst())) {
				string const_prefix = Environment::getStr("const_prefix");
				string name = vNodes[i]->getName();
				if(name.length()> const_prefix.length()&& !name.substr(0,const_prefix.length()).compare(const_prefix)) {
					name.erase(0,const_prefix.length());
					string neg_prefix = Environment::getStr("negative_prefix");
					if(name.length()> neg_prefix.length()&& !name.substr(0,neg_prefix.length()).compare(neg_prefix)) {
						name.erase(0,neg_prefix.length());
						name.insert(0,"-");
					}
					pair<bool,wedge> tt=Util::atoi(name.c_str());
					if(tt.first) {
						//transform const_moins_integer to -integer
						//transform const_integer to integer
						vNodes[i]->_data.value = tt.second;
						vNodes[i]->setType(DfgOperator::CONST);
					}
				}
			}
		}
	}

#ifdef FUNCTIONS_DEPRECATED
	//TODO there should be maybe only one collectDFS function that saves the traversal
	//on the vector as a pair <DfgNode*,level>
	/**
	 * @brief DfgMan::collectDFS Collects all the DFG nodes
	 *
	 * @param vNodes Container with all the nodes in the DFG graph.
	 * @param mNodeLevels a map of al DFG nodes with its respective level.
	 *
	 * @return the maximum level of the DFG
	 **/
	unsigned int DfgMan::collectDFS(vector <DfgNode*> & vNodes,
									map <DfgNode*, unsigned int >* mNodeLevels) {
		unsigned int l = 0;
		vNodes.clear();
		bool bInit = false;;
		if(mNodeLevels == NULL) {
			bInit = true;
			mNodeLevels = new map <DfgNode*, unsigned int>;
		}
		for(DfgMan::PrimaryOutputs::iterator mp = _mPos.begin(); mp!= _mPos.end(); mp++) {
			l = MAX(l, mp->second->collectDFS(vNodes, mNodeLevels));
		}
		if(bInit) {
			delete mNodeLevels;
		}
		return l;
	}

	void DfgMan::unlinearize(char* v) {
		vector <DfgNode*> vNodes;
		collectDFS(vNodes);
		DfgNode* pNode,* pLeft,* pRight;
		pNode = new DfgNode(this, v);
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			if(vNodes[i]->isPI())continue;
			if(vNodes[i]->isVarConst())continue;
			if(vNodes[i]->isConst())continue;
			assert(pNode != NULL);
			pLeft = vNodes[i]->getLeft();
			pRight = vNodes[i]->getRight();
			if((pLeft->isPI()|| pLeft->isVarConst())&& strncmp(pLeft->getName(), v, strlen(v)) == 0) {
				pLeft = pNode;
			}
			if((pRight->isPI()|| pRight->isVarConst())&& strncmp(pRight->getName(), v, strlen(v)) == 0) {
				pRight = pNode;
			}
			vNodes[i]->setKids(pLeft, pRight);
		}
		strash();
		updateDelayAndNumRefs();
	}
#endif

}
