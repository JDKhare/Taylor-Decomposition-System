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

//	File:		bus.cpp
//	Purpose:	Buses
//	Author:		Pierre Bomel, LESTER, UBS

#include <iostream>
#include "bus.h"
#include "reg.h"
#include "multimode_tools.h"
using namespace std;

// BusAccess class

// Function for print a(0) for ALPAREN0RPAREN
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
// Function for print a(0) for ALPAREN0RPAREN
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

BusAccess::BusAccess(long id, TYPE t, SOURCE s, const void * addr, long time)
		: _busID(id), _type(t), _source(s), _addr(addr), _time(time)
{
	/*cout << this << " new access on bus " << id << " at " << time;
	if (s == BANK_USE)
		cout << " bank use " << ((const BankUse*)addr)->address()->data()->name();
	else
		cout << " IO" << ((const Data*)addr)->name();
	cout << endl;
	*/
}

bool BusAccess::okBus(const Data *data) const
{
	//if (1) cout << this << " access on bus " << _busID << " at time " << _time << " for data " << data->name();
	bool res;
	if (_source == BANK_USE)
	{
		const BankUse *bu = (const BankUse *) _addr;
		const BankAddress *ba = bu->address();
		res = (ba->data() == data);
	}
	else
	{
		const Data * d = (const Data *) _addr;
		res = (d == data);
	}
	//if (1) cout << (res ? " OK" : " NOK") << endl;
	return res;
}

long BusAccess::busAccessWith() const
{
	long result=0;
	const Data * d;
	if (_source == BANK_USE)
	{
		const BankUse *bu = (const BankUse *) _addr;
		const BankAddress *ba = bu->address();
		d = ba->data();
	}
	else
	{
		d = (const Data *) _addr;
	}
	/* if we want force bus output width with internal reg width
		uncomment these lines
	if (d->reg())
		result=d->reg()->bitwidth();
	else */
	result=d->bitwidth();
	return result;
}

void BusAccess::info() const
{
	cout << "  bus=" << _busID << " type=" << ((_type == IN) ? "in" : "out");
	if (_source == BANK_USE)
	{
		cout << " source=" << "bank" << endl;
		const BankUse *bu = (const BankUse *) _addr;
		bu->info();
		const BankAddress *ba = bu->address();
		cout << "  data=" << ba->data()->name();
	}
	else
	{
		cout << " source=" << "io" << endl;
		const Data *data = (const Data *) _addr;
		cout << "  data=" << data->name();
	}
	cout << endl;
}

// BusCycle class

long BusCycle::getFreeBus(long buses) const
{
	if (accesses.size() == buses) return -1;
	// build a vector of "used" marks initialized at false.
	vector<bool> used_buses(buses);
	long bus;
	for (bus = 0; bus < buses; bus++)
	{
		used_buses[bus] = false;
	}
	// mark used buses
	for (ACCESSES::const_iterator i = accesses.begin(); i != accesses.end(); i++)
	{
		const BusAccess *ba = *i;		// get current bus access
		used_buses[ba->id()] = true;	// mark bus "used"
	}
	// extract free bus.
	for (bus = 0; bus < buses; bus++)
	{
		if (!used_buses[bus]) return bus;///GL
	}
	// error case, should never happen.
	cerr << "Internal error: unable to find a free bus" << endl;
	exit(1);
}

long BusCycle::getFreeBus(long buses, long in_bus, bool is_input, bool isstructural) const
{
	if (accesses.size() == buses) return -1;
	// build a vector of "used" marks initialized at false.
	vector<bool> used_buses(buses);
	long bus;
	// Caaliph 04/09/08
	if ((isstructural == true) && (is_input == false))
	{
		if (in_bus == buses) return -1;
		for (bus = 0; bus < in_bus; bus++)
		{
			used_buses[bus] = true;
		}
		for (bus = in_bus; bus < buses; bus++)
		{
			used_buses[bus] = false;
		}
	}
	else
	{
		// End Caaliph
		for (bus = 0; bus < buses; bus++)
		{
			used_buses[bus] = false;
		}
		// Caaliph 04/09/08
	}
	// End Caaliph
	// mark used buses
	for (ACCESSES::const_iterator i = accesses.begin(); i != accesses.end(); i++)
	{
		const BusAccess *ba = *i;		// get current bus access
		used_buses[ba->id()] = true;	// mark bus "used"
	}
	// extract free bus.
	for (bus = 0; bus < buses; bus++)
	{
		if (!used_buses[bus]) return bus;
	}
	// Caaliph 04/09/08
	if ((isstructural == true) && (is_input == false))
		if (used_buses[in_bus]==true) return -1;
	// End Caaliph
	// error case, should never happen.
	cerr << "Internal error: unable to find a free bus" << endl;
	exit(1);
}

