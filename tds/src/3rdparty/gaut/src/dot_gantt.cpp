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

//	File:		dot_gantt.cpp
//	Purpose:	.gantt file format output for GAUT kernel
//	Author:		Emmanuel Juin, LESTER, UBS


#include <iostream>
#include <iomanip>
#include "dot_gantt.h"
#include "bus.h"
#include "os_tools.h"

using namespace std;

//! The collection of nodes.
static DotGanttNodeRefs dot_gantt_nodes;
//! Local copy of CDFG parameter to avoid passing constant parameters.
static const CDFG *DotGantt_cdfg;
//! to receive ordered nodes
vector<CDFGnode *> node_list;
//! Local copy of regs parameter to avoid passing constant parameters.
static const RegOut *DotGantt_reg;
//! Local copy of buses parameter to avoid passing constant parameters.
static const BusOut *DotGantt_bus;
//! Local copy of cadency parameter to avoid passing constant parameters.
static const Cadency *DotGantt_cad;
//! Local copy of cadency parameter to avoid passing constant parameters.
static const Clock *DotGantt_clk;
//! Local storage for registers
//static RegRefs ic_registers; DH : 
//26/10/2008 replace by flag doGantt
//DH : 12/11/2008 need memory access time
static long DotGantt_mem_access;



static char *strrepl(char *Str, char *OldStr, char *NewStr)
{
	int OldLen, NewLen;
	char *p, *q;
	if (NULL == (p = strstr(Str, OldStr)))
		return Str;
	OldLen = strlen(OldStr);
	NewLen = strlen(NewStr);
	memmove(q = p+NewLen, p+OldLen, strlen(p+OldLen)+1);
	memcpy(p, NewStr, NewLen);
	return q;
}

static char *deconvertName(char *oldName)
{
	char *newName;

	newName = strdup(oldName);

	while (strstr(newName,"LPAREN")!=NULL)
		strrepl(newName,"LPAREN","(");
	while (strstr(newName,"SIGUMINUS")!=NULL)
		strrepl(newName,"SIGUMINUS","-");
	while (strstr(newName,"COMMA")!=NULL)
		strrepl(newName,"COMMA",",");
	while (strstr(newName,"RPAREN")!=NULL)
		strrepl(newName,"RPAREN",")");

	return newName;
}


static void resetFlagSetDoGantt()
{
	for (int j=node_list.size()-1; j>=0; j--)
	{
		CDFGnode *n_it = node_list[j];
		if (n_it->type() == CDFGnode::DATA)
		{
			Data *d = (Data *) n_it;	
			d->setDoGantt(false);
		}
	}
}

