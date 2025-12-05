/*
 * =====================================================================================
 *
 *       Filename:  TedMan.inl
 *    Description:
 *        Created:  11/8/2008 8:17:47 PM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif


/** @brief Set the constant limit*/
inline
void TedMan::setConstLimit(size_t l) {
	_const_limit = l;
}


/** @brief Get the maximum level without constants*/
inline
size_t TedMan::getMaxLevelWoConst(void)const {
	return getConstLimit();
}

/** @brief Unlinking a primary output*/
inline
TedMan* TedMan::unlinkPO(set <string> s) {
	return  _DuplicateUnlink(& s);
}

/** @brief GEt the level of a TedNode*/
inline
size_t TedMan::getLevel(const TedNode* v)const {
	if(v->isConst())
		return 0;
	return(_container.size()- _container.find(*v->getVar())->second.first +1);
}

/** @brief Does the database contain this variable*/
inline
bool TedMan::hasVar(const string & v)const {
	if(_container.find(TedVar(v)) == _container.end()) {
		return false;
	}
	return true;
}

/** @brief Is Pnode a simple variable or not*/
inline
bool TedMan::isSimpleVariable(const TedNode* pNode)const {
	if(pNode->numKids() == 1 && pNode->getKidNode(pNode->getKids()->highestOrder()) == TedNode::getOne()) {
		return true;
	}
	return false;
}

/*return a simple TedNode which represents variables*/
inline
TedNode* TedMan::getNode(const TedVar & v) {
	TedKids tk;
	tk.insert(1, TedNode::getOne(), 1, TedRegister(0));
	return _container.getNode(v, tk);
}

/** @brief return a simple TedNode from the variable*/
inline
TedNode* TedMan::getNode(const string & p) {
	TedKids tk;
	TedVar v(p);
	tk.insert(1, TedNode::getOne(), 1,TedRegister(0));
	return _container.getNode(v, tk);
}

/** @brief Get the constant limit*/
inline
size_t TedMan::getConstLimit(void)const {
	if(_const_limit != 0 && _const_limit < _container.size()) {
		return _const_limit;
	} else {
		return _container.size();
	}
}

inline
TedMan* TedMan::duplicate(void) {
	return _DuplicateUnlink();
}

inline
TedMan* TedMan::fixorderRetimedVars(bool verboseOnly) {
	TedOrderProper order(this);
	return order.fixorderRetimedVars(verboseOnly);
}

inline
TedMan* TedMan::fixorderVars(bool verboseOnly) {
	TedOrderProper order(this);
	return order.fixorderVars(verboseOnly);
}

inline
TedMan* TedMan::patchorderVars(void) {
	TedOrderProper order(this);
	return order.patchorderVars();
}


/** @brief Push the variable to Bottom*/
inline
TedMan* TedMan::bottomVariable(const string & pVar, TedOrderStrategy* order) {
	TedVar* pvar = checkVariables(pVar);
	order = new TedOrderReloc(this);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	if(owner) {
		return order->toBottom(*owner);
	}
	return order->toBottom(*pvar);
}

/** @brief Push the variable to top*/
inline
TedMan* TedMan::topVariable(const string & pVar, TedOrderStrategy* order) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	if(owner) {
		return order->toTop(*owner);
	}
	return order->toTop(*pvar);
}

/** @brief Register a variable in the database(_container)*/
inline
void TedMan::registerVar(const string & varName, bool binary) {
	// there is no point in trying to obtain the variable from the TedVarMan
	// as _container cast the key to an object TedVar, thus loosing any additional
	// information provided by TedVarMan
	//_container.registerVar(TedVar(varName));
	_container.registerVar(*TedVarMan::instance().createVar(varName,binary));
}

inline
void TedMan::registerVar(const string & varName, wedge value) {
	//initialization of a variable with a constant value
	//_container.registerVar(TedVar(varName,value));
	_container.registerVar(*TedVarMan::instance().createVar(varName,value));
}

inline
void TedMan::registerVars(vector<string>& vVars) {
	for(size_t i = 0; i < vVars.size(); i++) {
		registerVar(vVars[i].c_str());
	}
}

inline
void TedMan::registerVars(list<TedVar*>& vVars) {
	for(list<TedVar*>::iterator it = vVars.begin(); it != vVars.end(); it++) {
		_container.registerVar(*(*it));
	}
}

