/*------------------------------------------------------------------------------
  Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

  Cette licence est un accord légal conclu entre vous et l'Université de
  Bretagne Sud(UBS)concernant l'outil GAUT2.

  Cette licence est une licence CeCILL-B dont deux exemplaires(en langue
  française et anglaise)sont joints aux codes sources. Plus d'informations
  concernant cette licence sont accessibles à http://www.cecill.info.

  AUCUNE GARANTIE n'est associée à cette license !

  L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
  de GAUT2 et le distribue en l'état à des fins de coopération scientifique
  seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
  de protection de leurs données qu'il jugeront nécessaires.
  ------------------------------------------------------------------------------
  2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

  This license is a legal agreement between you and the University of"
  South Britany(UBS)regarding the GAUT2 software tool.

  This license is a CeCILL-B license. Two exemplaries(one in French and one in
  English)are provided with the source codes. More informations about this
  license are available at http://www.cecill.info.

  NO WARRANTY is provided with this license.

  The University of South Britany does not provide any warranty about
  the usage of GAUT2 and distributes it "as is" for scientific cooperation
  purpose only. All users are greatly advised to provide all the necessary
  protection issues to protect their data.
  ------------------------------------------------------------------------------
  */

//	File:		scheduling.cpp
//	Purpose:	Scheduling process
//	Author:		Pierre Bomel, LESTER, UBS

#include <vector>
using namespace std;
#include "scheduling.h"
#include "operation.h"
#include "selection.h"
#include "allocation.h"
#include "cdfg.h"
#include "check.h"
#include "io_constraints.h"
/* GL 11/09/07 : bipartite graph*/
#include "b_graph.h"
#include "reg.h"
/* Fin GL*/


void OperatorInstance::UpdateConnection(const Operation*o)
{
	vector<const Operation*> o_predecessors;
	get_operations_predecessors((CDFGnode*)o, &o_predecessors);
	if(Scheduling::_debug)cout << "Scheduling::Update connection " << no()<< " After " << o->name()<< " Binding" << endl;
	for(int i = 0; i<o_predecessors.size(); i++)
	{
		Operation*n = (Operation*)o_predecessors.at(i);
		Operation*op = (Operation*)n;
		OperationSchedulingState*s = (OperationSchedulingState*)op->_addr;// get operation's state
		if(Scheduling::_debug)cout << "Scheduling::Update connection " << no()<< "_" << s->instance->no()<< endl;
		NewConnection(s->instance);
	}
}

//! count number of register connected to an input of operator instance execpt the register
long OperatorInstance::numberOfInputMuxToInputOperatorInstanceExceptThisRegister(const Reg*reg,long input_pos)const
{
	long result=0;

	for(InputOperatorRegUseRefs::const_iterator it = _inputUseRegs.begin(); it != _inputUseRegs.end(); it++)
	{
		const InputOperatorRegUse*use = (*it).second;
		if((use->_inputpos==input_pos)&&(use->_reg!=reg))
		{
			result++;
		}
	}
	return result;
}


long OperatorInstance::numberOfInputMuxToAllInputsOperatorInstance()const
{
	long result=0;

	long nb_inputs=oper()->inputPorts()+1;

	int*_inputs=new int[nb_inputs];

	int i;

	//init number of connection on inputs
	for(i =0; i<nb_inputs;i++)_inputs[i]=0;

	for(InputOperatorRegUseRefs::const_iterator it = _inputUseRegs.begin(); it != _inputUseRegs.end(); it++)
	{
		const InputOperatorRegUse*use = (*it).second;
		_inputs[use->_inputpos]++;
	}
	//count number of mux on inputs
	for(i =0; i<nb_inputs;i++)
	{
		if(_inputs[i]>1)result+= (_inputs[i]-1)*inputsBitwidth(i);
	}
	delete [] _inputs;

	return result;
}

void OperatorInstance::add_inputUseReg(InputOperatorRegUse*use)
{
	InputOperatorRegUse*find_use;

	if(_inputUseRegs.search(use->name(),&find_use) ==false)
		_inputUseRegs.add(InputOperatorRegUseRef(use));
}

// Print info.
void FunctionSchedulingState::info()const
{
	cout << "function " << _func->name()<< endl;
	OPERATIONS::const_iterator it;
	const Operation*o;
	cout << "X dump" << endl;
	for(it = EnsX.begin(); it != EnsX.end(); it++)
	{
		o =*it;
		cout << o->name()<< endl;
	}
	cout << "C dump" << endl;
	for(it = EnsC.begin(); it != EnsC.end(); it++)
	{
		o =*it;
		cout << o->name()<< endl;
	}
	cout << "CC dump" << endl;
	for(it = EnsCC.begin(); it != EnsCC.end(); it++)
	{
		o =*it;
		cout << o->name()<< endl;
	}
}

//! tell if there are still operations to schedule now or later.
bool FunctionSchedulingState::operationsToSchedule()const
{
	if(Scheduling::_debug)
	{
		cout << "function " << _func->name()
			<< " X=" << EnsX.size()
			<< " C=" << EnsC.size()
			<< " CC=" << EnsCC.size()
			<< endl;
		info();

	}
	return EnsX.size()|| EnsC.size()|| EnsCC.size();
}

//! Free all instances of an operator in a stage.
//! @param Input. s is the pipeline stage number.
//! @param Input. o is a pointer to the operator to free.
/* GL 26/04/2007 :	Replace fllowing line
   operator is no more constant
   void Scheduling::freeOperatorInstances(long s, const Operator*o, AllocationOut*allocs) {
   By*/
void Scheduling::freeOperatorInstances(long s, Operator*o, AllocationOut*allocs)
{
	int i;
	/* Fin GL*/
	OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
	if(!o_ss)
	{
		cerr << "Internal error: operator " << o->name()<< " has no scheduling state" << endl;
		exit(1);
	}
	if(!o_ss->freed)
	{
		if(_debug)cout << "Scheduling::freeOperatorInstances  " << o_ss->E.size()<< " instances of operator " << o->name()<< endl;

		// SCHEDULING OUTPUT IS BUILD HERE !!!!
		// BECAUSE THIS IS THE ONLY PLACE WHERE WE KNOW WHAT INSTANCES HAVE BEEN REALLY USED
		long size = _sos->stages.size();
		long places = s + 1;
		for(i = size; i < places; i++)_sos->stages.push_back(SchedulingStage());
		if(size < places)
		{
			_sos->stages.resize(places);
		}
		// store a copy for scheduling results
		size = o_ss->E.size();
		/* Caaliph 27/06/2007*/
		if(!_sw->cdfgs()&& !_sw->n_ordo())//Caaliph: The schedulingOut is filled  after the binding process for the MM case
		{
			for(i = 0; i < size; i++)// only for used instances
			{
				_sos->stages[s].instances.push_back(o_ss->E[i]);
			}
		}
		/* End caaliph*/

		/* KT 06/06/2007 correction bug*/
		// update allocation for next stage
		if(o_ss->D.size()!= 0)
		{
			if(_sw->verbose())cout << "there remain "<<o_ss->D.size()<<" instances in D list of the "<< o->name()<<" : stage "<< s << endl;
			if(s+1 < _stages)
			{
				AllocationOutStage*next_stage = &(_allocs->at(s+1));		// get next instance of AllocationOutStage
				//long n = next_stage->_operators.size();
				for(int k = 0; k < o_ss->D.size(); k++)//add remained instances of the current stage on the next stage
				{
					_sos->stages[s].remined_instances.push_back(o);
					next_stage->operators().push_back(o);
					next_stage->use_rates().push_back(1);
				}
			}
			else
			{
				// a pipeline stage will be added
				for(int k = 0; k < o_ss->D.size(); k++)
				{
					_sos->stages[s].remined_instances.push_back(o);
				}
			}
		}

		/* End KT*/
		// delete unused stage instances
		o_ss->D.clear();
		o_ss->E.clear();
		o_ss->freed = true;
	}
}

/* Caaliph 27/06/2007*/
//! Free all instances of an operator in a stage.
//! @param Input. s is the pipeline stage number.
//! @param Input. o is a pointer to the operator to free.
void Scheduling::freeOperatorInstances(long s, const Operator*o)
{
	int i;
	OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
	if(!o_ss)
	{
		cerr << "Internal error: operator " << o->name()<< " has no scheduling state" << endl;
		exit(1);
	}
	if(!o_ss->freed)
	{
		if(_debug)cout << "Scheduling::freeOperatorInstances  " << o_ss->E.size()<< " instances of operator " << o->name()<< endl;

		// SCHEDULING OUTPUT IS BUILD HERE !!!!
		// BECAUSE THIS IS THE ONLY PLACE WHERE WE KNOW WHAT INSTANCES HAVE BEEN REALLY USED
		long size = _sos->stages.size();
		long places = s + 1;
		for(i = size; i < places; i++)_sos->stages.push_back(SchedulingStage());

		if(size < places)
		{
			_sos->stages.resize(places);
		}
		// store a copy for scheduling results
		size = o_ss->E.size();
		/* KT 17/09/2007*/
		for(i = 0; i < size; i++)// only for used instances
		{
			//////cout <<"used operateurs "<<o->name()<<endl;
			_sos->stages[s].instances.push_back(o_ss->E[i]);
		}
		/* End KT*/
		// delete unused stage instances
		o_ss->D.clear();
		o_ss->E.clear();
		o_ss->freed = true;
	}
}
/* End Caaliph*/

//! Free hardware(all operators)previously allocated for a stage.
//! @param Input. s is the pipeline stage number.
void Scheduling::freeStage(long s, AllocationOut*allocs)
{
	if(_debug)cout << "Scheduling::freeStage Free stage " << s << endl;


	//DH+CA : 8/09/2009*/
	for(int i = 0; i < _newAllocateOperator.size(); i++)
		freeOperatorInstances(s, _newAllocateOperator[i], _allocs);
	_newAllocateOperator.clear();
	// FIN DH+CA : 8/09/2009*/

	// The following code is intended for pipeline stages added.
	// The multi-delay and multi-function processes did not plan these stages.
	// So we have to allocate hardware by another mean.
	//if((s >= _stages)|| _sw->dynamicAllocation())// NEW: Pierre Bomel, July 2006
	//DH : add dynamic allocation by default in lower_bound mode
	if((s >= _stages)||((s >=1)&& _sw->dynamicAllocation()))// NEW: Pierre Bomel, July 2006

	{
		// we can dynamicaly add stages
		/* 15/07/2007 KT*/
		if(_sw->lower_bound())
		{
			for(int i = 0; i < _dynamicStages[s].size(); i++)
				freeOperatorInstances(s, _dynamicStages[s][i], _allocs);// avant correction du bug

		}
		else
			for(int i = 0; i < _allocs->at(s).operators().size(); i++)
				freeOperatorInstances(s,(Operator*)_allocs->at(s).operators()[i],_allocs);
		/* End KT*/
	}
	else   // use multi-function selection results
	{
		// for each selected operators : free its instances
		for(int i = 0; i < _allocs->at(s).operators().size(); i++)
			/* GL 26/04/2007 :	Replace fllowing line operator is no more constant freeOperatorInstances(s, _allocs->at(s).operators()[i],_allocs);
			   By*/
			freeOperatorInstances(s,(Operator*)_allocs->at(s).operators()[i],_allocs);
		/* Fin GL*/
	}

}
/*********** Caaliph: Same as freeStage 27/06/2007*****************/
void Scheduling::freeCycle(long c, long s)
{
	if(_debug)cout << "Scheduling::freeStage Free stage " << s << endl;
	// The following code is intended for pipeline stages added.
	// The multi-delay and multi-function processes did not plan these stages.
	// So we have to allocate hardware by another mean.
	/* 15/07/2007 KT   Replace
	   if((s >= _stages)|| _sw->dynamicAllocation()) { // NEW: Pierre Bomel, July 2006
	   by*/
	/****	if(((s >= _stages)|| _sw->dynamicAllocation())&& !_sw->withreuse()) {
	/* End KT*/
	// dynamicaly added stages
	/******		for(int i = 0; i < _dynamicStages[s].size(); i++)
	  freeOperatorInstances(s, _dynamicStages[s][i]);
	  } else { // use multi-function selection results*****/
	// for each selected operators : free its instances
	AllocationOutCycle*mc = &_allocs_bis->allocationOutCycle()[c];
	long nb_oper=mc->operators().size();
	const Operator*oper;
	for(int i = 0; i < mc->operators().size(); i++)
	{
		oper=mc->operators()[i];
		freeOperatorInstances(s, mc->operators()[i]);
	}
	///////	}
}
/******************** End Caaliph***********************/

