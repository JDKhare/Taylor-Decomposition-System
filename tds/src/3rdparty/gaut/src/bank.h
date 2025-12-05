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

//	File:		bank.h
//	Purpose:	GAUT memory banks
//	Author:		Pierre Bomel, LESTER, UBS

class Banks;
class Bank;
class BankAddress;
class BankUse;

#ifndef __BANK_H__
#define __BANK_H__

#include <iostream>
#include <map>
#include "data.h"
using namespace std;

//! An address in a bank.
class BankAddress
{
private:
	long					_address;		//!< address in the bank
	const Data				*_data;			//!< reference data
	const long				_bank_id;		//!< bank ID
public:
	//! Create a new Bank Address
	//! @param Input. address is the address in the bank.
	//! @param Input. d is a pointer to a constant Data.
	BankAddress(long address, const Data *d, long bank_id) : _address(address), _data(d), _bank_id(bank_id) {}
	//! Get the address.
	long address() const
	{
		return _address;
	}
	//! Get the pointer to the data.
	const Data * data() const
	{
		return _data;
	}
 	//! Get bank id
	long bankId() const
	{
		return _bank_id;
	}
	//BankAddress & operator=(const BankAddress & src) {_address = src._address; _data = src._data; return *this;}
	//! Print info.
	void info() const
	{
		cout << "(" << _data->name() << "," << _address << ")" << endl;
	}
};

//! An access/use to a bank.
class BankUse
{
public:
	//! Enumerated type to distinguish bank uses.
	enum Type
	{
		READ,	//!< Access is a read.
		WRITE	//!< Access is a write.
	};
private:
	const long				_bank;			//!< Bank number.
	const Type				_type;			//!< access type
	const long				_time_start;			//!< start time slot used //DH: 12/11/2008 
	const long				_time_end;			//!< end time slot used  //DH: 12/11/2008 
	const long				_real_time;		//!< real time   EJ 11/03/2008
	const BankAddress		*_address;		//!< address of the access

public:

	long _address_bus; //!< bus use to pass dynamic address. // EJ 14/02/2008 : dynamic memory
	const Data *_is_dynamic;  //!< after scheduling this use must delete

	//! Create a new Bank Use.
	//! @param Input. time is the time of the use.
	//! @param Input. address is the address in the bank.
	//! @param Input. type is the bank use type: READ/WRITE.
	BankUse(long time_start, long real_time, long time_end,long bank, const BankAddress *address, Type type, const Data *is_dynamic)
			: _bank(bank), _type(type), _time_start(time_start), _real_time(real_time), _address(address), _time_end(time_end)
	{
		_is_dynamic = is_dynamic;
	}
	//! Get start time.
	long time_start() const
	{
		return _time_start;
	}
	//! Get end time.
	long time_end() const
	{
		return _time_end;
	}
	//! Get real time
	long real_time() const
	{
		return _real_time;
	}
	//! Get type.
	long type() const
	{
		return _type;
	}
	//! Get address.
	const BankAddress * address() const
	{
		return _address;
	}
	//! Get bank.
	long bank() const
	{
		return _bank;
	}
	//! Print info.
	void info() const
	{
		cout << "  time_start=" << _time_start << " ";
		cout << "  time_end=" << _time_end << " ";
		cout << "type=" << (_type == READ ? "R" : "W");
		//DH: 10/11/2008
		if (_is_dynamic==NULL)
			_address->info();
		else
			cout << "(" << _is_dynamic->name() << ")" << endl;
	}
};

