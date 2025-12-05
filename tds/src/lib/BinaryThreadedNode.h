/*
 * =====================================================================================
 *
 *       Filename:  BinaryThreadedNode.h
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

#ifndef __BINARYNODE_H__
#define __BINARYNODE_H__

#ifndef NULL
#define NULL 0
#endif

namespace dtl {

	template<typename Key, typename T, typename Comparator >
		class BinaryThreadedTree;

	template<typename Key, typename T, typename Comparator>
		class BinaryThreadedNode {
			friend class BinaryThreadedTree<Key, T, Comparator>;
		protected:
			Key _key;
			T _data;
			BinaryThreadedNode* _left;
			BinaryThreadedNode* _right;
			bool _thread_back;

			//protected constructor
			BinaryThreadedNode(void): _key(), _data(), _left(NULL), _right(NULL), _thread_back(0) {}
			BinaryThreadedNode(Key key, T data): _key(key), _data(data), _left(NULL), _right(NULL), _thread_back(0) {}
		public:
			T& obtainData(void) { return _data; }
			T getData(void)const { return _data; }
			Key getKey(void)const { return _key; }
		};

}
#endif

