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

//	File:		operator.h
//	Purpose:	operators
//	Author:		Pierre Bomel, LESTER, UBS

class Operator;
class OperatorRef;
class OperatorRefs;
class ImplementedFunction;
class ImplementedFunctionRef;
class ImplementedFunctionRefs;
/* GL 24/06/08 : FsmCode */
class FsmCodes;
class FsmCodesRef;
class FsmCodesRefs;
/* Fin GL */

#ifndef __OPERATOR_H__
#define __OPERATOR_H__

#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include "map.h"

//! Smart pointer to an ImplementedFunction.
class ImplementedFunctionRef : public Ref<ImplementedFunction>
{
public:
	ImplementedFunctionRef(ImplementedFunction * ifunc = 0) : Ref<ImplementedFunction>(ifunc) {}
};
//! List of smart pointers to ImplementedFunctions.
class ImplementedFunctionRefs : public vector<ImplementedFunctionRef>
{
public:
	//! Add a new implemented function.
	void add(const ImplementedFunctionRef ifunc)
	{
		push_back(ifunc);
	}
	//! Print info.
	void info() const;
	//! Serilaize into file.
	void serialize(ofstream &f) const;
};

//! Smart pointer to an Operator.
class OperatorRef  : public MapRef<Operator>
{
public:
	OperatorRef(Operator *f) : MapRef<Operator>(f) {}
};
//! List of smart pointers to Operators.
class OperatorRefs : public MapRefs<Operator, OperatorRef> {};

#include "clock.h"
#include "cycles.h"
#include "function.h"
#include "os_tools.h"
#include "port.h"

//! Implemented Functions.
class ImplementedFunction
{
private:
	const long			_propagation_time;	//!< propagation time expressed in default time units
	const long			_power;				//!< power expressed in power units
	const Function*		_function;			//!< pointer to a Function
public:
	//! Create a new ImplementedFunction
	//! @param Input. propagation_time is the propagation time.
	//! @param Input. power is the power.
	//! @param Input. function is a pointer to the function implemented.
	ImplementedFunction(long propagation_time, long power, const Function *function) :
			_propagation_time(propagation_time),
			_power(power),
			_function(function)
	{}
	//! Print info.
	void info() const;
	//! Serialize into file.
	void serialize(ofstream &f) const;
	//! Get propagation time.
	long propagation_time() const
	{
		return _propagation_time;
	}
	//! Get power.
	long power() const
	{
		return _power;
	}
	//! Get implemented function.
	const Function * function() const
	{
		return _function;
	}
};

/* GL 24/06/08 : FsmCode */
//! Smart pointer to an FsmCodes.
class FsmCodesRef : public Ref<FsmCodes>
{
public:
	FsmCodesRef(FsmCodes * icodes = 0) : Ref<FsmCodes>(icodes) {}
};
//! List of smart pointers to FsmCodes.
class FsmCodesRefs : public vector<FsmCodesRef>
{
public:
	//! Add a new FSM code list.
	void add(const FsmCodesRef icodes)
	{
		push_back(icodes);
	}
	//! Print info.
	void info() const;
	//! Serilaize into file.
	void serialize(ofstream &f) const;
};
#include "function.h"
#include "os_tools.h"
//! FSM codes.
class FsmCodes
{
private:
	const vector <string>		_codes;				//!< list of FSM codes per cycle
	const Function*		_function;			//!< pointer to a Function
public:
	//! Create a new FsmCodes
	//! @param Input. codes are the FSM codes per cycle.
	//! @param Input. function is a pointer to the function implemented.
	FsmCodes(const Function *function, const vector<string> codes) :
			_codes(codes),
			_function(function)
	{}
	//! Print info.
	void info() const;
	//! Serialize into file.
	void serialize(ofstream &f) const;
	//! Get implemented function.
	const Function * function() const
	{
		return _function;
	}
	//! Give current code.
	string give_code(long cycle) const { return _codes[cycle];}

};
/* Fin GL */

