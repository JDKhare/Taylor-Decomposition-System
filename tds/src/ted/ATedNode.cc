/*
   l
 * =====================================================================================
 *
 *       Filename:  ATedNode.cc
 *    Description:
 *        Created:  11/10/2008 7:23:33 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 186                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <list>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "ATedNode.h"
#include "TedNode.h"
#include "TedMan.h"
#include "TedKids.h"
#include "TedVar.h"

namespace ted {

	set<ATedNode*> ATedNode::_transitoryContainer;
	TedContainer* ATedNode::_permanentContainer = NULL;

	void ATedNode::setContainer(TedContainer& tedCont) {
		_permanentContainer = &tedCont;
	}

	bool ATedNode::checkContainer(TedContainer& tedCont) {
		return(_permanentContainer == &tedCont);
	}

	void ATedNode::purge_transitory_container(void) {
#if 0
		for(set<ATedNode*>::iterator it = _transitoryContainer.begin(); it != _transitoryContainer.end(); it++) {
			ATedNode* pNode = (*it);
			assert(pNode);
			if (pNode->visited.isMarked()) {
				_transitoryContainer.erase(it);
			} else {
				delete pNode;
			}
		}
#else
		set<ATedNode*>::iterator it = _transitoryContainer.begin();
		while (it != _transitoryContainer.end()) {
			ATedNode* pNode = (*it);
			assert(pNode);
			if (pNode->visited.isMarked()) {
				_transitoryContainer.erase(it++);
			} else {
				++it;
				delete pNode;
			}
		}
#endif
		//All nodes that remains are located in a TedMan::getContainer()
		//so it is safe to disposse of the _transitoryContainer
		_transitoryContainer.clear();
	}

	void ATedNode::garbage_collect_unmarked(bool (Mark<TedNode>::*functor)(void) const) {
#if 0
		set<ATedNode*> tcopy;
		tcopy = _transitoryContainer;
		for(set<ATedNode*>::iterator it = tcopy.begin(); it != tcopy.end(); it++) {
			ATedNode* pNode = (*it);
			assert(pNode);
			if (!(pNode->visited.*functor)()) {
				_transitoryContainer.erase(it);
				delete pNode;
			}
		}
#else
		set<ATedNode*>::iterator it = _transitoryContainer.begin();
		while (it != _transitoryContainer.end()) {
			ATedNode* pNode = (*it);
			assert(pNode);
			if (!(pNode->visited.*functor)()) {
				_transitoryContainer.erase(it++);
				delete pNode;
			} else {
				++it;
			}
		}
#endif
	}

	/** @brief Duplicate a ETed recursively
	 *  @details The duplicated nodes are stored in the transitory container
	 **/
	ATedNode* ATedNode::duplicateRecursive(void) const {
		//Mark<TedNode>::newMark();
		map<const ATedNode*,ATedNode*> old2new;
		ATedNode* tmp = duplicateRecursiveRec(old2new);
		//Mark<TedNode>::newMark();
		return tmp;
	}

	ATedNode* ATedNode::duplicateRecursiveRec(map<const ATedNode*,ATedNode*>& old2new) const {
		//if(visited.isMarked()) {
		//	return old2new[this];
		//}
		if (old2new.find(this)!=old2new.end()) {
			return old2new[this];
		}
		//unsigned int index = 0;
		//ATedNode* pKid = NULL;
		//wedge      weight = 0;
		ATedNode* t = this->clone();
		_transitoryContainer.insert(t);
		if(!this->isConst()) {
			t->_tKids = TedKids::create_kids();
			FOREACH_KID_OF_NODE(this) {
				unsigned int index = _iterkids.getIndex();
				ATedNode* pKid = _iterkids.Node<ATedNode>();
				t->addKid(index,pKid->duplicateRecursiveRec(old2new),1);
			}
		}
		//visited.setMark();
		old2new[this] = t;
		return t;
	}

	void ATedNode::print_transitory_container(void) {
		ofstream out("tc_atednode.txt");
		out << " node_pointer | Tmark | Pmark | variable_ptr | kid_pointer " << endl;
		unsigned tmarked = 0;
		unsigned untmarked = 0;
		unsigned pmarked = 0;
		unsigned unpmarked = 0;
		for(set<ATedNode*>::const_iterator it = _transitoryContainer.begin(); it != _transitoryContainer.end(); it++) {
			ATedNode* pNode = (*it);
			out << " ";
			out.width(12);
			out.fill(' ');
			out << pNode << " | ";
			if (pNode->visited.isMarked()) {
				tmarked++;
				out << " MM " << " | ";
			} else {
				untmarked++;
				out << " UU " << " | ";
			}
			if (pNode->visited.isPMarked()) {
				pmarked++;
				out << " MM " << " | ";
			} else {
				unpmarked++;
				out << " UU " << " | ";
			}
			out.width(12);
			out.fill(' ');
			out << pNode->getVar() << " | ";
			out.width(11);
			out.fill(' ');
			out << pNode->getKids();
#ifdef _DEBUG
			out << " | " << hex << pNode->safe_guard;
			out << dec;
#endif
			out << endl;
		}
		out << "size=" << _transitoryContainer.size()<< endl;
		out << "tmarked=" << tmarked << "  untmarked=" << untmarked << endl;
		out << "pmarked=" << pmarked << "  unpmarked=" << unpmarked << endl;
		out.close();
		//	cout << "ATedNode report in transitory container printed" << endl;
	}

	bool ATedNode::transitory_container_empty() {
		return _transitoryContainer.empty();
	}
}
