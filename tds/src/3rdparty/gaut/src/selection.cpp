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

//	File:		selection.cpp
//	Purpose:	Selection process
//	Author:		Pierre Bomel, LESTER, UBS

#include "selection.h"
#include "io_constraints.h"
#include "operation.h"
#include "allocation.h"
using namespace std;

#include <iostream>
using namespace std;
#include "check.h"

// SelectionOuts data management -----------------------------------------------------

// Duplication for selection process to generate all SelectionOut alternatives
void SelectionOutRefs::duplicate(SelectionOutRefs & res) const
{
	// scan all SelectionOuts
	if (_debug) cout << "SelectionOutRefs: duplicate: size=" << size() << endl;
	for (SelectionOutRefs::const_iterator m_it = begin(); m_it != end(); m_it++)
	{
		if (_debug) cout << ".";
		SelectionOut *m = (*m_it).second;			// get SelectionOut
		SelectionOut *new_SelectionOut = new SelectionOut(*m);	// take address of a real copy
		res.add(SelectionOutRef(new_SelectionOut));
	}
	if (_debug) cout << endl;
}

void SelectionOutRefs::del()
{
	// scan all SelectionOuts
	if (_debug) cout << "SelectionOutRefs: delete: size=" << size() << endl;
	for (SelectionOutRefs::iterator m_it = begin(); m_it != end(); m_it++)
	{
		if (_debug) cout << ".";
		SelectionOut *m = (*m_it).second;				// get SelectionOut
		delete m;
	}
	if (_debug) cout << endl;
}

void SelectionOutRefs::pipelineStages(int stages)
{
	_pipelineStages = stages;
	for (SelectionOutRefs::iterator it = begin(); it != end(); it++)
	{
		SelectionOut *m = (*it).second;
		m->clear();
		for (int i = 0; i < _pipelineStages; i++)
		{
			m->operations().push_back(0);
			m->operators().push_back(0);
			m->area().push_back(0);
			m->parallelismRate().push_back(0.0);
		}
	}
}

void SelectionOutRefs::info(bool only_valid) const
{
	if (only_valid && !_valid) return;	// print valid ones only
	cout << "(valid=" << _valid << ",cost=" << _cost;
	cout << ",cadency=" << _cadency;
	cout << ",stages=" << _pipelineStages;
	cout << ",critical-loop=" << _criticalLoop << ")" << endl;
	for (const_iterator it = begin(); it != end(); it++)
	{
		const SelectionOut *m = (*it).second;
		m->info();
	}
}

bool SelectionOutRefs::searchFunction(const Function *f, const SelectionOut **m) const
{
	/* DH: 23/10/2008 to accelerate selection for (SelectionOutRefs::const_iterator it = begin(); it != end(); it++)
	{
		const SelectionOut *SelectionOut = (*it).second;
		if (SelectionOut->func() == f)
		{
			*m = SelectionOut;
			return true;
		}
	} 
	return true;
	*/
	SelectionOut *value=NULL;
	bool found=search(f->name(),&value);
	*m=value;
	return found;
}

bool SelectionOutRefs::searchOperator(const Operator *o, const SelectionOut **m) const
{
	for (SelectionOutRefs::const_iterator it = begin(); it != end(); it++)
	{
		const SelectionOut *SelectionOut = (*it).second;
		if (SelectionOut->oper() == o)
		{
			*m = SelectionOut;
			return true;
		}
	}
	return false;
}

void SelectionOutRefs::info() const
{
	info(false);
}

void SelectionOutRefs::serialize(ostream &f, int index) const
{
	// dump me
	f << "multi-delay-selection(" << index << ") {" << "\n";
	f << "  selection ";
	int i = 0;
	SelectionOutRefs::const_iterator m_it;
	for (m_it = begin(); m_it != end(); m_it++)
	{
		SelectionOut *m = (*m_it).second;
		f << (i ? "," : "") << index << "_" << i;
		i++;
	}
	f << ";" << "\n";
//	f << "#  cadency " << _cadency << ";" << "\n";
//	f << "#  pipeline_stages " << _pipelineStages << ";" << "\n";
//	f << "#  cost " << _cost << ";" << "\n";
//	f << "#  critical-loop " << _criticalLoop << ";" << "\n";
	f << "}" << "\n";
	// dump my SelectionOuts
	i = 0;
	for (m_it = begin(); m_it != end(); m_it++)
	{
		SelectionOut *m = (*m_it).second;
		m->serialize(f, index, i++);
	}
}

