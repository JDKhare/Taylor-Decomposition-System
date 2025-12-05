/*
 * =====================================================================================
 *
 *        Filename:  TEDMan.cc
 *     Description:  Tedman class
 *         Created:  04/17/2007 12:24:18 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <list>

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedNodeIterator.h"
#include "TedParents.h"
#include "TedDecompose.h"
#include "TedBitwidth.h"

#include "Bitwidth.h"
using namespace data;

#include "BinaryThreadedTree.h"
using dtl::BinaryThreadedTree;
using dtl::BinaryThreadedNode;

#include "Environment.h"
using namespace tds;

#include "DfgNode.h"
#include "DfgMan.h"
using dfg::DfgMan;
using dfg::DfgNode;
using dfg::DfgStat;

#include "util.h"
using util::Util;

#include "Convert.h"
using convert::Convert;

namespace ted {
#ifndef INLINE
#include "TedMan.inl"
#endif

	unsigned int TedMan::_factor_counter = 1;

	TedMan::const_dfs_iterator TedMan::const_dfs_begin(void)const{
		return const_dfs_iterator(this,const_dfs_iterator::begin);
	}

	TedMan::const_dfs_iterator TedMan::const_dfs_end(void)const{
		return const_dfs_iterator(const_dfs_iterator::end);
	}

	TedMan::dfs_iterator TedMan::dfs_begin(void) {
		return dfs_iterator(this,dfs_iterator::begin);
	}

	TedMan::dfs_iterator TedMan::dfs_end(void) {
		return dfs_iterator(dfs_iterator::end);
	}

	TedMan::TedMan(void): _vars(TedVarMan::instance()) {
		_const_limit = 0;
		ATedNode::setContainer(_container);
	}

	TedMan::~TedMan(void) {
		//ATedNode::purge_transitory_container();
	}

	void TedMan::purge_all_but(TedMan* p1) {
		assert(p1);
		Mark<TedNode>::newMark();
		Mark<TedKids>::newMark();
		p1->getContainer().update_TMark();
		ATedNode::purge_transitory_container();
		TedKids::purge_transitory_container();
	}

	void TedMan::purge_all_but(TedMan* p1,TedMan* p2) {
		assert(p1);
		assert(p2);
		Mark<TedNode>::newMark();
		Mark<TedKids>::newMark();
		p1->getContainer().update_TMark();
		p2->getContainer().update_TMark();
		ATedNode::purge_transitory_container();
		TedKids::purge_transitory_container();
	}

	void TedMan::purge_all_but(TedMan* p1,TedMan* p2, TedMan* p3) {
		assert(p1);
		assert(p2);
		assert(p3);
		Mark<TedNode>::newMark();
		Mark<TedKids>::newMark();
		p1->getContainer().update_TMark();
		p2->getContainer().update_TMark();
		p3->getContainer().update_TMark();
		ATedNode::purge_transitory_container();
		TedKids::purge_transitory_container();
	}

	bool TedMan::check_for_dangling_nodes(void) {
		TedParents parents;
		this->preOrderDFS(parents, &TedParents::collect);
		//this->preOrderDFSforPOs(parents, &TedParents::collect);
		for(TedContainer::const_iterator it = _container.begin(); it != _container.end(); it++) {
			for(TedSet::const_iterator jt = it->second.second->begin(); jt != it->second.second->end(); jt ++) {
				TedNode* pNode = (*jt);
				assert(pNode);
				assert(pNode->getKids());
				if (parents.find(pNode) ==parents.end()) {
					if (!this->isPO(pNode)) {
						return false;
					}
				}
			}
		}
		return true;
	}

	/** @brief Normalize a ATedNode and put resulting TedNodes in manager*/
	TedNodeRoot TedMan::normalize(ATedNode* pANode) {
		pANode = _PropogateZero_Rec(pANode);
		if(pANode) {
			pair<TedNode*, wedge> pe = _Normalize_Rec(pANode);
			return TedNodeRoot(pe.first, pe.second);
		} else {
			return TedNodeRoot(TedNode::getOne(),0);
		}
	}

	/** @brief routine to remove Zeros in ATed*/
	ATedNode* TedMan::_PropogateZero_Rec(ATedNode* pANode) {
		unsigned int index = 0;
		ATedNode* pEKid = NULL,* pETemp = NULL;
		wedge weight = 0;

		if(pANode->isConst()) {
			if(pANode->getConst() == 0) {
				return NULL;
			} else {
				return pANode;
			}
		}
		list<unsigned int> index2erase;
		for(TedKids::iterator _iterkids = pANode->getKids()->begin(), _iterend = pANode->getKids()->end();
			_iterkids != _iterend; _iterkids++) {
			//FOREACH_KID_OF_ANODE(pANode, index, pEKid, weight)
			index = _iterkids->first;
			pEKid = (ATedNode*)_iterkids->second.Node();
			TedRegister r = _iterkids->second.getRegister();
			weight = _iterkids->second.getWeight();
			assert(pEKid);
			pETemp = _PropogateZero_Rec(pEKid);
			if(pETemp == NULL) {
				index2erase.push_back(index);
			} else {
				pANode->replaceKid(index, pETemp, 1,r);
			}
		}
		while(!index2erase.empty()) {
			pANode->removeKid(index2erase.back());
			index2erase.pop_back();
		}
		if(pANode->numKids() == 0)
			return NULL;
		if(pANode->numKids() == 1 && pANode->hasKid(0)) {
			pANode = (ATedNode*)pANode->getKidNode(0);
		}
		return pANode;
	}

	/** @brief routine to normalize a ATedNode, returning a Ted and the greatest common divisor*/
	pair <TedNode*, wedge > TedMan::_Normalize_Rec(TedNode* pANode) {
		assert(pANode);
		TedKids tKids;
		unsigned int index = 0;
		TedNode* pEKid = NULL;
		if(pANode->isConst()) {
			return pair<TedNode*, wedge>(TedNode::getOne(), pANode->getConst());
		}
		FOREACH_KID_OF_NODE(pANode) {
			index = _iterkids.getIndex();
			pEKid = _iterkids.Node<TedNode>();
			pair<TedNode*,wedge> pEdge = _Normalize_Rec(pEKid);
			tKids.insert(index, pEdge.first, pEdge.second,_iterkids.getRegister());
		}
		wedge gcd = tKids.extractGCD();
		const TedVar* pVar = pANode->getVar();
		TedNode* pNode = _container.getNode(*pVar, tKids, pANode->getRegister());
		return pair<TedNode*, wedge>(pNode, gcd);
	}

	/** @brief Link a TedNode to Po, with name poname, weight weight*/
	void TedMan::linkTedToPO(TedNode* pNode, wedge weight, const string& poname) {
		TedNodeRoot po(pNode, weight);
		linkTedToPO(po, poname);
	}

	/** @brief Link a TedNodeRoot to name poname, if poname is Null, it is auto assigned*/
	void TedMan::linkTedToPO(const TedNodeRoot& po, const string& poname) {
		string name;
		if(poname.empty()) {
			name = "F"+Util::itoa(_pos.size());
		} else {
			name = poname;
		}
		if (_pos.find(name) ==_pos.end()) {
			_pos.insert(pair<string,TedNodeRoot>(name,po));
		} else {
			_pos[name] = po;
		}
	}

	bool TedMan::isPO(TedNode* node) {
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			if(node == p->second.Node()) {
				return true;
			}
		}
		return false;
	}


	TedNodeRoot* TedMan::getPO(TedNode* node) {
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			if(node == p->second.Node()) {
				return &(p->second);
			}
		}
		return NULL;
	}

	/** @brief Unlinking a primary output*/
	TedMan* TedMan::extractPO(set <string> se) {
		set <string > s;
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			if(se.find(p->first) == se.end()) {
				s.insert(p->first);
			}
		}
		return _DuplicateUnlink(& s);
	}

	/** @brief Collect nodes in the order of DFS*/
	void TedMan::collectDFS(vector <TedNode*> & vNodes) {
		Mark<TedNode>::newMark();
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			p->second.Node()->_collectDFS_rec(vNodes);
		}
	}



	/** @brief Show Dot File*/
	void TedMan::writeDot(ofstream& ofile, bool bigFont, bool compact_graph) {
		string poFontSize = "";
		string oneFontSize = "";
		if(bigFont) {
			poFontSize = "fixedsize=\"true\", fontsize=14, ";
			oneFontSize = "fixedsize=\"true\", fontsize=12, ";
		}
		ofile << "Digraph G {\n\tsize=\"10, 7.5\"; center = \"true\"; \n";
		ofile << "\t{\n";
		ofile << "\t\tnode [shape = plaintext];\n";
		ofile << "\t\tedge [style = invis];\n";
		ofile << "\t\tLevelPO [label=\"\"];\n";
		unsigned int maxLevel = _container.getMaxLevel();
		ofile << "\t\tLevel0 [label=\"\"];\n";
		for(unsigned int i = 1; i <= maxLevel; i++) {
			TedSet* used = _container[_container.getVarAtLevel(i)].second;
			if(used && !used->empty()) {
				ofile << "\t\tLevel" << i << " [label=\"";
				if(Environment::getBool("show_level"))
					ofile <<  i << ":" << _container.getVarAtLevel(i).getName();
				if(_vars.getIfExist(_container.getVarAtLevel(i).getName())->getBitwidth())
					ofile << " b="<< _vars.getIfExist(_container.getVarAtLevel(i).getName())->getBitwidth()->at();
				ofile << "\"];\n";
			}
		}
		ofile << "\t\tLevelPO";
		for(unsigned int i = maxLevel; i > 0; i--) {
			TedSet* used = _container[_container.getVarAtLevel(i)].second;
			if(used && !used->empty()) {
				ofile << "-> Level" << i;
			}
		}
		ofile << "\n\t}\n";
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			ofile << "\t{rank = same; LevelPO; Node_" << p->first;
			ofile << " [" << poFontSize << "label=\""<<p->first<<"\", shape=invtriangle]; }\n";
			ofile << "\tNode_"<<p->first << " -> \"Node_" << obfuscate(p->second.Node())<< "\" [label=\"";
			if(p->second.getWeight()!= 1)
				ofile << p->second.getWeight();
			if(p->second.getBitwidth())
				ofile << "b="<<p->second.getBitwidth()->at();
			if(p->second.getRegister()!=0) {
				ofile << "\\n---" << p->second.getRegister()<< "R---";
			}
			ofile << "\"]\n";
		}
		for(TedContainer::iterator p = _container.begin(); p != _container.end(); p++) {
			for(TedSet::iterator sp = p->second.second->begin(); sp != p->second.second->end(); sp++) {
				ofile << dotString(*sp,bigFont,compact_graph);
			}
		}
		ofile << "\n\t{rank = same; Level0; \"Node_" << obfuscate(TedNode::getOne())<< "\"";
		ofile << " [" << oneFontSize << "label=\"ONE\", shape=triangle]; }\n";
		ofile << "}";
	}

	/** @brief The dot string of the node*/
	string TedMan::dotString(const TedNode* pNode, bool bigFont, bool compact_graph)const {
		string nodeFontSize = "";
		if(bigFont) {
			nodeFontSize = "fixedsize=\"true\", fontsize=18, ";
		}
		ostringstream oss;
		unsigned int index=0;
		TedNode* pKid=NULL;
		wedge weight=0;
		if(pNode->getRegister()> 0 || pNode->getBitwidth()) {
			oss << "\n\tsubgraph \"reg_" << obfuscate(pNode)<< "\" {";
			oss << "\n\trank = same; Level" << getLevel(pNode)<<"; \"node_reg_" << obfuscate(pNode)<< "\" [label=\"";
			if(pNode->getRegister()> 0)
				oss << pNode->getRegister()<< "R ";
			if(pNode->getBitwidth())
				oss << "b=" << pNode->getBitwidth()->at();
			oss << "\", shape = plaintext]";
		}
		oss << "\n\t{rank = same; Level" << getLevel(pNode)<< "; \"Node_" << obfuscate(pNode)<< "\" [";
		oss << nodeFontSize << "label=\"";
		if(compact_graph && pNode->getName().length()> 4)
			oss << " ";
		else
			oss << pNode->getName();
		oss << "\", ";
		oss	<< "shape=" <<(pNode->isConst() ?"triangle":"ellipse")<< "]; }\n";
		if(pNode->getRegister()> 0 || pNode->getBitwidth()) {
			oss << "\n\t\"Node_" << obfuscate(pNode)<< "\" -> \"node_reg_" << obfuscate(pNode)<< "\" [arrowhead=inv, minlen=0]";
			oss << "\n\t}";
		}
		FOREACH_KID_OF_NODE(pNode) {
			index = _iterkids.getIndex();
			pKid = _iterkids.Node<TedNode>();
			weight = _iterkids.getWeight();
			oss << "\t\"Node_"<<obfuscate(pNode)<<"\" -> \"Node_"<<obfuscate(pKid)<<"\" [style="<<(index==0?"dashed":"solid");
			oss	<< ", label=\"";
			if(index>1) {
				oss << "^"<<index<<" ";
			}
			if(weight!=1) {
				oss << weight;
			}
			if(_iterkids->second.getRegister()!=0) {
				oss << "\\n---" << _iterkids->second.getRegister()<< "R---";
			}
			if(_iterkids->second.getBitwidth()) {
				oss << "b=" << _iterkids->second.getBitwidth()->at();
			}
			oss << "\"];\n";
		}
		return oss.str();
	}

	string TedMan::obfuscate(const TedNode* ptr)const {
		ostringstream oss;
		string name(ptr->getName());
		/* size_t found = name.find_first_of('.');
		   while(found!=string::npos) {
		   name.replace(found,1,"\\.");
		   found = name.find_first_of('.',found+3);
		   }*/
#ifdef _DEBUG
		oss << name << "_" << ptr;
#else
		const size_t distance = sizeof(TedNode);
		const size_t val = ((size_t)ptr)/distance;
		oss << name << "_" << hex << val;
#endif
		return oss.str();
	}

	void TedMan::writeDotDetailed(void) {
		writeDotDetailed(NULL);
	}

	void TedMan::writeDotDetailed(const TedNode* pTouch) {
		static unsigned int index = 0;
		ofstream ofile;
		string filename = Environment::getStr("show_directory")+ "debugTed" + Util::itoa(index++)+ ".dot";
		ofile.open(filename.c_str());
		if(!ofile.is_open()) {
			throw(string("04014. Cannot open temporary file for writing.\n"));
		}
		writeDotDetailed(ofile,pTouch);
		ofile.close();
		Util::showDotFile(filename.c_str());
	}

	void TedMan::writeDotDetailed(ofstream& ofile) {
		writeDotDetailed(ofile,NULL);
	}

	/** @brief Show Dot File*/
	void TedMan::writeDotDetailed(ofstream& ofile, const TedNode* pTouch) {
		ofile << "Digraph G {\n\tsize=\"10, 7.5\"; center = \"true\"; \n";
		ofile << "\t{\n";
		ofile << "\t\tnode [shape = plaintext];\n";
		ofile << "\t\tedge [style = invis];\n";
		ofile << "\t\tLevelPO [label=\"\"];\n";
		unsigned int maxLevel = _container.getMaxLevel();
		for(unsigned int i = 0; i <= maxLevel; i++) {
			ofile << "\t\tLevel" << i << " [label=\""<< i <<"\"];\n";
		}
		ofile << "\t\tLevelPO";
		for(unsigned int i = 0; i <= maxLevel; i++) {
			ofile << "-> Level" << maxLevel-i;
		}
		ofile << "\n\t}\n";
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			ofile << "\t{rank = same; LevelPO; Node_" << p->first;
			ofile << " [shape=record, label=\"{ " << p->first << " | " << &(p->second)<< "} | {";
			switch(p->second.getType()) {
			case TedNodeRoot::PO :
				ofile << "PO";
				break;
			case TedNodeRoot::PT :
				ofile << "PT";
				break;
			case TedNodeRoot::ST :
				ofile << "ST";
				break;
			case TedNodeRoot::TC :
				ofile << "TC";
				break;
			case TedNodeRoot::BC :
				ofile << "BC";
				break;
			default :
				ofile << "err";
				break;
			}
			ofile << " | r=" << p->second.getRegister()<< " | w=" << p->second.getWeight();
			ofile << " | b=" <<(p->second.getBitwidth() ? p->second.getBitwidth()->at(): "X");
			ofile << "}\"]; }\n";
			ofile << "\tNode_"<<p->first << " -> Node_" << p->second.Node()<< "[label=\"";
			ofile << "\"]\n";
		}
		for(TedContainer::iterator p = _container.begin(); p != _container.end(); p++) {
			for(TedSet::iterator sp = p->second.second->begin(); sp != p->second.second->end(); sp++) {
				ofile << dotStringDetailed(*sp,pTouch);
			}
		}
		ofile << "\n\t{rank = same; Level0; Node_" << TedNode::getOne();
		ofile << " [label=\"ONE\", shape=triangle]; }\n";
		ofile << "}";
	}

	/** @brief The dot string of the node*/
	string TedMan::dotStringDetailed(const TedNode* pNode, const TedNode* pTouch) const {
		assert(pNode);
		ostringstream oss;
		unsigned int index=0;
		TedNode* pKid=NULL;
		wedge weight=0;
		oss << "\n\t{rank = same; Level" << getLevel(pNode)<< "; Node_" << pNode << " [shape=record,";
		oss << "label=\" {";
		oss << pNode << " | { {";
		oss << pNode->getName();
		if(pNode->getVar()->isConst())
			oss << "(C)";
		else if(pNode->getVar()->isVar())
			oss << "(V)";
		else
			oss << "(VC)";
		if (pNode->getVar()->isBinary()) {
			oss << "(B)";
		}
		oss << "\\n";
		oss << "b=" <<(pNode->getVar()->getBitwidth() ? pNode->getVar()->getBitwidth()->at(): "X")<< "\\n";
		oss << pNode->getVar()<< "} | {";
		oss <<(pNode->visited.isMarked() ? "M" : "U")<< pNode->visited.getMark();
		oss << " | r=" << pNode->getRegister();
		oss << " | b=" <<(pNode->getBitwidth() ? pNode->getBitwidth()->at(): "X");
		oss << "} }";
		if(NULL!=pNode->getBackptr()) {
			oss << " | ptr=" << pNode->getBackptr();
		}
		oss <<" } \"]; }\n";
		FOREACH_KID_OF_NODE(pNode) {
			index = _iterkids.getIndex();
			pKid = _iterkids.Node<TedNode>();
			weight = _iterkids.getWeight();
			oss << "\tNode_"<<pNode<<" -> Node_"<<pKid<<" [style="<<(index==0?"dashed":"solid");
			if (pKid==pTouch) {
				oss << ", color=\"red\"";
			} else if (pNode==pTouch) {
				oss << ", color=\"blue\"";
			}
			oss	<< ", label=\"";
			oss << "^,w,r,b\\n";
			oss << index <<"," << weight << "," << _iterkids->second.getRegister()<< ",";
			if(_iterkids->second.getBitwidth())
				oss << _iterkids->second.getBitwidth()->at();
			else
				oss << "X";
			oss << "\"];\n";
		}
		return oss.str();
	}

	/** @brief For substitution routine
	 *  @note the vector container forces the variables to be order by their names
	 **/
	void TedMan::getVars(vector<string>& vVars) {
		char** pvVars = (char**)malloc(sizeof(char*)* _container.size());
		for(TedContainer::iterator p = _container.begin(); p != _container.end(); p++) {
			*(pvVars + p->second.first-1) = (char*)p->first.getName().c_str();
		}
		for(unsigned int i = 0; i < _container.size(); i++) {
			string saveVar(*((char**)(pvVars + i)));
			vVars.push_back(saveVar);
		}
		free(pvVars);
	}

	/** @brief For substitution routine
	 *  @note the list container guarantees that the variables are order by their position in the TED
	 **/
	void TedMan::getVars(list<TedVar*>& vVars) {
		TedContainerOrder tnodesOrdered = _container.order();
		for(TedContainerOrder::const_iterator it = tnodesOrdered.begin(); it!= tnodesOrdered.end(); it++) {
			TedVar* pvar  = checkVariables(it->second.first->getName());
			vVars.push_back(pvar);
		}
	}

	/** @brief to duplicate TedManager, when poname is not NULL, the po is unlinked*/
	TedMan* TedMan::_DuplicateUnlink(set <string>* ponames) {
		TedMan* pManNew = new TedMan();
		TedContainer::iterator msp;
		map <TedNode*, TedNode*> mVisited;
		set <TedNode*>* sNodes;
		TedNode* pNode;
		set <const char*>::iterator p;
		map <unsigned int, unsigned int> mNewLevel;
		vector <TedVar> vVars;
		size_t i;
		for(msp = _container.begin(); msp!= _container.end(); msp++) {
			sNodes = new set <TedNode*>;
			pManNew->_container[TedVar(msp->first)] = pair<unsigned int, set <TedNode*>*>(msp->second.first, sNodes);
		}
		for(PrimaryOutputs::iterator mp = _pos.begin(); mp!= _pos.end(); mp++) {
			if(ponames != NULL && ponames->size()!= 0) {
				if(ponames->find(mp->first.c_str())!= ponames->end())continue;
			}
			pNode = _Duplicate_Rec(pManNew, mp->second.Node(), mVisited);
			TedNodeRoot po(pNode,mp->second.getWeight(),mp->second.getType());
			pManNew->linkTedToPO(po, mp->first);

		}
		//removing empty levels
		if(ponames != NULL && ponames->size()!= 0) {
			for(msp = pManNew->_container.begin(); msp!= pManNew->_container.end(); msp++) {
				if(msp->second.second->size() == 0) {
					delete msp->second.second;
					vVars.push_back(msp->first);
					mNewLevel[msp->second.first] = 0;
				} else {
					mNewLevel[msp->second.first] = msp->second.first;
				}
			}
			for(i = 0; i < vVars.size(); i++) {
				pManNew->_container.erase(vVars[i]);
			}
			i = 0;
			for(map <unsigned int, unsigned int>::iterator p = mNewLevel.begin(); p != mNewLevel.end(); p++) {
				if(p->second == 0) {
					i++;
				}
				p->second -= i;
			}
			for(msp = pManNew->_container.begin(); msp!= pManNew->_container.end(); msp++) {
				msp->second.first = mNewLevel[msp->second.first];
			}
		}
		return pManNew;
	}

	/** @brief Duplicate recursively a TedNode*/
	TedNode* TedMan::_Duplicate_Rec(TedMan* pManNew, TedNode* pNode, map <TedNode*, TedNode*> & mVisited) {
		map <TedNode*, TedNode*>::iterator mp;
		TedNode* pNodeNew = NULL,* pKid = NULL,*pKidNew = NULL;
		unsigned int index =0;
		wedge weight = 0;
		TedKids tKids;
		if((mp=mVisited.find(pNode))!= mVisited.end())
			return mp->second;
		if(pNode == TedNode::getOne())
			return pNode;
		if(pNode->isConst())
			return(new ETedNode(pNode->getConst()));
		FOREACH_KID_OF_NODE(pNode) {
			index = _iterkids.getIndex();
			pKid = _iterkids.Node<TedNode>();
			weight = _iterkids.getWeight();
			pKidNew = _Duplicate_Rec(pManNew, pKid, mVisited);
			TedRegister kidReg = _iterkids.getRegister();
			tKids.insert(index, pKidNew, weight,kidReg);
		}
		pNodeNew = new ETedNode(*pNode->getVar(), tKids, pNode->getRegister());
		pManNew->_container[*(pNode->getVar())].second->insert(pNodeNew);
		mVisited[pNode] = pNodeNew;
		return pNodeNew;
	}

	/** @brief Linearize a Ted by spliting non-linear variables*/
	TedMan* TedMan::linearize(void) {
		TedMan* pManNew = new TedMan();
		TedContainerOrder mNodesOrdered = _container.order();
		unsigned int maxOrder = 0;
		for(TedContainerOrder::iterator it = mNodesOrdered.begin(); it!= mNodesOrdered.end(); it++)
			if(it->second.first->isVar()&& !it->second.second->empty()) {
				maxOrder = 0;
				for(TedSet::iterator jt = it->second.second->begin(); jt != it->second.second->end(); jt++) {
					if(maxOrder <(*jt)->getKids()->highestOrder()) {
						maxOrder = (*jt)->getKids()->highestOrder();
					}
				}
				assert(maxOrder>0);
				TedVar* currentVar = it->second.first;
				unsigned int keyLevel = pManNew->_container.getMaxLevel()+ 1;
				if(1 == maxOrder) { //It is already linear
					assert(_vars.getIfExist(currentVar->getName()));
					TedSet* sNodes = new TedSet;
					pManNew->_container[*currentVar] = pair<unsigned int, TedSet*>(keyLevel, sNodes);
				} else { //It needs to be linearized
					TedVarGroupOwner* owner = _vars.createOwner(*currentVar);
					for(unsigned int i = 0; i < maxOrder; i++) {
						TedSet* sNodes = new TedSet;
						TedVarGroupMember* member = _vars.createMember(*owner);
						//member object will be cast to a simple TedVar object, therefore by looking at the
						//keys in the container we will not be able to see if the key was a member anymore.
						//we will need to retrieve this information through TedMan::checkVariables
						pManNew->_container[(*member)] = pair<unsigned int, TedSet*>(keyLevel, sNodes);
						keyLevel++;
					}
				}
			}
		map <TedNode*, TedNode*> mVisited;
		for(PrimaryOutputs::iterator it = _pos.begin(); it!= _pos.end(); it++) {
			TedNode* pNode = _Linearize_Rec(pManNew, it->second.Node(), mVisited);
			TedNodeRoot root(pNode,it->second.getWeight());
			root.setRegister(it->second.getRegister());
			pManNew->linkTedToPO(root, it->first);
		}
		return pManNew;
	}

	/** @brief Linearize recursively a TedNode*/
	TedNode* TedMan::_Linearize_Rec(TedMan* pManNew, TedNode* pNode, map <TedNode*, TedNode*> & mVisited) {
		TedNode* pNodeNew = NULL,* pKid = NULL,*pKidNew = NULL;
		wedge weight = 0, gcd = 0;
		TedRegister retime_count = 0;
		TedKids tKids;
		string strNewVar;
		map<TedNode*,TedNode*>::iterator it = mVisited.find(pNode);
		if(it != mVisited.end()) {
			return it->second;
		}
		if(pNode == TedNode::getOne())
			return pNode;
		for(int i = pNode->getKids()->highestOrder(); i >= 0; i--) {
			if(i == pNode->getKids()->highestOrder()) {
				pKid = pNode->getKidNode(i);
				weight = pNode->getKidWeight(i);
				retime_count = pNode->getKidRegister(i);
				pKidNew = _Linearize_Rec(pManNew, pKid, mVisited);
				tKids.insert(1, pKidNew, weight,retime_count);
			} else {
				if(pNode->getKids()->has(i)) {
					pKid = pNode->getKidNode(i);
					weight = pNode->getKidWeight(i);
					retime_count = pNode->getKidRegister(i);
					pKidNew = _Linearize_Rec(pManNew, pKid, mVisited);
					tKids.insert(0, pKidNew, weight,retime_count);
				}
				gcd = tKids.extractGCD();
				const TedVar* var = _vars.getIfExist(pNode->getName());
				if(var) {
					if(var->isOwner()) {
						const TedVarGroupOwner* owner = dynamic_cast<const TedVarGroupOwner*>(var);
						var = owner->getMember(i+1);
					} //if var is member or simple, its var itself
				} else {
					var = pNode->getVar();
				}
				pNodeNew = pManNew->_container.getNode(*var, tKids,pNode->getRegister());
				tKids.clear();
				tKids.insert(1, pNodeNew, gcd,TedRegister(0));
			}
		}
		mVisited[pNode] = pNodeNew;
		return pNodeNew;
	}

	void TedMan::cost(void) {
		ReportCost repcost;
		FixCost* rc = repcost.compute_cost(*this);
		unsigned int edge0 = 0;
		unsigned int edgeN = 0;
		costNaive(edge0,edgeN);
		cout << "|            TED                        |        DFG         |        Schedule       |            Gaut                   |" << endl;
		cout << "|---------------------------------------|--------------------|-----------------------|-----------------------------------|" << endl;
		cout << "| node | edge0 | edgeN | factor | width | nMUL | nADD | nSUB | Latency | rMPY | rADD | Muxes | Latency | Register | Area |" << endl;
		cout << "| ";
		cout.fill(' ');
		cout.width(4);
		cout << rc->nodes << " | ";
		cout.width(5);
		cout << edge0 << " | ";
		cout.width(5);
		cout << edgeN << " | ";
		cout.width(6);
		cout << rc->normB << " | ";
		cout.width(5);
		cout << rc->bitwidth << " | ";
		cout.width(4);
		cout << rc->stat.nMul << " | ";
		cout.width(4);
		cout << rc->stat.nAdd << " | ";
		cout.width(4);
		cout << rc->stat.nSub << " | ";
		cout.width(7);
		cout << rc->stat.latency << " | ";
		cout.width(4);
		cout << rc->stat.rMPY << " | ";
		cout.width(4);
		cout << rc->stat.rADD << " | ";
		cout.width(5);
		cout << rc->muxes << " | ";
		cout.width(7);
		cout << rc->latency << " | ";
		cout.width(8);
		cout << rc->registers << " | ";
		cout.width(4);
		cout << rc->area << " |" << endl;
		delete rc;
	}

	/** @brief Get the number of ADD edges and MPY edges*/
	void TedMan::costNaive(unsigned int& nbadd, unsigned int& nbmpy) {
		TedNode* pNode,* pKid;
		wedge weight;
		unsigned int index;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;

		/*creating the references*/
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					weight = _iterkids.getWeight();
					if(index==0) {nbadd++; }
					if(index==1&&pKid!=TedNode::getOne()) {nbmpy++; }
					if(weight!=-1 && weight!=1) {nbmpy++; }
				}
			}
		}
		//cout<<"#ADD="<<nbadd<<" #MPY="<<nbmpy<<endl;
	}

	/** @brief Get the number of factor candidates*/
	unsigned int TedMan::getNumOfCandidates(void) {
		TedNode* pNode = NULL,* pKid = NULL;
		//unsigned int index = 0;
		TedContainer::iterator msp;
		map <TedNode*, unsigned int> mRefs;
		map <TedNode*, unsigned int>::iterator mp;
		set <TedNode*>::iterator sp;
		unsigned int ret = 0;
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					pKid = _iterkids.Node<TedNode>();
					mRefs[pKid] +=1;
				}
			}
		}
		for(mp = mRefs.begin(); mp != mRefs.end(); mp++) {
			if(mp->second <= 1) {
				continue;
			} else if(mp->first == TedNode::getOne()|| mp->first->isBasic()) {
				continue;
			} else {
				ret = ret + mp->second -1;
			}
		}
		return ret;
	}

	void TedMan::getParentNodes(TedNode* candidate, vector<TedNode*> &vparentnodes) {
		TedNode* pNode = NULL,*pKid = NULL;
		//unsigned int index = 0;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;

		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					pKid = _iterkids.Node<TedNode>();
					if(pKid==candidate) {
						vparentnodes.push_back(pNode);
					}

				}
			}
		}
	}

	void TedMan::searchDecompNode(vector<pair<TedNode*, TedNode*> > & list_decomp_node) {
		TedNode* pNode = NULL,* pKid = NULL;
		wedge weight = 0;
		unsigned int index = 0;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;
		map <TedNode*, vector<pair <TedNode*, pair< int, int> > > > linkofnodes;
		map <TedNode*, vector<pair <TedNode*, pair< int, int> > > >::iterator it_linkofnodes;
		vector<pair< TedNode*, pair<int,int> > >::iterator it_parent;
		vector<pair<TedNode*, TedNode*> > it_vector;
		list_decomp_node.clear();	 //nettoyage de la liste
		/*creation d'une liste de noeud avec enfants pour references*/
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				//pour chacun des enfants je met le noeud et dans la liste associs ses enfants
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					weight = _iterkids.getWeight();
					linkofnodes[pKid].push_back(pair<TedNode*, pair<int,int> >(pNode, pair<int,int>(index,weight)));
				}
			}
		}
		//	it_vector=arg_candidate_list.begin();
		/* pour chacun des noeud de  la liste*/
		string name_first_child;
		bool samename;
		for(it_linkofnodes=linkofnodes.begin();it_linkofnodes!=linkofnodes.end();it_linkofnodes++) {
			if(it_linkofnodes->first==TedNode::getOne())
				continue;
			if(it_linkofnodes->second.size()<2)
				continue;
			samename=true;
			name_first_child=it_linkofnodes->second.begin()->first->getName();
			//lister les parents
			for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++) {
				if(name_first_child!=it_parent->first->getName()) {
					samename=false;
					break;
				}
			}
			if(samename==false) {
				cout<<"------------------------------------------"<<endl;
				cout<<it_linkofnodes->first->getName()<<" "<<it_linkofnodes->first<<" is referenced by"<<endl;

				for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++) {
					cout<<it_parent->first->getName()<<" "<< it_parent->first<<" with a "<<it_parent->second.first<<"-edge"<<endl;
				}
			}
		}
		pNode=NULL;
	}

	void TedMan::collectParents(void) {
		TedParents parents;
		preOrderDFS(parents,&TedParents::collect);
		parents.numParents(TedNode::getOne());
	}

	void TedMan::computeBitwidth(void) {
		TedBitwidth tedwidth(this);
		tedwidth.compute();

	}

	Bitwidth* TedMan::getMaxBitwidth(void) {
		Bitwidth* retval = NULL;
		for(PrimaryOutputs::iterator it = _pos.begin(); it!= _pos.end(); it++) {
			Bitwidth* prec = it->second.getBitwidth();
			if(!retval) {
				retval = prec->clone();
			} else if(retval->isLessThan(*prec)) {
				retval->copy(*prec);
			}
		}
		return retval;
	}

	void TedMan::moveSupportTo(TedMan* destiny) {
		//O(n+n.pt+n.st)
		Mark<TedNode>::newMark();
		Mark<TedVar>::newMark();
		vector<TedNode*> vNodes;
		vector<string> terms;
		for(PrimaryOutputs::iterator it = _pos.begin(); it!=_pos.end(); it++) {
			if(it->second.getType() ==TedNodeRoot::PT || it->second.getType() ==TedNodeRoot::ST) {
				it->second.Node()->_collectDFS_rec(vNodes);
				destiny->linkTedToPO(it->second,it->first);
				terms.push_back(it->first);
			}
		}
		//unlink the POs from PT or ST
		while(!terms.empty()) {
			_pos.erase(terms.back());
			terms.pop_back();
		}
		BinaryThreadedTree<int,TedVar> bt;
		for(TedContainer::iterator it = _container.begin(); it != _container.end(); it++) {
			TedVar* var = _vars.getIfExist(it->first.getName());
			if(var && var->visited.isMarked())
				bt.insert(-1*it->second.first, it->first);
		}
		BinaryThreadedTree<int,TedVar>::inorder_iterator iot = bt.begin();
		BinaryThreadedTree<int,TedVar>::inorder_iterator iot_end = bt.end();
		for(;iot!=iot_end;++iot) {
			destiny->_container.registerVarAtTop((*iot)->getData());
			TedSet* nodes = _container[(*iot)->getData()].second;
			TedSet tempset =*nodes;
			for(TedSet::iterator jt = tempset.begin(); jt != tempset.end(); jt++) {
				if((*jt)->visited.isMarked()) {
					//if a node is shared between the root TED and its decomposed TED, this will
					//remove the node from the root TED container. Any method that traverses the root TED
					//container will work, but the node won't be physically registered there.
					destiny->_container[(*iot)->getData()].second->insert(*jt);
					nodes->erase(*jt);
				}
			}
			if(nodes->empty()) {
				_container.removeLevel((*iot)->getData());
			}
		}
	}

	TedMan* TedMan::decomposeAll(bool force) {
		TedCompareCost* compare = new MinLatency();
		TedMan* pTedMan = decomposeAll(compare,force);
		delete compare;
		return pTedMan;
	}

	TedMan* TedMan::decomposeAll(TedCompareCost* compare,bool force) {
		TedMan* pTedMan = duplicate();
		TedMan* support = new TedMan();
		bool continueDecomposing = !pTedMan->decompose(force);
		size_t watch_dog = pTedMan->_container.nodeCount();
		while(continueDecomposing && watch_dog-- > 0) {
			pTedMan->moveSupportTo(support);
			TedOrderStrategy* orderst = new TedOrderSwap(pTedMan);
			TedMan* newTedMan = pTedMan->permuteOrdering(orderst, compare,true);
			//TedMan* newTedMan = pTedMan->siftAll(orderst, compare);
			delete orderst;
			if(newTedMan!=pTedMan) {
				delete pTedMan;
				pTedMan = newTedMan;
			}
			continueDecomposing = !pTedMan->decompose(force);
		}
		support->moveSupportTo(pTedMan);
		delete support;
		return pTedMan;
	}


	bool TedMan::verify(const string& output1, const string& output2) {
		PrimaryOutputs::iterator out1 = _pos.find(output1);
		PrimaryOutputs::iterator out2 = _pos.find(output2);
		return (out1 != _pos.end() && out2 != _pos.end() && out1->second.Node() == out2->second.Node());
	}

	bool TedMan::regressionTest(stringstream& message) {
		bool ret = true;
		int passed = 0;
		int failed = 0;
		stringstream annotateFail;
		stringstream annotatePass;
		for(PrimaryOutputs::iterator it = _pos.begin(); it != _pos.end(); it++) {
			if((it->second).getWeight()!=0) {
				failed++;
				ret = false;
				annotateFail << ":" << it->first;
			} else {
				passed++;
				annotatePass << ":" << it->first;
			}
		}
		int total = passed+failed;
		if(total==passed) {
			message << ",all(" << passed << "/" << total << "),none,";
		} else if(total==failed) {
			message << ",none,all(" << failed << "/" << total << "),";
		} else {
			message << "," << passed << "/" << total << " " << annotatePass.str()<< ",";
			message << "," << failed << "/" << total << " " << annotateFail.str()<< ",";
		}
		return ret;
	}

	void TedMan::foreach(void(TedVar::*functor)(void)const) {
		TedContainerOrder tnodesOrdered = _container.order();
		for(TedContainerOrder::const_iterator it = tnodesOrdered.begin(); it!= tnodesOrdered.end(); it++) {
			TedVar* pvar  = checkVariables(it->second.first->getName());
			(pvar->*functor)();
		}
	}

	void TedMan::foreach(void(TedNode::*functor)(void)) {
		for(TedContainer::iterator p = _container.begin(); p != _container.end(); p++) {
			for(TedSet::iterator sp = p->second.second->begin(); sp != p->second.second->end(); sp++) {
				((*sp)->*functor)();
			}
		}
	}

	void TedMan::listOutputs(void) {
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			cout << p->first;
			switch(p->second.getType()) {
			case TedNodeRoot::PO:
				cout << " PrimaryOutput ";
				break;
			case TedNodeRoot::PT:
				cout << " ProductTerm ";
				break;
			case TedNodeRoot::ST:
				cout << " SumTerm ";
				break;
			case TedNodeRoot::BC:
				cout << " BottomCut ";
				break;
			case TedNodeRoot::TC:
				cout << " TopCut ";
				break;
			case TedNodeRoot::EV:
				cout << " ExpectedValue ";
				break;
			case TedNodeRoot::ET:
				cout << " ErrorTED ";
				break;
			default:
				cout << " Unkwnown ";
				break;
			}
			//TedNode* rootNode = p->second.Node();
			Mark<TedNode>::newMark();
			vector <TedNode*> vNodes;
			p->second.Node()->_collectDFS_rec(vNodes);
			cout << vNodes.size()<< endl;
		}
	}

	void TedMan::recoverRegisterVars(void) {
		TedParents parents;
		preOrderDFS(parents, &TedParents::collect);
		for(TedContainer::iterator p = _container.begin(); p != _container.end(); p++) {
			TedSet::iterator sp_last = p->second.second->begin();
			TedSet::iterator sp = p->second.second->begin();
			while (sp != p->second.second->end()) {
				sp_last = sp;
				sp_last++;
				TedNode* oldnode  = (*sp);
				if (oldnode->getRegister()>0 && !oldnode->getVar()->getBase()) {
					TedVar* rvar = _vars.createRetimedVar(*oldnode->getVar(),oldnode->getRegister());
					TedNode* newnode = _container.getNode(*rvar,*oldnode->getKids(),oldnode->getRegister());
					updateAllParents(oldnode,newnode,parents);
					if (oldnode!=newnode) {
						p->second.second->erase(sp);
						delete oldnode;
					}
				}
				sp = sp_last;
			}
			if (p->second.second->empty()) {
				//empty variable container
			}
		}
	}

