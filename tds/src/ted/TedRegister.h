/*
 * =====================================================================================
 *
 *        Filename:  TedRegister.h
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDREGISTER_H
#define __TEDREGISTER_H

namespace ted {

#define Testing_REG 0
#ifdef Testing_REG
	typedef int TedRegister;
#else
	class TedRegister {
	private:
		int times;
	public:
		void TedRegister(void) : times(0) {};
		void TedRegister(int v) : times(v) {};
		~TedRegister(void) { times = 0; }
		int get(void) const { return times; }
		void set(const int val) { times = val; }
		void inc(void) { times++; }
		void dec(void) { times--; }
	};
#endif

}

#endif