void SelectionOutRefs::serialize(const string &name, int index) const
{
	ofstream f(name.c_str(), ios::out);
	serialize(f, index);
	f.close();
}

// SelectionOut Management ---------------------------------------------------------------

#include "cycles.h"

SelectionOut::SelectionOut(Function *func, const Operator *oper, const Clock *clk) :
		_func(func),
		_oper(oper)
{
	_cycles = oper->synchronous() ? oper->latency() : clockCycles(func, _oper, clk);
	_length = _cycles * clk->period();
#ifdef CHECK
	SelectionOut_create++;
	//cout << "SelectionOut create" << endl; info();
#endif
}

SelectionOut::SelectionOut(const SelectionOut &src) :
		_func(src._func),
		_oper(src._oper),
		_cycles(src._cycles),
		_length(src._length)
{
#ifdef CHECK
	SelectionOut_create++;
	//cout << "SelectionOut init" << endl; info();
#endif
}

string SelectionOut::name() const
{
	return _func->name()/* DH 23/10/2008 : to accelerate SearchFunction in Selection+"/"+_oper->name() */;
}

void SelectionOut::info() const
{
	cout << "  " << _oper->name() << "->" << _func->name();
	cout << " (cycles=" << cycles();
	cout << ",length=" << length();
	cout << ",area=" << oper()->area();
	cout << ")" << endl;
	for (int i = 0; i < _operations.size(); i++)
	{
		cout << "    stage " << i;
		cout << " (operations=" << _operations[i];
		cout << ",parallelism=" << _parallelismRate[i];
		cout << ",operators=" << _operators[i];
		cout << ",cost=" << _area[i] << ")" << endl;
	}
}

void SelectionOut::serialize(ostream &f, int global_no, long local_no) const
{
	f << "mapping(" << global_no << "_" << local_no << ") {" << "\n";
	f << "  function " << _func->name() << ";" << "\n";
	f << "  operator " << _oper->name() << ";" << "\n";
//	f << "# cycles   " << _cycles << ";" << "\n";
//	f << "# length   " << _length << ";" << "\n";
	f << "}" << "\n";
}

// SELECTION -------------------------------------------------------------------------

// build a set of SelectionOuts from parsed data
void Selection::convert_to_SelectionOuts(
    const ParseObjects *objs,			// in (parsed data)
    vector<SelectionOutRefs> *soRefs_v	// out
)
{
	for (ParseObjects::Obase::const_iterator it = objs->begin(); it != objs->end(); it++)
	{
		const ParseObject *object = (*it).second;
		if (object->_type == "multi-delay-selection")
		{
			// build a new SelectionOut
			SelectionOutRefs m_ref;
			// search for elementary SelectionOuts
			m_ref.clear();
			for (int i = 0; i < object->attribute("mapping")->_values.size(); i++)
			{
				// search object
				string SelectionOut_name = object->attribute("mapping")->_values[i];
				const ParseObject *map;
				if (!objs->search("SelectionOut", SelectionOut_name, &map))
				{
					cerr << "Error in selection file: SelectionOut '" << SelectionOut_name << "' unknown" << endl;
					exit(1);
				}
				// build elementary SelectionOut
				string function_name  = map->attribute("function")->_values[0];
				Function *f;
				if (!_lib->functions().search(function_name, &f))
				{
					cerr << "Error in selection file: function '" << function_name << "' unknown" << endl;
					exit(1);
				}
				string operator_name  = map->attribute("operator")->_values[0];
				Operator *o;
				if (!_lib->operators().search(operator_name, &o))
				{
					cerr << "Error in selection file: operator '" << operator_name << "' unknown" << endl;
					exit(1);
				}
				long cycles = Parser::atol(map->attribute("cycles")->_values[0]);
				//SelectionOut *m = new SelectionOut(f, o, cycles, sysclk);
				SelectionOut *m = new SelectionOut(f, o, _sysclk); // will recompute cycles and length
				// add it to SelectionOutRefs
				m_ref.add(m);
			}
			soRefs_v->push_back(m_ref);		// add new SelectionOut into new SelectionOuts list
		}
	}
}

