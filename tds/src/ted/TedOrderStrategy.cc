/*
 * =====================================================================================
 *
 *       Filename:  TedOrderStrategy.cc
 *    Description:
 *        Created:  29/4/2009 10:27:16 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 196                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2009-11-18 17:06:42 -0500(Wed, 18 Nov 2009)$: Date of last commit
 * =====================================================================================
 */

#include <bitset>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <list>
#include <map>
#include <set>

using namespace std;

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ATedNode.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedParents.h"
#include "TedOrderStrategy.h"
#include "TedOrderCost.h"

#if defined(_MSC_VER)
#define sprintf sprintf_s
#endif

namespace ted {

	void TedOrderStrategy::printHistory(void) {
		printHistory(std::cout);
	}

	void TedOrderStrategy::printHistory(ostream& out) {
		map<string,unsigned int> hash_name;
		for(TedContainer::const_iterator it = _pManCurrent->getContainer().begin(); it != _pManCurrent->getContainer().end(); it++) {
			hash_name.insert(pair<string,unsigned int>(it->first.getName(),it->second.first));
		}
		for(map<string,unsigned int>::iterator it=hash_name.begin(), end=hash_name.end(); it!=end; it++) {
			out << it->first << ",";
		}
		out << endl;
		while(!history.empty()) {
			string str = history.front();
			history.pop_front();
			out << str << endl;
		}
	}

	void TedOrderStrategy::COST_CHECK(TedCompareCost* cmpFunc, TedMan** pManBest, const Cost& bestCost) {
#ifdef _DEBUG
		Cost* newCostBest = cmpFunc->compute_cost(**pManBest);
		cmpFunc->restart();
		if (bestCost !=*newCostBest) {
			cerr << endl;
			cerr << "Warning: Cost function \"" << cmpFunc->name()<< "\" varies over multiple iterations" << endl;
			cerr << "Warning: attempt 1: " << endl;
			bestCost.report(cmpFunc);
			cerr << "Warning: attempt 2: " << endl;
			newCostBest->report(cmpFunc);
		}
		delete newCostBest;
#endif
	}

	string TedOrderStrategy::recordOrder(TedMan* pMan) {
		string strvars = "";
#if 0
		TedContainerOrder tnodesOrdered = pMan->getContainer().order();
		for(TedContainerOrder::const_iterator it = tnodesOrdered.begin(); it!= tnodesOrdered.end(); it++) {
			TedVar* pvar  = pMan->checkVariables(it->second.first->getName());
			strvars += pvar->getName();
			strvars += " ";
		}
		strvars+=",";
#else
		map<string,unsigned int> hash_name;
		for(TedContainer::const_iterator it = pMan->getContainer().begin(); it != pMan->getContainer().end(); it++) {
			hash_name.insert(pair<string,unsigned int>(it->first.getName(),it->second.first));
		}
		for(map<string,unsigned int>::iterator it=hash_name.begin(), end=hash_name.end(); it!=end; it++) {
			strvars += Util::itoa(it->second);
			strvars += ",";
		}
#endif
		strvars += " ";
		return strvars;
	}

	void TedOrderStrategy::recordHistory(TedMan* pMan, Cost* store_cost, TedCompareCost* cmpFunc) {
		string record = recordOrder(pMan);
		record += store_cost->record();
		if (history.empty()) {
			index_history = history.size();
			best_history = store_cost->clone();
		} else if (cmpFunc->compare(*store_cost,*best_history)) {
			index_history = history.size();
			delete best_history;
			best_history = store_cost->clone();
		}
		history.push_back(record);
	}

	void TedOrderStrategy::COST_CHECK(TedCompareCost* cmpFunc, TedMan** pMan, const Cost& newCost, TedMan** pManBest, const Cost& bestCost) {
		assert(bestCost==newCost);
		COST_CHECK(cmpFunc,pMan,newCost);
		COST_CHECK(cmpFunc,pManBest,bestCost);
	}

	void TedOrderStrategy::updateBestManager(TedCompareCost* cmpFunc, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost) {
		return bestManager(cmpFunc,pMan,pManBest,pManNext,bestCost);
	}

