/*
 * =====================================================================================
 *
 *       Filename:  ConvertMap.h
 *    Description:
 *        Created:  10/01/2009 08:35:48 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __CONVERTMAP_H__
#define __CONVERTMAP_H__

#include <map>
using namespace std;

namespace convert {

	template <typename aKey, typename bKey, typename Manager, \
		typename Node, class aComp=less<aKey>, class bComp=less<bKey> >
		class ConvertMap {
		public:
			typedef Node NodeType;
			typedef Manager PManagerType;
			typedef const aKey OuterKey;
			typedef const bKey InnerKey;
			typedef map<InnerKey,NodeType*,bComp> Data;
			typedef map<OuterKey,Data,aComp> Database;
			typedef typename Data::iterator data_iterator;
			typedef typename Database::iterator database_iterator;

			explicit ConvertMap(PManagerType ptr) : _pMan(ptr) {};
			~ConvertMap(void) { _pMan=NULL; }

			bool has(OuterKey key) {
				database_iterator found = _data.find(key);
				if (found != _data.end()) {
					return true;
				}
				return false;
			}
			bool has(OuterKey okey,InnerKey ikey) {
				if (has(okey)) {
					Data& datum = _data[okey];
					data_iterator found = datum.find(ikey);
					if (found != datum.end()) {
						return true;
					}
				}
				return false;
			}
			virtual void registrate(OuterKey, InnerKey, NodeType *) = 0;
			virtual NodeType * get(OuterKey, InnerKey) = 0;
		protected:
			Database _data;
			PManagerType _pMan;
		};

}
#endif

