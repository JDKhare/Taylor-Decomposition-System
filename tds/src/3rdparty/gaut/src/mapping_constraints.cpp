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


#include <iostream>
#include "data.h"
#include "mapping_constraints.h"
#include "scheduling.h"
using namespace std;


//DH: 30/11/2009
void MEMC::asap_shift(const CDFG *cdfg, const Mem *mem)
{
	// All inputs (sink) with no io constraints on the asap field
	// have an asap value of 0 with a regular ASAP().
	// They must be delayed until the first bank/bus access for memory accesst

	// scan all CDFG nodes
	if (_debug) cout << "MEMC::asap_shift" << endl;
	for (CDFG::EDGES::const_iterator e_it = cdfg->source()->successors.begin(); e_it != cdfg->source()->successors.end(); e_it++)
	{
		CDFGedge *e = *e_it;						// get current edge
		CDFGnode *n = e->target;					// get current node
		if (n->type() != CDFGnode::DATA) continue;	// process only DATA nodes
		if (n->asap_locked()) continue;				// process only DATA nodes without asap constraints
		Data *d = (Data*)n;				// get Data 
		Slots *slots;
		long size;
		if (searchDataRead(d, &slots))
		{
			
			d->asap_locked(true);
			long min_start=slots->times[0];
			size=1;
			while (size<slots->times.size())
			{
				if (min_start>slots->times[size])
					min_start=slots->times[size];
				size++;
			}
			d->start(min_start+mem->access_time());
		}
	}
}
//FIN DH: 30/11/2009

long MEMC::getLastReadTime(const Data *d)
{
	Slots *slots;
	long size;
	if (searchDataRead(d, &slots) && (size = slots->times.size())) return slots->times[size-1];
	cerr << "Error: unable to find last read of " << d->name() << endl;
	exit(1);
}

//DH: 10/11/2008
bool	MEMC::check_bank_free (const Data *dynamic_access_or_element, long modulo_time)
{
	long b;
	vector<long> bank_no_v;
	long bank_no;
	Bank *bank;
	const BankAddress *ba;
	const BankUse *bu;

	if (!dynamic_access_or_element->isADynamicAccess())
	{
		//dynamic_element
		bank_no_v.push_back(dynamic_access_or_element->bank());			// get bank number
	}
	else
	{
		//dynamic_access
		for (b=0; b<dynamic_access_or_element->dynamic_elements()->size(); b++)
		{
			bank_no_v.push_back(dynamic_access_or_element->dynamic_element(b)->bank());
		}
	}
	// recherche d'un acces sur les bancs concernés 
	for (b = 0; b < bank_no_v.size(); b++)
	{
		bank_no = bank_no_v[b];
		if ((_banks->searchBank(bank_no, &bank)) &&
		    ((bank->searchRead(modulo_time, &bu)) || (bank->searchWrite(modulo_time, &bu))))
			return false;
	}
	return true;
}