// user driven (by a file) alternatives generation
void Selection::generate_alternatives(
    const string &name,					// in
    vector<SelectionOutRefs> *soRefs_v	// out
)
{
	// read a user file and build alternatives
	vector<string> selection_files;
	selection_files.push_back(name);		// selections file name
	Parser *parser = new Parser();			// build temporary parser
	ParseObjects *objs = new ParseObjects();// build temporary objects
	parser->parse(selection_files, objs);	// parse file
	delete parser;							// free memory
	//objs->info();							// dump parsed data
	convert_to_SelectionOuts(objs, soRefs_v);// exploit data
	delete objs;							// free parsing memory
}

// automatic generation of alternatives
void Selection::generate_alternatives(
    const FunctionRefs *used_functions,	// in
    vector<SelectionOutRefs> *soRefs_v	// out
)
{
	vector<Function *>   functions;				// the set of functions
	int findex;									// functions[] index
	long Nfunctions;
	vector<OperatorRefs> associated_operators;	// the set of selectable operators
	FunctionRefs::const_iterator f_it;			// Functions iterator
	OperatorRefs::const_iterator o_it;			// Operators iterator
	SelectionOutRefs::const_iterator m_it;		// SelectionOuts iterator


	// 1. associate to each function a set of implementing operators: build functions[] and associated_operators[]

	Nfunctions = 0;
	for (f_it = used_functions->begin(); f_it != used_functions->end(); f_it++)
	{
		Function *f = (*f_it).second;			// get list element
		// init functions[] and associated_operators[]
		functions.push_back(f);
		associated_operators.push_back(OperatorRefs());
		int n = f->numberOfMonoOperators();
		for (OperatorRefs::const_iterator o_it = f->implemented_by().begin(); o_it != f->implemented_by().end(); o_it++)
		{
			const Operator *o = (*o_it).second;
			// add operator to associated_operators[]
			if ((n == 0)||(o->mono()))
				// n == 0 : there are no mono-function operators, select all multi-function ones
				// when (n != 0), o_>mono() tells which operators are mono-function ones to add
				associated_operators[Nfunctions].add(OperatorRef((Operator*)o));
		}
		Nfunctions++;
	}

	//cout << "Nfunctions " << Nfunctions << endl;
	// 2. generate all alternatives: exploit functions[] and associate_operators[]

	vector<SelectionOutRefs> before, after;
	int bindex;													// "before" index
	// build first set of partial SelectionOuts from first function (index 0 in vectors)
	// we are 100% sure there is at least one function in the "used_functions" list
	findex = 0;													// look at function[0] only
	for (o_it = associated_operators[findex].begin(); o_it != associated_operators[findex].end(); o_it++)
	{
		const Operator *o = (*o_it).second;						// get operator = o
		SelectionOut *m = new SelectionOut(functions[findex], o, _sysclk);	// build SelectionOut(function[0], o) = m
		SelectionOutRefs m_refs;								// build an empty SelectionOuts list
		m_refs.add(SelectionOutRef(m));							// add element m into SelectionOuts empty list
		before.push_back(m_refs);								// add SelectionOuts list into "before" array
	}
	// build others sets
	for (findex = 1; findex < Nfunctions; findex++) 				// look at functions[1..Nfunctions-1]
	{
		const Function * f = functions[findex];					// get function
		// scan all partial solutions in "before"
		for (bindex = 0; bindex < before.size(); bindex++)
		{
			// scan all selectable operators for function f
			for (o_it = associated_operators[findex].begin(); o_it != associated_operators[findex].end(); o_it++)
			{
				const Operator *o = (*o_it).second;				// get operator = o
				SelectionOut *m = new SelectionOut(functions[findex], o, _sysclk);	// build SelectionOut(function[findex], o) = m
				SelectionOutRefs m_refs;						// build empty SelectionOuts list
				before[bindex].duplicate(m_refs);				// copy partial list into new one
				m_refs.add(SelectionOutRef(m));				// add new SelectionOut into new SelectionOuts list
				after.push_back(m_refs);						// add in "after" the new SelectionOuts list
			}
			before[bindex].del();
		}
		before.clear();
		//before = after;											// permute and restart
		for (int i=0;i<after.size();i++)
			before.push_back(after[i]);
		after.clear();
	}

	// 3. we have in "before" the full list of SelectionOuts: transfert it to SelectionOuts
		for (int i=0;i<before.size();i++)
			soRefs_v->push_back(before[i]);
	//(*soRefs_v) = before;
}

