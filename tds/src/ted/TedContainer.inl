/*
 * =====================================================================================
 *
 *       Filename:  TedContainer.inl
 *    Description:
 *        Created:  12/8/2008 5:17:28 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 178                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400(Wed, 07 Oct 2009)$: Date of last commit
 * =====================================================================================
 */


#ifndef INLINE
#define inline
#endif

/** @brief Get the level of a variable*/
inline
size_t TedContainer::getLevel(const TedVar& var)const {
	const_iterator it = find(var);
	if(it == end()) {
		//variable is not in the container or does not exist
		return 0;
	} else {
		// top variable    = getMaxLevel()
		// bottom variable = 1
		// variable ONE    = 0
		return(size()- it->second.first + 1);
	}
}

inline
bool TedContainer::isAtTop(const TedVar& var) const {
	return (getMaxLevel() == getLevel(var));
}

inline
bool TedContainer::isAtBottom(const TedVar& var) const {
	return (getLevel(var) ==1);
}

/** @brief The number of nodes*/
inline
size_t TedContainer::nodeCount(void)const {
	size_t c = 0;
	for(const_iterator it = begin(); it != end(); it++) {
		c += it->second.second->size();
	}
	return c;
}

inline
bool TedContainer::isFirstLower(const TedNode* v1, const TedNode* v2)const {
	return isFirstLower(*(v1->getVar()),*(v2->getVar()));
}

inline
bool TedContainer::isFirstHigher(const TedNode* v1, const TedNode* v2)const {
	return isFirstHigher(*(v1->getVar()),*(v2->getVar()));
}

inline
bool TedContainer::isSameHight(const TedNode* v1, const TedNode* v2)const {
	return isSameHight(*(v1->getVar()),*(v2->getVar()));
}

inline
size_t TedContainer::getMaxLevel(void)const{
	return size();
}

inline
bool TedContainer::isEmpty(void)const {
	return empty();
}

#if 0
inline
TedNode* TedContainer::getNode(const TedNode* pnode) {
	return getNode(*pnode->getVar(),*pnode->getKids(),pnode->getRegister());
}
#endif

inline
void TedContainer::update_TMark(void) {
	update_mark(&Mark<TedNode>::setMark);
}

inline
void TedContainer::update_PMark(void) {
	update_mark(&Mark<TedNode>::setPMark);
}