/* GL 18/09/08 : replace lines
long BusCycle::getFreeBusWithIOconstraints(const Data *d, BusOut *bout, long buses) const
{
	if (accesses.size() == buses) return -1;
	// build a vector of "used" marks initialized at false.
	vector<bool> used_buses(buses);
	long bus;
	for (bus = 0; bus < buses; bus++)
	{
		used_buses[bus] = false;
	}
	// mark used buses
	for (ACCESSES::const_iterator i = accesses.begin(); i != accesses.end(); i++)
	{
		const BusAccess *ba = *i;		// get current bus access
		used_buses[ba->id()] = true;	// mark bus "used"
	}
	// extract free bus.
	for (bus = 0; bus < buses; bus++)
	{
		if (!used_buses[bus])
		{
			if (d->type() == Data::OUTPUT)
			{
				if (bout->BusHasAnInputAccess(bus)) continue;
			}
			else
			{
				if (bout->BusHasAnOutputAccess(bus)) continue;
			}
			return bus;
		}
	}
	// not free access with IOconstraints
	return -1;
}
/* With */
long BusCycle::getFreeBusWithIOconstraints(const Data *d, BusOut *bout, long buses) const
{
	// build a vector of "used" marks initialized at false.
	vector<bool> used_buses(buses);
	long bus;
	for (bus = 0; bus < buses; bus++)
	{
		used_buses[bus] = false;
	}
	// mark used buses
	for (ACCESSES::const_iterator i = accesses.begin(); i != accesses.end(); i++)
	{
		const BusAccess *ba = *i;		// get current bus access
		used_buses[ba->id()] = true;	// mark bus "used"
	}
	// extract free bus.
	for (bus = 0; bus < buses; bus++)
	{
		if (bout->port(bus) != ((Data *)d)->port()) continue;
		if (!used_buses[bus]) return bus;
		/* else
		{
			// error case, should never happen.
			cerr << "Internal error: bus " << bus << " is not a free access with IOconstraints" << endl;
			exit(1);
		} */
	}
	return -1;
}
/* End GL */
void BusCycle::addBankUse(long bus, const BankUse *bu, long bus_access_time)
{
		BusAccess::TYPE a = (bu->type() == BankUse::READ) ? BusAccess::IN : BusAccess::OUT;
		//DH: 22/11/2008 accesses.push_back(new BusAccess(bus, a, BusAccess::BANK_USE, bu, bu->time_start()));
		accesses.push_back(new BusAccess(bus, a, BusAccess::BANK_USE, bu, bus_access_time));
}

void BusCycle::addIO(long bus, const Data *data,long bus_access_time)
{
//		if (1) {cout << this << " adding IO " << data->name() << " on bus " << bus << endl;}
	BusAccess::TYPE a = (data->type() == Data::INPUT) ? BusAccess::IN : BusAccess::OUT;
	accesses.push_back(new BusAccess(bus, a, BusAccess::IO, data, bus_access_time));
}

/* DH: comment use of looback write value */
void BusCycle::addVariable(long bus, const Data *data,long bus_access_time)
{
	if (1)
	{
		cout << this << " adding Variable " << data->name() << " on bus " << bus << endl;
	}
	BusAccess::TYPE a = BusAccess::OUT;
	accesses.push_back(new BusAccess(bus, a, BusAccess::IO, data, bus_access_time));
}

const BusAccess * BusCycle::getBus(const Data *data) const
{
	ACCESSES::const_iterator it;
	for (it = accesses.begin(); it != accesses.end(); it++)
	{
		const BusAccess *ba = *it;
		if (ba->okBus(data)) return ba;	// found
	}
	return 0;	// not found
}

