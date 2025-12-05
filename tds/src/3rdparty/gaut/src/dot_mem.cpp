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

//	File:		dot_mem.h
//	Purpose:	.mem file formet output for GAUT kernel
//	Author:		Pierre Bomel, LESTER, UBS


#include <iostream>
#include <iomanip>
#include "dot_mem.h"
#include "bank.h"
#include "bus.h"
using namespace std;

//! The collection of nodes.
static DotMemNodeRefs dot_mem_nodes;
//! Number of memory points.
static long memory_points = 0;
//! Number of communication points.
static long comm_points = 0;
//! The width of a column.
static const int WIDTH = 10;
//! Local copy of CDFG parameter to avoid passing constant parameters.
static const CDFG *DotMem_cdfg;

//! To track the nodes in a CDFG and extract useful data for .mem file.
//! @param Input. node is a pointer to a constant CDFG node.
static void DotMem_track_ios(const CDFGnode *node)
{
	const Data *data;
	switch (node->type())
	{
	case CDFGnode::DATA:
		data = (const Data*) node;									// dynamic link here
		if (DotMem::_debug) cout << "Data " << data->name();

		DotMemNode *mem_node;										// pointer to .mem "node"
		if (!dot_mem_nodes.search(data->name(), &mem_node))  		// not found in map, add it
		{
			if (DotMem::_debug) cout << ", no node";
			DotMemNode::Type type;
			switch (data->type())
			{
			case Data::INPUT:
				if (DotMem::_debug) cout << ", type=input";
				type = DotMemNode::INPUT;
				comm_points++;
				break;
			case Data::OUTPUT:
				if (DotMem::_debug) cout << ", type=output";
				type = DotMemNode::OUTPUT;
				comm_points++;
				break;
			case Data::VARIABLE:
				if (DotMem::_debug) cout << ", type=variable";
				if (data->aging())
				{
					type = DotMemNode::AGING;
					memory_points++;									// one more "memory point" detected
					if (DotMem::_debug) cout << "/aging";
				}
				else if (data->writevar())
				{
					type = DotMemNode::LOOPBACK;
					memory_points++;									// one more "memory point" detected
				if (DotMem::_debug) cout << "/loopbackR";
				}
				else if (!data->readvar())
				{
					if (data->dynamic_access())
					{
						type = DotMemNode::DYNAMIC;
						memory_points++;									// one more "memory point" detected
						if (DotMem::_debug) cout << "/dynamic";
					}
					else
					{
						type = DotMemNode::VARIABLE;
						if (DotMem::_debug) cout << "/variable";
						if (data->bank()!=-1)
							memory_points++;//static array
						else 
							return;
					}
				}
				else
				{
					type = DotMemNode::LOOPBACK_WRITE;
					memory_points++;									// one more "memory point" detected
					if (DotMem::_debug) cout << "/loopbackW";
				}
				break;
			case Data::CONSTANT:
				if (DotMem::_debug) cout << ", type=constant";
				if (!data->hardwire())
				{
					type = DotMemNode::CONSTANT;
					memory_points++;
				}
				else
						return;
				break;
			}
			mem_node = new DotMemNode(data, type);					// build node
			DotMemNodeRef mem_node_ref(mem_node);					// build smart pointer to new node
			dot_mem_nodes.add(mem_node_ref);						// add it to the "smart" map
		}
		break;
	case CDFGnode::OPERATION:
	case CDFGnode::CONTROL:
		if (DotMem::_debug) cout << "node " << node->name() << endl;
		// do nothing
		break;
	}
	if (DotMem::_debug) cout << endl;
}

