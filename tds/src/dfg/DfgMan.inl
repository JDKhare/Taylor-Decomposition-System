/*
 * =====================================================================================
 *
 *       Filename:  DfgMan.inl
 *    Description:
 *        Created:  11/8/2008 8:52:31 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 100                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400 (Wed, 07 Oct 2009)     $: Date of last commit
 * =====================================================================================
*/

#ifndef INLINE
#define inline
#endif

/**
 * @brief DfgMan::RegNode register a DFG node into the set
 *
 * @param pNode pointer to insert in the set
 **/
inline
void DfgMan::registerNode(DfgNode * pNode) {
	_sNodes.insert(pNode);
}

/**
 * @brief DfgMan::unregisterNode removes a DFG node from the set
 *
 * @param pNode pointer to remove from the set
 * @todo  it does not release memory, so check for leakage
 **/
inline
void DfgMan::unregisterNode(DfgNode * pNode) {
	_sNodes.erase(pNode);
}

inline
map <string, DfgNode *> * DfgMan::getPos(void) {
	return &_mPos;
}

inline
void DfgMan::linkDFGToPO(DfgNode * pNode, string str) {
	_mPos[str] = pNode;
}

