/*
 * =====================================================================================
 *
 *       Filename:  DfgBitReduction.h
 *    Description:
 *        Created:  11/04/2010 12:59:09 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __DFGBITREDUCTION_H__
#define __DFGBITREDUCTION_H__ 1

#include <map>

/**
 * @namespace dfg
 * @brief The Data Flow Graph data structure representation of the network or ted.
 **/
namespace dfg {

	using namespace std;

	class DfgMan;

	class DfgBitReduction {
	private:
		DfgMan* _currentManager;
		double _maxError;
		bool _report;

		class Cost {
		public:
			unsigned int bitw;
			double slope;
			double err;
		};
		struct CostOrder {
			bool operator() (Cost n1, Cost n2) const {
				bool ret = (n1.slope < n2.slope);
				if (n1.slope==n2.slope) {
					ret = n2.bitw < n1.bitw;
				}
				return ret;
			}
		};
		typedef multimap<Cost,DfgNode*,CostOrder> Sensitivity;

		Sensitivity _mpySense;
		Sensitivity _addSense;

		void updateBitwidthTowardPI(DfgNode*,unsigned int);
	public:
		explicit DfgBitReduction(DfgMan* manager,double maxError,bool report=true);
		~DfgBitReduction(void) {}

		void opTopological(void);
		void opSensitivity(void);
		void opPriorityCone(void);
	};


#ifdef INLINE
#include "DfgShifter.inl"
#endif

}
#endif


