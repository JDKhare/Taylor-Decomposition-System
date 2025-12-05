/*
 * =====================================================================================
 *
 *       Filename:  DfgBitReduction.cc
 *    Description:
 *        Created:  11/04/2010 01:00:48 PM
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

#include "DfgBitReduction.h"
#include "DfgNode.h"
#include "DfgMan.h"
#include "DfgOperator.h"

#include "util.h"
#include "csd.h"
using util::Util;
using util::Csd;

#include "Environment.h"
using tds::Environment;

namespace dfg {
	DfgBitReduction::DfgBitReduction(DfgMan* manager, double maxError, bool report)
		:_currentManager(manager), _maxError(maxError), _report(report) {
		}

	void DfgBitReduction::opTopological(void) {
		//Goes left, right - top,bottom, decrementing the bitwidth by 1.
		vector <DfgNode* > vNodes;
		int latency = _currentManager->collectDFS(vNodes);
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
		FixCost cost;
		for(unsigned int index = 1; index <= latency; index++) {
			set<DfgNode*>* tochange = wave[index];
			set<DfgNode*> avoid;
			double error = 0.0;
			bool forceFinish = false;
			while((error < _maxError)&& !forceFinish) {
				int i = 1;
				for(set<DfgNode*>::iterator it = tochange->begin(); it != tochange->end(); it++,i++) {
					lastnode = (*it);
					if(avoid.find(*it) == avoid.end()) {
						if(lastnode->_op->getBitwidth()->isReducible()) {
							lastnode->_op->getBitwidth()->reduce();
							_currentManager->computeBitwidth(&DfgNode::uptoLevel,index);
							FixCost* costG = gappa.compute_cost(*_currentManager);
							double innerError  = costG->bitwidth;
							delete costG;
							error = innerError;
							if(innerError > _maxError) {
								lastnode->_op->getBitwidth()->increment();
								_currentManager->computeBitwidth(&DfgNode::uptoLevel,index);
								FixCost* costG = gappa.compute_cost(*_currentManager);
								error = costG->bitwidth;
								delete costG;
								avoid.insert(lastnode);
							}
							if(_report) {
								cout << "[" << index << "/" << latency << "]";
								cout << " " << i << "/" << tochange->size();
								cout << " error=" << innerError;
								cout << " op=" << lastnode->getOpStr();
								cout << " id=" << lastnode->_op->getID();
								cout << " bitwidth=" << lastnode->_op->getBitwidth()->at();
								if(innerError > _maxError) {
									cout << " BACKTRACKING prev error=" << error << endl;
								}
								flush(cout);
								cout << '\xd';
							}
						} else {
							avoid.insert(lastnode);
						}
					}
				}
				forceFinish = (avoid.size() == tochange->size());
			}
		}
		for(unsigned int index = 1; index <= latency; index++) {
			delete wave[index];
		}
	}


	void DfgBitReduction::opPriorityCone(void) {
		//Goes left, right - top,bottom, decrementing the bitwidth by 1.
		bool report = _report;
		_report = false;
		opSensitivity();
		_report = report;
		DfgParents parents;
		_currentManager->collectParents(parents);
		//DfgNode* lastnode;
		MinGappaBound gappa;
		FixCost cost;
		double error = 0.0;
		Mark<DfgNode>::newMark();
		for(Sensitivity::iterator it = _mpySense.begin(); it != _mpySense.end(); it++) {
			Cost opcost = it->first;
			DfgNode* node = it->second;
			unsigned int bitw = opcost.bitw;
			unsigned int diff = node->_op->getBitwidth()->getSize();

			//attempt full sensitivity reduction
			node->_op->getBitwidth()->reduce(bitw);

			//correct the bitwidth by binary approximation
			FixCost* costG = gappa.compute_cost(*_currentManager);
			error  = costG->bitwidth;
			delete costG;
			bool binaryCorrection = (error>_maxError);
			while(binaryCorrection) {
				bitw = bitw/2;
				if(error>_maxError) {
					node->_op->getBitwidth()->increment(bitw);
				} else {
					node->_op->getBitwidth()->reduce(bitw);
				}
				costG = gappa.compute_cost(*_currentManager);
				error  = costG->bitwidth;
				delete costG;
				binaryCorrection = (bitw>1) ? binaryCorrection : false;
				if(_report) {
					cout << " error=" << error;
					cout << " op=" << node->getOpStr();
					cout << " id=" << node->_op->getID();
					cout << " bitwidth=" << node->_op->getBitwidth()->at()<< "       ";
					flush(cout);
					cout << '\xd';
				}
			}
			if(error>_maxError) {
				node->getOperator()->getBitwidth()->increment();
				costG = gappa.compute_cost(*_currentManager);
				error = costG->bitwidth;
				delete costG;
				assert(error<=_maxError);
			}
			if(_report) {
				cout << " error=" << error;
				cout << " op=" << node->getOpStr();
				cout << " id=" << node->_op->getID();
				cout << " bitwidth=" << node->_op->getBitwidth()->at();
				flush(cout);
				cout << endl;
			}
			assert(node->_op->getBitwidth()->getSize()<=diff);
			diff -= node->_op->getBitwidth()->getSize();

			//propagate the exact bitwidth computation toward the outputs
			//this can be optimize if it is levelized
			vector<DfgNode*> toUpdate = parents[node];
			while(!toUpdate.empty()) {
				DfgNode* update = toUpdate.back();
				toUpdate.pop_back();
				update->computeBitwidth();
				toUpdate.insert(toUpdate.begin(),parents[update].begin(),parents[update].end());
			}

			//propagate the exact bitwidth computation toward the inputs
			//inputs with different operators: preference is given to the path with multipliers
			//inputs with equal operators: bit reduction is split
			updateBitwidthTowardPI(node,diff);
		}
	}


	void DfgBitReduction::updateBitwidthTowardPI(DfgNode* node,unsigned int val) {
		if(!node->isOp()|| val==0) {
			return;
		}
		if(node->isReg()) {
			return;
		}
		assert(node->getOp());
		MinGappaBound gappa;
		FixCost* costG = gappa.compute_cost(*_currentManager);
		double errorBound = costG->bitwidth;
		double errorG;
		delete costG;

		unsigned int lb = node->getLeft()->getOperator()->getBitwidth()->getSize();
		unsigned int rb = node->getRight()->getOperator()->getBitwidth()->getSize();
		if(node->isAddorSub()) {
			DfgNode* nb; //big
			DfgNode* ns; //small
			if(lb>rb) {
				nb = node->getLeft();
				ns = node->getRight();
			} else {
				ns = node->getLeft();
				nb = node->getRight();
			}
			if(nb->isOp()) {
				nb->getOperator()->getBitwidth()->reduce(val);
				costG = gappa.compute_cost(*_currentManager);
				errorG = costG->bitwidth;
				delete costG;
				if( errorG > errorBound) {
					//do not accept change
					nb->getOperator()->getBitwidth()->increment(val);
				} else {
					updateBitwidthTowardPI(nb,val);
				}
			}
			unsigned int n = nb->getOperator()->getBitwidth()->getSize();
			unsigned int m = ns->getOperator()->getBitwidth()->getSize();
			if(m>n && ns->isOp()) {
				ns->getOperator()->getBitwidth()->reduce(m-n);
				costG = gappa.compute_cost(*_currentManager);
				errorG = costG->bitwidth;
				delete costG;
				if( errorG > errorBound) {
					//do not accept change
					ns->getOperator()->getBitwidth()->increment(m-n);
				} else {
					updateBitwidthTowardPI(ns,m-n);
				}
			}
		} else {
			assert(node->isMul());
			if((node->getLeft()->isMul()&& node->getRight()->isMul())||
			   (node->getLeft()->isAddorSub()&& node->getRight()->isAddorSub())) {
				unsigned int l = val-val/2;
				unsigned int r = val/2;
				//unsigned int n;
				DfgNode* nl =  node->getLeft();
				DfgNode* nr = node->getRight();
				nl->getOperator()->getBitwidth()->reduce(l);
				costG = gappa.compute_cost(*_currentManager);
				errorG = costG->bitwidth;
				delete costG;
				if( errorG > errorBound) {
					//do not accept change
					nl->getOperator()->getBitwidth()->increment(l);
				} else {
					updateBitwidthTowardPI(nl,l);
				}
				nr->getOperator()->getBitwidth()->reduce(r);
				costG = gappa.compute_cost(*_currentManager);
				errorG = costG->bitwidth;
				delete costG;
				if( errorG > errorBound) {
					//do not accept change
					nr->getOperator()->getBitwidth()->increment(r);
				} else {
					updateBitwidthTowardPI(nr,r);
				}
			} else if(!node->getLeft()->isOp()&& !node->getLeft()->isOp()) {
				return;
			} else if(!node->getLeft()->isOp()|| !node->getLeft()->isOp()) {
				DfgNode* m = node->getLeft()->isOp() ? node->getLeft(): node->getRight();
				m->getOperator()->getBitwidth()->reduce(val);
				costG = gappa.compute_cost(*_currentManager);
				errorG = costG->bitwidth;
				delete costG;
				if( errorG > errorBound) {
					//do not accept change
					m->getOperator()->getBitwidth()->increment(val);
				} else {
					updateBitwidthTowardPI(m,val);
				}
			} else {
				DfgNode* m = node->getLeft()->isMul() ? node->getLeft(): node->getRight();
				assert(m->isMul());
				m->getOperator()->getBitwidth()->reduce(val);
				costG = gappa.compute_cost(*_currentManager);
				errorG = costG->bitwidth;
				delete costG;
				if( errorG > errorBound) {
					//do not accept change
					m->getOperator()->getBitwidth()->increment(val);
				} else {
					updateBitwidthTowardPI(m,val);
				}
			}
		}
	}



	/** @brief collects sensitivity information on all Arithmetic DFG operators*/
	void DfgBitReduction::opSensitivity(void) {
		vector <DfgNode* > vNodes;
		int latency = _currentManager->collectDFS(vNodes);
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
		FixCost* costG = gappa.compute_cost(*_currentManager);
		double initError  = costG->bitwidth;
		delete costG;
		if(_report) {
			cout << "Error:  Initial=" << initError << "  maximum allow=" <<  _maxError << endl;
		}
		Cost opcost;
		for(unsigned int index = 1; index <= latency; index++) {
			set<DfgNode*>* tochange = wave[index];
			int i = 1;
			for(set<DfgNode*>::iterator it = tochange->begin(); it != tochange->end(); it++,i++) {
				lastnode = (*it);
				unsigned int bitw = lastnode->_op->getBitwidth()->getSize();
				double error = 0.0;
				bool goOn = true;
				while((error < _maxError)&& goOn) {
					if(lastnode->_op->getBitwidth()->isReducible()) {
						lastnode->_op->getBitwidth()->reduce();
						costG = gappa.compute_cost(*_currentManager);
						error  = costG->bitwidth;
						delete costG;
						if(_report) {
							cout << "[" << index << "/" << latency << "]";
							cout << " " << i << "/" << tochange->size();
							cout << " error=" << error;
							cout << " op=" << lastnode->getOpStr();
							cout << " id=" << lastnode->_op->getID();
							cout << " bitwidth=" << lastnode->_op->getBitwidth()->at()<< "       ";
							flush(cout);
							cout << '\xd';
						}
					} else {
						goOn = false;
					}
				}
				if(error > _maxError) {
					lastnode->_op->getBitwidth()->increment();
					costG = gappa.compute_cost(*_currentManager);
					error  = costG->bitwidth;
					delete costG;
				}
				opcost.bitw = bitw - lastnode->_op->getBitwidth()->getSize();
				opcost.slope = (error-initError)/opcost.bitw;
				opcost.err = _maxError-error;
				if(_report) {
					cout << endl;
#if 0
					cout << "  slope=" << opcost.slope;
					cout << "  bit:reduction=" << opcost.bitw << "  error:inc=" << opcost.slope*opcost.bitw;
					cout << "  error:left=" << opcost.err << endl;
#endif
				}
				if(lastnode->isMul()) {
					_mpySense.insert(pair<Cost,DfgNode*>(opcost,lastnode));
				} else {
					_addSense.insert(pair<Cost,DfgNode*>(opcost,lastnode));
				}
				lastnode->_op->getBitwidth()->increment(opcost.bitw);
				//while(lastnode->_op->getBitwidth()->getSize()< bitw) {
				//	lastnode->_op->getBitwidth()->increment();
				//}
			}
		}
		if(_report) {
			cout << "Sensitivity cost report:" << endl;
			for(Sensitivity::iterator it = _mpySense.begin(); it != _mpySense.end(); it++) {
				opcost = it->first;
				lastnode = it->second;
				cout << lastnode->getOpStr()<< "(id=" <<  lastnode->_op->getID()<< ")";
				cout << "  slope=" << opcost.slope;
				cout << "  bit:reduction=" << opcost.bitw << "  error:inc=" << opcost.slope*opcost.bitw;
				cout << "  error:left=" << opcost.err << endl;
			}
			for(Sensitivity::iterator it = _addSense.begin(); it != _addSense.end(); it++) {
				opcost = it->first;
				lastnode = it->second;
				cout << lastnode->getOpStr()<< "(id=" <<  lastnode->_op->getID()<< ")";
				cout << "  slope=" << opcost.slope;
				cout << "  bit:reduction=" << opcost.bitw << "  error:inc=" << opcost.slope*opcost.bitw;
				cout << "  error:left=" << opcost.err << endl;
			}
		}
		for(unsigned int index = 1; index <= latency; index++) {
			delete wave[index];
		}
	}

}
