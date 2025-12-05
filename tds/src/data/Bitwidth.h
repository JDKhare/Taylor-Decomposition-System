/*
 * =====================================================================================
 *
 *       Filename:  Bitwidth.h
 *    Description:
 *        Created:  06/02/2009 01:15:39 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __BITWIDTH_H__
#define __BITWIDTH_H__

#include <string>
#include <cmath>

#include "util.h"
using namespace util;

namespace data {

	using namespace std;

	class Bitwidth {
	private:
	public:
		Bitwidth(void) {};
		virtual ~Bitwidth(void) {};

		void sub(const Bitwidth*) { return; }

		virtual void add(const Bitwidth*) = 0;
		virtual void mpy(const Bitwidth*) = 0;
		virtual void power(unsigned int) = 0;
		virtual void lsh(unsigned int) = 0;
		virtual void rsh(unsigned int) = 0;
		virtual string at(void) const = 0;
		virtual Bitwidth * clone(void) const = 0;
		virtual void copy(const Bitwidth&) = 0;
		virtual bool isLessThan(const Bitwidth& other) const = 0;
		virtual void set(unsigned int, unsigned int opt = 0) = 0;
		virtual long norm(void) const = 0;
		virtual unsigned int getSize(void) const = 0;
		virtual long double getMaxRange(void)const  = 0;
		virtual bool isReducible(void) const = 0;
		virtual void reduce(void) = 0;
		virtual void reduce(unsigned int) = 0;
		virtual void increment(void) = 0;
		virtual void increment(unsigned int) = 0;
		virtual unsigned int getError(void) const = 0;
		//virtual bool isInteger(void) const { return false; }
		//virtual bool isFixedPoint(void) const { return false; }
	};

	class FixedPoint : public Bitwidth {
	protected:
		unsigned int _integer;
		unsigned int _fractional;
		unsigned int _error;
	public:
		explicit FixedPoint() : _integer(0), _fractional(0), _error(0) {};
		explicit FixedPoint(const FixedPoint& other) : Bitwidth(), _integer(other._integer), _fractional(other._fractional), _error(other._error) {};
		explicit FixedPoint(unsigned int length, unsigned int fraction) : Bitwidth(), _integer(length), _fractional(fraction), _error(0) {};
		~FixedPoint(void) {};

		void set(unsigned int word, unsigned int fraction = 0);
		void add(const Bitwidth* other);
		void mpy(const Bitwidth* other);
		void power(unsigned int value);
		bool isLessThan(const Bitwidth& other) const;
		bool isReducible(void) const { return (_fractional>0); }
		void reduce(void) { if(_fractional>0) {_fractional--; _error++; } }
		void reduce(unsigned int val) { if (_fractional<val) { _error+=_fractional;_fractional=0; } else {_fractional-=val; _error+=val; } }
		void increment(void) { _fractional++; _error--; }
		void increment(unsigned int val) { _fractional+=val; _error-=val; }
		unsigned int getError(void) const { return _error; }
		//bool isFixedPoint(void) const;

		virtual string at(void) const;
		virtual FixedPoint* clone(void) const;
		virtual void lsh(unsigned int);
		virtual void rsh(unsigned int);
		virtual void copy(const Bitwidth&);
		virtual long norm(void) const;
		virtual unsigned int getSize(void) const;
		virtual long double getMaxRange(void) const;
	};

	class Integer : public FixedPoint {
	public:
		explicit Integer(const Integer& other) : FixedPoint(other) {};
		explicit Integer(unsigned int length) : FixedPoint(length,0) {};
		~Integer(void) {};

		//bool isInteger(void) const;

		string at(void) const;
		Integer* clone(void) const;
		void lsh(unsigned int);
		void rsh(unsigned int);
		void copy(const Bitwidth&);
		long norm(void) const;
		unsigned int getSize(void) const;
		long double getMaxRange(void) const;
	};

#ifdef INLINE
#include "Bitwidth.inl"
#endif

}
#endif

