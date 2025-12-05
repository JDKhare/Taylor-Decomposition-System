#include "TedNodeIterator.h"
#include "TedNode.h"
#include "TedMan.h"

namespace ted {

	namespace {
		const bool AVOID_ANY_PROB = true;
	};

}

#if 0
#ifdef _MSC_VER
/*
   template iteratorDFS<TedNode>;
   template iteratorDFS<TedMan>;

   template<typename T>
   void iteratorDFS<T>::_beginTraversal(T* root) {
   root->collectDFS(_traversalDFS);
   _traversalPos = _traversalDFS.begin();
   _traversalEnd = _traversalDFS.end();
   _current = *(_traversalPos);
   }
   */
#else
template<>
void iteratorDFS<TedMan>::_beginTraversal(TedMan* root) {
	root->collectDFS(_traversalDFS);
	_traversalPos = _traversalDFS.begin();
	_traversalEnd = _traversalDFS.end();
	_current = *(_traversalPos);
}

template<>
void iteratorDFS<TedNode>::_beginTraversal(TedNode* root) {
	root->collectDFS(_traversalDFS);
	_traversalPos = _traversalDFS.begin();
	_traversalEnd = _traversalDFS.end();
	_current = *(_traversalPos);
}
#endif

#ifdef CONST_TEMPLATE_RESOLVED
//TOFIX the problem might lie in the argument must be const TedNode*
//if so, then we need to remove the const of the visited mark
template<>
void const_iteratorDFS<TedNode>::_beginTraversal(const TedNode* c_root) {
	Mark<TedNode>::newMark();
	TedNode* root = const_cast<TedNode*>(c_root);
	root->collectDFS(_traversalDFS);
	//root->_collectDFS_rec(_traversalDFS);
	_traversalPos = _traversalDFS.begin();
	_traversalEnd = _traversalDFS.end();
	_current = *(_traversalPos);
}

template<>
void const_iteratorDFS<TedMan>::_beginTraversal(const TedMan* c_root) {
	Mark<TedNode>::newMark();
	TedMan* root = const_cast<TedMan*>(c_root);
	root->collectDFS(_traversalDFS);
	/*for (PrimaryOutputs::iterator p = root->_pos.begin(), end = root->_pos.end(); p != end; p++) {
	  p->second.Node()->_collectDFS_rec(_traversalDFS);
	  }*/
	_traversalPos = _traversalDFS.begin();
	_traversalEnd = _traversalDFS.end();
	_current = *(_traversalPos);
}
#endif

#endif


