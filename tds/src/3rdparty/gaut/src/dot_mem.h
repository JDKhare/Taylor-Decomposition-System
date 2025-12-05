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

class DotMem;

#ifndef __DOTMEM_H__
#define __DOTMEM_H__

#include <fstream>
#include "parser.h"	// time Units
#include "os_tools.h"	// cadency
#include "map.h"
#include "switches.h"
#include "lib.h"
#include "mem.h"
#include "cadency.h"
#include "bus.h"
#include "reg.h"
#include "cdfg.h"
#include "parser.h"
using namespace std;

//! To store informations about a node (IO or memory point).
class DotMemNode
{
public:
	//! Enumerated type to distinguish .mem nodes.
	enum Type
	{
		CONSTANT,				//!< Only read Data.
		VARIABLE,				//!< Written first Data.
		LOOPBACK,				//!< Read first Data = data to update at the end of the execution.
		LOOPBACK_WRITE,			//!< Name of the variable used to update the loopback variable.
		AGING,					//!< Loopback Data with aging mecanism = data to access with a base-indirect addressing scheme.
		INPUT,					//!< Input only Data.
		OUTPUT,					//!< Output only Data.
		DYNAMIC					//!< Dynamic access only Data. // EJ 14/02/2008 : dynamic memory
	};
private:
	Type			_type;		//!< Node type.
	const Data	   *_data;		//!< Pointer to Data.
	long		_reads;		//!< Number of reads.
	long		_writes;	//!< Number of writes.
	const BankUse  *_bankuse;	//!< Bank use.
public:
	//! Creation of a new .mem node.
	//! @param Input. data is a pointer to a constant Data.
	//! @param Input. type is the .mem nod etype.
	DotMemNode(const Data *data, Type type)
			: _data(data), _type(type)
	{
		_reads = _writes = 0;
		_bankuse = 0;
	}
	//! Get node name.
	const string name() const
	{
		return _data->name();
	}
	//! Get node type.
	Type type() const
	{
		return _type;
	}
	//! Get node pointer to Data.
	const Data * data() const
	{
		return _data;
	}
	//! Get number of reads.
	long reads() const
	{
		return _reads;
	}
	// Get number of writes.
	long writes() const
	{
		return _writes;
	}
	//! Add reads.
	void addReads(long n)
	{
		_reads += n;
	}
	//! Add writes.
	void addWrites(long n)
	{
		_writes += n;
	}
	//! Set bank use.
	void bankUse(const BankUse *bu)
	{
		_bankuse = bu;
	}
	//! Get bank address.
	string fullAddress() const
	{
		string bank, addr;
		if (_bankuse)
		{
			bank = Parser::itos(_bankuse->bank());
			addr = Parser::itos(_bankuse->address()->address());
			return bank + "/" + addr;
		}
		else return "-1/-1";
	}
};

//! Smart pointer to DotMemNode, based on the generic MapRef.
class DotMemNodeRef  : public MapRef <DotMemNode>
{
public:
	DotMemNodeRef(DotMemNode *d) : MapRef<DotMemNode>(d){}
};

//! List of smart pointers to DotMemNode, based on the generic MapRefs.
class DotMemNodeRefs : public MapRefs<DotMemNode, DotMemNodeRef> {};

//! GAUT's .mem file format.management.
class DotMem
{
private:
public:
	static bool _debug;		// to debug
	//! .mem file dump from GEUT kernel outputs.
	//! @param Input. sw is a pointer to the user switches.
	//! @param Input. lib is a pointer to the library.
	//! @param Input. cdfg is a pointer to the CDFG.
	//! @param Input. clk is a pointer to the system clock.
	//! @param Input. mem is a pointer to the memory.
	//! @param Input. cadency is a pointer to the cadency.
	//! @param Input. reg_out is a pointer to the registers.
	//! @param Input. bout is a pointer to the bus synthesis outputs.
	DotMem(
	    const Switches *sw,					// in
		string *mem_file_name,              //in 
	    const long nb_bits,					// in
	    const CDFG *cdfg,					// in
	    const Clock *clk,					// in
	    const Mem *mem,						// in
	    const Cadency *cadency,				// in
	    const RegOut *reg_out,				// in
	    const BusOut *bout,					// in
	    const BusOut *about					// in. // EJ 14/02/2008 : dynamic memory
	);
};

#endif // __DOTMEM_H__

