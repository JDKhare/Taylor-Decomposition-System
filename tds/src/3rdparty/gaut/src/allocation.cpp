/*------------------------------------------------------------------------------
Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

Cette licence est un accord légal conclu entre vous et l'Université de
Bretagne Sud (UBS) concernant l'outil GAUT2.

Cette licence est une licence CeCILL-B dont deux exemplaires (en langue
française et anglaise) sont joints aux codes sources. Plus d'informations
concernant cette licence sont accessibles à http://www.cecill.info.

AUCUNE GARANTIE n'est associée à cette license !

L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
de GAUT2 et le distribue en l'état à des fins de coopération scientifique
seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
de protection de leurs données qu'il jugeront nécessaires.
------------------------------------------------------------------------------
2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

This license is a legal agreement between you and the University of
South Britany (UBS) regarding the GAUT2 software tool.

This license is a CeCILL-B license. Two exemplaries (one in French and one in
English) are provided with the source codes. More informations about this
license are available at http://www.cecill.info.

NO WARRANTY is provided with this license.

The University of South Britany does not provide any warranty about
the usage of GAUT2 and distributes it "as is" for scientific cooperation
purpose only. All users are greatly advised to provide all the necessary
protection issues to protect their data.
------------------------------------------------------------------------------
*/

//	File:		allocation.cpp
//	Purpose:	GAUT optimisation of selection process with multi-function operators
//	Author:		Pierre Bomel, LESTER, UBS

#include "selection.h"
#include "allocation.h"
#include <iostream>
using namespace std;

//! just to indent debug trace
static void b(int l)
{
	for (int i = 0; i < l; i++) cout << " ";
}

// Following is is a very nice recursive specification of the enumeration.
void Allocation::tryop(int l, AllocationOutStage &in, const Function *function, const Operator *op, AllocFunction *mf)
{
	if (_debug)
	{
		b(l);
		cout << "Trying operator " << op->name() << endl;
	}
	const SelectionOut *mdo;
	_sos->searchFunction(function, &mdo);			// get multi-delay selection for the function
	if (!mdo->oper()->compatible(op, function, _clk))
	{
		if (_debug) cout << "not compatible" << endl;
		return;// process only operators compatible with the selected one
	}
	long size = in.size();
	mf->oper(op);
	double rem = mf->rem;							// parallelism remainder of function
	// add 100% operators
	if (rem >= 1.0)
	{
		long ops = (long) floor(rem);				// number of mono-function operators
		for (long i = 0; i < ops; i++)
		{
			in.operators().push_back(op);			// add an operator
			in.use_rates().push_back(1.0);			// init operator use to 100%
			if (_debug)
			{
				b(l);
				cout << "adding 100% operator " << op->name() << " instance " << (in.operators().size()-1) << endl;
			}
		}
		mf->rem -= (double) ops;
	}
	// add other operators
	else
	{
		if (op->isMonoFunction())
		{
			in.operators().push_back(op);			// add an operator
			in.use_rates().push_back(rem);			// init operator use to 100%
			mf->rem = 0.0;							// reduce function parallelism remainder
			if (_debug)
			{
				b(l);
				cout << "adding 100% operator " << op->name() << " instance " << (in.operators().size()-1) << endl;
			}
		}
		else
		{
			in.operators().push_back(op);				// add an operator
			in.use_rates().push_back(0.0);				// init operator use to 0%
			if (_debug)
			{
				b(l);
				cout << "adding 0% operator " << op->name() << " instance " << (in.operators().size()-1) << endl;
			}
		}
	}
	// recursive call
	functionOptimize(l+2, in, function);			// retry
	in.operators().resize(size);					// truncate
	in.use_rates().resize(size);					// truncate
	mf->rem = rem;									// restore
}