//! Operators.
class Operator : public MapID
{
public:
	//! local type to distinguish reset polarities
	enum Polarity
	{
		HIGH,								//!< active high '1' reset
		LOW									//!< active low  '0' reset
	};
private:
	// these informations are coming from the library file
	const long				_area;				//!< area
	ImplementedFunctionRefs	_functions;			//!< implemented function
	const string			_component;			//!< component name
	const bool				_synchronous;		//!< true for synchronous, false for combinatorial
	const long				_cadency;			//!< cadency, for synchronous ops only
	const long				_latency;			//!< latency, for synchronous ops only
	long				_pipelineStages;	//!< pipeline stages, for synchronous ops only
	const Polarity			_polarity;			//!< reset polarity
	const string			_rst_name;			//!< reset port name
	const string			_clk_name;			//!< clock port name
	const string			_ctrl_name;			//!< control port name
	/* GL 18/06/08 */
	const Polarity			_e_polarity;		//!< enable polarity
	const string			_enable_name;		//!< enable port name
	/* Fin GL */
	/* GL 22/10/08 */
	string					_command_name;		//!< command port name for operators with external FSM
	long					_command_size;		//!< command port size for operators with external FSM
	/* Fin GL */
	vector<const Port *>	_in_ports;			//!< input ports, stored in the function declaration order
	vector<const Port *>	_out_ports;			//!< output ports, stored in the function declaration order
	vector<long>			_ctrl_codes;		//!< control codes for the function list
	bool					_passThrough;		//!< operator implement no hardware
	/* GL 18/06/08 :  synchronous without fsm operators */
	bool					_noFSM;				//!< operator has no internal FSM control
	/* Fin GL */
	/* GL 14/05/07 : Add Input and Output bitwidths and signeds */
	vector<long>			_inputsBitwidth;		//!< operator input widths
	vector<bool>			_inputsSigned;		//!< operator input signeds
	vector<long>			_outputsBitwidth;		//!< operator output widths
	vector<bool>			_outputsSigned;		//!< operator output signeds
	/* Fin GL*/
	/* GL 25/10/07 : combinational_delay */
	vector<long>			_combinational_delay;//!< combinatorial delay vector
	/* Fin GL */
	/* GL 20/11/07 */
	vector<string>			_functionList;		//!> operator function list according to library
	/* Fin GL */
	/* GL 24/06/08 */
	FsmCodesRefs			_FSM_codesList;		//!> operator FSM codes list according to library
	/* Fin GL */
	/* GL 21/10/08 : clock ration (divided by) */
	int						_clock_ratio;		//!> opertor clock ratio
	/* Fin GL */
	/* GL 05/12/08 : Automization of gals & lis generation */				
	bool					_synCOM;			//!> operator communication description Synchronous/Asynchronous
	/* Fin GL */
	/* GL 20/12/08 : Complex operators */
	vector<long> _input_begins;					//!> operator input chronograms
	vector<long> _output_begins;				//!> operator output chronograms
	/* Fin GL*/
	/* GL 10/06/09 : minimum perid */
	int						_minimum_period;	//!> opertor minimum period
	/* Fin GL */
public:
	void *_addr;								//!< for extensions
	//! Create new operator.
	//! @param Input. name is the unique operator name.
	//! @param Input. area is the area.
	//! @param Input. synchronous, true for a synchronous operator, false for a combinatorial.
	//! @param Input. cadency is the cadency.
	//! @param Input. latency is the latency.
	//! @param Input. pol is the reset polarity.
	//! @param Input. rst_name is the name of the reset port.
	//! @param Input. clk_name is the name of the clock port.
	//! @param Input. ctrl_name is the name of the control port.
	Operator(const string & name, const string & component, long area, bool synchronous, long cadency, long latency,
	         Polarity pol, const string rst_name, const string clk_name, const string ctrl_name
	        ) :
			MapID(name),
			_component(component),
			_area(area),
			_synchronous(synchronous),
			_cadency(cadency),
			_latency(latency),
			_polarity(pol),
			_rst_name(rst_name),
			/* GL 18/06/08 */
			_enable_name("enable"),
			_e_polarity(HIGH),
			/* Fin GL */
			_clk_name(clk_name),
			_ctrl_name(ctrl_name)
	{
		/* GL 14/05/07 : Add Input and Output bitwidths and signeds
						 16 is default bitwidth and 0 default signed */
		if (cadency !=0)
		{
			/* DEBUG_DH cout << cadency << endl;
			cout << latency << endl; FIN DEBUG_DH */
			_pipelineStages=(long)ceil((double)latency/(double)cadency);
		}
		else
			_pipelineStages=1;


		_in_ports.clear();
		_out_ports.clear();
		_inputsBitwidth.assign(inputPorts(), 16);
		_inputsSigned.assign(inputPorts(), 0);
		_outputsBitwidth.assign(outputPorts(), 16);
		_outputsSigned.assign(outputPorts(), 0);
		/* Fin GL*/
		_addr = 0;
		_passThrough = false;
		/* GL 18/06/08 :  synchronous without fsm operators */
		_noFSM = false;
		/* Fin GL */
		/* GL 05/12/08 : Automization of gals & lis generation */				
		_synCOM = true;
		/* Fin GL */
		/* GL 21/10/08 : clock ration (divided by) */
		_clock_ratio = 1;
		/* Fin GL */
		/* GL 20/12/08 : Complex operators */
		_input_begins.assign(inputPorts(),0);
		_output_begins.assign(outputPorts(),latency);
		/* Fin GL*/
		/* GL 10/06/09 : minimum period (ns)*/
		_minimum_period = 10;
		/* Fin GL*/
#ifdef CHECK
		operator_create++;	// debug
#endif
	}
	/* GL 14/05/07 : to build operator width bitwidths and sogneds info */
	Operator(const string & name, const string & component, long area, bool synchronous, long cadency, long latency,
	         Polarity pol, const string rst_name, const string clk_name, const string ctrl_name,
	         /* GL 18/06/08 */
	         Polarity e_pol, const string enable_name,
	         /* Fin GL */
	         vector<long> inputsBitwidth, vector<bool> inputsSigned, vector<long> outputsBitwidth, vector<bool> outputsSigned
	        ) :
			MapID(name),
			_component(component),
			_area(area),
			_synchronous(synchronous),
			_cadency(cadency),
			_latency(latency),
			_polarity(pol),
			_rst_name(rst_name),
			/* GL 18/06/08 */
			_enable_name(enable_name),
			_e_polarity(e_pol),
			/* Fin GL */
			_clk_name(clk_name),
			_ctrl_name(ctrl_name)

