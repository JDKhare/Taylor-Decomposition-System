/*
 * =====================================================================================
 *
 *       Filename:  ATedNode.h
 *    Description:
 *        Created:  11/10/2008 7:05:57 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-01-06 14:48:56 +0100(Wed, 06 Jan 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __ATEDNODE_H__
#define __ATEDNODE_H__

#include "TedNode.h"
#include "TedKids.h"
#include "TedContainer.h"

#include <map>

namespace ted {

	/**
	 * @class ATedNode
	 * @brief Defines an abstract and expanded TED node class, to support arithmetic operations.
	 **/
	class ATedNode : public TedNode {
	protected:
		static set<ATedNode*> _transitoryContainer;
		static TedContainer* _permanentContainer;

		//Build recursively ATed nodes from a Ted node
		template<typename T>
			static ATedNode* BuildRecursively(const TedNode* t);
		template<typename T>
			static ATedNode* BuildRecursively(const TedNode* t, TedRegister regVal = 0);

	public:
		explicit ATedNode(const TedNode& other): TedNode(other) { _transitoryContainer.insert(this); }
		explicit ATedNode(void): TedNode() { _transitoryContainer.insert(this); }
		explicit ATedNode(wedge constvalue): TedNode(constvalue) { _transitoryContainer.insert(this); }
		explicit ATedNode(const ATedNode& other): TedNode(other) { _transitoryContainer.insert(this); }
		explicit ATedNode(const TedVar & t, const TedKids & k, const TedRegister & r): TedNode(t,k,r) { _transitoryContainer.insert(this); }
		virtual ~ATedNode() {
			set<ATedNode*>::iterator it = _transitoryContainer.find(this);
			if (it!=_transitoryContainer.end()) {
				//the node might be located in the _permanentContainer of TedMan
				_transitoryContainer.erase(it);
			}
		};
		//Arithmetic operations
		virtual ATedNode* add(ATedNode* atNode) = 0;
		virtual ATedNode* sub(ATedNode* atNode) = 0;
		virtual ATedNode* mul(ATedNode* atNode) = 0;
		virtual ATedNode* div(ATedNode* atNode) = 0;
		virtual ATedNode* exp(ATedNode* atNode) = 0;
		virtual ATedNode* reg(ATedNode* atNode) = 0;

		virtual ATedNode* clone(void) const = 0;
		ATedNode* duplicateRecursive(void) const;
		ATedNode* duplicateRecursiveRec(std::map<const ATedNode*,ATedNode*>& old2new) const;

		static void setContainer(TedContainer& tedCont);
		static bool checkContainer(TedContainer& tedCont);
		static void purge_transitory_container(void);
		static bool transitory_container_empty(void);
		static void print_transitory_container(void);
		static void garbage_collect_unmarked(bool (Mark<TedNode>::*functor)(void) const);
	};


	template<typename T>
		ATedNode* ATedNode::BuildRecursively(const TedNode* t) {
			ATedNode* pENode = NULL,* pEKid = NULL;
			TedNode* pKid = NULL;
			unsigned int index = 0;
			wedge weight = 0;
			if(!t->isConst()) {
				pENode = new T(0);
				FOREACH_KID_OF_NODE(t) {
					index = _iterkids.getIndex();
					pKid =_iterkids.Node<TedNode>();
					weight =_iterkids.getWeight();
					pEKid = BuildRecursively<T>(pKid);
					pEKid = pEKid->mul(new T(weight));
					for(unsigned int i = 0; i < index; i++) {
						pEKid = pEKid->mul(new T(t->getName()));
					}
					pENode = pENode->add(pEKid);
				}
			} else {
				pENode = new T(1);
			}
			assert(pENode);
			return pENode;
		}

	// NOTE: a^2*a@2        --> a in top a@2 at bottom
	//(a^2*a@2)@3
	//
	// build recursively starts at top a^2 and before transforming it into(a@2)^2
	// it recursively generates(a@5).
	//
	// When it comes back to do(a@5)*(a@2)*(a@2)
	// variable(a@5)becomes the top variable
	//
	template<typename T>
		ATedNode* ATedNode::BuildRecursively(const TedNode* t,TedRegister regVal) {
			ATedNode* pENode = NULL;
			if(!t->isConst()) {
				pENode = NULL;
				const TedVar* newVar = NULL;
				if(regVal > 0) {
					const TedVar* currentVar = TedVarMan::instance().createVar(*t->getVar());
					const TedVar* baseVar = currentVar->getBase();
					if(baseVar) {
						newVar = TedVarMan::instance().createRetimedVar(*baseVar,regVal+currentVar->getRegister());
					} else {
						newVar = TedVarMan::instance().createRetimedVar(*currentVar,regVal);
					}
					_permanentContainer->registerRetimedVar(*newVar);
				} else {
					newVar = t->getVar();
				}
				FOREACH_KID_OF_NODE(t) {
					unsigned int index = _iterkids.getIndex();
					TedNode* pKid =_iterkids.Node<TedNode>();
					wedge weight =_iterkids.getWeight();
					ATedNode* pEKid = BuildRecursively<T>(pKid,regVal);
					pEKid = pEKid->mul(new T(weight));
					for(unsigned int i = 0; i < index; i++) {
						T* node = new T(newVar->getName());
						node->setRegister(newVar->getRegister());
						pEKid = pEKid->mul(node);
					}
					pENode = (pENode) ? pENode->add(pEKid): pEKid;
				}
			} else {
				pENode = new T(t->getConst());
			}
			assert(pENode);
			return pENode;
		}


}
#endif
