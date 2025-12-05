/*
 * =====================================================================================
 *
 *       Filename:  LinkedSet.h
 *    Description:
 *        Created:  11/28/2009 11:00:51 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 117                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-11-30 15:17:16 -0500 (Mon, 30 Nov 2009)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __LINKEDSET_H__
#define __LINKEDSET_H__

#include <set>
#include <list>
#include <functional>
#include <utility>
#include <iterator>

using namespace std;

namespace dtl {

	template<typename Key >
		class LinkedSet : public set<Key> {
		public:
			typedef list<Key>	value_list;
			typedef set<Key>	value_set;
			typedef pair<typename value_set::iterator,bool>	value_set_return;
			typedef typename value_list::iterator list_iterator;
			typedef typename value_list::reverse_iterator list_reverse_iterator;
		private:
			value_list _list;
		public:
			LinkedSet(void) {};
			~LinkedSet(void) { clear(); }
			void clear(void) {
				_list.clear();
				dynamic_cast<value_set*>(this)->clear();
			}
			list_reverse_iterator list_rbegin(void) {
				return _list.rbegin();
			}
			list_reverse_iterator list_rend(void) {
				return _list.rend();
			}
			list_iterator list_begin(void) {
				return _list.begin();
			}
			list_iterator list_end(void) {
				return _list.end();
			}

			value_set_return insert(const Key& data) {
				value_set_return onset = dynamic_cast<value_set*>(this)->insert(data);
				if(onset.second) {
					_list.push_back(data);
				}
				return onset;
			}
			Key& front(void) {
				return _list.front();
			}
			const Key& front(void) const {
				return _list.front();
			}
			void pop_front(void) {
				dynamic_cast<value_set*>(this)->erase(_list.front());
				_list.pop_front();
			}
		};
}

#endif