//! optimize a function, for a given stage, 1rst strategy
void Allocation::functionOptimize(int l, AllocationOutStage &in, const Function *function)
{
	const Operator *op;
	//bool passThrough = function->passTrough();

	// get function info from partial solution
	AllocFunction *mf;
	in.functions().search(function->name(), &mf);	// search
	double f_rem = mf->rem;							// get remainder
	if (f_rem == 0.0)  								// end of function optimization
	{
		if (_debug)
		{
			b(l);
			cout << "end of function optimization" << endl;
		}
		mf->processed = true;
		stageOptimize(l+2, in);
		mf->processed = false;						// restore
		return;
	}


	bool reused = false;							// mark "no operator reused"
	long size = in.operators().size();
	if (_debug)
	{
		b(l);
		cout << "function " << function->name() << " rem=" << f_rem << endl;
	}

	// 1 = allocate mono-function operators first
	if (f_rem >= 1.0)
	{
		const SelectionOut *mdo;
		_sos->searchFunction(function, &mdo);
		op = mdo->oper();
		if (op->isMonoFunction())
		{
			tryop(l, in, function, mdo->oper(), mf);
			return;
		}
	}

	// 2 = try to reuse an allocated operator
	else for (int o = 0; o < size; o++) // scan all operators allocated up to now
		{
			op = in.operators()[o];						// get current operator instance
			if (_debug)
			{
				b(l);
				cout << "scanning operator " << op->name() << endl;
			}
			if (!op->implements(function)) continue;	// process only operators realizing "function"
			double op_use = in.use_rates()[o];			// get operator use
			if (_debug)
			{
				b(l);
				cout << "operator " << op->name() << " use=" << op_use << endl;
			}
			if (op_use == 1.0) continue;				// operator instance already 100 % used
			double op_rem = 1.0 - op_use;				// compute operator parallelism rate remainder
			if (_debug)
			{
				b(l);
				cout << "operator " << op->name() << " rem=" << op_rem << endl;
			}
			double save = in.use_rates()[o];
			if (f_rem <= op_rem)  						// all the function is "stored" in the operator
			{
				mf->rem = 0.0;							// remove all function remainder
				in.use_rates()[o] += f_rem;				// increase operator use only when not "pass-through"
			}
			else  									// only a part of the function is "stored" in the operator
			{
				mf->rem -= op_rem;						// remove operator remainder from function remainder
				in.use_rates()[o] = 1.0;				// increase operator use to 100% only when not "pass-through"
			}
			if (_debug)
			{
				b(l);
				cout << "increasing use of operator " << op->name() << "/" << o
				<< " from " << op_use << " to " << in.use_rates()[o] << endl;
				b(l);
				cout << "decreasing rem of function " << function->name()
				<< " from " << f_rem << " to " << mf->rem << endl;
			}
			functionOptimize(l+2, in, function);		// retry
			in.operators().resize(size);							// truncate
			in.use_rates().resize(size);							// truncate
			mf->rem = f_rem;							// restore
			in.use_rates()[o] = save;					// restore
			reused = true;								// mark it "reused"
		}

	if (reused) return;

	// 3 = other possibility with all multi-function compatible operators
	OperatorRefs::const_iterator o_it;
	for (o_it = function->implemented_by().begin(); o_it != function->implemented_by().end(); o_it++)
	{
		op = (*o_it).second;
		if (op->isMultiFunction()) tryop(l, in, function, op, mf);
	}

	// 4 = other possibility with all mono-function operators
	for (o_it = function->implemented_by().begin(); o_it != function->implemented_by().end(); o_it++)
	{
		op = (*o_it).second;
		if (op->isMonoFunction()) tryop(l, in, function, op, mf);
	}

}

//! optimize a pipeline stage
void Allocation::stageOptimize(int l, AllocationOutStage &in)
{
	if (_debug)
	{
		b(l);
		cout << "stage=" << _stage << endl;
	}
	// if this a leaf of the search tree: check if it is the best before stopping
	if (!in.parallelismRateRemainder())  	// check if there is still something to optimize in the stage
	{
		if (in.cost() < _best->at(_stage).cost())
		{
			if (_debug)
			{
				b(l);
				cout << "No more parallelism: new best found" << endl;
				in.info();
			}
			_best->at(_stage) = in;					// remember this best (up to now) solution
		}
		return;
	}

	// ELSE explore each function with a non-null remainder and not already processed
	//else for(AllocFunctionRefs::iterator it = in.functions().begin(); it != in.functions().end(); it++){
	for (AllocFunctionRefs::iterator it = in.functions().begin(); it != in.functions().end(); it++)
	{
		AllocFunction *mf = (*it).second;			// get current function
		if (mf->processed) continue;				// avoid processed functions
		if (_debug)
		{
			b(l);
			cout << "Still parallelism: exploring function " << mf->function->name() << endl;
		}
		functionOptimize(l+2, in, mf->function);			// explore function
		break;
	}
}

