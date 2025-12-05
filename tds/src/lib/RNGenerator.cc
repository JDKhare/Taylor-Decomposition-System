/*
 * =====================================================================================
 *
 *       Filename:  RNGenerator.cc
 *    Description:
 *        Created:  03/17/2011 12:07:33 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#include "RNGenerator.h"
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>

namespace dtl {
	using namespace std;

	RNG RNG::_RNG;

	unsigned long RNGenerator::next() {
		register unsigned long hold;
		if(--i==0)i=43;
		if(--j==0)j=43;
		hold = word1[i]+carry;
		if(word1[j] < hold) {
			word1[i] = MAX_VAL -(hold-word1[j]);
			carry = 1;
		} else {
			word1[i] = word1[j]-hold;
			carry=0;
		}
		weyl-=362436069;
		return word1[i] - weyl;
	}

	RNGenerator::RNGenerator(unsigned query) {
		unsigned start=query;
		srand(start);

		// make initial numbers.
		for(j=1;j<=43;j++) {
			word1[j]=rand();
			word1[j]= ((word1[j] << 15)+ rand());
			word1[j]= ((word1[j] << 2)+ rand()%4);
			if(word1[j]>MAX_VAL)word1[j] = MAX_VAL;
		}

		// initialize markers
		i=44;  j=23;  carry=1; weyl=rand();

		for(unsigned long a=1,garbage; a<100000; a++)// Warm-up
			garbage = next();
	}

	void RNGenerator::initialize(unsigned start) {
		srand(start);
		// make initial numbers.  for(j=1;j<=43;j++)
		{
			word1[j]=rand();
			word1[j]= ((word1[j] << 15)+ rand());
			word1[j]= ((word1[j] << 2)+ rand()%4);
			if(word1[j]>MAX_VAL)word1[j] = MAX_VAL;
		}

		i=44;  j=23;  carry=1; weyl=rand();				// initialize markers

		for(unsigned long a=1,garbage; a<100000; a++)// Warm-up
			garbage = next();
	}

	double RNGenerator::randomDouble(double range) {
		return((((double)next())/MAX_VAL)*range);
	}


	double RNG::randomProb(void) {
		return randomDouble(1.0);
	}

	// Code adopted from David Heckerman
	//-----------------------------------------------------------
	//	DblGammaGreaterThanOne(dblAlpha)
	//
	//	routine to generate a gamma random variable with unit scale and
	//      alpha > 1
	//	reference: Ripley, Stochastic Simulation, p.90
	//	Chang and Feast, Appl.Stat.(28)p.290
	//-----------------------------------------------------------
	double RNG::dblGammaGreaterThanOne(double dblAlpha) {
		double rgdbl[6];

		rgdbl[1] = dblAlpha - 1.0;
		rgdbl[2] = (dblAlpha -(1.0 /(6.0* dblAlpha)))/ rgdbl[1];
		rgdbl[3] = 2.0 / rgdbl[1];
		rgdbl[4] = rgdbl[3] + 2.0;
		rgdbl[5] = 1.0 / sqrt(dblAlpha);

		for(;;) {
			double  dblRand1;
			double  dblRand2;
			do {
				dblRand1 = randomProb();
				dblRand2 = randomProb();

				if(dblAlpha > 2.5)
					dblRand1 = dblRand2 + rgdbl[5]*(1.0 - 1.86* dblRand1);

			} while(!(0.0 < dblRand1 && dblRand1 < 1.0));

			double dblTemp = rgdbl[2]* dblRand2 / dblRand1;

			if(rgdbl[3]* dblRand1 + dblTemp + 1.0 / dblTemp <= rgdbl[4] || rgdbl[3]* log(dblRand1)+ dblTemp - log(dblTemp)< 1.0) {
				return dblTemp* rgdbl[1];
			}
		}
		assert(false);
		return 0.0;
	}

	/* routine to generate a gamma random variable with unit scale and alpha
	   < 1
reference: Ripley, Stochastic Simulation, p.88*/

	double RNG::dblGammaLessThanOne(double dblAlpha) {
		double dblTemp;

		const double	dblexp = exp(1.0);

		for(;;) {
			double dblRand0 = randomProb();
			double dblRand1 = randomProb();
			if(dblRand0 <= (dblexp /(dblAlpha + dblexp))) {
				dblTemp = pow(((dblAlpha + dblexp)* dblRand0)/dblexp, 1.0/dblAlpha);
				if(dblRand1 <= exp(-1.0* dblTemp))
					return dblTemp;
			}
			else
			{
				dblTemp = -1.0* log((dblAlpha + dblexp)*(1.0 - dblRand0)/(dblAlpha* dblexp));
				if(dblRand1 <= pow(dblTemp,dblAlpha - 1.0))
					return dblTemp;
			}
		}
		assert(false);
		return 0.0;
	}  /* DblGammaLessThanOne*/

	// Routine to generate a gamma random variable with unit scale(beta = 1)
	double RNG::dblRanGamma(double dblAlpha) {
		assert(dblAlpha > 0.0);
		if(dblAlpha < 1.0)
			return dblGammaLessThanOne(dblAlpha);
		else
			if(dblAlpha > 1.0)
				return dblGammaGreaterThanOne(dblAlpha);

		return -log(randomProb());
	}  /* gamma*/

}