//! A Bank description.
class Bank
{
public:
	//! Local type for the accesses set.
	typedef map<	 long, BankAddress, less<long> > ADDRESSES;
	//! Local type for the uses set.
	typedef multimap<long, BankUse,     less<long> > USES;
	//! The set of addresses.
	ADDRESSES	addresses;			//!< used addresses
	//! The set of reads.
	USES		reads;				//!< reads
	//! The set of writes.
	USES		writes;				//!< writes
private:
	const long	_id;				//!< Bank number.
public:
	//! Create a new bank.
	//! @param Input. id is the bank ID.
	Bank(long id) : _id(id) {}
	//! Get bank ID.
	long id() const
	{
		return _id;
	}
	//! Search an address into a bank.
	//! @param Input. address is the address to search for.
	//! @param Output. ba is a pointer to a BankAddress.
	bool searchAddress(long address, const BankAddress **ba) const
	{
		ADDRESSES::const_iterator it = addresses.find(address);
		if (it == addresses.end()) return false;
		*ba = &(*it).second;
		return true;
	}
	//! Add an address to a bank.
	//! @param Input. address is the address to add.
	//! @param Input. d is a pointer to the Data of the access.
	void addAddress(long address, const Data *d)
	{
		pair<ADDRESSES::iterator, bool> p = addresses.insert(make_pair(address, BankAddress(address, d, _id)));
		if (!p.second)
		{
			cerr << "Error: bank(" << _id << ") address " << address << " defined twice" << endl;
			exit(1);
		}
	}
	//! Search a read access for a given time.
	//! @param Input. time is the access time.
	//! @param Output. bu is a pointer to a Bank Use.
	bool searchRead(long time, const BankUse **bu) const
	{
		USES::const_iterator it = reads.find(time);
		if (it != reads.end()) 
		{
			*bu = &(*it).second;
			return true;
		}
		else
		{
			const BankUse *buse;
			for (USES::const_iterator it = reads.begin(); it != reads.end(); it++)
			{
				buse = &(*it).second;
				if ((time >= buse->time_start()) &&
					(time < buse->time_end()))
				{
					*bu = buse;
					return true;
				}
			}
		}
		return false;
	}
	//! Add a read access for a given time.
	//! @param Input. time is the time of the access.
	//! @param Input. ba is a pointer to the bank address.
	void addRead(long time_start, long real_time, long time_end,const BankAddress *ba, const Data *is_dynamic)
	{
		if ((_id != -1) && (reads.find(time_start) != reads.end()))
		{
			cerr << "Error: bank(" << _id << ") read " << time_start << " defined twice" << endl;
			exit(1);
		}
		reads.insert(make_pair(time_start, BankUse(time_start, real_time, time_end,_id, ba, BankUse::READ, is_dynamic)));
	}
	//! Search a write access for a given time.
	//! @param Input. time is the access time.
	//! @param Output. bu is a pointer to a Bank Use.
	bool searchWrite(long time, const BankUse **bu) const
	{
		USES::const_iterator it = writes.find(time);
		if (it != writes.end())
		{
			*bu = &(*it).second;
			return true;
		}
		else
		{
			const BankUse *buse;
			for (USES::const_iterator it = writes.begin(); it != writes.end(); it++)
			{
				buse = &(*it).second;
				if ((time >= buse->time_start()) &&
					(time < buse->time_end()))
				{
					*bu = buse;
					return true;
				}
			}
		}
		return false;
	}
	//! Add a write access for a given time.
	//! @param Input. time is the time of the access.
	//! @param Input. ba is a pointer to the bank address.
	void addWrite(long time_start, long real_time, long time_end,const BankAddress *ba, const Data *is_dynamic)
	{
		if ((_id != -1) && (writes.find(time_start) != writes.end()))
		{
			cerr << "Error: bank(" << _id << ") write " << time_start << " defined twice" << endl;
			exit(1);
		}
		writes.insert(make_pair(time_start, BankUse(time_start, real_time, time_end,_id, ba, BankUse::WRITE, is_dynamic)));

	}
	//! Print info.
	void info() const
	{
		USES::const_iterator it;
		cout << "Bank " << id() << " reads " << endl;
		for (it = reads.begin(); it != reads.end(); it++)
		{
			const BankUse *bu = &((*it).second);
			bu->info();
		}
		cout << "Bank " << id() << " writes " << endl;
		for (it = writes.begin(); it != writes.end(); it++)
		{
			const BankUse *bu = &((*it).second);
			bu->info();
		}
	}
};

//! Sets of banks.
class Banks
{
public:
	//! Local type for sets of banks.
	typedef map<long, Bank, less<long> > BANKS;
	BANKS	banks;	//!< a set of banks.
	//! Add a bank to the set.
	//! @param Input. no is the bank number.
	void addBank(long no)
	{
		pair<BANKS::iterator, bool> p = banks.insert(make_pair(no, Bank(no)));
		if (!p.second)
		{
			cerr << "Error: bank(" << no << " defined twice" << endl;
			exit(1);
		}
	}
	//! Search for a bank.
	//! @param Input. no is the bank number.
	//! @param Output. b is a pointer to a bank.
	bool searchBank(long no, Bank **b)
	{
		BANKS::iterator it = banks.find(no);
		if (it == banks.end()) return false;
		*b = &((*it).second);
		return true;
	}
	//! Get number of banks.
	long size() const
	{
		return banks.size();
	}
	//! Print info.
	void info() const
	{
		for (BANKS::const_iterator it = banks.begin(); it != banks.end(); it++)
		{
			const Bank * b = & ((*it).second);
			b->info();
		}
	}
};

#endif // __BANK_H__