//! print summary (EJ 09/2007)
void Allocation::summary(AllocationOut *alloc)
{
	vector<Operator *>	operators;
	vector<int>	operatorStage;
	vector<int>	operatorNumber;

	for (_stage = 0; _stage < _stages; _stage++)
	{
		AllocationOutStage *mf = &(alloc->at(_stage));
		long nops = mf->operators().size();
		for (int i = 0; i < nops; i++)
		{
			Operator *o = (Operator *) mf->operators()[i];
			bool new_op = true;
			for (int j = 0; j < operators.size(); j++)
			{
				Operator *op = operators[j];
				if (op->name() == o->name() && operatorStage[j] == _stage)
				{
					operatorNumber[j]++;
					new_op = false;
					break;
				}
			}
			if (new_op == true)
			{
				operators.push_back(o);
				operatorStage.push_back(_stage);
				operatorNumber.push_back(1);
			}
		}
	}
	cout << "\n----------------------------------------------------------------------------------------------" << endl;
	cout << "  " << "Operator" << " \t;  " << "Number" << " \t;  " << "Stage " << " \t;  " << "Allocation " << endl;
	cout << "----------------------------------------------------------------------------------------------" << endl;
	int n;
	for (n = 0; n < operators.size(); n++)
	{
		cout << "  " << operators[n]->name() << " \t;  " << n << " \t;  " << operatorStage[n] << " \t;  " << operatorNumber[n] << endl;
	}
	cout << "----------------------------------------------------------------------------------------------" << endl;
	cout << endl;

	operators.clear();
	operatorStage.clear();
	operatorNumber.clear();
}

