/*
 * =====================================================================================
 *
 *        Filename:  TedKids.cc
 *     Description:  TedKids class
 *         Created:  04/17/2007 01:08:05 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include "TedNode.h"
#include "TedKids.h"

#include <climits>

namespace ted {

#ifndef INLINE
#include "TedKids.inl"
#endif

	set<TedKids*> TedKids::_transitoryContainer;

	TedKids* TedKids::create_kids(void) {
		TedKids* pkid = new TedKids();
		//_transitoryContainer.insert(pkid);
		return pkid;
	}

	void TedKids::destroy_kids(TedKids* pkid) {
		delete pkid;
	}

	void TedKids::garbage_collect_unmarked(bool (Mark<TedKids>::*functor)(void) const) {
#if 0
		set<TedKids*> tcopy;
		tcopy = _transitoryContainer;
		for(set<TedKids*>::iterator it = tcopy.begin(); it != tcopy.end(); it++) {
			TedKids* pKid = (*it);
			assert(pKid);
			if (!(pKid->visited.*functor)()) {
				_transitoryContainer.erase(it);
				delete pKid;
			}
		}
#else
		set<TedKids*>::iterator it = _transitoryContainer.begin();
		while (it != _transitoryContainer.end()) {
			TedKids* pKid = (*it);
			assert(pKid);
			if (!(pKid->visited.*functor)()) {
				_transitoryContainer.erase(it++);
				delete pKid;
			} else {
				++it;
			}
		}
#endif
	}

	void TedKids::purge_transitory_container(void) {
		//ASSUMES desired edges have been marked
#if 0
		set<TedKids*> tcopy;
		tcopy = _transitoryContainer;
		for(set<TedKids*>::iterator it = tcopy.begin(); it != tcopy.end(); it++) {
			TedKids* pkid = (*it);
			assert(pkid);
			if (pkid->visited.isMarked()) {
				_transitoryContainer.erase(it);
			} else {
				delete pkid;
			}
		}
#else
		set<TedKids*>::iterator it = _transitoryContainer.begin();
		while (it != _transitoryContainer.end()) {
			TedKids* pKid = (*it);
			assert(pKid);
			if (pKid->visited.isMarked()) {
				_transitoryContainer.erase(it++);
			} else {
				++it;
				delete pKid;
			}
		}
#endif
		_transitoryContainer.clear();
	}

	void TedKids::print_transitory_container(void) {
		ofstream out("tc_tedkids.txt");
		out << " kid_pointer | mark " << endl;
		unsigned marked = 0;
		unsigned unmarked = 0;
		for(set<TedKids*>::const_iterator it = _transitoryContainer.begin(); it != _transitoryContainer.end(); it++) {
			TedKids* pkid = (*it);
			out << " ";
			out.width(11);
			out.fill(' ');
			out << pkid << " | ";
			if (pkid->visited.isMarked()) {
				marked++;
				out << " MM " << " | ";
			} else {
				unmarked++;
				out << " UU " << " | ";
			}
#ifdef _DEBUG
			out << hex << pkid->safe_guard;
			out << dec;
#endif
			out << endl;
		}
		out << "size=" << _transitoryContainer.size()<< "  marked=" << marked << "  unmarked=" << unmarked << endl;
		out.close();
		//	cout << "TedKids report in transitory container printed" << endl;
	}

	TedKids::TedKids(void) {
		_transitoryContainer.insert(this);
#ifdef _DEBUG
		safe_guard = 0xfebebefe;
#endif
	}

	TedKids::~TedKids(void) {
		set<TedKids*>::iterator it = _transitoryContainer.find(this);
		if (it!=_transitoryContainer.end()) {
			_transitoryContainer.erase(it);
		}
#ifdef _DEBUG
		safe_guard = 0xdeadbeaf;
#endif
	}

	/** @brief get a kid at index*/
	TedNode* TedKids::getNode(unsigned int index) {
		Kids::iterator p;
		if((p=find(index)) == end()) {
			return NULL;
		}
		return p->second.Node();
	}

	/** @brief get a kid weigh at index*/
	wedge TedKids::getWeight(unsigned int index)const {
		Kids::const_iterator p;
		if((p=find(index)) == end()) {
			throw(string("04010. There is no kid with the given index"));
		}
		return p->second.getWeight();
	}

	/** @brief get a kid weigh at index*/
	TedRegister TedKids::getRegister(unsigned int index)const {
		Kids::const_iterator p;
		if((p=find(index)) == end()) {
			throw(string("04011. There is no kid with the given index"));
		}
		return p->second.getRegister();
	}

	/** @brief set a kid weigh at index*/
	void TedKids::setWeight(unsigned int index, wedge weight) {
		Kids::iterator p;
		if((p=find(index)) == end()) {
			throw(string("04012. There is no kid with the given index"));
		}
		p->second.setWeight(weight);
	}

	/** @brief Extract Greatest common divisor of kis*/
	wedge TedKids::extractGCD(void) {
		unsigned int index = 0;
		TedKids::reverse_iterator _iterkids = rbegin();
		wedge gcd = _iterkids.getWeight();
		if (gcd == 1) {
			return 1;
		}
		wedge highterm = gcd;
		wedge weight = 0;
		TedKids::reverse_iterator _iterend = rend();
		_iterkids++;
		for(; _iterkids != _iterend; _iterkids++) {
			weight = _iterkids.getWeight();
			gcd = _gcd(gcd, weight);
		}
		if ( (gcd<0 && highterm>0) || (gcd>0 && highterm<0) ) {
			gcd *= -1;
		}
		if (gcd==1) {
			return 1;
		}
		FOREACH_KID_OF_KIDS(this) {
			index = _iterkids.getIndex();
			weight = _iterkids.getWeight();
			setWeight(index, weight/gcd);
		}
		return gcd;
	}

	void TedKids::pushGCD(wedge weight) {
		assert(weight!=0);
		if(weight==1)
			return;
		FOREACH_KID_OF_KIDS(this) {
			setWeight(_iterkids.getIndex(),_iterkids.getWeight()*weight);
		}
	}

	// return INT_MAX   if there is only one child and it connects to TedNode::getOne
	// return value     the minimum register count in any children(other than TedNode::getOne)
	// return 0         if there is any child with no register(other than TedNode::getOne)
	TedRegister TedKids::minRegister(void) {
		TedRegister minReg = INT_MAX, currentReg = 0;
		//TedNode* pKid = NULL;
		for(Kids::iterator _iterkids = begin(); _iterkids != end(); _iterkids++) {
			if(_iterkids->second.Node() == TedNode::getOne()) {
				continue;
			}
			currentReg = _iterkids->second.getRegister();
			if(currentReg == 0 &&
			   (_iterkids->second.Node()->getVar()->isVarConst() || \
				_iterkids->second.Node()->getVar()->isConst()) ) {
				currentReg = _iterkids->second.Node()->getKids()->minRegister();
			}
			if(currentReg == 0)
				return currentReg;
			if(currentReg < 0) {
				throw(string("04013. Future retiming not implemented yet"));
			}
			if(currentReg > 0 && currentReg < minReg) {
				minReg = currentReg;
			}
		}
		return minReg;
	}

	void TedKids::retimeUp(TedRegister& amount) {
		TedRegister currentReg;
		for(Kids::iterator _iterkids = begin(); _iterkids != end(); _iterkids++) {
			if(_iterkids->second.Node() == TedNode::getOne())
				continue;
			currentReg = _iterkids->second.getRegister();
			if(currentReg==0 && \
			   (_iterkids->second.Node()->getVar()->isVarConst() || \
				_iterkids->second.Node()->getVar()->isConst()) ) {
				_iterkids->second.Node()->getKids()->retimeUp(amount);
			} else {
				currentReg -= amount;
				assert(currentReg >= 0);
				_iterkids->second.setRegister(currentReg);
			}
		}
	}

	void TedKids::retimeDown(TedRegister& amount) {
		TedRegister currentReg;
		for(Kids::iterator _iterkids = begin(); _iterkids != end(); _iterkids++) {
			if(_iterkids->second.Node() == TedNode::getOne())
				continue;
			currentReg = _iterkids->second.getRegister();
			if(currentReg==0 && \
			   (_iterkids->second.Node()->getVar()->isVarConst() || \
				_iterkids->second.Node()->getVar()->isConst()) ) {
				_iterkids->second.Node()->getKids()->retimeDown(amount);
			} else {
				currentReg += amount;
				assert(currentReg > 0);
				_iterkids->second.setRegister(currentReg);
			}
		}
	}

	TedKids TedKids::duplicate(void) const {
		TedKids newKids;
		for(const_iterator _iterkids = begin(), _iterend = end(); _iterkids != _iterend; _iterkids++) {
			newKids.insert(_iterkids.getIndex(), _iterkids->second.Node(), _iterkids->second.getWeight(), _iterkids->second.getRegister());
		}
		return newKids;
	}

	TedKids* TedKids::clone(void) const {
		TedKids* newKids = create_kids();
		for(const_iterator _iterkids = begin(), _iterend = end(); _iterkids != _iterend; _iterkids++) {
			newKids->insert(_iterkids.getIndex(), _iterkids->second.Node(), _iterkids->second.getWeight(), _iterkids->second.getRegister());
		}
		return newKids;
	}

	void TedKids::setRegister(unsigned int index, TedRegister reg) {
		Kids::iterator p;
		if((p=find(index)) == end()) {
			throw(string("04041. There is no kid with the given index"));
		}
		p->second.setRegister(reg);
	}

}