void BusCycle::info() const
{
	cout << "  time=" << _time << endl;
	ACCESSES::const_iterator it;
	for (it = accesses.begin(); it != accesses.end(); it++)
	{
		const BusAccess *ba = *it;
		ba->info();
	}
}

// BusOut class

const BusCycle * BusOut::getBusCycle(long time) const
{
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		if (bc->time() == time) return bc;
	}
	return 0;
}
 
const BusAccess * BusOut::getBus(long cycle, const Data *data) const
{
	if ((cycle < /*DH : add indice -1 for input read at 0// 0 */ -1) || (cycle >= _cycles.size()-1))
	{
		cerr << "Internal error: out of bound access to cycles array" << endl;
		exit(1);
	}
	return _cycles[cycle+1]->getBus(data);
}

void BusOut::info() const
{
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		cout << "Bus cycle " << (i - _cycles.begin()) << endl;
		bc->info();
	}
}

long BusOut::accessNumber() const
{
	int nb = 0;
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			nb++;
		}
	}
	return nb;
}


long BusOut::maxBusWith() const
{
	long maxWith = 0;
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;
			if (ba->busAccessWith()>maxWith)
				maxWith=ba->busAccessWith();
		}
	}
	return maxWith;
}


bool BusOut::OnlyBusInputAccess(long bus) const
{
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;
			if (ba->id() != bus) continue;
			if (ba->type() != BusAccess::IN)
				return false;
		}
	}
	return true;

}


bool BusOut::OnlyBusOutputAccess(long bus) const
{
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;
			if (ba->id() != bus) continue;
			if (ba->type() != BusAccess::OUT)
				return false;
		}
	}
	return true;
}


bool BusOut::BusHasAnInputAccess(long bus) const
{
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;
			if (ba->id() != bus) continue;
			if (ba->source() != BusAccess::BANK_USE)
			{
				const Data *data = (const Data *) ba->addr();
				if (data->type() == Data::INPUT) return true;
			}
		}
	}
	return false;
}

bool BusOut::BusHasAnOutputAccess(long bus) const
{
	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;
			if (ba->id() != bus) continue; //DH : 2/02/2009 add this test
			if (ba->source() != BusAccess::BANK_USE)
			{
				const Data *data = (const Data *) ba->addr();
				if (data->type() == Data::OUTPUT)
					return true;
			}
		}
	}
	return false;
}

long BusOut::IsAnInOutAccessOnBus(const Data *data) const
{

	const Data *d;

	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;

			if (ba->source() == BusAccess::BANK_USE)
			{
				const BankUse *bank_use = (const BankUse *) ba->addr();
				const BankAddress *bank_address = bank_use->address();
				d = bank_address->data();
			}
			else
			{
				d = (const Data *) ba->addr();
			}
			if (data->name() == d->name()) return ba->id(); // true
		}
	}
	return -1; // false
}

long BusOut::getTimeDataAccess(const Data *data) const
{

	const Data *d;

	for (CYCLES::const_iterator i = _cycles.begin(); i != _cycles.end(); i++)
	{
		const BusCycle *bc = *i;
		BusCycle::ACCESSES::const_iterator a_it;
		for (a_it = bc->accesses.begin(); a_it != bc->accesses.end(); a_it++)
		{
			const BusAccess *ba = *a_it;

			if (ba->source() == BusAccess::BANK_USE)
			{
				const BankUse *bank_use = (const BankUse *) ba->addr();
				const BankAddress *bank_address = bank_use->address();
				d = bank_address->data();
			}
			else
			{
				d = (const Data *) ba->addr();
			}
			if (data->name() == d->name()) return ba->time();
		}
	}
	cerr << "fatal error : data " << data->name() << " has no access on bus" << endl;
	exit(1);
}



// BusSynthesis class