inline
TedVar* TedMan::checkVariables(const string& pVar) {
	if(_vars.isEmpty()) {
		throw(string("04033. No variables registered"));
	}
	TedVar* pvar = _vars.getIfExist(pVar);
	if(NULL==pvar) {
		throw(string("04034. variable ")+ string(pVar)+ string(" doesn't exist"));
	}
	return pvar;
}

#if 0
inline
TedMan* TedMan::jumpAbove(const string& pVar1, const string& pVar2, TedOrderStrategy* order) {
	TedVar* pvar1 = checkVariables(pVar1);
	TedVar* pvar2 = checkVariables(pVar2);
	if(!pvar1 || !pvar2 || pvar1==pvar2)
		return NULL;
	int jump = _container.getLevel(*pvar2)- _container.getLevel(*pvar1);
	assert(0!=jump);
	return(jump>0) ? bblUpVariable(pVar1,order,jump): bblDownVariable(pVar1,order,-(++jump));
}

inline
TedMan* TedMan::jumpBelow(const string& pVar1, const string& pVar2, TedOrderStrategy* order) {
	TedVar* pvar1 = checkVariables(pVar1);
	TedVar* pvar2 = checkVariables(pVar2);
	if(!pvar1 || !pvar2 || pvar1==pvar2)
		return NULL;
	int jump = _container.getLevel(*pvar2)- _container.getLevel(*pvar1);
	assert(0!=jump);
	return(jump>0) ? bblUpVariable(pVar1,order,(--jump)): bblDownVariable(pVar1,order,-jump);
}

inline
TedMan* TedMan::exchange(const string& pVar1, const string& pVar2, TedOrderStrategy* order) {
	string upper(pVar1);
	string lower(pVar2);
	TedVar* pvar1 = checkVariables(upper);
	TedVar* pvar2 = checkVariables(lower);
	if(!pvar1 || !pvar2 || pvar1==pvar2)
		return NULL;
	int jump = _container.getLevel(*pvar1)- _container.getLevel(*pvar2);
	assert(0!=jump);
	if(jump<0) {
		string name(upper);
		upper = lower;
		lower = upper;
		jump*= -1;
	}
	//jump below upper
	jump--;
	TedMan* last = bblUpVariable(lower,order,jump);
	//upper jumps the same size below
	jump++;
	last = bblDownVariable(upper,order,jump);
	return last;
}
#endif
inline
TedMan* TedMan::jumpAboveVariable(const string& pVar1, const string& pVar2, TedOrderStrategy* order) {
	TedVar* pvar1 = checkVariables(pVar1);
	TedVar* pvar2 = checkVariables(pVar2);
	TedVarGroupOwner* owner1 = dynamic_cast<TedVarGroupOwner*>(pvar1);
	if(owner1) {
		return order->jumpAbove(*owner1,*pvar2);
	}
	return order->jumpAbove(*pvar1,*pvar2);
}

inline
TedMan* TedMan::jumpBelowVariable(const string& pVar1, const string& pVar2, TedOrderStrategy* order) {
	TedVar* pvar1 = checkVariables(pVar1);
	TedVar* pvar2 = checkVariables(pVar2);
	TedVarGroupOwner* owner1 = dynamic_cast<TedVarGroupOwner*>(pvar1);
	if(owner1) {
		return order->jumpBelow(*owner1,*pvar2);
	}
	return order->jumpBelow(*pvar1,*pvar2);
}

inline
TedMan* TedMan::exchangeVariable(const string& pVar1, const string& pVar2, TedOrderStrategy* order) {
	TedVar* pvar1 = checkVariables(pVar1);
	TedVar* pvar2 = checkVariables(pVar2);
	TedVarGroupOwner* owner1 = dynamic_cast<TedVarGroupOwner*>(pvar1);
	if(owner1) {
		return order->exchange(*owner1,*pvar2);
	}
	return order->exchange(*pvar1,*pvar2);
}

inline
void TedMan::eval(const string& pVar, int value) {
	TedVar* pvar = checkVariables(pVar);
	eval(*pvar,value);
}

