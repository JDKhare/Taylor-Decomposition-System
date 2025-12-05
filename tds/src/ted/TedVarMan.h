/*
 * =====================================================================================
 *
 *       Filename:  TedVarMan.h
 *    Description:
 *        Created:  12/4/2008 10:50:27 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDVARMAN_H__
#define __TEDVARMAN_H__

#include <string>
#include <map>
#include <cassert>

#include "TedVar.h"
#include "TedVarGroup.h"

namespace ted {
	using namespace std;

	class TedVarMan {
	private:
		map<string,TedVar*> _table;
		static TedVarMan* _pInstance;
		static bool _destroyed;

		TedVarMan(void) {};
		TedVarMan(const TedVarMan& other) {
			throw string("04001. Copy constructor disabled");
		};
		TedVarMan operator= (const TedVarMan& other) {
			throw string("04002. Assignment operator disabled");
		};
		virtual ~TedVarMan(void) {
			_pInstance = NULL;
			_destroyed = true;
		}
		static void onDeadReference(void) {
			throw string("04003. Dead Reference Detected");
		}
		static void onBirth(void);
	public:
		static TedVarMan& instance(void);
		static const char TOKEN_REG = '@';

		size_t isEmpty(void);
		void clear(void);
		TedVar* getIfExist(const string& name)const;
		void add(TedVar* pVar);
		TedVar* createVar(const string& name, bool binary=false);  //normal variable
		TedVar* createVar(wedge val);             //normal constant
		TedVar* createVar(const string& var, const string& range); //variable with range
		TedVar* createVar(const string& var, wedge value);  //constant variable
		TedVar* createVar(const string& var, int value);  //constant variable
		TedVar* createVar(const TedVar& var);
		TedVar* createRetimedVar(const TedVar& base, TedRegister amount); //variable with register
		TedVarGroupOwner* createOwner(TedVar& var);
		TedVarGroupMember* createMember(TedVarGroupOwner& owner);
		void remove(const string& name);
		void removeAll(void);
		void foreach(void(TedVar::*functor)(void));
	};

#ifdef INLINE
#include "TedVarMan.inl"
#endif


}
#endif


