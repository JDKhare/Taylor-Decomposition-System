/*
 * =====================================================================================
 *
 *        Filename:  Tds.cc
 *     Description:  Tds class
 *         Created:  04/19/2007 02:04:40 AM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 202                                          $: Revision of last commit
 *  $Author:: daniel@linux                                   $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include <string>
#include <ctime>
#include <algorithm>
#ifdef _MSC_VER
# define sprintf sprintf_s
#endif
using namespace std;

#include "Tds.h"
#include "Environment.h"

#include "TedMan.h"
#include "ETedNode.h"
using namespace ted;

#include "DfgMan.h"
#include "DfgNode.h"
using dfg::DfgNode;
using dfg::DfgMan;

#include "util.h"
using util::Util;

#include "pnode.h"
#include "poly.h"
#include "pman.h"
using polyparser::PolyParser;
using polyparser::PNode;
using polyparser::PMan;

#include "Ntk.h"
#include "Node.h"
using namespace network;

#include "Convert.h"
using namespace convert;

#include "matrice.h"

namespace tds {

	Netlist* Tds::_ntlExtracTed = NULL;

	struct DeleteObject {
		template <typename T> void operator()(const T* ptr)const {
			delete ptr;
		}
	};


	Tds::Tds(void) {
		_pTedMan = new TedMan();
		_pDfgMan = NULL;
		_reconstructNTL = false;
		_manual_purge = false;
		Environment::setDefault();
		CmdLoadEnv(0,NULL);
		Environment::parseSystem();
	}

	Tds::~Tds(void) {
		delete _pTedMan;
		if(_pDfgMan != NULL)
			delete _pDfgMan;
		for_each(_lHistory.begin(), _lHistory.end(), DeleteObject());
		CmdSaveEnv(0,NULL);
		Environment::clean();
	}

	Netlist* Tds::getCurrentNetlist(void) {
		if(_lHistory.empty())
			return NULL;
		return _lHistory.front();
	}

	Netlist* Tds::getPreviousNetlist(unsigned int index) {
		list<Netlist*>::iterator p;
		unsigned int i;

		assert(index < _lHistory.size());
		for(p = _lHistory.begin(), i = 0; p != _lHistory.end()&& i < index; p++, i++) {
			;
		}
		return(*p);
	}

	void Tds::clearHistory(void) {
		Netlist* nl;
		while(!_lHistory.empty()) {
			nl = _lHistory.back();
			_lHistory.pop_back();
			delete nl;
		}
	}

	void Tds::pushToHistory(Netlist* nl) {
		_lHistory.push_front(nl);
		if(_lHistory.size()> _history_size) {
			nl = _lHistory.back();
			_lHistory.pop_back();
			delete nl;
		}
	}

	void Tds::setHistorySize(unsigned int size) {
		Netlist* nl;
		while(_lHistory.size()> size) {
			nl = _lHistory.back();
			_lHistory.pop_back();
			delete nl;
		}
		_history_size= size;
	}

	TedMan* Tds::getTedMan(void) {
		return _pTedMan;
	}

	void Tds::parsePoly(char* p, bool intoConstant) {
		try {
			PolyParser poly(p);
			string prefix = (intoConstant) ? Environment::getStr(Environment::CONST_PREFIX): "";
			PMan pman(_pTedMan,prefix);
			ATedNode* pENode = pman.convertTo<ETedNode>(poly.getPostfix());
			TedNodeRoot pin  = _pTedMan->normalize(pENode);
			string polyName = poly.getName();
			int loopback = 0;
			int bracketBegin = polyName.find_first_of("[");
			int bracketEnd = polyName.find_last_of("]");
			if (bracketBegin +1 < bracketEnd && bracketEnd == polyName.length()-1) {
				string loopindex = polyName.substr(bracketBegin+1,bracketEnd-bracketBegin-1);
				std::pair<bool,wedge> loop = Util::atoi(loopindex.c_str());
				if (loop.first) {
					loopback = loop.second;
					polyName.erase(bracketBegin);
				}
			}
			if (_pTedMan->hasVar(polyName) && loopback==0) {
				cout << "Info: possible infinite recursion has been detected" << endl;
				cout << "      output variable \"" << polyName << "\" might depend on itself" <<  endl;
			}
			_pTedMan->linkTedToPO(pin,polyName);
			if(_pDfgMan != NULL) {
				delete _pDfgMan;
				_pDfgMan = NULL;
			}
#if 0
			if(pman.hasRegisters()) {
				TedMan* pManNew = _pTedMan->reorderVariablesWithRegisters(false);
				if (pManNew && pManNew != _pTedMan) {
					delete _pTedMan;
					_pTedMan = pManNew;
				}
			}
#endif
		} catch(const string& _error) {
			throw(_error);
		} catch(...) {
			throw(string("03004. Parsing the polynomial expression ")+ p);
		}
	}

	void Tds::buildTransform(Matrice* m) {
		ATedNode::setContainer(_pTedMan->getContainer());
		ATedNode* pvariable,* tt,* pconst;
		string s;
		for(unsigned int row = 0; row < m->nb_lignes(); row++) {
			for(unsigned col = 0; col < m->nb_colones(); col++) {
				if((*m)(row,col).name != "1" &&(*m)(row,col).name != "0") {
					_pTedMan->registerVar((*m)(row,col).name);
				}
			}
		}
		//TedNode::setConstLimitNow();
		//go on with real variable
		for(unsigned col = 0; col < m->nb_colones(); col++) {
			char Xstring[10];
			sprintf(Xstring,"X%d",col);
			string var(Xstring);
			_pTedMan->registerVar(var);
		}
		for(unsigned int row=0; row<m->nb_lignes();row++) {
			tt=new ETedNode(0);
			for(unsigned int col=0;col<m->nb_colones();col++) {
				char Xstring[10];
				sprintf(Xstring,"X%d",col);
				pvariable =new ETedNode(Xstring);
				//TEDNode* pcoeff;
				if((*m)(row,col).name != "1" &&(*m)(row,col).name != "0") {
					pconst = new ETedNode((*m)(row,col).name.c_str()); //creation of the constant node
					pvariable=pvariable->mul(pconst);
				}
				if(!(*m)(row,col).signe)pvariable = pvariable->mul(new ETedNode(-1));
				if((*m)(row,col).name != "0")tt=tt->add(pvariable);
			}
			s = "Y" + Util::itoa(row);
			_pTedMan->linkTedToPO(_pTedMan->normalize(tt), s);
		}
		_pTedMan->setConstLimit(m->nb_colones());
	}

	void Tds::buildTransform(CMatrice* m) {
		ATedNode::setContainer(_pTedMan->getContainer());
		ATedNode* pvariable,* tt,* pconst;
		string s;
		for(unsigned int row = 0; row < m->nb_lignes(); row++) {
			for(unsigned col = 0; col < m->nb_colones(); col++) {
				if((*m)(row,col).name != "1" &&(*m)(row,col).name != "0") {
					_pTedMan->registerVar((*m)(row,col).name.c_str());
				}
			}
		}
		// TEDNode::setConstLimitNow();
		//go on with real variable
		for(unsigned col = 0; col < m->nb_colones(); col++) {
			char Xstring[10];
			sprintf(Xstring,"X%d",col);
			string var(Xstring);
			_pTedMan->registerVar(var);
		}
		for(unsigned int row=0; row<m->nb_lignes();row++) {
			tt=new ETedNode(0);
			for(unsigned int col=0;col<m->nb_colones();col++) {
				char Xstring[10];
				sprintf(Xstring,"X%d",col);
				pvariable =new ETedNode(Xstring);
				//TEDNode* pcoeff;
				if((*m)(row,col).name != "1" &&(*m)(row,col).name != "0") {
					pconst = new ETedNode((*m)(row,col).name.c_str()); //creation of the constant node
					pvariable=pvariable->mul(pconst);
				}
				if(!(*m)(row,col).signe)pvariable = pvariable->mul(new ETedNode(-1));
				if((*m)(row,col).name != "0")tt=tt->add(pvariable);
			}
			s = "Y" + Util::itoa(row);
			_pTedMan->linkTedToPO(_pTedMan->normalize(tt), s);
		}
		_pTedMan->setConstLimit(m->nb_colones());
	}

	void Tds::writeTransformToC(CMatrice* cm, char* file) {
		vector <string> vToks, vInOuts;
		set    <string> sInputs, sOutputs;
		set    <string>::iterator p;
		vector <string>* vExprs = new vector <string>;
		printTransform(cm, vExprs);
		for(unsigned int i = 0; i < vExprs->size(); i++) {
			vToks=Util::split(vExprs->at(i), " +-*=\n\t");

			sOutputs.insert(vToks[0]);
			for(unsigned int j = 1; j < vToks.size(); j++) {
				sInputs.insert(vToks[j]);
			}
		}
		ofstream ofs(file);
		for(p = sInputs.begin(); p != sInputs.end(); p++) {
			vInOuts.push_back(*p);
		}
		for(p = sOutputs.begin(); p != sOutputs.end(); p++) {
			vInOuts.push_back(*p);
		}
		ofs <<"int main(";
		for(unsigned int i = 0; i < vInOuts.size(); i++) {
			ofs <<((i==0) ?"":", ")<< vInOuts[i];
		}
		ofs << ") {\n";
		for(unsigned int i = 0; i < vExprs->size(); i++) {
			ofs << "\tt"<< vExprs->at(i)<<";\n";
		}
		ofs <<"\treturn 0;\n}";
		ofs.close();
		delete vExprs;
	}

	void Tds::writeTransformToC(Matrice* m, char* file) {
		vector <string> vToks, vInOuts;
		set    <string> sInputs, sOutputs;
		set    <string>::iterator p;
		vector <string>* vExprs = new vector <string>;
		printTransform(m, vExprs);
		for(unsigned int i = 0; i < vExprs->size(); i++) {
			vToks=Util::split(vExprs->at(i), " +-*=\n\t");
			sOutputs.insert(vToks[0]);
			for(unsigned int j = 1; j < vToks.size(); j++) {
				sInputs.insert(vToks[j]);
			}
		}
		ofstream ofs(file);
		for(p = sInputs.begin(); p != sInputs.end(); p++) {
			vInOuts.push_back(*p);
		}
		for(p = sOutputs.begin(); p != sOutputs.end(); p++) {
			vInOuts.push_back(*p);
		}
		ofs <<"int main(";
		for(p = sInputs.begin(); p != sInputs.end(); p++) {
			ofs <<((p==sInputs.begin()) ?"int ":", int ")<<*p;
		}
		for(p = sOutputs.begin(); p != sOutputs.end(); p++) {
			ofs << ", int* " <<*p;
		}
		ofs << ") {\n";
		for(unsigned int i = 0; i < vExprs->size(); i++) {
			ofs << "\t"<< "* "<< vExprs->at(i)<<";\n";
		}
		ofs <<"\treturn 0;\n}";
		ofs.close();
		delete vExprs;
	}




	void Tds::printTransform(Matrice* m, vector <string>* vExprs) {
		string s, c;
		unsigned int nMul=0, nAdd=0, nSub=0;
		for(unsigned int i =0; i <m->nb_lignes();i ++) {
			s = "\tY" + Util::itoa(i)+ " = ";
			for(unsigned int j=0;j<m->nb_colones();j++) {
				if(m->el[i][j].name == "1") {
					c = "";
				}  else {
					c = m->el[i][j].name;
				}
				if(m->el[i][j].signe) {
					s = s +(j==0?"":"+")+ c.c_str()+(c==""?"":"*")+ "X"+Util::itoa(j);
				} else {
					s = s +"-"+c.c_str()+(c==""?"":"*")+ "X"+Util::itoa(j);
				}
			}
			if(vExprs== NULL) {
				cout << s << endl;
			} else {
				vExprs->push_back(s);
			}
			for(unsigned int i = 0; i < s.size(); i++) {
				switch(s[i]) {
				case '*':
					nMul ++;
					break;
				case '+':
					nAdd ++;
					break;
				case '-':
					nSub ++;
					break;
				default:
					break;
				}
			}
		}
		printf("\n\tnMul=%5d, nAdd=%5d, nSub=%5d\n", nMul, nAdd, nSub);
	}



	void Tds::printTransform(CMatrice* m, vector <string>* vExprs) {
		string s, c;
		unsigned int nMul=0, nAdd=0, nSub=0;
		for(unsigned int i =0; i <m->nb_lignes();i ++) {
			s = "\tY" + Util::itoa(i)+ " = ";
			for(unsigned int j=0;j<m->nb_colones();j++) {
				if(m->el[i][j].name == "1") {
					c = "";
				}  else {
					c = m->el[i][j].name;
				}
				if(m->el[i][j].signe) {
					s = s +(j==0?"":"+")+ c.c_str()+(c==""?"":"*")+ "X"+Util::itoa(j);
				} else {
					s = s +"-"+c.c_str()+(c==""?"":"*")+ "X"+Util::itoa(j);
				}
			}
			if(vExprs== NULL) {
				cout << s << endl;
			} else {
				vExprs->push_back(s);
			}
			for(unsigned int i = 0; i < s.size(); i++) {
				switch(s[i]) {
				case '*':
					nMul ++;
					break;
				case '+':
					nAdd ++;
					break;
				case '-':
					nSub ++;
					break;
				default:
					break;
				}
			}
		}
		printf("\n\tnMul=%5d, nAdd=%5d, nSub=%5d\n", nMul, nAdd, nSub);
	}

	/**
	 * @brief Tds::useShifter converts all the constant values into a series of shifters.
	 * @return the New TED Manager with the constant values replaced by shifters.
	 * @note
	 * 	the constant variable generated MUST be placed on top of all other variables in the TED,
	 * 	otherwise remapShift will complain!!.
	 */
	TedMan* Tds::useShifter(void) {
		if(_pTedMan == NULL) {
			return NULL;
		}
		list<TedVar*> vVars;
		_pTedMan->getVars(vVars);
		string shiftName = Environment::getStr(Environment::CONST_PREFIX);
		shiftName += "2";
		wedge value = 2;
		TedVar* var = TedVarMan::instance().createVar(shiftName,value);
		vVars.push_front(var);
		Convert fromT(_pTedMan);
		DfgMan* pDfgMan = fromT.ted2dfgFactor();
		DfgMan* pDfgManNew = pDfgMan->useShifter(shiftName);
		delete pDfgMan;
		//TedMan* pTedManNew = new TedMan();
		//Correct variable order is:  ShiftName + <old variable order>
		//pTedManNew->registerVar(shiftName,2);
		//pTedManNew->registerVars(vVars);
		Convert fromD(pDfgManNew);
		TedMan* pTedManNew = fromD.dfg2ted(vVars);
		delete pDfgManNew;
		//free((void*)v);
		return pTedManNew;
	}

	void Tds::setShell(Shell* s) {
		_MainShell = s;
	}