/** @brief Bubble up a variable*/
inline
TedMan* TedMan::bblUpVariable(const string & pVar, TedOrderStrategy* order) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	if(owner) {
		return order->bblUp(*owner);
	}
	return order->bblUp(*pvar);
}

inline
TedMan* TedMan::bblDownVariable(const string & pVar, TedOrderStrategy* order) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	if(owner) {
		return order->bblDown(*owner);
	}
	return order->bblDown(*pvar);
}

/** @brief Bubble up a variable*/
inline
TedMan* TedMan::bblUpVariable(const string & pVar, TedOrderStrategy* order, unsigned int count) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	TedMan* last = NULL;
	if(owner) {
		while(count!=0) {
			last = order->bblUp(*owner);
			count--;
		}
	} else {
		while(count!=0) {
			last = order->bblUp(*pvar);
			count--;
		}
	}
	return last;
}

inline
TedMan* TedMan::bblDownVariable(const string & pVar, TedOrderStrategy* order, unsigned int count) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	TedMan* last = NULL;
	if(owner) {
		while(count!=0) {
			last = order->bblDown(*owner);
			count--;
		}
	} else {
		while(count!=0) {
			last = order->bblDown(*pvar);
			count--;
		}
	}
	return last;
}

inline
void TedMan::retime_down_forced(const string& v) {
	TedVar* pvar = checkVariables(v);
	if (pvar) {
		TedRetiming retime(this);
		retime.downMaxR(*pvar);
	}

}

inline
void TedMan::retime(bool forward,TedRegister uptoR) {
	TedRetiming retime(this);
	(forward) ? retime.up(uptoR): retime.down(uptoR);
}

inline
void TedMan::retime(const string& pVar, bool allRelatedVars, bool forward, TedRegister uptoR) {
	TedVar* pvar  = checkVariables(pVar);
	if(pvar) {
		TedRetiming retime(this);
		if(allRelatedVars) {
			(forward) ? retime.upAllRelatedTo(*pvar,uptoR): retime.downAllRelatedTo(*pvar,uptoR);
		} else {
			(forward) ? retime.up(*pvar,uptoR): retime.down(*pvar,uptoR);
		}
	}
}

inline
void TedMan::clearVar(void) {
	_vars.removeAll();
	_vars.clear();
}

/** @brief sift one variable*/
inline
TedMan* TedMan::siftVar(const string & pVar, TedOrderStrategy* order, TedCompareCost* less) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	if(owner) {
		return order->sift(*owner,less);
	}
	return order->sift(*pvar,less);
}

inline
TedMan* TedMan::relocateVariable(const string & vname, unsigned int loc) {
	TedOrderStrategy* order = new TedOrderReloc(this);
	return order->toLocation(TedVar(vname),loc);
}

inline
TedMan* TedMan::flipVar(const string & pVar, TedOrderStrategy* order) {
	TedVar* pvar = checkVariables(pVar);
	TedVarGroupOwner* owner = dynamic_cast<TedVarGroupOwner*>(pvar);
	if(!owner) {
		throw(string("04035. variable \"")+ string(pVar)+ string("\" does not belong to a group"));
	}
	return order->flip(*owner);
}

inline
TedMan* TedMan::siftAll(TedOrderStrategy* order, TedCompareCost* less) {
	return order->siftAll(less);
}

inline
TedMan* TedMan::siftGroupedAll(TedOrderStrategy* order, TedCompareCost* less) {
	return order->siftGroupedAll(less);
}

inline
TedMan* TedMan::permuteOrdering(TedOrderStrategy* order, TedCompareCost* less, bool break_into_strides) {
	return order->permute(less,break_into_strides);
}

inline
TedMan* TedMan::annealing(TedOrderStrategy* order, TedCompareCost* less, bool break_into_strides, bool stride_backtrack) {
	return order->annealing(less,break_into_strides,stride_backtrack);
}

inline
bool TedMan::isEmpty(void)const {
	return _container.isEmpty();
}

inline
void TedMan::decomposeST(bool force) {
	TedDecompose decompose(this, force);
	decompose.sumTerm();
}

inline
void TedMan::decomposePT(bool force) {
	TedDecompose decompose(this, force);
	decompose.productTerm();
}

inline
bool TedMan::decompose(bool force) {
	TedDecompose decompose(this,force);
	return decompose.allTermsWithoutReorder();
}

