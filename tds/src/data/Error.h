/*
 * =====================================================================================
 *
 *       Filename:  Error.h
 *    Description:  Metrics for the Deviation, mean, error.
 *        Created:  01/07/2010 12:17:39 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */
#ifndef __ERROR_H__
#define __ERROR_H__

#include <cmath>

namespace data {

	class Error {
	private:
		long double _o;
		long double _u;
		long double _e;
	public:
		Error(long double o, long double u, long double e) : _o(o), _u(u), _e(e) {};
		Error(Error* const other) : _o(other->_o), _u(other->_u), _e(other->_e) {};
		Error(void) : _o(0.0), _u(0.0), _e(0.0) {};
		~Error(void) {};

		Error* add(Error* const other) {
			Error* ret= new Error();
			ret->_o = _o+other->_o;
			ret->_u = _u+other->_u;
			ret->_e = _e+other->_e;
			return ret;
		}

		Error* mpy(Error* const other) {
			Error* ret= new Error();
			ret->_o = _o*other->_o;
			ret->_u = _u*other->_u;
			ret->_e = _o*other->_e + other->_o*_e;
			return ret;
		}

		void insert(unsigned int n, unsigned int k) {
			_e += pow(2.0,(int)(-1*n))*(1-pow(2.0,(int)(-1*k)))/12;
		}
		long double getE(void) { return _e; }
	};

}
#endif