	{
		
				/*DEBUG_DH cout << cadency << endl;
		cout << latency << endl; FIN DEBUG_DH */
		if (cadency !=0)
			_pipelineStages=(long)ceil((double)latency/(double)cadency);
		else 
			_pipelineStages=1;

		_inputsBitwidth.assign(inputsBitwidth.begin(), inputsBitwidth.end());
		_inputsSigned.assign(inputsSigned.begin(), inputsSigned.end());
		_outputsBitwidth.assign(outputsBitwidth.begin(), outputsBitwidth.end());
		_outputsSigned.assign(outputsSigned.begin(), outputsSigned.end());
		_addr = 0;
		_passThrough = false;
		/* GL 18/06/08 :  synchronous without fsm operators */
		_noFSM = false;
		/* Fin GL */
		/* GL 05/12/08 : Automization of gals & lis generation */				
		_synCOM = true;
		/* Fin GL */
		/* GL 21/10/08 : clock ration (divided by) */
		_clock_ratio = 1;
		/* Fin GL */
		/* GL 20/12/08 : Complex operators */
		_input_begins.assign(inputPorts(),0);
		_output_begins.assign(outputPorts(),latency);
		/* Fin GL*/
		/* GL 10/06/09 : minimum period (ns)*/
		_minimum_period = 10;
		/* Fin GL*/

#ifdef CHECK
		operator_create++;	// debug
#endif
	}
	/* Fin GL*/

