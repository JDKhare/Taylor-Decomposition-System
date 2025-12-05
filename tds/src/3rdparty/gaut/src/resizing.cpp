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

//	File:		resizing.h
//	Purpose:	Resizing Operator's instances A posteriori according to bitwidths
//	Author:		Ghizlane Lhairech, LESTER, UBS
//	Date:		06/04 /07

#include <iostream>
#include "scheduling.h"
#include "cdfg.h"
#include "parser.h"
#include "lib.h"
#include "graph.h"
#include "operation.h"
#include "port.h"
#include "data.h"
#include "control.h"
#include "resizing.h"

using namespace std;

//! Local storage for OI lists.
static OI_Lists oi_lists;
//! Local storage for widths computing
static vector<long> input_widths;
static vector<long> output_widths;
//! Local storage for widths computing
static vector<Port::Signed> input_signed;
static vector<Port::Signed> output_signed;
static vector<long> input_fixed;
static vector<long> output_fixed;


static long max(long a, long b)
{
	return (a>b? a : b);
}
//! Compute operator instance bitwidths : first approach.
//! @param Input. ops is a pointer to operations vector bound to this operator
//! @param Input. in is the input's number
//! @param Input. in is the output's number
//! Inputs bitwidth is the max of all inputs. The same for outputs

