/*
 * =====================================================================================
 *
 *       Filename:  BinaryThreadedTree.h
 *    Description:
 *        Created:  04/22/2009 08:52:25 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 149                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400(Wed, 07 Oct 2009)$: Date of last commit
 * =====================================================================================
 */

#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

#include "BinaryThreadedNode.h"
#include <functional>

namespace dtl {

	template<typename Key, typename T, typename Compare = std::less<Key> >
		class BinaryThreadedTree {
		protected:
			typedef BinaryThreadedNode<Key,T,Compare> Node;
			Node* _root;
		public:
			BinaryThreadedTree(void): _root(NULL) {};

			bool isEmpty(void) { return(_root == NULL); }
			void insert(Key key, T data);
			void inorder(void);

			class inorder_iterator {
			protected:
				Node* _previous;
				Node* _present;
			public:
				Node* operator*(void)const;
				inorder_iterator& operator++(void);
				//inorder_iterator operator++(int);
				bool operator== (const inorder_iterator&)const;
				bool operator!= (const inorder_iterator&)const;
				inorder_iterator(void): _previous(NULL), _present(NULL) {};
				inorder_iterator(Node* head): _previous(NULL), _present(head) {};
			};

			inorder_iterator begin(void);
			inorder_iterator end(void);
		};

	template<typename Key, typename T, typename Compare>
		typename BinaryThreadedTree<Key,T,Compare>::inorder_iterator& BinaryThreadedTree<Key,T,Compare>::inorder_iterator::operator++(void) {
			if(NULL!=_present) {
				_previous = _present;
				_present = _present->_right;
				if(NULL!=_present && !_previous->_thread_back) {
					while(NULL!=_present->_left) {
						_present = _present->_left;
					}
				}
			}
			return(*this);
		}

	template<typename Key, typename T, typename Compare>
		BinaryThreadedNode<Key,T,Compare>* BinaryThreadedTree<Key,T,Compare>::inorder_iterator::operator*(void)const {
			return _present;
		}

	template<typename Key, typename T, typename Compare>
		bool BinaryThreadedTree<Key,T,Compare>::inorder_iterator::operator== (const inorder_iterator& other)const {
			if(other._present==this->_present)
				return true;
			else
				return false;
		}

	template<typename Key, typename T, typename Compare>
		bool BinaryThreadedTree<Key,T,Compare>::inorder_iterator::operator!= (const inorder_iterator& other)const {
			if(other._present!=this->_present)
				return true;
			else
				return false;
		}

	template<typename Key, typename T, typename Compare>
		typename BinaryThreadedTree<Key,T,Compare>::inorder_iterator BinaryThreadedTree<Key,T,Compare>::begin(void) {
			Node* present = _root;
			if(NULL!=present) {
				while(NULL!=present->_left)
					present = present->_left;
			}
			return inorder_iterator(present);
		}

	template<typename Key, typename T, typename Compare>
		typename BinaryThreadedTree<Key,T,Compare>::inorder_iterator BinaryThreadedTree<Key,T,Compare>::end(void) {
			return inorder_iterator();
		}

	template<typename Key, typename T, typename Compare>
		void BinaryThreadedTree<Key,T,Compare>::inorder(void) {
			Node* previous = NULL;
			Node* present = _root;
			if(NULL!=present) {
				while(NULL!=present->_left)
					present = present->_left;
				while(NULL!=present) {
					previous = present;
					present = present->_right;
					if(NULL!=present && !previous->_thread_back) {
						while(NULL!=present->_left) {
							present = present->_left;
						}
					}
				}
			}
		}

	template<typename Key, typename T, typename Compare >
		void BinaryThreadedTree<Key,T,Compare>::insert(Key key, T data) {
			Node* present = NULL;
			Node* previous = NULL;
			Node* newNode = new Node(key,data);
			if(NULL==_root) {
				_root = newNode;
				return;
			}
			present = _root;
			while(NULL!=present) {
				previous = present;
				if(Compare()(key,present->_key)) {
					present = present->_left;
				} else if(!present->_thread_back) {
					// go to the right node only if it is a descendant
					present = present->_right;
				} else {
					// don't follow successor link
					break;
				}
			}
			if(Compare()(key,previous->_key)) {
				// if new node is left child of this parent, the parent also becomes its successor
				previous->_left = newNode;
				newNode->_thread_back = true;
				newNode->_right = previous;
			} else if(previous->_thread_back) {
				//if the new node is not the rightmost node, make parent's successor, new node's successor
				newNode->_thread_back = true;
				previous->_thread_back = false;
				newNode->_right = previous->_right;
				previous->_right = newNode;
			} else {
				// it has no successor
				previous->_right = newNode;
			}
		}

}
#endif