//! manual allocation (EJ 26/06/2007)
void Allocation::manualAllocation(AllocationOut *alloc, const Switches *sw)
{
	vector<Operator *>	operators;
	vector<int>	operatorStage;
	vector<int>	operatorNumber;
	char chaine[100]; // TODO : pas de tableau statique dans gaut
	int valeur;

	cout << " " << endl << "______________ MANUAL ALLOCATION _____________" << endl << endl;

	for (_stage = 0; _stage < _stages; _stage++)
	{
		AllocationOutStage *mf = &(alloc->at(_stage));
		long nops = mf->operators().size();
		for (int i = 0; i < nops; i++)
		{
			Operator *o = (Operator *) mf->operators()[i];
			bool new_op = true;
			for (int j = 0; j < operators.size(); j++)
			{
				Operator *op = operators[j];
				if (op->name() == o->name() && operatorStage[j] == _stage)
				{
					operatorNumber[j]++;
					new_op = false;
					break;
				}
			}
			if (new_op == true)
			{
				operators.push_back(o);
				operatorStage.push_back(_stage);
				operatorNumber.push_back(1);
			}
		}
		mf->operators().clear();
	}

	// view allocation before change
	cout << "  " << "Operator" << " \t|  " << "Number" << " \t|  " << "Stage " << " \t|  " << "Allocation " << endl;
	cout << "_______________________________________________" << endl;
	int n;
	for (n = 0; n < operators.size(); n++)
	{
		cout << "  " << operators[n]->name() << " \t|  " << n << " \t|  " << operatorStage[n] << " \t|  " << operatorNumber[n] << endl;
	}
	cout << "_______________________________________________" << endl;
	cout << endl;

	// useIHM == false (prompt interface)
	if (!sw->use_IHM())
	{
		for (n = 0; n < operators.size(); n++)
		{
			cout << "\n\tEnter new allocation of " << operators[n]->name() << " in stage " << operatorStage[n] <<" : [%d] ? ";
			cout.flush();
			gets(chaine);
			if (strcmp(chaine,"")!=0)
			{
				sscanf(chaine,"%d",&valeur);
				operatorNumber[n] = valeur;
			}
		}
		// useIHM == true (java interface)
	}
	else
	{
		ofstream fo((Tools::prefix(sw->cdfg_file_name())+".tmp").c_str(), ios::out);
		fo << "TABLE " << operators.size() << " 4" << endl;
		for (n = 0; n < operators.size(); n++)
		{
			Operator *o = operators[n];
			fo << o->name() << " " << n+1 << " " << operatorStage[n] << " " << operatorNumber[n] << endl;
		}
		fo.close();
		cout << "REQ_ALLOC_FILE\n";						// send req to IHM java
		cout.flush();
		strcpy(chaine, "");
		while ( strcmp(chaine, "ACK_ALLOC_FILE") !=0 )  	// wait ack from IHM java
		{
			gets(chaine);
		}
		FILE *fi = fopen((Tools::prefix(sw->cdfg_file_name())+".tmp").c_str(), "r");
		fscanf(fi, "%s", chaine);
		fscanf(fi, "%s", chaine);
		fscanf(fi, "%s", chaine);
		for (n = 0; n < operators.size(); n++)
		{
			fscanf(fi, "%s", chaine);
			fscanf(fi, "%s", chaine);
			fscanf(fi, "%s", chaine);
			fscanf(fi, "%s", chaine);
			operatorNumber[n] = atoi(chaine);
		}
		fclose(fi);
	}

	//update allocationOut
	for (n = 0; n < operators.size(); n++)
	{
		Operator *o = operators[n];
		AllocationOutStage *mf = &(alloc->at(operatorStage[n]));
		for (int i=0; i<operatorNumber[n]; i++)
		{
			mf->operators().push_back(o);
		}
	}

	// view allocation after change
	cout << "  " << "Operator" << " \t|  " << "Number" << " \t|  " << "Stage " << " \t|  " << "Allocation " << endl;
	cout << "_______________________________________________" << endl;
	for (n = 0; n < operators.size(); n++)
	{
		cout << "  " << operators[n]->name() << " \t|  " << n << " \t|  " << operatorStage[n] << " \t|  " << operatorNumber[n] << endl;
	}
	cout << "_______________________________________________" << endl;
	cout << endl;

	operators.clear();
	operatorStage.clear();
	operatorNumber.clear();
}

//! optimize operators not used at 100% with multi-function ones
Allocation::Allocation(
    const Switches *sw,							// in
    const Clock *sysclk,						// in
    const Lib *lib,								// in
    const CDFG *cdfg,							// in
    const SelectionOutRefs *sos,				// in
    AllocationOut *alloc					// out
)
{
	// init search inputs
	_sos = sos;								// keep address of best mapping (mono-functions)
	_stages = _sos->pipelineStages();		// get # of pipeline stages
	_best = alloc;
	_best->resize(_stages);					// set MFsolution vector size
	_clk = sysclk;							// keep address of system clock
	// optimize separately each pipeline stage
	for (_stage = 0; _stage < _stages; _stage++)
	{
		AllocationOutStage first;
		for (SelectionOutRefs::const_iterator m_it = _sos->begin(); m_it != _sos->end(); m_it++) // scan all mappings
		{
			const SelectionOut *m = (*m_it).second;	// get current mapping
			//const Function *f = m->func();
			double rem = m->parallelismRate()[_stage];
			AllocFunction *mf = new AllocFunction(m->func(), rem);
			first.functions().add(AllocFunctionRef(mf));
		}
		stageOptimize(0, first);
	}
	// clear working space before returning best value
	for (_stage = 0; _stage < _stages; _stage++) _best->at(_stage).functions().clear();


	// EJ 06/2007 manual allocation
	if (sw->manual_allocation())
		manualAllocation(alloc, sw);
	// EJ 09/2007 print summary
	else if (!sw->silent_mode()) summary(alloc);
}

// end of: allocation.cpp