static void fixed_f1(vector<const Operation*> ops, long in, long out)
{

	long in_max = -999;
	long out_max = -999;
	long in_total_max = 0;
	long out_total_max = 0;

	input_signed.assign(in,Port::UFIXED);
	output_signed.assign(out,Port::UFIXED);
	for (int i=0; i<ops.size() ; i++)
	{
		for (int j=0; j<in; j++)
		{
			if (((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint()!=-999)
				in_max = max(in_max,((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint());
			if (((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint()!=-999)
				in_total_max = max(in_total_max,(((Data*)ops[i]->predecessors.at(j)->source)->bitwidth()-((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint()));
			else 
				in_total_max = max(in_total_max,(((Data*)ops[i]->predecessors.at(j)->source)->bitwidth()));
			if (((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "sfixed")
				input_signed[j] = Port::SFIXED;
		}
		for (int k=0; k<out; k++)
		{
			if (((Data *)ops[i]->successors.at(k)->target)->fixedPoint()!=-999)
				out_max = max(out_max,((Data *)ops[i]->successors.at(k)->target)->fixedPoint());
			if (((Data *)ops[i]->successors.at(k)->target)->fixedPoint()!=-999)
				out_total_max = max(out_total_max,(((Data *)ops[i]->successors.at(k)->target)->bitwidth()-((Data *)ops[i]->successors.at(k)->target)->fixedPoint()));
			else 
				out_total_max = max(out_total_max,(((Data *)ops[i]->successors.at(k)->target)->bitwidth()));
			if (((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "sfixed")
				output_signed[k] = Port::SFIXED;
		}
	}
	if (in_max!=-999)
		in_total_max += in_max;
	if (in_max!=-999)
		out_total_max += out_max;
	// vectors update
	input_fixed.assign(in,in_max);
	output_fixed.assign(out,out_max);
	input_widths.assign(in,in_total_max);
	output_widths.assign(out,out_total_max);
}
static void fixed_f2(vector<const Operation*> ops, long in, long out)
{

	input_signed.assign(in,Port::UFIXED);
	output_signed.assign(out,Port::UFIXED);
	vector<long> in_max(in,-999) ;
	vector<long> out_max(out,-999) ;
	vector<long> in_total_max(in,0) ;
	vector<long> out_total_max(out,0) ;
	for (int i=0; i<ops.size() ; i++)
	{
		for (int j=0; j<in; j++)
		{
			if (((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint()!=-999)
				in_max[j] = max(in_max[j],((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint());
			if (((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint()!=-999)
				in_total_max[j] = max(in_total_max[j],(((Data*)ops[i]->predecessors.at(j)->source)->bitwidth()-((Data*)ops[i]->predecessors.at(j)->source)->fixedPoint()));
			else
				in_total_max[j] = max(in_total_max[j],(((Data*)ops[i]->predecessors.at(j)->source)->bitwidth()));
			if (((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "sfixed")
				input_signed[j] = Port::SFIXED;
			//cout << " E" << j << " : " << ((Data*)ops[i]->predecessors.at(j)->source)->bitwidth();
		}
		for (int k=0; k<out; k++)
		{
			if (((Data *)ops[i]->successors.at(k)->target)->fixedPoint()!=-999)
				out_max[k] = max(out_max[k],((Data *)ops[i]->successors.at(k)->target)->fixedPoint());
			if (((Data *)ops[i]->successors.at(k)->target)->fixedPoint()!=-999)
				out_total_max[k] = max(out_total_max[k],(((Data *)ops[i]->successors.at(k)->target)->bitwidth()-((Data *)ops[i]->successors.at(k)->target)->fixedPoint()));
			else
				out_total_max[k] = max(out_total_max[k],(((Data *)ops[i]->successors.at(k)->target)->bitwidth()));
			if (((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "sfixed")
				output_signed[k] = Port::SFIXED;
			//cout << " O" << k << " : " << ((Data*)ops[i]->successors.at(k)->target)->bitwidth();
		}
	}
	int l;
	for ( l=0; l<in ; l++)
	{
		if (in_max[l]!=-999)
			in_total_max[l] = in_total_max[l] + in_max[l];
	}
	for ( l=0; l<out ; l++)
	{
		if (out_max[l]!=-999)
			out_total_max[l] = out_total_max[l] + out_max[l];
	}
	// vectors update
	input_fixed = in_max;
	output_fixed = out_max;
	input_widths = in_total_max ;
	output_widths = out_total_max ;
	//cout << endl;

}
static void f1(vector<const Operation*> ops, long in, long out)
{

	long in_max = 0;
	long out_max = 0;

	input_signed.assign(in,Port::UNSIGNED);
	output_signed.assign(out,Port::UNSIGNED);
	for (int i=0; i<ops.size() ; i++)
	{
		for (int j=0; j<in; j++)
		{
			in_max = max(in_max,((Data*)ops[i]->predecessors.at(j)->source)->bitwidth());
			if (((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "signed" || ((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "sfixed")
				input_signed[j] = Port::SIGNED;
		}
		for (int k=0; k<out; k++)
		{
			out_max = max(out_max,((Data *)ops[i]->successors.at(k)->target)->bitwidth());
			if (((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "signed" || ((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "sfixed")
				output_signed[k] = Port::SIGNED;
		}
	}
	// vectors update
	input_widths.assign(in,in_max);
	output_widths.assign(out,out_max);
}
//! Compute operator instance bitwidths : second approach.
//! @param Input. ops is a pointer to operations vector bound to this operator
//! @param Input. in is the input's number
//! @param Input. in is the output's number
//! Each input bitwidth is the max its inputs. The same for outputs
static void f2(vector<const Operation*> ops, long in, long out)
{

	input_signed.assign(in,Port::UNSIGNED);
	output_signed.assign(out,Port::UNSIGNED);
	vector<long> in_max(in,0) ;
	vector<long> out_max(out,0) ;
	for (int i=0; i<ops.size() ; i++)
	{
		for (int j=0; j<in; j++)
		{
			in_max[j] = max(in_max[j],((Data*)ops[i]->predecessors.at(j)->source)->bitwidth());
			if (((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "signed" || ((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "sfixed")
				input_signed[j] = Port::SIGNED;
			//cout << " E" << j << " : " << ((Data*)ops[i]->predecessors.at(j)->source)->bitwidth();
		}
		for (int k=0; k<out; k++)
		{
			out_max[k] = max(out_max[k],((Data *)ops[i]->successors.at(k)->target)->bitwidth());
			if (((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "signed" || ((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "sfixed")
				output_signed[k] = Port::SIGNED;
			//cout << " O" << k << " : " << ((Data*)ops[i]->successors.at(k)->target)->bitwidth();
		}
	}
	// vectors update
	input_widths = in_max;
	output_widths = out_max;
	//cout << endl;

}
//! Compute operator instance bitwidths : third approach.
//! @param Input. ops is a pointer to operations vector bound to this operator
//! @param Input. in is the input's number
//! @param Input. in is the output's number
//! Function AdHoc
static void f3(vector<const Operation*> ops, long in, long out)
{
	// to do
	/* GL 11/09/07 : replace line
	// Call f2 instead till actual function is develloped*/
	f2(ops, in, out);
	/* By
	vector<long> in_max;
	vector<long> out_max(out,0) ;
	input_signed.assign(in,Port::UNSIGNED);
	output_signed.assign(out,Port::UNSIGNED);
	for (int i=0; i<ops.size() ; i++){
		for (int j=0; j<in; j++){
			// sort all widths
			if (in_max.empty())
				in_max.push_back(((Data *)ops[i]->successors.at(j)->target)->bitwidth());
			else{
				int it = 0;
				while( it!= in_max.size()){
					if ( ((Data *)ops[i]->successors.at(j)->target)->bitwidth() > in_max[it] ) break;
					it++;
				}
				if (it == in_max.size())
					in_max.push_back( ((Data *)ops[i]->successors.at(j)->target)->bitwidth() );
				else
					in_max.insert(it, ((Data *)ops[i]->successors.at(j)->target)->bitwidth() );
			}
			if (((Data*)ops[i]->predecessors.at(j)->source)->getStringSigned()== "signed")
				input_signed[j] = Port::SIGNED;
		}
		for (int k=0; k<out; k++){
			out_max[k] = max(out_max[k],((Data *)ops[i]->successors.at(k)->target)->bitwidth());
			if (((Data*)ops[i]->successors.at(k)->target)->getStringSigned()== "signed")
				output_signed[k] = Port::SIGNED;
		}
	}
	// vectors update
	for (int index =0; index < in_max.size(); index+=ops.size())
		input_widths.push_back(in_max[index]);

	input_widths = in_max;
	output_widths = out_max;

	/* End GL */
}

//! Sort operations by instance.
//! @param Input. n is a pointer to the CDFGnode
//! operations bound to the same instance are stored into the same list
static void sortOperationsByInstance(const CDFGnode *n)
{

	const Operation *o;									// the current operation

	//if (Resizing::_debug) {
	//cout << "Resizing::sortOperationsByInstance  looking at node(" << n->name() << ")";
	//n->info();
//	}

	switch (n->type())
	{
	case CDFGnode::DATA:
		// nothing to do
		break;
	case CDFGnode::CONTROL:
		// nothing to do
		break;
	case CDFGnode::OPERATION:
		o = (const Operation *) n;						// dynamic link here
		// add it to the corresponding list
		if ( o->inst()!= NULL)
		{
			//cout <<"Resizing::sortOperationsByInstance  looking at instance(" << o->inst()->no() << ") for operation "<< o->name() << "_" << o->function_name() << endl;
			if (o->inst()->no() >= oi_lists.size())
			{
				oi_lists.resize(o->inst()->no()+1);
				oi_lists[o->inst()->no()].Op = (OperatorInstance *)o->inst();
			}
			if (oi_lists[o->inst()->no()].Op == NULL)
			{
				oi_lists[o->inst()->no()].Op = (OperatorInstance *)o->inst();
			}
			oi_lists[o->inst()->no()].Operations.push_back(o);
		}


		break;
	}

}


static void update_input_dependency_of_dynamic_access(const CDFGnode *n)
{
	const Operation *o;	// the current operation
	long i;

	if (n->type() != CDFGnode::OPERATION) return;

	o = (const Operation *) n;	// dynamic link here

	if (o->function_name() == "mem_read" || o->function_name() == "mem_write")
	{
		const CDFGedge *edge_ref = o->predecessors[1];		// get current predecessor edge
		const CDFGnode *node_ref = edge_ref->source;				// get source node

		for (i = 0; i <o->inputsBitwidth().size(); i++)
		{
			o->inputsBitwidth()[i] = o->inputsBitwidth()[0];
		}


		/*for (i = 0; i < o->predecessors.size(); i++)
		{
			/const CDFGedge *edge = o->predecessors[i];		// get current predecessor edge
			const CDFGnode *node = edge->source;				// get source node
			((Data*)node)->bitwidth(((Data*)node_ref)->bitwidth());
			((Data*)node)->fixedPoint(((Data*)node_ref)->fixedPoint());
			
		}*/
	}
}

//! Resizing results info
void Resizing::info() const
{

	int i, j, k;

	cout << "Resizing results..." << endl;
	cout << "\n------------------------------------------------------------------------------------------------------" << endl;
	cout << "  " << "Operator" << " \t\t;  " << "Implemented Operations" << " \t;  "  << endl;
	cout << "------------------------------------------------------------------------------------------------------" << endl;
	for (i =0; i<oi_lists.size(); i++)
	{
		if (oi_lists[i].Op != NULL)
		{
			cout << "  " << oi_lists[i].Op->oper()->name() << " \t;  " ;
			for (j= 0; j < oi_lists[i].Operations.size() ; j++)
			{
				cout << oi_lists[i].Operations[j]->name() << "_" << oi_lists[i].Operations[j]->function_name() ;
				for (k = 0; k < oi_lists[i].Op->oper()->inputPorts(); k++)
					cout << "_" << oi_lists[i].Operations[j]->inputsBitwidth(k);
				cout << ", ";
			}
			cout << endl;
		}
	}
	cout << "\n------------------------------------------------------------------------------------------------------" << endl;
	cout << "  " << "number of bits" << " \t;  " << _nbBits << endl;
	cout << "------------------------------------------------------------------------------------------------------" << endl;

}
//! Generate a new operator name.
//! @param Input. o is a pointer to operator
//! function name extended by actual witdhs and signes
string Resizing::newOperatorName(Operator *op) const
{
	// new name
	int i;
	string newName;

	// copy function name
	int pos=op->name().find("_op",0);
	newName=op->name().substr(0,pos+3);
	for (i = 0; i < op->inputPorts(); i++)
	{
		string Signed;
		/* GL 10/10/07 : replace line
		if(input_signed[i]==Port::SIGNED)
		/* By*/
		if ((input_signed[i]==Port::SIGNED) || (input_signed[i]==Port::SFIXED))
			/*Fin GL */
			Signed ="s";
		else
			Signed ="u";
		string bitwidth = Parser::itos(input_widths[i]);
		/* GL 10/10/07 : replace line */
		newName += "_"+Signed+bitwidth;
		/* By
		if(input_fixed.empty())
			newName += "_"+Signed+bitwidth;
		else
			newName += "_"+Signed+"f"+bitwidth;
		/* Fin GL */
	}
	for (i = 0; i < op->outputPorts(); i++)
	{
		string Signed;
		/* GL 10/10/07 : replace line
		if(output_signed[i]==Port::SIGNED)
		/* By*/
		if ((output_signed[i]==Port::SIGNED) || (output_signed[i]==Port::SFIXED))
			/*Fin GL */
			Signed ="s";
		else
			Signed ="u";
		string bitwidth = Parser::itos(output_widths[i]);
		/* GL 10/10/07 : replace line */
		newName += "_"+Signed+bitwidth;
		/* By
		if(input_fixed.empty())
			newName += "_"+Signed+bitwidth;
		else
			newName += "_"+Signed+"f"+bitwidth;
		/* Fin GL */
	}
	return newName;
}
//! Generate a new component name.
//! @param Input. o is a pointer to operator
//! function name extended by actual signes
string Resizing::newComponentName(Operator *op) const
{
	// new name
	int i = 0;

	string newName;
	int pos=op->component().find("_op",0);
	newName=op->component().substr(0,pos+3);
	for (i = 0; i < op->inputPorts(); i++)
	{
		string Signed;
		/* GL 10/10/07 : replace line
		if(input_signed[i]==Port::SIGNED)
		/* By*/
		if ((input_signed[i]==Port::SIGNED) || (input_signed[i]==Port::SFIXED))
			/*Fin GL */
			Signed ="s";
		else
			Signed ="u";
		/* GL 10/10/07 : replace line */
		newName += "_"+Signed;
		/* By
		if(input_fixed.empty())
			newName += "_"+Signed;
		else
			newName += "_"+Signed+"f";
		/* Fin GL */
	}
	for (i = 0; i < op->outputPorts(); i++)
	{
		string Signed;
		/* GL 10/10/07 : replace line
		if(output_signed[i]==Port::SIGNED)
		/* By*/
		if ((output_signed[i]==Port::SIGNED) || (output_signed[i]==Port::SFIXED))
			/*Fin GL */
			Signed ="s";
		else
			Signed ="u";
		/* GL 10/10/07 : replace line */
		newName += "_"+Signed;
		/* By
		if(input_fixed.empty())
			newName += "_"+Signed;
		else
			newName += "_"+Signed+"f";
		/* Fin GL */
	}
	return newName;
}

void Resizing::updateParamPort(Operator *op)
{
	int i;
	for (i = 0; i < op->inputPorts(); i++)
	{
		if (_debug) cout << "Resizing::updateParamPort inputPorts " << op->inputPorts() << endl;
		Port *p = (Port*) op->getInputPort(i);
		p->bitwidth(input_widths[i]);
		p->setSigned(input_signed[i]);
		/* GL/10/10/07 : add line */
		if (!input_fixed.empty()) p->fixedPoint(input_fixed[i]);
		else p->fixedPoint(-999);
		/* Fin GL */
		if (_debug) cout << "E" << i << "=>" << p->getStringSigned() << p->bitwidth() << " ";
	}
	for (i = 0; i < op->outputPorts(); i++)
	{
		Port *p = (Port*) op->getOutputPort(i);
		p->bitwidth(output_widths[i]);
		p->setSigned(output_signed[i]);
		/* GL/10/10/07 : add line */
		if (!output_fixed.empty()) p->fixedPoint(output_fixed[i]);
		else p->fixedPoint(-999);
		/* Fin GL */
		if (_debug) cout << "O" << i << "=>" << p->getStringSigned() << p->bitwidth() << " ";
	}
}

//!	from switches  select the resizing mode (approach)
//!	1 => f1
//!	2 => f2
//!	3 => f3
void Resizing::operatorSizing(void)
{


	int i, j;
	for (i = 0; i < oi_lists.size(); i++)
	{
		//get the correspondnat instance
		OperatorInstance *op = oi_lists[i].Op;
		if ( op != NULL)
		{
			if (_debug)
			{
				cout << "Resizing::operatorSizing  operator : New size(" << op->no() << ")";
				op->info();
				op->oper()->info();
			}

			if (_sw->resizingMode()==1) f1(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
			/* GL 19/07/07 : only for mul & div operators )
							Replace line
			if (_sw->resizingMode()==2) f2(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
			/* By */
			if (_sw->resizingMode()==2)
			{
				if ( ( ((Function*)(op->function()))->symbolic_function()== "add") || ( ((Function*) (op->function()))->symbolic_function()== "sub") )
					f1(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
				else
					f2(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
			}
			/* Fin GL */
			/* GL 30/09/2007 : Number of bits */
			for (j = 0; j < input_widths.size(); j++) _nbBits += input_widths[j];
			/* Fin GL */

			if (_sw->resizingMode()==3) f3(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
			/* GL 10/10/07 : add lines */
			//if (oi_lists[i].Operations[0]->sym_function() == "resize"){
			if (op->oper()->getInputPort(0)->fixedPoint() != -999)
			{
				if (_debug) cout << "Resizing::operatorSizing	Found fixed operator" << endl;
				if (_sw->resizingMode()==1) fixed_f1(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
				if (_sw->resizingMode()==2) fixed_f2(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
				if (_sw->resizingMode()==3) fixed_f2(oi_lists[i].Operations, op->oper()->inputPorts(),op->oper()->outputPorts());
			}
			/* Fin GL */
			///////////
			bool change = false;
			for (j = 0; j < op->oper()->inputPorts(); j++)
			{
				change = (op->oper()->getInputPort(j)->getSigned() != input_signed[j]);
				if (change == true) break;

				change = (op->oper()->getInputPort(j)->bitwidth() != input_widths[j]);
				if (change == true) break;
			}
			if (!change)
			{
				for (j = 0; j < op->oper()->outputPorts(); j++)
				{
					change = (op->oper()->getOutputPort(j)->getSigned() != output_signed[j]);
					if (change == true) break;

					change = (op->oper()->getOutputPort(j)->bitwidth() != output_widths[j]);
					if (change == true) break;
				}
			}
			/* GL 10/10/07 : replace lines */
			if (change)
			{
				/* By
				if (change || (oi_lists[i].Operations[0]->sym_function() == "resize")){////////////
				/* Fin GL */
				//create a new operator with suitable width and signed
				const string name = newOperatorName(op->oper());
				const string compName = newComponentName(op->oper());



				Operator *o = op->oper()->copyOf(name, compName);
				// update port's bitwidths
				updateParamPort(o);

				if (_debug)
				{
					cout << "Resizing::operatorSizing  operator " << op->oper()->name() << " has got a new name " << o->name() << endl;
					cout << "Resizing::operatorSizing  component " << op->oper()->component() << " has got a new name " << o->component() << endl;
				}

				op->oper(o);
				// Attribute this new operator to current operator instance
				if (_debug)
				{
					cout << "Resizing::operatorSizing New Instance " << op->no() << " ";
					op->info();
				}
			}

			/* GL 07/09/2007 : combinatorial area */
			_area+= op->oper()->area();
			/* Fin GL */
			input_widths.clear();
			output_widths.clear();
			/* GL 10/10/07 : add lines */
			input_fixed.clear();
			output_fixed.clear();
			/* Fin GL */
		}
	}
}

Resizing::Resizing(
    const Switches *sw,			// in
    const SchedulingOut	*sched, // in-out
    CDFG *cdfg					// in-out
)
{
	// copy of main parameters
	_sw=sw;
	_sched=sched;
	_cdfg=cdfg;


	// EJ 07/07/2008 levinson
	_cdfg->scan_nodes(&update_input_dependency_of_dynamic_access); //TO_VERIFY
	// End EJ


	/* GL 07/09/2007 : combinatorial area */
	_area = 0;
	/* Fin GL */
	/* GL 30/09/2007 : Number of bits */
	_nbBits = 0;
	/* Fin GL */
	
	oi_lists.clear();

	oi_lists.resize(sched->operators()); // TODO ajouter un argument pour la compil sous linux
	if (! _sw->silent_mode())
	{
		if (_sw->resizingMode()==1) cout << "Resizing Mode 1 : input widths are put to the max" << endl;
		if (_sw->resizingMode()==2) cout << "Resizing Mode 2 : input widths are put to the max for each position" << endl;
		if (_sw->resizingMode()==3) cout << "Resizing Mode 3 : Ad Hoc function not yet develloped" << endl;
	}
	if (_debug)
		cout << "Resizing ... sort operations" << endl;
	// scan nodes and sort operations
	_cdfg->scan_nodes(&sortOperationsByInstance);

	// compute and resize each operator instance widths
	if (_debug)
		cout << "Resizing ... size operators" << endl;
	operatorSizing();
}

// end of: resizing.cpp


