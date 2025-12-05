/*
 * =====================================================================================
 *
 *       Filename:  TedVarMan.inl
 *    Description:
 *        Created:  12/8/2008 7:44:20 PM
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

inline
void TedVarMan::onBirth(void) {
	static TedVarMan theInstance;
	_pInstance = &theInstance;
}

inline
TedVarMan& TedVarMan::instance(void) {
	if (!_pInstance){
		if(_destroyed) {
			onDeadReference();
		} else {
			onBirth();
		}
	}
	return (*_pInstance);
}

inline
size_t TedVarMan::isEmpty(void) {
	return _table.empty();
}

inline
void TedVarMan::clear(void) {
	for (map<string,TedVar*>::iterator it = _table.begin(); it != _table.end(); it++){
		delete it->second;
		it->second = NULL;
	}
	_table.clear();
}

inline
TedVar* TedVarMan::getIfExist(const string& name) const {
	map<string,TedVar*>::const_iterator it = _table.find(name);
	if ( it != _table.end() ) {
		return (*it).second;
	}
	return NULL;
}

inline
void TedVarMan::add(TedVar* pVar) {
	string name = pVar->getName();
	TedVar* old = getIfExist(name);
	if(!old)
		_table[name] = pVar;
}

inline
void TedVarMan::remove(const string& name) {
	map<string,TedVar*>::iterator it = _table.find(name);
	if ( it != _table.end() ) {
		delete it->second;
		it->second = NULL;
		_table.erase(it);
	}
}

inline
void TedVarMan::removeAll(void) {
	while(!_table.empty()) {
		delete _table.begin()->second;
		_table.erase(_table.begin());
	}
}

