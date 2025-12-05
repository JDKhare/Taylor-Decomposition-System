/*
 * =====================================================================================
 *
 *       Filename:  TedNodeRoot.inl
 *    Description:
 *        Created:  05/07/2009 12:11:50 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 153                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-11-23 04:40:19 -0500(Mon, 23 Nov 2009)$: Date of last commit
 * =====================================================================================
 */

inline
TedNode* TedNodeRoot::Node(void)const  { return _pNode; }

inline
void TedNodeRoot::Node(TedNode* node)  { _pNode = node; }

/** @brief Get the weight the pin connected to*/
inline
wedge TedNodeRoot::getWeight(void)const { return _weight; }

inline
TedNodeRoot::Type TedNodeRoot::getType(void)const {return _type; }

inline
TedRegister TedNodeRoot::getRegister(void)const { return _register; }

/** @brief Set the weight*/
inline
TedNode* TedNodeRoot::getLink(void) { return _link; }

/** @brief Set the weight*/
inline
void TedNodeRoot::setLink(TedNode* pnode) { _link = pnode; }

/** @brief Set the weight*/
inline
void TedNodeRoot::setWeight(wedge n) { _weight = n; }

inline
void TedNodeRoot::setType(Type type) { _type = type; }

inline
void TedNodeRoot::setRegister(const TedRegister& val) { _register = val; }

inline
Bitwidth* TedNodeRoot::getBitwidth()const { return _bitw; }

inline
void TedNodeRoot::setBitwidth(Bitwidth* ptr) { _bitw = ptr; }

inline
void TedNodeRoot::setError(double val) { _error = val; }

inline
double TedNodeRoot::getError(void)const { return _error; }