	/* GL 27/04/2007 :	Add fllowing lines
						to build a copy of an operator with another name */
	Operator(const string & name, const string &compName, Operator *opr
	        ) :
			MapID(name),
			_component(compName),
			// must change it later with true area
			_area(opr->_area),
			_synchronous(opr->_synchronous),
			_cadency(opr->_cadency),
			_latency(opr->_latency),
			_pipelineStages(opr->_pipelineStages),
			_polarity(opr->_polarity),
			_rst_name(opr->_rst_name),
			/* GL 18/06/08 */
			_enable_name(opr->_enable_name),
			_e_polarity(opr->_e_polarity),
			/* Fin GL */
			_clk_name(opr->_clk_name),
			_ctrl_name(opr->_ctrl_name)

	{
		_functions = opr->functions();
		_in_ports.resize(opr->inputPorts());
		_out_ports.resize(opr->outputPorts());
		_addr = 0;
		_passThrough = false;
		/* GL 18/06/08 :  synchronous without fsm operators */
		_noFSM = opr->noFSM();	
		/* Fin GL */
		/* GL 20/11/07 */
		_ctrl_codes = (vector<long> ) *opr->getCodes();
		_functionList = (vector<string> ) *opr->getFunctions();
		/* Fin GL */
		/* GL 05/12/08 : Automization of gals & lis generation */				
		_synCOM = opr->synCOM();
		/* Fin GL */
		/* GL 21/10/08 : clock ration (divided by) */
		_clock_ratio = opr->clock_ratio();
		/* Fin GL */
		/* GL 20/12/08 : Complex operators */
		_input_begins.assign(*opr->input_begins().begin(), *opr->input_begins().end());
		_output_begins.assign(*opr->output_begins().begin(), *opr->output_begins().end());
		/* Fin GL*/
		/* GL 10/06/09 : minimum period (ns)*/
		_minimum_period = opr->minimum_period();
		/* Fin GL*/
#ifdef CHECK
		operator_create++;	// debug
#endif
	}

	/* Fin GL */

	~Operator();
	//! Get component name
	string component() const
	{
		return _component;
	}
	//! Get polarity
	Polarity rst_polarity() const
	{
		return _polarity;
	}
	//! Get reset port name
	string rst_name() const
	{
		return _rst_name;
	}
	/* GL 18/06/08 */
	//! Get enable polarity
	Polarity enable_polarity() const
	{
		return _e_polarity;
	}
	//! Get enable port name
	string enable_name() const
	{
		return _enable_name;
	}
	/* Fin GL */
	//! Get clock port name
	string clk_name() const
	{
		return _clk_name;
	}
	//! Get control port name
	string ctrl_name() const
	{
		return _ctrl_name;
	}
	//! Get an input port bound to a symbolic port
	//! @param Input. pos is the declaration position of the symbolic port (the function port).
	const Port * getInputPortBoundTo(long pos) const
	{
		return _in_ports[pos];
	}
	//! Get an output port bound to a symbolic port
	//! @param Input. pos is the declaration position of the symbolic port (the function port).
	const Port * getOutputPortBoundTo(long pos) const
	{
		return _out_ports[pos];
	}
	//! Add a port
	//! @param Input. i is the function's symbolic port position.
	//! @param Input. p is a pointer to an operator port.
	void addPort(long i, const Port *p);
	//! Resize input ports vector
	void resizeInputPorts(long size)
	{
		_in_ports.resize(size);
	}
	//! Get number of input ports
	long inputPorts() const
	{
		return _in_ports.size();
	}
	//! Get an input port by position
	const Port *getInputPort(long i) const
	{
		return _in_ports[i];
	}
	
	//! Get number of output ports
	long outputPorts() const
	{
		return _out_ports.size();
	}
	