/*
reg_4.A(1,0).asap.0=0
reg_4.A(1,0).alap.0=20
fifo2Stage.31.B(3,3).asap.20=20
fifo2Stage.31.B(3,3).alap.20=20
mul.57.mul_op93.asap.0=0
mul.57.mul_op93.alap.0=20
mul.57.mul_op93.pred=B(3,3)
mul.57.mul_op93.pred=A(2,3)
mul.57.mul_op93.suc=B(3,3)
*/
static void gantt_UT(ofstream &f,bool modulo_cadency)
{
	long clk = DotGantt_clk->period();
	long cadency = DotGantt_cad->length();
	long cycles = cadency / clk;

	f << "latency = " << cadency	<< "\n";
	f << "cycle = " << cycles		<< "\n";
	f << "phase_min = " << clk		<< "\n" << "\n";

	for (int j=node_list.size()-1; j>=0; j--)
	{
		CDFGnode *n_it = node_list[j];
		if (dot_gantt_nodes.search(n_it->name(), &n_it)) continue;

		if (n_it->type() == CDFGnode::OPERATION)
		{
			const Operation *o = (const Operation*) n_it;	// get the current operation
			if (!o->inst()) continue;

			//long start = o->start() % cadency; EJ 04/03/2008 gantt no pipeline
			long start = o->start();
			long stop;
			if (o->isChained()==-1)
				stop = start+o->chainingLength();
			else
				stop  = (start + o->cycles() * clk);

			if (modulo_cadency)
			{
				start = start % cadency;
				stop = stop % cadency;
			}
			f << o->function()->name() << "." << o->inst()->no() << "."
			<< o->function()->name() << "_" << o->name() << ".asap." << start << "=" << start
			<< "\n";
			f << o->function()->name() << "." << o->inst()->no() << "."
			<<  o->function()->name() << "_" << o->name() << ".alap." << start << "=" << stop
			<< "\n";
			int i;
			for (i = 0; i < o->predecessors.size(); i++)
			{
				string name = (string)deconvertName((char*)o->predecessors[i]->source->name().c_str());
				f << o->function()->name() << "." << o->inst()->no() << "."
				<<  o->function()->name() << "_" << o->name() << ".pred=" << name << "\n";
			}

			for (i = 0; i < o->successors.size(); i++)
			{
				string name = (string)deconvertName((char*)o->successors[i]->target->name().c_str());
				f << o->function()->name() << "." << o->inst()->no() << "."
				<<  o->function()->name() << "_" << o->name() << ".suc=" << name << "\n";
			}

		}
		else if (n_it->type() == CDFGnode::DATA)
		{
			Data *d = (Data *) n_it;			// get the current data
			//skip data between chained operations 
			if (Reg::is_chained(d))
				continue;
			//skip unused constant (mul to shift conversion)
			if ((d->type()==Data::CONSTANT) &&
				(d->successors.empty()==true))
				continue;
			//skip unused variable (unused delay element)
			if ((d->type()==Data::VARIABLE) &&
				(d->successors.empty()==true))
			{
				//debug cout << d->name() << endl;
				continue;
			}
			//skip input only used in delay
			if ((d->type()==Data::INPUT ) && (d->successors.empty()) && (d->readvar()))
			{
				continue;
			}
			//skip unused dynamic element 
			if ((d->isADynamicElement()) && (d->predecessors.empty()==true))
				continue;
			//skip unused dynamic access only keep dynamic read access storage
			if ((d->isADynamicAccess()) && (d->predecessors.size()==1) )
			{
				CDFGnode *predecessor= (CDFGnode *)d->predecessors.at(0)->source;
				if (predecessor->type()==CDFGnode::CONTROL)
					continue;
				if (predecessor->type()==CDFGnode::OPERATION)
				{
					Operation *mem_operation = (Operation *)predecessor;
					if (mem_operation->function()->symbolic_function()=="mem_write")
				continue;
				}
			}						
			Reg *rr;
			Reg *reg = d->reg();
			if (!d->doGantt()) 
			{
				d->setDoGantt(true);
				string data_name = (string)deconvertName((char*)d->name().c_str());

				long start;
				long stop;

				if (modulo_cadency)
				{
					start = d->get_start_modulo_cadency();
					stop = d->get_end_modulo_cadency();
				}
				else
				{
					start = d->get_start_no_modulo_cadency();
					stop = d->get_end_no_modulo_cadency();
				}

				if (start == stop) stop += clk;

				long fifo = reg->fifoStage();
				if (stop)
				{
					if (fifo)
					{
						f << "fifo" << fifo+1 << "Stage." << reg->no() << "." << data_name << ".asap." << start << "=" << start << "\n";
						f << "fifo" << fifo+1 << "Stage." << reg->no() << "." << data_name << ".alap." << start << "=" << stop << "\n";
					}
					else if ((d->type()==Data::CONSTANT) && (d->hardwire() == true))
					{
						f << "constant" << "." << reg->no() << "." << data_name << ".asap." << start << "=" << start << "\n";
						f << "constant" << "." << reg->no() <<"."<< data_name << ".alap." << start << "=" << stop << "\n";
					}
					else
					{
						f << "register" << "." << reg->no() << "." << data_name << ".asap." << start << "=" << start << "\n";
						f << "register" << "." << reg->no() <<"."<< data_name << ".alap." << start << "=" << stop << "\n";
					}
				}
			}
		}
		DotGanttNodeRef gantt_node_ref(n_it);	// build smart pointer to new node
		dot_gantt_nodes.add(gantt_node_ref);	// add it to the "smart" map
	}
}

