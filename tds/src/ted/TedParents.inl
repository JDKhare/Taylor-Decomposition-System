/*
 * =====================================================================================
 *
 *       Filename:  TedParents.inl
 *    Description:
 *        Created:  05/07/2009 10:38:00 AM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

inline
MyParent::~MyParent(void) {
	for (MyParent::iterator it = this->begin(), end=this->end(); it != end; it++) {
		delete (it->second);
	}
}

inline
void MyParent::registerKey(TedNode* pNode) {
	if(find(pNode) ==end()) {
		this->insert(std::pair<TedNode*,MyIndex*>(pNode,new MyIndex));
	}
}

inline
void MyParent::unregisterKey(TedNode* pNode) {
	if(find(pNode) != end()) {
		MyIndex* index = (*this)[pNode];
		delete index;
		this->erase(pNode);
	}
}

inline
void MyParent::registerParent(TedNode* pNode, unsigned int edge) {
	assert(find(pNode)!=end());
	(*this)[pNode]->push_back(edge);
}


inline
void MyParent::unregisterParent(TedNode* pNode, unsigned int edge) {
	assert(find(pNode)!=end());
	MyIndex* currentIndex = (*this)[pNode];
	MyIndex::iterator it = std::find(currentIndex->begin(),currentIndex->end(),edge);
	if (it!=currentIndex->end()) {
		currentIndex->erase(it);
	}
	if (currentIndex->empty()) {
		unregisterKey(pNode);
	}
}

inline
size_t MyParent::numParents(void) const {
	unsigned int count = 0;
	for (MyParent::const_iterator it = this->begin(), end=this->end(); it != end; it++) {
		count += it->second->size();
	}
	return count;
}

inline
TedParents::~TedParents(void) {
	for(TedParents::iterator it = this->begin(), end = this->end(); it!= end; it++) {
		delete (it->second);
	}
}

inline
void TedParents::collect(TedNode* pNode, TedNode* parent, unsigned int edge) {
	registerKey(pNode);
	registerParent(pNode,parent,edge);
	if(edge>1) {
		_type = NONLINEAR;
	}
}

inline
bool TedParents::isLinear(void) const {
	return (_type==LINEAR);
}

inline
void TedParents::registerKey(TedNode* pNode) {
	if(find(pNode) ==end()) {
		insert(std::pair<TedNode*,MyParent*>(pNode,new MyParent()));
	}
}

inline
void TedParents::unregisterKey(TedNode* pNode) {
	if(find(pNode)!=end()) {
		MyParent* myparent = (*this)[pNode];
		delete myparent;
		erase(pNode);
	}
}

inline
void TedParents::registerParent(TedNode* pNode, TedNode* parent, unsigned int edge) {
	assert(find(pNode)!=end());
	MyParent* myparent = (*this)[pNode];
	myparent->registerKey(parent);
	myparent->registerParent(parent,edge);
}

inline
void TedParents::unregisterParent(TedNode* pNode, TedNode* parent, unsigned int edge) {
	assert(find(pNode)!=end());
	MyParent* myparent = (*this)[pNode];
	myparent->unregisterParent(parent,edge);
	if (myparent->empty()) {
		unregisterKey(pNode);
	}
}

inline
bool TedParents::hasParents(TedNode* pNode) const {
	return (find(pNode)!=end());
}

inline
size_t TedParents::numParents(TedNode* pNode) const {
	const_iterator it = find(pNode);
	unsigned int count = 0;
	if (it!=end()) {
		count = it->second->numParents();
	}
	return count;
}

inline
bool TedParents::operator!= (const TedParents& other) const {
	return !(*this==other);
}

inline
bool TedParents::operator== (const TedParents& other) const {
	if (this->size() != other.size()) {
		return false;
	}
	for(TedParents::const_iterator it = this->begin(), jt = other.begin(), it_end = this->end(); it != it_end; it++, jt++) {
		TedNode* nthis = it->first;
		TedNode* nthat = jt->first;
		if (!nthis || !nthat || nthis!=nthat) {
			return false;
		}
		MyParent* pthis = it->second;
		MyParent* pthat = jt->second;
		if (!pthis || !pthat || pthis->size() != pthat->size()) {
			return false;
		}
		for(MyParent::const_iterator pit = pthis->begin(), pjt = pthat->begin(), pit_end = pthis->end(); pit != pit_end; pit++, pjt++) {
			TedNode* nnthis = pit->first;
			TedNode* nnthat = pjt->first;
			if (!nnthis || !nnthat || nnthis!=nnthat) {
				return false;
			}
			MyIndex* ithis = pit->second;
			MyIndex* ithat = pjt->second;
			if (!ithis || !ithat || ithis->size() != ithat->size() ) {
				return false;
			}
			for(MyIndex::const_iterator ipit = ithis->begin(), ipit_end=ithis->end(); ipit!=ipit_end;ipit++) {
				int ival = (*ipit);
				if (std::find(ithat->begin(),ithat->end(),ival) ==ithat->end()) {
					return false;
				}
			}

		}
	}
}

