/*
 * =====================================================================================
 *
 *       Filename:  ConvertDfg2Ntl.cc
 *    Description:
 *        Created:  11/10/2010 09:45:41 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <iostream>
using namespace std;

#include "Node.h"
#include "Ntk.h"
using namespace network;

#include "DfgMan.h"
#include "DfgNode.h"
using namespace dfg;

#include "util.h"
using util::Util;

#include "Environment.h"
using namespace tds;

#include "ConvertDfg2Ntl.h"
namespace convert {


	void ConvertDfg2Ntl::decomposedNodes(Netlist* newNtl, MapNames2Dfg& mpPoDfgNode, MapNames2Ntl& mDecomposedNodes) {
		assert(newNtl);
		assert(_dMan);
		DfgNode* dNode;
		Node* nNode;
		vector <DfgNode*> dfgNodes;
		MapDfg2Ntl mBin2Nodes;
		MapDfg2Name mBin2Name;
		MapDfg2Name::iterator it;
		for(MapNames2Dfg::iterator p = mpPoDfgNode.begin(); p != mpPoDfgNode.end(); p++) {
			if(p->second) {
				mBin2Name[p->second] = p->first;
			}
		}
		_dMan->collectDFS(dfgNodes);
		for(unsigned int i = 0; i < dfgNodes.size(); i++) {
			dNode = dfgNodes[i];
			if(dNode->isPI()|| dNode->isVarConst()) {
				const char* const_prefix = Environment::getStr("const_prefix").c_str();
				int const_len = strlen(const_prefix);
				if(strncmp(const_prefix, dNode->getName(), const_len) == 0) {
					const char* neg_prefix = Environment::getStr("negative_prefix").c_str();
					int neg_len = strlen(neg_prefix);
					pair <bool, wedge> tt;
					bool minus = false;
					if(strncmp(neg_prefix,dNode->getName()+const_len,neg_len) ==0) {
						tt=Util::atoi(dNode->getName()+const_len+neg_len);
						minus = true;
					} else {
						tt=Util::atoi(dNode->getName()+const_len);
					}
					if(tt.first) {
						wedge val = minus ? -tt.second : tt.second;
						nNode = new Node(val);
					} else {
						nNode =  new Node(dNode->getName(), CONSTVAR);
					}
				} else {
					nNode =  new Node(_dMan->getNameIfRegisters(dNode), PI);
					//nNode =  new Node(dNode->getName(), PI);
				}
			} else if(dNode->isConst()) {
				nNode = new Node(dNode->getConst());
			} else {
				it = mBin2Name.find(dNode);
				Node* leftNode = mBin2Nodes[dNode->getLeft()];
				assert(leftNode);
				if(dNode->isReg()) {
					string rname;
					if (it!=mBin2Name.end())
						rname = it->second;
					else
						rname = _dMan->writeCDFGop(dNode); // string("R_"+Util::itoa(i));
					nNode = new Node(rname.c_str(),DFF);
					//
					// retrieve number of Registers from the DFG node
					string retime_value = dNode->getName();
					retime_value.erase(retime_value.find_first_of(DfgNode::TOKEN_REG));
					std::pair<bool,wedge> ret = Util::atoi(retime_value.c_str());
					assert(ret.first);
					assert(dNode->getLeft() ==dNode->getRight());
					wedge start = ret.second;
					//
					Node* tmpNode = nNode;
					Node* tmpKid = NULL;
					for (;ret.second>0;ret.second--) {
						tmpNode->setFunc(FUNC_REG);
						if (ret.second!=start) {
							newNtl->add(tmpNode);
						}
						if (ret.second!=1) {
							tmpKid = new Node(string(rname+string("_R")+Util::itoa(ret.second-1)).c_str(),DFF);
						} else {
							tmpKid = leftNode;
						}
						tmpNode->addFanin(tmpKid);
						tmpNode = tmpKid;
					}
				} else {
					if(it != mBin2Name.end()) {
						nNode = new Node(it->second.c_str(), OPERATOR);
					} else {
						nNode = new Node(_dMan->writeCDFGop(dNode).c_str(), OPERATOR); //string("T_"+Util::itoa(i))
					}
					nNode->addFanin(leftNode);
					Node* rightNode = mBin2Nodes[dNode->getRight()];
					assert(rightNode);
					nNode->addFanin(rightNode);
					if(dNode->isMul())
						nNode->setFunc(FUNC_MUL);
					else if(dNode->isAdd())
						nNode->setFunc(FUNC_ADD);
					else if(dNode->isSub())
						nNode->setFunc(FUNC_SUB);
					else if(dNode->isLSH())
						nNode->setFunc(FUNC_SLL);
					else {
						cerr << "DFG operator \"" << dNode->getOpStr()<< "\" not supported for NTL translation" << endl;
						cerr << "writing the CDFG into a file might now work" << endl;
						nNode->setFunc(FUNC_ERROR);
					}
				}
			}
			newNtl->add(nNode);
			mBin2Nodes[dNode] = nNode;
		}
		for(MapNames2Dfg::iterator p = mpPoDfgNode.begin(); p != mpPoDfgNode.end(); p++) {
			dNode = p->second;
			if(NULL == dNode) {
				nNode = NULL;
			} else {
				nNode = mBin2Nodes[dNode];
			}
			mDecomposedNodes[p->first] = nNode;
		}
	}

	Netlist* ConvertDfg2Ntl::translate(void) {
		Node* pNode,* pFanin,* pNewNode,* pNewFanin;
		unsigned int j;
		Netlist* newNtl = new Netlist((string(_nMan->getName())+ "_2ntk").c_str());
		MapNames2Dfg* mpPoDfgNode = _dMan->getPos();
		MapNames2Ntl mDecomposedNodes;
		decomposedNodes(newNtl,*mpPoDfgNode, mDecomposedNodes);
		vector <Node*> ntlNodes;
		ntlNodes = _nMan->collectDFS(ntlNodes);
		// we are traversing the Ntl from which the TED functionality was extracted, PIs, POs, place holders & structural elements
		for(unsigned int i = 0; i < ntlNodes.size(); i++) {
			pNode = ntlNodes[i];
			//		cout << pNode << " name=" << pNode->getName()<< " type=" << pNode->getType()<< " func="<< pNode->func()<< endl;
#define METHOD2 1
#undef  METHOD1
#undef  METHOD0
#if defined(METHOD2)
			//
			// METHOD 2: generates a new netlist from an old existing one.
			//           It is safe to delete this new netlist without compromising the old one.
			//
			pNewNode = newNtl->get(pNode->getName());
			if(strcmp(pNode->func(),FUNC_TED) == 0) { //node to replace
				//all points should be reattached
				assert(pNewNode || mDecomposedNodes[pNode->getName()]);
				//			cout << "$ted node " << pNode << " " << pNode->getName()<< " " << pNode->func()<< endl;
			} else {
				if(pNewNode == NULL) {
					// if a node does not exist on the new NTL, export it
					//				cout << "exporting node " << pNode << " " << pNode->getName()<< " " << pNode->func()<< endl;
					pNewNode = new Node(pNode->getName(), pNode->getType());
					newNtl->add(pNewNode);
				} else {
					// node exist, but PIs in the new netlist might correspond to extracted nodes in the old netlist
					//				cout << pNewNode << " name=" << pNewNode->getName()<< " type=" << pNewNode->getType()<< " func="<< pNewNode->func()<< endl;
					if(pNode->isPI()|| pNode->isConst()|| pNode->isConstVar()) {
						//safe to disregard
						continue;
					} else {
						//reconnect
						pNewNode->setType(pNode->getType());
						//					cout << "Reconnected" <<endl;
					}
				}
				pNewNode->setFunc(pNode->func());
				// connect the exported node to the new ntl
				FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
					pNewFanin = newNtl->get(pFanin->getName());
					//			cout << "  " << j << " fanin=" << pFanin << " name=" << pFanin->getName()<< " type=" << pFanin->getType()<< " func="<< pFanin->func()<< endl;
					//			cout << "  " << "  pNewFanin=" << pNewFanin << endl;
					if(pNewFanin == NULL) {
						if(mDecomposedNodes.find(pFanin->getName()) == mDecomposedNodes.end()) {
							cerr << "ntl_name=" << newNtl->getName()<< " pFanin_name=" << pFanin->getName()<< " pNode_name=" << pNode->getName()<< " j=" << j << endl;
							throw string("error found");
						}
						assert(mDecomposedNodes.find(pFanin->getName())!= mDecomposedNodes.end());
						pNewFanin = mDecomposedNodes[pFanin->getName()];
						newNtl->add(pNewFanin);
					}
					assert(pNewFanin != NULL);
					pNewNode->addFanin(pNewFanin);
				}

			}
#elif defined(METHOD1)
			//
			// METHOD 1: Reuses nodes from past netlist, therefore eliminating the most recent netlist invalidates nodes in ancestor
			//           which are stored on the netlist history
			//
			pNewNode = newNtl->get(pNode->getName());
			if(strcmp(pNode->func(),FUNC_TED) == 0) { //node to replace
				//all points should be reattached
				assert(pNewNode || mDecomposedNodes[pNode->getName()]);
				//			cout << "$ted node " << pNode << " " << pNode->getName()<< " " << pNode->func()<< endl;
			} else {
				if(pNewNode == NULL) {
					// if a node does not exist on the new NTL, export it
					//				cout << "exporting node " << pNode << " " << pNode->getName()<< " " << pNode->func()<< endl;
					pNewNode = new Node(pNode->getName(), pNode->getType());
					newNtl->add(pNewNode);
				} else {
					// node exist, but PIs in the new netlist might correspond to extracted nodes in the old netlist
					//				cout << pNewNode << " name=" << pNewNode->getName()<< " type=" << pNewNode->getType()<< " func="<< pNewNode->func()<< endl;
					if(pNode->isPI()|| pNode->isConst()|| pNode->isConstVar()) {
						//safe to disregard
						continue;
					} else {
						//reconnect
						pNewNode->setType(pNode->getType());
						//					cout << "Reconnected" <<endl;
					}
				}
				pNewNode->setFunc(pNode->func());
				// connect the exported node to the new ntl
				FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
					pNewFanin = newNtl->get(pFanin->getName());
					//			cout << "  " << j << " fanin=" << pFanin << " name=" << pFanin->getName()<< " type=" << pFanin->getType()<< " func="<< pFanin->func()<< endl;
					//			cout << "  " << "  pNewFanin=" << pNewFanin << endl;
					if(pNewFanin == NULL) {
						if(mDecomposedNodes.find(pFanin->getName()) == mDecomposedNodes.end()) {
							cerr << "ntl_name=" << newNtl->getName()<< " pFanin_name=" << pFanin->getName()<< " pNode_name=" << pNode->getName()<< " j=" << j << endl;
							throw string("error found");
						}
						assert(mDecomposedNodes.find(pFanin->getName())!= mDecomposedNodes.end());
						pNewFanin = mDecomposedNodes[pFanin->getName()];
						newNtl->add(pNewFanin);
					}
					assert(pNewFanin != NULL);
					pNewNode->addFanin(pNewFanin);
				}

			}
#elif defined(METHOD0)
			//
			// METHOD 0:
			//
			if(pNode->getData()!= NULL) {
				cout << "SKIPPING node " << pNode->getName()<< " -> " << pNode << endl;
				//we are assuming an assing goes on top of them?
				//Ntl Operators and $ted place holders do not have any CDFG Data
				continue;
			}
			pNewNode = newNtl->get(pNode->getName());
			if(pNewNode == NULL) {
				pNewNode = new Node(pNode->getName(), pNode->getType());
				cout << pNewNode << " name=" << pNewNode->getName()<< " type=" << pNewNode->getType()<< " func="<< pNewNode->func()<< endl;
				newNtl->add(pNewNode);
			} else {
				cout << pNewNode << " name=" << pNewNode->getName()<< " type=" << pNewNode->getType()<< " func="<< pNewNode->func()<< endl;
				pNewNode->setType(pNode->getType());
			}
			//pNewNode->setFunc(ntlNodes[i]->func());
			FOREACH_FANIN_OF_NODE(pNode, j, pFanin) {
				pNewFanin = newNtl->get(pFanin->getName());
				//if(pNewFanin != NULL)continue;
				cout << "  " << j << " fanin=" << pFanin << " name=" << pFanin->getName()<< " type=" << pFanin->getType()<< " func="<< pFanin->func()<< endl;
				cout << "  " << "  pNewFanin=" << pNewFanin << endl;
				if(pNewFanin == NULL) {
					assert(mDecomposedNodes.find(pFanin->getName())!= mDecomposedNodes.end());
					pNewFanin = mDecomposedNodes[pFanin->getName()];
					newNtl->add(pNewFanin);
				}
				assert(pNewFanin != NULL);
				pNewNode->addFanin(pNewFanin);
			}
#endif
		}
		return newNtl;
	}

}