//DH: 10/11/2008: 
//read =  true
//lecture d'une entrée ou une donnée en mémoire
//dans le fichier .mem la donnée lue est présente sur le bus entre start et stop et est échantillonée par l'UT sur le stop avec stop <=limit.
//Le banc mémoire est utilisé entre start et stop. Cas ideal (banc libre) stop=limit et start=limit-mem_access
//read = false:
//ecriture d'une sortie ou d'une donnée en mémoire (sortie d'une opération limit = temps à partir duquel la sortie de l'operation
//peut être positionnée sur un bus de sortie pour l'UM ou l'UCOM)
//dans le fichier .mem la donnée lue est présente sur le bus entre start et stop et est échantillonée par l'UM/UCOM (ecriture mémoire synchrone)
//sur le stop avec start >= limit+clk_period et stop <=cadence. Par contre le banc mémoire est utilisé entre stop-mem_acess et stop !!!
//cas ideal (banc/bus libre) start=limit+clk_period, stop=limit+clk_period+mem_access (donnée presente sur le bus de sortie),
bool MEMC::allocateStaticSlot(const CDFGnode *n, long limit, bool read) {
	// compute real limit (modulo cadency)
	long cadency = _cadency->length();				// get cadency
	long mem_access_time = _mem->access_time();		// get memory access time
	long slots = cadency / mem_access_time;			// compute number of memory access slots
	if (_debug) cout << "slots=" << slots << endl;

	// get Data
	Data *d = (Data *) n;	// dynamic link
	if (read && (d->type() == Data::INPUT)) {
		if (_debug) cout << "this is an INPUT" << endl;
		return true;// this is an INPUT IO
	}
	//DH : 18/11/2008 */
	if (read && (d->type() == Data::CONSTANT) && d->hardwire()) {
		if (_debug) cout << "this is an hardwired constant" << endl;
		return true;// 
	}
	if (!read && (d->type() == Data::OUTPUT)) {
		if (_debug) cout << "this is an OUTPUT" << endl;
		return true;// this is an OUTPUT IO
	}
	if (_debug) cout << "data " << d->name() << endl;
	long bank_no = d->bank();			// get bank number
	long address = d->address();		// get address in bank

	// prepare banks with their addresses
	Bank *bank;
	if (!_banks->searchBank(bank_no, &bank)) {	// first access to bank, create bank
		_banks->addBank(bank_no);
		_banks->searchBank(bank_no, &bank);
	}
	const BankAddress *ba;
	if (!bank->searchAddress(address, &ba)) {	// first access to address in bank, create address
		bank->addAddress(address, d);
		bank->searchAddress(address, &ba);
	}

	// is there enough space to add one more slot ?
	if (bank_no != -1) {
		if (read) {
			if (bank->reads.size() == slots /* DH: add this if lock read at time 0*/ -1 ) {
				cerr << "Error: cadency " << cadency << " is too short to place all reads in bank " << bank_no << endl;
				exit(1);
			}
		} else {
			if (bank->writes.size() == slots) {
				cerr << "Error: cadency " << cadency << " is too short to place all writes in bank " << bank_no << endl;
				exit(1);
			}
		}
	}

	// READ case
	vector<bool> slots_visited(slots);
	for(int i = 0; i < slots; i++) slots_visited[i] = false;

	if (read) { 
		// read case, 
        //search backwards the first available slot or previous read if data is a constant (only read)
        long first_i = floor((double)limit / (double)mem_access_time);
        int i = first_i;
        while ( i>=0 /* DH/29/01/2009 (i > 0)  || ((i==0) && (bank_no == -1)) */ )
        {

            long full_time = i * mem_access_time;
            // modulo cadency values
            long modulo_time = full_time % cadency;
            long slot = i % slots;
            if (slots_visited[slot])
            {
                cerr << "Error: due to IO+mapping constraints: unable to find a read slot in bank " << bank_no << " for " << d->name() << endl;
                exit(1);
            }
            slots_visited[slot] = true;
            //feasability check
            bool next = false;
            if (d->asap_locked())
            {
                long modulo_asap = d->asap() % cadency;
                if (modulo_time+mem_access_time < modulo_asap) next = true;
            }
            if (next)
			{
				continue; // next
				i--;
			} 
            if (_debug) cout << "looking at slot " << slot << " time=" << full_time <<  " real_time=" << limit << endl;
            const BankUse *bu;
            if (bank_no != -1)
            {
                // EJ 07/12/2007 : check current planning before planning a new read
                if (check_current_planning(modulo_time, bank, ba, read)) 
				{
					i--;
					continue;
				}
                 // End EJ
                if (!bank->searchRead(modulo_time, &bu) && (!bank->searchWrite(modulo_time, &bu)))
                {
                    if (_debug) cout << "bank " << bank_no << " planning a read at " << modulo_time << " for " << d->name() << endl;
                    _saved_read_banks.push_back(bank);
                    _saved_read_times.push_back(modulo_time);
                    _saved_read_real_times.push_back(full_time);
                    _saved_read_addresses.push_back(ba);
                    return true;
                 } else if ((bu->address()->data() == d) && (bu->type()==BankUse::READ))
                 { // still OK, reuse the access
                    if (_debug) cout << "bank " << bank_no << " already a read at " << modulo_time << " for " << d->name() << endl;
	                return true;
                 } // we found a dynamic or write access on data = delay operation 
				 else //DH: 30/11/2009 for debig
				 {
	                if (_debug)
					{
						cout << "bank " << bank_no << " already used at " << modulo_time << endl;
						bu->info();
					}
				 }
             } 
             else
             {
                 if (_debug) cout << "bank " << bank_no << " planning a read at " << modulo_time << " for " << d->name() << endl;
                 _saved_read_banks.push_back(bank);
                 _saved_read_times.push_back(modulo_time);
                 _saved_read_real_times.push_back(full_time);
                 _saved_read_addresses.push_back(ba);
                 return true;
             }
             i--;
        }
	// WRITE case
	} else { // write case, search forward the first available slot
		// allocate slot: search from real_limit to to _cadency->length() modulo mem_access_time
		//la donnée est presente sur le bus de sortie à partir de limit+clk_period (!!!fichier .mem = start=limit et stop=limit+mem_access)
		//on doit empecher l'utilisation du banc à partir de limit meme si 
		//physiquement le banc memoire est utilisé de stop-mem_acess à stop
		//pour empecher une autre access sur le banc au même moment limit: ex de cas dévaroble
		//ecriture statique de Coeff[0] suivie d'une lecture statique de Coeff[0]
		long first_i = ceil((double)(limit) / (double)mem_access_time);
		for(int i = first_i;; i++) {
			// modulo cadency values
			long full_time = i * mem_access_time;
			long modulo_time = full_time % cadency;
			long slot = i % slots;
			if (slots_visited[slot]) {
				cerr << "Error: due to IO+mapping constraints: unable to find a write slot in bank " << bank_no << " for " << d->name() << endl;
				exit(1);
			}
			slots_visited[slot] = true;
			// feasability check
			bool next = false;
			if (d->alap_locked()) {
				long modulo_alap = d->alap() % cadency;
				if (modulo_time+mem_access_time > modulo_alap) next = true;
			}
			if (next) continue; // next
			if (_debug) cout << "looking at slot " << slot << " time=" << modulo_time <<  " real_time=" << limit << endl;
			const BankUse *bu;
			if (bank_no != -1) {
				// EJ 07/12/2007 : check current planning before planning a new write
				if (check_current_planning(modulo_time, bank, ba, read)) continue;
				// End EJ
				if (!bank->searchRead(modulo_time, &bu) && (!bank->searchWrite(modulo_time, &bu))) {
					if (_debug) cout << "bank " << bank_no << " planning a write at " << modulo_time << " for " << d->name() << endl;
					_saved_write_banks.push_back(bank);
					_saved_write_times.push_back(modulo_time);
					_saved_write_real_times.push_back(full_time);
					_saved_write_addresses.push_back(ba);
					return true;
				} else if ((bu->address()->data() == d) && (bu->type()==BankUse::WRITE)) { // stil OK, reuse the access
					if (_debug) cout << "bank " << bank_no << " already a write at " << modulo_time << " for " << d->name() << endl;
					return true;
				}// we found a dynamic or read access on data = delay operation
			} else {
				if (_debug) cout << "bank " << bank_no << " planning a write at " << modulo_time << " for " << d->name() << endl;
				_saved_write_banks.push_back(bank);
				_saved_write_times.push_back(modulo_time);
				_saved_write_real_times.push_back(full_time);
				_saved_write_addresses.push_back(ba);
				return true;
			}
		}
	}
	if (_debug) cout << "done" << endl;
	return false;
} 

