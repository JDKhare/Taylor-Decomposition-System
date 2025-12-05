/*
 * =====================================================================================
 *
 *        Filename:  TedVar.h
 *     Description:  TedVariables
 *         Created:  04/17/2007 12:46:35 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDVAR_H__
#define __TEDVAR_H__

#include <string>
#include <cassert>
#include <iostream>

#include "Mark.h"
using dtl::Mark;

#include "types.h"
#include "TedRegister.h"

//#include "MemoryTracerInterface.h"
namespace data {
	class Error;
	class Bitwidth;
}
using namespace data;

namespace ted {
	using namespace std;

	class TedVarGroupOwner;

	/**
	 * @class TedVar
	 * @brief Hold the Ted variable name
	 **/
	class TedVar {
	public:
		enum Type { CONST, VAR, VARCONST, VARBINARY, ONEHOT };
	protected:
		Type _type;
		wedge _value;
		string _name;
		Bitwidth* _bitw;
		Error* _error;
		string _range;
		mutable const TedVar* _base; //not NULL when isRetimed == true
		mutable TedRegister _register;
	public:
		Mark<TedVar> visited;

		TedVar(const string & str,bool binary=false);
		TedVar(wedge value);
		TedVar(const string & str, wedge value);
		TedVar(const string & str, const string & range);
		TedVar(const TedVar & other);
		virtual ~TedVar() {};

		void setBase(const TedVar* base)const { _base = base; }  //keep the constness promise to the compiler
		void setRegister(TedRegister& amount)const { _register = amount; if(amount) assert(_base); }  //keep the constness promise to the compiler
		

		Type getType(void)const;
		bool operator<(const TedVar &)const;
		bool equal(const TedVar &)const;
		const string& getName(void)const;
		virtual const string& getRootName(void) const;
		void setName(const string& name);
		void setValue(wedge);
		wedge getValue(void)const;
		bool isConst(void)const;
		bool isVar(void)const;
		bool isVarConst(void)const;
		bool isBinary(void)const;
		void setConst(wedge value);
		void setBinary(void);

		void defaultBitwidthForInteger(void);
		void defaultBitwidthForFixedpoint(void);
		virtual void setBitwidth(Bitwidth*);
		Bitwidth* getBitwidth(void)const;
		virtual void setError(Error*);
		Error* getError(void)const;
		bool hasRange(void)const;
		virtual void setRange(const string&);
		string getRange(void)const;

		virtual TedRegister getRegister(void)const { return _register; }
		virtual const TedVar* getBase(void) const { return _base; }
		virtual bool isRetimed(void) const { assert((_base) ?(_register != 0):(_register==0)); return(_base != NULL); }
		virtual bool isMember(void)const { return false; }
		virtual bool isOwner(void)const { return false; }
		virtual const TedVarGroupOwner* getOwner(void)const { return NULL; }
		virtual unsigned int getIndex(void)const { return 0; }
		virtual void mark(void) { visited.setMark(); }
		virtual bool isMarked(void) { return visited.isMarked(); }
		virtual void printInfo(void)const { cout << _name << " "; }
		void printInfoDetailed(void)const;

		bool operator== (const TedVar& other)const {
			return(_type == other._type && _value == other._value && _name == other._name);
		}
		bool operator!= (const TedVar& other)const {
			return !(*this==other);
		}
	};

#ifdef INLINE
#include "TedVar.inl"
#endif

}
#endif