#if 0
	void _FillNetlistWithDecomposedNodes(Netlist* oNl, DfgMan* pDfgMan, map <string, DfgNode*>* mpPoDfgNode, map <string, Node*> & mDecomposedNodes, bool bChangeConstant) {
		assert(oNl);
		assert(pDfgMan);
		assert(mpPoDfgNode);
		DfgNode* pBNode;
		Node* pNode;
		vector <DfgNode*> vBNodes;
		map <DfgNode*, Node*> mBin2Nodes;
		map <DfgNode*, string> mBin2Name;
		map <DfgNode*, string>::iterator it;
		map <string, Node*> mPiNodes;
		for(map <string, DfgNode*>::iterator p = mpPoDfgNode->begin(); p != mpPoDfgNode->end(); p++) {
			if(p->second) {
				mBin2Name[p->second] = p->first;
			}
		}
		pDfgMan->collectDFS(vBNodes);
		for(unsigned int i = 0; i < vBNodes.size(); i++) {
			pBNode = vBNodes[i];
			if(pBNode->isPI()|| pBNode->isVarConst()) {
				//pNode =  new Node(pBNode->getName(), PI);
				const char* p = Environment::getStr(Environment::CONST_PREFIX).c_str();
				int   len = strlen(p);
				pair <bool, long> tt;
				if(bChangeConstant && strncmp(p, pBNode->getName(), len) == 0) {
					tt=Util::atoi(pBNode->getName()+len);
					if(tt.first) {
						pNode = new Node(Util::itoa(tt.second).c_str(), PI);
					} else {
						pNode =  new Node(pBNode->getName(), PI);
					}
				} else {
					pNode =  new Node(pBNode->getName(), PI);
				}
			} else if(pBNode->isConst()) {
				string constname = Environment::getStr(Environment::CONST_PREFIX);
				constname += Util::itoa(pBNode->getConst());
				pNode = new Node(constname.c_str(), PI);
			} else {
				it = mBin2Name.find(pBNode);
				if(it != mBin2Name.end()) {
					pNode = new Node(it->second.c_str(), INT);
				} else {
					pNode = new Node(string("T_"+Util::itoa(i)).c_str(), INT);
				}
				Node* leftNode = mBin2Nodes[pBNode->getLeft()];
				assert(leftNode);
				Node* rightNode = mBin2Nodes[pBNode->getRight()];
				assert(rightNode);
				pNode->addFanin(leftNode);
				pNode->addFanin(rightNode);
				if(pBNode->isMul())pNode->setFunc(FUNC_MUL);
				else if(pBNode->isAdd())pNode->setFunc(FUNC_ADD);
				else if(pBNode->isSub())pNode->setFunc(FUNC_SUB);
				else assert(0);
			}
#if 0
			if(pBNode->getLeft() == NULL && pBNode->getRight() == NULL) {
				map <string, Node*>::iterator p = mPiNodes.find(pBNode->var);
				if(p != mPiNodes.end()) {
					pNode = p->second;
				} else {
					pNode = new Node(pBNode->var, INT);
					mPiNodes[pBNode->var] =pNode;
				}
			} else {
				it = mBin2Name.find(pBNode);
				if(it != mBin2Name.end()) {
					pNode = new Node(it->second, INT);
				} else {
					pNode = new Node(string("T_"+Util::itoa(i)), INT);
				}
				pNode->addFanin(mBin2Nodes[pBNode->getLeft()]);
				pNode->addFanin(mBin2Nodes[pBNode->getRight()]);
				if(pBNode->isMul())pNode->SetFuncIndex(GC_MUL);
				else if(pBNode->isAdd())pNode->SetFuncIndex(GC_ADD);
				else if(pBNode->isSub())pNode->SetFuncIndex(GC_SUB);
				else assert(0);
			}
#endif
			oNl->add(pNode);
			mBin2Nodes[pBNode] = pNode;
		}
		for(map <string, DfgNode*>::iterator p = mpPoDfgNode->begin(); p != mpPoDfgNode->end(); p++) {
			pBNode = p->second;
			if(NULL == pBNode) {
				pNode = NULL;
			} else {
				pNode = mBin2Nodes[pBNode];
			}
			mDecomposedNodes[p->first] = pNode;
		}
	}

	Netlist* Tds::dfg2nl(Netlist* iNl, DfgMan* pDfgMan, bool bChangeConstant) {
		if(!iNl)
			throw(string("03007. There are no netlists in the design"));
		if(!pDfgMan)
			throw(string("03008. No associated DFG"));

		Node* pNode,* pFanin,* pNewNode,* pNewFanin;
		unsigned int j;

		Netlist* oNl = new Netlist((string(iNl->getName())+ "_2ntk").c_str());
		map<string, DfgNode*>* mpPoDfgNode = pDfgMan->getPos();
		map<string, Node*> mDecomposedNodes;
		_FillNetlistWithDecomposedNodes(oNl, pDfgMan, mpPoDfgNode, mDecomposedNodes, bChangeConstant);
		vector <Node*> vNodes;
		vNodes = iNl->collectDFS(vNodes);
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			pNode = vNodes[i];
			cout << pNode << " name=" << pNode->getName()<< " type=" << pNode->getType()<< " func="<< pNode->func()<< endl;
			if(pNode->getData()!= NULL)
				continue;
			pNewNode = oNl->get(pNode->getName());
			if(pNewNode == NULL) {
				pNewNode = new Node(pNode->getName(), pNode->getType());
				cout << pNewNode << " name=" << pNewNode->getName()<< " type=" << pNewNode->getType()<< " func="<< pNewNode->func()<< endl;
				oNl->add(pNewNode);
			} else {
				cout << pNewNode << " name=" << pNewNode->getName()<< " type=" << pNewNode->getType()<< " func="<< pNewNode->func()<< endl;
				pNewNode->setType(pNode->getType());
			}
			pNewNode->setFunc(vNodes[i]->func());
			FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
				pNewFanin = oNl->get(pFanin->getName());
				//if(pNewFanin != NULL)continue;
				cout << "  " << j << " fanin=" << pFanin << " name=" << pFanin->getName()<< " type=" << pFanin->getType()<< " func="<< pFanin->func()<< endl;
				cout << "  " << "  pNewFanin=" << pNewFanin << endl;
				if(pNewFanin == NULL) {
					assert(mDecomposedNodes.find(pFanin->getName())!= mDecomposedNodes.end());
					pNewFanin = mDecomposedNodes[pFanin->getName()];
					oNl->add(pNewFanin);
				}
				assert(pNewFanin != NULL);
				pNewNode->addFanin(pNewFanin);
			}
		}
		return oNl;
	}
