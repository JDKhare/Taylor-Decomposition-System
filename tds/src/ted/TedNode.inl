/*
 * =====================================================================================
 *
 *       Filename:  TedNode.inl
 *    Description:
 *        Created:  11/8/2008 10:05:31 AM
 *         Author:  Daniel F Gomez-Prado
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-05-14 21:26:03 -0400(Fri, 14 May 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif


inline
TedNode* TedNode::getOne(void) {
	assert(_pOne);
	return const_cast<TedNode*>(_pOne);
}

/** @brief Get the variable*/
inline
const TedVar* TedNode::getVar(void)const {
	return _tVar;
}

/** @brief Get the kids*/
inline
TedKids* TedNode::getKids(void)const {
	return _tKids;
}

/** @brief is it a constant node*/
inline
bool TedNode::isConst(void)const {
	assert(_tVar);
	return _tVar->isConst();
}

/** @brief get the constant value*/
inline
wedge  TedNode::getConst(void)const {
	assert(_tVar);
	return _tVar->getValue();
}

/** @brief set the constant value*/
inline
void TedNode::setConst(wedge val) {
	assert(_tVar);
	_tVar->setConst(val);
}

/** @brief Set the variable*/
inline
void TedNode::setVar(const TedVar & v) {
	_tVar = TedVarMan::instance().createVar(v.getName());
	//_tVar = new TedVar(v);
}

/** @brief Add a kid*/
inline
void TedNode::addKid(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg) {
	if(isConst())return;
	getKids()->insert(index, pKid, weight, reg);
}

/** @brief Set a kid*/
inline
void TedNode::setKid(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg) {
	assert(pKid);
	if(isConst())return;
	getKids()->erase(index);
	getKids()->insert(index, pKid, weight, reg);
}

/** @brief replace a kid*/
inline
void TedNode::replaceKid(unsigned int index, TedNode* pKid, wedge weight, const TedRegister& reg) {
	assert(pKid);
	if(isConst())return;
	getKids()->replace(index, pKid, weight, reg);
}

/** @brief remove a kid*/
inline
void TedNode::removeKid(unsigned int index) {
	if(isConst())return;
	getKids()->erase(index);
}

/** @brief number of kids*/
inline
unsigned int TedNode::numKids(void)const {
	if(isConst())return 0;
	return getKids()->size();
}

/** @brief does it has kid at index*/
inline
bool TedNode::hasKid(unsigned int index)const {
	if(isConst())return false;
	return getKids()->has(index);
}

/** @brief collect child nodes in the order of DFS*/
inline
void TedNode::collectDFS(vector <TedNode*> & vNodes) {
	Mark<TedNode>::newMark();
	_collectDFS_rec(vNodes);
}

/** @brief Set the kids*/
inline
void  TedNode::setKids(TedKids & k) {
	//	assert(_tKids==NULL);
	_tKids = k.clone();
}

inline
void  TedNode::setKids(TedKids* k) {
	//	assert(_tKids==NULL);
	_tKids = k;
}

/** @brief Get the kid node*/
inline
TedNode* TedNode::getKidNode(unsigned int index)const {
	if(isConst())return NULL;
	return getKids()->getNode(index);
}

inline
TedRegister TedNode::getKidRegister(unsigned int index)const {
	if(isConst()) return 0;
	return getKids()->getRegister(index);
}

/** @brief Get the kid weight*/
inline
wedge TedNode::getKidWeight(unsigned int index)const {
	if(isConst())
		return wedge_max;
	return getKids()->getWeight(index);
}

/** @brief Set the kid weight*/
inline
void TedNode::setKidWeight(unsigned int index, wedge weight) {
	if(isConst()) {
		assert(0);
		return;
	}
	return getKids()->setWeight(index, weight);
}

/** @brief Is Pnode a simple variable or not*/
inline
bool TedNode::isBasic(void)const {
	if(numKids() == 1 && getKidNode(_tKids->highestOrder()) == _pOne) {
		return true;
	}
	return false;
}

/** @brief the name of the variable*/
inline
const string& TedNode::getName(void)const {
	return _tVar->getName();
}

inline 
const string& TedNode::getRootName(void) const {
	return _tVar->getRootName();
}

/** @brief construct an empty TedNode*/
inline
TedNode::TedNode(void): _register(0), _nRefs(0), _tVar(NULL), _tKids(NULL), _bitw(NULL), _backptr(NULL) {
#ifdef _DEBUG
	safe_guard = 0xfefe0000;
#endif
}

/** @brief Construct a constant Ted Node from value*/
inline
TedNode::TedNode(wedge constvalue): _register(0), _nRefs(0), _tVar(NULL), _tKids(NULL), _bitw(NULL), _backptr(NULL) {
	_tVar = TedVarMan::instance().createVar(constvalue);
#ifdef _DEBUG
	safe_guard = 0xeeeeeeee;
#endif
}

inline
TedNode::TedNode(const TedVar & t, const TedKids & k, const TedRegister & r): _register(r), _nRefs(0), _bitw(NULL), _backptr(NULL) {
	_tVar = TedVarMan::instance().createVar(t);
	_tKids = k.clone();
#ifdef _DEBUG
	safe_guard = 0xdddddddd;
#endif
}

/** @brief Construct a TedNode from variable and kids*/
inline
TedNode::TedNode(const TedVar & t, const TedKids & k): _register(0), _nRefs(0), _bitw(NULL), _backptr(NULL) {
	_tVar = TedVarMan::instance().createVar(t);
	_tKids = k.clone();
#ifdef _DEBUG
	safe_guard = 0xcfcfcfcf;
#endif
}

/** @brief Construct a TedNode from a variable, no kids*/
inline
TedNode::TedNode(const TedVar & t): _register(0), _nRefs(0), _tVar(NULL), _tKids(NULL), _bitw(NULL), _backptr(NULL) {
	_tVar = TedVarMan::instance().createVar(t);
#ifdef _DEBUG
	safe_guard = 0xbbbbbbbb;
#endif
}

inline
TedNode::TedNode(const TedNode& other): _register(other._register), _nRefs(other._nRefs), _tVar(NULL), _tKids(NULL), _bitw(NULL), _backptr(NULL) {
	_tVar = TedVarMan::instance().createVar(*(other._tVar));
	//_tKids = other._tKids;
#ifdef _DEBUG
	safe_guard = 0xaaaaaaaa;
#endif
}

inline
Bitwidth* TedNode::getBitwidth()const {
	return _bitw;
}

inline
void TedNode::setBitwidth(Bitwidth* ptr) {
	_bitw = ptr;
}

inline
void TedNode::setBackptr(TedNode* pNode) {
	_backptr = pNode;
}

inline
TedNode* TedNode::getBackptr(void)const {
	return _backptr;
}

