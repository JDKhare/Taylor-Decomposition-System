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

//	File:		operation.h
//	Purpose:	OPERATION
//	Author:		Pierre Bomel, LESTER, UBS

class Operation;
class OperationRef;
class OperationRefs;

#ifndef __OPERATION_H__
#define __OPERATION_H__

#include <iostream>
using namespace std;
#include "map.h"
#include "cdfg.h"
#include "check.h"
/* GL 29/10/07 : add line */
#include "multimode_tools.h"
/* Fin GL */

class OperatorInstance;	// forward ref

//! Smart pointer on Operations
class OperationRef  : public MapRef <Operation>
{
public:
	OperationRef(Operation *d) : MapRef<Operation>(d){}
};
//! List of smart pointers on Operations
class OperationRefs : public MapRefs<Operation, OperationRef> {};

#include "function.h"
#include "selection.h"

//! CDFG Operation nodes
class Operation : public CDFGnode
{
private:
	Function *_function;			//!< pointer to function
public:
	/*const */
	string _function_name;	//!< function name
private:
	// pre-scheduling data
	long _cycles;					//!< number of cycles (pre-scheduling data)
	long _length;					//!< length in time units (pre-scheduling data)
	// scheduling data
	long _asap;						//!< asap time (post-scheduling data)
	bool _asap_locked;				//!< asap locked status
	long _alap;						//!< alap time (post-scheduling data)
	bool _alap_locked;				//!< alap locked status
	long _start;					//!< start time (post-scheduling data)
	/* Caaliph 27/06/2007 */
	long _delay;
	long _identify;
	long _mode;
	string _name_operator;//add by Caaliph
	bool _compatible_ops; // Caaliph
	/* End Caaliph */
	/* GL 14/05/07 : Add Input and Output bitwidths and signeds */
	vector<long>			_inputsBitwidth;	//!< operator input widths
	vector<bool>			_inputsSigned;		//!< operator input signeds
	vector<long>			_outputsBitwidth;	//!< operator output widths
	vector<bool>			_outputsSigned;		//!< operator output signeds
	/* Fin GL*/
	/* 29/10/07 : add lines */
	long _chained;
	/* Fin GL */
	/* GL 07/01/07	: add lines */
	long _overcost;
	/* Fin GL */
	/* GL 20/12/08 : Complex operators */
	vector<long>			_input_begins;		//!< input-begin dates
	vector<long>			_output_begins;		//!< output-begin dates
	/* Fin GL */
	const OperatorInstance *_inst;	//!< pointer to operator instance
	int _index;
	// EJ 07/07/2008 : levinson
	bool			_port_number;	//!< operation is mem_read or mem_write (dynamic memory), number of input ports
	// End EJ

public:
	//! Create a new Operation node
	//! @param Input. name is the unique Control node name.
	//! @param Input. f is a pointer to the function.
	//!
	//! All private members are initialized to 0. start time = -1. locked are false.
	Operation(const string &name, const string &function_name) :
			CDFGnode(name, OPERATION)/*, _function_name(function_name)*/
	{
		_function_name = function_name;
		_asap = _alap = _cycles = _length = 0;
		_asap_locked = _alap_locked = false;
		//_addr = 0;
		_start = -1;
		_inst = 0;	// no instance when not scheduled
		/* 29/10/07 : add lines */
		_chained = false;
		/* Fin GL */
		/* GL 07/01/07	: add lines */
		_overcost = 0;
		/* Fin GL */
		_port_number = 0; // EJ 07/07/2008 : levinson
#ifdef CHECK
		operation_create++;	// debug
#endif
	}
	~Operation()
	{
#ifdef CHECK
		operation_delete++;	// debug
#endif
	}
	vector<const Operation *> compatiblewith;
	// set info
	//! Set start time.
	void start(long start)
	{
		_start = start;
	}

	// EJ 07/07/2008 : levinson
	//! if != 0, operation is mem_read or mem_write (dynamic memory) : number of ports
	void port_number(long n)
	{
		_port_number = n;
	}
	bool port_number() const
	{
		return _port_number;
	}
	// End EJ

