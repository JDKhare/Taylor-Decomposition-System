/*
 * =====================================================================================
 *
 *       Filename:  Gaut.h
 *    Description:
 *        Created:  04/06/2011 08:10:00 PM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __GAUT_H__
#define __GAUT_H__ 1

#include "TedOrderCost.h"

namespace network {
	using namespace ted;

	class Gaut {
	private:
		string _bin;
		string _builtins;
		string _lib;
		string _switch;
		string _cdfg_base_name;
		bool _executed;
	public:
		Gaut(string cdfg, int cadency,int mem, int clock,const char* library="notech_16b.lib");
		Gaut(string cdfg);
		~Gaut(void) {}

		bool run(void);
		FixCost* compute_cost(void);

	};

}


#endif


