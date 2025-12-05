/*
 * =====================================================================================
 *
 *       Filename:  TedKid.h
 *    Description:
 *        Created:  11/10/2008 8:53:07 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-01-08 15:28:25 +0100(Fri, 08 Jan 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDKID_H__
#define __TEDKID_H__

#include "TedRegister.h"
#include "Bitwidth.h"
using data::Bitwidth;

#include "types.h"

namespace ted {

	class TedNode;

	/**
	 * @class TedKid
	 * @brief Implements a TED node child(kid)
	 **/
	class TedKid {
	private:
		TedNode* _pNode;
		wedge       _weight;
		TedRegister _register;
		Bitwidth* _bitw;
	public:
		TedKid(void): _pNode(NULL), _weight(0), _register(0), _bitw(NULL) {};
		TedKid(TedNode* n, wedge w): _pNode(n), _weight(w), _register(0), _bitw(NULL) {};
		TedKid(TedNode* n, wedge w, const TedRegister& r): _pNode(n), _weight(w), _register(r), _bitw(NULL) {};
		~TedKid(void) {
			_pNode = NULL;
			cleanBitwidth();
		}

		TedNode* Node(void)const { return _pNode; }
		void setNode(TedNode* node) { _pNode = node; }
		wedge getWeight(void)const { return _weight; }
		void setWeight(wedge w) { _weight = w; }
		TedRegister getRegister(void)const { return _register; }
		void setRegister(const TedRegister& w) { _register = w; }
		Bitwidth* getBitwidth(void)const { return _bitw; }
		void setBitwidth(Bitwidth* other) { _bitw = other; }
		void cleanBitwidth(void) {
			if(_bitw)
				delete _bitw;
			_bitw=NULL;
		}

		bool operator== (const TedKid& other)const {
			return(_pNode == other._pNode && _weight == other._weight && _register==other._register);
		}
		//bool operator<(TedKid & t) {
		//    if(_pNode < t._pNode) {
		//        return true;
		//    } else if(_pNode > t._pNode) {
		//        return false;
		//    } else {
		//        return _weight < t._weight;
		//    }
		//}
	};

}
#endif