#if 0
	//
	// MOVED to TedRetime and then inlined in TedMan.inl
	//
	void TedMan::retimeForward(void) {
		TedContainerOrder tnodesOrdered = _container.order();
		// from bottom register to top register
		TedRetiming retime(this);
		for(TedContainerOrder::reverse_iterator it = tnodesOrdered.rbegin(); it!= tnodesOrdered.rend(); it++) {
			TedVar* pvar  = checkVariables(it->second.first->getName());
			retime.up(*pvar);
		}
	}

	void TedMan::retimeBackward(void) {
		TedContainerOrder tnodesOrdered = _container.order();
		// from bottom register to top register
		TedRetiming retime(this);
		for(TedContainerOrder::iterator it = tnodesOrdered.begin(); it!= tnodesOrdered.end(); it++) {
			TedVar* pvar  = checkVariables(it->second.first->getName());
			retime.down(*pvar);
		}
	}
#endif

	void TedMan::deriveSchedule(void) {
		bool isDecomposed = false;
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			if(p->second.getType() == TedNodeRoot::PT || p->second.getType() == TedNodeRoot::ST) {
				isDecomposed = true;
				break;
			}
		}
		if(!isDecomposed) {
			cout << "TED must be decomposed first in order to derive its schedule" << endl;
			return;
		}
		for(PrimaryOutputs::iterator p = _pos.begin(); p != _pos.end(); p++) {
			if(p->second.getType() == TedNodeRoot::PT || p->second.getType() == TedNodeRoot::ST) {
				isDecomposed = true;
				break;
			}
		}
	}

	void TedMan::purge_transitory_container(void) {
		Mark<TedNode>::newMark();
		Mark<TedKids>::newMark();
		this->getContainer().update_TMark();
		TedKids::purge_transitory_container();
		ATedNode::purge_transitory_container();
	}