	void TedOrderStrategy::updateBestManagerAnnealing(TedCompareCost* cmpFunc, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost,RNG& rng,double temperature, Cost& start_cost) {
		//For RELOC and SWAP only
		assert(*pManNext &&*pManBest &&*pMan);
		assert(*pManNext !=*pMan);
		assert(*pManNext !=*pManBest);
		if(*pMan !=*pManBest)
			delete*pMan;
		*pMan =*pManNext;
		Cost* pnewCost = cmpFunc->compute_cost(**pMan);
		recordHistory(*pMan,pnewCost,cmpFunc);
		cmpFunc->restart();
		Cost& newCost =*pnewCost;
		bool accept_cost = false;
		if(cmpFunc->compare(newCost,bestCost)) {
			accept_cost = true;
		} else {
			double adjusted_by = cmpFunc->get_cost(bestCost);
			double threshold = exp(-temperature/adjusted_by);
			double random = rng.randomProb();
			accept_cost = (random < threshold);
		}
		if(accept_cost && (newCost != bestCost)) {
			bestCost = newCost;
			delete*pManBest;
			*pManBest =*pMan;
			COST_CHECK(cmpFunc,pMan,newCost,pManBest,bestCost);
		}
		delete pnewCost;
	}

	void TedOrderStrategy::bestManager(TedCompareCost* cmpFunc, TedMan** pMan, TedMan** pManBest, TedMan** pManNext, Cost& bestCost) {
		assert(*pManNext &&*pManBest &&*pMan);
		assert(*pManNext !=*pMan);
		assert(*pManNext !=*pManBest);
		if(*pMan !=*pManBest)
			delete*pMan;
		*pMan =*pManNext;
		updateCurrentManager(*pMan);
		Cost* pnewCost = cmpFunc->compute_cost(**pMan);
		recordHistory(*pMan,pnewCost,cmpFunc);
		cmpFunc->restart();
		Cost& newCost =*pnewCost;
		if(cmpFunc->compare(newCost,bestCost)) {
			bestCost = newCost;
			delete*pManBest;
			*pManBest = this->duplicateBestManager(*pMan);
			COST_CHECK(cmpFunc,pMan,newCost,pManBest,bestCost);
		}
		delete pnewCost;
	}

	void TedOrderStrategy::defineWindow(string lower_var, string upper_var) {
		TedVar* lower = _pManCurrent->checkVariables(lower_var);
		if (lower) {
			_minLevel = _pManCurrent->_container.getLevel(*lower);
		}
		TedVar* upper = _pManCurrent->checkVariables(upper_var);
		if (upper) {
			_maxLevel = _pManCurrent->_container.getLevel(*upper);
		}
		if (_minLevel >= _maxLevel) {
			defaultWindow();
			cout << "Info: the window defined by variables [" << lower_var << "," << upper_var << "] is incorrect," << endl;
			cout << "      the window must be defined as [lower var, upper var]. The window will be dismissed." << endl;
		}
	}

	void TedOrderStrategy::defaultWindow(void) {
		_minLevel = 1;
		_maxLevel = _pManCurrent->_container.getMaxLevel();
	}


	TedMan* TedOrderStrategy::sift(const TedVarGroupOwner& owner, TedCompareCost* cmpFunc) {
		unsigned int minvarlevel = _pManCurrent->_container.getMaxLevel();
		unsigned int maxvarlevel = 0;
		unsigned int indexUpper = 0;
		unsigned int indexLower = 0;
		for(unsigned int i=1; i <= owner.getSize(); i++) {
			unsigned int level = _pManCurrent->_container.getLevel(*(owner.getMember(i)));
			if(level>maxvarlevel) {
				maxvarlevel = level;
				indexUpper = i;
			}
			if(level<minvarlevel) {
				minvarlevel = level;
				indexLower = i;
			}
		}
		if(minvarlevel == 0)
			return NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);

		// ENFORCE DEFINED WINDOW: by default the window encompass all variables
		unsigned int maxlevel = _maxLevel;
		unsigned int minlevel = _minLevel;
		unsigned int midlevel = (maxlevel+minlevel)/2;

