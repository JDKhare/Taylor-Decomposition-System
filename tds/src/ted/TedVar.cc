/*
 * =====================================================================================
 *
 *        Filename:  TedVar.cc
 *     Description:  TedVariables class
 *         Created:  04/17/2007 12:47:50 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 207                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include "TedVar.h"
#include "TedVarGroup.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <limits>

#include "Environment.h"
using tds::Environment;

#include "Bitwidth.h"
using data::Bitwidth;

namespace ted {

#ifndef INLINE
#include "TedVar.inl"
#endif

	namespace {
		string safe_itoa(wedge n) {
			static long long max = numeric_limits<long long>::max();
			static long long min = numeric_limits<long long>::min();
			unsigned int len;
			if(n == 0) {
				return string("0");
			}
			bool bNeg = false;
			if(n < 0) {
				n = -1*n;
				bNeg = true;
			}
			wedge m = n;
			len = 0;
			while(m > 0) {
				m /= 10;
				len++;
			}
			assert(len>0);
			//char* ptr = (char*)malloc(sizeof(char)*len+1);
			char* ptr = new char[len+1];
			for(unsigned int i = 0; i < len; i ++) {
				wedge d = n - n/10*10;
				n = n/10;
				ptr[len-1-i] = d + '0';
			}
			ptr[len] = 0;
			string ret(ptr);
			if(bNeg) {
				ret.insert(ret.begin(),'-');
			}
			delete[] ptr;
			return ret;
		}
	}

	TedVar::TedVar(wedge val): _value(val), _type(CONST), _name(), _bitw(NULL), _error(NULL), _base(NULL), _register(0) {
		_name = safe_itoa(val);
	}

	void TedVar::setConst(wedge val) {
		assert(_type==CONST || _type==VARCONST);
		_value = val;
		_name = safe_itoa(val);
	}

	void TedVar::defaultBitwidthForInteger(void) {
		unsigned int word = Environment::getInt("bitwidth_integer");
		if(_bitw)
			delete _bitw;
		_bitw = new Integer(word);
	}

	void TedVar::defaultBitwidthForFixedpoint(void) {
		pair<unsigned int,unsigned int> word = Environment::getFraction("bitwidth_fixedpoint");
		if(_bitw)
			delete _bitw;
		_bitw = new FixedPoint(word.first,word.second);
	}

	void TedVar::printInfoDetailed(void)const {
		printInfo();
		switch(_type) {
		case VAR:
			cout << "-> PI";
			break;
		case VARCONST:
			cout << "-> constant PI " << _value;
			break;
		case VARBINARY:
			cout << "-> Binary PI";
			break;
		case CONST:
			cout << "-> constant " << _value;
			break;
		}
		if (isRetimed()) {
			cout << ", register " << _register;
		}
		if (_base) {
			cout << ", base var ";
			_base->printInfo();
		}
		if(_bitw) {
			cout << ", bitwidth " << _bitw->at();
		}
		if(!_range.empty()) {
			cout << ", range " << _range;
		}
		cout << endl;
	}

	string TedVar::getRange(void)const {
		if(_type==VAR) {
			return _range;
		} else {
			string range = _range;
			size_t index = range.find_first_of('b');
			if(index!=string::npos) {
				range.replace(index,1,"b-");
			}
			return range;
		}
	}


	const string& TedVar::getRootName(void) const { 
		return isRetimed() ? _base->getName() : getName(); 
	}

	const string& TedVarGroupMember::getRootName(void) const {
		return getOwner()->getRootName();
	}

	const string& TedVarGroupOwner::getRootName(void) const {
			//return _old->getRootName();
		return TedVar::getRootName();
	}

}

