/*
 * =====================================================================================
 *
 *       Filename:  csd.cc
 *    Description:
 *        Created:  07/05/2007 04:11:45 PM EDT
 *         Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <bitset>
#include <cstdio>
#include <cassert>
using namespace std;

#include "csd.h"
namespace util {

	Csd::Csd() {
		_p.reset();
		_n.reset();
	}
	int Csd::at(size_t index) {
		if (_p.test(index)) {
			return 1;
		} else if (_n.test(index)) {
			return -1;
		} else {
			return 0;
		}
	}

	int Csd::getLevel(void) {
		int level = -1;
		for (size_t index = 0; index < M; index++) {
			if(_p.test(index)) {
				level++;
			}
			if(_n.test(index)) {
				level++;
			}
		}
		return level;
	}

	int Csd::operator[](size_t index) {
		return at(index);
	}

	void Csd::printBits(void) {
		for (size_t i = 0; i < M; i++) {
			printf("%3d", _p.test(i));
		}
		printf("\n");
		for (size_t i = 0; i < M; i++) {
			printf("%3d", _n.test(i));
		}
		printf("\n");
		for (size_t i = 0; i < M; i++) {
			printf("%3d", at(i));
		}
		printf("\n");
	}

	size_t Csd::size(void) {
		return M;
	}


	/*
	   int main() {
	   Csd x;

	   for (int i = -1000; i < 1000; i++) {
	   x.SetValue(i);
	   if ( i != x.ReadValue<int>()) {
	   printf("ERROR at %s\n", i);
	   exit(1);
	   }
	   printf("i=%d, read = %d\n", i, x.ReadValue<int>());
//	printf("readback = %d\n", x.ReadValue());
}

return 0;
}
*/

}