void BusSynthesis::scanBank(
    const Bank::USES & uses,					// in
    const	Cadency			*cadency,			// in
    const	Clock			*clk,				// in
    const Mem *mem,								// in
    BusOut		*bout,							// out
    BusOut		*about							// out // EJ 14/02/2008 : dynamic memory
)
{
	// variables
	Bank::USES::const_iterator u_it;			// to scan banks' accesses
	const BankUse *use;
	long bus_access_time;
	BusCycle * bc, * abc;
	long bus;
	DataRefs constants_read;	// the set of constants read
	Data *constant;

	// scan all reads/writes of the bank
	for (u_it = uses.begin(); u_it != uses.end(); u_it++)
	{
		use = &(*u_it).second;	// get current bank use
		//update in case of static WRITE : data bus is used from bankuse_start to  bankuse_end (=bank_use_start+mem_access)
		//see (allocateStaticSlot in mapping_constraints.cpp)
		if ((use->_is_dynamic==NULL) && (use->type() == BankUse::WRITE))
			bus_access_time = use->time_start() % cadency->length();
		//dynamic write : data bus is use during  bank_use-> bank_use+mem_access
		else if ((use->_is_dynamic!=NULL) && (use->type() == BankUse::WRITE))
			bus_access_time = use->time_start() % cadency->length();
		//static read : data bus is read at bank_use->end=use->time_start()+mem->access_time()
		else if ((use->_is_dynamic==NULL) && (use->type() == BankUse::READ))
			bus_access_time = (use->time_start()+mem->access_time()) % cadency->length();
		//dynamic read : data bus is read at bank_use->end=use->time_start()+mem->access_time()
		else if ((use->_is_dynamic!=NULL) && (use->type() == BankUse::READ))
			bus_access_time = (use->time_start()+mem->access_time()) % cadency->length();
		const Data *data = use->address()->data();
		if (use->_is_dynamic!=NULL)
			data = use->_is_dynamic;

		if (_debug) cout << "  Bus: bank access at time=" << bus_access_time << " data=" << data->name() << endl;

		// check constants are read a single time
		if (data->type() == Data::CONSTANT)
		{
			if (_debug) cout << "is a constant" << endl;
			if (data->hardwire())   // EJ 26/10/2007 hardwire constant
			{
				if (_debug) cout << "hardwire constant" << endl;
				continue;
			}
			if (constants_read.search(data->name(), &constant))
			{
				if (_debug) cout << "already processed" << endl;
				continue;	// already processed
			}
			constants_read.add(DataRef((Data *)data));
		}

		// find the bus cycle
		if (!(bc = (BusCycle *) bout->getBusCycle(bus_access_time)))
		{
			cerr << "Internal error: unable to find a bus cycle at time " << bus_access_time << endl;
			exit(1);
		}
		if (_debug) 
		{
			cout << "  Bus: found a bus cycle at " << bus_access_time << endl;
			use->info();
		}
		// find the bus ID
		if ((bus = bc->getFreeBus(bout->buses())) == -1)   // no free bus
		{
			bus = bout->newBus();			// allocate a new bus
			if (_debug) cout << "  Bus: allocated new bus=" << bus << endl;
		}
		else
			/* GL 29/09/08 : replace lines */
			if (_ioConstraints && bout->is_locked(bus))  // no free bus
			{
				if (_debug) cout << "  Bus: bus " << bus << " is locked for IO use " << endl;
				bus = bout->newBus();			// allocate a new bus
				if (_debug) cout << "  Bus: allocated new bus=" << bus << endl;
			}
			else
				/* End GL */
		{
			if (_debug) cout << "  Bus: found free bus=" << bus << endl;
		}
		// add the access to the bus cycle
		bc->addBankUse(bus, use,bus_access_time);
		// Caaliph 29/08/08
		((Data*)data)->setbusid(bus);

		if (use->_is_dynamic!=NULL) 
		{
			long buss_addr_access_time = (use->time_start() - clk->period()) % cadency->length();
			////DH: 22/11/2008 duplication d'info ((Data*)data->dynamic_access())->_dynamic_data_bus = bus;
			// find the bus cycle
			if (!(abc = (BusCycle *) about->getBusCycle(buss_addr_access_time)))
			{	
				cerr << "Internal error: unable to find a address bus cycle at time " << buss_addr_access_time << endl;
				exit(1);
			}
			if (_debug)
			{
				cout << "  Address Bus: found a bus cycle at " << buss_addr_access_time << endl;
				use->info();
			}
			// find the address bus ID
			if ((bus = abc->getFreeBus(about->buses())) == -1)   // no free bus
			{
				bus = about->newBus();			// allocate a new bus
				if (_debug) cout << "  Address Bus: allocated new bus=" << bus << endl;
			}
			else
			{
				if (_debug) cout << "  Address Bus: found free bus=" << bus << endl;
			}
			// add the access to the bus cycle
			abc->addBankUse(bus, use,buss_addr_access_time);
			// copy info to bout class
			((BankUse*)use)->_address_bus = bus;
			// copy info to data class
			((Data*)data->dynamic_access())->_dynamic_address_bus = bus;
		}
	}
}

