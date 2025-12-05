/*
 * =====================================================================================
 *
 *       Filename:  csd.h
 *    Description:  Canonical Signed Digit
 *        Created:  07/05/2007 04:14:31 PM EDT
 *         Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-14 12:25:47 -0400 (Mon, 14 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __CSD_H__
#define __CSD_H__

#include <bitset>
using namespace std;

namespace util {

	/**
	 * @class Csd
	 * @brief Computes the Canonical Signed Digit
	 **/
	class Csd {
	private:
		static const size_t M = sizeof(unsigned long) * 8;
		bitset <M> _p; //positive bits;
		bitset <M> _n; //negative bits;
	public:
		Csd();
		template <typename T> Csd(T t);
		template <typename T> T readValue(void);
		template <typename T> void setValue(T t);

		int  at(size_t index);
		int  operator [] (size_t index);
		int  getLevel(void);
		void printBits(void);
		size_t size(void);
	};

	template <typename T> Csd::Csd(T t) {
		setValue(t);
	}

	template <typename T> T Csd::readValue(void) {
		T t;
		t = 0;
		for (size_t i = 0; i < M; i++) {
			if (_p.test(i)) {
				t = t + (0x01<<i);
			}
			if (_n.test(i)) {
				t = t - (0x01<<i);
			}
		}
		return t;
	}

#if defined(_MSC_VER)
#define BITSET_CONSTRUCTOR_TYPE int
#else
#define BITSET_CONSTRUCTOR_TYPE unsigned long
#endif

	template <typename T> void Csd::setValue(T t) {
		BITSET_CONSTRUCTOR_TYPE init = t;
		bitset <M+1> x(init);
		bitset <M+1> c;

		_p.reset();
		_n.reset();

		x[M] = x[M-1];
		c[0] = 0;

		for (int i = 0; i < M; i++) {
			c[i+1] = x[i]&x[i+1] | x[i]&c[i] | x[i+1]&c[i];
			switch ((int)x[i]+(int)c[i]-2*(int)c[i+1]) {
			case 1:
				_p.set(i, 1);
				_n.set(i, 0);
				break;
			case 0:
				_p.set(i, 0);
				_n.set(i, 0);
				break;
			case -1:
				_n.set(i, 1);
				_p.set(i, 0);
				break;
			default:
				break;
			}
		}
	}

}

#endif
