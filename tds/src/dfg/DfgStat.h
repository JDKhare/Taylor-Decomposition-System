/*
 * =====================================================================================
 *
 *       Filename:  DfgStat.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/14/2009 10:00:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 */
#ifndef __DFGSTAT_H__
#define __DFGSTAT_H__

namespace dfg {

	/**
	 * @class DfgStat
	 * @brief Computes DFG statistics
	 **/
	class DfgStat {
	public:
		unsigned int nMul;
		unsigned int nMulConst;
		unsigned int nAdd;
		unsigned int nAddConst;
		unsigned int nSub;
		unsigned int nSubConst;
		unsigned int latency;
		unsigned int rMPY;
		unsigned int rADD;
		DfgStat(void): nMul(0), nMulConst(0), nAdd(0), nAddConst(0), nSub(0), nSubConst(0), latency(0), rMPY(0), rADD(0) {};
		~DfgStat(void) {};

		DfgStat& operator+= (unsigned int val) {
			nMul += val;
			nMulConst += val;
			nAdd += val;
			nAddConst += val;
			nSub += val;
			nSubConst += val;
			latency += val;
			rMPY += val;
			rADD += val;
			return (*this);
		}
		DfgStat& operator*= (unsigned int val) {
			nMul*= val;
			nMulConst*= val;
			nAdd*= val;
			nAddConst*= val;
			nSub*= val;
			nSubConst*= val;
			latency*= val;
			rMPY*= val;
			rADD*= val;
			return (*this);
		}
		DfgStat& operator/= (unsigned int val) {
			nMul /= val;
			nMulConst /= val;
			nAdd /= val;
			nAddConst /= val;
			nSub /= val;
			nSubConst /= val;
			latency /= val;
			rMPY /= val;
			rADD /= val;
			return (*this);
		}

		const DfgStat operator+(unsigned int val)const {
			return DfgStat(*this)+= val;
		}
		const DfgStat operator*(unsigned int val)const {
			return DfgStat(*this)*= val;
		}
		const DfgStat operator/(unsigned int val)const {
			return DfgStat(*this)/= val;
		}

		bool operator== (const DfgStat& other) const {
			return (nMul==other.nMul) && (nMulConst==other.nMulConst) && \
						 (nAdd==other.nAdd) && (nAddConst==other.nAddConst) && \
						 (nSub==other.nSub) && (nSubConst==other.nSubConst) && \
						 (latency==other.latency) && (rMPY==other.rMPY) && (rADD==other.rADD);
		}
		bool operator!= (const DfgStat& other) const {
			return !(*this==other);
		}
	};

}
#endif