	//! Get an output port by position
	const Port *getOutputPort(long i) const
	{
		return _out_ports[i];
	}
	//! Resize output ports vector
	void resizeOutputPorts(long size)
	{
		_out_ports.resize(size);
	}
	//! Resize control codes vector
	void resizeCtrlCodes(long size)
	{
		_ctrl_codes.resize(size);
	}
	//! Set a control code for i-th function (0..N-1)
	//! @param Input. i is the function/code index.
	//! @param Input. code is the value of the code for the operator to execute the i-th function.
	void setCtrlCode(long i, long code)
	{
		if ((i < 0) || (i >= _ctrl_codes.size()))
		{
			cerr << "Internal error: out of bound access to control codes" << endl;
			exit(1);
		}
		_ctrl_codes[i] = code;
	}
	//! Get a control code for i-th function
	//! @param Input. i is the function/code index.
	long getCtrlCode(long i) const
	{
		return _ctrl_codes[i];
	}
	//! Get access to the control codes vector
	const vector<long> * getCodes() const
	{
		return &_ctrl_codes;
	}
	//! Get area.
	long area() const
	{
		return _area;
	}
	//! Get list of smart pointers to implemented functions.
	ImplementedFunctionRefs & functions()
	{
		return _functions;
	}
	//! Print info.
	void info() const;
	//! Serialize into a file.
	void serialize(ofstream &f) const;
	// propagation time for mapped operators
	//! Get propagation time.
	//! @param Input. f is a pointer to the implemented function.
	//!
	//! @result a time in tu.
	long propagation_time(const Function *f) const;
	//! Get number of pipeline stages for synchronous operators.
	long pipelineStages() const
	{
		return _pipelineStages;
	}
	//! Check if operator is pipelined or not.
	bool pipeline() const
	{
		return (pipelineStages() > 1);
	}
	//! Get cadency for pipeline operators.
	long cadency() const
	{
		return _cadency;
	}
	//! Get latency for pipeline operators.
	long latency() const
	{
		return _latency;
	}
	//! Searches if an operator implements a function.
	//! @param Input. f is the function to search for.
	//! @result a boolean, true = operator implements function "f", false otherwise.
	bool implements(const Function *f, const ImplementedFunction **implf) const
	{
		for (ImplementedFunctionRefs::const_iterator it = _functions.begin(); it != _functions.end(); it++)
		{
			const ImplementedFunction *imf = *it;
			if (imf->function() == f)
			{
				*implf = imf;
				return true;
			}
		}
		return false;
	}
	//! Searches if an operator implements a function defined by is name
	bool implements(const string &function_name) const;
	//! Add an implemented function to an operator.
	//! @param Input. f is a pointer to the implemented function to add.
	bool implements(const Function *f) const
	{
		const ImplementedFunction *imf;
		return implements(f, &imf);
	}
	//! Check if the operator is a mono-function one.
	bool mono() const
	{
		return _functions.size() == 1;    // tell if the operator is a single function one
	}
	//! Checks if operator is a mono-function one. Same than mono().
	bool isMonoFunction() const
	{
		return mono();
	}
	//! Checks if operator is a multi-functions one.
	bool isMultiFunction() const
	{
		return !mono();
	}
	//! Checks if two operators are compatible when implementing a given function.
	//! @param Input. o is a pointer to the other operator.
	//! @param Input. f is a pointer to the implemented function.
	//! @param Input. clk is a pointer to the system clock.
	bool compatible(const Operator *o, const Function *f, const Clock *clk) const;
	//! Check if operator is a "passthrough" one.
	//! An operator is a "pass-throug" one if all its implemented
	//! function gave a null propagation time.
	bool passThrough() const
	{
		return _passThrough;
	}
	//! function gave a null propagation time.
	void passThrough(bool passThrough)
	{
		_passThrough = passThrough;
	}
	/* GL 18/06/08 :  synchronous without fsm operators */
	//! Make an operator with external FSM
	void noFSM(bool noFSM)
	{
		_noFSM = noFSM;
	}
	//! Check if operator is defined without FSM
	bool noFSM() const
	{
		return _noFSM;
	}
	/* Fin GL */
	//! Check if operator is synchronous
	bool synchronous() const
	{
		return _synchronous;
	}
	//! Check if operator is asynchronous
	bool asynchronous() const
	{
		return !_synchronous;
	}
	/* GL 27/04/2007 :	Add fllowing lines */
	//! Copy of operator items with newlocation and new name
	Operator * copyOf(string name, string compName);
	/* Fin GL */
	/* GL 14/05/2007 :	Add fllowing lines */
	//! Get inputs bitwidth
	vector<long> inputsBitwidth()
	{
		return _inputsBitwidth;
	};
	//! Get inputs Signed
	vector<bool> inputsSigned()
	{
		return _inputsSigned;
	};
	//! Get outputs bitwidth
	vector<long> outputsBitwidth()
	{
		return _outputsBitwidth;
	};
	//! Get outputs Signed
	vector<bool> outputsSigned()
	{
		return _outputsSigned;
	};
	/* Fin GL */
	/* GL 16/07/07 */
	//! Get sumof operator's widths
	long sumOfwidths()
	{
		long sum=0;
		for (int i =0; i<inputPorts();i++) sum += inputsBitwidth()[i];
		return sum;
	};
	/* Fin GL */
	/* GL 25/10/07 : combinational_delay */
	//! Get function combinational delay.
	long getCombinationalDelay(const Function *f) const ;
	//! Set function combinational delay.
	void setCombinationalDelay(long i, long d)
	{
		_combinational_delay[i] = d;
	};
	//! Resize combinational delay vector .
	void resizeCombinationalDelay(long size)
	{
		_combinational_delay.resize(size);
	}
	/* Fin GL */
	/* GL 20/11/07 */
	//! Get access to the function list vector
	const vector<string> * getFunctions() const
	{
		return &_functionList;
	}
	//! Get function ctrlCode
	long getCtrlCode(string function_name) const
	{
		int i = 0;
		while ((function_name != getFunctionList(i)) && (i<_functionList.size())) i++;
		return getCtrlCode(i);
	};
	//! Get function list
	string getFunctionList(long i) const
	{
		return _functionList[i];
	};
	//! Set function list
	void setFunctionList(long i, string function_name)
	{
		_functionList[i] = function_name;
	};
	//! Resize function list
	void resizeFunctionList(long size)
	{
		_functionList.resize(size);
	}
	/* Fin GL */