// do an ASAP first, then an ALAP, take into account IO constraints if required
bool Selection::ASAP_ALAP(
    const SelectionOutRefs *soRefs		// in
)
{
	_cdfg->reset(soRefs);						// reset all asap, alap, length and cycles times
	// start time = 0
	if (!_cdfg->ASAP(0)) return false;
	// if IO constraints: change ASAP time of inputs (all at 0 after a regular ASAP())
	//if (_sw->ioConstraints()) IOC::asap_shift(_cdfg, _sysclk, _cadency, false);
	//IOC::asap_shift(_cdfg);
	// end time is the one reached previously by ASAP()
	long stop = _cdfg->sink()->asap() + _cdfg->sink()->length();
	if (!_cdfg->ALAP(stop)) return false;
	// if IO constraints: change ALAP time of outputs (all at cadency after a regular ALAP()
	//if (_sw->ioConstraints()) IOC::alap_shift(_cdfg,false);
	//IOC::alap_shift(_cdfg);
	return true;
}

// Compute cost of a single alternative
bool Selection::compute_cost_of_alternative(
    SelectionOutRefs *soRefs				// in-out
)
{
	SelectionOutRefs::const_iterator m_it;			// SelectionOut iterator
	if (!ASAP_ALAP(soRefs)) return false;			// schedule CDFG with current SelectionOut
	const double L = _cdfg->estimated_best_latency();// get latency from scheduled CDFG
	const double C = _cadency->length();				// get cadency
	const int N = ceil(L/C);						// N = number of pipeline stages
	const Data  *critical_data;
	long criticalLoop = _cdfg->criticalLoop(&critical_data);
	// get CDFG critical loop & data
	//cdfg->info();
	//if (critical_data) cout << "critical data " << critical_data->name() << " -> " << criticalLoop << endl;
	//else cout << "no critical loop" << endl;
	soRefs->isValid();
	soRefs->criticalLoop(criticalLoop);				// set the SelectionOuts critical loop
	soRefs->cadency(C);								// set the SelectionOuts cadency
	soRefs->pipelineStages(N);						// set # of pipeline stages in the SelectionOut
	long critical_loop_max = C - _mem->access_time();
	if (critical_data && (criticalLoop > critical_loop_max))
	{
		/*if (_sw->verbose())*/
		cout << "WARNING: critical-loop(" << criticalLoop << ") of data '" << critical_data->name()
		<< "' > cadency(" << C << ")-mem_access_time(" << _mem->access_time() << ")"
		<< "=" << critical_loop_max << endl;
		return false;								// impossible to meet the cadency constraint
	}
	// this is the cost to compute
	long total_area = 0;
	for (m_it = soRefs->begin(); m_it != soRefs->end(); m_it++)
	{
		SelectionOut *m = (*m_it).second;			// get current SelectionOut
		const Function *F = m->func();				// get function F
		const Operator *Op = m->oper();				// get mapped operator Op
		const long D = m->length();					// get number of time units of Op
		vector<long> operations(N);					// create a vector to store the # number of operations/stage
		/* KT  06/06/2007 :	switch specifying the allocation mode (when global allocation is used)*/
		long Nb_ops =0;								// number of opertions having the function F in the cdfg
		// allocation mode is global allocation
		if (_sw->global_allocation())
		{
			numberOfOperatorsPerPipelineStage(soRefs, Op, &operations);	// fill funcs with actual values
			for (int j = 0; j < N; j++)  			// compute the number of opertions having the function F in the cdfg
			{
				Nb_ops = Nb_ops + operations[j];
			}
			long operators;
			long selected_operators ;
			if (_sw->theoretical())
			{
				operators = ceil((Nb_ops*D)/C);		// number of operators required
				selected_operators = ceil((Nb_ops*D)/C);
			}
			if (_sw->practical())
			{
				operators = ceil(Nb_ops/floor(C/D));		// number of operators required
				selected_operators = ceil(Nb_ops/floor(C/D));
			}

			long area = operators * Op->area();		// surface
			if ((selected_operators == 0) && !Op->passThrough()) selected_operators = 1;
			m->operations()[0] = Nb_ops;
			m->operators()[0] = selected_operators;
			m->area()[0] = area;
			m->parallelismRate()[0] = Nb_ops/floor(C/D);
			if (_sw->verbose()) cout <<"we have "<<operators  <<"  "<<Op->name()<<" and "<< Nb_ops <<" operations "<<endl;
			total_area += area;
		}


		// allocation mode is allocation/stage
		if (_sw->distributed_allocation())
		{
			numberOfOperatorsPerPipelineStage(soRefs, Op, &operations);	// fill funcs with actual values
			for (int i = 0; i < N; i++)
			{
				const long O = operations[i];			// number of occurences of Op in stage i
				double parallelismRate;
				if (_sw->theoretical())	parallelismRate = (O*D)/C;
				if (_sw->practical())	parallelismRate = O/floor(C/D);
				long operators = ceil(parallelismRate);	// number of operators required
				long area = operators * Op->area();		// surface
				//long selected_operators = floor(parallelismRate); // keep only the 100% used ones
				long selected_operators = ceil(parallelismRate);
				//if (!selected_operators) selected_operators = 1;
				if ((selected_operators == 0) && !Op->passThrough()) selected_operators = 1;
				m->operations()[i] = O;
				m->operators()[i] = selected_operators;
				m->area()[i] = area;
				m->parallelismRate()[i] = parallelismRate;
				total_area += area;						// accumulate costs

			}
		}

	}
	soRefs->cost(total_area);							// total_area contains now the cost !
	return true;
}

