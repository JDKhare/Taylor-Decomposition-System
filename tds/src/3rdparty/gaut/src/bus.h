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

//	File:		bus.h
//	Purpose:	GAUT buses
//	Author:		Pierre Bomel, LESTER, UBS

class BusOut;
class BusSynthesis;

#ifndef __BUS_H__
#define __BUS_H__

#include <iostream>
#include <vector>
#include "cadency.h"
#include "clock.h"
#include "scheduling.h"
#include "bank.h"
#include "cdfg.h"
using namespace std;

//! Bus accesses.
class BusAccess
{
public:
	enum TYPE {IN, OUT};
	enum SOURCE {BANK_USE, IO};		//!< Only IOs and memory bank reads/writes populate can be bus accesses.
private:
	const long			_busID;		//!< Bus identity.
	const TYPE			_type;		//!< Access type.
	const SOURCE		_source;	//!< Access source type.
	const void		    *_addr;		//!< Pointer to the source.
	const long			_time;		//!< Access time.
public:
	//! Bus access creation.
	BusAccess(long id, TYPE t, SOURCE s, const void * addr, long time);
	//! Get bus ID.
	long id() const
	{
		return _busID;
	}
	//! Get type.
	TYPE type() const
	{
		return _type;
	}
	//! Get source.
	SOURCE source() const
	{
		return _source;
	}
	//! Get pointer to source.
	const void * addr() const
	{
		return _addr;
	}
	//! Get time;
	long time() const
	{
		return _time;
	}
	//! Check bus access for a given Data.
	//! @param Input. data is a pointer to the Data.
	//! @result a boolean. True = access is for Data, false otherwise.
	bool okBus(const Data *data) const;
	
	//! Get bitwith (date, register) of the bus access
	long busAccessWith() const;

	//! Print info.
	void info() const;
};

//! Bus cycles.
//! Contains zero to N (N being the number of buses) bus accesses.
class BusCycle
{
private:
	const long				_time;		//!< Cycle time.

public:
	//! Type for accesses.
	typedef vector<const BusAccess *> ACCESSES;
	ACCESSES accesses;					//!< Bus accesses at this precise cycle time.
	BusCycle(long time) : _time(time) {}//!< Bus cycles must be initialized with a time.
	//! Free of BusCycle data structures.
	~BusCycle()
	{
		// explicit free of accesses
		for (ACCESSES::iterator i = accesses.begin(); i != accesses.end(); i++) delete (*i);
	}
	long time() const
	{
		return _time;    //!< Get bus cycle time.
	}
	long getFreeBus(long buses, long in_bus, bool is_input, bool isstructural) const;
	// End Caaliph
	//! Get a free bus, with a bus number limit.
	long getFreeBus(long buses) const;
	//! Add an access to a bus cycle.
	void addBankUse(long bus, const BankUse *bu,long bus_access_time);
	//! Add an IO access to a bus cycle
	void addIO(long bus, const Data *data, long bus_access_time);
	//! Add a variable access to a bus cycle
	void addVariable(long bus, const Data *data, long bus_access_time);
	//! Get bus access for a given Data.
	//! @param Input. data is a pointer to the Data.
	//! @result a pointer to the bus access if any, or NULL otherwise.
	const BusAccess * getBus(const Data *data) const;
	//! Print info.
	void info() const;
	//! Get a free bus, with a bus number limit. (without bus bidirectionnal)
	long getFreeBusWithIOconstraints(const Data *d, BusOut *bout, long buses) const;
};

//! Bus Synthesis output data structure.
//! It contains the number of buses and the use of each bus cycle.
class BusOut
{
public:
	typedef vector<const BusCycle *> CYCLES;
	//! Create a new BusOut data structure.
	BusOut(const Cadency *cad, const Clock *clk)
	{
		_buses = 0;		// no buses at start
		// build BusOut data structure with empty bus accesses lists.
		_cycles.push_back(new BusCycle(-clk->period())); //DH: 22/11/2008 for input read at time 0
		for (long cyc = 0; cyc < cad->length(); cyc += clk->period())
			_cycles.push_back(new BusCycle(cyc));
	}
	//! Free a BusOut data structure.
	~BusOut()
	{
		// explicit free of cycles
		for (CYCLES::iterator i = _cycles.begin(); i != _cycles.end(); i++) delete (*i);
	}
	//! Get a new bus number.
	long newBus()
	{
		/* GL 29/09/08 : add lines */
		_IoLocks.push_back(false);
		/* End GL */
		return _buses++;    //!< Increment bus number and return a new bus ID.
	}
	//! Get bus cycle corresponding to "time".
	//! @result a pointer to a BusCycle or NULL.
	const BusCycle *getBusCycle(long time) const;
	//! Get number of buses.
	long buses() const
	{
		return _buses;
	}
	//! Get number of buses = buses()
	long size() const
	{
		return buses();
	}
	//! Get constant cycles set.
	const CYCLES & cycles() const
	{
		return _cycles;
	}
	//! Get bus access for a given (cycle, Data)
	//! @param Input. cycle is the cycle number.
	//! @param Input. data is a pointer to the Data.
	//! @result a pointer to the bus access if any, or NULL otherwise.
	const BusAccess * getBus(long cycle, const Data *data) const;
	//! Print info.
	void info() const;