DotMem::DotMem(
    const Switches *sw,					// in
	string *mem_file_name,              //in 
    const long nb_bits,						// in
    const CDFG *cdfg,					// in
    const Clock *clk,					// in
    const Mem *mem,						// in
    const Cadency *cadency,				// in
    const RegOut *reg_out,				// in
    const BusOut *bout,					// in
    const BusOut *about					// in. // EJ 14/02/2008 : dynamic memory
)
{
	char *mem_file;

	// copy parameters
	DotMem_cdfg = cdfg;
	//DH: 12/11/2008 merge the old two constructor in only one
	if (mem_file_name==NULL)
		mem_file=strdup(sw->mem_file_name().c_str());
	else
		mem_file=strdup(mem_file_name->c_str());
	cout << mem_file << endl;
	//Tools::check_file_does _not_exist(sw->mem_file_name());		// check
	ofstream of(mem_file, ios::out);			// open

	// tag file
	Tools::tag(of, "# ", sw->mem_file_name(), sw->gaut_name(), sw->gaut_version());
	of << "\n";

	// write info into file
	//	PHASE_MIN 	= 10 ;
	//	ACCESMEMOIRE 	= 10 ;
	//	TpsCalcul 	= 720 ;
	//	NbTranchePip 	= 2 ;
	//	nb_bus 	= 41 ;
	//	nb_bits 	= 20 ;
	//	TpsTraversee 	= 360 ;
	// These names should be converted to english ones.
	of << "PHASE_MIN    = " << clk->period()      << " ;" << "\n";
	of << "ACCESMEMOIRE = " << mem->access_time() << " ;" << "\n";
	of << "TpsCalcul    = " << cadency->length()  << " ;" << "\n";

	int NbTranchePip = 1;
	BusOut::CYCLES::const_iterator b_it;// cycles iterator
	//DH: 12/11/2008 : modification du calcul de nombre de tranche
	for (b_it = bout->cycles().begin(); b_it != bout->cycles().end(); b_it++)  		// scan all cycles
	{
		const BusCycle *cycle = *b_it;												// get current cycle
		//long time = cycle->time();													// get cycle time
		BusCycle::ACCESSES::const_iterator a_it;									// access iterator
		const BankUse *bu;															// pointer to a bank use
		const Data *data;															// pointer to a data
		for (a_it = cycle->accesses.begin(); a_it != cycle->accesses.end(); a_it++) 	// scan all accesses
		{
			const BusAccess *a = *a_it;										// get current access
			// compute info
			bool way_in = (a->type() == BusAccess::IN);
			long slice;
			if (a->source() == BusAccess::BANK_USE)
			{
				//donnee en memoire
				bu = (const BankUse *) a->addr();
				//fin  de lecture ou écriture
				//pour une lecture bu->real_time()=data->start()-mem_access
				//pour une ecriture bu->real_time()=data->start()+clk->period()
				slice = 1 +(bu->real_time()+sw->memory_access_time()) / cadency->length(); 
			}
			else
			{
				//entre/sortie 
				data = (const Data *) a->addr();
				if (way_in)
				{
					slice = 1 + data->start() / cadency->length();
				}
				else
				{
					slice = 1 + (data->start()+clk->period()) / cadency->length();
				}
			}
			if (slice > NbTranchePip) NbTranchePip = slice;
		}
	}
	of << "NbTranchePip = " << NbTranchePip << " ;" << "\n";
	
	/* DH: 26/10/2009 */
	cout << "Number of slice (pipeline stage) : " <<  NbTranchePip << endl;
	/* DH: 26/10/2009 */

	of << "nb_bus       = " << bout->buses()      << " ;" << "\n";
	if ((about!=NULL) && (about->buses()>0))
		of << "nb_address_bus = " << about->buses()	<< " ;" << "\n"; // EJ 14/02/2008 : dynamic memory
	of << "nb_bits      = " << nb_bits        << " ;" << "\n";
	of << "TpsTraversee = " << cdfg->latency()    << " ;" << "\n";
	of << "\n";

	// scan CDFG to extract useful informations.
	DotMem_cdfg->scan_nodes(DotMem_track_ios);

	// scan bus synthesis ouputs to extract useful informations
	for (b_it = bout->cycles().begin(); b_it != bout->cycles().end(); b_it++)  		// scan all cycles
	{
		const BusCycle *cycle = *b_it;												// get current cycle
		long time = cycle->time();													// get cycle time
		if (_debug) cout << "cycle " << time << endl;
		BusCycle::ACCESSES::const_iterator a_it;									// access iterator
		const BankUse *bu;															// pointer to a bank use
		const BankAddress *ba;														// pointer to a bank address
		const Data *data;															// pointer to a data
		DotMemNode *mem_node;														// pointer to .mem "node"
		for (a_it = cycle->accesses.begin(); a_it != cycle->accesses.end(); a_it++) 	// scan all accesses
		{
			const BusAccess *a = *a_it;
			// get current access
			bool way_in = a->type() == BusAccess::IN;								// way of the access: true = IN
			if (_debug) cout << "bus access " << a->id() << (way_in ? " IN" : " OUT") << endl;
			switch (a->source())
			{
			case BusAccess::BANK_USE:												// R/W to a memory bank
				bu = (const BankUse *) a->addr();									// get bank use
				ba = bu->address();													// get bank address
				//DH: 10/11/2088
				if (bu->_is_dynamic==NULL)
					data = ba->data();
				else
					data = bu->_is_dynamic;
				if (_debug) cout << "bank use for " << data->name() << endl;
				if (!dot_mem_nodes.search(data->name(), &mem_node))  				// not found in map, error
				{
					cerr << "Internal error: unable to find node " << data->name() << endl;
					exit(1);
				}
				mem_node->bankUse(bu);
				switch (mem_node->type())
				{
				case DotMemNode::INPUT:
				case DotMemNode::OUTPUT:
					if (_debug) cout << "INPUT/OUTPUT" << endl;
					// nothing to do
					break;
				case DotMemNode::CONSTANT:
				case DotMemNode::VARIABLE:
					if (_debug) cout << "CONSTANT/VARIABLE" << endl;
					if (bu->type() == BankUse::READ)	mem_node->addReads(1);
					else								mem_node->addWrites(1);
					break;
				case DotMemNode::AGING:
					if (_debug) cout << "AGING" << endl;
					if (bu->type() == BankUse::READ)	mem_node->addReads(1);
					else								mem_node->addWrites(1);
					break;
				case DotMemNode::LOOPBACK:
					if (_debug) cout << "LOOPBACK" << endl;
					if (bu->type() == BankUse::READ)
					{
						mem_node->addReads(1);
					}
					else
					{
						cerr << "Internal error: found a WRITE of " << data->name() << " at [" << bu->time_start() <<":" << bu->time_start()  << "] in bank " << bu->bank() << endl;
						exit(1);
					}
					break;
				case DotMemNode::LOOPBACK_WRITE:
					if (_debug) cout << "LOOPBACK_WRITE" << endl;
					if (bu->type() == BankUse::WRITE)
					{
						dot_mem_nodes.search(data->name(), &mem_node);
						mem_node->addWrites(1);
					}
					else
					{
						cerr << "Internal error: found a READ of " << data->name() << " at [" << bu->time_start() <<":" << bu->time_start()  << "] from bank " << bu->bank() << endl;
						exit(1);
					}
					break;
					// EJ 14/02/2008 : dynamic memory
				case DotMemNode::DYNAMIC:
					if (_debug) cout << "DYNAMIC" << endl;
					if (bu->type() == BankUse::READ)	mem_node->addReads(1);
					else								mem_node->addWrites(1);
					break;
					// End EJ
				}
				break;
			case BusAccess::IO:														// communication with env.
				if (_debug) cout << "IO" << endl;
				// nothing to do
				break;
			}
		}
	}

	// list of "memory points"
	of << "# ADRESSE DES CASES MEMOIRE DE L'UNITE DE MEMORISATION" << "\n";
	of << "LISTEPOINTS : " << (memory_points + comm_points) << "\n";
	of << "# Adresse |    Lecture |   Ecriture |       Type |  Evolution |  Tps Debut |    Tps Fin |   Banc/addr ;" << "\n";
//        y_old |          1 |          0 |   Vieillis |          y |          0 |       4000 |       0/29 ;
//     const_12 |          2 |          0 |  Constante |   const_12 |          0 |       4000 |        0/2 ;
//      const_9 |          2 |          0 |  Constante |    const_9 |          0 |       4000 |       0/11 ;
//      const_3 |          6 |          0 |  Constante |    const_3 |          0 |       4000 |        0/6 ;
//      const_6 |         12 |          0 |  Constante |    const_6 |          0 |       4000 |        0/9 ;

	DotMemNodeRefs::const_iterator it;
	for (it = dot_mem_nodes.begin(); it != dot_mem_nodes.end(); it++)
	{
		const DotMemNode *n = (*it).second;
		const Data *d = n->data();
		long reads, writes;
		reads = writes = 0;
		string type, evol, address;
		string full_address = n->fullAddress();
		switch (n->type())
		{
		case DotMemNode::INPUT:
		case DotMemNode::OUTPUT:
			// nothing to do
			continue;
		case DotMemNode::CONSTANT:
			type = "Constante";
			evol = d->name();
			break;
		case DotMemNode::VARIABLE:
			// DH: 22/10/2008 don't process normal variable 
			if (d->bank()==-1)
				continue;	
			type = "Variable";
			evol = d->name();
			break;
		case DotMemNode::LOOPBACK:
			type = "Adaptation";
			// EJ 05/02/2008 : correction .mem
			evol = d->writevar()->name();
			//evol = d->name();
			// End EJ
			break;
		case DotMemNode::LOOPBACK_WRITE:
			type = "Variable";
			evol = d->name();
			/*// EJ 05/02/2008 : correction .mem
			//continue;
			// End EJ*/
			break;
		case DotMemNode::AGING:
			type = "Vieillis";
			evol = d->writevar()->name();
			break;
			// EJ 14/02/2008 : dynamic memory
		case DotMemNode::DYNAMIC:
			if (d->dynamic_address()>=0) type = "DynamicElement";
			else						type = "DynamicAccess";
			evol = d->dynamic_access()->name();
			string bank = Parser::itos(d->bank());
			string addr = Parser::itos(d->address());
			full_address = bank + "/" + addr;
			break;
			// End EJ
		}
		of << setw(WIDTH)	<< n->name()		<< " | ";
		of << setw(WIDTH)	<< n->reads()		<< " | ";
		of << setw(WIDTH)	<< n->writes()		<< " | ";
		of << setw(WIDTH)	<< type				<< " | ";
		of << setw(WIDTH)	<< evol				<< " | ";
		of << setw(WIDTH)	<< d->asap()		<< " | ";
		of << setw(WIDTH)	<< d->alap()		<< " | ";
		of << setw(WIDTH)	<< full_address << " ; ";
		of << "\n";
	}
	of << "\n";

	// list of "communication points"
	of << "# ADRESSE DES CASES MEMOIRE DE L'UNITE DE COMMUNICATION" << "\n";
	of << "# Adresse |    Lecture |   Ecriture |       Type |  Evolution |  Tps Debut |    Tps Fin ;" << "\n";
//				   cv_ok |          0 |          1 |   Port_Out |      cv_ok |        340 |        340 ;
//				   bit_d |          0 |          1 |   Port_Out |      bit_d |        350 |        350 ;
//				   alarm |          0 |          1 |   Port_Out |      alarm |        320 |        320 ;
//				       y |          1 |          0 |    Port_in |          y |          0 |          0 ;
	for (it = dot_mem_nodes.begin(); it != dot_mem_nodes.end(); it++)
	{
		const DotMemNode *n = (*it).second;
		const Data *d = n->data();
		string type;
		long read, write;
		read = write = 0;
		switch (n->type())
		{
		case DotMemNode::INPUT:
			type = "Port_in";
			read = 1;
			break;
		case DotMemNode::OUTPUT:
			type = "Port_Out";
			write = 1;
			break;
		case DotMemNode::CONSTANT:
		case DotMemNode::LOOPBACK_WRITE:
		case DotMemNode::VARIABLE:
		case DotMemNode::LOOPBACK:
		case DotMemNode::AGING:
		case DotMemNode::DYNAMIC:
			continue;
		}
		of << setw(WIDTH)	<< d->name()	<< " | ";
		of << setw(WIDTH)	<< read			<< " | ";
		of << setw(WIDTH)	<< write		<< " | ";
		of << setw(WIDTH)	<< type			<< " | ";
		of << setw(WIDTH)	<< d->name()	<< " | ";
		of << setw(WIDTH)	<< d->asap()	<< " | ";
		of << setw(WIDTH)	<< d->alap()	<< " ; ";
		of << "\n";
	}
	of << "\n";

	// list of bus accesses for all memory and communication points.
	of << "LISTEACCES : " << bout->accessNumber() << "\n";
	of << "#   Debut |        Fin |     NumBus/NumBusAdr |     NumReg |       Sens |    Adresse |    Tranche |       ASAP |       ALAP ;" << "\n";
//					 120 |        130 |         35 |         51 |    Lecture |    const_4 |          1 |        120 |        130 ;
//					 290 |        300 |         24 |         40 |    Lecture |    const_4 |          1 |        290 |        300 ;
//					 330 |        340 |          4 |          4 |   Ecriture |      cv_ok |          1 |        330 |        340 ;
//					 340 |        350 |          4 |          4 |   Ecriture |      bit_d |          1 |        340 |       4000 ;
//					 310 |        320 |         22 |         37 |   Ecriture |      alarm |          1 |        310 |        320 ;
//					 -10 |          0 |          4 |          4 |    Lecture |          y |          1 |        -10 |         10 ;
	bool error = false;
	for (b_it = bout->cycles().begin(); b_it != bout->cycles().end(); b_it++)  		// scan all cycles
	{
		const BusCycle *cycle = *b_it;												// get current cycle
		//long time = cycle->time();													// get cycle time
		BusCycle::ACCESSES::const_iterator a_it;									// access iterator
		const BankUse *bu;															// pointer to a bank use
		const BankAddress *ba;														// pointer to a bank address
		const Data *data;															// pointer to a data
		for (a_it = cycle->accesses.begin(); a_it != cycle->accesses.end(); a_it++) 	// scan all accesses
		{
			const BusAccess *a = *a_it;										// get current access
			// compute info
			bool way_in = (a->type() == BusAccess::IN);
			string way = way_in ? "Lecture" : "Ecriture";
			long bus = a->id() + 1; // buses are numbered from 1 to N in VHDL
			long bus_address = -1;  // address bus not use
			string address;
			long asap = 0;
			long alap = 0;
			long slice;
			if (a->source() == BusAccess::BANK_USE)
			{
				bu = (const BankUse *) a->addr();
				ba = bu->address();
				//DH: 10/11/2088
				if (bu->_is_dynamic==NULL)
					data = ba->data();
				else
				{
					data = bu->_is_dynamic;
					bus_address = bu->_address_bus + 1;
				}
				address = data->name();
				asap = data->asap();
				alap = data->alap();
				//donnee en memoire
				//fin  de lecture ou écriture
				//pour une lecture bu->real_time()=data->start()-mem_access
				//pour une ecriture bu->real_time()=data->start()+clk->period()
				slice = 1 + (bu->real_time()+sw->memory_access_time()) / cadency->length(); 
			}
			else
			{
				data = (const Data *) a->addr();
				//DH 22/10/2008 : Skip normal variable 
				if ((data->type() == Data::VARIABLE) && !data->aging() && !data->writevar() && !data->readvar() && !data->dynamic_access() && (data->bank()==-1))
					continue;
				address = data->name();
				asap = data->asap();
				alap = data->alap();
				if (way_in)
				{
						slice = 1 + data->start() / cadency->length();
				}
				else
				{
					slice = 1 + (data->start()+clk->period()) / cadency->length();
				}
			}
			// start , stop
			long start = a->time() % cadency->length();
			long stop  = a->time() % cadency->length();
			//cas d'une ecriture en memoire: la donnée est presente de a->time-clk_period a a->time
			if ((a->source() == BusAccess::BANK_USE) && (!way_in))
			{
				start = (a->time()-clk->period()) % cadency->length();
			}
			//cas d'une lecture en memoire: la donnée est presente de a->time-mem_access à a->time
			if ((a->source() == BusAccess::BANK_USE) && (way_in))
			{
				start  = (a->time()-mem->access_time()) % cadency->length();
			}
			//cas d'une lecture de l'UT depuis l'UCOM : la donnée est présente sur le bus de a->time-clk_period a a->time
			if ((way_in) && (a->source() == BusAccess::IO)) //cas d'une entree IO
			{
				start = (a->time()-clk->period()) % cadency->length();
			}
			if (!(way_in) && (a->source() == BusAccess::IO)) //cas d'une sortie IO								
			{
				//cas d'une ecriture de l'UT vers l'UCOM
				start = (a->time()-clk->period()) % cadency->length();
			}
			if ((stop == cadency->length()) || (stop == 0))
			{
				start = -clk->period();
				stop  = 0;
			}
			if (data->type() == Data::CONSTANT) slice = 1;

			const Reg * reg = data->reg();
			long reg_no;
			//skip reg no for input only used in delay
			if ((data->type()==Data::INPUT ) && (data->successors.empty()) && (data->readvar()))
			{
				reg_no = -1;
			}
			else if (Reg::is_chained(data))
			{
				reg_no = -1;
			}
			else reg_no = (reg) ? reg->no() : -999;
		/*// EJ 05/02/2008 : correction .mem
			if (data->type() == Data::VARIABLE)
			{
				if (data->readvar() && !data->readvar()->aging())
				{
					address = data->readvar()->name();
					slice = -slice;
				}
			}
			// End EJ*/
			// print info
			of << setw(WIDTH)	<< start	<< " | ";
			of << setw(WIDTH)	<< stop		<< " | ";
			of << setw(WIDTH)	<< bus		;
			if ((about!=NULL) && (about->buses()>0)) of <<"/"<< bus_address;
			of << " | ";
			of << setw(WIDTH)	<< reg_no	<< " | ";
			of << setw(WIDTH)	<< way		<< " | ";
			of << setw(WIDTH)	<< address	<< " | ";
			of << setw(WIDTH)	<< slice	<< " | ";
			of << setw(WIDTH)	<< asap		<< " | ";
			of << setw(WIDTH)	<< alap		<< " ; ";
			of << "\n";

			/* EJ 30/11/2006 temporary patch */
			if (data->type() == Data::VARIABLE)
			{
				if (data->writevar())
				{
					if (data->start() >= data->writevar()->start() && data->aging() == false)
					{
						cerr << "Internal error: critical loop is not respected" << endl;
						cerr << data->name() << " at " << data->start() << " > " << data->writevar()->name() << " at " << data->writevar()->start() << endl;
						error = true;
					}
				}

			}
			/* End EJ */
		}
	}
	of << endl;

	// close file
	of.close();
	if (error == true) exit(1);
	memory_points = 0;
	comm_points = 0;
	dot_mem_nodes.clear();		// free memory
}

//  end of: dot_mem.cpp