void BusSynthesis::scanIOs(
    const vector<CDFGedge*> &nodes,
    const	Cadency			*cadency,			// in
    const	Clock			*clk,				// in
    const Mem *mem,
    bool is_input,
    BusOut *bout
)
{

	long bus_access_time;
	BusCycle * bc;
	long bus;
	// Caaliph 04/09/08
	long in_bus;
	bool first=false;
	// end Caaliph

	vector<CDFGedge*>::const_iterator it;		// to scan CDFG nodes
	for (it = nodes.begin(); it != nodes.end(); it++)
	{
		const CDFGedge *edge = *it;
		const CDFGnode *node = is_input ? edge->target : edge->source;
		const Data *d = (const Data *) node;
		if ((d->type() != Data::INPUT) && ((d->type() != Data::OUTPUT))) continue; // process only IOs
		if (_debug) cout << "  Bus: found data " << d->name() << endl;
		
		if (d->type() == Data::INPUT)
			bus_access_time = d->start() % cadency->length();
		else
			bus_access_time = (d->start()+clk->period()) % cadency->length();

		if (_debug) cout << "  Bus: IO at time=" << bus_access_time << endl;
		// find the bus cycle
		if (!(bc = (BusCycle *) bout->getBusCycle(bus_access_time)))
		{
			cerr << "Internal error: unable to find a bus cycle at time " << bus_access_time << endl;
			exit(1);
		}
		if (_debug) cout << "  Bus: found a bus cycle at " << bus_access_time << endl;
		// find the bus ID

		// Caaliph 04/09/08
		if ((_isstructural==true) && (is_input==false) && (first==false))
		{
			_in_bus = bout->buses();
			first = true;
		}
		/* GL 18/09/08 : replace line
		bus = bc->getFreeBus(bout->buses());
		/* with */
		if (_ioConstraints) bus = bc->getFreeBusWithIOconstraints(d, bout, bout->buses()); /// ligne commentées// End GL
		else				bus = bc->getFreeBus(bout->buses(), _in_bus, is_input, _isstructural);
		// End Caaliph

		if (bus == -1)  									// no free bus
		{
			bus = bout->newBus();							// allocate a new bus
			if (_debug) cout << "  Bus: allocated new bus=" << bus << endl;
			/* GL 18/09/08 : add lines */
			// update port under IOconstraints
			if (_ioConstraints)
			{
				bout->port(bus,((Data*)d)->port());
				bout->lock(bus);
				if (_debug) cout << "  Bus: Port " <<  ((Data*)d)->port() << " is affected to bus " << bus << endl;
			}
			/* End GL */
		}
		else
		{
			if (_debug) cout << "  Bus: found free bus=" << bus << endl;
		}
		// add the access to the bus cycle
		bc->addIO(bus, d,bus_access_time);

		// Caaliph 28/08/08
		((Data*)d)->setbusid(bus);
		// End Caaliph
		if (_ioConstraints)
		{
			cout << "Bus " << bus << /*GL */" (Port " << bout->port(bus) << ")" <</* GL*/ " has IO " << deconvertName((char*)d->name().c_str()) << " at time " << bus_access_time << endl;
		}
	}
}

