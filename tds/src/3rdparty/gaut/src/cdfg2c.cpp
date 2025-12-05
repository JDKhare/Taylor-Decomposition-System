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

//	File:		Cdfg2c.cpp
//	Purpose:	CDFG to C (debug)
//	Author:		D. Heller, Lab-Sticc, UBS

#include <iostream>
#include <iomanip>
#include "scheduling.h"

#include "cdfg2c.h"

#include "bank.h"
#include "bus.h"
#include "cdfg.h"
#include "graph.h"
#include "data.h"
#include "operation.h"
#include "ic.h"
using namespace std;


//! Local copy of CDFG parameter to avoid passing constant parameters.
static const CDFG *Cdfg2c_cdfg;

//! to receive ordered nodes
vector<CDFGnode *> Cdfg2c_node_list;

static void recursive_aging(ofstream &f,const Data *data);//recursive method for aging generation


string Cdfg2c::get_predefined_operation(const string &operation_name)
{
	if (operation_name.compare("assign")==0)
		return "";
	else if ((operation_name.compare("or")==0) || (operation_name.compare("or_fp")==0))
		return "|";
	else if	((operation_name.compare("and")==0) || (operation_name.compare("and_fp")==0))
		return "&";
	else if (operation_name.compare("not")==0)
		return "~";
	else if	((operation_name.compare("truth_or")==0) || (operation_name.compare("truth_or_fp")==0))
		return "||";
	else if	((operation_name.compare("truth_and")==0) || (operation_name.compare("truth_and_fp")==0))
		return "&&";
	else if	(operation_name.compare("truth_not")==0)
		return "!";
	else if ((operation_name.compare("eq")==0) || (operation_name.compare("eq_fp")==0))
		return "==";
	else if ((operation_name.compare("ne")==0) || (operation_name.compare("ne_fp")==0))
		return "!=";
	else if ((operation_name.compare("lt")==0) || (operation_name.compare("lt_fp")==0))
		return "<";
	else if	((operation_name.compare("le")==0) || (operation_name.compare("le_fp")==0))
		return "<=";
	else if ((operation_name.compare("gt")==0) || (operation_name.compare("gt_fp")==0))
		return ">";
	else if ((operation_name.compare("ge")==0) || (operation_name.compare("ge_fp")==0))
		return ">=";
	else if	((operation_name.compare("add")==0) || (operation_name.compare("add_fp")==0))
		return "+"; 
	else if	((operation_name.compare("sub")==0) || (operation_name.compare("sub_fp")==0))
		return "-";
	else if	(operation_name.compare("mul")==0)
		return "*";
	else if	(operation_name.compare("mod")==0)
		return "%";
	else if	(operation_name.compare("div")==0)
		return "/";
	else if	(operation_name.compare("uminus")==0)
		return "uminus";	
	/* else if	(strcmp(operation_name,"nand")==0)
		return "nand";
	else if	(strcmp(operation_name,"nor")==0)
		return "nor"; only in old vhdl mode */
	else if	((operation_name.compare("xor")==0) || (operation_name.compare("xor_fp")==0))
		return "^";
	else if	(operation_name.compare("abs")==0)
		return "abs";	
	else if	(operation_name.compare("sll")==0)
		return "<<";	
	else if	(operation_name.compare("srl")==0)
		return ">>";	
	else if	(operation_name.compare("sla")==0)
		return "<<";	
	else if	(operation_name.compare("sra")==0)
		return ">>";	
	/* else if	(strcmp(operation_name,"rol")==0)
		return "-";	
	else if	(strcmp(operation_name,"ror")==0)
		return "-"; only in old vhdl mode */	
 	else if	((operation_name.compare("eqmux")==0) || (operation_name.compare("eqmux_fp")==0))
		return "predefined";	
	else if	((operation_name.compare("nemux")==0) || (operation_name.compare("nemux_fp")==0))
		return "predefined";	
	else if	((operation_name.compare("ltmux")==0) || (operation_name.compare("ltmux_fp")==0))
		return "predefined";	
	else if	((operation_name.compare("lemux")==0) || (operation_name.compare("lemux_fp")==0))
		return "predefined";	
	else if	((operation_name.compare("gemux")==0) || (operation_name.compare("gemux_fp")==0))
		return "predefined";	
	else if	((operation_name.compare("gtmux")==0) || (operation_name.compare("gtmux_fp")==0))
		return "predefined";	
	else if	(operation_name.compare("shr")==0)
		return ">>";	
	else if	(operation_name.compare("shl")==0)
		return "<<";	
	else if	(operation_name.compare("bitselect")==0)
		return "predefined";	
	else if	(operation_name.compare("sliceread")==0)
		return "slc";	
	else if	(operation_name.compare("slicewrite")==0)
		return "predefined"; 	
	else if	(operation_name.compare("resize")==0)
		return "resize"; 	
	else if	(operation_name.compare("mem_write")==0)
		return "mem_write";	
	else if	(operation_name.compare("mem_read")==0)
		return "mem_read";
	else if	(operation_name.compare("trnwrap")==0)
		return "";	
	else if	(operation_name.compare("trnsat")==0)
		return "";	
	else if	(operation_name.compare("trnsatzero")==0)
		return "";	
	else if	(operation_name.compare("trnsatsym")==0)
		return "";	
	else if	(operation_name.compare("rndwrap")==0)
		return "";	
	else if	(operation_name.compare("rndsat")==0)
		return "";	
	else if	(operation_name.compare("rndsatzero")==0)
		return "";	
	else if	(operation_name.compare("rndsatsym")==0)
		return "=";	
	else if	(operation_name.compare("trnzerowrap")==0)
		return "";	
	else if	(operation_name.compare("trnzerosat")==0)
		return "";	
	else if	(operation_name.compare("trnzerosatzero")==0)
		return "";	
	else if	(operation_name.compare("trnzerosatsym")==0)
		return "";	
	else if	(operation_name.compare("rndinfwrap")==0)
		return "";	
	else if	(operation_name.compare("rndinfsat")==0)
		return "";	
	else if	(operation_name.compare("rndinfsatzero")==0)
		return "";	
	else if	(operation_name.compare("rndinfsatsym")==0)
		return "";	
	else if	(operation_name.compare("rndmininfwrap")==0)
		return "";	
	else if	(operation_name.compare("rndmininfsat")==0)
		return "";	
	else if	(operation_name.compare("rndmininfsatzero")==0)
		return "";	
	else if	(operation_name.compare("rndmininfsatsym")==0)
		return "";	
	else if	(operation_name.compare("rndconvwrap")==0)
		return "";	
	else if	(operation_name.compare("rndconvsat")==0)
		return "";	
	else if	(operation_name.compare("rndconvsatzero")==0)
		return "";	
	else if	(operation_name.compare("rndconvsatsym")==0)
		return "";	
	else if	(operation_name.compare("rndzerowrap")==0)
		return "";	
	else if	(operation_name.compare("rndzerosat")==0)
		return "";	
	else if	(operation_name.compare("rndzerosatzero")==0)
		return "";	
	else if	(operation_name.compare("rndzerosatsym")==0)
		return "";	
	else return "undefined";	
}

