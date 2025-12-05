/*
 * =====================================================================================
 *
 *       Filename:  DfgMap.h
 *    Description:
 *        Created:  10/01/2009 08:35:48 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-07 17:43:11 -0400 (Sat, 07 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */


#ifndef __MAPDFGNODE_H__
#define __MAPDFGNODE_H__

#include <string>
#include <cstring>
#include <map>

#include "DfgNode.h"
#include "DfgMan.h"
#include "ConvertMap.h"

namespace convert {
	using namespace dfg;
	using namespace std;

	struct _Node_MapOrder {
		bool operator() (DfgNode * n1, DfgNode * n2) const {
			DfgOperator::Type o1 = n1->getOp();
			DfgOperator::Type o2 = n2->getOp();
			//by default, order by operation type
			bool retval = (o1 < o2 );
			if (o1==o2) {
				DfgNode* n1L = n1->getLeft();
				DfgNode* n1R = n1->getRight();
				DfgNode* n2L = n2->getLeft();
				DfgNode* n2R = n2->getRight();
				switch (o1) {
				case DfgOperator::REG:
					assert(n1L==n1R && n2L==n2R);
					if (n1R != n2R) {
						//no match order by previous branch
						retval = (n1R < n2R);
					} else {
						//matched
						retval = false;
					}
					break;
				case DfgOperator::RSH: // fall through SUB
				case DfgOperator::LSH: // fall through SUB
				case DfgOperator::SUB:
					//match exactly both left and right branches
					if (n1L != n2L) {
						//no match, order by left branch
						retval = (n1L < n2L);
					} else if (n1R != n2R) {
						//partial match, order by right branch
						retval = (n1R < n2R);
					} else {
						//matched
						retval = false;
					}
					break;
				case DfgOperator::ADD: // fall through MUL
				case DfgOperator::MUL:
					//match operation regardless of branches
					if (n1L != n2L) {
						if (n1L != n2R) {
							//nothing math, order by left branch
							retval = (n1L < n2L);
						} else {
							if (n1R != n2L) {
								//partial match, order by left branch
								retval = (n1L < n2L);
							} else {
								//matched
								retval = false;
							}
						}
					} else {
						if (n1R != n2R) {
							//partial match, order by right branch
							retval = (n1R < n2R);
						} else {
							//matched
							retval = false;
						}
					}
					break;
				case DfgOperator::VARCONST: // fall through VAR
				case DfgOperator::VAR:
					//order by name
					retval = (strcmp(n1->getName(),n2->getName()) < 0);
					break;
				case DfgOperator::CONST:
					//order by const value
					retval = (n1->getValue() < n2->getValue());
					break;
				default:
					assert(false);
					break;
				}
			}
			return retval;
		}
	};

	class MapDfgNode : public ConvertMap<DfgNode*,DfgNode*,DfgMan*,DfgNode,_Node_MapOrder> {
	public:
		explicit MapDfgNode(PManagerType ptr) : ConvertMap<DfgNode*,DfgNode*,DfgMan*,DfgNode,_Node_MapOrder>(ptr) {};
		~MapDfgNode(void) {};

		void registrate(OuterKey, InnerKey, NodeType*);
		NodeType* get(OuterKey, InnerKey);
	};

	class MapDfgNodeR : public ConvertMap<DfgNode*,int,DfgMan*,DfgNode,_Node_MapOrder> {
	public:
		explicit MapDfgNodeR(PManagerType ptr) : ConvertMap<DfgNode*,int,DfgMan*,DfgNode,_Node_MapOrder>(ptr) {};
		~MapDfgNodeR(void) {};

		void registrate(OuterKey, InnerKey, NodeType*);
		NodeType* get(OuterKey, InnerKey);
	};
}
#endif