bool MEMC::allocateDynamicAccessSlot(const CDFGnode *n, long limit, bool read)
{
	//limit = fin de l'operation mem_read/mem_write - mem_access avec
	//durée de l'operation mem_read = 1 clk_period + mem_access et mem_write = 1 clk_period + mem_access  
	//sur le clk_period est positionne l'adresse dynamque puis
	//sur le mem_access on fait l'accès à la mémoire en ecriture ou lecture
	// compute real limit (modulo cadency)
	long cadency = _cadency->length();				// get cadency
	long mem_access_time = _mem->access_time();		// get memory access time
	long slots = cadency / mem_access_time;			// compute number of memory access slots
	if (_debug) cout << "slots=" << slots << endl;
	long i;

	// get Data
	Data *d = (Data *) n;	// dynamic link
	if (_debug) cout << "Dynamic data access : " << d->name() << endl;

	// Recherche de tous les bancs qui doivent être disponible au même slot
	vector<long> bank_no_v;
	vector<long> address_v;
	vector<const Data *> data_v;
	if (d->dynamic_elements()->size() == 0)
	{
		cout << "Error :  dynamic access to empty structure : " << d->dynamic_access()->name() << endl;
		exit(1);
	}
	for (i=0; i<d->dynamic_elements()->size(); i++)
	{
		bank_no_v.push_back(d->dynamic_element(i)->bank());
		address_v.push_back(d->dynamic_element(i)->address());
		data_v.push_back(d->dynamic_element(i)); 
	}
	long bank_no, address;
	// prepare banks with their addresses
	Bank *bank;
	const BankAddress *ba;

	for (i=0; i<bank_no_v.size(); i++)
	{
		bank_no = bank_no_v[i];
		if (!_banks->searchBank(bank_no, &bank))  	// first access to bank, create bank
		{
			_banks->addBank(bank_no);
			_banks->searchBank(bank_no, &bank);
		}
		address = address_v[i];
		if (!bank->searchAddress(address, &ba))  	// first access to address in bank, create address
		{
			bank->addAddress(address, data_v[i]);
			bank->searchAddress(address, &ba);
		}
	}
	// is there enough space to add one more slot ?
	for (i = 0; i <bank_no_v.size(); i++)
	{
		bank_no = bank_no_v[i];
		if (bank_no != -1)
		{
			_banks->searchBank(bank_no, &bank);
			if (read)
			{
				if (bank->reads.size() == slots/* DH: add this if lock read at time 0*/ -1 ) /* -1 cannot read at time 0*/
				{
					cerr << "Error: cadency " << cadency << " is too short to place all reads in bank " << bank_no << endl;
					exit(1);
				}
			}
			else
			{
				if (bank->writes.size() == slots)
				{
					cerr << "Error: cadency " << cadency << " is too short to place all writes in bank " << bank_no << endl;
					exit(1);
				}
			}
		}
	}
	// READ case
	vector<bool> slots_visited(slots);
	for (i = 0; i < slots; i++) slots_visited[i] = false;
	if (read)   // dynamic read case,test if  available slot
	{
		//mem_read
		//l'addresse dynamique est presente sur le bus d'adresse de limit-clk_period à limit
		//la donnée fournie par la mémoire est présenté sur le bus de données de limit(debut de l'opération mem_read+ clk_period, => start fichier mem) à
		// limit+mem_access (=> stop fichier_mem).
		//le banc mémoire est utilisé de limit à limit+mem_acces
		i = floor((double)limit / (double)mem_access_time);
		// modulo cadency values
		long full_time = i * mem_access_time;
		long modulo_time = full_time % cadency;
		long slot = i % slots;
		if (slots_visited[slot])
		{
			cerr << "Error: due to IO+mapping constraints: unable to find a read slot in bank " << bank_no << " for " << d->name() << endl;
			exit(1);
		}
		slots_visited[slot] = true;
		// feasability check
		if (d->alap_locked())
		{
			long modulo_alap = d->alap() % cadency;
			if (modulo_time+mem_access_time > modulo_alap) 
				return false;
		}
		if (check_bank_free(d,modulo_time))
		{
			//les bancs memoires de l'acces dynamique sont libres 
			long max_bank_visited=0;
			for (i = 0; i < bank_no_v.size(); i++)
			{
				bank_no = bank_no_v[i];
				if (bank_no>max_bank_visited)
					max_bank_visited=bank_no;
			}
			vector<bool> banks_visited(max_bank_visited+1);
			for (i = 0; i <= max_bank_visited; i++) banks_visited[i] = false;
			for (i = 0; i < bank_no_v.size(); i++)
			{
				bank_no = bank_no_v[i];
				if (!banks_visited[bank_no])
				{
					banks_visited[bank_no]=true;
					if (_debug) cout << "bank " << bank_no << " planning a dynamic read at " << modulo_time << " for " << d->dynamic_access()->name() << endl;
					_banks->searchBank(bank_no, &bank);
					address = address_v[i];
					bank->searchAddress(address, &ba);
					_saved_read_banks.push_back(bank);
					_saved_read_times.push_back(modulo_time);
					_saved_read_real_times.push_back(full_time);
					_saved_read_addresses.push_back(ba);
					_saved_read_dynamic.push_back(d);
				}
			}
			banks_visited.clear();
			return true;
		}
	}
	else   // dynamic write case, search if available slot
	{
		//mem_write
		//l'addresse dynamique est presente sur le bus d'adresse de limit-clk_period à limit
		//la donnée fournie par la mémoire est présenté sur le bus de données de limit(fin de l'opération mem_write-mem_access, => start fichier mem) à
		// limit+mem_access (=> stop fichier_mem, stop opération mem_write).
		//le banc mémoire est utilisé de limit à limit+mem_acces
		i = ceil((double)(limit)  / (double)mem_access_time);
		// modulo cadency values
		long full_time = i * mem_access_time;
		long modulo_time = full_time % cadency;
		long slot = i % slots;
		if (slots_visited[slot])
		{
			cerr << "Error: due to IO+mapping constraints: unable to find a write slot in bank " << bank_no << " for " << d->name() << endl;
			exit(1);
		}
		slots_visited[slot] = true;
		// feasability check
		if (d->alap_locked())
		{
			long modulo_alap = d->alap() % cadency;
			if (modulo_time+mem_access_time > modulo_alap) return false;
		}
		if (check_bank_free(d,modulo_time))
		{
			//les bancs memoires de l'acces dynamique sont libres 
			long max_bank_visited=0;
			for (i = 0; i < bank_no_v.size(); i++)
			{
				bank_no = bank_no_v[i];
				if (bank_no>max_bank_visited)
					max_bank_visited=bank_no;
			}
			vector<bool> banks_visited(max_bank_visited+1);
			for (i = 0; i <= max_bank_visited; i++) banks_visited[i] = false;
			for (i = 0; i < bank_no_v.size(); i++)
			{
				bank_no = bank_no_v[i];
				if (!banks_visited[bank_no])
				{
					banks_visited[bank_no]=true;
					if (_debug) cout << "bank " << bank_no << " planning a dynamic write at " << modulo_time << " for " << d->dynamic_access()->name() << endl;
					_banks->searchBank(bank_no, &bank);
					address = address_v[i];
					bank->searchAddress(address, &ba);
					_saved_write_banks.push_back(bank);
					_saved_write_times.push_back(modulo_time);
					_saved_write_real_times.push_back(full_time);
					_saved_write_addresses.push_back(ba);
					_saved_write_dynamic.push_back(d);
				}
			}
			banks_visited.clear();
			return true;
		}
	}
	if (_debug) cout << "done" << endl;
	return false;
}