	//! Get max bus with
	long maxBusWith() const;

	//! Get bus access number
	long accessNumber() const;
	//! BusHasAnInputAccess
	bool BusHasAnInputAccess(long bus) const;
	//! BusHasAnOutputAccess
	bool BusHasAnOutputAccess(long bus) const;
	bool OnlyBusInputAccess(long bus) const;
	bool OnlyBusOutputAccess(long bus) const;

	//! Is an InOut Access on bus ? return bus id or -1 (EJ 09/2007)
	long IsAnInOutAccessOnBus(const Data *data) const;
	//! Get time Access on data (EJ 09/2007)
	long getTimeDataAccess(const Data *data) const;
	/* GL 18/09/08 : add lines */
	//! Set port according to IOcontraints
	void port(long bus, long port)
	{
		_bus_port.resize(buses());
		_bus_port[bus] = port;
	}
	//! Get bus port number
	long port (long bus)
	{
		return _bus_port[bus];
	}
	/* End GL */
	/* GL 29/09/08 : add lines */
	//! Lock bus for IOcontraints
	void lock(long bus)
	{
		_IoLocks[bus] = true;
	}
	//! Tell if the bus is locked
	long is_locked (long bus)
	{
		return _IoLocks[bus];
	}
	/* End GL */

private:
	//! A vector of bus cycles. Each bus cycle correspond to a clock cycle.
	//! Bus cycles are stored in "time-ascending" order.
	CYCLES				_cycles;
	long				_buses;			//!< Number of buses.
	/* GL 18/09/08 : add lines */
	vector<long>  _bus_port;
	/* End GL */
	/* GL 29/09/08 : add lines */
	vector<bool>  _IoLocks;
	/* End GL */
};

//! Bus synthesis.
class BusSynthesis
{
private:

	static bool _debug; //!< For dynamic debug.
	bool _ioConstraints;
	// Caaliph 04/09/08
	bool _isinput;
	bool _isstructural;
	long _in_bus;
	// End Caaliph

	//! To scan read or write accesses of a memory bank.
	void scanBank(const Bank::USES & uses,					// in
	              const	Cadency			*cadency,			// in
	              const	Clock			*clk,				// in
	              const	Mem			*mem,					// in
	              BusOut		*bout,					// out
	              BusOut		*about					// out
	             );
	//! Scan IOs
	void scanIOs( const vector<CDFGedge*> &nodes,			// in
	              const	Cadency			*cadency,			// in
	              const	Clock			*clk,				// in
	              const	Mem			*mem,					// in
	              bool is_input,							// in
	              BusOut *bout								// out
	            );
	//! Scan Variables
	void scanVariables( const vector<CDFGedge*> &nodes,		// in
	                    const	Cadency			*cadency,			// in
	                    const	Clock			*clk,				// in
	                    const	Mem			*mem,					// in
	                    bool is_input,							// in
	                    BusOut *bout								// out
	                  );

public:

	//! Bus synthesis. The object contains all private date to run the synthesis.
	//!
	//! There should be a minimum number of buses to reduce the external connectivity
	//! of the IC. But buses depend heavily on the communication schedule: ie the
	//! number of R/W of variables in memory and number of IOs.
	//! So, to reduce the number of buses, the IO and RW schedule should be
	//! aware of this "bus optimization issue", but this is not the case now.
	//! Future work ...
	//!
	//! Today's strategy (if we can call this a strategy ...) is:
	//!	-# By looking at the memory schedule we can determine the minimum
	//!	number of buses necessary and split the RW operations among them.
	//!	-# By looking at the IO schedule, we can assign IO operations to
	//!	unused bused (if any). We'll only need to add more buses when
	//!	congestion (all buses used) occurs.
	//!
	//! @param Input. cadency is a pointer to the user cadency.
	//! @param Input. clock is a pointer to the system clock.
	//! @param Input. mem is a pointer to the memory model.
	//! @param Input. cdfg is a pointer to the scheduled CDFG.
	//! @param Input. sched is a pointer to the scheduling output.
	//! @param Output. bout is a pointer to the object containing the output of the "bus synthesis".


	BusSynthesis(
	    const	Cadency			*cadency,			// in
	    const	Clock			*clk,				// in
	    const	Mem				*mem,				// in
	    const	CDFG			*cdfg,				// in
	    const	SchedulingOut	*sched,				// in
	    BusOut			*bout,				// out
	    BusOut			*about,				// out // EJ 14/02/2008 : dynamic memory
	    Switches	*sw
	);
};

#endif // __BUS_H__

// end of: bus.h


