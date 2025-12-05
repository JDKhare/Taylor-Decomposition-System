/*
 * =====================================================================================
 *
 *       Filename:  LinkedMap.h
 *    Description:
 *        Created:  05/17/2009 03:52:25 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 114                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-11-29 00:28:19 -0500 (Sun, 29 Nov 2009)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __LINKEDMAP_H__
#define __LINKEDMAP_H__

#include <map>
#include <list>
#include <functional>
#include <utility>
#include <iterator>

using namespace std;

namespace dtl {

	template<typename Key, typename T>
		class LinkedMap : public map<Key,T> {
		public:
			typedef pair<Key,T> value_pair;
			typedef list<Key>	value_list;
			typedef map<Key,T>	value_map;
			typedef pair<typename value_map::iterator,bool>	value_map_return;
			typedef typename value_list::iterator list_iterator;
			typedef typename value_list::reverse_iterator list_reverse_iterator;
		private:
			value_list _list;
		public:
			LinkedMap(void) {};
			~LinkedMap(void) { clear(); }
			void clear(void) {
				_list.clear();
				dynamic_cast<value_map*>(this)->clear();
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
			void insert(const value_pair& data) {
				//warning: this might insert a key twice, but still have only
				//one data stored in the map.
				_list.push_back(data.first);
				dynamic_cast<value_map*>(this)->insert(data);
			}
		};
}

#endif