		TedMan* pManBest = this->duplicateBestManager(pMan);
		TedMan* pManNext = NULL;
		Cost* bestCost = cmpFunc->compute_cost(*pManBest);
		cmpFunc->restart();
		updateCurrentManager(pMan);
		if(minvarlevel >= midlevel) {
			while(maxvarlevel < maxlevel) {
				pManNext = bblUp(owner);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
				maxvarlevel = pMan->_container.getLevel(*(owner.getMember(indexUpper)));
			}
			while(minvarlevel > minlevel) {
				pManNext = bblDown(owner);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
				minvarlevel = pMan->_container.getLevel(*(owner.getMember(indexLower)));
			}
		} else {
			while(minvarlevel > minlevel) {
				pManNext = bblDown(owner);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
				minvarlevel = pMan->_container.getLevel(*(owner.getMember(indexLower)));
			}
			while(maxvarlevel < maxlevel) {
				pManNext = bblUp(owner);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
				maxvarlevel = pMan->_container.getLevel(*(owner.getMember(indexUpper)));
			}
		}
		recordHistory(pManBest,bestCost,cmpFunc);
		delete bestCost;
		if(pMan != pManBest)
			this->deleteManager(pMan);
		updateCurrentManager(pManBest);
		assert(pManBest==_pManCurrent);
		return pManBest;
	}

	TedMan* TedOrderStrategy::sift(const TedVar& var, TedCompareCost* cmpFunc) {
		if(_pManCurrent->_container.getLevel(var) == 0)
			return NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManNext;

		// ENFORCE DEFINED WINDOW: by default the window encompass all variables
		unsigned int maxlevel = _maxLevel;
		unsigned int minlevel = _minLevel;
		unsigned int midlevel = (maxlevel+minlevel)/2;

		TedMan* pManBest = this->duplicateBestManager(pMan);
		updateCurrentManager(pMan);
		Cost* bestCost = cmpFunc->compute_cost(*pManBest);
		cmpFunc->restart();
		if(pMan->_container.getLevel(var)> midlevel) {
			while(pMan->_container.getLevel(var)< maxlevel) {
				pManNext = bblUp(var);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
			}
			while(pMan->_container.getLevel(var)> minlevel) {
				pManNext = bblDown(var);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
			}
		} else {
			while(pMan->_container.getLevel(var)> minlevel) {
				pManNext = bblDown(var);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
			}
			while(pMan->_container.getLevel(var)< maxlevel) {
				pManNext = bblUp(var);
				this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
			}
		}
		recordHistory(pManBest,bestCost,cmpFunc);
		delete bestCost;
		if(pMan != pManBest)
			this->deleteManager(pMan);
		assert(pManBest!=_pManSteady);
		updateCurrentManager(pManBest);
		assert(pManBest==_pManCurrent);
		return pManBest;
	}


	/** @brief Optimize variable ordering by sifting algorithm*/
	TedMan* TedOrderStrategy::siftAll(TedCompareCost* cmpFunc) {
		TedMan* pMan = _pManCurrent->duplicate();
		TedMan* pManBest = this->duplicateBestManager(pMan);
		updateCurrentManager(pMan);
		Cost* bestCost = cmpFunc->compute_cost(*pManBest);
		cmpFunc->restart();
		Util::changemode(1);
		int index = 0;
		int max = _pManSteady->_container.size();
		bool ended = true;
		for(TedContainer::iterator it = _pManSteady->_container.begin(); it != _pManSteady->_container.end(); it++,index++) {
			if(Util::kbhit()) {
				int esc = getchar();
				if(esc==0x1B) {
					ended = false;
					break;
				}
			}
			if(_progressBar) {
				string msg = "sift(";
				msg += Util::itoa(cmpFunc->get_cost(*bestCost));
				msg += ")";
				Util::progressBar(index,max,msg);
			}
			string varName = it->first.getName();
			TedVar* var = _pManSteady->_vars.getIfExist(varName);
			assert(var);
			unsigned int varLevel = _pManSteady->_container.getLevel(*var);
			// USE CANDIDATES FROM WITHIN THE DEFINED WINDOW
			if ( varLevel >= _minLevel && varLevel <= _maxLevel) {
				TedMan* pManNext = sift(*var, cmpFunc);
				bestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
				assert(pMan==_pManCurrent);
				TedMan::purge_all_but(_pManSteady,pMan,pManBest);
			}
		}
		COST_CHECK(cmpFunc,&pManBest,*bestCost);
		recordHistory(pManBest,bestCost,cmpFunc);
		delete bestCost;
		if(ended&&_progressBar)
			Util::progressBar(max,max);
		if(_progressBar)
			cout << endl;
		Util::changemode(0);
		if(pManBest != pMan)
			delete pMan;
		assert(pManBest!=_pManSteady);
		updateCurrentManager(pManBest);
		return pManBest;
	}

	/** @brief Optimize variable ordering by sifting algorithm, while maintaining the grouped variables as one*/
	TedMan* TedOrderStrategy::siftGroupedAll(TedCompareCost* cmpFunc) {
		TedMan*pManNext;
		TedMan* pMan = _pManCurrent->duplicate();
		TedMan* pManBest = this->duplicateBestManager(pMan);
		updateCurrentManager(pMan);
		Cost* bestCost = cmpFunc->compute_cost(*pManBest);
		//recordHistory(pManBest,bestCost,cmpFunc);
		cmpFunc->restart();
		Util::changemode(1);
		int index = 0;
		int max = _pManSteady->_container.size();
		bool ended = true;
		for(TedContainer::iterator it = _pManSteady->_container.begin(); it != _pManSteady->_container.end(); it++,index++) {
			if(Util::kbhit()) {
				int esc = getchar();
				if(esc==0x1B) {
					ended = false;
					break;
				}
			}
			if(_progressBar) {
				string msg = "sift-grouped-all(";
				msg += Util::itoa(cmpFunc->get_cost(*bestCost));
				msg += ")";
				Util::progressBar(index,max,msg);
			}
			string varName = it->first.getName();
			TedVar* var = _pManSteady->_vars.getIfExist(varName);
			// USE CANDIDATES FROM WITHIN THE DEFINED WINDOW
			unsigned int varLevel = _pManSteady->_container.getLevel(*var);
			if ( varLevel >= _minLevel && varLevel <= _maxLevel) {
				TedVarGroupMember* member = dynamic_cast<TedVarGroupMember*>(var);
				if(member)
					pManNext = sift(*(member->getOwner()),cmpFunc);
				else
					pManNext = sift(*var, cmpFunc);
				bestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
				assert(pMan==_pManCurrent);
				TedMan::purge_all_but(_pManSteady,pMan,pManBest);
			}
		}
		COST_CHECK(cmpFunc,&pManBest,*bestCost);
		recordHistory(pManBest,bestCost,cmpFunc);
		delete bestCost;
		if(ended&&_progressBar)
			Util::progressBar(max,max);
		if(_progressBar)
			cout << endl;
		Util::changemode(0);
		if(pManBest != pMan)
			delete pMan;
		updateCurrentManager(pManBest);
		return pManBest;
	}


	static int factorial(int n) {
		if(n <= 1)
			return 1;
		return n*factorial(n-1);
	}

	namespace {
		/* Johnson Trotter Permutation Enumeration Algorithm:
http://www.cut-the-knot.org/Curriculum/Combinatorics/JohnsonTrotter.shtml
*/
		int _JT_find_largest_mobile(vector<TedOrderStrategy::DINT> & vdindex) {
			int index, target, mobile_index = -1, mobile_val = -1;
			int indexSize = vdindex.size()-1;
			for(index = indexSize; index >= 0; index--) {
				target = vdindex[index].second ? index-1 : index+1;
				if(target < 0 || target >indexSize)
					continue;
				if(vdindex[index].first > vdindex[target].first) {
					if((int)vdindex[index].first > mobile_val) {
						mobile_val = vdindex[index].first;
						mobile_index = index;
					}
				}
			}
			return mobile_index;
		}
		void _JT_update_flags(vector<TedOrderStrategy::DINT> & vdindex,int mobile_val) {
			for(unsigned int j = 0; j < vdindex.size(); j++) {
				if(vdindex[j].first > mobile_val) {
					vdindex[j].second ^= true;
				}
			}
		}
	}


	TedMan* TedOrderStrategy::permute(TedCompareCost* cmpFunc, bool break_into_strides) {
		//TedMan* pManNext = NULL;
		vector< set<TedVar> > strides = getStrides(break_into_strides);
		int stridesize = strides.size();
		for(int i = 0; i < stridesize; i++) {
			set<TedVar>& stride = strides[i];
			permute(cmpFunc,&stride);
		}
		return _pManCurrent;
	}

	TedMan* TedOrderStrategy::permute(TedCompareCost* cmpFunc, set<TedVar>* sVars) {
		list <unsigned int> lIndex;
		vector <string> vVars;
		TedMan* pManNext = NULL;
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManBest = this->duplicateBestManager(pMan);
		if(sVars && !sVars->empty()) {
			for(set<TedVar>::iterator it = sVars->begin(), end = sVars->end(); it != end; it++) {
				vVars.push_back(it->getName());
			}
		} else {
			pMan->getVars(vVars);
		}
		vector <DINT > vdindex;
		DINT diTemp;
		for(unsigned int j = 0; j < vVars.size(); j++) {
			vdindex.push_back(DINT(j, true));
		}
		int index, target, index_down, indexed;
		updateCurrentManager(pMan);
		Cost* bestCost = cmpFunc->compute_cost(*pManBest);
		cmpFunc->restart();
		Util::changemode(1);
		int count = 1;
#if 0
		int max = factorial(_pManSteady->_container.size());
#else
		int max = factorial(vVars.size());
#endif
		int refresh = MIN(MAX(max,100),1000);
		int refresh_count = 0;
		bool ended = true;
		bool abort_on_key = ((vVars.size()> 6)&& !_unstopable) ? true : false;
		if(abort_on_key) {
			cout << "Info: " << factorial(vVars.size())<< " orders. Press ESC to abort" << endl;
		}
		while((index = _JT_find_largest_mobile(vdindex))!= -1) {
			if(abort_on_key && Util::kbhit()) {
				int esc = getchar();
				if(esc == 0x1B) {
					ended = false;
					break;
				}
			}
			if(_progressBar) {
				string msg = "permute(";
				msg += Util::itoa(cmpFunc->get_cost(*bestCost));
				msg += ")";
				Util::progressBar(count,max,msg);
			}
			count++;
			if(vdindex[index].second) {
				target = index-1;
				index_down = target;
				indexed = index;
			} else {
				target = index+1;
				index_down = index;
				indexed = target;
			}
			TedVar* pvar = _pManSteady->_vars.getIfExist(vVars[index_down]);
			assert(pvar);
			TedVar* pdest = _pManSteady->_vars.getIfExist(vVars[indexed]);
			assert(pdest);
			assert(pvar!=pdest);
			pManNext = exchange(*pvar,*pdest);
			assert(pManNext);
			_JT_update_flags(vdindex,vdindex[index].first);
			diTemp = vdindex[index];
			vdindex[index] = vdindex[target];
			vdindex[target] = diTemp;
			string pTemp(vVars[index_down]);
			vVars[index_down] = vVars[index_down+1];
			vVars[index_down+1] = pTemp;
			this->updateBestManager(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost);
			refresh_count++;
			if (refresh_count>refresh) {
				refresh_count=0;
				TedMan::purge_all_but(pMan,pManBest,_pManSteady);
			}
			if (cmpFunc->get_cost(*bestCost) <= stop_criteria()) {
				cout << " ...stop criteria reached (" << stop_criteria() << ")" << endl;
				break;
			}
		}
		recordHistory(pManBest,bestCost,cmpFunc);
		delete bestCost;
		if(ended&&_progressBar)
			Util::progressBar(max,max);
		Util::changemode(0);
		if(_progressBar)
			cout << endl;
		if(pManBest != pMan)
			this->deleteManager(pMan);
		updateCurrentManager(pManBest);
		return pManBest;
	}

	bool TedOrderStrategy::stopAnnealing(const Cost& c1,const Cost& c2,const Cost& c3,const Cost& c4, long temperature) {
		if(STOP_ANNEALING < temperature) {
			return false;
		} else if((c1 == c2)&&(c1 == c3)&&(c1 == c4)) {
			return true;
		} else {
			return false;
		}
	}

	TedMan* TedOrderStrategy::annealing(TedCompareCost* cmpFunc, bool break_into_strides = true, bool always_backtrack=false) {
		vector< set<TedVar> > strides = getStrides(break_into_strides);
		int stridesize = strides.size();
		for(int i = 0; i < stridesize; i++) {
			set<TedVar>& stride = strides[i];
			bool enable_backtrack = ((i+1) ==stridesize) || always_backtrack;
			if(stride.size()<7)
				permute(cmpFunc,&stride);
			else
				annealing(cmpFunc,&stride,enable_backtrack);
		}
		return _pManCurrent;
	}

	TedMan* TedOrderStrategy::annealing(TedCompareCost* cmpFunc, set<TedVar>* sVars, bool enable_backtrack) {
		static const double BETA = 0.57;
		static const double ALPHA = 0.85;
		static const unsigned int RATIO = 7;
		static const unsigned int COST_ADJ = 9;
		static const double PROB_EX = 0.40;
		static const double PROB_JA = 0.35;
		static const double PROB_JB = 0.25;
		assert(PROB_EX+PROB_JA+PROB_JB==1.0);

		unsigned int maxlevel = sVars->size()-1;
		vector <string> vVars;
		if(sVars && !sVars->empty()) {
			for(set<TedVar>::iterator it = sVars->begin(), end = sVars->end(); it != end; it++) {
				vVars.push_back(it->getName());
			}
		} else {
			_pManSteady->getVars(vVars);
		}

		TedMan* pManNext = NULL;
		//
		// Avoid sifting at all in here,
		// let's focus in annealing only
		// if sifting is needed, let the user ask for it.
		//
		TedMan* pMan = this->duplicateManager(_pManCurrent);
		TedMan* pManBest = this->duplicateBestManager(pMan);
		updateCurrentManager(pMan);
		assert(pManBest);

		Cost* bestCost = cmpFunc->compute_cost(*pManBest);
		Cost* start_cost = bestCost->clone();
		recordHistory(pManBest,start_cost,cmpFunc);

		unsigned int max = maxlevel*RATIO;
		double temperature = cmpFunc->get_cost(*bestCost)*BETA;
		unsigned int maxT = temperature*100;
		double newTemperature = 0.0;

		cmpFunc->restart();
		Cost* c1 = bestCost->clone();
		(*c1)+= COST_ADJ;
		Cost* c2 = c1->clone();
		(*c2)+= COST_ADJ;
		Cost* c3 = bestCost->clone();
		Cost* c4 = c2->clone();
		(*c4)+= COST_ADJ;
		if(_progressBar) {
			char msg[40];
			sprintf(msg,"T=%-4.2f Window=%-5d Cost=%-4.2f",temperature,max,cmpFunc->get_cost(*bestCost));
			string buf(msg);
			Util::progressBar(temperature,maxT,buf);
		}

		RNG rng;

		while(!stopAnnealing(*c1,*c2,*c3,*c4,temperature)) {
			unsigned int old_index1 = 0;
			unsigned int old_index2 = 0;
			for(unsigned int inner = 0; inner < max; inner++) {
				unsigned int index1 = rng.randomInt(maxlevel);
				if(index1==0) {
					index1 = maxlevel;
				}
				unsigned int index2 = index1;
				while(index1==index2 || (index1==old_index1) && (index2==old_index2) || (index1==old_index2) && (index2==old_index1)) {
					index2 = rng.randomInt(maxlevel);
					if(index2==0) {
						index2 = maxlevel;
					}
				}
				const TedVar& var1 =*_pManSteady->_vars.getIfExist(vVars[index1]);
				const TedVar& var2 =*_pManSteady->_vars.getIfExist(vVars[index2]);
				unsigned long op = rng.randomProb();
				if(op<PROB_EX) {
					//exchange
					pManNext = exchange(var1,var2);
				} else if(op < PROB_EX+PROB_JA) {
					//jump above
					pManNext = jumpAbove(var1,var2);
				} else {
					//jump below
					pManNext = jumpBelow(var1,var2);
				}
				assert(pManNext);
				assert(pManNext->getContainer().check_children_are_included());
				old_index1 = index1;
				old_index2 = index2;
				updateBestManagerAnnealing(cmpFunc,&pMan,&pManBest,&pManNext,*bestCost,rng,temperature,*start_cost);
			}
			TedMan::purge_all_but(_pManSteady,pMan,pManBest);
			(*c1) = (*c2);
			(*c2) = (*c3);
			(*c3) = (*c4);
			(*c4) = (*bestCost);
			newTemperature = ALPHA*temperature;
			if(newTemperature >= STOP_ANNEALING) {
				double newMax = log(ALPHA*newTemperature)/log(temperature)*max;
				max = ceil(newMax);
			}
			temperature = newTemperature;
			if(_progressBar) {
				char msg[40];
				sprintf(msg,"T=%-4.2f Window=%-5d Cost=%-4.2f   ",temperature,max,cmpFunc->get_cost(*bestCost));
				string buf(msg);
				Util::progressBar(maxT-100*temperature,maxT,buf);
			}
		}
		recordHistory(pManBest,bestCost,cmpFunc);
		bool do_backtrack = (cmpFunc->compare(*best_history,*bestCost));
		delete bestCost;
		delete c1;
		delete c2;
		delete c3;
		delete c4;
		if(_progressBar) {
			Util::progressBar(maxT,maxT);
			cout << "                    " << endl;
		}
		if(pManBest != pMan)
			this->deleteManager(pMan);
		updateCurrentManager(pManBest);
		TedMan::purge_all_but(pManBest,_pManSteady);
		if (enable_backtrack && do_backtrack) {
			cout << "Info: backtracking previous result with cost " << cmpFunc->get_cost(*best_history) << endl;
			list<string>::iterator history_it = history.begin();
			for(int i = 0; i < index_history; i++, history_it++);
			string line =*history_it;
			vector<string> var_names = Util::split(line,",");
			map<string,TedVar> hash_var;
			for(TedContainer::const_iterator it = _pManCurrent->getContainer().begin(); it != _pManCurrent->getContainer().end(); it++) {
				hash_var.insert(pair<string,TedVar>(it->first.getName(),it->first));
			}
			map<int,TedVar> recoverOrder;
			int var_index = 0;
			for (map<string,TedVar>::iterator it = hash_var.begin(); it != hash_var.end(); it++, var_index++) {
				pair<bool,int> index = Util::atoi(var_names[var_index].c_str());
				assert(index.first);
				assert(index.second < hash_var.size());
				recoverOrder.insert(pair<int,TedVar>(index.second,it->second));
			}
			//cout << "Info: previous order found" << endl;
			//
			// WARNING: only works for PROPER ordering
			// all other orderings (reloc, swap) will get errors
			//
			for (map<int,TedVar>::iterator it = recoverOrder.begin(); it != recoverOrder.end(); it++) {
				toBottom(it->second);
			}
			//cout << "Info: best order restablished" << endl;
			//	Cost* backtrackCost = cmpFunc->compute_cost(*pManBest);
			//	assert(*backtrackCost ==*best_history);
		}
		return pManBest;
	}

	vector< set<TedVar> > TedOrderStrategy::getStrides(bool break_mo_into_strides) {
		if (break_mo_into_strides) {
			return getStrides();
		} else {
			set<TedVar> all_vars;
			for (TedContainer::iterator it = _pManSteady->getContainer().begin(), end = _pManSteady->getContainer().end(); it != end; it++) {
				// ENFORCE WINDOW SIZE
				unsigned int varLevel = _pManSteady->getContainer().getLevel(it->first);
				if (varLevel >= _minLevel && varLevel <= _maxLevel) {
					all_vars.insert(it->first);
				}
			}
			vector< set<TedVar> > one_stride;
			one_stride.push_back(all_vars);
			return one_stride;
		}
	}

	//Traverses all POs and collects for each PO the support variables
	vector< set<TedVar> > TedOrderStrategy::getStrides(void) {
		map<const TedNode*, set< TedVar > > varlist;
		const int var_size = 1024;
		const int max_level = _pManSteady->getContainer().getMaxLevel();
		assert(max_level<var_size);
		vector < bitset<var_size> > vb;
		vector < set<TedVar> > vs;

		for(TedMan::PrimaryOutputs::iterator p = _pManSteady->getPOs().begin(); p != _pManSteady->getPOs().end(); p++) {
			const TedNode* pNode = p->second.Node();
			getStrides(pNode,varlist);
			bitset<var_size> bs;
			set<TedVar> st = varlist[pNode];
			for(set<TedVar>::iterator it = st.begin(), end = st.end(); it != end; it++) {
				unsigned int level = _pManSteady->getContainer().getLevel(*it);
				bs.set(level);
			}
			if(bs.count() == (max_level-1)) {
				//there is one PO that uses all variables
				//level 0 correspond to variable ONE which is never used
				vb.clear();
				vs.clear();
				vs.push_back(st);
				return vs;
			}
			bool append = true;
			bool found = false;
			if(!vb.empty()) {
				vector< bitset<var_size> >::iterator it = vb.begin();
				vector< bitset<var_size> >::iterator it_end = vb.end();
				vector< set<TedVar> >::iterator jt = vs.begin();
				vector< set<TedVar> >::iterator jt_end = vs.end();
				for(int index=0; it != it_end; it++,jt++,index++) {
					bitset<var_size> test =*it;
					if(test == bs) {
						append = false;
						if (found) {
							// a previous equal stride was found
							// current iterator is a duplicate
							vb.erase(it);
							vs.erase(jt);
						}
						found = true;
						//					break;
					} else {
						bitset<var_size> discriminant = (test ^ bs);
						bitset<var_size> test_contains_bs = (discriminant & test);
						bitset<var_size> bs_contains_test = (discriminant & bs);
						if(test_contains_bs == discriminant) {
							// test contains bs
							append = false;
							if (found) {
								vb.erase(it);
								vs.erase(jt);
							} else {
								//continue the search for duplicates
								//with the most complete stride
								bs =*it;
								st =*jt;
							}
							found = true;
							//						break;
						} else if(bs_contains_test == discriminant) {
							//bs contains test
							append = false;
							if (found) {
								vb.erase(it);
								vs.erase(jt);
							} else {
								//replace in place
								vb[index] = bs;
								vs[index] = st;
							}
							//break;
						}
					}

				}
			}
			if(append) {
				vb.push_back(bs);
				vs.push_back(st);
			}
		}
		for(int i = 0; i <vb.size(); i++) {
			string strade = vb[i].to_string();
			cout << "strade="<< strade.substr(var_size-max_level-1)<< endl;
		}
		return vs;
	}

	//recursively traverse a node annotating its support
	void TedOrderStrategy::getStrides(const TedNode* pNode, map<const TedNode*,set<TedVar> >& varlist) {
		if(varlist.find(pNode)!= varlist.end()|| pNode == TedNode::getOne()) {
			return;
		}
		set<TedVar> newVars;
		// ENFORCE WINDOW SIZE
		unsigned int level = _pManSteady->getContainer().getLevel(*pNode->getVar());
		if (level >= _minLevel && level <= _maxLevel) {
			newVars.insert(*pNode->getVar());
		}
		//unsigned int index=0;
		TedNode* pKid=NULL;
		//int weight=0;
		FOREACH_KID_OF_NODE(pNode) {
			pKid = _iterkids.Node<TedNode>();
			getStrides(pKid,varlist);
			newVars.insert(varlist[pKid].begin(), varlist[pKid].end());
		}
		varlist.insert(pair<const TedNode*,set<TedVar> >(pNode,newVars));
	}

	TedOrderStrategy::~TedOrderStrategy(void) {
		if(best_history)
			delete best_history;
	}

}
