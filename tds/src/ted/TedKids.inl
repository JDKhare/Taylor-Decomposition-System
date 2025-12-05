/*
 * =====================================================================================
 *
 *       Filename:  TedKids.inl
 *    Description:
 *        Created:  11/8/2008 8:04:53 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 149                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline
wedge TedKids::_gcd(wedge a, wedge b)const {
	if(b==0)return a;
	return(_gcd(b, a%b));
}


/** @brief Replace the kid with index, node, and weight*/
inline
void TedKids::replace(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg) {
	/*  make sure the index do exist*/
	assert(find(index)!= end());
	Kids* myKids = dynamic_cast<Kids*>(this);
	(*myKids)[index] = TedKid(pKid, weight, reg);
}

/** @brief Add a kid with index, node, and weight*/
inline
void TedKids::insert(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg) {
	assert(find(index) == end());
	Kids* myKids = dynamic_cast<Kids*>(this);
	myKids->insert(pair<unsigned int, TedKid>(index, TedKid(pKid, weight, reg)));;
}

/** @brief the highest order of kids*/
inline
unsigned int TedKids::highestOrder(void)const {
	return rbegin()->first;
}

/** @brief if a kid at index exist or not*/
inline
bool TedKids::has(unsigned index)const {
	return find(index)!= end();
}

/** @brief check if two TedKids are same or not*/
inline
bool TedKids::equal(const TedKids & other)const {
	const Kids* myKids = dynamic_cast<const Kids*>(this);
	const Kids* otherKids = dynamic_cast<const Kids*>(&other);
	return*myKids ==*otherKids;
}