// compute the cost of all alternatives
void Selection::compute_cost_of_alternatives( vector<SelectionOutRefs> *soRefs_v)  	// in-out
{
	for (int i = 0; i < soRefs_v->size(); i++)
	{
		if (!compute_cost_of_alternative(&(*soRefs_v)[i]))
		{
			cout << "Info: selection alternative # " << i << " aborted" << endl;
			(*soRefs_v)[i].isNotValid();				// mark it unvalid
		}
	}
}

// find best alternative: this is the one with the minimum area
int Selection::select_best_alternative_and_update_cdfg(
    const vector<SelectionOutRefs> *soRefs_v,	// in
    SelectionOutRefs *map				// out
)
{
	if (soRefs_v->size() == 0)
	{
		cerr << "Internal error: no selection alternative" << endl;
		exit(1);
	}
	int selected = 0;
	// init minimum
	const SelectionOutRefs *soRefs = &(*soRefs_v)[selected];
	*map = *soRefs;
	long min = soRefs->cost();
	if (_debug) cout << "min=" << min << endl;
	// try to improve
	for (int i = 1; i < soRefs_v->size(); i++)
	{
		soRefs = &(*soRefs_v)[i];
		if (!soRefs->valid()) continue;
		if (soRefs->cost() < min)
		{
			*map = *soRefs;
			min = soRefs->cost();
			selected = i;
		}
	}
	// now update cdfg
	ASAP_ALAP(map);									// re-schedule CDFG with best SelectionOut
	return selected;
}

void Selection::numberOfOperatorsPerPipelineStage(
    const SelectionOutRefs *soRefs,
    const Operator *Op,
    vector<long> *operations
)
{
	int N = soRefs->pipelineStages();
	operations->clear();				// reset first
	vector<long> low_limit(N);
	long base = 0;
	int i;
	for (i = 0; i < N; i++)
	{
		operations->push_back(0);	// rebuild operations vector with 0s
		low_limit[i] = base;		// init limit
		base += soRefs->cadency();
	}
	// scan all nodes of the graph
	vector<CDFGnode*>::const_iterator n_it;
	for (n_it = _cdfg->nodes().begin(); n_it != _cdfg->nodes().end(); n_it++)
	{
		const CDFGnode *node = *n_it;					// get current node
		if (node->type() != CDFGnode::OPERATION) continue;		// process only Operations
		const Operation *o = (const Operation*)node;	// dynamic link
		const Function *f = o->function();				// get operation's function
		const SelectionOut *mdo;
		soRefs->searchFunction(f, &mdo);				// get function mapping
		const Operator *op = mdo->oper();				// get mapped operator
		if (Op != op) continue;							// process only Op operators
		int rank = -1;
		for (i = 1; i < N; i++)
		{
			long stop_time = node->asap() + node->length();
			if (stop_time <= low_limit[i])
			{
				rank = i-1;
				break;
			}
		}
		if (rank == -1) rank = N-1;
		(*operations)[rank]++;						// one more operations in the slice # rank
	}
}

