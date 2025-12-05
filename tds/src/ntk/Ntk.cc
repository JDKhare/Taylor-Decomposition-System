/*
 * =====================================================================================
 *
 *       Filename:  Network.cc
 *    Description:
 *        Created:  05/07/2007 12:21:20 PM EDT
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
#include <fstream>
#include <algorithm>
#ifdef _MSC_VER
# define sscanf sscanf_s
#endif

#include "Ntk.h"
#include "Node.h"

#include "util.h"
using namespace util;

#include "Tds.h"
#include "Environment.h"
using namespace tds;

namespace network {
	/**
	 * @brief Internal linkage
	 **/
	namespace {
		/**
		 * @brief Implements a delete of a pointer through a constructor.
		 **/
		struct DeleteObject {
			/**
			 * @brief Overloading the operator()makes the constructor of the structure to behave as a function
			 * @param ptr the type must be a pair, map or multimap; and the poiter to delete must be its second element
			 **/
			template <typename T> void operator()(const T ptr)const {
				delete ptr.second;
			}
		};

		string generateName(string& suggestedName) {
			string name(suggestedName);
			Util::strSubstitute(name,"LPAREN","_");
			Util::strSubstitute(name,"RPAREN");
			return name;
		}

	}

	const char* Netlist::temporary_prefix = "tmp_";

	Netlist::Netlist(const char* _name) {
		this->_name = _name;
		_extracTed = false;
	}

	Netlist::~Netlist(void) {
		for_each(_mNodes.begin(), _mNodes.end(), DeleteObject());
	}

	const char* Netlist::getName(void) {
		return _name.c_str();
	}

	map <const char*, Node*, ltpChar> & Netlist::mapNodes(void) {
		return _mNodes;
	}

	void Netlist::add(Node* node) {
		_mNodes.insert(pair<const char*, Node*>(node->getName(), node));
	}

	void Netlist::remove(Node* node) {
		_mNodes.erase(node->getName());
	}

	Node* Netlist::get(const char* name) {
		map<const char*, Node*, ltpChar>::iterator p;
		p = _mNodes.find(name);
		if(p == _mNodes.end()) {
			return NULL;
		} else {
			return p->second;
		}
	}

	Node* Netlist::getOrCreate(const char* name, NODE_TYPE t) {
		Node* pNode = get(name);
		if(pNode == NULL) {
			pNode = new Node(name, t);
			add(pNode);
		}
		return pNode;
	}

	void Netlist::updateLevel(const vector<Node*>& dfsNodes) {
		assert(!dfsNodes.empty());
		uint level, j;
		Node* pFanin;
		_maxLevel = 0;
		for(uint i = 0; i < dfsNodes.size(); i++) {
			if(dfsNodes[i]->numFanins() == 0) {
				dfsNodes[i]->_level = 0;
			} else {
				level = 0;
				FOREACH_FANIN_OF_NODE(dfsNodes[i], j, pFanin) {
					if(level < pFanin->_level) {
						level = pFanin->_level;
					}
				}
				dfsNodes[i]->_level = level + 1;
			}
			if(_maxLevel < dfsNodes[i]->_level) {
				_maxLevel = dfsNodes[i]->_level;
			}
		}
	}

	unsigned int Netlist::getMaxLevel(void) {
		return _maxLevel;
	}

	vector <Node*> & Netlist::collectDFS(vector <Node*> & vNodes) {
		Node* pNode;
		Mark<Node>::newMark();
		map <const char*, Node*, ltpChar>::iterator p;
		vNodes.clear();
		for(p = _mNodes.begin(); p != _mNodes.end(); p++) {
			pNode = p->second;
			if(pNode->isPO()) {
				_collectDFS_rec(pNode, vNodes);
			}
		}
		return vNodes;
	}

	void Netlist::getPoPis(vector <Node*>* vPos, vector <Node*>* vPis) {
		for(map<const char*,Node*,ltpChar>::iterator p = _mNodes.begin(), pend = _mNodes.end(); p != pend; p++) {
			Node* pNode = p->second;
			if(vPos != NULL && pNode->isPO()) {
				vPos->push_back(pNode);
			} else if(vPis != NULL && pNode->isPI()) {
				vPis->push_back(pNode);
			}
		}
	}

	size_t Netlist::size(void) {
		return _mNodes.size();
	}

	void Netlist::_collectDFS_rec(Node* pNode, vector <Node*> & vNodes) {
		assert(pNode);
		map <Node*, unsigned int>::iterator p;
		unsigned int i = 0, nFanins = 0;
		if(pNode->visited.isMarked())
			return;
		pNode->visited.setMark();
		nFanins = pNode->numFanins();
		for(i = 0; i < nFanins; i++) {
			_collectDFS_rec(pNode->getFanin(i), vNodes);
		}
		vNodes.push_back(pNode);
	}

	char* _getline(ifstream & in, char* line, unsigned int size) {
		unsigned int i;
		if(!in)return NULL;
		do {
			in.getline(line, size);
			//              cout << line << endl;
		} while(in && strncmp(line, "--", 2) == 0);
		if(!in)return NULL;

		for(i = 0; i<strlen(line); i++)if(line[i]==';') {
			line[i+1]=0;
		}
		return line;
	}


	Node* _GetConstNode(int value, map <string, Node*> & mNodes) {
		Node* pNode = new Node(value);

		for(map <string, Node*>::iterator p = mNodes.begin(); p != mNodes.end(); p++) {
			if(string(p->second->getName()) ==  string(pNode->getName())) {
				delete pNode;
				pNode = p->second;
				break;
			}
		}

		return pNode;
	}

	void Netlist::toCombinationalNetlist(void) {
		vector <Node*> vNodes;
		Node* Co,* pNode;
		unsigned int i, j;

		collectDFS(vNodes);
		for(i = 0; i < vNodes.size(); i++) {
			if(vNodes[i]->isReg()) {
				Co = new Node((string(vNodes[i]->getName())+ "_i").c_str(), PO);
				FOREACH_FANIN_OF_NODE(vNodes[i], j, pNode) {
					Co->addFanin(pNode);
				}
				vNodes[i]->clearFanin();
				vNodes[i]->setType(PI);
				add(Co);
			}
		}
	}

	void Netlist::toSequentialnetlist(void) {
		vector <Node*> vNodes, vPis;
		map <string, Node*> mPos;
		map <string, Node*>::iterator p;

		Node* pFanin,* pNode;
		unsigned int i, j;

		collectDFS(vNodes);

		for(i = 0; i < vNodes.size(); i++) {
			if(vNodes[i]->isPI())vPis.push_back(vNodes[i]);
			if(vNodes[i]->isPO())mPos[vNodes[i]->getName()] = vNodes[i];
		}
		for(i = 0; i < vPis.size(); i++) {
			if(vPis[i]->isReg()&&(p = mPos.find(string(vPis[i]->getName())+ "_i"))!= mPos.end()) {
				pNode = p->second;
				FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
					vPis[i]->addFanin(pFanin);
				}

				remove(pNode);
				delete pNode;
				vPis[i]->setType(OPERATOR);
			}
		}
	}

	void Netlist::writeDot(ofstream& ofile, bool bVerbose) {
		unsigned int maxLevel;
		unsigned int i;
		vector <Node*> vNodes;
		string cmd;

		if(size() ==0) {
			return;
		}

		collectDFS(vNodes);
		updateDelayAndRefs(vNodes);
		updateLevel(vNodes);
		maxLevel = getMaxLevel();
		ofile << "Digraph G {\n\tsize=\"10, 7.5\"; center = \"true\"; \n";
		ofile << "\t{\n"
			<< "\t\tnode [shape = plaintext];\n"
			<< "\t\tedge [style = invis];\n"
			<< "\t\tLevelTitle [label=\""<< getName()<< "\"];\n"
			<< "\t\tLevelPO [label=\"\"];\n";

		for(i = 0; i < maxLevel; i++) {
			ofile << "\t\tLevel" << i << " [label=\"\"];\n";
		}
		ofile << "\t\tLevelTitle->LevelPO";
		for(i = 0; i < maxLevel; i++) {
			ofile << "-> Level" << maxLevel-i-1;
		}
		ofile << "\n\t}\n";

		for(i = 0; i < vNodes.size(); i ++) {
			vNodes[i]->dotString(cmd, bVerbose);
			ofile << cmd;
		}
		ofile << "}";
		ofile.close();
	}

	Node* Netlist::_AddOp(const char* name, const char* type, const char* pFanin1, const char* pFanin2) {
		Node* pNode,* pNodeFanin1,* pNodeFanin2;

		if((pNode = get(name))!= NULL) {
			assert(0);
			return NULL;
		}
		pNode   = getOrCreate(name);
		pNodeFanin1 = getOrCreate(pFanin1, PI);
		pNodeFanin2 = getOrCreate(pFanin2, PI);
		pNode->addFanin(pNodeFanin1);
		pNode->addFanin(pNodeFanin2);
		pNode->setFunc(type);
		return pNode;
	}

	Node* Netlist::addOpAdd(const char* name, const char* pFanin1, const char* pFanin2) {
		return _AddOp(name, FUNC_ADD, pFanin1, pFanin2);
	}
	Node* Netlist::addOpSub(const char* name, const char* pFanin1, const char* pFanin2) {
		return _AddOp(name, FUNC_SUB, pFanin1, pFanin2);
	}
	Node* Netlist::addOpMul(const char* name, const char* pFanin1, const char* pFanin2) {
		return _AddOp(name, FUNC_MUL, pFanin1, pFanin2);
	}


	void Netlist::_ReadMatrixRow(size_t r, string & line, const char* fv, const char* cv) {
		vector<string> vCoEffs;
		Node* pNode,* pNodeLast;
		string nName, v;
		unsigned int counter;

		vCoEffs = Util::split(line, " \n");
		nName = "m"+Util::itoa(r)+"_";
		v = cv;

		pNodeLast = NULL;
		counter = 0;
		for(size_t c = 0; c < vCoEffs.size(); c++) {
			pNode = addOpMul((nName+Util::itoa(counter++)).c_str(), vCoEffs[c].c_str(),(v+Util::itoa(c)).c_str());
			if(pNodeLast != NULL) {
				pNode = addOpAdd((nName+Util::itoa(counter++)).c_str(), pNodeLast->getName(), pNode->getName());
			}
			pNodeLast = pNode;
		}

		pNode = getOrCreate((string(fv)+Util::itoa(r)).c_str(), PO);
		pNode->addFanin(pNodeLast);
	}

	Netlist* Netlist::readMatrix(const char* file) {
		ifstream in(file);
		string line;
		Netlist* nl;
		size_t i;

		if(in.is_open()) {
			nl = new Netlist(file);
		} else {
			return NULL;
		}

		i = 0;
		while(Util::readline(in, line, "#")!= 0) {
			nl->_ReadMatrixRow(i++, line, "Y", "X");
		}

		in.close();
		return nl;
	}


	int Netlist::updateDelayAndRefs(const vector<Node*>& dfsNodes) {
		assert(!dfsNodes.empty());
		int max = -1, req;
		Node* pNode,* pFanin;
		unsigned int j;
		for(unsigned int i = 0; i < dfsNodes.size(); i++) {
			pNode = dfsNodes[i];
			pNode->_nRefs = 0;
			if(pNode->isPI()) {
				pNode->setArrTime(0);
				continue;
			}
			max = -1;
			FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
				pFanin->incRef();
				if(max < pFanin->getArrTime()) {
					max = pFanin->getArrTime();
				}
			}
			pNode->setArrTime(max + 1);
		}
		req = 0;
		for(unsigned int i = 0; i < dfsNodes.size(); i++) {
			if(dfsNodes[i]->isPO()) {
				if(req < dfsNodes[i]->getArrTime()) {
					req = dfsNodes[i]->getArrTime();
				}
				//cerr << "======== po " << dfsNodes[i]->getName()<< " ==========" << endl;
				dfsNodes[i]->updateRequiredTime(req);
			}
		}
		return req;
	}

	Netlist* Netlist::balance(void) {
		vector <Node*> vPos, vPis, vNodes;
		collectDFS(vNodes);
		updateDelayAndRefs(vNodes);
		int req = 0;
		getPoPis(&vPos, & vPis);
		Mark<Node>::newMark();
		for(unsigned int i = 0; i < vPos.size(); i++) {
			vPos[i]->balance_rec();
			if(req < vPos[i]->getArrTime()) {
				req = vPos[i]->getArrTime();
			}
		}
		for(unsigned int i = 0; i < vPos.size(); i++) {
			vPos[i]->updateRequiredTime(req);
		}
		return this;
	}

	void Netlist::getStats(int & nAdd, int & nSub, int & nMul, int & nStructural, int & nPi, int & nPo, int & nLatency, int &nReg) {
		nAdd = nSub = nMul = nStructural = nPi = nPo = nReg =0;
		vector <Node*> vNodes;
		collectDFS(vNodes);
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			if(vNodes[i]->isPI()) {
				nPi++;
			} else if(vNodes[i]->isPO()) {
				nPo++;
			} else if(vNodes[i]->isAdd()) {
				nAdd++;
			} else if(vNodes[i]->isSub()) {
				nSub++;
			} else if(vNodes[i]->isMul()) {
				nMul++;
			} else if(vNodes[i]->isReg()) {
				nReg++;
			} else {
				nStructural ++;
			}
		}
		nLatency = updateDelayAndRefs(vNodes);
	}

	CDFGData Netlist::readCDFGData(ifstream& in) {
		string line;
		CDFGData data;
		bool doContinue = true;
		const char* prop_bit  = "bitwidth";
		const char* prop_sign = "signed";
		const char* prop_bank = "bank";
		const char* prop_addr = "address";
		const char* prop_val  = "value";
		const char* prop_wire = "hardwire";
		const char* prop_time = "time";
		const char* prop_port = "port";
		const char* prop_dff  = "aging";
		const char* token_comment = "#";
		// to be ignored at least for now
		const char* prop_fixedpoint   = "fixedpoint";
		const char* prop_quantization = "quantization";
		const char* prop_overflow     = "overflow";
		const char* prop_loopback     = "loopback";
		do {
			Util::readline(in, line, token_comment);
			vector<string> vToks = Util::split(line, "() {,;");
			if(vToks[0] == prop_bit) {
				data.bitwidth = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_sign) {
				data.sign = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_bank) {
				data.bank = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_addr) {
				data.address = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_val) {
				if(vToks[1][0] == '_')
					vToks[1][0] = '-';
				data.value = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_wire) {
				data.hardwire = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_time) {
				data.time = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_port) {
				data.port = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_dff) {
				data.aging = generateName(vToks[1]);
			} else if(vToks[0] == prop_fixedpoint) {
				data.fixedpoint = Util::atoi(vToks[1].c_str()).second;
			} else if(vToks[0] == prop_quantization) {
				//nothing
			} else if(vToks[0] == prop_overflow) {
				//nothing
			} else if(vToks[0] == prop_loopback) {
				data.loopback = generateName(vToks[1]);
			} else if(vToks[0] == "}") {
				doContinue = false;
			} else {
				cout << "Skipping unknown property: " << vToks[0] << endl;
			}
		} while(doContinue);
		return data;
	}

	Netlist* Netlist::readCDFG(const char* file, bool inferFF) {
		const char* token_const = "constant";
		const char* token_in    = "input";
		const char* token_out   = "output";
		const char* token_var   = "variable";
		const char* token_tmp   = "temporary";
		const char* token_op    = "operation";
		const char* token_sink  = "sink";
		const char* token_src   = "source";
		const char* token_comment = "#";
		const char* token_next  = "}";
		Netlist* nl = NULL;
		ifstream in(file);
		string line;
		vector <string> vToks;
		map <string, string> mNodesDFF;
		vector <Node*> vFanins;
		string func;
		Node* pNode = NULL,* pFanin = NULL;

		map <string, Node*> mRenamedNodes;
		map <string, Node*>::iterator mp;
		if(!in.is_open()) {
			return NULL;
		}
		string strfile = file;
		int i = strfile.rfind(".");
		if(i != string::npos) {
			strfile = strfile.substr(0, i);
		}
		nl = new Netlist(strfile.c_str());
		while(Util::readline(in, line, token_comment)!= 0) {
			vToks = Util::split(line, "() {,;");
			string name;
			if(vToks.size()>1) {
				name = generateName(vToks[1]);
			}
			if(vToks[0] == token_const) {
				CDFGData data = nl->readCDFGData(in);
				if(data.value!=0)
					pNode = nl->getOrCreate(name.c_str(), CONST);
				else  {
					// a constant with value 0 other than const_0 is incorrect
					string const_0(Environment::getStr("const_prefix"));
					const_0+="0";
					if(strcmp(const_0.c_str(),name.c_str()) ==0)
						pNode = nl->getOrCreate(name.c_str(), CONST);
					else
						pNode = nl->getOrCreate(name.c_str(), CONSTVAR);
				}
				pNode->setData(data);
			} else if(vToks[0] == token_in) {
				CDFGData data = nl->readCDFGData(in);
				pNode = nl->getOrCreate(name.c_str(), PI);
				pNode->setData(data);
			} else if(vToks[0] == token_out) {
				CDFGData data = nl->readCDFGData(in);
				pNode = nl->getOrCreate(name.c_str(), PO);
				pNode->setData(data);
			} else if(vToks[0] == token_var) {
				CDFGData data = nl->readCDFGData(in);
				pNode = nl->getOrCreate(name.c_str(), VAR);
				pNode->setData(data);
				if(inferFF && !data.aging.empty()) {
					mNodesDFF[name] = data.aging;
				}
			} else if(vToks[0] == token_tmp) {
				CDFGData data = nl->readCDFGData(in);
				pNode = nl->getOrCreate(name.c_str(), TEMPORARY);
				pNode->setData(data);
			} else if(vToks[0] == token_op) {
				pNode = NULL;
				vFanins.clear();
				bool continueReading = false;
				const char* op_type = "function";
				const char* op_data = "read";
				const char* op_result = "write";
				string op_name = vToks[1];
				do {
					Util::readline(in, line, token_comment);
					vToks = Util::split(line, "() {,;");
					if(vToks[0] == op_type) {
						continueReading = false;
						if(vToks[1] == "add") {
							func = FUNC_ADD;
						} else if(vToks[1] == "sub") {
							func = FUNC_SUB;
						} else if(vToks[1] == "mul") {
							func = FUNC_MUL;
						} else if(vToks[1] == "sll") {
							func = FUNC_SLL;
						} else {
							func = vToks[1];
						}
					} else if(vToks[0] == op_data) {
						for(unsigned int i = 1; i < vToks.size(); i++) {
							pFanin = NULL;
							string data_in_name = generateName(vToks[i]);
							pFanin = nl->get(data_in_name.c_str());
							if(pFanin == NULL) {
								mp = mRenamedNodes.find(data_in_name);
								if(mp == mRenamedNodes.end()) {
									cerr << "Error: missing input " << data_in_name << " with i=" << i << endl;
									flush(cerr);
								}
								if(mp!= mRenamedNodes.end()) {
									cerr << "data_name" << data_in_name << " i=" << i << endl;
								}
								assert(mp != mRenamedNodes.end());
								pFanin = mp->second;
							}
							assert(pFanin != NULL);
							vFanins.push_back(pFanin);
						}
						continueReading = true;
					} else if(vToks[0] == op_result) {
						continueReading = false;
						// retrieve the temporary variable node in which the result is written
						// and convert this variable into the operator node
						string data_out_name = generateName(vToks[1]);
						pNode = nl->get(data_out_name.c_str());
						assert(pNode && !pNode->isOperator());
					} else if(continueReading) {
						//op_data could span multiple lines
						for(unsigned int i = 0; i < vToks.size(); i++) {
							pFanin = NULL;
							string data_in_name = generateName(vToks[i]);
							pFanin = nl->get(data_in_name.c_str());
							if(pFanin == NULL) {
								mp = mRenamedNodes.find(data_in_name);
								if(mp == mRenamedNodes.end()) {
									cerr << " where is " << data_in_name << " with i=" << i << endl;
									flush(cerr);
								}
								assert(mp != mRenamedNodes.end());
								pFanin = mp->second;
							}
							assert(pFanin != NULL);
							vFanins.push_back(pFanin);
						}
					}
				} while(vToks[0] != token_next);
				// If pNode exists it is already in the MAP
				// We will transform the variable node into an operator node now
				assert(pNode != NULL);
				assert(!pNode->isPI() && !pNode->isConst());
				if(pNode->isPO()) {
					// unfortunately there is no much consistency in the GAUT cdfg output
					// some outputs are preceded by the ASSIGN function and some others
					// are connected directly to an arithmetic operator
					//
					// we'll normalized that to only two type of outputs [loopback and assign]
					//
					// create a INTERNAL place holder
					// with function FUNC_ASSIGN and node type TED
					if(!strcmp(func.c_str(),FUNC_ASSIGN)) {
						Node* pNodeTemp = nl->getOrCreate(op_name.c_str(), TED);
						pNodeTemp->setFunc(FUNC_ASSIGN);
						pNode->addFanin(pNodeTemp);
						pNode = pNodeTemp;
					} else {
						string op_arith_name = op_name + "_arith";
						Node* pNodeTemp = nl->getOrCreate(op_arith_name.c_str(), TED);
						pNodeTemp->setFunc(FUNC_ASSIGN);
						pNode->addFanin(pNodeTemp);
						// if the output function is other than assign then we need to ensure
						// that the arithmetic operation is being properly accessed
						Node* pNodeOp = nl->getOrCreate(op_name.c_str(), OPERATOR);
						pNodeOp->setFunc(func.c_str());
						pNodeTemp->addFanin(pNodeOp);
						pNode = pNodeOp;
					}
					// use the INTERNAL place holder
				} else {
					pNode->setType(OPERATOR);
					pNode->setFunc(func.c_str());
				}
				for(unsigned int i = 0; i < vFanins.size(); i++) {
					pNode->addFanin(vFanins[i]);
				}
				//nl->add(pNode);
			} else	if(vToks[0] == token_sink) {
				do {
					Util::readline(in, line, token_comment);
					vToks = Util::split(line, ",; ");
					for(unsigned int i = 0; i < vToks.size(); i++) {
						string po_name = generateName(vToks[i]);
						Node* pNode = nl->get(po_name.c_str());
						// pNode could be NULL as we start with the "targets" keyword from sink
						if (pNode && !pNode->isPO()) {
							string out_name = po_name+"_loopback";
							Node* pNodeOut = nl->getOrCreate(out_name.c_str(),PO);
							string loopback_name = po_name+"_ted_holder";
							Node* pNodeLoop = nl->getOrCreate(loopback_name.c_str(),TED);
							pNodeLoop->setFunc(FUNC_LOOPBACK);
							// connect all pieces
							pNodeLoop->addFanin(pNode);
							pNodeOut->addFanin(pNodeLoop);
						}
					}
				} while(line != token_next);
			} else	if(vToks[0] == token_src ) {
				do {
					Util::readline(in, line, token_comment);
				} while(line != token_next);
			} else {
				cout << "unknown " << vToks[0] << " in cdfg file, check GAUT version is " << GAUT_VERSION << endl;
			}
		}
		//NOTE: when inferFF is false, mNodesDFF remains empty
		for(map <string, string>::iterator p = mNodesDFF.begin(); p != mNodesDFF.end(); p++) {
			pNode = nl->get(p->first.c_str());
			pNode->setType(DFF);
			pNode->addFanin(nl->get(p->second.c_str()));
			pNode->setFunc(FUNC_REG);
		}
		in.close();
		return nl;
	}

	void Netlist::writeOpCDFG(ofstream& ofs, Node* pNode) {
		ofs << "operation(op_"<< pNode->getName()<<") {" << endl;
		string func;
		if(pNode->isMul()) {
			func = "mul";
		} else if(pNode->isAdd()) {
			func = "add";
		} else if(pNode->isSub()) {
			func = "sub";
		} else if(pNode->isSll()) {
			func = "sll";
		} else {
			func = pNode->func();
		}
		ofs << "\tfunction " << func <<";" << endl;

		Node* pFanin;
		unsigned int j;
		ofs << "\tread ";
		FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
			ofs <<(j==0?"":",");
			ofs <<(pFanin->isOperator() ? temporary_prefix : "");
			ofs << pFanin->getName();
		}
		ofs <<";" << endl;
	}

