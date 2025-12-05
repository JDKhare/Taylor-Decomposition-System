/*
 * =====================================================================================
 *
 *        Filename:  TedNodeIterator.h
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDNODEITERATOR_H__
#define __TEDNODEITERATOR_H__

#include <vector>
#include <cassert>

#undef CONST_TEMPLATE_RESOLVED

#ifndef NULL
#define NULL 0
#endif

namespace ted {

	class TedNode;

	using namespace std;

	template<typename Head>
		class iteratorDFS {
		public:
			enum atBegin { begin };
			enum atEnd { end };
			vector<TedNode*> _traversalDFS;
			vector<TedNode*>::iterator _traversalPos;
			vector<TedNode*>::iterator _traversalEnd;
			TedNode* _current;
			iteratorDFS(void) : _current(NULL) {};
			explicit iteratorDFS<Head>(Head* root, atBegin elem) : _current(NULL) {
				_beginTraversal(root);
			};
			explicit iteratorDFS(atEnd elem) : _current(NULL) {
				_end();
			};
			iteratorDFS(const iteratorDFS& other) :
				_traversalDFS(other._traversalDFS), _traversalPos(other._traversalPos),
				_traversalEnd(other._traversalEnd), _current(other._current) {};
			~iteratorDFS(void) {_current=NULL;_traversalDFS.clear(); };
			TedNode* operator*(void) const { return _current; }
			TedNode* operator->(void) const { return _current; }
			iteratorDFS& operator= (const iteratorDFS& other) {
				_current = other._current;
				_traversalPos = other._traversalPos;
				_traversalEnd = other._traversalEnd;
				_traversalDFS = other._traversalDFS;
				return*this;
			}
			void operator++(int) {
				_traversalPos++;
				if(_traversalPos!=_traversalEnd)
					_current  = (*_traversalPos);
				else
					_current = NULL;
			}
			bool operator== (const iteratorDFS& other) {
				return (_current==other._current);
			}
			bool operator!= (const iteratorDFS& other) {
				return (_current!=other._current);
			}
		private:
			void _beginTraversal(Head* root);
			void _end(void) {
				_current = NULL;
			}
		};

	template<typename Head>
		void iteratorDFS<Head>::_beginTraversal(Head* root) {
			root->collectDFS(_traversalDFS);
			_traversalPos = _traversalDFS.begin();
			_traversalEnd = _traversalDFS.end();
			_current =*(_traversalPos);
		}

	template<typename Head>
		class const_iteratorDFS {
		public:
			enum atBegin { begin };
			enum atEnd { end };
			vector<TedNode*> _traversalDFS;
			vector<TedNode*>::iterator _traversalPos;
			vector<TedNode*>::iterator _traversalEnd;
			const TedNode* _current;
			const_iteratorDFS(void) :  _current(NULL) {};
			explicit const_iteratorDFS(const Head* c_root, atBegin elem) : _current(NULL) {
				_beginTraversal(c_root);
			};
			explicit const_iteratorDFS(atEnd elem) : _current(NULL) {
				_end();
			};
			const_iteratorDFS(const const_iteratorDFS& other) :
				_traversalDFS(other._traversalDFS), _traversalPos(other._traversalPos),
				_traversalEnd(other._traversalEnd),	_current(other._current) {};
			~const_iteratorDFS(void) {_current=NULL;_traversalDFS.clear(); };
			const TedNode* operator*(void) const { return _current; }
			const TedNode* operator->(void) const { return _current; }
			const_iteratorDFS& operator= (const const_iteratorDFS& other) {
				_current = other._current;
				_traversalPos = other._traversalPos;
				_traversalEnd = other._traversalEnd;
				_traversalDFS = other._traversalDFS;
				return*this;
			}
			void operator++(int) {
				_traversalPos++;
				if(_traversalPos!=_traversalEnd)
					_current  = (*_traversalPos);
				else
					_current = NULL;
			}
			bool operator== (const const_iteratorDFS& other) {
				return (_current==other._current);
			}
			bool operator!= (const const_iteratorDFS& other) {
				return (_current!=other._current);
			}
		private:
			void _beginTraversal(const Head* root);
			void _end(void) {
				_current = NULL;
			}
		};

	template<typename Head>
		void const_iteratorDFS<Head>::_beginTraversal(const Head* c_root) {
			Head* root = const_cast<Head*>(c_root);
			root->collectDFS(_traversalDFS);
			_traversalPos = _traversalDFS.begin();
			_traversalEnd = _traversalDFS.end();
			_current =*(_traversalPos);
		}

}
#endif