bool MEMC::allocateStaticReadSlot(const CDFGnode *n, long limit)
{
	return allocateStaticSlot(n, limit, true);
}
bool MEMC::allocateStaticWriteSlot(const CDFGnode *n, long limit)
{
	return allocateStaticSlot(n, limit, false);
}


bool MEMC::allocateDynamicReadSlot(const CDFGnode *n, long limit)
{
	return allocateDynamicAccessSlot(n, limit, true);
}
bool MEMC::allocateDynamicWriteSlot(const CDFGnode *n, long limit)
{
	return allocateDynamicAccessSlot(n, limit, false);
}

bool MEMC::checkReads(const CDFG *cdfg, const Operation *o, long time)
{
	// if a predecessor of "o" is an input variable (successor of CDFG's source)
	// check we can read it ALAP
	// scan all predecessors
	if (_debug) cout << endl << "checkReads: operation " << o->name() << " time=" << time << endl;
	_saved_read_banks.clear();
	_saved_read_times.clear();
	_saved_read_real_times.clear();
	_saved_read_addresses.clear();
	_saved_read_dynamic.clear();

	int i, j;

	for (i = 0; i < o->predecessors.size(); i++)
	{
		const CDFGedge *e = o->predecessors[i];		// get current predecessor edge
		const CDFGnode *n = e->source;	// get source node
		const Data *d = (const Data*) n;
		if (_debug) cout << "  predecessor " << n->name() << endl;
		//DH: 06/11/2008
		if (((o->function_name() == "mem_read" || o->function_name() == "mem_write")) &&
			  ((d->isADynamicAccess() || d->dynamic_address() != -1)))
		{
			//DH: 10/11/2008 don't allocate bank use for dynamic depedency of mem_read/mem_write operation
			//but verify that the bank is not freeze by a previous access
			if (!check_bank_free(d,time))
				return false;
			else continue; 
		}
		else if (!n->isASuccessorOf(cdfg->source()))
		{
			if (_debug) cout << "not a successor of source" << endl;
				continue;	// check only input variables and dynamic access
		}
		if (!allocateStaticReadSlot(n, time /* add this if lock read at time 0*/ -_mem->access_time()))//la donnée doit etre utilisable par l'UT a time
		{
			return false;	// unable to read
		}
	}
	return true;
}

