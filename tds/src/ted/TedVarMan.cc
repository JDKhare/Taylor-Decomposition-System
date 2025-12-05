/*
 * =====================================================================================
 *
 *       Filename:  TedVarMan.cpp
 *    Description:
 *        Created:  12/4/2008 12:45:18 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 208                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include "TedVarMan.h"

#include "Bitwidth.h"
using data::Bitwidth;

namespace ted {
#ifndef INLINE
#include "TedVarMan.inl"
#endif

	TedVarMan* TedVarMan::_pInstance = NULL;
	bool TedVarMan::_destroyed = false;


	TedVar* TedVarMan::createVar(wedge val) {
		//do not register in the table
		return new TedVar(val);
	}

	TedVar* TedVarMan::createVar(const string& name,bool binary) {
		TedVar* inTable = getIfExist(name);
		if(!inTable) {
			inTable = new TedVar(name,binary);
			_table[name] = inTable;
		} else {
			if(binary && !inTable->isBinary()) {
				//warning: redefines a variable as binary
				//         it might already have non binary terms.
				cout << "Warning: Redefining variable \"" << name << "\" as a binary variable." << endl;
				cout << "         If any node in this variable had children with degree 2 or more" << endl;
				cout << "         operations (+,-,*,^) might result in an exception." << endl;
				inTable->setBinary();
			}
		}
		return inTable;
	}

	TedVar* TedVarMan::createVar(const string& name, const string& range) {
		TedVar* inTable = getIfExist(name);
		if(!inTable) {
			inTable = new TedVar(name,range);
			_table[name] = inTable;
		}
		assert(inTable->getRange() ==range);
		return inTable;
	}

	TedVar* TedVarMan::createVar(const string& name, wedge value) {
		TedVar* inTable = getIfExist(name);
		if(!inTable) {
			inTable = new TedVar(name,value);
			_table[name] = inTable;
		}
		assert(inTable->getValue() ==value);
		return inTable;
	}

	TedVar* TedVarMan::createVar(const TedVar& var) {
		switch(var.getType()) {
		case TedVar::CONST:
			return createVar(var.getValue());
		case TedVar::VAR:
			return createVar(var.getName());
		case TedVar::VARCONST:
			return createVar(var.getName(),var.getValue());
		case TedVar::VARBINARY:
			return createVar(var.getName(),true);
		default:
			throw(string("04039. UNKOWN variable type"));
		}
	}

	TedVar* TedVarMan::createRetimedVar(const TedVar& base, TedRegister regval) {
		assert(regval!=0);
		string name(base.getName());
		name += TOKEN_REG;
		name += Util::itoa(regval);
		TedVar* inTable = getIfExist(name);
		if(!inTable) {
			inTable = createVar(name);
			inTable->setBase(&base);
			inTable->setRegister(regval);
			_table[name] = inTable;
			return inTable;
		} else {
			//owner nor member do not have name of the form:  "base.getName().TOKEN_REG.regval"
			//so the following 2 cases should be an error
			if(inTable->isOwner()) {	
				TedVarGroupMember* memberTable = dynamic_cast<TedVarGroupOwner*>(inTable)->createMember();
				string newName = memberTable->getName();
				_table[newName] = memberTable;
				return memberTable;
			} else if (inTable->isMember()) {
				throw(string("04007. base variable ")+name+string(" is a member of ")+string(inTable->getOwner()->getName()));
			} else {
				assert((*inTable->getBase() == base)&&(inTable->getRegister() ==regval));
			}
			return inTable;
		}

	}

	TedVarGroupOwner* TedVarMan::createOwner(TedVar& var) {
		string name(var.getName());
		TedVar* trueVar = getIfExist(name);
		if(trueVar) {
			if(trueVar->isOwner()) {
				//throw(string("04028. variable ")+name+string(" already an owner"));
				std::cout << "Warning: variable " << name << " has been previously linearized" << std::endl;
				std::cout << "         variable members could require reordering" << std::endl;
				return dynamic_cast<TedVarGroupOwner*>(trueVar);
			}
			if(trueVar->isMember()) {
				throw(string("04029. variable ")+name+string(" is a member of ")+string(trueVar->getOwner()->getName()));
			}
			_table.erase(name);
			TedVarGroupOwner* inTable = new TedVarGroupOwner(*trueVar);
			_table[name] = inTable;
			return inTable;
		}
		throw(string("04030. variable ")+name+string(" was never inserted in TedVarMan"));
	}

	TedVarGroupMember* TedVarMan::createMember(TedVarGroupOwner& owner) {
		string name(owner.getName());
		map<string,TedVar*>::iterator it = _table.find(name);
		if(&owner == (*it).second) {
			TedVarGroupMember* inTable = owner.createMember();
			//make sure there is no member already in the table
			if(NULL == getIfExist(inTable->getName())) {
				_table[string(inTable->getName())] = inTable;
				return inTable;
			} else {
				throw(string("04031. variable member ")+string(inTable->getName())+string(" already exists"));
			}
		}
		throw(string("04032. variable ")+name+string(" claims to be an owner, but it's not"));
	}

	void TedVarMan::foreach(void(TedVar::*functor)(void)) {
		for(map<string,TedVar*>::iterator it = _table.begin(); it != _table.end(); it++) {
			(it->second->*functor)();
		}
	}

}
