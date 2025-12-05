/*
 * =====================================================================================
 *
 *       Filename:  TedOrderDecorator.h
 *    Description:
 *        Created:  07/11/2011 10:21:26 PM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __TEDORDERDECORATOR_H__
#define __TEDORDERDECORATOR_H__ 1

#include <sstream>

#include "DfgMan.h"
#include "TedMan.h"
#include "RNGenerator.h"
#include "TedOrderCost.h"

using dtl::RNG;

using dfg::DfgMan;
using dfg::DfgStat;

#include "Ntk.h"
using network::Netlist;

namespace ted {

	class TedCompareDecorator;

	/** @brief Object used for comparison*/
	class CustomCost : public Cost {
	protected:
		double _stop;
		double _value;
		CustomCost* _next;

	public:
		CustomCost(const CustomCost& other): Cost(), _value(0), _next(NULL) {
			_stop = other._stop;
			_value = other._value;
			if (other._next) {
				_next = new CustomCost(*other._next);
			}
		}
		CustomCost(double value,CustomCost* next = NULL): Cost(), _stop(0.0), _value(value), _next(next) {}
		CustomCost(double stop,double value,CustomCost* next = NULL): Cost(), _stop(stop), _value(value), _next(next) {}
		~CustomCost(void) { delete _next; }

		CustomCost* clone(void) {
			return new CustomCost(*this);
		}

		virtual Cost& operator= (const Cost& other) {
			if(this == &other) {
				return*this;
			}
			const CustomCost* ptr = dynamic_cast<const CustomCost*>(&other);
			if(ptr) {
				_stop = ptr->_stop;
				_value = ptr->_value;
				if (_next) {
					delete _next;
					_next = NULL;
				}
				_next = ptr->_next->clone();
			}
			return(*this);
		}

		CustomCost& operator+= (unsigned int val) {
			CustomCost* it = this;
			while(it) {
				it->_value += val;
				it = it->_next;
			}
			return*this;
		}
		CustomCost& operator*= (unsigned int val) {
			CustomCost* it = this;
			while(it) {
				it->_value*= val;
				it = it->_next;
			}
			return*this;
		}
		CustomCost& operator/= (unsigned int val) {
			CustomCost* it = this;
			while(it) {
				it->_value /= val;
				it = it->_next;
			}
			return*this;
		}

		bool operator<(const CustomCost& el2)const {
			const CustomCost* it = this;
			const CustomCost* jt = &el2;
			while(it && jt) {
				if(it->_value < jt->_value)return true;
				if(jt->_value < it->_value)return false;
				it = it->_next;
				jt = jt->_next;
			}
			//
			// check that the comparison have the same number of elements
			//
			assert(!it && !jt);
			return false;
		}

		bool operator<= (const CustomCost& el2)const {
			const CustomCost* it = this;
			const CustomCost* jt = &el2;
			while(it && jt) {
				if(it->_value < jt->_value)return true;
				if(jt->_value < it->_value)return false;
				it = it->_next;
				jt = jt->_next;
			}
			//
			// check that the comparison have the same number of elements
			//
			assert(!it && !jt);
			return true;
		}

		static bool compare(const CustomCost& t1, const CustomCost& t2) {
			return(_type==STRICTLESS) ?(t1<t2):(t1<=t2);
		}

		const CustomCost operator+(unsigned int val)const {
			CustomCost tmp(*this);
			tmp+= val;
			return tmp;
		}
		const CustomCost operator*(unsigned int val)const {
			CustomCost tmp(*this);
			tmp*= val;
			return tmp;
		}
		const CustomCost operator/(unsigned int val)const {
			CustomCost tmp(*this);
			tmp/= val;
			return tmp;
		}
		bool operator== (const Cost& other)const {
			const CustomCost* ptr = dynamic_cast<const CustomCost*>(&other);
			if(ptr) {
				return(*this==*ptr);
			}
			return false;
		}
		bool operator== (const CustomCost& other)const {
			const CustomCost* it = this;
			const CustomCost* jt = &other;
			while(it && jt) {
				if (it->_value!=jt->_value || it->_stop !=jt->_stop)
					return false;
				it = it->_next;
				jt = jt->_next;
			}
			//
			// check that the comparison have the same number of elements
			//
			assert(!it && !jt);
			return true;
		}

		void report(TedCompareCost* cmpFunc) const;

		void report(void)const {
			const CustomCost* it = this;
			int index = 0;
			ostream& out = _out ?*_out : std::cout;
			while(it) {
				out << index++ << ":" << it->_value << " ";
				it = it->_next;
			}
			//
			// check that the comparison have the same number of elements
			//
			cout << endl;
		}

		string record(void)const {
			const CustomCost* it = this;
			//int index = 0;
			string ret = "";
			while(it) {
				std::ostringstream os;
				ret += (os << it->_value) ? os.str() : "err";
				ret += ",";
				it = it->_next;
			}
			return ret;
		}

		double get_stop(void) const {
			return _stop;
		}

		double get_cost(void)const {
			return _value;
		}
	};


	class TedCompareBase : public TedCompareCost {
	public:
		friend class CustomCost;

		TedCompareBase(void): TedCompareCost() {}
		~TedCompareBase(void) {}

		virtual void recycle(void) {}

		virtual double get_cost(TedMan& manager) = 0;
		virtual string my_name(void) = 0;

		double get_cost(const Cost& ref) {
			const CustomCost* ptr = dynamic_cast<const CustomCost*>(&ref);
			if(ptr) {
				return ptr->get_cost();
			}
			return 0;
		}
		bool compare(const Cost& left, const Cost& right)const {
			const CustomCost* l_ptr = dynamic_cast<const CustomCost*>(&left);
			const CustomCost* r_ptr = dynamic_cast<const CustomCost*>(&right);
			assert(l_ptr && r_ptr);
			return CustomCost::compare(*l_ptr,*r_ptr);
		}

	};

	class TedCompareBaseStop : public TedCompareBase {
	public:
		TedCompareBaseStop(void) : TedCompareBase() {}
		~TedCompareBaseStop(void) {}

		void restart(void) {}
		double get_cost(TedMan& manager) { return 0; }
		CustomCost* compute_cost(TedMan& manager) {
			return new CustomCost(get_cost(manager));
		};
		string name(void) { return string("eof"); }
		string my_name(void) { return string("eof"); }
	};

	class TedCompareDecorator : public TedCompareBase {
	public:
		TedCompareBase* _inner;
	public:
		TedCompareDecorator(TedCompareCost* inner): _inner(NULL) {
			_inner = dynamic_cast<TedCompareBase*>(inner);
		}
		TedCompareDecorator(TedCompareBase* inner): TedCompareBase(), _inner(inner) {}
		~TedCompareDecorator(void) { delete _inner; }

		CustomCost* compute_cost(TedMan& manager) {
			return new CustomCost(get_stop(),get_cost(manager),dynamic_cast<CustomCost*>(_inner->compute_cost(manager)));
		};
		virtual double get_stop(void) const { return 0.0; }
		void restart(void) {
			_inner->restart();
			recycle();
		}
		string name(void) {
			string nm = my_name();
			nm += "::";
			nm += _inner->name();
			return nm;
		}
		TedCompareBase* next(void) { return _inner; }
	};

	/** @brief Minimizes the TED node count adjusted to next candidate TED extraction*/
	class TedNodes : public TedCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		TedNodes(TedCompareCost* inner): TedCompareDecorator(inner) {}
		TedNodes(TedCompareBase* inner): TedCompareDecorator(inner) {}
		~TedNodes(void) {}

		double get_cost(TedMan& manager) {
			return manager.getContainer().nodeCount();
		};
		string my_name(void) { return string("ted_nodes"); }
	};

	/** @brief Minimizes the TED node count adjusted to next candidate TED extraction*/
	class TedEdges : public TedCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		TedEdges(TedCompareCost* inner): TedCompareDecorator(inner) {}
		TedEdges(TedCompareBase* inner): TedCompareDecorator(inner) {}
		~TedEdges(void) {}

		double get_cost(TedMan& manager) {
			unsigned int edge0 = 0;
			unsigned int edgeN = 0;
			manager.costNaive(edge0,edgeN);
			return edge0+edgeN;
		};
		string my_name(void) { return string("ted_edges"); }
	};

	/** @brief Minimizes the TED node count adjusted to next candidate TED extraction*/
	class TedEdges0 : public TedCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		TedEdges0(TedCompareCost* inner): TedCompareDecorator(inner) {}
		TedEdges0(TedCompareBase* inner): TedCompareDecorator(inner) {}
		~TedEdges0(void) {}

		double get_cost(TedMan& manager) {
			unsigned int edge0 = 0;
			unsigned int edgeN = 0;
			manager.costNaive(edge0,edgeN);
			return edge0;
		};
		string my_name(void) { return string("ted_additive_edges"); }
	};

	/** @brief Minimizes the TED node count adjusted to next candidate TED extraction*/
	class TedEdgesN : public TedCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		TedEdgesN(TedCompareCost* inner): TedCompareDecorator(inner) {}
		TedEdgesN(TedCompareBase* inner): TedCompareDecorator(inner) {}
		~TedEdgesN(void) {}

		double get_cost(TedMan& manager) {
			unsigned int edge0 = 0;
			unsigned int edgeN = 0;
			manager.costNaive(edge0,edgeN);
			return edgeN;
		};
		string my_name(void) { return string("ted_multiplicative_edges"); }
	};

	/** @brief Minimizes the TED Bitwidth at the POs*/
	class TedWidth : public TedCompareDecorator {
	public:
		TedWidth(TedCompareCost* inner): TedCompareDecorator(inner) {}
		TedWidth(TedCompareBase* inner): TedCompareDecorator(inner) {}
		~TedWidth(void) {};

		double get_cost(TedMan& manager) {
			return manager.getMaxBitwidth()->norm();
		}
		string my_name(void) { return string("ted_bitwidth"); }
	};

	class DfgCompareDecorator : public TedCompareDecorator {
	protected:
		bool _dfg_owner;
		DfgMan* _anchor_dfg_man;
		DfgStat* _anchor_stat;
		DfgMan** _dfg_man;
		DfgStat** _stat;

		void compute_dfg(TedMan& manager);
	public:
		DfgCompareDecorator(TedCompareCost* inner): TedCompareDecorator(inner), _anchor_dfg_man(NULL), _anchor_stat(NULL), _dfg_man(NULL), _stat(NULL) {
			construct();
		}
		DfgCompareDecorator(TedCompareBase* inner): TedCompareDecorator(inner), _anchor_dfg_man(NULL), _anchor_stat(NULL), _dfg_man(NULL), _stat(NULL) {
			construct();
		}
		void construct(void) {
			TedCompareDecorator* it = dynamic_cast<TedCompareDecorator*>(_inner);
			_dfg_owner = true;
			while(it) {
				DfgCompareDecorator* test = dynamic_cast<DfgCompareDecorator*>(it);
				if(test) {
					it = NULL;
					_dfg_owner = false;
					_dfg_man = (test->_dfg_man);
					_stat = (test->_stat);
				} else {
					it = dynamic_cast<TedCompareDecorator*>(it->_inner);
				}
			}
			if(_dfg_owner) {
				_anchor_dfg_man = NULL;
				_anchor_stat = new DfgStat();
				_dfg_man = &_anchor_dfg_man;
				_stat = &_anchor_stat;
			}
		}
		~DfgCompareDecorator(void) {
			cleanup();
		}

		void recycle(void) {
			cleanup();
			if(_dfg_owner) {
				_anchor_stat = new DfgStat();
			}
		}

		void cleanup(void) {
			if(_dfg_owner) {
				delete _anchor_dfg_man;
				delete _anchor_stat;
			}
			_anchor_dfg_man = NULL;
			_anchor_stat = NULL;
		}
	};

	class DfgGappa : public DfgCompareDecorator {
	protected:
		double get_gappa_value(string filename);
	public:
		DfgGappa(TedCompareCost* inner): DfgCompareDecorator(inner) {};
		DfgGappa(TedCompareBase* inner): DfgCompareDecorator(inner) {};
		~DfgGappa(void) {};

		double get_cost(TedMan& ted_manager) {
			compute_dfg(ted_manager);
			(*_dfg_man)->computeBitwidth();
			(*_dfg_man)->writeGappa("ted.gappa");
			system("gappa < ted.gappa 2>ted.gappa.out");
			return get_gappa_value("ted.gappa.out");
		}
		string my_name(void) { return string("ted_gappa_bound"); }
	};


	/** @brief Minimizes the number of MPY in the DFG, adjusted to next candidate TED extraction*/
	class DfgOpMpy : public DfgCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		DfgOpMpy(TedCompareCost* inner): DfgCompareDecorator(inner) {};
		DfgOpMpy(TedCompareBase* inner): DfgCompareDecorator(inner) {};
		~DfgOpMpy(void) {};

		double get_cost(TedMan& ted_manager) {
			compute_dfg(ted_manager);
			return(*_stat)->nMul;
		}
		string my_name(void) { return string("dfg_mpy"); }
	};

	/** @brief Minimizes the total operation count in the DFG, adjusted to next candidate TED extraction*/
	class DfgOpAdd : public DfgCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		DfgOpAdd(TedCompareCost* inner): DfgCompareDecorator(inner) {};
		DfgOpAdd(TedCompareBase* inner): DfgCompareDecorator(inner) {};
		virtual ~DfgOpAdd(void) {};

		double get_cost(TedMan& ted_manager) {
			compute_dfg(ted_manager);
			return(*_stat)->nAdd+(*_stat)->nSub;
		}
		string my_name(void) { return string("{dfg_add+dfg_sub}"); }
	};

	/** @brief Abstract Base Class for TED minimization taking into account DFG information*/
	class DfgScheduleCompareDecorator : public DfgCompareDecorator {
	protected:
		void compute_dfg_arch(TedMan& ted_manager);
	public:
		DfgScheduleCompareDecorator(TedCompareCost* inner): DfgCompareDecorator(inner) {};
		DfgScheduleCompareDecorator(TedCompareBase* inner): DfgCompareDecorator(inner) {};
		~DfgScheduleCompareDecorator(void) {};
	};

	class DfgScheduleMpyRes : public DfgScheduleCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		DfgScheduleMpyRes(TedCompareCost* inner): DfgScheduleCompareDecorator(inner) {};
		DfgScheduleMpyRes(TedCompareBase* inner): DfgScheduleCompareDecorator(inner) {};
		~DfgScheduleMpyRes(void) {}

		double get_cost(TedMan& ted_manager) {
			compute_dfg_arch(ted_manager);
			return(*_stat)->rMPY;
		}
		string my_name(void) { return string("dfg_rmpy"); }
	};


	class DfgScheduleOpAddRes : public DfgScheduleCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		DfgScheduleOpAddRes(TedCompareCost* inner): DfgScheduleCompareDecorator(inner) {};
		DfgScheduleOpAddRes(TedCompareBase* inner): DfgScheduleCompareDecorator(inner) {};
		~DfgScheduleOpAddRes(void) {}

		double get_cost(TedMan& ted_manager) {
			compute_dfg_arch(ted_manager);
			return(*_stat)->rADD;
		}
		string my_name(void) { return string("dfg_radd"); }
	};

	class DfgScheduleOpLatency : public DfgScheduleCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		DfgScheduleOpLatency(TedCompareCost* inner): DfgScheduleCompareDecorator(inner) {};
		DfgScheduleOpLatency(TedCompareBase* inner): DfgScheduleCompareDecorator(inner) {};
		~DfgScheduleOpLatency(void) {}

		double get_cost(TedMan& ted_manager) {
			compute_dfg_arch(ted_manager);
			return(*_stat)->latency;
		}
		string my_name(void) { return string("dfg_latency"); }
	};


	/** @brief Abstract Base Class for TED minimization taking into account DFG information*/
	class GautCompareDecorator : public DfgCompareDecorator {
	protected:
		bool _ntl_owner;
		int* _anchor_mux;
		int* _anchor_latency;
		int* _anchor_registers;
		int* _anchor_area;
		int** _mux;
		int** _latency;
		int** _registers;
		int** _area;

		void compute_ntl_stat(TedMan& ted_manager);

		virtual void optimizeDFG(DfgMan* dfg) { dfg->strash(); }
		virtual void writeCDFG(Netlist* ntl) { ntl->writeCDFG(cdfg_file.c_str()); }
		virtual void writeCDFG(DfgMan* dfg) { dfg->writeCDFG(cdfg_file.c_str()); }

		void populateFilenames(const char* default_prefix);

		string prefix;
		string cdfg_file;
		string cost_file;
		string dump_file;
	public:
		GautCompareDecorator(TedCompareCost* inner): DfgCompareDecorator(inner) {
			construct();
		}
		GautCompareDecorator(TedCompareBase* inner): DfgCompareDecorator(inner) {
			construct();
		}
		void construct(void) {
			TedCompareDecorator* it = dynamic_cast<TedCompareDecorator*>(_inner);
			_ntl_owner = true;
			while(it) {
				GautCompareDecorator* test = dynamic_cast<GautCompareDecorator*>(it);
				if(test) {
					it = NULL;
					_ntl_owner = false;
					_mux = (test->_mux);
					_latency = (test->_latency);
					_registers = (test->_registers);
					_area = (test->_area);
				} else {
					it = dynamic_cast<TedCompareDecorator*>(it->_inner);
				}
			}
			if(_ntl_owner) {
				_anchor_mux = NULL;
				_anchor_latency = NULL;
				_anchor_registers = NULL;
				_anchor_area = NULL;
				_mux = &_anchor_mux;
				_latency = &_anchor_latency;
				_registers = &_anchor_registers;
				_area = &_anchor_area;
			}
		}
		~GautCompareDecorator(void) {
			recycle();
		}

		void recycle(void) {
			if(_ntl_owner) {
				delete _anchor_mux;
				delete _anchor_latency;
				delete _anchor_registers;
				delete _anchor_area;
				DfgCompareDecorator::recycle();
			}
			_anchor_mux = NULL;
			_anchor_latency = NULL;
			_anchor_registers = NULL;
			_anchor_area = NULL;
		}

	};

	class GautMux : public GautCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		GautMux(TedCompareCost* inner): GautCompareDecorator(inner) {};
		GautMux(TedCompareBase* inner): GautCompareDecorator(inner) {};
		~GautMux(void) {}

		double get_cost(TedMan& manager) {
			compute_ntl_stat(manager);
			return**_mux;
		}
		string my_name(void) { return string("gaut_mux"); }
	};

	class GautLatency : public GautCompareDecorator {
	protected:
		void writeCDFG(DfgMan* dfg) {
			dfg->balance();
			dfg->writeCDFG(cdfg_file.c_str());
		}
		void optimizeDFG(DfgMan* dfg) {
			dfg->balance();
			dfg->strash();
		}
	public:
		static double stop;
		double get_stop(void) { return stop; }
		GautLatency(TedCompareCost* inner): GautCompareDecorator(inner) {};
		GautLatency(TedCompareBase* inner): GautCompareDecorator(inner) {};
		~GautLatency(void) {}

		double get_cost(TedMan& manager) {
			compute_ntl_stat(manager);
			return**_latency;
		}
		string my_name(void) { return string("gaut_latency"); }
	};

	class GautReg : public GautCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		GautReg(TedCompareCost* inner): GautCompareDecorator(inner) {};
		GautReg(TedCompareBase* inner): GautCompareDecorator(inner) {};
		~GautReg(void) {}

		double get_cost(TedMan& manager) {
			compute_ntl_stat(manager);
			return**_registers;
		}
		string my_name(void) { return string("gaut_registers"); }
	};

	class GautArea : public GautCompareDecorator {
	public:
		static double stop;
		double get_stop(void) { return stop; }
		GautArea(TedCompareCost* inner): GautCompareDecorator(inner) {};
		GautArea(TedCompareBase* inner): GautCompareDecorator(inner) {};
		~GautArea(void) {}

		double get_cost(TedMan& manager) {
			compute_ntl_stat(manager);
			return**_area;
		}
		string my_name(void) { return string("gaut_area"); }
	};

}


#endif


