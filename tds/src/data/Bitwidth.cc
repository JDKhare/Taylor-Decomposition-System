/*
 * =====================================================================================
 *
 *       Filename:  Bitwidth.cc
 *    Description:
 *        Created:  09/09/2009 08:32:43 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <cassert>

#include "Bitwidth.h"
namespace data {

#ifndef INLINE
#include "Bitwidth.inl"
#endif

	void Integer::lsh(unsigned int amount) {
		_integer += amount;
	}

	void Integer::rsh(unsigned int amount) {
		if (amount >= _integer) {
			//underflow
			_integer = 0;
		} else {
			_integer -= amount;
		}
	}

	void Integer::copy(const Bitwidth& _other) {
		const Integer *  other = dynamic_cast<const Integer *>(&_other);
		_integer = other->_integer;
	}

	void FixedPoint::add(const Bitwidth* _other) {
		const FixedPoint *  other = dynamic_cast<const FixedPoint *>(_other);
		bool atLeastOneZero = (0==_integer && 0==_fractional) || (0==other->_integer && 0==other->_fractional);
		_integer = (_integer >= other->_integer) ? _integer : other->_integer;
		if(!atLeastOneZero)
			++_integer;
		_fractional = (_fractional >= other->_fractional) ? _fractional : other->_fractional;
	}

	void FixedPoint::power(unsigned int value) {
		assert(value>0);
		_integer *= value;
		_fractional *= value;
	}

	void FixedPoint::mpy(const Bitwidth* _other) {
		const FixedPoint *  other = dynamic_cast<const FixedPoint *>(_other);
		_integer += other->_integer;
		_fractional += other->_fractional;
	}

	void FixedPoint::lsh(unsigned int amount) {
		if (amount >= _fractional) {
			_integer += amount;
			_fractional = 0;
		} else {
			_integer += amount;
			_fractional -= amount;
		}
	}

	void FixedPoint::rsh(unsigned int amount) {
		if (amount >= _integer) {
			_fractional += amount;
			_integer = 0;
		} else {
			_fractional += amount;
			_integer -= amount;
		}
	}

	bool FixedPoint::isLessThan(const Bitwidth& _other) const {
		const FixedPoint *  other = dynamic_cast<const FixedPoint *>(&_other);
		if (_integer < other->_integer) {
			return true;
		} else if (_integer > other->_integer) {
			return false;
		} else {
			if (_fractional < other->_fractional) {
				return true;
			} else if (_fractional > other->_fractional) {
				return false;
			} else {
				return false;
			}
		}
	}

	void FixedPoint::copy(const Bitwidth& _other) {
		const FixedPoint *  other = dynamic_cast<const FixedPoint *>(&_other);
		_integer = other->_integer;
		_fractional = other->_fractional;
		_error = other->_error;
	}

}
