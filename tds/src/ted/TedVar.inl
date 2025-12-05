/*
 * =====================================================================================
 *
 *       Filename:  TedVar.inl
 *    Description:
 *        Created:  11/8/2008 3:47:34 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 207                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif


/**
 * @brief The default ordering is determined by alphabetic order, but
 *        its order can be changed by registering its variables at TedMan.
 **/
inline bool TedVar::operator <(const TedVar & op2)const { return _name < op2._name; }

/** @brief   Resolve if two variables are equal**/
inline bool TedVar::equal(const TedVar& other)const { return _name == other._name; }

/** @brief   Provides a string representation of the variable**/
inline const string& TedVar::getName(void)const { return  _name; }

inline
wedge TedVar::getValue(void)const {
	assert(_type==CONST || _type==VARCONST);
	return _value;
}

inline
void TedVar::setValue(wedge val) {
	_type = VARCONST;
	_value = val;
}

inline TedVar::Type TedVar::getType(void)const { return _type; }

inline bool TedVar::isConst(void)const { return _type==CONST; }

inline bool TedVar::isVar(void)const { return _type==VAR || _type==VARBINARY; }

inline bool TedVar::isBinary(void)const { return _type==VARBINARY; }

inline void TedVar::setBinary(void) { _type=VARBINARY; }

inline bool TedVar::isVarConst(void)const { return _type==VARCONST; }

inline TedVar::TedVar(const TedVar & other):  _type(other._type), _value(other._value), _name(other._name), _bitw(other._bitw), _error(other._error), _range(other._range), _base(other._base), _register(other._register) {}

inline TedVar::TedVar(const string & str,bool binary): _type(binary?VARBINARY:VAR), _value(0), _name(str), _bitw(NULL), _error(NULL), _base(NULL), _register(0) {}

inline TedVar::TedVar(const string & str, wedge val): _type(VARCONST), _value(val), _name(str), _bitw(NULL), _error(NULL), _base(NULL), _register(0) {}

inline TedVar::TedVar(const string & str, const string & range): _type(VARCONST), _value(0), _name(str), _bitw(NULL), _error(NULL), _range(range), _base(NULL), _register(0) {}

inline void TedVar::setBitwidth(Bitwidth* prec) { _bitw = prec; }

inline Bitwidth* TedVar::getBitwidth(void)const { return _bitw; }

inline void TedVar::setError(Error* err) { _error = err; }

inline Error* TedVar::getError(void)const { return _error; }

inline void TedVar::setName(const string& name) { _name = name; }

inline void TedVar::setRange(const string& range) { _range = range; }

inline bool TedVar::hasRange(void)const { return !_range.empty(); }

