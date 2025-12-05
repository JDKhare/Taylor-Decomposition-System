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

//	File:		mapping_constraints.cpp
//	Purpose:	Specific processing for memory mapping constraints
//	Author:		Pierre Bomel, LESTER, UBS

class MEMC;

#ifndef __MEMC_H__
#define __MEMC_H__

#include "bank.h"
#include "cdfg.h"
#include "mem.h"
#include "operation.h"
using namespace std;

//! Memory banks R/W time slots.
class Slots
{
public:
	Slots() {times.clear();}
	//Slots(const Slots &slots) {times = slots.times;}
	//Slots& operator=(const Slots &slots) {times = slots.times; return *this;}
	vector<long> times;	//!< time slots
};

//! Memory banks constraints.
class MEMC
{
private:
	Banks *_banks;											//!< the set of banks
	const Mem *_mem;										//!< the memory model
	const Cadency *_cadency;								//!< the user cadency constraint
	const CDFG *_cdfg;										//!< the CDFG
	static bool  _debug;									//!< debug

	// to store decisions and later "freeze" (lock) them
	vector<Bank *>				_saved_read_banks;			//!< to store read banks IDs
	vector<long>				_saved_read_times;			//!< to store banks read times
	vector<long>				_saved_read_real_times;		//!< to store banks read real times
	vector<const BankAddress *> _saved_read_addresses;		//!< to store banks read addresses
	vector<Bank *>				_saved_write_banks;			//!< to store write banks IDs
	vector<long>				_saved_write_times;			//!< to store banks write times
	vector<long>				_saved_write_real_times;	//!< to store banks write real times
	vector<const BankAddress *> _saved_write_addresses;		//!< to store banks write addresses

	// EJ 14/02/2008 dynamic memory
	vector<const Data *>				_saved_read_dynamic;
	vector<const Data *>				_saved_write_dynamic;

	//! Allocate a new time slot
	//! @param Input. n is a pointer to a CDFG node
	//! @param Input. limit is the max time limit
	//! @param Input. read is a boolean, true => read access, false => write access
	//!
	//! @result a boolean telling if the time slot has been allocated (true) or not (false).
	bool allocateStaticSlot(const CDFGnode *n, long limit, bool read);
	bool allocateDynamicAccessSlot(const CDFGnode *n, long limit, bool read);
	//! Allocate a new read time slot
	//! @param Input. n is a pointer to a CDFG node
	//! @param Input. limit is the max time limit
	//!
	//! @result a boolean telling if the time slot has been allocated (true) or not (false).
	bool allocateStaticReadSlot(const CDFGnode *n, long limit);
	bool allocateDynamicReadSlot(const CDFGnode *n, long limit);
	//! Allocate a new write time slot
	//! @param Input. n is a pointer to a CDFG node
	//! @param Input. limit is the max time limit
	//!
	//! @result a boolean telling if the time slot has been allocated (true) or not (false).
	bool allocateStaticWriteSlot(const CDFGnode *n, long limit);
	bool allocateDynamicWriteSlot(const CDFGnode *n, long limit);

	// to fetch the access by data
	typedef map<const Data *, Slots, less<const Data *> > SLOTMAP;	//!< local type for time slots maps.
	SLOTMAP data_reads;												//!< map of read time slots
	SLOTMAP data_writes;											//!< map of write time slots
	//! Search a data read time slot
	//! @param Input. d is a pointer to a CDFG Data node.
	//! @param Output. slots is a set of time slots: a Slots.
	//!
	//! @result a boolean telling if at least a time slot has been found (true) or not (false).
	bool searchDataRead(const Data *d, Slots **slots)
	{
		SLOTMAP::iterator it = data_reads.find(d);
		if (it == data_reads.end()) return false;
		*slots = &(*it).second;
		return true;
	}
	//! Add an empty set of read time slots for a Data.
	//! @param Input. d is a pointer to the CDFG Data.
	void addDataRead(const Data *d)
	{
		pair<SLOTMAP::iterator, bool> p = data_reads.insert(make_pair(d, Slots()));
		if (!p.second)
		{
			cerr << "Error: Mapping: data '" << d->name() << "' defined twice" << endl;
			exit(1);
		}
	}

	//! Check current planning before planning a new read/write (EJ 07/12/2007)
	bool check_current_planning(long modulo_time, const Bank *bank, const BankAddress *ba, bool read);


public:
	//! Create a new memory banks constraint.
	//! @param Input. mem is a pointer to the memory model.
	//! @param Input. cadency is a pointer to the user cadency constraint.
	//! @param Input. cdfg is the CDFG.
	//! @param Input. banks is the set of banks.
	MEMC(const Mem *mem, const Cadency *cadency, const CDFG *cdfg, Banks *banks)
			: _banks(banks), _mem(mem), _cadency(cadency), _cdfg(cdfg) {/* _debug=true;*/}
	//! Check for occurence of a read time slot for an operation.
	//! @param Input. cdfg is the CDFG.
	//! @param Input. o is a pointer to the operation.
	//! @param Input. time is the time slot time.
	//!
	//! @result a boolean telling if there is a read time slot (true) or not (false).
	bool checkReads(const CDFG *cdfg, const Operation *o, long time);
	//! Check for occurence of a write time slot for an operation.
	//! @param Input. cdfg is the CDFG.
	//! @param Input. o is a pointer to the operation.
	//! @param Input. time is the time slot time.
	//!
	//! @result a boolean telling if there is a read time slot (true) or not (false).
	bool checkWrites(const CDFG *cdfg, const Operation *o, long time);
	//! Check for occurence of a read/write time slot for an operation mem_read or mem_write.
	//! @param Input. cdfg is the CDFG.
	//! @param Input. o is a pointer to the operation.
	//! @param Input. time is the time slot time.
	//!
	//! @result a boolean telling if the bank is free at modulo_time.
	bool	check_bank_free (const Data *dynamic_access_or_element, long modulo_time);
	//! Freeze all previous decisions. i.e. the ones stored into the _saved* members.
	void freeze();
	//! Get last read time of a Data.
	//! @param Input. d is a pointer to the Data.
	//!
	//! @result a time if there is a last read time slot or -1.
	long getLastReadTime(const Data *d);

	void asap_shift(const CDFG *cdfg, const Mem *mem);
};

#endif // __MEMC_H__