// select operators among set of operators having same function but different delays
Selection::Selection(
    const Switches *sw,							// in
    const Cadency *cadency,						// in
    const Mem *mem,								// in
    const Clock *sysclk,						// in
    const Lib *lib,								// in
    CDFG *cdfg,							// in-out
    SelectionOutRefs *map					// out
)
{
	// copies of main parameters
	FunctionRefs used_functions;

	_sw = sw;
	_cadency = cadency;
	_mem = mem;
	_sysclk = sysclk;
	_lib = lib;
	_cdfg = cdfg;

	int i;

	// 1 = extract list of used functions
	//		input = cdfg
	//		output stored in "used_functions"
	_cdfg->extract_list_of_used_functions(&used_functions);
	//used_functions.info();
	// trace
	if  (_sw->verbose()) 
	{ 
		cout << "CDFG used functions" << endl;
		used_functions.info();
	}

	// 2 = generate set of SelectionOuts, result is ALWAYS stored in "soRefs_v"
	//		two choices
	//			- automatic generation
	//					input = name of file where to read the selection + sysclk + lib + cdfg
	//					output = soRefs_v
	//			- user driven generation
	//					input = used_functions + cadency + sysclk + lib + cdfg
	//					output = soRefs_v
	vector<SelectionOutRefs> soRefs_v;
	if (_sw->selection())  	// build SelectionOut alternatives from user selection file
	{
		/* if (!_sw->silent_mode()) */ cout << "User specified selections in file: " << _sw->selectionFile() << endl;
		generate_alternatives(_sw->selectionFile(), &soRefs_v);
	}
	else   // automatic build
	{
		if (_sw->verbose()) cout << "Generating selections" << endl;
		generate_alternatives(&used_functions, &soRefs_v);
	}
	// 3 = compute cost of all SelectionOuts, might supress some, be ceraful it might leave 0 "good" alternatives
	//		input = sw + cadency + SelectionOuts + cdfg
	//		output = SelectionOuts updated with cost value
	compute_cost_of_alternatives(&soRefs_v);
	// check number of good SelectionOuts
	long good = 0;
	for (i = 0; i < soRefs_v.size(); i++)
		if (soRefs_v[i].valid())
			good++;
	// trace
	if (_sw->verbose()) {
		cout << good << " selection alternatives generated" << endl;
		if (_sw->verbose()) {
			cout << "SelectionOut alternatives" << endl;
			for(i = 0; i < soRefs_v.size(); i++) {
				if (!soRefs_v[i].valid()) continue;
				cout << "Alternative " << i << endl;
				soRefs_v[i].info(true);
			}
		}
	}
	// sanity check
	if (good == 0)
	{
		cerr << "No alternatives to process, unable to select best one" << endl;
		exit(1);
	}
	// user required dump of all selections
	if (_sw->dumpSelections())
	{
		if (!_sw->silent_mode()) cout << "Dumping all SelectionOuts in file: " << _sw->dumpSelectionsFile() << endl;
		ofstream f(_sw->dumpSelectionsFile().c_str(), ios::out);
		for (int i = 0; i < soRefs_v.size(); i++)
			soRefs_v[i].serialize(f, i);
		f.close();
	}

	// 4 = select best alternative among all
	//		input = sw + SelectionOuts
	//		output = map + cdfg scheduled with best SelectionOut selected
	int selected = select_best_alternative_and_update_cdfg(&soRefs_v, map);
	// user required dump of best SelectionOut
	if (_sw->dumpBestSelection())
	{
		if (!_sw->silent_mode())
			cout << "Dumping best SelectionOut in file: " << _sw->dumpBestSelectionFile() << endl;
		map->serialize(_sw->dumpBestSelectionFile(), 0);
	}

	if (_debug) cout << "selected = " << selected << endl;
	// free memory of SelectionOuts for unselected SelectionOutRefs
	for (i = 0; i < soRefs_v.size(); i++)
		if (i != selected)
			soRefs_v[i].del();
}

// end of: selection.cpp