static void init(ofstream &f)
{
	int i,j;
	f << "#include <fstream>"							<< "\n";
	f << "#include \"ac_fixed.h\"" << "\n"; // ac_fixed include ac_int
	f << "using namespace std;" << "\n"<< "\n";

	f << "#define iteration 10"							<< "\n";
	f << "#define verbose   0"							<< "\n";
	f << "#define trace     1"							<< "\n" << "\n";

	map<string,const Operation*> operations;

	for (i = 0; i < Cdfg2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *n = Cdfg2c_cdfg->nodes().at(i);
		if (n->type() == CDFGnode::OPERATION)
		{
			const Operation *o = (const Operation *) n;
			if (operations.find(o->function_signature(Cdfg2c_cdfg))==operations.end())
			{
				operations[o->function_signature(Cdfg2c_cdfg)]=o;
			}
		}
	}
	//declare method for user defined operation
	f << "\n" << "//predefined or user defined  operation"	<< "\n";
	for( map<string,const Operation*>::iterator ii=operations.begin(); ii!=operations.end(); ++ii)
	{
			const Operation *operation = (const Operation *)(*ii).second;
			
			string operation_name=Cdfg2c::get_predefined_operation(operation->function_name());
			if ((operation_name.compare("undefined")==0) ||
				(operation_name.compare("predefined")==0)) 
			{
				f << "void " << operation->function_signature(Cdfg2c_cdfg) << "(";
				for (i = 0; i < operation->predecessors.size(); i++)
				{
					if (i) f << ", ";

					CDFGnode *predecessor = operation->predecessors[i]->source;	
					Data *d = ((Data*)predecessor);
					f << "const " << d->get_mentor_type() << " &input_" << i;
				}
	
				f << ", ";
				for (i = 0; i < operation->successors.size(); i++)
				{
					if (i) f << ", ";
					CDFGnode *successor = operation->successors[i]->target;
					Data *d = ((Data*)successor);
					f << d->get_mentor_type() << " &output_" << i;

				}
				f << ")" << "\n";
				f << "{" << "\n";
				if (operation_name.compare("undefined")==0)
				{
					f << "\t//TODO complete method internal code" << "\n";
					for (i = 0; i < operation->successors.size(); i++)
					{
						f << "\toutput_" << i << " =...;" << "\n";
					}
						f << "\treturn;" << "\n";
				}
			 	else if	((operation->function_name().compare("eqmux")==0) ||
						 (operation->function_name().compare("eqmux_fp")==0))
				{
					f << "\tif (input_0 == input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("nemux")==0) ||
						 (operation->function_name().compare("nemux_fp")==0))
				{
					f << "\tif (input_0 != input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("ltmux")==0) ||
						  (operation->function_name().compare("ltmux_fp")==0))
				{
					f << "\tif (input_0 < input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("lemux")==0) ||
						 (operation->function_name().compare("lemux_fp")==0))
				{
					f << "\tif (input_0 <= input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("gemux")==0) ||
						(operation->function_name().compare("gemux_fp")==0))
				{
					f << "\tif (input_0 >= input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	((operation->function_name().compare("gtmux")==0) ||
						(operation->function_name().compare("gtmux_fp")==0))
				{
					f << "\tif (input_0 > input_1)" << "\n";
					f << "\t\toutput_0 = input_2;" << "\n";
					f << "\telse" << "\n";
					f << "\t\toutput_0 = input_3;" << "\n";
				}
			 	else if	(operation->function_name().compare("bitselect")==0)
				{
					f << "\toutput_0=input_0[input_1.to_uint()];" << "\n";
				}
			 	else if	(operation->function_name().compare("slicewrite")==0)
				{
					f << "\toutput_0=input_0.set_slc(input_0,input_1);" << "\n";
				}
				else
				{
					f << "\t//TODO complete method internal code" << "\n";
					for (i = 0; i < operation->successors.size(); i++)
					{
						f << "\toutput_" << i << " =...;" << "\n";
					}
						f << "\treturn;" << "\n";
				}
				f << "}" << "\n";
			}
	}
	operations.clear();
	//constant
	f << "//constant" << "\n";
	for (i = 0; i < Cdfg2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Cdfg2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::CONSTANT)
			{


				f << "const " << data->get_mentor_type() << " " << data->name() << " = " << data->debug_value() << ";" << "\n";
			}
		}
	}
	//input
	f << "//input" << "\n";
	for (i = 0; i < Cdfg2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Cdfg2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::INPUT)
			{
				f  << data->get_mentor_type() << " " << data->name() << "[iteration] = {0,1,2,3,4,5,6,7,8,9};"  << "\n";
			}
		}
	}
	//output
	f << "//output" << "\n";
	for (i = 0; i < Cdfg2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Cdfg2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::OUTPUT)
			{
				f  << data->get_mentor_type() << " " << data->name() << " = 0;"  << "\n";
			}
		}
	}
	//variable
	f << "//variable" << "\n";
	for (i = 0; i < Cdfg2c_cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = Cdfg2c_cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (data->type()==Data::VARIABLE)
			{
				// memory dynamic ?
				if (data->isADynamicAccess() && data->dynamic_access()->name() == data->name())
				{
					f <<data->get_mentor_type() << " " << data->name() << "[" << data->dynamic_elements()->size() << "]={";
					for (j=0; j<data->dynamic_elements()->size(); j++)
					{
						f << data->dynamic_element(j)->resetvalue();
						if (j == data->dynamic_elements()->size()-1)
							f << "};" << "\n";
						else
							f << ",";
					}
				}
				else if (data->dynamic_address() == -1)
				f  << data->get_mentor_type() << " " << data->name() << " = " << data->resetvalue() << ";" << "\n";
			}  
		}
	}
	f << "\n";

	f << "// process" << "\n";
	f << "int main (int argc, char *argv[])"				<< "\n";
	f << "{"												<< "\n";
	f << "\tint it;"										<< "\n";
	f << "\tfor (it=0; it<iteration; it++)" << "\n" << " \t{"	<< "\n";
	f << "\t\tif(trace) cout <<  \"Iteration : \" << it << endl;"	<< "\n";
}