	/* GL 24/06/08 */
	string give_code(const Function *f, long cycle) const
	{
		for (FsmCodesRefs::const_iterator it = _FSM_codesList.begin(); it != _FSM_codesList.end(); it++)
		{
			const FsmCodes *fc = *it;
			if (fc->function() == f)
				return (fc)->give_code(cycle);
		}
		return "error";
	}
	/* Fin GL */
	/* GL 22/10/08 */
	//! Set command port name
	void command_name(string cmd) {_command_name = cmd;} 
	//! Get command port name
	long command_size(void) const {return _command_size;} 
	//! Set command port size
	void command_size(long size) {_command_size = size;} 
	//! Get command port size
	string command_name(void) const {return _command_name;} 

	void addCode(const Function *f, const vector<string> codes) 
	{
		FsmCodes *fc = new FsmCodes(f, codes);
		const FsmCodesRef fc_ref(fc);
		_FSM_codesList.add(fc_ref);

	}
	/* Fin GL */
	/* GL 21/10/08 : clock ration (divided by) */
	void clock_ratio(int ratio){_clock_ratio = ratio;}
	int clock_ratio() const {return _clock_ratio;}
	/* Fin GL */
	/* GL 05/12/08 : Automization of gals & lis generation */				
	void synCOM(bool com){_synCOM = com;}
	bool synCOM() const {return _synCOM;}
	/* Fin GL */
	/* GL 20/12/08 : Complex operators */

	void input_begins(vector<long> inputBegins){
		_input_begins.assign(inputBegins.begin(), inputBegins.end());
	}
	void output_begins(vector<long> outputBegins){
		_output_begins.assign(outputBegins.begin(), outputBegins.end());
	}

	vector<long> input_begins() const{return _input_begins;}
	vector<long> output_begins() const{return _output_begins;}

	/* Fin GL*/
	/* GL 10/06/09 : minimum period (ns)*/
	void minimum_period(long period){_minimum_period = period;}
	long minimum_period(void) const{return _minimum_period;}
	/* Fin GL */

};

#endif // __OPERATOR_H__
