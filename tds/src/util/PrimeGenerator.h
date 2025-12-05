/*
 * =====================================================================================
 *
 *       Filename:  PrimeGenerator.h
 *    Description:  GeneratePrimes
 *        Created:  04/24/2007 11:03:22 AM EDT
 *         Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __PRIMEGENERATOR_H__
#define __PRIMEGENERATOR_H__

namespace util {

	/**
	 * @class PrimeGenerator
	 * @brief A prime table with more than 10k prime numbers
	 **/
	class PrimeGenerator {
	private:
		int _index;
		static unsigned int _primetable[10000];
	public:
		PrimeGenerator(void);
		unsigned int next(void);
		void         reset(void);
	};

}

#endif
