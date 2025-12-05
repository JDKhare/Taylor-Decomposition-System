/*
 * =====================================================================================
 *
 *        Filename:  TedVarGroup.inl
 *     Description:  TedVariables Grouped
 *         Created:  05/07/2009 10:49:01 AM
 *          Author:  Daniel Gomez-Prado
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 207                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 22:55:53 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */

inline
bool TedVarGroupMember::isMember(void) const {
	return true;
}

inline
void TedVarGroupMember::mark(void) {
	visited.setMark();
	_owner->visited.setMark();
}

inline
void TedVarGroupMember::markMember(void) {
	visited.setMark();
}

inline
const TedVarGroupOwner* TedVarGroupMember::getOwner(void) const {
	assert(_owner);
	return _owner;
}

inline
unsigned int TedVarGroupMember::getIndex(void) const {
	assert(_index>0);
	return _index;
}

inline
bool TedVarGroupOwner::isOwner(void) const {
	return true;
}

inline
void TedVarGroupOwner::mark(void) {
	visited.setMark();
	for(unsigned int index = 0; index < _members.size(); index++) {
		_members[index]->visited.setMark();
	}
}

inline
bool TedVarGroupOwner::isMarked(void) {
	return visited.isMarked();
}

inline
const TedVarGroupOwner* TedVarGroupOwner::getOwner(void) const {
	return this;
}

inline
unsigned int TedVarGroupOwner::getSize(void) const {
	return _members.size();
}

inline
void TedVarGroupOwner::addMember(TedVarGroupMember* pVar) {
	_members.push_back(pVar);
}

inline
TedVarGroupMember* TedVarGroupOwner::createMember(void){
	int index = getSize()+1;
	string memberName(getName());
	memberName += "[";
	memberName += Util::itoa(index);
	memberName += "]";
#if 0
	TedVar newVar(memberName);
	TedVarGroupMember* newMember = new TedVarGroupMember(newVar, this, index);
#else
	TedVarGroupMember* newMember = new TedVarGroupMember(*this, this, index);
	newMember->setName(memberName);
#endif
	_members.push_back(newMember);
	assert(newMember == _members[--index]);
	return newMember;
}

inline
TedVarGroupMember* TedVarGroupOwner::getMember(unsigned int index) const {
	assert(0 < index && index <= getSize());
	return _members[--index];
}

inline
TedVarGroupMember* TedVarGroupOwner::getMember(void) {
	if (_iterate >= getSize()) {
		_iterate=0;
	}
	return _members[_iterate++];
}

inline
void TedVarGroupMember::printInfo(void) const {
	cout << _name << "==" << _owner->getName() << " ";
}

inline
void TedVarGroupOwner::printInfo(void) const {
	cout << _name << "<1..." << getSize() << "> ";
}

inline
void TedVarGroupOwner::setBitwidth(Bitwidth * bitw) {
	_bitw = bitw;
	for(unsigned int index = 0; index < _members.size(); index++) {
		_members[index]->setBitwidth(bitw->clone());
	}
}

inline
void TedVarGroupOwner::setRange(const string& range) {
	_range = range;
	for(unsigned int index = 0; index < _members.size(); index++) {
		_members[index]->setRange(range);
	}
}

inline
bool TedVarGroupMember::isRetimed(void) const { 
	assert(_owner);
	return _owner->isRetimed(); 
}

inline
bool TedVarGroupOwner::isRetimed(void) const { 
	assert(_old);
	return _old->isRetimed(); 
}

inline
const TedVar* TedVarGroupMember::getBase(void) const {
	assert(_owner);
	return _owner->getBase();
}

inline
const TedVar* TedVarGroupOwner::getBase(void) const {
#if 0
	assert(_old);
	return _old->getBase();
#else
	assert(_old && _old->getBase()==_base);
	return _base;
#endif
}

inline
TedRegister TedVarGroupMember::getRegister(void)const {
	assert(_owner);
	return _owner->getRegister();
}

inline 
TedRegister TedVarGroupOwner::getRegister(void)const {
#if 0
		assert(_old);
		return _old->getRegister();
#else
		return TedVar::getRegister();
#endif
}