/*
bus.17.A(2,3)_slice3.asap.-10=-10
bus.17.A(2,3)_slice3.alap.-10=0
bus.18.A(3,0).asap.-10=-10
bus.18.A(3,0).alap.-10=0
bus.19.A(3,1).asap.-10=-10
bus.19.A(3,1).alap.-10=0
bus.20.C(0,1)_slice5.asap.-10=-10
bus.20.C(0,1)_slice5.alap.-10=0
*/
static void gantt_ES(ofstream &f)
{
	const BusCycle *bus_cycle;					//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;	//!< a bus accesses iterator
	const BusAccess *bus_access;

	long clk = DotGantt_clk->period();
	long cadency = DotGantt_cad->length();
	long cycles = cadency / clk;

	f << "latency = " << cadency	<< "\n";
	f << "cycle = " << cycles		<< "\n";
	f << "phase_min = " << clk		<< "\n" << "\n";

	for (long cycle = -1 /* for input at time 0 */; cycle < cycles; cycle++)
	{
		bus_cycle = DotGantt_bus->getBusCycle(cycle * clk);
		if (bus_cycle) for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
			{
				bus_access = *ba_it;
				if (bus_access->source() != BusAccess::IO) continue;

				const Data *data = (const Data *) bus_access->addr();

				/* DH pipeline 
				long start = data->start(); 
				long stop  = data->start(); */
				long start = data->get_start_modulo_cadency();
				long stop = data->get_start_modulo_cadency();


				if (bus_access->type() == BusAccess::IN) start -= clk;
				if (bus_access->type() != BusAccess::IN) stop +=  clk;
				/*if (stop == cadency) {
					start = -clk;
					stop  = 0;
				}*/
				long slice=0;
				if (bus_access->type() == BusAccess::IN)	slice = 1 + data->start() / cadency;
				else
					slice = 1 + (data->start() + clk) / cadency;

				string name = (string)deconvertName((char*)data->name().c_str());


				if (slice > 1)
				{
					f << "bus." << bus_access->id()+1 << "." << name << "_slice" << slice << ".asap." << start << "=" << start << "\n";
					f << "bus." << bus_access->id()+1 << "." << name << "_slice" << slice << ".alap." << start << "=" << stop << "\n";
				}
				else
				{
					if (bus_access->type() == BusAccess::IN)
					{
						f << "bus." << bus_access->id()+1 << "." << name << ".Input.asap." << start << "=" << start << "\n";
						f << "bus." << bus_access->id()+1 << "." << name << ".Input.alap." << start << "=" << stop << "\n";
					}
					else
					{
						f << "bus." << bus_access->id()+1 << "." << name << ".asap." << start << "=" << start << "\n";
						f << "bus." << bus_access->id()+1 << "." << name << ".alap." << start << "=" << stop << "\n";
					}
				}
			}
	}
}

