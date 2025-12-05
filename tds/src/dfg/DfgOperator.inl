/*
 * =====================================================================================
 *
 *       Filename:  DfgOperator.inl
 *    Description:
 *        Created:  01/11/2012 12:52:59 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */


#ifndef INLINE
#define inline
#endif

inline
Error* DfgOperator::getError(void)const { return _error; }
inline
Bitwidth* DfgOperator::getBitwidth(void)const { return _bitw; }
inline
void DfgOperator::set(Bitwidth* pre) { _bitw = pre; }
inline
void DfgOperator::set(Error* pre) { _error = pre; }
inline
void DfgOperator::clearBitwidth(void) { delete _bitw; _bitw=NULL; }
inline
void DfgOperator::clearError(void) { delete _error; _error=NULL; }
inline
unsigned int DfgOperator::getID(void)const { return _id; }
inline
DfgOperator::Type DfgOperator::getOp(void) { return _type; }
inline
void DfgOperator::setType(Type type) { _type=type; }


