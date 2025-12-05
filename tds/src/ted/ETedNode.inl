/*
 * =====================================================================================
 *
 *       Filename:  ETedNode.inl
 *    Description:
 *        Created:  11/13/2008 8:10:32 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 207                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400(Wed, 07 Oct 2009)$: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline
ATedNode* ETedNode::clone(void) const {
	return new ETedNode(*this);
}

inline
ATedNode* ETedNode::BuildRecursive(const TedNode* t,TedRegister regVal) {
	ATedNode* ret = BuildRecursively<ETedNode>(t,regVal);
	return ret;
}

