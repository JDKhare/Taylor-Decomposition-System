/*
 * =====================================================================================
 *
 *        Filename:  TedKidsIterator.h
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-10-15 20:28:55 -0400(Thu, 15 Oct 2009)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TEDKIDSITERATOR_H__
#define __TEDKIDSITERATOR_H__

#include <vector>
#include <cassert>
#include "TedKid.h"


#undef CONST_TEMPLATE_RESOLVED

namespace ted {

	using namespace std;
	class TedNode;

	template<typename iterType>
		class iteratorKids : public iterType {
		public:
			iteratorKids(const iterType& it): iterType(it) {};
			~iteratorKids(void) {};
			int getIndex(void)const {
				const iterType* it = dynamic_cast<const iterType*>(this);
				return(*it)->first;
			}
			TedKid& getKid(void)const{
				const iterType* it = dynamic_cast<const iterType*>(this);
				return(*it)->second;
			}
			wedge getWeight(void)const {
				const iterType* it = dynamic_cast<const iterType*>(this);
				return(*it)->second.getWeight();
			}
			TedRegister getRegister(void)const {
				const iterType* it = dynamic_cast<const iterType*>(this);
				return(*it)->second.getRegister();
			}
			TedKid& operator*(void) {
				const iterType* it = dynamic_cast<const iterType*>(this);
				return(*it)->second;
			}
			iterType operator->(void) {
				iterType* it = dynamic_cast<iterType*>(this);
				return(*it);
			}
			iterType operator->(void) const {
				iterType* it = dynamic_cast<iterType*>(this);
				return(*it);
			}

			template<typename T>
				T* Node(void)const {
					const iterType* it = dynamic_cast<const iterType*>(this);
					return dynamic_cast<T*>((*it)->second.Node());
				}
#ifdef _MSC_VER
			template<>
				TedNode* Node<TedNode>(void)const {
					const iterType* it = dynamic_cast<const iterType*>(this);
					return(*it)->second.Node();
				}
#endif
		};

}
#endif

