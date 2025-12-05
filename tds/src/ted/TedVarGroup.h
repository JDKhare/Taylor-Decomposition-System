/*
 * =====================================================================================
 *
 *        Filename:  TedVarGroup.h
 *     Description:  TedVariables Grouped
 *         Created:  12/3/2008 12:46:35 PM EDT
 *          Author:  Daniel Gomez-Prado
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 207                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 22:55:53 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDVARGROUP_H__
#define __TEDVARGROUP_H__

#include <string>
#include <cassert>

#include "TedVar.h"
#include "Bitwidth.h"
using namespace data;

#include "util.h"
using util::Util;

using namespace std;

namespace ted {

	class TedVarGroupOwner;

	class TedVarGroupMember : public TedVar {
	private:
		TedVarGroupOwner* _owner;
		unsigned int _index;
	public:
		explicit TedVarGroupMember(const TedVar& other, TedVarGroupOwner* owner, unsigned int index) :
			TedVar(other), _owner(owner), _index(index) { assert(_index>0); assert(_owner); }
		virtual ~TedVarGroupMember(void) {};
		virtual bool isMember(void) const;
		virtual void mark(void);
		void markMember(void);
		const TedVarGroupOwner* getOwner(void) const;
		unsigned int getIndex(void) const;
		virtual void printInfo(void) const;

		virtual bool isRetimed(void) const;
		virtual const TedVar* getBase(void) const;
		virtual TedRegister getRegister(void)const;

		virtual const string& getRootName(void) const; /* defined in TedVar.cc */
	};

	/**
	 * @class TedVarGroupOwner
	 * @brief Hold the Ted variable name
	 **/
	class TedVarGroupOwner : public TedVar {
	private:
		vector<TedVarGroupMember*> _members;
		//used to iterate through all the members sequentially
		unsigned int _iterate;
		const TedVar* _old;
		bool _recovered;
	public:
		explicit TedVarGroupOwner(const TedVar& other) : TedVar(other), _iterate(0), _old(&other), _recovered(false) {
			//if (_old->isRetimed()) {
			//}
		};
		const TedVar* recover(void) { _recovered = true; return _old; }
		virtual ~TedVarGroupOwner(void) { if(_old && !_recovered) delete _old; };
		virtual bool isOwner(void) const;
		virtual void mark(void);
		virtual bool isMarked(void);
		const TedVarGroupOwner* getOwner(void) const;
		unsigned int getSize(void) const;
		void addMember(TedVarGroupMember* pVar);
		TedVarGroupMember* createMember(void);
		TedVarGroupMember* getMember(unsigned int index) const;
		TedVarGroupMember* getMember(void);
		virtual void printInfo(void) const;
		void setBitwidth(Bitwidth *);
		void setRange(const string&);

		virtual bool isRetimed(void) const;
		virtual const TedVar* getBase(void) const;
		virtual TedRegister getRegister(void)const;

		virtual const string& getRootName(void) const; /* defined in TedVar.cc */
	};

	// ALWAYS INLINE THE FOLLOWING FUNCTIONS
#include "TedVarGroup.inl"

}
#endif