#if 0 
	//TODO update bitwidth
	void Netlist::writeCDFG(const char* file) {
		//	Node::startAddingCostantPrefix();
		ofstream ofs(file);
		vector <Node*> vNodes, vConsts, vInputs, vOutputs, vVariables, vOperator, vTed, vAgings, vSource;
		map<Node*,Node*> outputTable;
		collectDFS(vNodes);

		string const_prefix = Environment::getStr("const_prefix");
		int const_length = const_prefix.size();
		string prefix_name = Util::getPrefix(getName());
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			Node* node = vNodes[i];
			switch(node->getType()) {
			case CONSTVAR: //fall through CONST
			case CONST:
				{
					if(strncmp(node->getName(), const_prefix.c_str(), const_length) ==0) {
						vConsts.push_back(node);
					} else {
						vInputs.push_back(node);
					}
					break;
				}
			case PI:
				{
					vInputs.push_back(node);
					break;
				}
			case PO:
				{
					vOutputs.push_back(node);
					outputTable[node->getFanin(0)] = node;
					//assert(node->getFanin(0)->isInterface());
					break;
				}
			case OPERATOR:
				{
					vOperator.push_back(node);
					break;
				}
			case TED:
				{
					vTed.push_back(node);
					break;
				}
			case DFF:
				{
					assert(strcmp(node->func(), FUNC_REG) == 0);
					assert(node->numFanins() == 1);
					vAgings.push_back(node);
					break;
				}
			case TEMPORARY: //fall through(treat it as variable)
			case VAR:
				{
					vVariables.push_back(node);
					break;
				}
			default: assert(false);
			}
		}

		ofs << "#VLSICAD-UMASS, 2012"<<endl;
		ofs << "#Date : " <<endl;
		ofs << "#Entry : "<< getName()<<endl;
		ofs << "source("<<prefix_name<<"_start) {"<<endl;
		ofs << "\ttargets ";
		vSource.insert(vSource.end(), vConsts.begin(), vConsts.end());
		vSource.insert(vSource.end(), vInputs.begin(), vInputs.end());