static void process_operation(ofstream &f)
{
	for (int i=Cdfg2c_node_list.size()-1; i>=0; i--)
	{
		CDFGnode *n_it = Cdfg2c_node_list[i];
		if (n_it->type() != CDFGnode::OPERATION) continue;
		const Operation *operation = (const Operation*) n_it;	// get the current operation
	
		f << "\t\t";
		string operation_name=Cdfg2c::get_predefined_operation(operation->function_name());
		if ((operation_name.compare("undefined")==0) ||
			(operation_name.compare("predefined")==0))
		{
			int j;
			f << operation->function_signature(Cdfg2c_cdfg) << "(";
			for (j = 0; j < operation->predecessors.size(); j++)
			{
				if (j) f << ", ";
				CDFGnode *predecessor = operation->predecessors[j]->source;
				Data *d = (Data *)predecessor;
				f << d->get_data_name();
			}
			f << ", ";
			for (j = 0; j < operation->successors.size(); j++)
			{
				if (j) f << ", ";
				CDFGnode *successor = operation->successors[j]->target;
				Data *d = (Data *)successor;
				f << d->get_data_name();
			}
			f << ");" << "\n";
		}
		else //predefined operator: unary or binary operator or slc
		{
			if (operation_name.compare("mem_write")!=0) 
			{
				for (int j = 0; j < operation->successors.size(); j++)
				{
					if (j) f << " = ";
					CDFGnode *successor = operation->successors[j]->target;
					Data *d = (Data *)successor;
					f << d->get_data_name();
				}
				f << " = ";
			}
			//unary operator ?
			if ((operation_name.compare("")==0) ||
				(operation_name.compare("uminus")==0) || (operation_name.compare("abs")==0))
			{
				//assignment or acfixed_resize(rndsat;...)
				if (operation->predecessors.size()!=1)
				{
					cout << "Invalid operand number for " << operation_name << "\n";
					exit (-1);
				}
				else
				{
					if (operation_name.compare("uminus")==0)
						f << " -";
					CDFGnode *predecessor = operation->predecessors[0]->source;
					Data *d = (Data *)predecessor;
					f << d->get_data_name() << ";" << "\n";
				}
			}
			else if (operation_name.compare("slc")==0) 
			{
				//slice_read
				CDFGnode *operand0_node = operation->predecessors[0]->source;
				Data *operand0 = (Data *)operand0_node;
				CDFGnode *index_node = operation->predecessors[1]->source;
				Data *index_operand = (Data *)index_node;
				CDFGnode *rang_node = operation->predecessors[2]->source;
				Data *rang_operand = (Data *)rang_node;
				f <<  operand0->get_data_name() << ".slc<" <<rang_operand->value() << ">(" << index_operand->get_data_name() << ");" << "\n";
			}
			else if (operation_name.compare("mem_read")==0) 
			{
				//mem_read
				CDFGnode *successsor = operation->successors[0]->target;
				Data *d = (Data *)successsor;
				const Data *dynamic_access =  d->dynamic_access();
				CDFGnode *index_node = operation->predecessors[0]->source;
				Data *index_operand = (Data *)index_node;
				f <<  dynamic_access->get_data_name() << "[" << index_operand->get_data_name() << "];" << "\n";
			}
			else if (operation_name.compare("mem_write")==0) 
			{//mem_write
					CDFGnode *successsor = operation->successors[0]->target;
					Data *d = (Data *)successsor;
					const Data *dynamic_access =  d->dynamic_access();
					CDFGnode *index_node = operation->predecessors[0]->source;
					Data *index_operand = (Data *)index_node;
					CDFGnode *value_node = operation->predecessors[1]->source;
					Data *value_operand = (Data *)value_node;
					f << dynamic_access->get_data_name() << "[" << index_operand->get_data_name() << "] = " <<  value_operand->get_data_name() << ";" << "\n";
			}
			else if (operation_name.compare("resize")==0) 
			{//ac_int_resize (sign_extend)
					CDFGnode *successor = operation->successors[0]->target;
					Data *d = (Data *)successor;
					CDFGnode *operand0_node = operation->predecessors[0]->source;
					Data *operand0 = (Data *)operand0_node;
					f << "(" << d->get_mentor_type() << ")" << operand0->get_data_name() <<";" << "\n";
			}
			else //binary operator
			{
				if (operation->predecessors.size()!=2)
				{
					cout << "Invalid operand number for " << operation_name << "\n";
					exit (-1);
				}
				else
				{
					CDFGnode *operand0_node = operation->predecessors[0]->source;
					Data *operand0 = (Data *)operand0_node;
					CDFGnode * operand1_node = operation->predecessors[1]->source;
					Data *operand1 = (Data *)operand1_node;
					f << operand0->get_data_name() << " "<< operation_name << " " <<operand1->get_data_name() << ";" << "\n";
				}
			}

		}
		//trace
		for (int j = 0; j < operation->successors.size(); j++)
		{
			CDFGnode *successsor = operation->successors[j]->target;
			Data *d = (Data *)successsor;
			if (d->type()==Data::OUTPUT)
				f << "\t\t\tcout << \"" << d->get_data_name() << "[ \" << it << \"] = \"" << " << " << d->get_data_name()  << " << endl;" << "\n";
			else
			{
				f << "\t\tif (trace)" << "\n";
				f << "\t\t{" << "\n";
				if (d->getSigned()==Data::STD_LOGIC_VECTOR) 
					f << "\t\t\tcout << \"" << d->get_data_name() << "[ \" << it << \"] = \"" << " << " << d->get_data_name()  << " << endl;" << "\n";
				else
				{
					f << "\t\t\tcout << \"" << d->get_data_name() << "[ \" << it << \"] = \"" << " << " << d->get_data_name()  << " << \"(\" << " << d->get_data_name() << ".to_string(AC_BIN)" << " << \")\"" << " << endl;" << "\n";
				}
				f << "\t\t}" << "\n";
			}
		}
	}


}