//! Detect unscheduled operations in CDFG.
//! This is a code called when walking through the CDSFG with scan_nodes.
//! It is executed for each node.
//! @param Input. n is a pointer to the current node to check.
//! @result is stored in the global variable _not_scheduled.
vector<const Operation*> _not_scheduled;	// to detect unscheduled operations when creating dynamicaly pipeline stages
static void find_not_scheduled(const CDFGnode*n)
{
	if(n->type()!= CDFGnode::OPERATION)return;			// search operations only
	const Operation*o = (const Operation*)n;	// dynamic link
	const OperationSchedulingState* o_ss = (OperationSchedulingState*)o->_addr;	// get scheduling state
	if(o_ss->start()!= -1)return;				// search for not scheduled operations only
	_not_scheduled.push_back(o);					// store it !
}
/************** Caaliph: Same as newStage 27/06/2007*****************/
void Scheduling::newCycle(long c, long s)
{
	long i,j;
	if(_debug)cout << "Scheduling::newCycle New Cycle " << s << endl;
	vector<const Operator*> oper;
	long nops = _allocs_bis->allocationOutCycle().at(c).operators().size();
	for(j=0; j<nops; j++)
	{
		oper.push_back(_allocs_bis->allocationOutCycle().at(c).operators()[j]);
	}
	long nops_copy = _allocs_bis->allocationOutCycle().at(c).copy_operators().size();
	for(j=0; j<nops_copy; j++)
	{
		vector<const Operator*>::iterator it;
		for(it=oper.begin(); it!=oper.end(); it++)
		{
			if(*it==_allocs_bis->allocationOutCycle().at(c).copy_operators()[j])
			{
				oper.erase(it);
				it=oper.end()-1;
			}
		}
	}
	nops = oper.size();
	for(i = 0; i < nops; i++)
	{
		// get operator identity for current stage
		Operator*o = (Operator*)oper[i];
		// add an instance into D
		OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
		// create new instance
		/* KT 15/07/2007 Replace line
		   OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk);
		   by*/
		OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk);
		o_ss->D.push_back(oi);		// add to tail
		o_ss->freed = false;
		if(_debug)cout << "Scheduling::newStage added an instance of operator " << o->name()<< " in D" << endl;
	}
	long mcad=_max_cad/_clk->period();
	if(mcad < c)
	{
		vector<const Operator*> oper;
		long nops = _allocs_bis->allocationOutCycle().at(c-mcad).operators().size();
		for(j=0; j<nops; j++)
		{
			oper.push_back(_allocs_bis->allocationOutCycle().at(c-mcad).operators()[j]);
		}
		long nops_copy = _allocs_bis->allocationOutCycle().at(c).copy_operators().size();
		for(j=0; j<nops_copy; j++)
		{
			vector<const Operator*>::iterator it;
			for(it=oper.begin(); it!=oper.end(); it++)
			{
				if(*it==_allocs_bis->allocationOutCycle().at(c).copy_operators()[j])
				{
					oper.erase(it);
					it=oper.end()-1;
				}
			}
		}
		nops = oper.size();
		for(i = 0; i < nops; i++)
		{
			// get operator identity for current stage
			Operator*o = (Operator*)oper[i];
			// add an instance into D
			OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
			// create new instance
			/* KT 15/07/2007 Replace line
			   OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk);
			   by*/
			OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk, _sels);
			o_ss->D.push_back(oi);		// add to tail
			o_ss->freed = false;
			if(_debug)cout << "Scheduling::newStage added an instance of operator " << o->name()<< " in D" << endl;
		}
	}
}
/******************** End Caaliph***********************/

// DH : branch and bound on critical path search
void Scheduling::resetAllMaxCriticalPredecessors(void)
{
	int i;
	for(i = 0; i < _cdfg->numberOfNodes(); i++)
	{
		CDFGnode*n = _cdfg->nodes().at(i);
		n->setMaxCriticalPredecessors(-1);
	}
}
// FIN DH : branch and bound on critical path search


//! Allocate operators for a new stage when graph has been elongated.
//! The user can also force a systematic
//! "dynamic allocation" of operator with the "user switches".
//! This is an important operator-choice point.
//! @param Input. s is the pipeline stage number.
void Scheduling::newStage(long s, SchedulingOut*sos)
{

	if(_debug)cout << "Scheduling::newStage New stage " << s << endl;
	//if(_sw->verbose())cout << "******************************************************* "<< endl;
	//if(_sw->verbose())cout << "Scheduling stage " << s << endl;
	//if(_sw->verbose())cout << "******************************************************* "<< endl;


	/*DH: 10/09/2008 add always all unscheduled instances from previous stage in  already exist stage: see fft_GL
	  if((s >= 1)&&(s < _stages))
	  {
	  AllocationOutStage*next_stage = &(_allocs->at(s));
	  for(int k =0; k < _sos->stages[s-1].remined_instances.size(); k++)//add remained instances of the current stage to the next stage
	  {
	  next_stage->operators().push_back(_sos->stages[s-1].remined_instances[k]);
	  next_stage->use_rates().push_back(1);
	  }
	// for all operators instances
	long nops = _allocs->at(s).operators().size();
	//cout << " le nombre d'opérators dans allocs est " << nops <<endl;
	for(int i = 0; i < nops; i++)
	{
	Operator*o = (Operator*)_allocs->at(s).operators()[i];
	// add an instance into D
	OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
	OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk, _sels); // create new instance
	o_ss->D.push_back(oi);		// add to tail
	o_ss->freed = false;
	}
	}

	FIN DH: 10/09/2008 add always all unscheduled instances from previous stage in  already exist stage*/

	/* 15/07/2007 KT*/
	// The following code is intended for pipeline stages added
	if(_sw->upper_bound())
	{
		if(s >= _stages)
		{
			// add a stage in the allocation table
			_allocs->resize(_stages+1);
			_stages = _stages +1;
			AllocationOutStage*next_stage = &(_allocs->at(s));
			for(int k =0; k < _sos->stages[s-1].remined_instances.size(); k++)//add remained instances of the current stage to the next stage
			{
				next_stage->operators().push_back(_sos->stages[s-1].remined_instances[k]);
				next_stage->use_rates().push_back(1);
			}
		}
		// for all operators instances
		long nops = _allocs->at(s).operators().size();
		//cout << " le nombre d'opérators dans allocs est " << nops <<endl;
		for(int i = 0; i < nops; i++)
		{
			Operator*o = (Operator*)_allocs->at(s).operators()[i];
			// add an instance into D
			OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
			OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk, _sels); // create new instance
			o_ss->D.push_back(oi);		// add to tail
			o_ss->freed = false;
		}
	}

	else if(_sw->lower_bound())
	{
		/* End KT*/
		//DH : add dynamic allocation by default in lower_bound mode
		if((s >= _stages)||((s >=1)&& _sw->dynamicAllocation()))// NEW: Pierre Bomel, July 2006
		{
			// adapt _dynamicStages
			long places = s + 1;
			long size = _dynamicStages.size();
			if(size < places)
				_dynamicStages.resize(places);

			if(_debug)cout << "Scheduling::newStage new pipeline stage: dynamic allocation" << endl;
			// Our strategy is the following:
			//
			// From the remaining operations we get:
			//		the set of their functions
			//		the critical time = crit_time
			// We can compute a number of remaining stages by : nstages = ceil((max - min)/ cadency)
			// For each function we have:
			//		a number of operations to execute = noperations
			//		a selected operator(from the multi-delay) = op
			//		a total computing time : comp_time = noperations* cycles(op)* clk->period()
			//		the number of operators to allocate for a stage is : noperators = comp_time/crit_time

			// for all remaining unscheduled operations
			_cdfg->scan_nodes(find_not_scheduled);
			// build the set of functions/# operations from the operation list
			vector<const Function*> functions;						// functions identities
			vector<long> nops;										// number of operations/function
			vector<const Operation*>::const_iterator o_it;
			vector<const Function*>::const_iterator f_it;
			if(!_not_scheduled.size())return;						// end of scheduling
			// else allocate hardware
			for(o_it = _not_scheduled.begin(); o_it != _not_scheduled.end(); o_it++)
			{
				const Operation*o =*o_it;							// get current operation
				/*	 DH : take only operations those asap  are less or equal index_stage*cadency 10/09/2009
					 schedule ASAP
					 */
				if(o->asap()<(s + 1)*_cadency->length())
				{
					/*	 FIN DH : take only operations those asap  are less or equal index_stage*cadency 10/09/2009
						 schedule ASAP
						 */
					if(_debug)cout << "Scheduling::newStage operation " << o->name()<< " alap=" << o->alap();
					const Function*f = o->function();					// get function
					bool found = false;
					for(f_it = functions.begin();(f_it != functions.end())&& !found; f_it++)
					{
						const Function*ff =*f_it;
						if(found = (ff == f))break;
					}
					if(!found)
					{
						if(_debug)cout << " added";
						functions.push_back(f);							// new function
						nops.push_back(1);								// one operation
					}
					else
					{
						if(_debug)cout << " increased";
						nops[f_it - functions.begin()]++;				// one more operation
					}
					if(_debug)cout << endl;
				}
			}
			_not_scheduled.clear();									// free memory
			// now allocate operators

			// DH : branch and bound on critical path search
			resetAllMaxCriticalPredecessors();
			// FIN DH : branch and bound on critical path search
			long crit_time = critical(_cdfg->sink());				// get critical time of unscheduled operations
			if(_debug)cout << "Scheduling::newStage critical=" << crit_time << endl;
			for(f_it = functions.begin(); f_it != functions.end(); f_it++)
			{
				const Function*f =*f_it;							// get current function
				long  noperations = nops[f_it - functions.begin()];	// get number of operations
				if(_debug)cout << noperations << "Scheduling::newStage operations of type " << f->name()<< endl;
				const SelectionOut*sel;
				_sels->searchFunction(f, &sel);						// search multi-delay selection for the function
				/* GL 26/04/2007 :	Replace fllowing line
				   operator is no more constant
				   const Operator*o = sel->oper();					// get operator
				   By*/
				Operator*o = (Operator*)sel->oper();							// get operator
				/* Fin GL*/
				long  comp_time = noperations* sel->cycles()* _clk->period();
				if(_debug)cout << "Scheduling::newStage computing time=" << comp_time << endl;
				long  noperators = ceil((double)comp_time/(double)crit_time);
				if(_debug)cout << "Scheduling::newStage allocating " << noperators << " " << o->name()<< endl;

				// add it in the dynamic stage array
				_dynamicStages[s].push_back(o);

				// add in D
				OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
				for(int i = 0; i < noperators; i++)
				{
					// create new instance
					OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk, _sels);
					o_ss->D.push_back(oi);		// add to tail
					o_ss->freed = false;
					if(_debug/*_sw->verbose()&& !_sw->silent_mode()*/)
						cout << "WARNING: added an instance of operator " << o->name()/*<< " in D"*/ << endl;
				}
			}

		}
		else   // use multi-function selection results
		{
			// for all operators instances
			long nops = _allocs->at(s).operators().size();
			int i;
			for(i = 0; i < nops; i++)
			{
				// get operator identity for current stage
				/* GL 26/04/2007 :	Replace fllowing line operator is no more constant
				   const Operator*o = _allocs->at(s).operators()[i];
				   By*/
				Operator*o = (Operator*)_allocs->at(s).operators()[i];
				/* Fin GL*/
				// add an instance into D
				OperatorSchedulingState*o_ss = (OperatorSchedulingState*)o->_addr;
				// create new instance
				OperatorInstance*oi = new OperatorInstance(o, _cadency, _clk, _sels);
				o_ss->D.push_back(oi);		// add to tail
				o_ss->freed = false;
				if(_debug)cout << "Scheduling::newStage added an instance of operator " << o->name()<< " in D" << endl;
			}
		}
	}
}



//! Compute critical time of unscheduled operations for a node.
//! @param Input. n is a pointer to the CDFG node.
//! @result a number(long)of time units.
long Scheduling::critical(CDFGnode*n)const
{
	if(n->type() == CDFGnode::OPERATION)
	{
		const Operation*o = (const Operation*)n;	// dynamic link
		const OperationSchedulingState*o_ss = (OperationSchedulingState*)o->_addr;	// get scheduling state
		if(o_ss->start()!= -1)
			return 0;
		else
			return(o->length()+ maxCriticalPredecessors(n));
	}
	else
		return maxCriticalPredecessors(n);
}



//! Compute max critical time of unscheduled operations for all predecessors of a node.
//! @param Input. n is a pointer to a CDFG node.
//! @result a number(long)of time units.

// DH : branch and bound on critical path search

long Scheduling::maxCriticalPredecessors(CDFGnode*n)const
{

	long max = n->getMaxCriticalPredecessors();
	if(max==-1)
	{
		vector<long> criticals;
		int i;
		for(i = 0; i < n->predecessors.size(); i++)
		{
			const CDFGedge*e = n->predecessors[i];
			CDFGnode*n = e->source;
			criticals.push_back(critical(n));
		}
		for(i = 0; i < criticals.size(); i++)if(criticals[i] > max)max = criticals[i];
		if(max>=0)
			n->setMaxCriticalPredecessors(max);
		else n->setMaxCriticalPredecessors(0);
		criticals.clear();
	}
	return max;
}



//! Detect if a CDFG node is finished.
//! @param Input. node is a pointer to a CDFG node.
//! @result true if the node is finished, false otherwise.
bool Scheduling::finished(long l, const CDFGnode*node)const
{
	bool res = true;
	if(_debug)cout << blanks(l)<< "finished ? " << node->name()<< endl;
	vector<CDFGedge*>::const_iterator e_it;
	switch(node->type())
	{
	case CDFGnode::CONTROL:
	case CDFGnode::DATA:
		if(_debug)cout << blanks(l)<< "finished propagate to predecessors" << endl;
		// check predecessors
		for(e_it = node->predecessors.begin(); e_it != node->predecessors.end(); e_it++)
		{
			const CDFGedge*e = (*e_it);				// get current edge
			const CDFGnode*t = e->source;				// get source node
			if(!finished(l+2, t))// if at least one is not finished, I'm not
			{
				res = false;
				break;									// stop here
			}
		}
		break;
	case CDFGnode::OPERATION:
		if(_debug)cout << blanks(l)<< "finished is an operation" << endl;
		const Operation*o = (Operation*)node;		// dynamic link here
		OperationSchedulingState*o_ss = (OperationSchedulingState*)o->_addr;
		res = (o_ss->start()!= -1)&&((o_ss->start()+ o->length())<= _time);
		if(_debug)cout << blanks(l)<< "start=" << o_ss->start()<< " length=" << o->length()<< " time=" << _time << endl;
		if(_debug)cout << blanks(l)<<(res ? "YES" : "NO")<< endl;
		break;
	}
	return res;
}