#if 1
		// Aging variables should be considered as targets because
		// they represent the output of a register, at such they
		// could be available to schedule its operations without any
		// metastability problem
		vSource.insert(vSource.end(), vAgings.begin(), vAgings.end());
#endif
		for(unsigned int i = 0; i < vSource.size(); i++) {
			ofs <<(i==0?" ":",\n\t")<< vSource[i]->getName();
		}
		ofs << ";\n}" << endl;

		ofs << "#Constant declaration" << endl;
		for(unsigned int i = 0; i < vConsts.size(); i++) {
			ofs << "constant("<<vConsts[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			const char* value = vConsts[i]->getName();
			value += const_length;
			ofs << "\tvalue "<<Util::atoi(value).second <<";" << endl;
			ofs << "\tbank 0;" << endl;
			ofs << "\taddress "<< i <<";" << endl;
			ofs << "\thardwire 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Input declaration" << endl;
		for(unsigned int i = 0; i < vInputs.size(); i++) {
			ofs << "input("<<vInputs[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Output declaration" << endl;
		for(unsigned int i = 0; i < vOutputs.size(); i++) {
			ofs << "output("<<vOutputs[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Variable declaration" << endl;
		for(unsigned int i = 0; i < vVariables.size(); i++) {
			string out = vVariables[i]->getName();
			Node* pOut;
			if(out.size()>3 && out.substr(out.size()-3, 3) =="_in" &&(pOut = get(out.substr(0, out.size()-3).c_str()))!= NULL && pOut->isPO()) {
				cerr << "Legacy parse:var detected" << endl;
				continue;
			}
			ofs << "variable("<<vVariables[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Temporary declaration" << endl;
		for(unsigned int i = 0; i < vOperator.size(); i++) {
			ofs << "temporary(" << temporary_prefix << vOperator[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Aging declaration" << endl;
		int counter = 0;
		for(unsigned int i = 0; i < vAgings.size(); i++) {
			string out = vAgings[i]->getName();
			Node* pOut;
			if(out.size()>3 && out.substr(out.size()-3, 3) =="_in" &&(pOut = get(out.substr(0, out.size()-3).c_str()))!= NULL && pOut->isPO()) {
				cerr << "Legacy parse:aging detected" << endl;
				continue;
			}
			ofs << "variable("<<vAgings[i]->getName()<<") {" << endl;
			assert(vAgings[i]->numFanins() == 1);
			if (vAgings[i]->getFanin(0)->getType() ==OPERATOR) {
				ofs << "\taging "  << temporary_prefix << vAgings[i]->getFanin(0)->getName()<<";" << endl;
			} else {
				ofs << "\taging "  << vAgings[i]->getFanin(0)->getName()<<";" << endl;
			}
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "\tbank 1;" << endl;
			ofs << "\taddress "<<(counter++)<<";" << endl;
			ofs << "}" << endl;
		}

		ofs <<"#Operations" << endl;
		for(unsigned int i = 0; i < vOperator.size(); i++) {
			if(strcmp(vOperator[i]->func(),"DFF") ==0)continue;
			//ofs << "operation(op"<<i<<") {" << endl;
			writeOpCDFG(ofs,vOperator[i]);
			Node* pOut = vOperator[i];
			if(outputTable.find(pOut)!=outputTable.end()) {
				ofs << "\twrite " << outputTable[pOut]->getName()<<";" << endl;
			} else {
				ofs << "\twrite " <<(pOut->isOperator() ? temporary_prefix : "")<<pOut->getName()<<";" << endl;
			}
			ofs << "}" << endl;
		}

		ofs << "#Reconnect outputs" << endl;
		for(unsigned int i=0; i < vTed.size(); i++) {
			writeOpCDFG(ofs,vTed[i]);
			Node* pOut = vTed[i];
			ofs << "\twrite " << outputTable[pOut]->getName()<<";" << endl;
			ofs << "}" << endl;
		}

		ofs <<"sink("<<prefix_name<<"_end) {" << endl;
		ofs <<"\ttargets ";
		for(unsigned int i = 0; i < vOutputs.size(); i++) {
			ofs <<(i==0?" ":",\n\t")<<vOutputs[i]->getName();
		}
		ofs <<";" << endl;
		ofs <<"}" << endl;
		ofs.close();

		//	Node::stopAddingCostantPrefix();
	}
#else
	//TODO update bitwidth
	void Netlist::writeCDFG(const char* file) {
		//	Node::startAddingCostantPrefix();
		ofstream ofs(file);
		string prefix_name = Util::getPrefix(getName());
		string const_prefix = Environment::getStr("const_prefix");
		int const_length = const_prefix.size();

		Decomposed dn(*this);

		ofs << "#VLSICAD-UMASS, 2012"<<endl;
		ofs << "#Date : " <<endl;
		ofs << "#Entry : "<< getName()<<endl;
		ofs << "source("<<prefix_name<<"_start) {"<<endl;
		ofs << "\ttargets ";

		for(unsigned int i = 0; i < dn.vSource.size(); i++) {
			ofs <<(i==0?" ":",\n\t")<< dn.vSource[i]->getName();
		}
		ofs << ";\n}" << endl;

		ofs << "#Constant declaration" << endl;
		for(unsigned int i = 0; i < dn.vConsts.size(); i++) {
			ofs << "constant("<<dn.vConsts[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			const char* value = dn.vConsts[i]->getName();
			value += const_length;
			ofs << "\tvalue "<<Util::atoi(value).second <<";" << endl;
			ofs << "\tbank 0;" << endl;
			ofs << "\taddress "<< i <<";" << endl;
			ofs << "\thardwire 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Input declaration" << endl;
		for(unsigned int i = 0; i < dn.vInputs.size(); i++) {
			ofs << "input("<< dn.vInputs[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Output declaration" << endl;
		for(unsigned int i = 0; i < dn.vOutputs.size(); i++) {
			ofs << "output("<< dn.vOutputs[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Variable declaration" << endl;
		for(unsigned int i = 0; i < dn.vVariables.size(); i++) {
			string out = dn.vVariables[i]->getName();
			Node* pOut;
			if(out.size()>3 && out.substr(out.size()-3, 3) =="_in" &&(pOut = get(out.substr(0, out.size()-3).c_str()))!= NULL && pOut->isPO()) {
				cerr << "Legacy parse:var detected" << endl;
				continue;
			}
			ofs << "variable("<< dn.vVariables[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Temporary declaration" << endl;
		for(unsigned int i = 0; i < dn.vOperator.size(); i++) {
			ofs << "temporary(" << temporary_prefix << dn.vOperator[i]->getName()<<") {" << endl;
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "}" << endl;
		}

		ofs << "#Aging declaration" << endl;
		int counter = 0;
		for(unsigned int i = 0; i < dn.vAgings.size(); i++) {
			string out = dn.vAgings[i]->getName();
			Node* pOut;
			if(out.size()>3 && out.substr(out.size()-3, 3) =="_in" &&(pOut = get(out.substr(0, out.size()-3).c_str()))!= NULL && pOut->isPO()) {
				cerr << "Legacy parse:aging detected" << endl;
				continue;
			}
			ofs << "variable("<<dn.vAgings[i]->getName()<<") {" << endl;
			assert(dn.vAgings[i]->numFanins() == 1);
			if (dn.vAgings[i]->getFanin(0)->getType() ==OPERATOR) {
				ofs << "\taging "  << temporary_prefix << dn.vAgings[i]->getFanin(0)->getName()<<";" << endl;
			} else {
				ofs << "\taging "  << dn.vAgings[i]->getFanin(0)->getName()<<";" << endl;
			}
			ofs << "\tbitwidth 32;" << endl;
			ofs << "\tsigned 1;" << endl;
			ofs << "\tbank 1;" << endl;
			ofs << "\taddress "<<(counter++)<<";" << endl;
			ofs << "}" << endl;
		}

		ofs <<"#Operations" << endl;
		for(unsigned int i = 0; i < dn.vOperator.size(); i++) {
			if(strcmp(dn.vOperator[i]->func(),"DFF") ==0)continue;
			//ofs << "operation(op"<<i<<") {" << endl;
			writeOpCDFG(ofs,dn.vOperator[i]);
			Node* pOut = dn.vOperator[i];
			if(dn.outputTable.find(pOut)!=dn.outputTable.end()) {
				ofs << "\twrite " << dn.outputTable[pOut]->getName()<<";" << endl;
			} else {
				ofs << "\twrite " <<(pOut->isOperator() ? temporary_prefix : "")<<pOut->getName()<<";" << endl;
			}
			ofs << "}" << endl;
		}

		ofs << "#Reconnect outputs" << endl;
		for(unsigned int i=0; i < dn.vTed.size(); i++) {
			writeOpCDFG(ofs,dn.vTed[i]);
			Node* pOut = dn.vTed[i];
			ofs << "\twrite " << dn.outputTable[pOut]->getName()<<";" << endl;
			ofs << "}" << endl;
		}

		ofs <<"sink("<<prefix_name<<"_end) {" << endl;
		ofs <<"\ttargets ";
		for(unsigned int i = 0; i < dn.vOutputs.size(); i++) {
			ofs <<(i==0?" ":",\n\t")<<dn.vOutputs[i]->getName();
		}
		ofs <<";" << endl;
		ofs <<"}" << endl;
		ofs.close();

		//	Node::stopAddingCostantPrefix();
	}
#endif


	Node* _DuplicateExtract_Rec(Node* pOldNode, Netlist* pNewNl) {
		Node* pNewNode,* pNewFanin,* pOldFanin;
		unsigned int i;
		if((pNewNode = pNewNl->get(pOldNode->getName()))!= NULL) {
			return pNewNode;
		}
		pNewNode = pOldNode->duplicate();
		pNewNl->add(pNewNode);
		FOREACH_FANIN_OF_NODE(pOldNode, i, pOldFanin) {
			pNewFanin = _DuplicateExtract_Rec(pOldFanin, pNewNl);
			pNewNode->addFanin(pNewFanin);
		}
		return pNewNode;
	}


	Netlist* Netlist::duplicateExtract(vector<Node*>* vPos) {
		assert(vPos);
		Netlist* nl = new Netlist(getName());
		for(unsigned int i = 0; i < vPos->size(); i++) {
			_DuplicateExtract_Rec((*vPos)[i], nl);
		}
		return nl;
	}

	Netlist* Netlist::duplicateExtract(void) {
		vector<Node*>* vPos = new vector <Node*>;
		getPoPis(vPos);
		Netlist* nl = new Netlist(getName());
		for(unsigned int i = 0; i < vPos->size(); i++) {
			_DuplicateExtract_Rec((*vPos)[i], nl);
		}
		delete vPos;
		return nl;
	}


	void Netlist::writeOpC(ofstream& ofs, Node* pNode) {
		string func;
		ofs << "op_";
		if(pNode->isMul()) {
			func = "mul";
		} else if(pNode->isAdd()) {
			func = "add";
		} else if(pNode->isSub()) {
			func = "sub";
		} else if(pNode->isSll()) {
			func = "sll";
		} else {
			func = pNode->func();
		}
		ofs << func <<"( ";

		Node* pFanin;
		unsigned int j;
		FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
			ofs <<(j==0?"":",");
			ofs <<(pFanin->isOperator() ? temporary_prefix : "");
			ofs << pFanin->getName();
		}
		ofs <<" )";
	}

	//TODO update bitwidth
	void Netlist::writeC(const char* file) {
		ofstream ofs(file);
		string prefix_name = Util::getPrefix(getName());
		string const_prefix = Environment::getStr("const_prefix");
		int const_length = const_prefix.size();

		Decomposed dn(*this);

		ofs << "/*" << endl;
		ofs << " * VLSICAD-UMASS, 2012"<<endl;
		ofs << " * Date : " <<endl;
		ofs << " * Entry : "<< getName()<<endl;
		ofs << " */" << endl << endl;
		// header
		ofs << "#include \"tds.h\"" << endl;

		ofs << "int main (";
		for(unsigned int i = 0; i < dn.vInputs.size(); i++) {
			ofs <<(i==0?" ":",") << "int " << dn.vInputs[i]->getName();
		}
		for(unsigned int i = 0; i < dn.vOutputs.size(); i++) {
			ofs <<(i==0?" ":",")<< "int* " << dn.vOutputs[i]->getName();
		}
		ofs << ") {" << endl;
		// constants
		ofs << "/* " << dn.vConsts.size() << " Constant declaration */" << endl;
		for(unsigned int i = 0; i < dn.vConsts.size(); i++) {
			//if (!dn.vConsts[i]->hardwire) {
			ofs << "static ";
			//}
			ofs << "const int "<<dn.vConsts[i]->getName();
			const char* value = dn.vConsts[i]->getName();
			value += const_length;
			if (Util::atoi(value).first) {
				ofs << " = " << Util::atoi(value).second;
			}
			ofs << ";" << endl;
		}
		// variables
		ofs << "/* " << dn.vVariables.size() << " Variable declaration */" << endl;
		for(unsigned int i = 0; i < dn.vVariables.size(); i++) {
			string out = dn.vVariables[i]->getName();
			Node* pOut;
			if(out.size()>3 && out.substr(out.size()-3, 3) =="_in" &&(pOut = get(out.substr(0, out.size()-3).c_str()))!= NULL && pOut->isPO()) {
				cerr << "Legacy parse:var detected" << endl;
				continue;
			}
			ofs << "int "<< dn.vVariables[i]->getName()<<";" << endl;
		}

		ofs << "/* " << dn.vOperator.size() << " Temporary declaration */" << endl;
		for(unsigned int i = 0; i < dn.vOperator.size(); i++) {
			ofs << "int " << temporary_prefix << dn.vOperator[i]->getName()<<";" << endl;
		}

		ofs << "/* " << dn.vAgings.size() << " Aging declaration */" << endl;
		int counter = 0;
		for(unsigned int i = 0; i < dn.vAgings.size(); i++) {
			string out = dn.vAgings[i]->getName();
			Node* pOut;
			if(out.size()>3 && out.substr(out.size()-3, 3) =="_in" &&(pOut = get(out.substr(0, out.size()-3).c_str()))!= NULL && pOut->isPO()) {
				cerr << "Legacy parse:aging detected" << endl;
				continue;
			}
			ofs << "int " << dn.vAgings[i]->getName()<<" = ";
			assert(dn.vAgings[i]->numFanins() == 1);
			if (dn.vAgings[i]->getFanin(0)->getType() ==OPERATOR) {
				ofs << temporary_prefix << dn.vAgings[i]->getFanin(0)->getName();
			} else {
				ofs << dn.vAgings[i]->getFanin(0)->getName();
			}
			ofs << ";" << endl;
		}

		ofs <<"/* " << dn.vOperator.size() << " Operations */" << endl;
		for(unsigned int i = 0; i < dn.vOperator.size(); i++) {
			Node* pOut = dn.vOperator[i];
			if(strcmp(pOut->func(),"DFF") ==0) {
				continue;
			}
			ofs << "/* op_"<< pOut->getName()<<" */" << endl;
			if(dn.outputTable.find(pOut)!=dn.outputTable.end()) {
				ofs << dn.outputTable[pOut]->getName()<< " = ";
			} else {
				ofs << (pOut->isOperator() ? temporary_prefix : "") << pOut->getName() << " = ";
			}
			writeOpC(ofs,pOut);
			ofs << ";" << endl;
		}

		ofs << " /* Reconnecting " << dn.vTed.size() << " outputs */" << endl;
		for(unsigned int i=0; i < dn.vTed.size(); i++) {
			Node* pOut = dn.vTed[i];
			ofs << "*" << dn.outputTable[pOut]->getName()<< " = ";
			writeOpC(ofs,pOut);
			ofs << ";" << endl;
		}
		ofs << "return 0;" << endl;
		ofs <<"};" << endl;
		ofs.close();
	}

	void Netlist::Decomposed::run(void) {
		_myself.collectDFS(vNodes);
		string const_prefix = Environment::getStr("const_prefix");
		int const_length = const_prefix.size();
		for(unsigned int i = 0; i < vNodes.size(); i++) {
			Node* node = vNodes[i];
			switch(node->getType()) {
			case CONSTVAR: //fall through CONST
			case CONST:
				{
					if(strncmp(node->getName(), const_prefix.c_str(), const_length) ==0) {
						vConsts.push_back(node);
					} else {
						vInputs.push_back(node);
					}
					break;
				}
			case PI:
				{
					vInputs.push_back(node);
					break;
				}
			case PO:
				{
					vOutputs.push_back(node);
					outputTable[node->getFanin(0)] = node;
					//assert(node->getFanin(0)->isInterface());
					break;
				}
			case OPERATOR:
				{
					vOperator.push_back(node);
					break;
				}
			case TED:
				{
					vTed.push_back(node);
					break;
				}
			case DFF:
				{
					assert(strcmp(node->func(), FUNC_REG) == 0);
					assert(node->numFanins() == 1);
					vAgings.push_back(node);
					break;
				}
			case TEMPORARY: //fall through(treat it as variable)
			case VAR:
				{
					vVariables.push_back(node);
					break;
				}
			default: assert(false);
			}
		}
		vSource.insert(vSource.end(), vConsts.begin(), vConsts.end());
		vSource.insert(vSource.end(), vInputs.begin(), vInputs.end());
#if 1
		// Aging variables should be considered as targets because
		// they represent the output of a register, at such they
		// could be available to schedule its operations without any
		// metastability problem
		vSource.insert(vSource.end(), vAgings.begin(), vAgings.end());
#endif
	}

}
