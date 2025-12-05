/*
 * =====================================================================================
 *
 *       Filename:  Bitwidth.inl
 *    Description:
 *        Created:  09/09/2009 10:12:54 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 121                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-08 09:28:25 -0500 (Fri, 08 Jan 2010)     $: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline string Integer::at(void) const { return Util::itoa(_integer); }
inline Integer* Integer::clone(void) const { return new Integer(*this); }
inline long Integer::norm(void) const { return _integer; }
inline unsigned int Integer::getSize(void) const { return _integer; }
//inline bool Integer::isInteger(void) const { return true; }
inline long double Integer::getMaxRange(void) const { return pow((double)2.0,(int)_integer)-1; }

inline string FixedPoint::at(void) const { return Util::itoa(_integer)+","+Util::itoa(_fractional); }
inline FixedPoint* FixedPoint::clone(void) const { return new FixedPoint(*this); }
inline long FixedPoint::norm(void) const { return _integer+_fractional; }
inline unsigned int FixedPoint::getSize(void) const { return _fractional; }
//inline bool FixedPoint::isFixedPoint(void) const { return true; }

inline
long double FixedPoint::getMaxRange(void) const {
	return (long double) pow((double)2.0,(int)_integer) - (long double) pow((double)2.0,(int)(-1*_fractional));
}

inline
void FixedPoint::set(unsigned int word, unsigned int fraction) {
	_integer = word;
	_fractional = fraction;
};