// X(3) = X(2);
// X(2) = X(1);
// X(1) = Xn;
static void recursive_aging(ofstream &f,const Data *data)
{
	if (data->writevar())
	{
		if (Data::INPUT == data->writevar()->type())
		{
			f << "\t\t" << data->name() << " = " << data->writevar()->name() << "[it]; // aging" << "\n";
		}
		else
		{
			f << "\t\t" << data->name() << " = " << data->writevar()->name() << "; // aging" << "\n";
		}
		recursive_aging(f,data->writevar());
	}
} 

void Cdfg2c::process_loopback(ofstream &f,const CDFG *cdfg)
{
	for (int i = 0; i < cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			if (!data->aging()  && data->writevar())
			{
				f << "\t\t" << data->name() << " = " << data->writevar()->name() << "; //loopback" << "\n";
			}
		}
	}
}

void Cdfg2c::process_aging(ofstream &f,const CDFG *cdfg)
{
	for (int i = 0; i < cdfg->numberOfNodes(); i++)
	{
		const CDFGnode *node = cdfg->nodes().at(i);
		if (node->type() == CDFGnode::DATA)
		{
			const Data *data;
			data = (const Data*) node;	
			//end of aging vector
			if (data->aging() && !(data->readvar()))
			{
				f << "\t\t" << data->name() << " = " << data->writevar()->name() << "; // aging" << "\n";
				recursive_aging(f,data->writevar());
			}
		}
	}
}



Cdfg2c::Cdfg2c(
    const CDFG *cdfg					// in
)
{

	// copy parameters
	Cdfg2c_cdfg = cdfg;


	// build the evaluation order in the graph
	int n = Cdfg2c_cdfg->numberOfNodes();
	vector<bool> inserted(n);	// to avoid double insertion
	for (int i = 0; i < n; i++) inserted[i] = false;
	orderSuccessors<CDFGnode, CDFGedge>((CDFGnode*) Cdfg2c_cdfg->source(),
	                                    inserted, Cdfg2c_node_list);	// order nodes now
	// ofstream cdfg to C
	ofstream fc("graph2cpp.cpp", ios::out);
	init(fc);
	process_operation(fc);
	process_loopback(fc,Cdfg2c_cdfg);
	process_aging(fc,Cdfg2c_cdfg);
	fc << "  }" << "\n" << "  return 0;"						<< "\n";
	fc << "}"												<< endl;
	fc.close();
	Cdfg2c_node_list.clear(); // 

}

// end of: Cdfg2c.cpp