	/* Caaliph 27/06/2007 */
	//! Set operator identification
	void identify(long identify)
	{
		_identify = identify;
	}
	//! Set operator identification
	void mode(long mode)
	{
		_mode = mode;
	}
	void delay(long delay)
	{
		_delay= delay;
	}
	void compatible_ops(bool comp)
	{
		_compatible_ops = comp;
	}
	void name_oper(string name_oper)
	{
		_name_operator = name_oper;
	}
	string name_oper() const
	{
		return _name_operator;
	}
	/* End Caaliph */
	//! Set asap time.
	void asap(long asap)
	{
		_asap = asap;
	}
	//! Set alap time.
	void alap(long alap)
	{
		_alap = alap;
	}
	//! Set asap locked status.
	void asap_locked(bool b)
	{
		_asap_locked = b;
	}
	//! Set alap locked status.
	void alap_locked(bool b)
	{
		_alap_locked = b;
	}
	//! Set number of cycles for a given selection.
	//! @param INput. sos is a pointer to a SelectionOutRefs (a list of smart pointers to SelectionOut).
	void cycles(const SelectionOutRefs *sos,long clk_period,long memory_access_time)
	{
		for (SelectionOutRefs::const_iterator it = sos->begin(); it != sos->end(); it++)
		{
			const SelectionOut *so = (*it).second;
			if (so->func() == _function)  	// if this is me
			{
				_cycles = so->cycles();	// the selected cycles are mine
				_length = so->length();	// the selected length is mine too
				/* GL 20/12/08 : Complex operators */
				_input_begins.clear();_output_begins.clear();
				int i;
				const Operator *o = so->oper();  
				for (i=0; i<o->input_begins().size(); i++)
					_input_begins.push_back(o->input_begins()[i]);
				for (i=0; i<o->output_begins().size(); i++)
					_output_begins.push_back(o->output_begins()[i]);
				/* Fin GL */
				//DH: 12/11/2008
				//rajout de mem_access pour les fonctions mem_read, mem_write: lib 2 cycles !!!
				//mem_write = 1 cycle (sortie adresse dynamique sur le bus adresse/ data sur le bus de donnée)  + 1 cycle (decodage banc/adresse) +  mem_access 
				//mem_access (transfert mémoire avec gel des bancs concernés): ecriture de la dpram synchrone
				//mem_read = 1 cycle (sortie adresse dynamique sur le bus adresse)+ 1 cycle (decodage banc/adresse) + mem_access (transfert mémoire/data sur le bus de donnée)
				//avec gel des bancs concernés): lecture de la dpram asynchrone
				if ((so->func()->symbolic_function()=="mem_read") ||
					(so->func()->symbolic_function()=="mem_write"))
				{
					_cycles += memory_access_time/clk_period;
					_length += memory_access_time ;
				}
				return;						// can stop now
			}
		}
	}
	//! Set operator instance
	void inst(const OperatorInstance *inst)
	{
		_inst = inst;
	}
	// get info
	//! Get start time.
	long start() const
	{
		return _start;
	}
	/* Caaliph 27/06/2007 */
	long identify() const
	{
		return _identify;
	}
	long mode() const
	{
		return _mode;
	}
	long delay() const
	{
		return _delay;
	}
	long delay_inc()
	{
		return _delay++;
	}
	bool compatible_ops() const
	{
		return _compatible_ops;
	}
	/* End Caaliph */
	//! Get asap time.
	long asap() const
	{
		return _asap;
	}
	//! Get alap time.
	long alap() const
	{
		return _alap;
	}
	//! Get asap locked status.
	bool asap_locked() const
	{
		return _asap_locked;
	}
	//! Get alap locked status.
	bool alap_locked() const
	{
		return _alap_locked;
	}
	//! Get number of cycles.
	long cycles() const
	{
		return _cycles;
	}
	//! Get length in time units.
	long length() const
	{
		return _length;
	}
	/* GL 20/12/08 : Complex operators */
	long output_length(int i, long clock) const
	{
		return _output_begins[i] * clock ;
	}
	long input_length(int i, long clock) const
	{
		return _input_begins[i] * clock ;
	}
	/* Fin GL */
	//! Get function.
	const Function * function() const
	{
		return _function;
	}
	//! Set function.
	void function(Function *function)
	{
		_function = function;
	}
	//! Get function_name
	const string & function_name() const
	{
		return _function_name;
	}
	/* KT 05/05/2008 */
	//! get index
	int index()
	{
		return _index;
	}
	//! set index
	void index (int index)
	{
		_index = index;
	}
	/*  End KT */
	/* GL 03/05/07	: symbolic function */
	//! Get symbolic function
	const string sym_function() const
	{
		return _function->symbolic_function();
	}
	/* Fin GL */
	/* GL 14/05/2007 :	Add fllowing lines */
	long inputsBitwidth(long i) const
	{
		return _inputsBitwidth[i];
	};				//!< Get inputBitwidth by pos.