/*
memoryBank.[R].-1.tmp.asap.10=10
memoryBank.[R].-1.tmp.alap.10=20
memoryBank.[W]-1.tmp0001ECRITURE.asap.50=50
memoryBank.[W]-1.tmp0001ECRITURE.alap.50=60
*/
static void gantt_MU(ofstream &f)
{
	const BusCycle *bus_cycle;					//!< a bus cycle
	BusCycle::ACCESSES::const_iterator ba_it;	//!< a bus accesses iterator
	const BusAccess *bus_access;
	const BankUse *bu;							// pointer to a bank use
	const BankAddress *ba;						// pointer to a bank address
	const Data *data;

	long clk = DotGantt_clk->period();
	long cadency = DotGantt_cad->length();
	long cycles = cadency / clk;

	f << "latency = " << cadency	<< "\n";
	f << "cycle = " << cycles		<< "\n";
	f << "phase_min = " << clk		<< "\n" << "\n";

	for (long cycle = 0; cycle < cycles; cycle++)
	{
		bus_cycle = DotGantt_bus->getBusCycle(cycle * clk);
		if (bus_cycle) for (ba_it = bus_cycle->accesses.begin(); ba_it != bus_cycle->accesses.end(); ba_it++)
			{
				bus_access = *ba_it;
				if (bus_access->source() != BusAccess::BANK_USE) continue;

				bu = (const BankUse *) bus_access->addr();
				ba = bu->address();
				if (bu->_is_dynamic==NULL)
					data = ba->data();
				else
					data = bu->_is_dynamic;

				long start = bu->real_time() % cadency;
				long stop_no_modulo_cadency  = start  +  DotGantt_mem_access;
				long stop  = stop_no_modulo_cadency % cadency;

				if ((stop==0) && (stop_no_modulo_cadency>=cadency))
					stop=cadency;

				long slice;
				slice = 1 + stop/cadency;

				string name = (string)deconvertName((char*)data->name().c_str());

				if (data->isADynamicAccess())
				{
					f << "memoryBank";
					if (bus_access->type() != BusAccess::IN) f << "[R].";
					else f << "[W]";
					f << bu->bank() << "." << name;
					if (slice != 1) f << "_slice" << slice;
					if (bus_access->type() != BusAccess::OUT) f << "ECRITURE";
					f << ".asap." << start << "=" << start << "\n";
					f << "memoryBank";
					if (bus_access->type() != BusAccess::IN) f << "[R].";
					else f << "[W]";
					f << bu->bank() << "." << name;
					if (slice != 1) f << "_slice" << slice;
					if (bus_access->type() != BusAccess::OUT) f << "ECRITURE";
					f << ".alap." << start << "=" << stop << "\n";
				}
				else
				{
					f << "memoryBank";
					if (bus_access->type() == BusAccess::IN) f << "[R].";
					else f << "[W]";
					f << bu->bank() << "." << name;
					if (slice != 1) f << "_slice" << slice;
					if (bus_access->type() == BusAccess::OUT) f << "ECRITURE";
					f << ".asap." << start << "=" << start << "\n";
					f << "memoryBank";
					if (bus_access->type() == BusAccess::IN) f << "[R].";
					else f << "[W]";
					f << bu->bank() << "." << name;
					if (slice != 1) f << "_slice" << slice;
					if (bus_access->type() == BusAccess::OUT) f << "ECRITURE";
					f << ".alap." << start << "=" << stop << "\n";
				}
			}
	}
}

