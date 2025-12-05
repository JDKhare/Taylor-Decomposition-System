/*
 * =====================================================================================
 *
 *       Filename:  DfgOperator.h
 *    Description:
 *        Created:  11/20/2009 09:44:13 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 208                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-01-26 13:54:58 -0500(Tue, 26 Jan 2010)$: Date of last commit
 * =====================================================================================

*/
#ifndef __DFGOPERATOR_H__
#define __DFGOPERATOR_H__

#include "Bitwidth.h"
#include "Error.h"
using data::Error;
using data::Bitwidth;


namespace dfg {

	enum RoundingType {
		ZR,  //toward zero     i.e -49.5 becomes -49 && 49.5 becomes 49  truncate
		AW,  //away from zero  i.e -49.5 becomes -50 && 49.5 becomes 50
		DN,  //toward -oo      i.e -49.5 becomes -50 && 49.5 becomes 49  floor
		UP   //toward +oo      i.e -49.5 becomes -49 && 49.5 becomes 50  ceiling
	};

	class DfgNode;

	class DfgOperator {
	public:
		static unsigned int idcount;
		static map<unsigned int,DfgOperator*> manager;
		enum Type { ERROR=0, ADD, MUL, DIV, SUB, LSH, RSH, VAR, CONST, VARCONST, REG, EQ };

	private:
		Bitwidth* _bitw;
		RoundingType _rounding;
		unsigned int _id;
		unsigned int _count;
		Type _type;
		Error* _error;

	public:
		DfgOperator(Bitwidth* pre, Type type);
		DfgOperator(Type type);
		~DfgOperator(void);

		Error* getError(void)const;
		Bitwidth* getBitwidth(void)const;
		void set(Bitwidth* pre);
		void set(Error* pre);
		void clearBitwidth(void);
		void clearError(void);
		unsigned int getID(void)const;
		DfgOperator::Type getOp(void);
		void setType(Type type);
	};


#ifdef INLINE
#include "DfgOperator.inl"
#endif

}
#endif

