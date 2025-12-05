/*
 * =====================================================================================
 *
 *       Filename:  RNGenerator.h
 *    Description:
 *        Created:  03/17/2011 12:04:46 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __RNGenerator_h__
#define __RNGenerator_h__ 1

namespace dtl {

	class RNGenerator {
	public:
		RNGenerator(unsigned query=1);
		void initialize(unsigned start);
		unsigned long randomLong(void) {return next(); }
		unsigned long randomLong(unsigned long range) {return(next()% range); }
		unsigned randomInt(unsigned range) {return unsigned(next()% range); }
		double randomDouble(double range);
	private:
		//UINT_MAX is 4294967295
		static const unsigned int MAX_VAL = 4294967291u;
		unsigned long word1[44];
		unsigned long weyl;
		int i,j,carry;
		unsigned long next();
	};

	class RNG : public RNGenerator {
	public:
		double randomProb(void);
		double dblGammaGreaterThanOne(double dblAlpha);
		double dblGammaLessThanOne(double dblAlpha);
		double dblRanGamma(double dblAlpha);

		static double generate(void) { return _RNG.randomProb(); }
	private:
		static RNG _RNG;
	};

}

#endif