DotGantt::DotGantt(
    const Switches *sw,					// in
    const CDFG *cdfg,					// in
    const Clock *clk,					// in
    const Cadency *cadency,				// in
    const RegOut *reg_out,				// in
    const BusOut *bout					// in
)
{

	DotGantt_cdfg = cdfg;
	DotGantt_bus  = bout;
	DotGantt_cad  = cadency;
	DotGantt_clk  = clk;
	DotGantt_reg = reg_out;
	DotGantt_mem_access = sw->memory_access_time();

	// build the evaluation order in the graph
	int n = DotGantt_cdfg->numberOfNodes();
	vector<bool> inserted(n);	// to avoid double insertion
	for (int i = 0; i < n; i++) inserted[i] = false;
	orderSuccessors<CDFGnode, CDFGedge>((CDFGnode*) DotGantt_cdfg->source(),
	                                    inserted, node_list);	// order nodes now

	string gantt_UT_file_name = Tools::prefix(sw->cdfg_file_name())+"_UT.gantt";
	string gantt_UT_PIPE_file_name = Tools::prefix(sw->cdfg_file_name())+"_UT_PIPE.gantt";
	string gantt_ES_file_name = Tools::prefix(sw->cdfg_file_name())+"_ES.gantt";
	string gantt_MU_file_name = Tools::prefix(sw->cdfg_file_name())+"_MU.gantt";

	ofstream ut(gantt_UT_file_name.c_str(), ios::out);
	Tools::tag(ut, "# ", gantt_UT_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_UT(ut,false);
	ut.close();

	resetFlagSetDoGantt();
	dot_gantt_nodes.clear();		// free memory

	ofstream ut_pipe(gantt_UT_PIPE_file_name.c_str(), ios::out);
	Tools::tag(ut_pipe, "# ", gantt_UT_PIPE_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_UT(ut_pipe,true);
	ut_pipe.close();

	node_list.clear(); //add by Caaliph: free memory 27/06/2007

	ofstream es(gantt_ES_file_name.c_str(), ios::out);
	Tools::tag(es, "# ", gantt_ES_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_ES(es);
	es.close();

	ofstream mu(gantt_MU_file_name.c_str(), ios::out);
	Tools::tag(mu, "# ", gantt_MU_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_MU(mu);
	mu.close();

	dot_gantt_nodes.clear();		// free memory
}

/* Caaliph : 27/06/2007 */
DotGantt::DotGantt(
    const Switches *sw,					// in
   	const string &_cdfg_file_name,		//in //DH passage par reference 12/11/2008 : attention au passage par veleur (recopie)
    const CDFG *cdfg,					// in
    const Clock *clk,					// in
    const Cadency *cadency,				// in
    const RegOut *reg_out,				// in
    const BusOut *bout					// in
)
{

	DotGantt_cdfg = cdfg;
	DotGantt_bus  = bout;
	DotGantt_cad  = cadency;
	DotGantt_clk  = clk;
	DotGantt_reg = reg_out;
	DotGantt_mem_access = sw->memory_access_time();

	// build the evaluation order in the graph
	int n = DotGantt_cdfg->numberOfNodes();
	vector<bool> inserted(n);	// to avoid double insertion
	for (int i = 0; i < n; i++) inserted[i] = false;
	orderSuccessors<CDFGnode, CDFGedge>((CDFGnode*) DotGantt_cdfg->source(),
	                                    inserted, node_list);	// order nodes now

	string gantt_UT_file_name = Tools::prefix(_cdfg_file_name)+"_UT.gantt";
	string gantt_UT_PIPE_file_name = Tools::prefix(sw->cdfg_file_name())+"_UT_PIPE.gantt";
	string gantt_ES_file_name = Tools::prefix(_cdfg_file_name)+"_ES.gantt";
	string gantt_MU_file_name = Tools::prefix(_cdfg_file_name)+"_MU.gantt";

	ofstream ut(gantt_UT_file_name.c_str(), ios::out);
	Tools::tag(ut, "# ", gantt_UT_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_UT(ut,false);
	ut.close();

	resetFlagSetDoGantt();
	dot_gantt_nodes.clear();		// free memory

	ofstream ut_pipe(gantt_UT_PIPE_file_name.c_str(), ios::out);
	Tools::tag(ut_pipe, "# ", gantt_UT_PIPE_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_UT(ut_pipe,true);
	ut_pipe.close();

	node_list.clear(); //add by Caaliph: free memory 27/06/2007

	ofstream es(gantt_ES_file_name.c_str(), ios::out);
	Tools::tag(es, "# ", gantt_ES_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_ES(es);
	es.close();

	ofstream mu(gantt_MU_file_name.c_str(), ios::out);
	Tools::tag(mu, "# ", gantt_MU_file_name, sw->gaut_name(), sw->gaut_version());
	gantt_MU(mu);
	mu.close();

	dot_gantt_nodes.clear();		// free memory
}
/* End Caaliph */

//  end of: dot_gantt.cpp