//! Detect if all predecessors are finished.
//! @param Input. o is a pointer to an operation to check.
//! @result true if all the operations'predecessors are
//! finished, false otherwise.
bool Scheduling::allPredecessorsFinished(long l, const Operation*o)const
{
	if(_debug)cout << blanks(l)<< "allPredecessorsFinished ? for " << o->name()<< endl;
	vector<CDFGedge*>::const_iterator e_it;
	/* GL 20/12/08 : Complex operators, fine grain scheduling*/
	if((_sw->hSchedule() == "fine_grain")&&(o->type() ==CDFGnode::OPERATION)) {
		int begins = 0, i;
		for(e_it = o->predecessors.begin(), i=0; e_it != o->predecessors.end(); e_it++,i++)
		{
			const CDFGedge*e = (*e_it);				// get current edge
			const CDFGnode*t = e->source;				// get source node
			if(!finished(l+2, t)) {
				begins = ((const Operation*)o)->input_length(i,_clk->period());
				if(begins == 0)break; //stop here
			}
		}
		if(begins!=0)// if all non finished predecessors begin after 0 its ok
		{
			/*GL 25/05/09 : fine grain with freeze*/
			/*Fin GL*/
			lockAllPredecessors((const Operation*)o);
			if(_debug)cout << blanks(l)<< "YES" << endl;
			return true;
		}
	}
	else
		/* Fin GL*/
		for(e_it = o->predecessors.begin(); e_it != o->predecessors.end(); e_it++)
		{
			const CDFGedge*e = (*e_it);				// get current edge
			const CDFGnode*t = e->source;				// get source node
			if(!finished(l+2, t))// at least one predecessor not finished, so I'm not
			{
				if(_debug)cout << blanks(l)<< "NO" << endl;
				return false;							// propagate
			}
		}
	if(_debug)cout << blanks(l)<< "YES" << endl;
	return true;									// all scheduled !
}
/* GL : complex operators, fine grain scheduling*/
//! fine grain schedulin, lock all predecessors when the first is finished.
//! @param Input. o is a pointer to an operation to check.
void Scheduling::lockAllPredecessors(const Operation*o)const
{
	if(_debug)cout << "lockAllPredecessors for " << o->name()<< endl;
	vector<CDFGedge*>::const_iterator e_it;
	for(e_it = o->predecessors.begin(); e_it != o->predecessors.end(); e_it++)
	{
		const CDFGedge*e = (*e_it);				// get current edge
		const CDFGnode*t = e->source;				// get source node
		if(_debug)cout << "lockAllPredecessors " << t->name()<< ", alap : " << t->alap()<< ", asap : " << t->asap()<< endl;
		if(t->type() == CDFGnode::DATA)((Data*)t)->alap_locked(true);		// lock data nodes
	}
}
/* Fin GL*/
//! Try to set a CDFG node "executable". The node can be set to
//! the "executable" state only if all its predecessors have been scheduled.
//! @param Input. node is a pointer to the node.
void Scheduling::setNodeExecutable(long l, const CDFGnode*node)
{
	if(_debug)cout << blanks(l)<< "setNodeExecutable " << node->name()<< endl;
	vector<CDFGedge*>::const_iterator e_it;
	switch(node->type())
	{
	case CDFGnode::CONTROL:
	case CDFGnode::DATA:
		if(_debug)cout << blanks(l)<< "setNodeExecutable is a control/data, propagate to successors" << endl;
		// propagate to successors
		setSuccessorsExecutable(l+2, node);
		break;
	case CDFGnode::OPERATION:
		if(_debug)cout << blanks(l)<< "setNodeExecutable is an operation" << endl;
		const Operation*o = (Operation*)node;		// dynamic link here
		const Function*f = o->function();				// get operation's function
		if(allPredecessorsFinished(l+2, o))
		{
			if(_debug)cout << blanks(l)<< "setNodeExecutable all predecessors finished, set executable " << node->name()<< endl;
			FunctionSchedulingState*f_ss = (FunctionSchedulingState*)f->_addr;	// get function's state
			/* GL 20/02/08*/
			if(!scheduled(node))
				/* Fin GL*/
				/* 29/10/07 : add lines*/
				if((_sw->chaining()&& !o->isChained())||(!_sw->chaining()))
					/* Fin GL*/
					add(f_ss->EnsX, o);
		}
		break;
	}
}

//! Try to set successors of a node "executable". To be used when an operation finishes.
//! @param Input. node is a pointer to a CDFG node.
void Scheduling::setSuccessorsExecutable(long l, const CDFGnode*node)
{
	if(_debug)cout << blanks(l)<< "setSuccessorsExecutable " << node->name()<< endl;
	vector<CDFGedge*>::const_iterator e_it;
	for(e_it = node->successors.begin(); e_it != node->successors.end(); e_it++)
	{
		const CDFGedge*e = (*e_it);					// get current edge
		const CDFGnode*t = e->target;					// get target node
		setNodeExecutable(l+2, t);						// propagate
	}
}