bool MEMC::checkWrites(const CDFG *cdfg, const Operation *o, long time)
{
	// if a successor of "o" is an output variable (predecessor of CDFG's sink)
	// check we can write it ASAP
	if (_debug) cout << endl << "checkWrites: operation " << o->name() << " time=" << time << endl;
	_saved_write_banks.clear();
	_saved_write_times.clear();
	_saved_write_real_times.clear();
	_saved_write_addresses.clear();
	_saved_write_dynamic.clear();

	int i, j;

	/*  EJ 14/02/2008 : dynamic memory */
	/* DH 06/11/2008 !!! : la sortie de l'operation mem_read est une lecture en memoire */
	if (o->function_name() == "mem_read")
	{
		const CDFGedge *e = o->successors[0];
		const CDFGnode *n = e->target;
		if (n->type() != CDFGnode::DATA)
		{
			cout << "fatal error : successor of mem_read operation isn't a data" << endl;
			exit(1);
		}
		const Data *d = (const Data*) n;
		if (!d->isADynamicAccess() && d->dynamic_address() == -1)
		{
			cout << "fatal error : successor of mem_read operation isn't a dynamic access or a dynamic element" << endl;
			exit(1);
		}

		_saved_read_banks.clear();
		_saved_read_times.clear();
		_saved_read_real_times.clear();
		_saved_read_addresses.clear();
		_saved_read_dynamic.clear();
		if (!allocateDynamicReadSlot(n, time/* add this if lock read at time 0*/ -_mem->access_time())) //la donnée doit etre utilisable par l'UT a time
		{
			return false;	// unable to read memory
		}
		return true;
	}
	else if (o->function_name() == "mem_write")
	{
		const CDFGedge *e = o->successors[0];
		const CDFGnode *n = e->target;
		if (n->type() != CDFGnode::DATA)
		{
			cout << "fatal error : successor of mem_read operation isn't a data" << endl;
			exit(1);
		}
		const Data *d = (const Data*) n;
		if (!d->isADynamicAccess() && d->dynamic_address() == -1)
		{
			cout << "fatal error : successor of mem_read operation isn't a dynamic access or a dynamic element" << endl;
			exit(1);
		}

		if (!allocateDynamicWriteSlot(n, time/* DH: 15/12/2008*/-_mem->access_time()/* FIN DH: 15/12/2008*/)) //la donnée doit etre utilisable par l'UT a time: donc ecrite en memoire de time-mem_access-clock_period à time
		{
			return false;	// unable to write 
		}


		return true;
	}
	// End EJ
	for (i = 0; i < o->successors.size(); i++)
	{
		const CDFGedge *e = o->successors[i];		// get current successors edge
		const CDFGnode *n = e->target;	// get target node
		if (_debug) cout << "  successor " << n->name() << endl;
		if (!n->isAPredecessorOf(cdfg->sink()))
		{
			if (_debug) cout << "not a predecessor of sink" << endl;
			continue;	// check only output variables
		}
		if (!allocateStaticWriteSlot(n, time+_cdfg->_clk))//la donnée peut_etre fournie par l'UT  dès time mais sera présent sur un bus de donnée au front suivant
		{
			return false;	// unable to write to memory
		}
	}
	return true;

}


