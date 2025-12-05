/*
 * =====================================================================================
 *
 *       Filename:  TedOrderCost.cc
 *    Description:
 *        Created:  29/4/2009 12:45:16 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

#include "Environment.h"

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedParents.h"
#include "TedOrderCost.h"

#include "Bitwidth.h"
#include "util.h"
#include "Convert.h"
#include "Ntk.h"
#include "Gaut.h"
#include "Tds.h"

namespace ted {

	using util::Util;
	using data::Bitwidth;
	using namespace tds;
	using namespace convert;
	using namespace network;

	Cost::CompareType Cost::_type = Cost::STRICTLESS;

	FixCost* MinNodes::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		cost->nodes = manager.getContainer().nodeCount()*10 - manager.getNumOfCandidates();
		return cost;
	}

	bool MinNodes::compare_fix(const FixCost& left, const FixCost& right)const {
		return Cost::compare(left.nodes,right.nodes);
	}

	FixCost* MinBitwidth::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		manager.computeBitwidth();
		cost->nodes = manager.getContainer().nodeCount();
		cost->normB = manager.getMaxBitwidth()->norm();
		return cost;
	}

	bool MinBitwidth::compare_fix(const FixCost& left, const FixCost& right)const {
		if(left.normB < right.normB) {
			return true;
		} else if(left.normB > right.normB) {
			return false;
		} else {
			return Cost::compare(left.nodes,right.nodes);
		}
	}

	void DfgCost::computeDfgStat(TedMan& manager, DfgStat& stat) {
		//DfgMan* pDfgMan = new DfgMan(&manager);
		Convert from(&manager);
		DfgMan* pDfgMan = from.ted2dfgFactor();
		pDfgMan->getNumOperators(stat);
		delete pDfgMan;
	}

	FixCost* MinMpyCount::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		computeDfgStat(manager,cost->stat);
		unsigned int candidates = manager.getNumOfCandidates();
		cost->normB = cost->stat.nMul*10 - candidates;
		return cost;
	}

	bool MinMpyCount::compare_fix(const FixCost& left, const FixCost& right)const {
		return Cost::compare(left.normB,right.normB);
	}

	FixCost* MinOpCount::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		computeDfgStat(manager,cost->stat);
		unsigned int candidates = manager.getNumOfCandidates();
		cost->normB = (cost->stat.nMul+cost->stat.nAdd+cost->stat.nSub)*10 - candidates;
		return cost;
	}

	bool MinOpCount::compare_fix(const FixCost& left, const FixCost& right)const {
		return Cost::compare(left.normB,right.normB);
	}

	void ScheduleCost::computeScheduleStat(TedMan& manager, DfgStat& stat) {
		Convert from(&manager);
		DfgMan* pDfgMan = from.ted2dfgFactor();
		pDfgMan->schedule(Environment::getInt("rMPY"),Environment::getInt("rADD"),Environment::getInt("rSUB"));
		pDfgMan->scheduleStat(stat);
		delete pDfgMan;
	}

	FixCost* MinOperations::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		computeScheduleStat(manager,cost->stat);
		cost->nodes = manager.getContainer().nodeCount();
		return cost;
	}

	bool MinOperations::compare_fix(const FixCost& left, const FixCost& right)const {
		//Minimize number of operations as first objective
		if(left.stat.nMul > right.stat.nMul) {
			return false;
		} else if(left.stat.nMul < right.stat.nMul) {
			return true;
		} else if((left.stat.nAdd + left.stat.nSub)<(right.stat.nAdd + right.stat.nSub)) {
			return true;
		} else if((left.stat.nAdd + left.stat.nSub)>(right.stat.nAdd + right.stat.nSub)) {
			return false;
		} else {
			//if we are equal in operations count, minimize latency
			if(left.stat.latency < right.stat.latency) {
				return true;
			} else if(left.stat.latency > right.stat.latency) {
				return false;
			} else {
				//if we are equal in latency & operations count, minimize resources
				if(left.stat.rMPY < right.stat.rMPY) {
					return true;
				} else if(left.stat.rMPY > right.stat.rMPY) {
					return false;
				} else if(left.stat.rADD < right.stat.rADD) {
					return true;
				} else if(left.stat.rADD > right.stat.rADD) {
					return false;
				} else {
					//all metrics are the same
					return Cost::decide();
				}
			}
		}
	}

	FixCost* MinLatency::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		computeScheduleStat(manager,cost->stat);
		return cost;
	}

	bool MinLatency::compare_fix(const FixCost& left, const FixCost& right)const {
		//Minimize latency as first objective
		if(left.stat.latency < right.stat.latency) {
			return true;
		} else if(left.stat.latency > right.stat.latency) {
			return false;
		} else {
			//if we are equal in latency, minimize resources
			if(left.stat.rMPY < right.stat.rMPY) {
				return true;
			} else if(left.stat.rMPY > right.stat.rMPY) {
				return false;
			} else if(left.stat.rADD < right.stat.rADD) {
				return true;
			} else if(left.stat.rADD > right.stat.rADD) {
				return false;
			} else {
				//if we are equal in latency & resources, minimize operands count
				if(left.stat.nMul < right.stat.nMul) {
					return true;
				} else if(left.stat.nMul > right.stat.nMul) {
					return false;
				} else if((left.stat.nAdd + left.stat.nSub)<(right.stat.nAdd + right.stat.nSub)) {
					return true;
				} else if((left.stat.nAdd + left.stat.nSub)>(right.stat.nAdd + right.stat.nSub)) {
					return false;
				} else {
					//all metrics are the same
					return Cost::decide();
				}
			}
		}
	}

	FixCost* MinGappaBound::compute_fix_cost(TedMan& manager) {
		Convert from(&manager);
		DfgMan* pDfgMan = from.ted2dfgFactor();
		//DfgMan* pDfgMan = new DfgMan(&manager);
		pDfgMan->computeBitwidth();
		FixCost* cost = compute_fix_cost(*pDfgMan);
		delete pDfgMan;
		return cost;
	}

	FixCost* MinGappaBound::compute_fix_cost(DfgMan& manager) {
		manager.writeGappa("ted.gappa");
		system("gappa < ted.gappa 2>ted.gappa.out");
		FixCost* cost = new FixCost();
		cost->bitwidth = getValue("ted.gappa.out");
		return cost;
	}


	bool MinGappaBound::compare_fix(const FixCost& left, const FixCost& right)const {
		return Cost::compare(left.bitwidth,right.bitwidth);
	}

	double MinGappaBound::getValue(string filename) {
		static const int LINE_SIZE = 65535;
		static char buffer[LINE_SIZE];
		char* ptr = buffer;
		char* current = NULL;
		int posBegin = 0;
		int posEnd = 0;
		double result = -1;
		double value = -1;
		ifstream in;
		in.open(filename.c_str());
		/*  file format example:
		 *  Results for a in [0, 65535] and b in [0, 65535] and c in [0, 65535] and d in [0, 65535] and \
		 *  e in [0, 65535] and f in [0, 65535] and g in [0, 65535] and h in [0,65535]:
		 *	|mul1 - Mmul1| in [0, 8589803521b-28 {31.9995, 2^(4.99998)}]
		 */
		while(in.getline(buffer,LINE_SIZE)) {
			posBegin = strcspn(ptr,"{");
			if((*ptr!='|')||(posBegin==strlen(ptr))) {
				continue;
			}
			// only read the line that starts with '|'
			// and that contains '{'
			current = ptr+posBegin+1;
			posEnd = strcspn(current,",");
			if(posEnd==strlen(current)) {
				continue;
			}
			current[posEnd] = '\0';
			value = atof(current);
			if(value != 0.0 && result < value) {
				result = value;
			}
		}
		in.close();
		return result;
	}

	FixCost* TedScheduleCost::compute_fix_cost(TedMan& manager) {
		TedMan* tman = manager.decomposeAll(false);
		FixCost* cost = new FixCost();
		assert(false);
		// nothing has been computed for the cost
		return cost;
	}

	bool TedScheduleCost::compare_fix(const FixCost& left, const FixCost& right)const {
		//Minimize latency as first objective
		if(left.stat.latency < right.stat.latency) {
			return true;
		} else if(left.stat.latency > right.stat.latency) {
			return false;
		} else {
			//if we are equal in latency, minimize resources
			if(left.stat.rMPY < right.stat.rMPY) {
				return true;
			} else if(left.stat.rMPY > right.stat.rMPY) {
				return false;
			} else if(left.stat.rADD < right.stat.rADD) {
				return true;
			} else if(left.stat.rADD > right.stat.rADD) {
				return false;
			} else {
				//if we are equal in latency & resources, minimize operands count
				return Cost::decide();
			}
		}
	}


	FixCost* ReportCost::compute_fix_cost(TedMan& manager) {
		FixCost* cost = new FixCost();
		computeScheduleStat(manager,cost->stat);
		cost->nodes = manager.getContainer().nodeCount();
		cost->normB = manager.getNumOfCandidates();
		try {
			manager.computeBitwidth();
			cost->bitwidth = manager.getMaxBitwidth()->norm();
		} catch(...) {
			cost->bitwidth = 0;
		}
		NtlMinArch gaut_cost;
		FixCost* costB = gaut_cost.compute_cost(manager);
		cost->muxes = costB->muxes;
		cost->latency = costB->latency;
		cost->registers = costB->registers;
		cost->area = costB->area;
		delete costB;
		return cost;
	}

	void FixCost::report(void)const {
		ostream& out = (_out) ?*_out : std::cout;
		out << "|            TED             |        DFG         |        Schedule       |            Gaut                   |" << endl;
		out << "|----------------------------|--------------------|-----------------------|-----------------------------------|" << endl;
		out << "| nodes | factors | bitwidth | nMUL | nADD | nSUB | latency | rMPY | rADD | Muxes | latency | Register | Area |" << endl;
		out << "| ";
		out.width(5);
		out.fill(' ');
		out << ((nodes==INT_MAX) ? 0 : nodes) << " | ";
		out.width(7);
		out << ((normB==LONG_MAX) ? 0 : normB) << " | ";
		out.width(8);
		out << bitwidth << " | ";
		out.width(4);
		out << stat.nMul << " | ";
		out.width(4);
		out << stat.nAdd << " | ";
		out.width(4);
		out << stat.nSub << " | ";
		out.width(7);
		out << stat.latency << " | ";
		out.width(4);
		out << stat.rMPY << " | ";
		out.width(4);
		out << stat.rADD << " | ";
		out.width(5);
		out << ((muxes==INT_MAX) ? 0 : muxes) << " | ";
		out.width(7);
		out << ((latency==INT_MAX) ? 0 : latency) << " | ";
		out.width(8);
		out << ((registers==INT_MAX) ? 0 : registers) << " | ";
		out.width(4);
		out << ((area==INT_MAX) ? 0 : area) << " |";
	}

	void NtlCost::populateFilenames(const char* default_prefix) {
		//
		// all these parameters are required before writeCDFG and compute_fix_cost
		//
		prefix = Util::getPrefix(default_prefix);
#if 0
		//prefix = default_prefix;
		static int index = 0;
		prefix += Util::itoa(index++);
#endif
		cdfg_file = prefix + ".cdfg";
		cost_file = prefix + ".tds";
		dump_file = prefix + ".txt";
	}

	void NtlCost::computeNtlStat(TedMan& manager) {
		Convert fromted(&manager);
		DfgMan* pDfgMan = fromted.ted2dfgFactor();
		Netlist* ntl = Tds::getNtlExtracTed();
		if(ntl!=NULL) {
			populateFilenames(ntl->getName());
			optimizeDFG(pDfgMan);
			Convert fromdfg(pDfgMan, ntl);
			bool evaluateConstantNodes = Environment::getBool("const_cdfg_eval");
			Netlist* sol = fromdfg.dfg2ntl(evaluateConstantNodes);
			writeCDFG(sol);
			delete sol;
		} else {
			populateFilenames(Environment::getStr("default_design_name").c_str());
			writeCDFG(pDfgMan);
		}
		delete pDfgMan;
	}

	void NtlCost::optimizeDFG(DfgMan* dfg) {
		dfg->strash();
	}

	void NtlCost::writeCDFG(Netlist* ntl) {
		ntl->writeCDFG(cdfg_file.c_str());
	}

	void NtlCost::writeCDFG(DfgMan* dfg) {
		dfg->writeCDFG(cdfg_file.c_str());
	}

	FixCost* NtlCost::compute_fix_cost(TedMan& manager) {
		computeNtlStat(manager);
		Gaut gaut(prefix);
		gaut.run();
		return gaut.compute_cost();
	}

	bool NtlMinMux::compare_fix(const FixCost& left, const FixCost& right)const {
		if(left.muxes < right.muxes) {
			return true;
		} else if(left.muxes > right.muxes) {
			return false;
		} else {
			return Cost::compare(left.latency,right.latency);
		}
	}

	void NtlMinLatency::optimizeDFG(DfgMan* dfg) {
		//	dfg->balance();
		dfg->strash();
	}

#if 0
	void NtlMinLatency::writeCDFG(Netlist* ntl) {
		Netlist* ntl2 = ntl->balance();
		ntl2->writeCDFG(cdfg_file.c_str());
		if(ntl2 != ntl)
			delete ntl2;
	}
#endif

	void NtlMinLatency::writeCDFG(DfgMan* dfg) {
		dfg->balance();
		dfg->writeCDFG(cdfg_file.c_str());
	}

	bool NtlMinLatency::compare_fix(const FixCost& left, const FixCost& right)const {
		if(left.latency < right.latency) {
			return true;
		} else if(left.latency > right.latency) {
			return false;
		} else {
			return Cost::compare(left.muxes,right.muxes);
		}
	}

	bool NtlMinArch::compare_fix(const FixCost& left, const FixCost& right)const {
		if(left.muxes < right.muxes) {
			if(left.latency <= right.latency) {
				return true;
			} else {
				return false;
				if((left.muxes - right.muxes)>(right.latency - left.latency)*20) {
					return true;
				} else {
					return false;
				}
			}
		} else if(left.muxes > right.muxes || left.latency > right.latency) {
			return false;
		} else {
			return left.latency < right.latency;
		}
	}

}