#endif

	//
	//
	//Netlist* Tds::collapse(Netlist* iNl) {
	//    Netlist* oNl;
	//    vector <Node*> vNodes;
	//    set    <Node*> vFanins;
	//    set    <Node*>::iterator p;
	//    TedNodeRoot  pin;
	//
	//    Node* pNode,* pNewNode,* pFanin,* pNewFanin;
	//    unsigned int i, j;
	//    ETedNode* pTNode;
	//    TedNode* pTedNode;
	//
	//    map<string,uint> inputs;
	//    map<string,uint> consts;
	//    map<Node*, ETedNode*> mNode2TEDNodes;
	//    map<Node*, set <Node*> > msFanins;
	//    set<Node*> sFanins;
	//    pair <bool, int> pa;
	//
	//    const char** ppVars;
	//
	//    if(_pTedMan != NULL)delete(_pTedMan);
	//    _pTedMan = new TedMan();
	//    if(_pDfgMan != NULL)delete(_pDfgMan);
	//    _pDfgMan = NULL;
	//    oNl = new Netlist((string(iNl->getName())+ "_C").c_str());
	//
	//    iNl->collectDFS(vNodes);
	//    for(i = 0; i < vNodes.size(); i++) {
	//	if(vNodes[i]->isArithmetic()) {
	//	    FOREACH_FANIN_OF_NODE(vNodes[i], j, pNode) {
	//		if(! pNode->isArithmetic()) {
	//		    if(pNode->isConst()) {
	//			consts.insert(pair<string, uint>(pNode->getName(), consts.size()));
	//		    } else {
	//			pa = Util::atoi(pNode->getName());
	//			if(!pa.first) {
	//			    inputs.insert(pair<string, uint>(pNode->getName(), inputs.size()));
	//			}
	//		    }
	//		    pa = Util::atoi(pNode->getName());
	//		    if(pa.first) {
	//			mNode2TEDNodes.insert(pair<Node*, ETedNode*>(pNode, new ETedNode(_pTedMan, pa.second)));
	//		    } else {
	//			mNode2TEDNodes.insert(pair<Node*, ETedNode*>(pNode, new ETedNode(_pTedMan, pNode->getName())));
	//		    }
	//		    sFanins.clear();
	//		    sFanins.insert(pNode);
	//		    msFanins.insert(pair<Node*, set <Node*> >(pNode, sFanins));
	//		}
	//	    }
	//	}
	//    }
	//
	//    ppVars = (const char**)calloc(inputs.size()+consts.size(), sizeof(const char*));
	//    for(map <string, unsigned int>::iterator p = inputs.begin(); p != inputs.end(); p++) {
	//	//	p->second += consts.size();
	//*(ppVars + p->second + consts.size()) = p->first.c_str();
	//    }
	//    for(map <string, unsigned int>::iterator p = consts.begin(); p != consts.end(); p++) {
	//	//	inputs.insert(*p);
	//*(ppVars + p->second) = p->first.c_str();
	//    }
	//    for(i = 0; i < inputs.size()+consts.size(); i++) {
	//	_pTedMan->registerVar(*(ppVars+i));
	//    }
	//    free(ppVars);
	//
	//    _pTedMan->setConstLimit(inputs.size());
	//
	//    for(i = 0; i < vNodes.size(); i++) {
	//	pNode = vNodes[i];
	//	if(pNode->isAdd()) {
	//	    pTNode = mNode2TEDNodes[pNode->GetFanin(0)]->add(_pTedMan, mNode2TEDNodes[pNode->GetFanin(1)]->duplicateRecursive(_pTedMan))->duplicateRecursive(_pTedMan);
	//	} else if(pNode->isSub()) {
	//	    pTNode = mNode2TEDNodes[pNode->GetFanin(0)]->sub(_pTedMan, mNode2TEDNodes[pNode->GetFanin(1)]->duplicateRecursive(_pTedMan))->duplicateRecursive(_pTedMan);
	//	} else if(pNode->isMul()) {
	//	    pTNode = mNode2TEDNodes[pNode->GetFanin(0)]->mul(_pTedMan, mNode2TEDNodes[pNode->GetFanin(1)]->duplicateRecursive(_pTedMan))->duplicateRecursive(_pTedMan);
	//	}
	//
	//	if(pNode->isArithmetic()) {
	//	    mNode2TEDNodes.insert(pair<Node*, ETedNode*>(pNode, pTNode));
	//	    sFanins.clear();
	//	    for(set <Node*>::iterator p = msFanins[pNode->GetFanin(0)].begin(); p != msFanins[pNode->GetFanin(0)].end(); p++) {
	//		sFanins.insert(*p);
	//	    }
	//	    for(set <Node*>::iterator p = msFanins[pNode->GetFanin(1)].begin(); p != msFanins[pNode->GetFanin(1)].end(); p++) {
	//		sFanins.insert(*p);
	//	    }
	//	    msFanins.insert(pair<Node*, set <Node*> >(pNode, sFanins));
	//	}
	//    }
	//
	//
	//    for(i = 0; i < vNodes.size(); i++) {
	//	pNode = vNodes[i];
	//	if(!pNode->isArithmetic()) {
	//	    pNewNode = new Node(pNode->getName(), pNode->getType());
	//	    //	    pNewNode->SetFuncIndex(pNode->GetFuncIndex());
	//	    pNewNode->setFunc(pNode->func());
	//	    FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
	//		if(!pFanin->isArithmetic()) {
	//		    pNewFanin = oNl->get(pFanin->getName());
	//		    assert(pNewFanin != NULL);
	//		} else {
	//		    pNewFanin = new Node(pFanin->getName(), pFanin->getType());
	//		    sFanins = msFanins[pFanin];
	//		    for(set <Node*>::iterator p = sFanins.begin(); p != sFanins.end(); p++) {
	//			pNewFanin->addFanin(oNl->get((*p)->getName()));
	//		    }
	//		    pTNode = mNode2TEDNodes[pFanin];
	//
	//		    pin = _pTedMan->normalize(pTNode);
	//		    pTedNode = pin.Node();
	//		    _pTedMan->linkTedToPO(pin, pNewFanin->getName());
	//		    pNewFanin->setData((void*)pTedNode);
	//		    pNewFanin->setFunc("$ted");
	//		    //		    pTNode->linkToTedNodePO(pNewFanin->getName());
	//		    //		    pNewFanin->SetTed(pTNode);
	//		    //		    pNewFanin->SetFuncIndex("$ted");
	//		    oNl->add(pNewFanin);
	//		}
	//		pNewNode->addFanin(pNewFanin);
	//	    }
	//	    oNl->add(pNewNode);
	//	}
	//    }
	//
	//    //_pTedMan->PurgeETedNodes();
	//    //    TEDNode::releaseUnusedTEDNodes();
	//    return oNl;
	//}

}