	vector<long> inputsBitwidth() const
	{
		return _inputsBitwidth;
	};

	bool inputsSigned(long i) const
	{
		return _inputsSigned[i];
	};					//!< Get inputSigned by pos.
	long outputsBitwidth(long i) const
	{
		return _outputsBitwidth[i];
	};				//!< Get outputBitwidth by pos.
	bool outputsSigned(long i) const
	{
		return _outputsSigned[i];
	};					//!< Get outputSigned by pos.

	void inputsBitwidth(long i, long width)
	{
		_inputsBitwidth[i]=width;
	};	//!< Set inputBitwidth by pos.
	void inputsSigned(long i, bool signe)
	{
		_inputsSigned[i]=signe;
	};		//!< Set inputBitwidth by pos.
	void outputsBitwidth(long i, long width)
	{
		_outputsBitwidth[i]=width;
	};	//!< Set inputBitwidth by pos.
	void outputsSigned(long i, bool signe)
	{
		_outputsSigned[i]=signe;
	};		//!< Set inputBitwidth by pos.
	long sumOfwidths() const
	{
		long sum=0;
		for (int i =0; i<_inputsBitwidth.size();i++) sum += inputsBitwidth(i);
		return sum;
	}

	void initWidths()
	{
		int i;
		for (i = 0; i < predecessors.size() ;i++)
		{
			_inputsBitwidth.push_back(0);
			_inputsSigned.push_back(false);
		}
		for (i = 0; i< successors.size() ;i++)
		{
			_outputsBitwidth.push_back(0);
			_outputsSigned.push_back(false);
		}

	}
	/* Fin GL */
	/* GL 10/10/07	: add lines (fixed point)*/
	//! check if it's the largest operand
	bool isTheLargest(const Data *d) const
	{
		long max = 0;
		for (int i =0; i<_inputsBitwidth.size(); i++) if (_inputsBitwidth[i]> max) max = _inputsBitwidth[i];
		return (d->bitwidth() == max);
	}
	/* Fin GL */
	/* 29/10/07 : add lines */
	//! Returns chaining information
	long isChained() const
	{
		return _chained;
	}
	void chained(long c)
	{
		_chained = c;
	}
	//! Get length when chaining is performed in time units.
	//TO_VERIFY
	long chainingLength() const
	{
		if (_chained == -1) 
		{
		long l = 0;
			vector<const Operation *> successors;
			get_operations_successors((CDFGnode *)this, &successors);
			for (int i = 0; i < successors.size(); i++)
		{
				const Operation *pre = successors.at(i);
				if ((pre->isChained()==1) && (pre->length()> l))
					l = pre->length();
		}
		return (l+_length);
	}
		else
			return _length;
	}
	/* Fin GL */
	/* GL 07/01/07	: add lines */
	//! compute an operation overcost
	void computeOvercost();
	//! Get an operation overcost
	long overcost() const
	{
		return _overcost;
	};
	/* Fin GL */

	//! Get operator instance.
	const OperatorInstance * inst() const
	{
		return _inst;
	}