void BusSynthesis::scanVariables(
    const vector<CDFGedge*> &nodes,
    const	Cadency			*cadency,			// in
    const	Clock			*clk,				// in
    const Mem *mem,
    bool is_input,
    BusOut *bout
)
{

	long bus_access_time;
	BusCycle * bc;
	long bus;

	vector<CDFGedge*>::const_iterator it;		// to scan CDFG nodes
	for (it = nodes.begin(); it != nodes.end(); it++)
	{
		const CDFGedge *edge = *it;
		const CDFGnode *node = is_input ? edge->target : edge->source;
		const Data *d = (const Data *) node;
		if (d->type() != Data::VARIABLE)	continue;	// process only Variables
		if (d->aging() != true)				continue;	// process only aging
		if (d->writevar()->writevar())		continue;	// process only head
		if (d->writevar()->type() != Data::VARIABLE) continue; // not process IO

		if (_debug) cout << "  Bus: found data " << d->writevar()->name() << endl;
		bus_access_time = (d->writevar()->start()+clk->period()) % cadency->length();


		if (_debug) cout << "  Bus: Variable at time=" << bus_access_time << endl;
		// find the bus cycle
		if (!(bc = (BusCycle *) bout->getBusCycle(bus_access_time)))
		{
			cerr << "Internal error: unable to find a bus cycle at time " << bus_access_time << endl;
			exit(1);
		}
		if (_debug) cout << "  Bus: found a bus cycle at " << bus_access_time << endl;
		// find the bus ID
		if ((bus = bc->getFreeBus(bout->buses())) == -1)   // no free bus
		{
			bus = bout->newBus();			// allocate a new bus
			if (_debug) cout << "  Bus: allocated new bus=" << bus << endl;
		}
		else
		/* GL 29/09/08 : replace lines */
			if (_ioConstraints && bout->is_locked(bus))  // no free bus
			{
				if (_debug) cout << "  Bus: bus " << bus << " is locked for IO use " << endl;
				bus = bout->newBus();			// allocate a new bus
				if (_debug) cout << "  Bus: allocated new bus=" << bus << endl;
			}
			else
		/* End GL */
		{
			if (_debug) cout << "  Bus: found free bus=" << bus << endl;
		}
		// add the access to the bus cycle
		bc->addVariable(bus, d->writevar(),bus_access_time);
		// Caaliph 29/08/08
		((Data*)d->writevar())->setbusid(bus);
	}
}

BusSynthesis::BusSynthesis(
    const	Cadency			*cadency,			// in
    const	Clock			*clk,				// in
    const	Mem				*mem,				// in
    const	CDFG			*cdfg,				// in
    const	SchedulingOut	*sched,				// in
    BusOut			*bout,				// out
    BusOut			*about,				// out // EJ 14/02/2008 : dynamic memory
    Switches	*sw
)
{
	//_debug=true;
	// Caaliph 04/09/08
	_ioConstraints = sw->ioConstraints();
	_isstructural = sw->dumpSTAR_CDFG();

	// variables
	Banks::BANKS::const_iterator b_it;			// to scan banks
	const Bank *bank;

	/* GL 18/09/08 : add line */
	// 1 = look at IO schedule
	// scan all starting nodes
	if (_debug) cout << "Bus: scanning inputs" << endl;
	scanIOs(cdfg->source()->successors, cadency, clk, mem, true, bout);
	if (_debug) cout << "Bus: scanning outputs" << endl;
	scanIOs(cdfg->sink()->predecessors, cadency, clk, mem, false, bout);
	/* End GL */

	// 2 = look at RW schedule
	// scan all banks
	if (sched->banks.size())
	{
		for (b_it = sched->banks.banks.begin(); b_it != sched->banks.banks.end(); b_it++)
		{
			bank = &(*b_it).second;		// get current bank
			if (_debug) cout << "Bus: scanning bank " << bank->id() << endl;
			if (_debug) cout << "Bus: scanning READ accesses" << endl;
			scanBank(bank->reads, cadency, clk, mem, bout, about);
			if (_debug) cout << "Bus: scanning WRITE accesses" << endl;
			scanBank(bank->writes, cadency, clk, mem, bout, about);
		}
	}


	// 3 = look at Variable schedule (update aging vector)
	scanVariables(cdfg->source()->successors, cadency, clk, mem, true, bout);
}

// end of: bus.cpp
