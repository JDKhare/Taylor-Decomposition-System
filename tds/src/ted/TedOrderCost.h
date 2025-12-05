/*
 * =====================================================================================
 *
 *       Filename:  TedOrderCost.h
 *    Description:
 *        Created:  4/29/2009 12:25:50 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDORDERCOST_H__
#define __TEDORDERCOST_H__

#include "DfgMan.h"
#include "TedMan.h"
#include "RNGenerator.h"
#include "Ntk.h"

#if defined(_MSC_VER)
#define sprintf sprintf_s
#endif

namespace ted {

	using dtl::RNG;
	using network::Netlist;
	using namespace dfg;

	class TedCompareCost;

	/** @brief Object used for comparison*/
	class Cost {
	public:
		enum CompareType { STRICTLESS, LESSEQUAL, LESSEQUALRANDOM};
	protected:
		static CompareType _type;
		ostream* _out;
	public:

		template<typename T>
			static bool compareStrict(T t1, T t2) { return t1<t2; }
		template<typename T>
			static bool compareLE(T t1, T t2) { return t1 <= t2; }
		template<typename T>
			static bool compare(T t1, T t2) { return(_type==STRICTLESS) ? compareStrict<T>(t1,t2): compareLE<T>(t1,t2); }

		static bool decide(void) {
			switch(_type) {
			case LESSEQUAL: return true;
			case LESSEQUALRANDOM: return(RNG::generate()< 0.5);
			case STRICTLESS: //fallthrough
			default: return false;
			}
		}
		static void setCompare(CompareType t) { _type = t; }

		Cost(void) : _out(NULL) {}
		virtual ~Cost(void) { _out = NULL; }

		virtual Cost* clone(void) = 0;
		virtual Cost& operator+= (unsigned int val) = 0;
		virtual Cost& operator*= (unsigned int val) = 0;
		virtual Cost& operator/= (unsigned int val) = 0;
		//virtual bool operator<(const Cost& el2)const = 0;
		//virtual bool operator<= (const Cost& el2)const = 0;
		void set_stream(ostream* out) { _out = out; }
		virtual bool operator== (const Cost& other)const = 0;
		virtual void report(void) const = 0;
		virtual string record(void) const = 0;
		virtual void report(TedCompareCost* cmpFunc) const { report(); }
		virtual Cost& operator= (const Cost& other) = 0;
		bool operator!= (const Cost& other)const {
			return !(*this==other);
		}

		virtual double get_cost(void) const = 0;

	};

	/** @brief Object used for comparison*/
	class FixCost : public Cost {
	public:
		size_t nodes;
		long normB;
		double bitwidth;
		int muxes;
		int latency;
		int registers;
		int area;
		DfgStat stat;
	public:

		double get_cost(void) const { return nodes; }

		string record(void) const {
			static char buf[100];
			sprintf(buf,"%d,%ld,%f,""%d,%d,%d,%d,%d,%d,%d,%d,%d,""%d,%d,%d",nodes,normB,bitwidth, \
					stat.nMul, stat.nMulConst, stat.nAdd, stat.nAddConst, stat.nSub, stat.nSubConst, stat.latency, stat.rMPY, stat.rADD, \
					muxes,latency,registers);
			return string(buf);
		}

		FixCost(void): Cost(), nodes(INT_MAX), normB(LONG_MAX), bitwidth(0.0), muxes(INT_MAX), latency(INT_MAX), registers(INT_MAX), area(INT_MAX) {}
		FixCost(const FixCost& other):
			Cost(), nodes(other.nodes), normB(other.normB), bitwidth(other.bitwidth),
			muxes(other.muxes), latency(other.latency), registers(other.registers), area(other.area), stat(other.stat) {}

		FixCost* clone(void) {
			return new FixCost(*this);
		}

		virtual Cost& operator= (const Cost& other) {
			if(this == &other) {
				return*this;
			}
			const FixCost* ptr = dynamic_cast<const FixCost*>(&other);
			if(ptr) {
				nodes = ptr->nodes;
				normB = ptr->normB;
				bitwidth = ptr->bitwidth;
				muxes = ptr->muxes;
				latency = ptr->latency;
				registers = ptr->registers;
				area = ptr->area;
				stat = ptr->stat;
			}
			return(*this);
		}

		FixCost& operator+= (unsigned int val) {
			nodes += val;
			normB += val;
			bitwidth += val;
			muxes += val;
			latency += val;
			registers += val;
			area += val;
			stat += val;
			return (*this);
		}
		FixCost& operator*= (unsigned int val) {
			nodes*= val;
			normB*= val;
			bitwidth*= val;
			muxes*= val;
			latency*= val;
			registers*= val;
			area*=val;
			stat*= val;
			return (*this);
		}
		FixCost& operator/= (unsigned int val) {
			nodes /= val;
			normB /= val;
			bitwidth /= val;
			muxes /= val;
			latency /= val;
			registers /= val;
			area /= val;
			stat /= val;
			return (*this);
		}

		const FixCost operator+(unsigned int val)const {
			return FixCost(*this)+= val;
		}
		const FixCost operator*(unsigned int val)const {
			return FixCost(*this)*= val;
		}
		const FixCost operator/(unsigned int val)const {
			return FixCost(*this)/= val;
		}

		bool operator== (const Cost& other)const {
			const FixCost* ptr = dynamic_cast<const FixCost*>(&other);
			if(ptr) {
				return(*this==*ptr);
			}
			return false;
		}
		bool operator== (const FixCost& other)const {
			return(nodes==other.nodes)&&(normB==other.normB)&&(bitwidth==other.bitwidth)&&(muxes==other.muxes)&&(latency==other.latency)&&(registers==other.registers)&&(area==other.area)&&(stat==other.stat);
		}

		void report(void)const;
	};

	/** @brief Abstract Base Class used for TED minimization
	 *  @details
	 *  This base class is used for automatic selecting the appropiate inherited comparison
	 *  method operator(). Therefore we can do the comparison as:
	 *     -(*TedOrderCostPtr)(leftManager,rightManager)
	 *     -(*TedOrderCostPtr)((*TedOrderCostPtr)(leftManager),FixCostStored)
	 *  an approach such as:
	 *     -(*TedOrderCostPtr(leftManager))<(*TedOrderCostPtr(rightManager))
	 *  can also work if the appropiate less-than method is defined for each TedOrderCostPtr,
	 *  this is we will have to have a template class FixCost being instantiated for each
	 *  TedOrderCostPtr.
	 **/
	class TedCompareCost {
	public:
		double _stop;

		TedCompareCost(void) : _stop(0.0) {};
		virtual ~TedCompareCost(void) {};

		virtual void restart(void) = 0;
		virtual bool compare(const Cost& t1, const Cost& t2)const = 0;
		virtual Cost* compute_cost(TedMan& manager) = 0;
		virtual double get_cost(const Cost& ref) = 0;
		virtual string name(void) = 0;
		virtual double stop_criteria(void) const { return _stop; }
		void stop_criteria(double stop) { _stop = stop; }
	};

	class TedOrderCost : public TedCompareCost {
	public:
		TedOrderCost(void): TedCompareCost() {};
		~TedOrderCost(void) {};

		virtual FixCost* compute_fix_cost(TedMan& manager) = 0;
		FixCost* compute_cost(TedMan& manager) { return compute_fix_cost(manager); }

		virtual bool compare_fix(const FixCost& left, const FixCost& right)const = 0;

		void restart(void) {}
		bool compare(const Cost& left, const Cost& right)const {
			const FixCost* l_ptr = dynamic_cast<const FixCost*>(&left);
			const FixCost* r_ptr = dynamic_cast<const FixCost*>(&right);
			assert(l_ptr && r_ptr);
			return compare_fix(*l_ptr,*r_ptr);
		}

		virtual double get_fix_cost(const FixCost&) = 0;
		double get_cost(const Cost& ref) {
			const FixCost* ptr = dynamic_cast<const FixCost*>(&ref);
			assert(ptr);
			return get_fix_cost(*ptr);
		}
	};

	/** @brief Minimizes the TED node count adjusted to next candidate TED extraction*/
	class MinNodes : public TedOrderCost {
	public:
		//MinNodes(const CompareType t): TedOrderCost(t) {};
		MinNodes(void): TedOrderCost() {};
		virtual ~MinNodes(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.nodes; }
		virtual string name(void) { return string("ted_node::ted_edge"); }
	};

	/** @brief Minimizes the TED Bitwidth at the POs*/
	class MinBitwidth : public TedOrderCost {
	public:
		//MinBitwidth(const CompareType t): TedOrderCost(t) {};
		MinBitwidth(void): TedOrderCost() {};
		virtual ~MinBitwidth(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.normB; }
		virtual string name(void) { return string("ted_bitwith"); }
	};

	class MinGappaBound : public TedOrderCost {
	protected:
		double getValue(string filename);
	public:
		//MinGappaBound(const CompareType t): TedOrderCost(t) {};
		MinGappaBound(void): TedOrderCost() {};
		virtual ~MinGappaBound(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		FixCost* compute_fix_cost(DfgMan& manager);
		FixCost* compute_cost(DfgMan& manager) { return compute_fix_cost(manager); }
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.bitwidth; }
		virtual string name(void) { return string("ted_gappa_bound"); }
	};

	/** @brief Abstract Base Class for TED minimization taking into account DFG information*/
	class DfgCost : public TedOrderCost {
	protected:
		virtual void computeDfgStat(TedMan& manager, DfgStat& stat);
	public:
		//DfgCost(const CompareType t): TedOrderCost(t) {};
		DfgCost(void): TedOrderCost() {};
		virtual ~DfgCost(void) {};
	};

	/** @brief Minimizes the number of MPY in the DFG, adjusted to next candidate TED extraction*/
	class MinMpyCount : public DfgCost {
	public:
		//MinMpyCount(const CompareType t): DfgCost(t) {};
		MinMpyCount(void): DfgCost() {};
		virtual ~MinMpyCount(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.normB; }
		virtual string name(void) { return string("dfg_mpy"); }
	};

	/** @brief Minimizes the total operation count in the DFG, adjusted to next candidate TED extraction*/
	class MinOpCount : public DfgCost {
	public:
		//MinOpCount(const CompareType t): DfgCost(t) {};
		MinOpCount(void): DfgCost() {};
		virtual ~MinOpCount(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.normB; }
		virtual string name(void) { return string("dfg_mpy::{dfg_add+dfg_sub}"); }
	};

	/** @brief Abstract Base Class for TED minimization taking into account DFG information*/
	class ScheduleCost : public TedOrderCost {
	protected:
		void computeScheduleStat(TedMan& manager, DfgStat& stat);
	public:
		//ScheduleCost(const CompareType t): TedOrderCost(t) {};
		ScheduleCost(void): TedOrderCost() {};
		virtual ~ScheduleCost(void) {};
	};

	/** @brief Minimizes the TED node count, in equality uses as discriminant the number of MPY, ADD+SUB*/
	class MinOperations : public ScheduleCost {
	public:
		//MinOperations(const CompareType t): ScheduleCost(t) {};
		MinOperations(void): ScheduleCost() {};
		virtual ~MinOperations(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.stat.nMul; }
		virtual string name(void) { return string("dfg_rmpy::dfg_radd"); }
	};

	/** @brief Minimizes TED based on DFG latency.
	  @note accepts as a better TED, another one with the same latency.*/
	class MinLatency : public ScheduleCost {
	public:
		//MinLatency(const CompareType t): ScheduleCost(t) {};
		MinLatency(void): ScheduleCost() {};
		virtual ~MinLatency(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.stat.latency; }
		virtual string name(void) { return string("dfg_latency"); }
	};

	/** @brief Minimizes the TED based on its schedule after decomposition*/
	class TedScheduleCost : public TedOrderCost {
	protected:
		void scheduleDecomposed(TedMan& manager, DfgStat& stat);
	public:
		//TedScheduleCost(const CompareType t): TedOrderCost(t) {};
		TedScheduleCost(void): TedOrderCost() {};
		virtual ~TedScheduleCost(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
		virtual bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.stat.latency; }
		virtual string name(void) { return string("schedule_dfg_latency"); }
	};

	/** @brief Only for reporting purpouses, not intended for optimization*/
	class ReportCost : public ScheduleCost {
	public:
		//no CompareType as this class is for report only
		ReportCost(void): ScheduleCost() {}
		~ReportCost(void) {}
		FixCost* compute_fix_cost(TedMan& manager);
		bool compare_fix(const FixCost& left, const FixCost& right)const { return false; }
		virtual double get_fix_cost(const FixCost& cost) { return -1; }
		virtual string name(void) { return string("schedule_report"); }
	};

	/** @brief Abstract Base Class for TED minimization taking into account DFG information*/
	class NtlCost : public TedOrderCost {
	protected:
		virtual void computeNtlStat(TedMan& manager);
		virtual void writeCDFG(Netlist*);
		virtual void writeCDFG(DfgMan*);
		virtual void optimizeDFG(DfgMan*);

		void populateFilenames(const char* default_prefix);

		string prefix;
		string cdfg_file;
		string cost_file;
		string dump_file;
	public:
		//NtlCost(const CompareType t): TedOrderCost(t) { }
		NtlCost(void): TedOrderCost() { }
		virtual ~NtlCost(void) {};
		FixCost* compute_fix_cost(TedMan& manager);
	};

	class NtlMinMux : public NtlCost {
	public:
		//NtlMinMux(const CompareType t): NtlCost(t) {};
		NtlMinMux(void): NtlCost() {};
		~NtlMinMux(void) {}
		bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.muxes; }
		virtual string name(void) { return string("gaut_mux"); }
	};

	class NtlMinLatency : public NtlCost {
	protected:
		//void writeCDFG(Netlist*);
		void writeCDFG(DfgMan*);
		void optimizeDFG(DfgMan*);
	public:
		//NtlMinLatency(const CompareType t): NtlCost(t) {};
		NtlMinLatency(void): NtlCost() {};
		~NtlMinLatency(void) {}
		bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.latency; }
		virtual string name(void) { return string("gaut_latency"); }
	};

	class NtlMinArch : public NtlCost {
	public:
		//NtlMinArch(const CompareType t): NtlCost(t) {};
		NtlMinArch(void): NtlCost() {};
		~NtlMinArch(void) {}
		bool compare_fix(const FixCost& left, const FixCost& right)const;
		virtual double get_fix_cost(const FixCost& cost) { return cost.muxes; }
		virtual string name(void) { return string("gaut_registers"); }
	};


}
#endif