// freeze (lock) previous decision about MEM accesses and reserved time slots

void MEMC::freeze()
{
	if (_debug) cout << "Freeze memory accesses" << endl;
	Bank *b;
	long time, real_time;
	const BankAddress *ba;
	Data *d;
	int i;
	bool is_dynamic_access= _saved_read_dynamic.size()!=0;
	for (i = 0; i < _saved_read_banks.size(); i++)
	{
		b = _saved_read_banks[i];
		time = _saved_read_times[i];
		real_time = _saved_read_real_times[i];
		ba = _saved_read_addresses[i];
		//DH 31/11/2009: modif to add only new bank access
		//add the time to the time map
		bool alreadyread=true;
		Slots *slots;
		d = (Data *)ba->data();
		if (!searchDataRead(d, &slots))
		{
			addDataRead(d);
			alreadyread=false;
			searchDataRead(d, &slots);
		}
		slots->times.push_back(time);
		if (alreadyread==false)
		{
			if (is_dynamic_access)
			{
				if (_debug) cout << "bank " << b->id() << " at " << time << " dynamic read  " << _saved_read_dynamic[i]->name() << endl;
				b->addRead(time, real_time, time+_mem->access_time(), ba, _saved_read_dynamic[i]);
			}
			else
			{
				if (_debug) cout << "bank " << b->id() << " at " << time << " static read  " << ba->data()->name() << endl;
				b->addRead(time, real_time, time+_mem->access_time(),ba, NULL);
			}
		}
	}
	is_dynamic_access= _saved_write_dynamic.size()!=0;
	for (i = 0; i < _saved_write_banks.size(); i++)
	{
		b = _saved_write_banks[i];
		time = _saved_write_times[i];
		real_time = _saved_write_real_times[i];
		ba = _saved_write_addresses[i];
		if (is_dynamic_access)
		{
			if (_debug) cout << "bank " << b->id() << " at " << time << " dynamic write " << _saved_write_dynamic[i]->name() << endl;
			b->addWrite(time, real_time, time+_mem->access_time(),ba, _saved_write_dynamic[i]);
		}
		else
		{
			//ecriture statique : il faut présenter la donnée sur le bus pendant un cycle avant de commencer l'écriture en mémoire
			if (_debug) cout << "bank " << b->id() << " at " << time << " write " << ba->data()->name() << endl;
			b->addWrite(time, real_time, time+_mem->access_time() ,ba, NULL);
		}
	}
}
 

// Check current planning before planning a new read/write (EJ 07/12/2007)
bool MEMC::check_current_planning(long modulo_time, const Bank *bank, const BankAddress *ba, bool read)
{
	int a;
	int bank_no = bank->id();
	if (read)
	{
		for (a = 0; a < _saved_read_times.size(); a++)
		{
			if (bank_no == _saved_read_banks[a]->id() && modulo_time == _saved_read_times[a])
			{
				if (_saved_read_addresses[a]->address() == ba->address())
				{
					if (_debug) cout << "bank " << bank_no << " already a read at " << modulo_time << " for " << ba->data()->name() << endl;
				}
				return true;
			}
		}
	}
	else
	{
		for (a = 0; a < _saved_write_times.size(); a++)
		{
			if (bank_no == _saved_write_banks[a]->id() && modulo_time == _saved_write_times[a])
			{
				if (_saved_write_addresses[a]->address() == ba->address())
				{
					if (_debug) cout << "bank " << bank_no << " already a write at " << modulo_time << " for " << ba->data()->name() << endl;
				}
				return true;
			}
		}

	}
	return false; // check previous planning
}

// end of: mapping_constraints.cpp