//! Try to get an operator ready for an operation.
//! This is an important operator-choice point. For each operator implementing the operation's function:
//!	-# Scan all operators in the E list. Return first ready, if any.
//!	-# Scan all operators in the D list. return first ready, if any.
//!	-# Else return NULL(no operator ready).//!
//! @param Input. o is a pointer to the operation for which we must allocate an operator.
//! @result a pointer to an operator instance or NULL.
OperatorInstance* Scheduling::getInstanceReady(const Operation*o, SchedulingOut*sos)
{
	if(_debug)cout << "getInstanceReady Searching instance for " << o->name()<< endl;
	// get operation's function
	const Function*f = o->function();
	if(_debug)cout << "getInstanceReady  function = " << f->name()<< endl;
	FunctionSchedulingState*f_ss = (FunctionSchedulingState*)f->_addr;
	if(f_ss->no_more_hardware)
	{
		if(_debug)cout << "no more hardware for function " << f->name()<< ", stop" << endl;
		return 0;
	}
	// scan all operators implementing the function
	// get operator identity for current stage
	/* GL 26/04/2007 :	Replace fllowing line operator is no more constant
	   const Operator*op;
	   By*/
	Operator*op;
	/* Fin GL*/
	for(OperatorRefs::const_iterator o_it = f_ss->allocated.begin(); o_it != f_ss->allocated.end(); o_it++)
	{
		op = (*o_it).second;							// get current operator
		if(_debug)cout << "getInstanceReady    operator = " << op->name()<< endl;
		if(!op->_addr)continue;						// not selected
		OperatorSchedulingState*s = (OperatorSchedulingState*)op->_addr;	// get scheduling state
		if(s->no_more_hardware)
		{
			if(_debug)cout << "no more hardware for operator " << op->name()<< ", stop" << endl;
			continue;									// no need to continue with this one
		}
		// search an instance in E first
		if(_debug)cout << "getInstanceReady      E=" << s->E.size()<< endl;
		if(_debug)cout << "getInstanceReady      D=" << s->D.size()<< endl;
		/* GL 26/06/07	: compatibility with option withreuse
		   Replace lines
		/* GL 01 /06/07 : To store the last ready instance
		int ready = -1;
		OperatorInstance*lastReady=0;
		/* Fin GL*/
		/* By*/
		OperatorInstance*lastReady=0;
		OperatorInstance*UsedOi = 0;
		/* Fin GL*/
		/* KT 06/06/2007 option withreuse*/
		//search a used instance on the precedent stages
		if((_sw->withreuse())&&(_stage > 0))
		{
			for(int j=0; j<_stage; j++)
			{
				for(int i= 0; i< _sos->stages[j].instances.size(); i++)
				{
					bool test = false;
					if((_sos->stages[j].instances[i]->oper()->name() == op->name())&&(_sos->stages[j].instances[i]->cpt_use < _sos->stages[j].instances[i]->cpt_max_use))/* KT 15/07/2007*/
					{
						// check if the current instance can do the operation o
						test = true;
						for(int k=0; k <o->cycles(); k++)
						{
							if(_sos->stages[j].instances[i]->_reservation_table[(_time % _cadency->length())/_clk->period()+ k ]!=0)
								test = false;
						}
					}
					if(_sw->upper_bound())
					{
						if(test)
						{
							UsedOi = _sos->stages[j].instances[i];
							if(((_time % _cadency->length())/_clk->period())%o->cycles()!= 0)
								test = false;
						}
					}
					if(test)
					{
						UsedOi = _sos->stages[j].instances[i];
						UsedOi->cpt_use++;
						/* End KT*/
						/* GL 26/06/07	: compatibility with option withreuse when binding mode is different from 0 :
						   if an already used instance fits current operation, return instance
						   if not, search in E and if no one fits return the already used one*/
						if(_sw->bindingMode() ==0)
						{
							if(_debug)cout << "getInstanceReady      OK" << endl;
							/* KT 15/07/2007 update reservation table*/
							long debut= (_time % _cadency->length())/_clk->period();
							long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
							for(int i= debut; i< fin;i++)
							{
								if(i < _cadency->length())
									UsedOi->_reservation_table[i]=1;
								else
									UsedOi->_reservation_table[i-_cadency->length()]=1;
							}
							/* End KT*/
							return UsedOi;
						}
						else
						{
							if(UsedOi->fitsOperation(o))
							{
								if(_debug)cout << "getInstanceReady      OK" << endl;
								UsedOi->updateWidth(o);
								/* KT 15/07/2007 update reservation table*/
								long debut= (_time % _cadency->length())/_clk->period();
								long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
								for(int i= debut; i< fin;i++)
								{
									if(i < _cadency->length()/_clk->period())
										UsedOi->_reservation_table[i]=1;
									else
										UsedOi->_reservation_table[i-_cadency->length()/_clk->period()]=1;
								}
								/* End KT*/
								return UsedOi;// got it
							}
						}
					}
					/* Fin GL*/
				}
			}
			/* GL 27/09/07 : add lines
			   if(UsedOi) {
			   if(_debug)cout << "getInstanceReady      UsedOi OK" << endl;
			   if(_debug)cout << "getInstanceReady	update widths " << UsedOi->no()<< " : " << UsedOi->oper()->name()<< endl;
			   updateWidth(UsedOi, o);
			   long debut= (_time % _cadency->length())/_clk->period();
			   long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
			   for(int i= debut; i< fin;i++) {
			   if(i < _cadency->length()/_clk->period())
			   UsedOi->_reservation_table[i]=1;
			   else
			   UsedOi->_reservation_table[i-_cadency->length()/_clk->period()]=1;
			   }
			   return UsedOi;
			   }

			/* Fin GL*/
		}


		/* End KT*/
		/* Caaliph 27/06/2007*/
		if(!_sw->cdfgs())/// Add by Caaliph to avoid the use of the list E(MM case)
		{
			/* End Caaliph*/
			for(int i = 0; i < s->E.size(); i++)
			{
				if(_debug)cout << "getInstanceReady      E instance " << i << endl;
				OperatorInstance*oi = s->E[i];				// get current instance

				/* KT 15/07/2007 : check if the current instance can do the operation o*/
				bool test_2 =true;
				if(_sw->upper_bound())
				{
					if(((_time % _cadency->length())/_clk->period())%o->cycles()!= 0)
						test_2 = false;
				}
				/* Replace
				   if(oi->ready()) {
				   By*/
				if(oi->ready()&& test_2  &&(oi->cpt_use < oi->cpt_max_use))
				{
					oi->cpt_use++;
					/*End KT*/

					/* GL 22 /05/07 : replace lines
					   if(_debug)cout << "getInstanceReady      OK" << endl;
					   return oi;// got it
					/* By
					bindingMode 0 : random choice
else		  : the first suitable bitwidth if any
else the last ready*/

					if(_sw->bindingMode() == 0)
					{
						if(_debug)cout << "getInstanceReady      OK" << endl;
						if(_sw->withreuse())
						{
							/* KT 15/07/2007 :update reservation table*/
							long debut= (_time % _cadency->length())/_clk->period();
							long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
							for(int i= debut; i< fin;i++)
							{
								if(i < _cadency->length()/_clk->period())
									oi->_reservation_table[i]=1;
								else
									oi->_reservation_table[i-_cadency->length()/_clk->period()]=1;
							}
							/* End KT*/
						}
						return oi;// got it
					}
					else
					{
						if(_debug)cout << "getInstanceReady      OK" << endl;
						oi->updateWidth(o);				// reorder E(the list is maintained sorted)
						remove(s->E,oi);
						insert(s->E,oi);
						if(_sw->withreuse())
						{
							/* KT 15/07/2007 :update reservation table*/
							long debut= (_time % _cadency->length())/_clk->period();
							long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
							for(int i= debut; i< fin;i++)
							{
								if(i < _cadency->length()/_clk->period())
									oi->_reservation_table[i]=1;
								else
									oi->_reservation_table[i-_cadency->length()/_clk->period()]=1;
							}
							/* End KT*/
						}
						return oi;// got it
					}
					/* Fin GL*/
				}
				if(_debug)cout << "getInstanceReady      not ready" << endl;
		}
			}

			/* GL 01 /06/07 : return the last ready
			   if(_sw->bindingMode()!= 0) {
			/* GL 26/06/07	compatibility with option withreuse
			replace line
			if(ready != -1) {
			if(_debug)cout << "getInstanceReady      LastReady OK" << endl;
			if(_debug)cout << "getInstanceReady	update widths " << lastReady->no()<< " : " << lastReady->oper()->name()<< endl;
			// resize oi => that will increses its bitwidth
			updateWidth(lastReady, o);				// reorder E(the list is maintained sorted)
			remove(s->E,lastReady);
			insert(s->E,lastReady);

			return lastReady;
			}
			/* By*/
			/* GL 27/09/07 : remove lines
			   if((_sw->withreuse())&&(UsedOi!=0)) {
			   if(_debug)cout << "getInstanceReady      UsedOi OK" << endl;
			   if(_debug)cout << "getInstanceReady	update widths " << UsedOi->no()<< " : " << UsedOi->oper()->name()<< endl;
			// resize oi => that will increses its bitwidth
			updateWidth(UsedOi, o);

			if(_sw->withreuse()) {
			/* KT 15/07/2007 :update reservation table
			long debut= (_time % _cadency->length())/_clk->period();
			long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
			for(int i= debut; i< fin;i++) {
			if(i < _cadency->length()/_clk->period())
			UsedOi->_reservation_table[i]=1;
			else
			UsedOi->_reservation_table[i-_cadency->length()/_clk->period()]=1;
			}
			}
			UsedOi->cpt_use++;
			/* End KT
			return UsedOi;
			}
			else  /*Fin GL if(lastReady!=0) {
			if(_debug)cout << "getInstanceReady      LastReady OK" << endl;
			if(_debug)cout << "getInstanceReady	update widths " << lastReady->no()<< " : " << lastReady->oper()->name()<< endl;
			// resize oi => that will increses its bitwidth
			updateWidth(lastReady, o);				// reorder E(the list is maintained sorted)
			remove(s->E,lastReady);
			insert(s->E,lastReady);
			if(_sw->withreuse()) {
			/* KT 15/07/2007 :update reservation table
			long debut= (_time % _cadency->length())/_clk->period();
			long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
			for(int i= debut; i< fin;i++) {
			if(i < _cadency->length()/_clk->period())
			lastReady->_reservation_table[i]=1;
			else
			lastReady->_reservation_table[i-_cadency->length()/_clk->period()]=1;
			}
			/* End KT
			}
			return lastReady;
			}
			if(_debug)cout << "getInstanceReady      no one ready" << endl;
			}
			/* Fin GL*/
			// else search an instance in D
			/* KT 15/07/2007 : check if the current instance can do the operation o*/
			bool test_3 =true;
			if(_sw->upper_bound())
			{
				if(((_time % _cadency->length())/_clk->period())%o->cycles()!= 0)
				{
					test_3 = false;
				}
			}
			/* Replace
			   if(!s->D.empty()
			   By*/
			if(!s->D.empty()&& test_3)
			{
				if(_debug)cout << "getInstanceReady      D instance" << endl;
				OperatorInstance*oi = s->D[0];				// get current instance
				if(_sw->withreuse())
				{
					/* update reservation table*/
					long debut= (_time % _cadency->length())/_clk->period();
					long fin = (_time % _cadency->length())/_clk->period()+ o->cycles();
					for(int i= debut; i< fin;i++)
					{
						if(i < _cadency->length()/_clk->period())
							oi->_reservation_table[i]=1;
						else
							oi->_reservation_table[i-_cadency->length()/_clk->period()]=1;
					}
				}
				/* End KT*/

				// get current instance
				s->D.erase(s->D.begin());					// remove from D
				/* GL 16/05/2007 : bindingMode 0 : normal flow
				   bindingMode 1 : operator instances are sorted from smallest to largest*/
				if(_sw->bindingMode() == 0)
					s->E.push_back(oi);		// add into E
				else
				{
					// update oi widths
					/* GL 27/09/2007 : Replace line
					   updateWidth(oi, o);
					/* By*/
					newWidth(oi, o);
					/* Fin GL*/
					insert(s->E, oi);	// insert into sorted list
				}
				/* Fin GL*/
				/* Caaliph 27/06/2007*/
				if(_sw->cdfgs())/// Add by Caaliph
				{
					//Store a copy of scheduled operators
					if((_max_cad/_clk->period())<=_alloc_cycle_count)
						_allocs_bis->allocationOutCycle()[_alloc_cycle_count].operators().push_back(oi->oper());
					_allocs_bis->allocationOutCycle()[_alloc_cycle_count].copy_operators().push_back(oi->oper());
				} /// end addition
				/* End Caaliph*/
				if(_debug)cout << "getInstanceReady      OK" << endl;
				/* KT 15/07/2007*/
				oi->cpt_use++;
				/* End KT*/
				return oi;									// got it
			}
			if(_debug)cout << "getInstanceReady      no D instance" << endl;
			if(_debug)cout << "mark: no more hardware for operator " << op->name()<< endl;
			s->no_more_hardware = true;					// mark scheduling state
		}
		if(_debug)cout << "getInstanceReady no instance ready" << endl;
		if(_debug)cout << "mark: no more hardware for function " << f->name()<< endl;

		FUNCTIONS::iterator f_it;
		struct func_sched*fs;
		f_ss->no_more_hardware = true;
		// propagate
		for(f_it = _functions.begin(); f_it != _functions.end(); f_it++)
		{
			fs = &(*f_it);
			if(fs->func == f)fs->no_more_hardware = true;
		}
		// now check global resources availability with other functions
		bool at_least_one_with_hardware = false;
		for(f_it = _functions.begin(); f_it != _functions.end(); f_it++)
		{
			fs = &(*f_it);
			if((!fs->no_more_hardware)&& fs->n)
			{
				at_least_one_with_hardware = true;
				break;
			}
		}
		if(!at_least_one_with_hardware)
		{
			if(_debug)cout << "detected no more hardware" << endl;
			_no_more_hardware = true;
		}

		// return fail state
		return 0;
	}

	//! Forced allocation of a new operator instance in E for an operation.
	//! @param Input. o is a pointer to the operation to which a new operator must be allocated.
	//! @result a pointer to an allocated operator instance.
	OperatorInstance* Scheduling::allocateInstance(const Operation*o)
	{
		if(_debug)cout << "Scheduling::allocateInstance Allocating into E a new operator for operation " << o->name()<< endl;
		const Function*f = o->function();					// get operation's function
		const SelectionOut*sel;
		_sels->searchFunction(f, &sel); // scan multi-delay selection stage output
		/* GL 26/04/2007 :	Replace fllowing line
		   operator is no more constant
		   const Operator*op = sel->oper();					// get operator
		   By*/
		Operator*op = (Operator*)sel->oper();					// get operator
		/* Fin GL*/

		// EJ 26/06/2007 backup only first allocate operator(in order to reload scheduling with it)
		if(_sw->operator_optimization())
		{
			if(_newAllocateInstance == false)
			{
				_newAllocateInstance = true;
				_newAllocateOperator.push_back(op);
			}
		}
		//DH+CA : 8/09/2009*/
		else
		{
			_newAllocateOperator.push_back(op);
		}
		//FIN DH+CA : 8/09/2009*/

		OperatorInstance*oi = new OperatorInstance(op, _cadency, _clk, _sels);	// create a new instance
		/* Caaliph 27/06/2007*/
		if(_sw->cdfgs())/// Add by Caaliph
		{
			_allocs_bis->allocationOutCycle()[_alloc_cycle_count].operators().push_back(oi->oper());
			_allocs_bis->allocationOutCycle()[_alloc_cycle_count].copy_operators().push_back(oi->oper());
		} /// end addition
		oi->bind(f);//// Added by Caaliph : allow to fill the function part of oi(avoid bug during vhdl generation)
		((Operation*)o)->inst(oi);////GL

		/* End Caaliph*/
		OperatorSchedulingState*s = (OperatorSchedulingState*)op->_addr;	// get operator state
		/* GL 16/05/2007 : bindingMode 0 : normal flow
		   bindingMode 1 : operator instances are sorted from smallest to largest*/
		if(_sw->bindingMode() == 0)
		{
			s->freed = false;
			s->E.push_back(oi);		// add into E
		}
		else
		{
			// update oi widths
			oi->updateWidth(o);
			insert(s->E, oi);	// insert into sorted list
		}
		/* Fin GL*/
		return oi;											// done
	}

	//! Get an operator instance for an operation or wait.
	//!	-# Compute the time limit = now + operation_length
	//!	-# If the time limit crosses a pipeline boundary, reject operation for later scheduling.
	//!	-# If there are memory mapping constraints:
	//!		- Check memory mapping constraints for READ at _time. If collision reject operation.
	//!		- Check memory mapping constraints for WRITE at _time + operation_length. If collision reject operation.
	//!	-# If there is an operator instance ready, OK return it now, else continue.
	//!	-# From here we must either reject the operation or force a dynamic operator allocation.
	//!	-# If there are IO constraints:
	//!		- If the operation has been too much delayed and does nor respect it's IO constraint ==> internal error.
	//!		- If this is the last clock cycle when the operation can be scheduled: force a dynamic
	//! allocation of a new operator instance.
	//!		- Else reject operation.
	//!
	//! For the special case when operation is "=", there is no allocation
	//! of hardware instance, but this is correct.
	//!
	//! @param Input. o is a pointer to the operation to schedule now.
	//! @result a pointer to an operator instance if operation has been successfuly scheduled or NULL.
	OperatorInstance* Scheduling::getInstanceOrWait(const Operation*o, SchedulingOut*sos, bool*delayed_by_memory_constraint)
	{
		long len = o->length();
		long limit = _time + len;	// end of operation if start = "now"
		if(_debug)cout << "getInstanceOrWait get instance or wait for operation " << o->name()<< " time=" << _time << " limit=" << limit
			<< " boundary=" << _stageTimeBoundary << endl;

		// check operation is schedulable in a slice
		if(len > _stageTimeBoundary)
		{
			cout << endl;
			cerr << "Error: cadency(" << _cadency->length()<< ")does not allow scheduling of '" << o->function()->name()<< "' operations" << endl;
			cerr << "because they last " << len << " time units" << endl;
			exit(1);
		}

		if(limit > _stageTimeBoundary)
		{
			if(_debug)cout << "getInstanceOrWait Operation " << o->name()<< " rejected to next stage" << endl;
			return 0;			// scheduling in this stage is impossible
		}

		// check mapping constraint
		if(!_memc.checkReads(_cdfg, o, _time))
		{
			if(_debug)cout << "getInstanceOrWait rejected for READ collision" << endl;
			*delayed_by_memory_constraint = true;  // EJ 07/12/2007
			return 0;
		}
		if(!_memc.checkWrites(_cdfg, o, _time + len))
		{
			if(_debug)cout << "getInstanceOrWait rejected for WRITE collision" << endl;
			*delayed_by_memory_constraint = true; // EJ 07/12/2007
			return 0;
		}

		// if this is the "=" operation return 0(or dynamic memory acces)
		if(o->function()->passThrough())
		{
			if(_debug)cout << "getInstanceOrWait detected a '=' or dynamic memory access" << endl; // EJ 14/02/2008 : dynamic memory
			return 0;
		}

		// Has the operation been delayed too much ?
		if(o->alap_locked()&&(o->alap()< _time))
		{
			cerr << "Error: operation " << o->name()<< " has been delayed too much" << endl;
			exit(1);
		}

		// try to find an operator ready
		OperatorInstance*oi;
		if((oi = getInstanceReady(o,_sos)))return oi;			// ok enough hardware ready

		// Has the operation been delayed too much ?
		if(o->alap_locked()&&(o->alap()< _time))
		{
			cerr << "Error: operation " << o->name()<< " has been delayed too much" << endl;
			exit(1);
		}

		// Allocate hardware :
		// -> if Constraint Alap on a node

		/* Caaliph 27/06/2007*/
		if(_sw->cdfgs())//Caaliph:Allocate an operator if necessary
		{
			if((o->alap_locked()&&(o->alap() == _time))
			   ||(o->delay()>= ((_cadency->length()/_clk->period())-1))||(!o->compatible_ops()&&(o->alap() ==_time)))
			{
				if(_sw->verbose()&& !_sw->silent_mode())cout << "WARNING: allocation forced for operation " << o->name()<< endl;
				return allocateInstance(o);	// we cannot wait
			}
			/* End Caaliph*/

		}
		else
		{
			if((o->alap_locked()&&(o->alap() == _time))
			   // Caaliph 10/09/08
			   ||((_sw->scheduling_strategy() == Switches::AS_SO_AS_PO)&&(o->asap() == _time))
			   // End Caaliph
			  )
			{
				if(_sw->verbose()&& !_sw->silent_mode())cout << "WARNING: allocation forced for operation " << o->name()<< endl;
				if(_sw->upper_bound())
				{
					// EJ 09/2007 : with upper bound, gaut
					cout << "WARNING : allocation forced for operation " << o->name()<< endl;
					cerr << "error : gaut should not allocate other operators with upper bound option : too strong constraints" << endl;
					exit(1);
				}
				return allocateInstance(o);	// we cannot wait
			}
		}

		// can the operation start and stop before the end of the pipeline stage ?
		if(_debug)cout << "getInstanceOrWait no hardware ready: delaying operation " << o->name()<< endl;
		return 0;											// we cant delay
	}

	//! For a specific operation : check for operations ending now and release operators(combinatorial or pipelined).
	//! @param Input. o is a pointer to the operation from which to extract the bound operator to release.
	bool Scheduling::freeOperators(const Operation*o)
	{
		bool done = false;
		if(_debug)cout << "freeOperators    looking at operation " << o->name()<< " at time " << _time;
		OperationSchedulingState*o_ss = (OperationSchedulingState*)o->_addr;
		/* GL 24/04/08 : pipeline operators
		   Function*f = (Function*)o->function();
		   FunctionSchedulingState*f_ss = (FunctionSchedulingState*)f->_addr;
		/* Fin GL*/
		OperatorInstance*oi = o_ss->instance;				// get assigned instance
		if(oi)
		{
			if(!oi->running())
			{
				if(_debug)cout << ", not running";
				return done;									// cannot be freed because not running
			}
			if(_debug)cout << ", running";
			/* GL 26/04/2007 :	Replace fllowing line
			   operator is no more constant
			   const Operator*op = oi->oper();
			   By*/
			Operator*op = oi->oper();
			/* Fin GL*/
			long op_end = o_ss->start()+ o->cycles()* _clk->period();// operation real end
			long release_length;
			if(op->pipeline())// pipeline operator
				release_length = op->cadency();					// after cadency op can be reused, but operation is not yet finished
			else												// non-pipeline operators
				release_length = cycles(o->function(), op, _clk);// after propagation time op can be reused, operation is finished
			release_length*= _clk->period();
			long release_end = o_ss->start()+ release_length;
			if(_debug)cout << ", start=" << o_ss->start()<< ", release-length=" << release_length << ", release_end=" << release_end;
			if(release_end <= _time)
			{
				if(_debug)cout << " ending" << endl;
				oi->isReady();									// free operator
				done = true;
				/* GL 24/04/08 : pipeline operators
				   if(op->pipeline())add(f_ss->EnsCC, o);		// add operation into EnsCC
				/* Fin GL*/
				if(op_end <= _time)// if operation is finished only
				{
					if(_debug)cout << "operation is now finished" << endl;
					setSuccessorsExecutable(0, o);
					//releaseSuccessors(o);						// try to add successor operations in the "executable" list
				}
			}
			else
			{
				if(_debug)cout << " still running" << endl;
			}
		}
		else  	// "=" operations
		{
			/* GL 20/02/08 : Correction Bug Function passthrough*/
			/*				 Replace lines
							 if(_debug)cout << " ending" << endl;
							 done = true;
							 setSuccessorsExecutable(0, o);
			/* By*/
			long op_end = o_ss->start()+ o->cycles()* _clk->period();// operation real end
			if(op_end <= _time)
			{
				if(_debug)cout << " ending" << endl;
				done = true;
				setSuccessorsExecutable(0, o);
			}
			else
			{
				if(_debug)cout << " still running" << endl;
			}
			/* Fin GL*/
		}
		return done;
	}

	//! For a specific function : check for operations ending now and release operators.
	//! @param Input. f is a pointer to the function to scan for operators to release.
	void Scheduling::freeOperators(const Function*f)
	{
		if(_debug)cout << "freeOperators  looking at function " << f->name()<< endl;
		FunctionSchedulingState*s = (FunctionSchedulingState*)f->_addr;	// get state
		vector<long> operations_to_remove;
		// scan all running operations
		int i;
		for(i = 0; i < s->EnsC.size(); i++)if(freeOperators(s->EnsC[i]))operations_to_remove.push_back(i);
		// definitely remove all these ended operations
		for(i = operations_to_remove.size()-1; i >= 0; i--)s->EnsC.erase(s->EnsC.begin()+operations_to_remove[i]);
	}

	//! Check for operations ending now and release their allocated operators.
	void Scheduling::freeOperators()
	{
		if(_debug)cout << "freeOperators Freeing operators" << endl;
		// scan all known functions in CDFG
		FunctionRefs::const_iterator i;
		for(i = _used_functions.begin(); i != _used_functions.end(); i++)freeOperators((*i).second);
	}

	//! Protect critical loop of a loopback variable.
	//! The critical loop is the time between the first read and the last write of a
	//! loopback variable. This time must be smaller than the cadency less the
	//! memory access time.
	//!	-# Get the
	void Scheduling::protectCriticalLoop(const Data*read_var)
	{
		long cadency = _cadency->length();
		long access_time = _mem->access_time();
		Data*write_var = (Data*)read_var->writevar();	// get write side of the critical loop
		long read_time = _memc.getLastReadTime(read_var);
		long write_time_limit = cadency - access_time - _clk->period();

		// recompute ALAP backward from write_variable to read variable.
		//write_var->forceAlapBackward(read_var, write_time_limit);

		if(write_var->asap()> write_time_limit)
		{
			cerr << "Internal error: Critical Loop can't to be respected(" << write_var->name()<< "=>" << read_var->name()<< ")" << endl;
			exit(1);
		}
		if(write_var->alap_locked() == true && write_var->alap()<= write_time_limit)
		{
			if(_sw->verbose()&& !_sw->silent_mode())
			{
				cout << "WARNING: READ of '" << read_var->name()<< "' at " << read_time % cadency;
				cout << " forces ALAP WRITE of '" << write_var->name()<< "' at " << write_var->alap()% cadency << endl;
			}
			return;
		}
		if(_sw->verbose()&& !_sw->silent_mode())
		{
			cout << "WARNING: READ of '" << read_var->name()<< "' at " << read_time % cadency;
			cout << " forces ALAP WRITE of '" << write_var->name()<< "' at " << write_time_limit % cadency << endl;
		}
		// recompute ALAP backward from write_variable to source.
		write_var->alap(write_time_limit);
		write_var->alap_locked(true);
		write_var->propagateAlapLock(write_time_limit);
	}

	//! Protect critical loops of an operation.
	//! If one of the data inputs of the operation is a loopback variable, protect its critical loop.
	void Scheduling::protectCriticalLoops(const Operation*o)
	{
		// scan all inputs of the operation
		for(int i = 0; i < o->predecessors.size(); i++)
		{
			const CDFGedge*e = o->predecessors[i];			// get current predecessor edge
			const CDFGnode*p = e->source;					// get source node
			if(p->type()!= CDFGnode::DATA)continue;		// process only Data nodes
			const Data*d = (const Data*)p;				// dynamic link
			if(d->type()!= Data::VARIABLE)continue;		// avoid constants, input and outputs
			if(d->writevar())
			{
				if(d->writevar()->aging() == true)
					continue;									// avoid aging variable
				if (d->aging() == true)
					continue;
				protectCriticalLoop(d);							// protect the critical loop of the data
				there_is_loopback_variable = true;				// inform that there is a loopback variable
			}
		}
	}

	/* GL 24/05/07 : Bind at the end of the cycle
	//! Bind an operation to an operator at the end of cycle.
	//!	-# Remove operation from set of scheduled operations _toBind.*/
	void Scheduling::bind()
	{
		if(_debug)cout << "Scheduling::bind  Binding operations after one cycle.... Operations to bind : " << _toBind->size()<< endl;
		/* GL 07/09/2007 : new binding mode*/
		if(_sw->bindingMode() ==4)
		{
			MWBMatching();
			return;
		}
		/* Fin GL*/
		// scan operations in order now
		if(_sw->verbose())cout << ", " << _toBind->size()<< " to bind";
		while(_toBind->size())
		{
			const Operation*o = _toBind->top();
			if(_debug)cout << "Scheduling::bind      " << _toBind->size()<< " operations To bind" << endl;
			// get operation's function
			const Function*f = o->function();
			if(_debug)cout << "Scheduling::bind  function = " << f->name()<< endl;
			OperationSchedulingState*s = (OperationSchedulingState*)o->_addr;// get operation's state
			FunctionSchedulingState*f_ss = (FunctionSchedulingState*)f->_addr;
			// get instance
			bool oiOK = false;
			if(_debug)
			{
				cout <<" Scheduling::bind	Function : ";
				f_ss->info();
				cout << " Operators : " << f_ss->assigned.size()<< endl;
			}
			for(int i = 0; i < f_ss->assigned.size()&& !oiOK; i++)
			{
				OperatorInstance*oi = f_ss->assigned[i];
				/* GL 23/07/07 : Replace lines
				   oiOK = true;
				   for(int j = 0; j<oi->oper()->inputPorts()&& oiOK;j++) {
				   oiOK = (oi->inputsBitwidth(j)>= ((Data*)o->predecessors[j]->source)->bitwidth());
				   }
				/* By*/
				oiOK = oi->fitsOperation(o);
				/* Fin GL*/
				if(oiOK)
				{
					if(_debug)cout << "Binding		OK" << endl;
					// store binding in operator
					s->instance = oi;					// assign instance to operation
					/* GL 02/10/07 : update oi connections*/
					if(oi)oi->UpdateConnection(o);
					/* Fin GL*/
					if(oi)oi->bind(f);				// store binding information into operator instance
					remove(f_ss->assigned, oi);
					_toBind->pop();
					//break;
				}
			}
			if(!oiOK && f_ss->assigned.size())
			{
				OperatorInstance*oi = f_ss->assigned[f_ss->assigned.size()- 1];
				oi->updateWidth(o);	//don't need reorder the list....This instance is the largest
				// store binding in operator
				s->instance = oi;					// assign instance to operation
				/* GL 02/10/07 : update oi connections*/
				if(oi)oi->UpdateConnection(o);
				/* Fin GL*/
				if(oi)oi->bind(f);				// store binding information into operator instance
				remove(f_ss->assigned, oi);
				_toBind->pop();		// remove it
			}

		}
	}
	/* Fin GL*/
	/* GL 05/09/07 : New Function : computing weights*/
	//! -# Minimum bipartite weighted matching
	//! -# for all matched nodes, compute edge weights
	//! -# select edge with minimum weight
	//! -# remove from set of nodes
	void Scheduling::MWBMatching()
	{

		FunctionRefs::const_iterator f_it;
		Function*f;
		FunctionSchedulingState* f_ss;
		OperationSchedulingState*s;
		const Operation*o;
		OperatorInstance*oi;
		long best, best_index;

		for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
		{
			f = (*f_it).second;			// get current function
			if(_debug)cout << "Scheduling::MWBMatching  Function " << f->name()<< endl;
			f_ss = (FunctionSchedulingState*)f->_addr;
			if(_debug)cout << "Scheduling::MWBMatching  " << f_ss->to_bind.size()<< " to bind" << endl;

			if(f_ss->assigned.size())
			{
				// create a bipartie graph
				BG*b_graph = new BG(f_ss->to_bind, f_ss->assigned);

				if(_debug)b_graph->info();
				b_graph->munkres(f);
				/* DH: 15/10/2009 replace max with munkre algorithm
				   while(b_graph->numberOfNodes()!=0)
				   {
				   b_graph->updateWeights();
				   if(_debug)b_graph->info();
				   BGedge*e = b_graph->maxMatching();
				   o = e->source->operation();
				   oi = e->target->instance();
				   if(_debug)cout <<"Scheduling::MWBMatching  Matching = " << o->name()<< " opr" << oi->no()<< endl;
				   s = (OperationSchedulingState*)o->_addr;// get operation's state
				// store binding in operator
				s->instance = oi;					// assign instance to operation
				if(oi)oi->UpdateConnection(o);
				if(oi)oi->bind(f);				// store binding information into operator instance
				if(!oi->fitsOperation(o))oi->updateWidth(o);
				// remove nodes
				remove(f_ss->assigned, oi);
				remove(f_ss->to_bind, o);
				b_graph->removeNode(e->source);
				b_graph->removeNode(e->target);
				}*/
			}
			if(f_ss->assigned.size()!=0 || f_ss->to_bind.size()!=0)
			{
				cerr << "Internal error: they're still " << f_ss->to_bind.size()<< " operations not bound in current cycle" << endl;
				cerr << "operators " <<  f_ss->assigned.size()<< endl;
				exit(1);
			}
		}
	}
	/* Fin GL*/
	//! Bind an operation to an operator at current time(_time).
	//!	-# Remove operation from set of schedulable operations EnsX.
	//!	-# Add operation to set of running operations EnsC.
	//!	-# Set operation start time at _time.
	//!	-# Assign operator instance to operation.
	//!	-# Mark operation as "running".
	//!	-# if there are memory mapping constraints, "freeze" memory accesses and protect critical loops.
	//!	-# Assign function to operator instance.
	//!

	// Some stuff for a "still alive" trace.
	static int trace_count = 0;	// to print a trace at console while scheduling
	static int trace_pos = 0;
	static char trace_char[] = {'|', '/', '-', '\\', '-', '|', '/', '-', '\\'};
	static const int trace_chars = sizeof(trace_char);
	static const int trace_limit = 50;

	void Scheduling::bind(const Operation*o, FunctionSchedulingState*f_ss, OperatorInstance*oi)
	{
		// mark operation as "scheduled"
		//OperationSchedulingState*o_ss = (OperationSchedulingState*)o->_addr;
		//o_ss->scheduled = true;
		//f_ss->EnsX.pop_front();
		remove(f_ss->EnsX, o);				// remove it from the set of ready operations
		//f_ss->EnsC.push_back(o);			// add operation into EnsC
		add(f_ss->EnsC, o);					// add operation into EnsC
		OperationSchedulingState*s = (OperationSchedulingState*)o->_addr;// get operation's state
		s->start(_time);					// set operation starting time
		//if(_sw->dumpSchedule())
		//	cout << "stage " << _stage << " " << o->name()<< " scheduled at " << s->start << " on instance " << oi << " of " << oi->oper()->name()<< endl;
		/* GL 24/05/07 : binding mode 2, 3; assign at the end of cycle
		   Replace line
		   s->instance = oi;					// assign instance to operation
		/* By*/
		/* GL 09/07/2007 : replace line
		   if((_sw->bindingMode()!=2)&&(_sw->bindingMode()!=3))
		/* By*/
		if(_sw->bindingMode()<2)/*GL*/ /**/
		{
			s->instance = oi;					// assign instance to operation
			/* GL 02/10/07 : update oi connections*/
			if(oi)oi->UpdateConnection(o);
		}
		/* Fin GL*/
		/* Fin GL*/
		if(oi)oi->isRunning();		// mark instance "running"
		if(_debug)cout << "Scheduling::bind Assigning operation " << o->name()<< " on instance " <<(oi ? oi->no(): -1)<< " of operator " <<(oi ? oi->oper()->name(): "=")<< " at start=" << _time << endl;

		// freeze memory accesses previous choices to validate the operation
		_memc.freeze();					// freeze read/write allocations
		// protect critical loops to avoid strange cases where schedule is not valid
		protectCriticalLoops(o);

		// store binding in operator
		const Function*f = o->function();	// get function of operation
		/* GL 24/05/07 : binding mode 2, 3; assign at the end of cycle
		   Replace line
		   if(oi)oi->bind(f);				// store binding information into operator instance
		/* By*/
		if(_sw->bindingMode()<2)
			if(oi)oi->bind(f);				// store binding information into operator instance
		/* Fin GL*/

		// "still alive" trace if gaut is launch with dos/unix prompt(EJ 09/2007)
		if(!_sw->silent_mode()&& !_sw->verbose()&& !_sw->use_IHM())
		{
			trace_count++;
			if(trace_count == trace_limit)
			{
				cout << trace_char[trace_pos++] << '\b';
				if(trace_pos == trace_chars)trace_pos = 0;
				trace_count = 0;
			}
		}
	}

	//! Build lists of schedulable operations at current time(_time). This produces lists of operations, ordered
	//! by mobility for all operators(of all functions used in the CDFG)having schedulable operations at _time.
	//!	For each function F in the CDFG, scan each executable operation O of EnsX(F). Then:
	//!	-# Compute mobility(O) = alap(O)- _time.
	//!	-# If asap(O)> _time forget O, else add O into all operations lists of operators allocated to F. These
	//! list are ordered by mobility.
	void Scheduling::orderOperations()
	{

		/* GL 20/06/07 : new OPERATIONS type*/
		OPERATIONS*ops = new OPERATIONS();
		ops->comp = &Priority::mobility;

		if(_sw->schedulingMode() ==1)
			ops->comp = &Priority::mobility_upBitwidth;
		if(_sw->schedulingMode() ==2)
			ops->comp = &Priority::mobility_downBitwidth;
		if(_sw->schedulingMode() ==3)
			ops->comp = &Priority::mobility_bitwidth;
		/* Fin GL*/

		FunctionRefs::const_iterator f_it;
		OperatorRefs::const_iterator o_it;
		FunctionSchedulingState::OPERATIONS::iterator it;
		long newops, i;
		Function*f;
		FunctionSchedulingState* f_ss;
		const Operation*o;
		OperationSchedulingState*o_ss;
		struct func_sched fs,*fs_ptr;

		// recompute the mobility of all operations in the global scheduling list
		if(_debug)cout << "recomputing mobility of " << _operations->size()<< " operations" << endl;
		while(!_operations->empty())
		{
			o = _operations->top();
			_operations->pop();
			o_ss = (OperationSchedulingState*)o->_addr;	// get its scheduling state
			o_ss->mobility = o->alap()- _time;				// update its mobility with _time
			/* GL 07/01/08 : add lines*/
			// compute operation overcost
			if(_sw->schedulingMode() ==3)((Operation*)o)->computeOvercost();
			/* Fin GL*/
			if(_debug)cout << " operation " << o->name()<< " alap=" << o->alap()<< " time=" << _time << " mobility=" << o_ss->mobility << endl;
			ops->push(o);
		}
		delete _operations;
		_operations = ops;

		// add new operations, if any
		newops = 0;	// to trace number of new operations ordered
		if(_debug)cout << endl << "Scheduling::orderOperations building candidates" << endl;
		for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
		{
			f = (*f_it).second;			// get current function
			if(_debug)cout << "Scheduling::orderOperations  Function " << f->name()<< endl;
			f_ss = (FunctionSchedulingState*)f->_addr;
			// look at function's operations to execute
			for(it = f_ss->EnsX.begin(); it != f_ss->EnsX.end(); it++)
			{
				o =*it;									// get current operation
				o_ss = (OperationSchedulingState*)o->_addr;
				if(o_ss->to_schedule)continue;			// already stored in global scheduling list
				// ELSE
				if(_debug)cout << "Scheduling::orderOperations    Operation " << o->name()<< ", asap=" << o->asap()<< endl;
				if(o->asap()> _time)
				{
					if(_debug)cout << "Scheduling::orderOperations      stop here, asap=" << o->asap()<< ", time=" << _time << endl;
					continue;								// process next operation
				}
				// ELSE
				o_ss->mobility = o->alap()- _time;			// compute mobility
				/* GL 07/01/08 : add lines*/
				// compute operation overcost
				if(_sw->schedulingMode() ==3)((Operation*)o)->computeOvercost();
				/* Fin GL*/
				o_ss->to_schedule = true;					// change operation scheduling state
				if(_debug)cout << "Scheduling::orderOperations      alap=" << o->alap()<< ", time=" << _time << " mobility=" << o_ss->mobility << endl;
				// add the operation to the set of schedulable operations of the function's allocated operators
				_operations->push(o);						// add it in global list of operations to schedule
				newops++;									// one more added

				// update global scheduling functions list info
				for(i = 0; i < _functions.size(); i++)
				{
					fs_ptr = &_functions[i];
					if(fs_ptr->func == o->function())
					{
						fs_ptr->n++;    // one more operation for this function
						break;
					}
				}
				if(i == _functions.size())// not found, add a new function in the list
				{
					fs.func = f;
					fs.n = 1;								// with one operation !
					fs.no_more_hardware = false;
					_functions.push_back(fs);				// add it
				}
			}
		}
		if(_sw->verbose())cout << " " << newops << " new ops";
	}

	//! Try to schedule the best operation candidate.
	//!	-# Try to find an operator instance ready for an operation to schedule.
	//!	-# If found, bind operation and operator instance.
	//!
	//! @result a boolean, true = scheduling succeeded, false = scheduling failed
	bool Scheduling::tryToScheduleOperation(const Operation*o, SchedulingOut*sos)
	{
		if(_debug)cout << endl << "Scheduling::scheduleOperation****schedule " << o->name()<< " # " << o << endl;
		FunctionSchedulingState*s = (FunctionSchedulingState*)o->function()->_addr;	// get state
		// find an operator for an operation
		OperatorInstance*oi;
		bool delayed_by_memory_constraint = false; // EJ 07/12/2007 delayed by a memory constraint
		if(oi = getInstanceOrWait(o,_sos, &delayed_by_memory_constraint))
		{
			/* GL 24/05/07 : binding mode 2, 3; assign at the end of cycle*/
			if(_sw->bindingMode()>1)
			{
				/* GL 07/09/2007 : new binding mode*/
				if(_sw->bindingMode() ==2 || _sw->bindingMode() ==3)
					/* Fin GL*/
					_toBind->push(o); // store in sorted list _toBind, and bind operations later
				/* GL 07/09/2007 : new binding mode*/
				if(_sw->bindingMode() ==4)
					/*s->to_bind.push_back(o);//*/insert(s->to_bind, o);
				/* Fin GL*/

				/*s->assigned.push_back(oi);//*/
				insert(s->assigned, oi);
				if(_debug)cout << "tryToScheduleOperation  Operations to bind " << _toBind->size()<< " : New operation scheduled " << o->name()<< endl;
			}
			/* Fin GL*/

			bind(o, s, oi);	// assign the operation to a real operator
			return true;
		}
		else if(delayed_by_memory_constraint)// EJ 07/12/2007 delayed by a memory constraint
		{
			if(_debug)cout << "delayed by a memory constraint" << endl;
			return false;

		}
		else if(o->function()->passThrough()&& o->function_name() == "mem_read")// EJ 14/02/2008 : dynamic memory
		{
			bind(o, s, 0);	// assign the operation to the virtual operator "mem_read"
			return true;
		}
		else if(o->function()->passThrough()&& o->function_name() == "mem_write")// EJ 14/02/2008 : dynamic memory
		{
			bind(o, s, 0);	// assign the operation to the virtual operator "mem_write"
			return true;
		}
		else if(o->function()->passThrough()&& o->function_name() == "assign")
		{
			bind(o, s, 0);	// assign the operation to the virtual operator "="
			return true;
		}
		else if(o->function()->passThrough()&& o->function_name() == "sliceread")// DH: passTrought operator sliceread(range bit selection)
		{
			bind(o, s, 0);	// assign the operation to the virtual operator "="
			return true;
		}
		return false;	// scheduling failed
	}
	/* GL / 08/01/08 : add lines*/
	//! Detect if a CDFG node is scheduled.
	//! @param Input. node is a pointer to a CDFG node.
	//! @result true if the node is finished, false otherwise.
	bool Scheduling::scheduled(const CDFGnode*node)const
	{
		bool res = true;
		if(_debug)cout << "scheduled ? " << node->name()<< endl;
		vector<CDFGedge*>::const_iterator e_it;
		switch(node->type())
		{
		case CDFGnode::CONTROL:
		case CDFGnode::DATA:
			if(_debug)cout << "scheduled propagate to predecessors" << endl;
			// check predecessors
			for(e_it = node->predecessors.begin(); e_it != node->predecessors.end(); e_it++)
			{
				const CDFGedge*e = (*e_it);				// get current edge
				const CDFGnode*t = e->source;				// get source node
				if(!scheduled(t))// if at least one is not finished, I'm not
				{
					res = false;
					break;									// stop here
				}
			}
			break;
		case CDFGnode::OPERATION:
			if(_debug)cout << "scheduled is an operation" << endl;
			const Operation*o = (Operation*)node;		// dynamic link here
			OperationSchedulingState*o_ss = (OperationSchedulingState*)o->_addr;
			res = (o_ss->start()!= -1);
			if(_debug)cout <<(res ? "YES" : "NO")<< endl;
			break;
		}
		return res;
	}
	bool Scheduling::allPredecessorsScheduled(const Operation*o)const
	{
		if(_debug)cout << "allPredecessorsScheduled ? for " << o->name()<< endl;
		vector<CDFGedge*>::const_iterator e_it;
		for(e_it = o->predecessors.begin(); e_it != o->predecessors.end(); e_it++)
		{
			const CDFGedge*e = (*e_it);				// get current edge
			const CDFGnode*t = e->source;				// get source node
			if(!scheduled(t))// at least one predecessor not scheduled, so I'm not
			{
				if(_debug)cout << "NO" << endl;
				return false;							// propagate
			}
		}
		if(_debug)cout << "YES" << endl;
		return true;									// all scheduled !
	}
	/* Fin GL*/
	/* 29/10/07 : add lines*/
	long Scheduling::chainOperationsInPattern(const Operation* o)
	{
		vector<const Operation*> toChain;
		long delay;
		// check if operation is in a pattern and if so perform chaining
		if(_sw->chaining())
		{
			string s;
			bool found = false;
			vector<const Operation*> o_successors;
			get_operations_successors((CDFGnode*)o, &o_successors);
			for(int i = 0; i< o_successors.size()&& !found; i++)
			{
				Pattern*p;
				const Operation*suc = o_successors.at(i);
				s = "all_" + suc->sym_function();
				if(o->inst())
					delay = (long)ceil((double)((o->inst()->oper()->getCombinationalDelay(o->function())+ suc->function()->getCombinationalDelayMax())/_clk->period()))+ 1;
				else
					delay = (long)ceil((double)((o->function()->getCombinationalDelayMax()+ suc->function()->getCombinationalDelayMax())/_clk->period()))+ 1;
				// look for all_function pattern
				if(_cdfg->patterns().search(s, &p))
				{
					if(delay > p->cycle())break;
					found = true;
					if(_debug)cout << "Scheduling::chainOperationsInPattern	Found 'all_function' pattern" << s << endl;
					toChain.push_back(suc);
				}
				else
				{
					s = o->sym_function()+ "_" + suc->sym_function();
					// look for function1_function2 pattern
					if(_cdfg->patterns().search(s, &p))
					{
						//CAA 03/12/2009: To take into account the real delay for the chaining
						Function*f;				
							if(!_lib->functions().search(s, &f))
								cout << "Warning: Do not forget to characterize the real operator for the chaining to get the real timing" <<endl;					
							else 					
								delay = (long)ceil((double)(f->getCombinationalDelayMax())/_clk->period());					
									//End CAA 03/12/2009
									if(delay > p->cycle())break;
						found = true;
						if(_debug)cout << "Scheduling::chainOperationsInPattern	Found 'function1_function2' pattern" << s << endl;
						toChain.push_back(suc);
					}
					else
					{
						vector<const Operation*> suc_predecessors;
						get_operations_predecessors((CDFGnode*)o, &suc_predecessors);
						for(int j = 0; j< suc_predecessors.size()&& !found; j++)
						{
							const Operation*pre = suc_predecessors.at(j);
							s = o->sym_function()+ "." + pre->sym_function()+ "_" + suc->sym_function();
							// look for function1.function2_function3 pattern
							if(_cdfg->patterns().search(s, &p))
							{
								//CAA 03/12/2009: To take into account the real delay for the chaining
								Function*f;							
									if(!_lib->functions().search(s, &f))
										cout << "Warning: Do not forget to characterize the real operator for the chaining to get the real timing" <<endl;
									else
										delay = (long)ceil((double)(f->getCombinationalDelayMax())/_clk->period());				
											//End CAA 03/12/2009
											if(delay > p->cycle())break;
								found = true;
								if(_debug)cout << "Scheduling::chainOperationsInPattern	Found 'function1.function2_function3' pattern" << s << endl;
								toChain.push_back(pre);
								toChain.push_back(suc);
							}
						}
					}
				}
			}
			if(_debug)cout << "Scheduling::chainOperationsInPattern	Chaining " << toChain.size()<< " operation" << endl;
			while((!_no_more_hardware || _sw->ioConstraints()|| _sw->scheduling_strategy()!= Switches::DEFAULT)&&(toChain.size()!=0))
			{
				const Operation* op = toChain[toChain.size()-1];
				/* GL / 08/01/08 : add lines*/
				if(!allPredecessorsScheduled(op))
				{
					toChain.pop_back();
					continue;
				}
				/* Fin GL*/
				if(!tryToScheduleOperation(op ,_sos))
				{
					if(_debug)cout << "Scheduling::chainOperationsInPattern      Operation chained " << op->name()<< " scheduling failed" << endl;
					return 1;
				}
				else
				{
					if(_debug)cout << "Scheduling::chainOperationsInPattern      Operation chained " << op->name()<< " scheduling succeeded" << endl;
					((Operation*)op)->chained(1); // means chained to ...
					((Operation*)o)->chained(-1);// means chained with ...
					// update global scheduling list info
					for(long i = 0; i < _functions.size(); i++)
					{
						struct func_sched*fs = &_functions[i];
						if(fs->func == op->function())
						{
							fs->n--;    // one less operation for this function
							break;
						}
					}
					return 2;
				}
				toChain.pop_back();
			}
		}
		return 0;
	}
	/* Fin GL*/
	//!	Iterativeley select the best candidate among all the mobility-orderer operations
	//! and try to schedule it until there are no more operations to schedule.
	//! Due to IO and memory-mapping constraints, this is a SCHEDULING-TRY. Constraints might be
	//! such that some operation needs to be delayed to a later clock cycle.
	void Scheduling::scheduleOperations(SchedulingOut*sos)
	{
		// schedule current candidates

		if(_debug)cout << "scheduling candidates" << endl;
		//	OperatorSchedulingState::OPERATIONS*operations;
		//	long n = 0; // trace
		//	while(o = selectBestCandidate(&operations)) {

		// reset scheduling state of functions and operators
		FunctionRefs::const_iterator f_it;
		OperatorRefs::const_iterator o_it;
		vector<const Operation*>::iterator ops_it;
		const Function*f;
		const Operation*o;
		FunctionSchedulingState* f_ss;
		/* GL 26/04/2007 :	Replace fllowing line
		   operator is no more constant
		   const Operator*oper;
		   By*/
		Operator*oper;
		/* Fin GL*/
		OperatorSchedulingState*o_ss;
		long processed = 0;
		long scheduled = 0; // number of operations scheduled
		long failed = 0;	// number of operations for which scheduling failed

		// reset hardware status
		_no_more_hardware = false;
		for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
		{
			f = (*f_it).second;			// get current function
			f_ss = (FunctionSchedulingState*)f->_addr;
			f_ss->no_more_hardware = false;	// reset
			for(o_it = f_ss->allocated.begin(); o_it != f_ss->allocated.end(); o_it++)
			{
				oper = (*o_it).second;
				o_ss = (OperatorSchedulingState*)oper->_addr;
				o_ss->no_more_hardware = false; // reset
			}
		}

		// scan operations in order now
		if(_sw->verbose())cout << ", " << _operations->size()<< " to schedule";

		// if no more hardware and no others constraints operations then next cycle else continue
		while((!_no_more_hardware							||
			   _sw->ioConstraints()||
			   _sw->scheduling_strategy()!= Switches::DEFAULT	||
			   there_is_loopback_variable)&& // EJ 09/2007 there_is_loopback_variable
			  _operations->size())
		{
			processed++;
			/* 29/10/07 : add lines*/
			if(_sw->chaining()&&((o = _operations->top())->isChained()))// attention au test -1/1
			{
				if(_debug)cout << "Scheduling::scheduleOperations	" << o->name()<< " Already scheduled(chained)" << endl;
				_operations->pop();		// remove it
				continue;
			}
			/* Fin GL*/
			if(!tryToScheduleOperation(o = _operations->top(),_sos))
			{
				if(_debug)cout << "Scheduling::scheduleOperations      Operation " << o->name()<< " scheduling failed" << endl;
				_operations_failed.push_back(o);
				/* Caaliph 27/06/2007*/
				if(_sw->cdfgs())//Increment the operation delay: use for MM scheduling
				{
					if(o->alap()<=_time)
						((Operation*)o)->delay_inc();
				}
				/* End Caaliph*/
				failed++;
			}
			else
			{
				if(_debug)cout << "Scheduling::scheduleOperations      Operation " << o->name()<< " scheduling succeeded" << endl;
				// update global scheduling list info
				for(long i = 0; i < _functions.size(); i++)
				{
					struct func_sched*fs = &_functions[i];
					if(fs->func == o->function())
					{
						fs->n--;    // one less operation for this function
						break;
					}
				}
				scheduled++;
				/* 29/10/07 : add lines*/
				if(_sw->chaining())
				{
					if(long c = chainOperationsInPattern(o)!= 0)
					{
						processed++;
						if(c==1)failed ++;
						if(c==2)scheduled++;
					}
				}
				/* Fin GL*/
			}
			_operations->pop();		// remove it
		}
		if(_sw->verbose())cout << ", p=" << processed << " s=" << scheduled << " f=" << failed    << endl;

		// assure push back all rested operations into global list
		while(_operations->size())
		{
			o = _operations->top();
			_operations_failed.push_back(o);
			_operations->pop();
		}

		// push back failed operations into global list
		for(ops_it = _operations_failed.begin(); ops_it != _operations_failed.end(); ops_it++)
		{
			o =*ops_it;
			if(_debug)cout << "Scheduling::scheduleOperations      Operation " << o->name()<< " back to scheduling list" << endl;
			_operations->push(o);
		}
		_operations_failed.clear();	// empty the failed operations list
	}

	//! Schedule operations ready at current clock cycle.
	//!	-# If there are operations to schedule at current time, schedule them.
	//!	-# If there are operations to schedule now or later return true, else return false.
	//!
	//! @result a boolean telling if there are still some operations to schedule, now and/or later.
	//! A false return value mean scheduling is finished.
	bool Scheduling::schedule(SchedulingOut*sos)
	{
		// schedule operations now.
		if(_sw->verbose())cout << "time=" << _time;
		orderOperations();
		scheduleOperations(_sos);
		// Check for necessity to continue to schedule.
		// There are still operations to schedule if there is at least one function with EnsX, EnsC, or EnsCC not empty.
		FunctionRefs::const_iterator f_it;
		bool operations_to_schedule = false;
		for(f_it = _used_functions.begin(); f_it != _used_functions.end()&& !operations_to_schedule; f_it++)
		{
			Function*f = (*f_it).second;			// get current function
			FunctionSchedulingState* f_ss = (FunctionSchedulingState*)f->_addr;
			if(long n = f_ss->operationsToSchedule())
			{
				if(_debug)cout << "function " << f->name()<< " has " << n << " operations to schedule" << endl;
				operations_to_schedule = true;
			}
		}
		return operations_to_schedule;
	}

	//! Allocate temporary memory for an operation.
	static long address_in_bank = 0;
	static void alloc_node(CDFGnode*n)
	{
		Data*d;
		switch(n->type())
		{
		case CDFGnode::OPERATION:
			// allocat ememory for scheduling
			n->_addr = new OperationSchedulingState();
			break;
		case CDFGnode::DATA:
			d = (Data*)n;	// dynamic link
			// number unmapped data in bank -1
			if((d->bank() == -1)&&(d->address() == -1))d->setBank(-1, address_in_bank++);
			break;
		}
	}

	//! Free temporary memory for an operation
	static void free_node(CDFGnode*n)
	{
		if(n->type() == CDFGnode::OPERATION) {
			delete n->_addr;
		}
	}

	Scheduling::~Scheduling()
	{
#ifdef CHECK
		Scheduling_delete++;
#endif
		// free temporary memory allocated to CDFG
		_cdfg->scan_nodes(free_node);
	}

	//! CDFG update starts from the source and goes to the sink.
	//! Update CDFG start values of nodes with start values produced by scheduling.
	//! For each operation, copy the start value produced by the scheduling.
	//! For each data: copy the end of the operation that produces the data.
	//! For each control node: source node has a time of 0, sink node has the maximum
	//! time of the CDFG(ie the maximum end time of all preceding operations/data.
	void Scheduling::updateCDFG(CDFGnode*n)
	{
		const OperationSchedulingState*oss;
		const CDFGedge*e;		// an edge
		const CDFGnode*n_src;	// a source node
		const CDFGnode*n_dst;	// a destination node
		Operation*o;			// an operation
		Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
		if(n->start()!= -1)return;		// already updated
		//if(!_sw->silent_mode())cout << ".";
		switch(n->type())
		{
		case CDFGnode::OPERATION:
			// get pointer to scheduling data
			oss = (const OperationSchedulingState*)n->_addr; // dynamic link here
			if(oss->start() == -1)
			{
				cerr << "Internal error: operation " << n->name()<< " has not been scheduled" << endl;
				exit(1);
			}
			n->start(oss->start()); // propagate from scheduling data to CDFG data.
			/* GL : correction chainage pour Dominique*/
			if(((const Operation*)n)->isChained() == 1) {
				vector<const Operation*> predecessors;
				get_operations_predecessors(n, &predecessors);
				for(int i = 0; i < predecessors.size(); i++)
				{
					const Operation*pre = predecessors.at(i);
					if(pre->isChained() == - 1)n->start(pre->length()+ oss->start()- _clk->period()); // operation really begins 1 cycle before the source.
				}
			}
			/* Fin GL*/
			o = (Operation*)n;	// get operation
			o->inst(oss->instance);	// keep instance pointer
			break;
		case CDFGnode::DATA:
		case CDFGnode::CONTROL:
			// get max "end" value of previous nodes
			long max = 0;
			// scan all edges to predecessors
			for(e_it = n->predecessors.begin(); e_it != n->predecessors.end(); e_it++)
			{
				e =*e_it;			// get the edge
				n_src = e->source;	// get the source node
				if(n_src->start()< 0)return; // not all predecessors updated
				long end = n_src->start()+ n_src->length();
				if(end > max)max = end;
			}
			n->start(max);
			break;
		}
		// propagate to all successors
		for(e_it = n->successors.begin(); e_it != n->successors.end(); e_it++)updateCDFG((*e_it)->target);
	}

	void Scheduling::clearCDFG(CDFGnode*n)
	{
		const OperationSchedulingState*oss;
		const CDFGedge*e;		// an edge
		const CDFGnode*n_src;	// a source node
		const CDFGnode*n_dst;	// a destination node

		Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
		if(n->start() == -1)return;		// already updated
		//if(!_sw->silent_mode())cout << ".";
		switch(n->type())
		{
		case CDFGnode::OPERATION:
			n->start(-1); // propagate from scheduling data to CDFG data.
			break;
		case CDFGnode::DATA:
		case CDFGnode::CONTROL:
			// scan all edges to predecessors
			for(e_it = n->predecessors.begin(); e_it != n->predecessors.end(); e_it++)
			{
				e =*e_it;			// get the edge
				n_src = e->source;	// get the source node
				if(n_src->start()!= -1)return; // not all predecessors updated
			}
			n->start(-1);
			break;
		}
		// propagate to all successors
		for(e_it = n->successors.begin(); e_it != n->successors.end(); e_it++)clearCDFG((*e_it)->target);
	}

	Scheduling::Scheduling(
						   const Switches*sw,					// in
						   CDFG*cdfg,					// in
						   const Cadency*cadency,				// in
						   const Clock*clk,					// in
						   const Mem*mem,						// in
						   SelectionOutRefs*sels,				// in = the output of the multi-delay selection stage
						   AllocationOut*allocs,				// in = the output of the multi-fucntion selection stage
						   const Lib*lib, // CAA for the chaining
						   SchedulingOut*sos					// out
						  ): _memc(mem, cadency, cdfg, &sos->banks)
	{
#ifdef CHECK
		Scheduling_create++;
#endif

		FunctionRefs::iterator f_it;
		unsigned int pass = 0;

		// init locals
		_sw = sw;
		_cdfg = cdfg;
		_allocs = allocs;
		_sels = sels;
		_clk = clk;
		_cadency = cadency;
		_mem = mem;
		_sos = sos;
		_stages = _allocs->size();
		_newAllocateOperator.clear();
		_lib = lib; // CAA for chaining
		there_is_loopback_variable = false; // EJ 09/2007 init not yet find loopback variable

		/* GL 29/05/07 : Print modes*/
		if(_sw->verbose())cout << "Scheduling mode... " << _sw->schedulingMode()<< " Binding mode... " << _sw->bindingMode()<< endl;

		do
		{
			_operations = new OPERATIONS();
			/* GL 22/06/07 : new OPERATIONS type*/

			_operations->comp = &Priority::mobility;
			if(_sw->schedulingMode() ==1)
				_operations->comp =&Priority::mobility_upBitwidth;
			if(_sw->schedulingMode() ==2)
				_operations->comp =&Priority::mobility_downBitwidth;
			if(_sw->schedulingMode() ==3)
				_operations->comp =&Priority::mobility_bitwidth;

			_toBind = new OPERATIONS();

			if(_sw->bindingMode() ==2)
				_toBind->comp =&Priority::upBitwidth;
			if(_sw->bindingMode() ==3)
				_toBind->comp = &Priority::downBitwidth;

			/* Fin GL*/
			/* EJ 26/06/2007*/
			pass = pass + 1;
			if(_sw->verbose()&& _sw->operator_optimization() == true)cout << "Pass ..." << pass << endl;
			cout.flush();
			_newAllocateInstance = false;
			/* End EJ*/

			// allocate temporary memory to all functions
			cdfg->extract_list_of_used_functions(&_used_functions);

			// check all operations can be scheduled
			for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
			{
				Function*f = (*f_it).second;
				FunctionSchedulingState*f_ss = new FunctionSchedulingState(f);
				f->_addr = f_ss;									// allocate temporary state, X, C & CC are empty

				// allocate temporary memory to all operators
				for(int stage = 0; stage < _stages; stage++)// for all stages
				{
					AllocationOutStage*mf = &(_allocs->at(stage));		// get current instance
					long nops = mf->operators().size();				// get number of operator types****stages
					for(int i = 0; i < nops; i++)// for all instances
					{
						Operator*o = (Operator*)mf->operators()[i];	// get operator identity
						if(!o->_addr)
							o->_addr = new OperatorSchedulingState();// allocate temporary state
						// add it to the allocated operators of the function
						if(o->implements(f))
						{
							Operator*foo;
							if(!f_ss->allocated.search(o->name(), &foo))
								f_ss->allocated.add(OperatorRef(o));
						}
					}
				}
			}
			// 1/ Allocate temporary memory to all CDFG operations.
			// 2/ Before we schedule the CDFG, we must look at the Data not mapped to banks.
			// by default, unmapped Data have an address(-1, -1) = (bank no, address in bank)
			// we must allocate a unique address for each Data in bank -1 for the scheduling
			// to work properly.
			cdfg->scan_nodes(alloc_node);

			// init time
			_time = 0;

			// init function's states with list of first operations(the successors of the source)
			setSuccessorsExecutable(0, cdfg->source());				// build first set of executable operations
			_stage = 0;												// pipeline stages are numbered from 0 to N-1
			_stageTimeBoundary = _cadency->length();
			newStage(_stage, _sos);										// allocate hardware

			// schedule all CDFG
			while(schedule(_sos))// schedule all operations "now"
			{
				/* GL 24/05/07 : binding mode 2, 3; assign at the end of cycle*/
				if(_sw->bindingMode()> 1)
					bind();
				if(_toBind->size())
				{
					cerr << "Error: There are still " << _toBind->size()<< " operations not Bound in the current cycle " << endl;
					exit(1);
				}
				/* Fin GL*/
				// fly time
				_time += _clk->period();							// one more clock cycle
				if(_debug)cout << endl << "Scheduling: waiting one clock cycle time=" << _time << endl;
				freeOperators();									// free operators for which operations are finished
				if(_time == _stageTimeBoundary)// crossing a stage time boundary
				{
					freeStage(_stage, _allocs);
					_stage++;
					newStage(_stage, _sos);
					_stageTimeBoundary += _cadency->length();
				}
			}
			freeStage(_stage, _allocs);										// free last operator instances

			// EJ 26/06/2007 : optimization operator
			if(_newAllocateInstance == true && _sw->operator_optimization() == true)
			{
				// reset count OperatorInstance
				OperatorInstance*oinst = new OperatorInstance(0, _cadency, _clk, _sels);
				delete oinst;
				for(int s=0; s<=_stage; s++)_sos->stages[s].instances.clear();
				// clear start
				clearCDFG(_cdfg->source());
				// free global scheduling list
				delete _operations;
				/* GL 24/05/07 : bind operations after one cycle*/
				delete _toBind;
				/* Fin GL*/
				// add new operator in allocation
				Operator*o = ((Operator*)_newAllocateOperator[0]);
				o->_addr = 0;
				_allocs->at(0).operators().push_back((const Operator*)o);
				_newAllocateOperator.clear(); // free operator list, EJ 09/2007
			}

		}
		while(_newAllocateInstance == true && _sw->operator_optimization() == true);

		// update CDFG start values with info produced by scheduling
		updateCDFG(_cdfg->source());
		_memc.asap_shift(_cdfg, _mem);//DH : 30/11/2009
		IOC::alap_shift(_cdfg, true);
		IOC::asap_shift(_cdfg, _clk, _cadency, true);

		// free temporary memory of operators
		for(int stage = 0; stage < _stages; stage++)// for all stages
		{
			AllocationOutStage*mf = &(_allocs->at(stage));		// get current instance
			long nops = mf->operators().size();					// get number of stages
			for(int i = 0; i < nops; i++)// for all instances
			{
				Operator*o = (Operator*)mf->operators()[i];	// get operator identity
				if(o->_addr)
				{
					delete(OperatorSchedulingState*)o->_addr;// delete it
					o->_addr = 0;								// reset to empty
				}
			}
		}
		// free temporary memory of all functions
		for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
		{
			Function*function = (*f_it).second;
			delete(FunctionSchedulingState*)function->_addr;	// delete it
			function->_addr = 0;								// reset to empty
		}
		// free global scheduling list
		delete _operations;
		/* GL 24/05/07 : bind operations after one cycle*/
		delete _toBind;
		/* Fin GL*/
	}





	/************* Caaliph: MM Scheduling 27/06/2007*********************/
	Scheduling::Scheduling(
						   const Switches*sw,					// in
						   CDFG*cdfg,					// in
						   const Cadency*cadency,				// in
						   long  max_cad,
						   const Clock*clk,					// in
						   const Mem*mem,						// in
						   SelectionOutRefs*sels,				// in = the output of the multi-delay selection stage
						   AllocationOut*allocs,				// in = the output of the multi-fucntion selection stage
						   AllocationOutStep*allocs_bis,			// in = the output of the multi-fucntion selection stage(use for multimode)
						   SchedulingOut*sos					// out
						  ): _memc(mem, cadency, cdfg, &sos->banks)
	{
#ifdef CHECK
		Scheduling_create++;
#endif

		FunctionRefs::iterator f_it;
		unsigned int pass = 0;

		// init locals
		_sw = sw;
		_cdfg = cdfg;
		_allocs = allocs;
		_allocs_bis = allocs_bis;
		_sels = sels;
		_clk = clk;
		_cadency = cadency;
		_max_cad = max_cad;
		_mem = mem;
		_sos = sos;
		_stages = _allocs->size();
		_newAllocateOperator.clear();


		do
		{
			_operations = new OPERATIONS();
			/* GL 22/06/07 : new OPERATIONS type*/

			_operations->comp = &Priority::mobility;
			if(_sw->schedulingMode() ==1)
				_operations->comp =&Priority::mobility_upBitwidth;
			if(_sw->schedulingMode() ==2)
				_operations->comp =&Priority::mobility_downBitwidth;
			if(_sw->schedulingMode() ==3)
				_operations->comp =&Priority::mobility_bitwidth;

			_toBind = new OPERATIONS();

			if(_sw->bindingMode() ==2)
				_toBind->comp =&Priority::upBitwidth;
			if(_sw->bindingMode() ==3)
				_toBind->comp = &Priority::downBitwidth;

			/* Fin GL*/
			/* EJ 26/06/2007*/
			pass = pass + 1;
			if(_sw->verbose()&& _sw->operator_optimization() == true)cout << "Pass ..." << pass << endl;
			cout.flush();
			_newAllocateInstance = false;
			/* End EJ*/
			// allocate temporary memory to all functions
			cdfg->extract_list_of_used_functions(&_used_functions);

			// check all operations can be scheduled
			for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
			{
				Function*f = (*f_it).second;
				FunctionSchedulingState*f_ss = new FunctionSchedulingState(f);
				f->_addr = f_ss;									// allocate temporary state, X, C & CC are empty

				/*	for(int stage = 0; stage < _stages; stage++) {		// for all stages
					AllocationOutStage*mf = &(_allocs->at(stage));        // get current instance*/

				for(int cycle = 0; cycle < _cadency->length()/_clk->period(); cycle++)
				{

					AllocationOutCycle*mc = &_allocs_bis->allocationOutCycle()[cycle];
					long nops = mc->operators().size();				// get number of autres choses
					for(int i = 0; i < nops; i++)// for all instances
					{
						Operator*o = (Operator*)mc->operators()[i];	// get operator identity
						if(!o->_addr)o->_addr = new OperatorSchedulingState();// allocate temporary state
						// add it to the allocated operators of the function
						if(o->implements(f))
						{
							Operator*foo;
							if(!f_ss->allocated.search(o->name(), &foo))
								f_ss->allocated.add(OperatorRef(o));
						}
					}
				}
				//	}
				for(int stage = 0; stage < _stages; stage++)// for all stages
				{
					AllocationOutStage*mf = &(_allocs->at(stage));		// get current instance
					long nops = mf->operators().size();				// get number of stages
					for(int i = 0; i < nops; i++)// for all instances
					{
						Operator*o = (Operator*)mf->operators()[i];	// get operator identity
						if(!o->_addr)o->_addr = new OperatorSchedulingState();// allocate temporary state
						// add it to the allocated operators of the function
						if(o->implements(f))
						{
							Operator*foo;
							if(!f_ss->allocated.search(o->name(), &foo))
								f_ss->allocated.add(OperatorRef(o));
						}
					}
				}
			}

			// 1/ Allocate temporary memory to all CDFG operations.
			// 2/ Before we schedule the CDFG, we must look at the Data not mapped to banks.
			// by default, unmapped Data have an address(-1, -1) = (bank no, address in bank)
			// we must allocate a unique address for each Data in bank -1 for the scheduling
			// to work properly.
			cdfg->scan_nodes(alloc_node);

			// init time
			_time = 0;

			// init function's states with list of first operations(the successors of the source)
			setSuccessorsExecutable(0, cdfg->source());				// build first set of executable operations
			_stage = 0;												// pipeline stages are numbered from 0 to N-1
			_stageTimeBoundary = _cadency->length();

			bool _newAllocateInstance_temp = false; //Added by Caaliph: Temporary allocated instance

			_newAllocateInstance = false;

			_alloc_cycle_count=_time / _clk->period();
			newCycle(_alloc_cycle_count, _stage);
			// schedule all CDFG
			while(schedule(_sos))// schedule all operations "now"
			{
				/* GL 24/05/07 : binding mode 2, 3; assign at the end of cycle*/
				if(_sw->bindingMode()> 1)
					bind();
				if(_toBind->size())
				{
					cerr << "Error: There are still " << _toBind->size()<< " operations not Bound in the current cycle " << endl;
					exit(1);
				}
				/* Fin GL*/
				// fly time
				_time += _clk->period();							// one more clock cycle
				_alloc_cycle_count++;
				if(_debug)cout << endl << "Scheduling: waiting one clock cycle time=" << _time << endl;
				freeOperators();									// free operators for which operations are finished

				freeCycle(_alloc_cycle_count - 1, _stage);
				if(_time == _stageTimeBoundary)// crossing a stage time boundary
				{
					/////freeStage(_stage);
					_stage++;
					_alloc_cycle_count = 0;
					newCycle(_alloc_cycle_count, _stage);
					_stageTimeBoundary += _cadency->length();
				}
				else
				{
					if(_newAllocateInstance == true)
					{
						_newAllocateInstance = false;
						_newAllocateInstance_temp = true;
					}
					newCycle(_alloc_cycle_count, _stage);
				}
			}
			freeCycle(_alloc_cycle_count, _stage);
			_newAllocateInstance = _newAllocateInstance_temp;

			/* EJ 26/06/2007*/
			if(_newAllocateInstance == true && _sw->operator_optimization() == true)
			{
				// reset count OperatorInstance
				OperatorInstance*oinst = new OperatorInstance(0, _cadency, _clk, _sels);
				delete oinst;
				for(int s=0; s<=_stage; s++)_sos->stages[s].instances.clear();
				// clear start
				clearCDFG(_cdfg->source());
				// free global scheduling list
				delete _operations;
				/* GL 24/05/07 : bind operations after one cycle*/
				delete _toBind;
				/* Fin GL*/
				// add new operator in allocation
				Operator*o = ((Operator*)_newAllocateOperator[0]);
				o->_addr = 0;
				_allocs->at(0).operators().push_back((const Operator*)o);
			}
			/* End EJ*/

		}
		while(_newAllocateInstance == true && _sw->operator_optimization() == true && pass < 50);

		// update CDFG start values with info produced by scheduling
		updateCDFG(_cdfg->source());
		IOC::alap_shift(_cdfg, true);
		IOC::asap_shift(_cdfg, _clk, _cadency, true);

		for(int step=0; step<_cadency->length()/_clk->period()/*_alloc_cycle_count*/; step++)
			_allocs_bis->allocationOutCycle()[step].copy_operators().clear();

		// free temporary memory of all functions
		for(f_it = _used_functions.begin(); f_it != _used_functions.end(); f_it++)
		{
			Function*function = (*f_it).second;
			delete(FunctionSchedulingState*)function->_addr;	// delete it
			function->_addr = 0;								// reset to empty
		}
		// free global scheduling list
		delete _operations;
		/* GL 24/05/07 : bind operations after one cycle*/
		delete _toBind;
		/* Fin GL*/
	}
	/*************** End MM Scheduling******************/
	/* End Caaliph*/

	long OperatorInstance::count = 0;

	// end of: scheduling.cpp