#ifdef FUNCTIONS_DEPRECATED
	/** @brief Build a Ted From DFG*/
	TedMan* TedMan::buildTedFromDFG(TedMan* pTedMan, DfgMan* pDfgMan) {
		assert(ATedNode::checkContainer(pTedMan->getContainer()));
		map <DfgNode*, ATedNode*> mVisited;
		for(map <string, DfgNode*>::iterator p = pDfgMan->_mPos.begin(); p != pDfgMan->_mPos.end(); p++) {
			ATedNode* pANode = _BuildTedFromDfg_Rec(p->second, mVisited, pTedMan);
			TedNodeRoot pin(pTedMan->normalize(pANode));
			pTedMan->linkTedToPO(pin.Node(), pin.getWeight(), p->first);
		}
		return pTedMan;
	}

	ATedNode* _BuildTedFromDfg_Rec(DfgNode* pDfgNode, map<DfgNode*,ATedNode*> & mVisited, TedMan* pTedMan) {
		ATedNode* pANode = NULL,* pELeft=NULL,* pERight=NULL;
		if(mVisited.find(pDfgNode)!= mVisited.end()) {
			return mVisited[pDfgNode]->duplicateRecursive();
		}
		if(pDfgNode->isConst()) {
			return new ETedNode(pDfgNode->getConst());
		} else if(pDfgNode->isPI()|| pDfgNode->isVarConst()) {
			//not needed
			//TedVar* pvar = NULL;
			//	//pvar = pTedMan->checkVariables(pDfgNode->getName());			   -----\
			//pvar = TedVarMan::instance().createVar(TedVar(pDfgNode->getName()));	 <--/
#ifdef CAUSE_BUG_102
			if(pvar->isOwner()) {
				return new ETedNode(dynamic_cast<TedVarGroupOwner*>(pvar)->getMember()->getName());
			}
#endif
			return new ETedNode(pDfgNode->getName());
		} else {
			assert(pDfgNode->isOp());
		}
		pELeft = _BuildTedFromDfg_Rec(pDfgNode->getLeft(), mVisited, pTedMan);
		pERight= _BuildTedFromDfg_Rec(pDfgNode->getRight(), mVisited, pTedMan);
		if(pDfgNode->isAdd()) {
			pANode = pELeft->add(pERight);
		} else if(pDfgNode->isSub()) {
			// I believe this would have require duplication
			pANode = pELeft->sub(pERight);
		} else if(pDfgNode->isMul()) {
			pANode = pELeft->mul(pERight);
		} else if(pDfgNode->isLSH()) {
			assert(pERight->isConst());
			int val = 2;
			int exponent = pERight->getConst();
			for(int i = 1; i < exponent; i++) {
				val*= 2;
			}
			pERight->setConst(val);
			pANode = pELeft->mul(pERight);
		}
		return pANode;
	}

	void  _UpdateLowerUpperBoundSet(TedMan* pMan, multiset <_UpLowBound>* sTeds, int* minUpb) {
		int upb, lowb;
		_UpLowBound pin;
		multiset <_UpLowBound>::iterator ppin;
		pMan->getUpLowMulBound(&upb, &lowb);
		if(lowb >=*minUpb) {
			return;
		} else if(upb >=*minUpb) {
			pin.lowb=lowb;
			pin.upb = upb;
			pin.pTedMan = pMan->duplicate();
			sTeds->insert(pin);
		} else {
			*minUpb = upb;
			pin.lowb = upb;
			ppin = sTeds->insert(pin);
			sTeds->erase(ppin, sTeds->end());
			pin.lowb=lowb;
			pin.upb = upb;
			pin.pTedMan = pMan->duplicate();
			sTeds->insert(pin);
		}
	}
#endif

}