	// dumps
	//! Print info.
	void info() const;
	//! Serialize.
	void serialize(ofstream &f) const
	{
		int i;
		f << "operation(" << name() << ") {" << "\n";
		f << "  function " << function()->name() << ";" << "\n";
		f << "  read ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			f << predecessors[i]->source->name();
		}
		f << ";" << "\n";
		f << "  read_map ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getInputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  write ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			f << successors[i]->target->name();
		}
		f << ";" << "\n";
		f << "  write_map ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getOutputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  start " << start() << ";" << "\n";
		/* Caaliph: Mis en commentaire */
		/*f << "}" << endl; */
		/* End Caaliph */
	}
	/* Caaliph 05/07/2007 */
	void serialize_STAR(ofstream &f, Cadency *cadency) const
	{
		int i;
		double stage_op, stage_data;
		f << "operation(" << name() << ") {" << "\n";
		f << "  function " << function()->name() << ";" << "\n";
		f << "  read ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			if (predecessors[i]->source->type()==DATA)
			{
				if (((Data*)(predecessors[i]->source))->duplication()>1)
				{
					stage_op=floor((double)start()/(double)cadency->length());
					stage_data=floor((double)((Data*)(predecessors[i]->source))->start()/(double)cadency->length());
					f << predecessors[i]->source->name()<<"_"<<stage_op-stage_data;
				}
				else
					f << predecessors[i]->source->name();
			}
			else f << predecessors[i]->source->name();
		}
		f << ";" << "\n";
		f << "  read_map ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getInputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  write ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			if (successors[i]->target->type()==DATA)
			{
				if (((Data*)(successors[i]->target))->duplication()>1)
				{
					f << successors[i]->target->name()<<"_"<<0;
				}
				else
					f << successors[i]->target->name();
			}
			else f << successors[i]->target->name();
		}
		f << ";" << "\n";
		f << "  write_map ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getOutputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  real_start " << start() << ";" << "\n";
		f << "  start " << start()%cadency->length() << ";" << "\n";
		/* Caaliph 27/06/2007 mise en commentaire
		de la ligne suivante */
		//f << "}" << endl; // Caaliph
		/* End Caaliph */
	}


	void serialize(ofstream &f, const string &mode) const
	{
		int i;
		vector<CDFGnode *> node_list;

		f << "operation(" << name() << ") {" << "\n";
		f << "  function " << function()->name() << ";" << "\n";
		f << "  read ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			f << predecessors[i]->source->name();
		}
		f << ";" << "\n";
		f << "  read_map ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getInputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  write ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			f << successors[i]->target->name();
		}
		f << ";" << "\n";
		f << "  write_map ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getOutputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  start " << start() << ";" << "\n";
		//f << "}" << endl; // add by Caaliph
	}

	void serialize(ofstream &f, vector<const CDFGnode*> *v, const string &mode) const
	{
		int i;
		vector<CDFGnode *> node_list;

		f << "operation(" << name() << ") {" << "\n";
		f << "  function " << function()->name() << ";" << "\n";
		f << "  read ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			f << predecessors[i]->source->name();
		}
		f << ";" << "\n";
		f << "  read_map ";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getInputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  write ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			f << successors[i]->target->name();
		}
		f << ";" << "\n";
		f << "  write_map ";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << ", ";
			const Port *p = function()->getOutputPortByPos(i);
			f << p->name();
		}
		f << ";" << "\n";
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  start " << start() << ";" << "\n";
		//f << "}" << endl;
	}
	/* End Caaliph */

	//DH: 12/11/2008
	bool hasAOutputSuccessor(const CDFG *cdfg) const
	{
		for (int i = 0; i < successors.size(); i++)
		{
			CDFGnode *succecessor = successors[i]->target;
			if (succecessor->isAPredecessorOf(cdfg->sink()))
				return true;
		}
		return false;
	}

	//DH 11/03/2009
	int getInputPos(const Data *input) const
	{
		int result=-1;
		int i;
		for (i = 0; (i < predecessors.size()) && (result==-1); i++)
		{
			CDFGnode *predecessor= predecessors[i]->source;
			if ((predecessor->type()==DATA) && (((Data*)predecessor)==input))
				result=i;
		}
		return result;
	}
	int getInputPosDFF(const Data *input) const
	{
		int result=-1;
		int i;
		for (i = 0; (i < predecessors.size()) && (result==-1); i++)
		{
			CDFGnode *predecessor= predecessors[i]->source;
			if ((predecessor->type()==DATA)) {
				if (((Data*)predecessor)==input) {
					result=i;
				} else if (predecessor->predecessors.size()==1) {
					predecessor = predecessor->predecessors[0]->source;
					if ((predecessor->type()==DATA) && ((Data*)predecessor)==input){
						result = i;
					}
				}
			}
		}
		return result;
	}

	int getOutputPos(const Data *output) const
	{
		int result=-1;
		int i;
		for (i = 0; (i < successors.size()) && (result==-1); i++)
		{
			CDFGnode *successor= successors[i]->target;
			if ((successor->type()==DATA) && (((Data*)successor)==output))
				result=i;
		}
		return result;

	}
	
	
		//! Get function_signature for cdfg2c generation
	string function_signature(const CDFG *cdfg) const
	{

		ostringstream f;
		int i;

		f << function_name() << "_";
		for (i = 0; i < predecessors.size(); i++)
		{
			if (i) f << "_";
			CDFGnode *predecessor= predecessors[i]->source;
			if (predecessor->type()==DATA)
				f << ((Data*)predecessor)->get_signature_type();
		}
		f << "_";
		for (i = 0; i < successors.size(); i++)
		{
			if (i) f << "_";
			CDFGnode *succecessor = successors[i]->target;
			if (succecessor->type()==DATA)
				f << ((Data*)succecessor)->get_signature_type();
		}

		return f.str();
	}

};

#endif // __OPERATION_H__
