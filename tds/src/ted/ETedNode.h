/*
 * =====================================================================================
 *
 *        Filename:  TedENode.h
 *     Description:  Expanded TedNode class weights are always one
 *         Created:  04/17/2007 03:27:43 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 207                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __ETEDNODE_H__
#define __ETEDNODE_H__

#include "ATedNode.h"

namespace ted {

	class TedMan;

	/**
	 * @class ETedNode
	 * @brief Implements Euler Arithmetic operations.
	 **/
	class ETedNode : public ATedNode {
	public:
		explicit ETedNode(const TedNode& other): ATedNode(other) {};
		explicit ETedNode(const string & var,bool binary=false);
		explicit ETedNode(const string & var, const string & range);
		explicit ETedNode(const string & constvar, wedge value);
		explicit ETedNode(wedge constvalue);
		explicit ETedNode(const ETedNode& other): ATedNode(other) {};
		explicit ETedNode(const TedVar & t, const TedKids & k, const TedRegister & r): ATedNode(t,k,r) {};
		virtual ~ETedNode(void) {};

		//ETED arithmetic operations
		ATedNode* add_with_content(TedNode* etNode, TedRegister reg);
		ATedNode* add_with_content(wedge constvalue);
		ATedNode* add(ATedNode* etNode);
		ATedNode* sub(ATedNode* etNode);
		ATedNode* mul(ATedNode* etNode);
		ATedNode* div(ATedNode* etNode) { return NULL; };
		ATedNode* exp(ATedNode* etNode);
		ATedNode* reg(ATedNode* atNode);

		bool isErasable(TedNode* ptr);

		virtual ATedNode* clone(void) const;

		static ATedNode* BuildRecursive(const TedNode* t, TedRegister regVal=0);
	};

#ifdef INLINE
#include "ETedNode.inl"
#endif

}
#endif
