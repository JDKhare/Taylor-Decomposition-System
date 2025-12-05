/*
 * =====================================================================================
 *
 *       Filename:  TedOrderDecorator.cc
 *    Description:
 *        Created:  07/11/2011 10:21:36 PM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Environment.h"

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"
#include "TedParents.h"
#include "TedOrderCostCustom.h"

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
	using namespace std;
	using namespace network;
	using namespace convert;

	double TedNodes::stop = 0.0;
	double TedEdges::stop = 0.0;
	double TedEdges0::stop = 0.0;
	double TedEdgesN::stop = 0.0;
	double DfgOpMpy::stop = 0.0;
	double DfgOpAdd::stop = 0.0;
	double DfgScheduleMpyRes::stop = 0.0;
	double DfgScheduleOpAddRes::stop = 0.0;
	double DfgScheduleOpLatency::stop = 0.0;
	double GautMux::stop = 0.0;
	double GautLatency::stop = 0.0;
	double GautArea::stop = 0.0;
	double GautReg::stop = 0.0;


	void CustomCost::report(TedCompareCost* cmpFunc)const {
		const CustomCost* it = this;
		TedCompareDecorator* cmp = dynamic_cast<TedCompareDecorator*>(cmpFunc);
		//int index = 0;
		ostream& out = _out ?*_out : std::cout;
		while(cmp) {
			out << cmp->my_name()<< ":" << it->_value << " ";
			it = it->_next;
			cmp = dynamic_cast<TedCompareDecorator*>(cmp->next());
		}
		//
		// check that the comparison have the same number of elements
		//
		cout << endl;
	}

	void DfgCompareDecorator::compute_dfg(TedMan& manager) {
		if(!*_dfg_man) {
			Convert from(&manager);
			(*_dfg_man) = from.ted2dfgFactor();
			(*_dfg_man)->getNumOperators(**_stat);
		}
	}

	double DfgGappa::get_gappa_value(string filename) {
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
		while(in.getline(buffer,LINE_SIZE)) {
			posBegin = strcspn(ptr,"{");
			if((*ptr!='|')||(posBegin==strlen(ptr))) {
				continue;
			}
			current = ptr+posBegin+1;
			posEnd = strcspn(current,",");
			if(posEnd==strlen(current)) {
				continue;
			}
			current[posEnd] = '\0';
			//cout << "current=" << current << endl;
			value = atof(current);
			if(value != 0.0 && result < value) {
				result = value;
			}
		}
		in.close();
		return result;
	}

	void DfgScheduleCompareDecorator::compute_dfg_arch(TedMan& ted_manager) {
		unsigned int nmul = Environment::getInt("rMPY");
		unsigned int nadd = Environment::getInt("rADD");
		unsigned int nsub = Environment::getInt("rSUB");
		compute_dfg(ted_manager);
		(*_dfg_man)->schedule(nmul,nadd,nsub);
		(*_dfg_man)->scheduleStat(**_stat);
	}

	void GautCompareDecorator::compute_ntl_stat(TedMan& ted_manager) {
		if(*_mux==NULL ||*_latency==NULL ||*_registers==NULL) {
			compute_dfg(ted_manager);
			Netlist* ntl = Tds::getNtlExtracTed();
			if(ntl!=NULL) {
				populateFilenames(ntl->getName());
				optimizeDFG(*_dfg_man);
				Convert fromdfg(*_dfg_man, ntl);
				bool evaluateConstantNodes = Environment::getBool("const_cdfg_eval");
				Netlist* sol = fromdfg.dfg2ntl(evaluateConstantNodes);
				writeCDFG(sol);
				delete sol;
			} else {
				populateFilenames(Environment::getStr("default_design_name").c_str());
				writeCDFG(*_dfg_man);
			}
			Gaut gaut(prefix);
			gaut.run();
			FixCost* cost = gaut.compute_cost();
			assert(cost);
			*_mux = new int(cost->muxes);
			*_latency = new int(cost->latency);
			*_registers = new int(cost->registers);
			*_area = new int(cost->area);
			delete cost;
		}
	}


	void GautCompareDecorator::populateFilenames(const char* default_prefix) {
		// all these parameters are required before writeCDFG and executeGaut in computeNtlStat
		prefix = Util::getPrefix(default_prefix);
		cdfg_file = prefix + ".cdfg";
		cost_file = prefix + ".tds";
		dump_file = prefix + ".txt";
	}

}
