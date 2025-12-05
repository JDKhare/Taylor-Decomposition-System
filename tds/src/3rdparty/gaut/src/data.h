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

//	File:		data.h
//	Purpose:	DATA
//	Author:		Pierre Bomel, LESTER, UBS

class Data;
class DataRef;
class DataRefs;
class Operation;

#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <iostream>

using namespace std;
#include "map.h"
#include "value.h"
#include "cdfg.h"
#include "selection.h"
#include "check.h"
#include "mentor_type.h"

/* DH: 27/10/2008 */
class Reg;


//! Smart pointer to a CDFG Data node.
class DataRef  : public MapRef <Data>
{
public:
	DataRef(Data *d) : MapRef<Data>(d){}
};
//! List of smart pointers to CDFG Data nodes.
class DataRefs : public MapRefs<Data, DataRef> {};

//! CDFG Data nodes.
class Data     : public CDFGnode
{
public:
	//! Local enumerated type to distinguish Data.
	enum Type
	{
		INPUT,		//!< Input.
		OUTPUT,		//!< Output.
		VARIABLE,	//!< variable.
		CONSTANT	//!< Constant.

	};

	//! local type to distinguish ports (s)igned/(u)nsigned
	enum Signed
	{
		SIGNED,					//!< Signed port
		UNSIGNED,				//!< Unsigned port
		/* GL 05/10/07 : Fixed precision */
		SFIXED,					//!< Sfixed port
		UFIXED,				//!< Ufixed port
		/* Fin GL */
		STD_LOGIC_VECTOR		//!< std_logic_vector port (default)
	};


	// EJ 14/02/2008 : dynamic memory
	long _dynamic_address_bus; // temporaire
	//DH: 22/11/2008 duplication d'info long _dynamic_data_bus; // temporaire


private:
	const Type	_type;			//!< Data type.
	const Value *_value;		//!< For constants only 
	const Value *_resetvalue;	//!< For variable only 
	const Data *_writevar;		//!< For loopback variables only.
	const Data *_readvar;		//!< For loopback variables only.
	long _bitwidth;				//!< bitwidth.
	Signed _signed;				//!< Signed data
	int _index;
	long _regno;		//!< the reg containing the current dada


	/* GL 05/10/07 : fixed point */
	long _fixedPoint;
	string _quantization;
	string _overflow;
	/* Fin GL */

	// scheduling data
	long _time;					//!< Time constraints, otherwise -1.
	long _asap;					//!< Asap time.
	bool _asap_locked;			//!< Asap locked toggle.
	long _alap;					//!< Alap time.
	bool _alap_locked;			//!< Alap locked toggle.
	long _start;				//!< Start time after scheduling.
	long _start_modulo_cadency;				//!< Start time after scheduling modulo cadency.
	long _end_modulo_cadency;				//!< End time after scheduling modulo cadency.
	long _start_no_modulo_cadency;				//!< Start time after scheduling without modulo cadency
	long _end_no_modulo_cadency;				//!< End time after scheduling without modulo cadency

	/* Caaliph : 03/07/2007 */
	long _end;
	long _lifetime;
	long _lifetime_GAUT;
	double _duplication;
	/* End Caaliph */

	// for memory mapping, if any
	long _bank;					//!< Bank number.
	long _address;				//!< Address in bank number.
	bool _hardwire;				//!< For constant only, if true constant is hardwire in UT

	// for dynamic access, if any
	long _dynamic_address;					//!< Address in dynamic table : // EJ 14/02/2008 : dynamic memory
	const Data * _dynamic_access;			//!< For dynamic variables only : // EJ 14/02/2008 : dynamic memory
	vector<const Data*> *_dynamic_elements;	//!< dynamic access table of a dynamic variable : // EJ 14/02/2008 : dynamic memory
	// Caaliph 28/08/08
	long _busid;

	/* KT : 10/03/2008 */
	const CDFGnode *_operationpred  ; //!< the operation predecessing the data
	vector <Data *> _compatible_with;  //!< This datas are compatible with courent data
	/* GL 08/09/08 : Add lines */
	long _port;					//!< Bus port.
	/* End GL */
	/* DH : 26/10/2008 */
	bool _doGantt; //!< flag for gantt generation
	Reg *_reg; //! < dynamic link toward register allocated



public:

	bool _aging;				//!< For aging sample windows.


	//! Set the operation predecessing the data
	void operationpred(const CDFGnode *operationpred)
	{
		_operationpred = operationpred;
	}
	//! Set the datas not compatible with courent data
	void compatible_with(vector <Data *> compatible_with)
	{
		_compatible_with = compatible_with;
	}
	//! Get the operation predecessing the data
	const CDFGnode* operationpred() const
	{
		return _operationpred ;
	}
	//! Get the datas not compatible with courent data
	vector <Data *> compatible_with()
	{
		return _compatible_with;
	}
	/* End KT */

	/* GL 08/09/08 : Add lines */
	//! Get Bus port
	long port()
	{
		return _port;
	}
	//! Set Bus port
	void port(long port)
	{
		_port = port;
	}
	/* End GL */

	//! Create a new CDFG Data node.
	//! @param Input. name is the node name.
	//! @param Input. type is the node Data type.
	Data(const string &name, Type type) : CDFGnode(name, DATA), _type(type)
	{
		_time = -1;
		_asap = _alap = 0;
		_asap_locked = _alap_locked = false;
		_bank = _address = -1;	// no specification of memory mapping
		_value = 0;
		_resetvalue = 0;
		_writevar = _readvar = 0;
		_aging = false;
		_hardwire = false;
		_start = -1;
		_start_modulo_cadency = -1;
		_end_modulo_cadency = -1;
		_start_no_modulo_cadency = -1;
		_end_no_modulo_cadency = -1;
		_dynamic_access = 0;	//!< For dynamic access :  // EJ 14/02/2008 : dynamic memory
		_dynamic_address = -1;	//!< For dynamic element : // EJ 14/02/2008 : dynamic memory
		_operationpred  = 0;
		// Caaliph 28/08/08
		_busid = -1;
		_regno = -1;
		_end=-1;
		_lifetime=-1;
		_lifetime_GAUT=-1;
		_duplication=0.0;
		_doGantt=false;
		_reg=0; 

#ifdef CHECK
		data_create++;
#endif
	}
	//! Delete a CDFG Data node.
	~Data()
	{
		switch (_type)
		{
		case CONSTANT:
			if (_value) delete _value;
			break;
		case VARIABLE:
			if (_resetvalue) delete _resetvalue;
			break;
		}
#ifdef CHECK
		data_delete++;
#endif
	}
	//! Set bank address.
	//! @param Input. bank is the bank number.
	//! @param Input. address is the address in the bank.
	void setBank(long bank, long address)
	{
		_bank = bank, _address = address;
	}
	//! Set value.
	void value(long value)
	{
		_value = new Value(value);
	}
	//! Set value.
	void resetvalue(long resetvalue)
	{
		_resetvalue = new Value(resetvalue);
	}
	//! Define node as a "loopback node". It mean the data will be updated by the write of a variable.
	//! @param Input. d is a pointer to the write update variable.
	void loopback(Data *d)
	{
		_writevar = d;
		d->_readvar = this;
	}
	//! Define node as an "aging node". (not use)
	void aging(Data *d)
	{
		_writevar = d;
		d->_readvar = this;
		_aging = true;
	}
	//! Set bitwidth
	void bitwidth(long bitwidth)
	{
		_bitwidth = bitwidth;
	}
	//! Set signed
	void setSigned(Signed s)
	{
		_signed = s;
	}
	//! Set start time.
	void start(long start)
	{
		if (_time == -1)	_start = start;
		else				_start = _time;
	}

	//! Set start_modulo_cadency time.
	void set_start_modulo_cadency(long start_modulo_cadency)
	{
		_start_modulo_cadency = start_modulo_cadency;
		
	}
	
	//! Get start_modulo_cadency time.
	long get_start_modulo_cadency() const
	{ 
		return _start_modulo_cadency;
		
	}


	//! Set end_modulo_cadency time.
	void set_end_modulo_cadency(long end_modulo_cadency)
	{
		_end_modulo_cadency = end_modulo_cadency;
		
	}
	
	//! Get end_modulo_cadency time.
	long get_end_modulo_cadency() const
	{
		return _end_modulo_cadency;
		
	}


		//! Set start_no_modulo_cadency time.
	void set_start_no_modulo_cadency(long start_no_modulo_cadency)
	{
		_start_no_modulo_cadency = start_no_modulo_cadency;
		
	}
	
	//! Get start_no_modulo_cadency time.
	long get_start_no_modulo_cadency() const
	{ 
		return _start_no_modulo_cadency;
		
	}


	//! Set end_no_modulo_cadency time.
	void set_end_no_modulo_cadency(long end_no_modulo_cadency)
	{
		_end_no_modulo_cadency = end_no_modulo_cadency;
		
	}
	
	//! Get end_no_modulo_cadency time.
	long get_end_no_modulo_cadency() const
	{
		return _end_no_modulo_cadency;
		
	}

	
	//! Data Alive at cycle_time
	bool is_alive_at_cycletime(long cycle_time) const
	{
		if ((cycle_time >=_start_modulo_cadency) &&
			(cycle_time <	_end_modulo_cadency))
			return true;
		else 
			return false;
	}

	/* KT 05/05/2008 */
	//! get index
	int getindex()
	{
		return _index;
	}
	//! set index
	void index (int index)
	{
		_index = index;
	}
	//! get register
	long regno () const
	{
		return _regno;
	}
	//! set register
	void regno (long regno)
	{
		_regno = regno;
	}
	/*  End KT */
	/* Caaliph 03/07/2007 */
	//! Set End
	void end(long end)
	{
		_end=end;
	}
	//! Get End
	long end() const
	{
		return _end;
	}
	//! Set lifetime
	void lifetime(long lifetime)
	{
		_lifetime=lifetime;
	}
	//! Get lifetime
	long lifetime() const
	{
		return _lifetime;
	}
	//! Set lifetime
	void lifetime_GAUT(long lifetime_GAUT)
	{
		_lifetime_GAUT=lifetime_GAUT;
	}
	//! Get lifetime
	long lifetime_GAUT() const
	{
		return _lifetime_GAUT;
	}
	//! Set duplication
	void duplication(double duplication)
	{
		_duplication=duplication;
	}
	//! Get duplication
	double duplication() const
	{
		return _duplication;
	}
	//! Set doGantt
	void setDoGantt(bool doGantt)
	{
		_doGantt=doGantt;
	}
	//! Get duplication
	bool doGantt() const
	{
		return _doGantt;
	}
	//! Set Reg
	void setReg(Reg * reg)
	{
		_reg=reg;
	}
	//! Get Reg
	Reg *reg() const
	{
		return _reg;
	}




	// ! Set Bus ID : CA 28/08/08
	void setbusid(long busid)
	{
		_busid = busid;
	}
	// ! Get BUS ID
	long getbusid() const
	{
		return _busid;
	}

	//! Set asap.
	void asap(long asap)
	{
		if (_time != -1)
		{
			cerr << "Internal error: trying to set asap time of constrained IO data '" << name() << "'" << endl;
			exit(1);
		}
		_asap = asap;
	}
	//! Set alap.
	void alap(long alap)
	{
		if (_time != -1)
		{
			cerr << "Internal error: trying to set alap time of constrained IO data '" << name() << "'" << endl;
			exit(1);
		}
		_alap = alap;
	}
	//! Tells asap is locked.
	void asap_locked(bool b)
	{
		if (_time != -1)
		{
			cerr << "Internal error: trying to asap lock constrained IO data '" << name() << "'" << endl;
			exit(1);
		}
		_asap_locked = b;
	}
	//! Telle alap is locked.
	void alap_locked(bool b)
	{
		if (_time != -1)
		{
			cerr << "Internal error: trying to alap lock constrained IO data '" << name() << "'" << endl;
			exit(1);
		}
		_alap_locked = b;
	}
	void alap_force(long alap)
	{
		_alap = alap;
	}

	//! Set time constraint.
	void time(long time)
	{
		_time = time;
		/* DH : 7/09/2009 to be sure taht correst asp_alap is done durection selection/allocation */
		_alap = _asap = time;
		/* FIN DH : 7/09/2009 to be sure taht correst asp_alap is done durection selection/allocation */
		_asap_locked = _alap_locked = (time != -1);
	}
	//! Set cycles ... unused now, should be removed.
	void cycles(const SelectionOutRefs *mdos,long clk_period,long memory_access_time) {} // do nothing
	//! Get bank number.
	long bank() const
	{
		return _bank;
	}
	//! Get address.
	long address() const
	{
		return _address;
	}
	//! Get value.
	long value() const
	{
		if (_value) return _value->value();
		else
		{
			cerr << "Internal error: trying to get value of data '" << name() << "'" << endl;
			exit(1);
		}
	}

	long getOffset()
	{
		long result=0;
		if (_fixedPoint!=-999)
			result=_bitwidth-_fixedPoint;
		return result;
	}
	
	//! Get resetvalue.
	string resetvalue() const
	{
		if (_resetvalue)
		{

			if (_signed == SFIXED)
			{
				BIT_VECTOR bv=Mentor_type::signedLong_to_signedBitVector(_resetvalue->value(),_bitwidth);
				double realvalue=Mentor_type::bvtod(bv,_bitwidth,_fixedPoint,SIGNED_M);
				ostringstream myString;
				myString << realvalue;
				return myString.str();
			}
			else if (_signed == UFIXED)
			{
				BIT_VECTOR bv=Mentor_type::unsignedLong_to_unsignedBitVector(_resetvalue->value(),_bitwidth);
				double realvalue=Mentor_type::bvtod(bv,_bitwidth,_fixedPoint,UNSIGNED_M);
				ostringstream myString;
				myString << realvalue;
				return myString.str();
			}
			else //AC_INT
			{
				long longvalue=_resetvalue->value();
				ostringstream myString;
				myString << longvalue;
				return myString.str();
			}
		}
		else return "0";
	}

	//! Get debug value : transform internal bit_vector representaion value on int or double 
	string debug_value() const
	{
		if (_value)
		{

			if (_signed == SFIXED)
			{
				BIT_VECTOR bv=Mentor_type::signedLong_to_signedBitVector(_value->value(),_bitwidth);
				double realvalue=Mentor_type::bvtod(bv,_bitwidth,_fixedPoint,SIGNED_M);
				ostringstream myString;
				myString << realvalue;
				return myString.str();
			}
			else if (_signed == UFIXED)
			{
				BIT_VECTOR bv=Mentor_type::unsignedLong_to_unsignedBitVector(_value->value(),_bitwidth);
				double realvalue=Mentor_type::bvtod(bv,_bitwidth,_fixedPoint,UNSIGNED_M);
				ostringstream myString;
				myString << realvalue;
				return myString.str();
			}
			else //AC_INT
			{
				long longvalue=_value->value();
				ostringstream myString;
				myString << longvalue;
				return myString.str();
			}
		}
		else return "0";
	}


	//! Set hardwire constant. EJ 26/10/2007
	void hardwire(bool hardwire)
	{
		if (_type != CONSTANT)
		{
			cout << "warning : function hardwire can to use only for constant" << endl;
		}
		_hardwire = hardwire;
	}

	//! If this is hardwire constant. EJ 26/10/2007
	bool hardwire() const
	{
		if (_type != CONSTANT)
		{
			cout << "warning : function hardwire can to use only for constant" << endl;
		}
		return _hardwire;
	}


	//! Set new dynamic elements vector. // EJ 14/02/2008 : dynamic memory
	void new_dynamic_elements(long size)
	{
		_dynamic_elements=new vector<const Data *>;
		//_dynamic_elements->resize(size);
	}
	//! Set dynamic address. // EJ 14/02/2008 : dynamic memory
	void dynamic_address(long dynamic_address)
	{
		_dynamic_address = dynamic_address;
	}
	//! Get dynamic address. // EJ 14/02/2008 : dynamic memory
	long dynamic_address() const
	{
		return _dynamic_address;
	}
	//! Add dynamic element to dynamic access node. // EJ 14/02/2008 : dynamic memory
	void dynamic_elements(const Data *d)
	{
		this->_dynamic_access->_dynamic_elements->push_back(d);
	}
	//! Get a dynamic element. // EJ 14/02/2008 : dynamic memory
	const Data * dynamic_element(long _dynamic_address) const
	{
		return this->_dynamic_access->_dynamic_elements->at(_dynamic_address);
	}
	// Get all dynamic element. // EJ 14/02/2008 : dynamic memory
	vector<const Data *> * dynamic_elements() const
	{
		return this->_dynamic_access->_dynamic_elements;
	}

	void dynamic_access(const Data * d) 
	{
		_dynamic_access = d;
	}

	
	//! Get dynamic access || Is it a dynamic access node. // EJ 14/02/2008 : dynamic memory
	const Data * dynamic_access() const
	{
		return _dynamic_access;
	}
	//! Is a dynamic access ? // EJ 14/02/2008 : dynamic memory
	bool isADynamicAccess() const
	{
		if (_dynamic_access && _dynamic_address == -1) return true;
		return false;
	}

	//! Is a dynamic access ? // DH 06/11/2008 : dynamic memory
	bool isADynamicElement() const
	{
		if (_dynamic_address != -1) return true;
		return false;
	}



	//! If this is a loopback variable, get its write variable if any, or NULL.
	const Data * writevar() const
	{
		return _writevar;
	}
	//! If this is the write variable, get its read loopback variable if any, or NULL.
	const Data * readvar() const
	{
		return _readvar;
	}
	//! If this is the aging variable.
	bool aging() const
	{
		return _aging;
	}
	//! Get bitwidth
	long bitwidth() const
	{
		return _bitwidth;
	}
	//! Get signed
	Signed getSigned() const
	{
		return _signed;
	}
	//! Get string signed
	string getStringSigned() const
	{
		if (_signed == SIGNED) return "signed";
		else if (_signed == UNSIGNED) return "unsigned";
		/* GL 05/10/07 : Fixed precision */
		else if (_signed == SFIXED) return "sfixed";
		else if (_signed == UFIXED) return "ufixed";
		/* Fin GL */
		else return "std_logic_vector";
	}
	//! Get start time.
	long start() const
	{
		return _start;
	}
	//! Get asap time.
	long asap() const
	{
		if (_time == -1)	return _asap;
		else				return _time;
	}
	//! Get alap time.
	long alap() const
	{
		if (_time == -1)	return _alap;
		else				return _time;
	}
	//! Checks if asap is locked.
	bool asap_locked() const
	{
		return _asap_locked;
	}
	//! Checks if alap is locked.
	bool alap_locked() const
	{
		return _alap_locked;
	}
	//! get time constraint.
	long time() const
	{
		return _time;
	}
	//! Get cycles. Always = 0 because Data nodes do not span cycles ...
	long cycles() const
	{
		return 0;
	}
	//! get length in time units. Always = 0 because Data nodes do not span cycles ...
	long length() const
	{
		return 0;
	}
	//! Get type.
	Type type() const
	{
		return _type;
	}
	//! get node full name for loopback variables.
	string fullName() const
	{
		string n = name();
		if (readvar() && _aging == false) n += ("/" + readvar()->name());
		return n;
	}

	/* GL 05/10/07 : fixed point */
	long fixedPoint() const
	{
		return _fixedPoint;
	}
	const string quantization() const
	{
		return _quantization;
	}
	const string overflow()const
	{
		return _overflow;
	}
	void fixedPoint(long fixedPoint)
	{
		_fixedPoint = fixedPoint;
	}
	void quantization(string quantization)
	{
		_quantization = quantization;
	}
	void overflow(string overflow)
	{
		_overflow = overflow;
	}

	/* Fin GL */

	//! Print info.
	void info() const
	{
		cout << "  Data " << name();
		if ((_type == VARIABLE) && writevar()) cout << "/write=" << writevar()->name();
		if ((_type == VARIABLE) && readvar()) cout << "/read=" << readvar()->name();
		cout << "(type=";
		switch (_type)
		{
		case INPUT:
			cout << "input";
			break;
		case OUTPUT:
			cout << "output";
			break;
		case VARIABLE:
			cout << "variable";
			break;
		case CONSTANT:
			cout << "constant(" << _value->value() << ")";
			break;
		}
		/* GL 25/04/2007 :	print Bitwidth Info */
		cout << ",bitwidth=" << bitwidth();
		/* Fin GL */
		/* GL 08/10/2007 :	print fixed point Info */
		if (_fixedPoint!= -999) cout << ",fixed=" << "<" << fixedPoint() << "," << quantization() << "," << overflow() << ">";
		/* Fin GL */
		cout << ",asap=";
		if (asap_locked()) cout << "[";
		if (_time == -1)	cout << _asap;
		else				cout << _time;
		if (asap_locked()) cout << "]";

		cout << ",alap=";
		if (alap_locked()) cout << "[";
		if (_time == -1)	cout << _alap;
		else				cout << _time;
		if (alap_locked()) cout << "]";

		cout << ",length=" << length();
		cout << ",time=" << time();
		cout << ",start=" << start();

		cout << ")" << endl;
	}

	//! Serialize into a file.
	//! @param Input. f is an output file stream previously opened.
	void serialize(ofstream &f) const
	{
		switch (type())
		{
		case INPUT:
			f << "input";
			break;
		case OUTPUT:
			f << "output";
			break;
		case VARIABLE:
			f << "variable";
			break;
		case CONSTANT:
			f << "constant";
			break;
		}
		f << "(" << name() << ") {" << "\n";
		if (type() == CONSTANT) f << "  value " << value() << ";" << "\n";
		if (time() != -1) f << "  time " << time() << ";" << "\n";
		f << "  asap " << asap() << ";" << "\n";
		f << "  alap " << alap() << ";" << "\n";
		f << "  cycles " << cycles() << ";" << "\n";
		f << "  length " << length() << ";" << "\n";
		f << "  start " << start() << ";" << "\n";
		f << "}" << "\n";
	}

	/* Caaliph 04/07/2007 */
	//! Serialize into a file.
	//! @param Input. f is an output file stream previously opened.
	void serialize(ofstream &f, Cadency *cadency, const string &prefix_name, int *k, int *count_op) const
	{
		if (type()==INPUT||type()==VARIABLE || type()==CONSTANT)
		{
			if (duplication()>1)
			{
				for (int i=0; i<duplication(); i++)
				{
					if (type()==INPUT)
						if (i==0) f<<"input";
						else f<<"variable";
					else if (type()==CONSTANT)
						if (i==0) f<<"constant";
						else f<<"variable";
					else f<<"variable";
					//	string name = name()+"_"+Parser::itos(i);
					// CAA 22/01/09
					// If it is the first duplication, it is not necessary
					// to put a number
					if(i==0)
						f << "(" << name() <<") {" << "\n";
					else
					// Fin CAA
						f << "(" << name() <<"_"<<i<<") {" << "\n";
					if(i==0)
					{
						if(type()==VARIABLE)
							if(aging() == 1)
								f << "  aging " << writevar()->name() << ";" <<"\n";					
					}
					if (type()==CONSTANT) {
						if (i==0)
							f << "  value " << value() << ";" << "\n";
					}
					if (time() != -1) f << "  time " << time() << ";" << "\n";
					if (i==0)
					{
						f << "  asap " << asap() << ";" << "\n";
						f << "  alap " << alap() << ";" << "\n";
					}
					else
					{
						f << "  asap " << i*cadency->length() << ";" << "\n";
						f << "  alap " << i*cadency->length() << ";" << "\n";
					}
					f << "  cycles " << cycles() << ";" << "\n";
					f << "  length " << length() << ";" << "\n";
					if (i==0)
					{
						f << "  real_start " << start() << ";" << "\n";
						f << "  start " << start()%cadency->length() << ";" << "\n";
					}
					else
					{
						f << "  real_start " << i*cadency->length() << ";" << "\n";
						f << "  start " << 0 << ";" << "\n";
					}
					if (i==duplication()-1)
					{
						long fin;
						if ((end()>=cadency->length())&&(end()%cadency->length())==0) fin=cadency->length();
						else fin=end()%cadency->length();
						f << "  end " << fin << ";" << "\n";
					}
					else f << "  end " << cadency->length() << ";" << "\n";
					f<< "  org_lifetime "<<lifetime()<<";"<<"\n";
					// Caaliph 29/08/08
					if (i==0)
					{
						if ((type()==CONSTANT) || (type()==INPUT))
							f<< "  bus BUS_DONNEES_"<<getbusid()+1<<"_"<<prefix_name<<";"<< "\n";
						// CAA 10/02/09
						if(type()==VARIABLE)
							if(aging() == 1)
								f<< "  bus BUS_DONNEES_"<<getbusid()+1<<"_"<<prefix_name<<";"<< "\n";
						// Fin CAA
					}
					/* Caaliph 03/09/08 : add register attributes */
					if (i==0)
						f<< "  register r"<<regno()<<";"<<"\n";
					else
					{
						if (type()==CONSTANT)
							f<< "  register r"<<regno()<<"_cst"<<i<<";"<<"\n";
						else if (type()==INPUT)
							f<< "  register r"<<regno()<<"_in"<<i<<";"<<"\n";
						else if (type()==VARIABLE)
							f<< "  register r"<<regno()<<"_var"<<i<<";"<<"\n";
					}
					/* Fin Caaliph */
					f << "}" << "\n";
					if (i<duplication()-1)
					{
						f << "operation(opf" <<*k<<"_"<<i<< ") {" << "\n";
						f << "  function bypass" << ";" << "\n";
						// CAA 22/01/09
						if(i==0)
							f << "  read " <<name();
						else
						// Fin CAA
						f << "  read " <<name()<<"_"<<i;
						f << ";" << "\n";
						f << "  read_map a";
						f << ";" << "\n";
						f << "  write " <<name()<<"_"<<i+1;
						f << ";" << "\n";
						f << "  write_map o";
						f << ";" << "\n";
						f << "  asap " << (i+1)*cadency->length() << ";" << "\n";
						f << "  alap " << (i+1)*cadency->length() << ";" << "\n";
						f << "  cycles " << 0 << ";" << "\n";
						f << "  length " << 0 << ";" << "\n";
						f << "  real_start " << (i+1)*cadency->length() << ";" << "\n";
						f << "  start " << cadency->length() << ";" << "\n";
						f << "  identify "<<*count_op<<";"<<"\n";
						f << "  operator comp_"<<*count_op<<"_bypass;"<<"\n";
						f << "}" << "\n";
						(*count_op)++;
					}
				}
				(*k)++;
			}
			else
			{
				if (type()==INPUT) f<<"input";
				else if (type()==CONSTANT) f<<"constant";
				else f<<"variable";										// TODO reg
				f << "(" << name() << ") {" << "\n";
				if (type()==CONSTANT) f << "  value " << value() << ";" << "\n";
				if (time() != -1) f << "  time " << time() << ";" << "\n";
				if(type()==VARIABLE)
					if(aging() == 1)
						f << "  aging " << writevar()->name() << ";" <<"\n";
				f << "  asap " << asap() << ";" << "\n";
				f << "  alap " << alap() << ";" << "\n";
				f << "  cycles " << cycles() << ";" << "\n";
				f << "  length " << length() << ";" << "\n";
				f << "  real_start " << start() << ";" << "\n";
				f << "  start " << start()%cadency->length() << ";" << "\n";
				long fin;
				if ((end()>=cadency->length())&&(end()%cadency->length())==0) fin=cadency->length();
				else fin=end()%cadency->length();
				f << "  end " << fin << ";" << "\n";
				f<< "  org_lifetime "<<lifetime()<<";"<<"\n";
				// Caaliph 29/08/08
				if ((type()==CONSTANT) || (type()==INPUT))
					f<< "  bus BUS_DONNEES_"<<getbusid()+1<<"_"<<prefix_name<<";"<< "\n";
				// CAA 10/02/09
				if(type()==VARIABLE)
					if(aging())
						f<< "  bus BUS_DONNEES_"<<getbusid()+1<<"_"<<prefix_name<<";"<< endl;
				// Fin CAA
				/* Caaliph 04/09/08 : add register attributes */
				f<< "  register r"<<regno()<<";"<<"\n";
				/* Fin Caaliph */
				f << "}" << "\n";
			}
		}
		else
		{
			if (type()==OUTPUT) f<<"output";
			//if (type()==CONSTANT) f<<"constant";
			f << "(" << name() << ") {" << "\n";			// TODO reg
			//if (type() == CONSTANT) f << "  value " << value() << ";" << "\n";
			if (time() != -1) f << "  time " << time() << ";" << "\n";
			f << "  asap " << asap() << ";" << "\n";
			f << "  alap " << alap() << ";" << "\n";
			f << "  cycles " << cycles() << ";" << "\n";
			f << "  length " << length() << ";" << "\n";
			f << "  real_start " << start() << ";" << "\n";
			f << "  start " << start()%cadency->length() << ";" << "\n";
			// Caaliph 29/08/08
			//if((type()==OUTPUT) || (type()==INPUT))
			f<< "  bus BUS_DONNEES_"<<getbusid()+1<<"_"<<prefix_name<<";"<< "\n";
			/* Caaliph 04/09/08 : add register attributes */
			f<< "  register r"<<regno()<<";"<<"\n";
			/* Fin Caaliph */
			f << "}" << "\n";
		}
	}
	/* End Caaliph */

	//! Get string type for cpp generation
	string get_mentor_type() const
	{
		if (_signed == SIGNED) return "ac_int<" +Parser::ltos(_bitwidth)+",true>";
		else if (_signed == UNSIGNED) return "ac_int<"+Parser::ltos(_bitwidth)+",false>";
		else if (_signed == SFIXED) return "ac_fixed<"+Parser::ltos(_bitwidth)+","+Parser::ltos(_fixedPoint)+",true,"+_quantization+","+_overflow+">";
		else if (_signed == UFIXED) return "ac_fixed<"+Parser::ltos(_bitwidth)+","+Parser::ltos(_fixedPoint)+",false,"+_quantization+","+_overflow+">";
		else return "word";
	}


	//! Get string type for cpp generation
	string get_soclib_type() const
	{
		string quanti=_quantization;
		quanti=quanti.replace(0,3,"SC_");
		string over=_overflow;
		over=over.replace(0,3,"SC_");

		if (_signed == SIGNED) return "sc_int<" +Parser::ltos(_bitwidth)+">";
		else if (_signed == UNSIGNED) return "sc_uint<"+Parser::ltos(_bitwidth)+">";
		else if (_signed == SFIXED) return "sc_fixed<"+Parser::ltos(_bitwidth)+","+Parser::ltos(_fixedPoint)+","+quanti+","+over+">";
		else if (_signed == UFIXED) return "sc_ufixed<"+Parser::ltos(_bitwidth)+","+Parser::ltos(_fixedPoint)+","+quanti+","+over+">";
		else return "word_t";
	}

	
	//! Get string type for operation signature
	string get_signature_type() const
	{
		if (_signed == SIGNED) return "s" +Parser::ltos(_bitwidth);
		else if (_signed == UNSIGNED) return "u"+Parser::ltos(_bitwidth);
		else if (_signed == SFIXED) return "s"+Parser::ltos(_bitwidth)+"f"+Parser::ltos(_fixedPoint)+"q"+_quantization +"o"+_overflow;
		else if (_signed == UFIXED) return "u"+Parser::ltos(_bitwidth)+"f"+Parser::ltos(_fixedPoint)+"q"+_quantization+"o"+_overflow;
		else return "word";
	}

	//! Get string name for cpp generation
	string get_data_name() const
	{

		if (dynamic_address() != -1)
		{
			ostringstream myString;
			myString << dynamic_access()->name()<< "[" << dynamic_address() << "]";
			return myString.str();
		}
		else
		{
			if (Data::INPUT == type())
			{
				ostringstream myString;
				myString << name()<< "[it]";
				return myString.str();
			}
			else 
				return name();
		}
	}

	//! Get string name for cpp generation
	string get_data_name_soclib() const
	{

		if (dynamic_address() != -1)
		{
			ostringstream myString;
			myString << "m_" << dynamic_access()->name()<< "[" << dynamic_address() << "]";
			return myString.str();
		}
		else
		{
			ostringstream myString;
			myString << "m_" << name();
			return myString.str();
		}
	}


	//! Get string  for assignment bus ic
	//register std_logic_vector->bus std_logic_vector
/* 	| register			| bus		|	conversion register->bus	|	mode
	|					|			| 								|
	| slv				|	slv		|	to_slv(reg,bus'size)		| bitwidth_aware
	|					|			|								|
	| word				|	word	|	nothing to do				| !bitwidth_aware */

	string get_data_bus_assignment(const string &tabul,long bus_width,const string &bus_name,bool bitwidth_aware, const string &reg_name,long reg_width,const Switches *_sw) const
	{
		ostringstream myString;
		string prefix="";
		if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
			prefix="_out";
		if (bitwidth_aware == true)
		{
			/* if (!_sw->vhdl_type()!=Switches::UT_FSM_D) 
			{ */
				//register std_logic_vector -> register std_logic_vector
				if (bitwidth() < bus_width)
				{
					if (getSigned() == Data::UNSIGNED ||  getSigned() == Data::UFIXED)
						myString << tabul << bus_name << " <= (others => '0');" << "\n";
					else
						myString << tabul << bus_name << " <= (others => " << reg_name << prefix <<"(" << bitwidth()-1 <<"));" << "--sign extend\n";
					myString << tabul << bus_name <<"(" << bitwidth() -1 << " downto 0)" << " <= " << reg_name << prefix <<"(" << bitwidth() -1 << " downto 0)" << ";\n";
				}
				else if (bitwidth() > bus_width)
				{
						myString << tabul << "assert (" << bus_width << ">=" << bitwidth() <<") report \"WARNING : ouput bus width " <<  bus_width << " is lower than data width " << bitwidth() <<  "\" severity warning;\n" ;
						/* myString << tabul << "-- WARNING : ouput bus width is lower than data width" << "\n";*/
						myString << tabul << bus_name << " <= " << reg_name << prefix <<"(" <<  bus_width-1 << " downto 0);\n" ; 
				}
				else
				{
					myString <<  tabul << bus_name << " <= " << reg_name << prefix <<"(" << bitwidth() -1 << " downto 0)" << ";\n";
				}
			/* }
			else
			{
				//register signed/unsigned/ufixed/sfixed -> bus std_logic_vector
				if (bitwidth() < bus_width)
				{
					if (getSigned() == Data::UNSIGNED ||  getSigned() == Data::UFIXED)
						myString << tabul << bus_name << " <= (others => '0');" << "\n";
					else
						myString << tabul << bus_name << " <= (others => " << reg_name << prefix <<"(" << bitwidth()-1 <<"));" << "--sign extend\n";
					myString << tabul << bus_name <<"(" << bitwidth() -1 << " downto 0)" << " <= to_slv(" << reg_name << prefix <<"(" << bitwidth() -1 << " downto 0)," << bitwidth() << ");\n";
				}
				else if (bitwidth() > bus_width)
				{
						myString << tabul << "assert (" << bus_width << ">=" << bitwidth() <<") report \"WARNING : ouput bus width " <<  bus_width << " is lower than data width " << bitwidth() <<  "\" severity warning;\n" ;
						// myString << tabul << "-- WARNING : ouput bus width is lower than data width" << "\n";
						myString << tabul << bus_name << " <= to_slv(" << reg_name << prefix <<"(" <<  bus_width-1 << " downto 0)," << bus_width <<");\n" ; 
				}
				else
				{
					myString <<  tabul << bus_name << " <= to_slv(" << reg_name << prefix <<"(" << bitwidth() -1 << " downto 0)," << bitwidth() << ");\n";
				}
			}*/
		}
		else
				myString << tabul << bus_name << " <= " << reg_name << prefix <<";\n";
		return myString.str();
	}


	// DH : 22/09/2008
	//! Get string  for read bus ic
	//bus std_logic_vector -> register std_logic_vector(UT_FSM_SIGS,UT_FSM_R,UT_ROM) or signed/unsigned/ufixed/sfixed(UT_FSM_D)
	/*		| bus			| register			| conversion bus->register								| mode
			|				|					|														|
			|	slv			| slv				|	slv(data'bitwidth-1 downto 0)						| bitwidth_aware
			|				|					|														|
			|	word		| word				| nothing to do											| !bitwidth_aware
	*/
	string get_data_bus_read(const string &tabul,long bus_width,const string &bus_name,bool bitwidth_aware, const string &reg_name,long reg_width,const string &assign_op,const Switches *_sw) const
	{
		ostringstream myString;
		string prefix="";
	
		if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
				prefix="_in";
		if (bitwidth_aware == false)
		{
				//bus std_logic_vector(word) ->reg std_logic_vector(word)
				myString  << tabul << reg_name << prefix << " " << assign_op << " " << bus_name << ";" << "\n"; 
		}
		else
		{
			if (bitwidth() > bus_width)
			{
					myString << tabul << "assert (" << bus_width << ">=" << bitwidth() <<") report \"ERROR : input bus width " <<  bus_width << " is lower than data " << name()  << "(" << reg_name << ") width " << bitwidth() <<  "\" severity error;\n" ;
			}
			else
			{
				/* if (_sw->vhdl_type()!=Switches::UT_FSM_D) 
				{ */
					if (bitwidth() < reg_width)
					{
						if (getSigned() == Data::UNSIGNED ||  getSigned() == Data::UFIXED)
							myString << tabul << reg_name << prefix << " " << assign_op << " (others => '0');" << "\n";
						else
							myString << tabul << reg_name << prefix << " " << assign_op << " (others => " << bus_name << "(" <<bitwidth()-1 <<"));" << "\n";
					}
					myString << tabul << reg_name << prefix << "(" << bitwidth()-1 << " downto 0) " << assign_op << " " << bus_name << "(" << bitwidth()-1 << " downto 0);" << "\n";
				/* }
				else //bus std_logic_vector ->signed/unsigned/ufixed/sfixed
				{
					ostringstream convert_bus_to_register;
					if ( fixedPoint() != -999) // data is sfixed/ufixed
					{
						//convert slv to sfixed/ufixed
						convert_bus_to_register << "to_" << getStringSigned() << "( " << bus_name << "(" << bitwidth() - 1 <<" downto 0)"  <<"," << fixedPoint() -1 << " , -" << bitwidth() - fixedPoint() << ")";
					}
					else
					{
						//convert slv to signed/unsigned
						if (bitwidth()==reg_width)
							convert_bus_to_register << getStringSigned() << "(" << bus_name << ")";
						else
							convert_bus_to_register << getStringSigned() << "(" << bus_name << "(" << bitwidth()-1 << " downto 0))";
					}
					myString << tabul << reg_name << prefix << assign_op << convert_bus_to_register.str() << ";\n";
				} */
			}

		}
		return myString.str();
	}


	//! Get string  for read bus ic
	string get_data_bus_read_fifo(const string &tabul,long bus_width,const string &bus_name,bool bitwidth_aware,const string &reg_name,const long reg_width,const string &assign_op,const Switches *_sw) const

	{
		ostringstream myString;
		string prefix="";
	
		if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
				prefix="_in";
		if (bitwidth_aware == false)
		{
				//bus std_logic_vector(word) ->reg std_logic_vector(word)
				myString  << tabul << reg_name << prefix << "(0) " << assign_op << " " << bus_name << ";" << "\n"; 
		}
		else
		{
			if (bitwidth() > bus_width)
			{
					myString << tabul << "assert (" << bus_width << ">=" << bitwidth() <<") report \"ERROR : input bus width " <<  bus_width << " is lower than data " << name()  << "(" << reg_name << ") width " << bitwidth() <<  "\" severity error;\n" ;
			}
			else
			{
			/* 	if (_sw->vhdl_type()!=Switches::UT_FSM_D) 
				{ */
					if (bitwidth() < reg_width)
					{
						if (getSigned() == Data::UNSIGNED ||  getSigned() == Data::UFIXED)
							myString << tabul << reg_name << prefix << "(0) " << assign_op << " (others => '0');" << "\n";
						else
							myString << tabul << reg_name << prefix << "(0) " << assign_op << " (others => " << bus_name << "(" <<bitwidth()-1 <<"));" << "\n";
					}
					if ((_sw->vhdl_type()==Switches::UT_ROM) || (_sw->vhdl_type()==Switches::UT_FSM_R))
						myString << tabul << reg_name << prefix << "(" << bitwidth()-1 << " downto 0) " << assign_op << " " << bus_name << "(" << bitwidth()-1 << " downto 0);" << "\n";
					else
						myString << tabul << reg_name << prefix << "(0)(" << bitwidth()-1 << " downto 0) " << assign_op << " " << bus_name << "(" << bitwidth()-1 << " downto 0);" << "\n";
			/*	}
				else //bus std_logic_vector ->signed/unsigned/ufixed/sfixed
				{
					ostringstream convert_bus_to_register;
					if ( fixedPoint() != -999) // data is sfixed/ufixed
					{
						//convert slv to sfixed/ufixed
						convert_bus_to_register << "to_" << getStringSigned() << "( " << bus_name << "(" << bitwidth() - 1 <<" downto 0)"  <<"," << fixedPoint() -1 << " , -" << bitwidth() - fixedPoint() << ")";
					}
					else
					{
						//convert slv to signed/unsigned
						if (bitwidth()==reg_width)
							convert_bus_to_register << getStringSigned() << "(" << bus_name << ")";
						else
							convert_bus_to_register << getStringSigned() << "(" << bus_name << "(" << bitwidth()-1 << " downto 0))";
					}
					myString << tabul << reg_name << prefix << "(0)" << assign_op << convert_bus_to_register.str() << ";\n";
				} */
			}

		}
		return myString.str();
	}

	//! Get string  for data type vhdl
	string get_data_type_vhdl(bool bitwidth_aware) const
	{
		ostringstream myString;
		if (bitwidth_aware==true)
		{
			myString << getStringSigned() << "(";
			if (fixedPoint() == -999)
				myString << bitwidth()-1 << " downto 0)";
			else
				myString << fixedPoint()-1 << " downto -" <<  bitwidth() - fixedPoint() <<")";
		}
		else
		{
				myString << "word";
		}
		return myString.str();
	}




void constant_declaration(ofstream &f,const string &tabul, bool bitwidth_aware, long bits,const string &reg_name, long reg_bitwidth) const
{
	int i;
	

	if (bitwidth_aware)   // signed, unsigned
	{
		string function = (getSigned()==Data::SIGNED)?"to_signed":"to_unsigned";
		string paren = "";
		if (fixedPoint() != -999)
		{
			function = (getSigned()==Data::SIGNED)?"to_sfixed("+function:"to_ufixed("+function;
			paren =")";
		}
		f << tabul << "constant " << reg_name << ": " << get_data_type_vhdl(bitwidth_aware) << " := " << function << "(" << value()
			<< ", " << reg_bitwidth << ")" << paren << ";" << "\n";
	}
	else    // std_logic_vector
	{
		f << tabul << "constant " << reg_name << " : " << get_data_type_vhdl(bitwidth_aware) << " := std_logic_vector( to_signed(" << value()
			<< ", " << bits << "));" << "\n";
	}
}


//! Returns chaining information
long isChained() const
{
	return 0;
}

//! Get length when chaining is performed in time units.
long chainingLength() const
{
	return 0;
}

void compute_data_life_no_modulo_cadency(long period,long mem_access) 
{

	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
	const CDFGedge *e;		// an edge
	const CDFGnode *n_dst,*n_src;	// a destination node

	//debug info();
	//compute start and end time 
	long min_start_no_modulo_cadency=start();
	long max_end_no_modulo_cadency=0;

	//data lifetime = [ min_start_no_modulo_cadency : max_end_no_modulo_cadency]
	//calculate min_start_no_modulo_cadency: minimum of predecesessor (operation) end 
	for (e_it = predecessors.begin(); e_it != predecessors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_src = e->source ;	// get the target node
		if (n_src->type() != CDFGnode::OPERATION) continue;
		long oper_end=n_src->start()+n_src->length();
		if (oper_end  < min_start_no_modulo_cadency)
				min_start_no_modulo_cadency = oper_end;
	}
	//calculate max_end_no_modulo_cadency : maximum of successor (operation) end 
	if (type()==Data::VARIABLE || type()==Data::INPUT || type()==Data::CONSTANT)
	{

		if ((type()==Data::VARIABLE ) && !aging() && (readvar()))
		{
			//LOOPBACK
			max_end_no_modulo_cadency=(start()+period+mem_access);
		}
		else if ((type()==Data::INPUT ) && (successors.empty()) && (readvar()))
		{
			//input only used in delay
			max_end_no_modulo_cadency=(start()+period);
		}
		else
		{
			//VARIABLE, INPUT, CONSTANT
			for (e_it = successors.begin(); e_it != successors.end(); e_it++)
			{
				e = *e_it;			// get the edge
				n_dst = e->target ;	// get the target node
				if (n_dst->type() != CDFGnode::OPERATION) continue;
				long oper_end;
				if (n_dst->isChained()==-1)
					oper_end=n_dst->start()+n_dst->chainingLength();
				else
					oper_end=n_dst->start()+n_dst->length();
				if (oper_end > max_end_no_modulo_cadency)
					max_end_no_modulo_cadency = oper_end;
			}
		}
	}
	else
	{
		//OUTPUT
		max_end_no_modulo_cadency=(start()+period);
	}
	set_start_no_modulo_cadency(min_start_no_modulo_cadency);
	set_end_no_modulo_cadency(max_end_no_modulo_cadency);
}



void compute_data_life_modulo_cadency(long cadency, long period,long mem_access)
{

	Graph<CDFGnode,CDFGedge>::EDGES::const_iterator e_it;	// an edge iterator
	const CDFGedge *e;		// an edge
	const CDFGnode *n_dst,*n_src;	// a destination node

	//debug info();
	//compute start and end time modulo cadency for pipeline
	long min_start_no_modulo_cadency=start();
	long min_start_modulo_cadency=start() % cadency;
	long max_end_modulo_cadency=0;
	long max_end_no_modulo_cadency=0;


	//data lifetime = [ min_start_modulo_cadency : max_end_modulo_cadency]
	//calculate min_start_modulo_cadency: predecesessor operation end 
	for (e_it = predecessors.begin(); e_it != predecessors.end(); e_it++)
	{
		e = *e_it;			// get the edge
		n_src = e->source ;	// get the target node
		if (n_src->type() != CDFGnode::OPERATION) continue;
		long oper_end=n_src->start()+n_src->length();
		if (oper_end < min_start_no_modulo_cadency)
				min_start_no_modulo_cadency = oper_end;
	}
	min_start_modulo_cadency =  min_start_no_modulo_cadency % cadency;
	//calculate max_end_modulo_cadency : last used of variable = (maximum of successor operation end) % cadency
	if (type()==Data::VARIABLE || type()==Data::INPUT || type()==Data::CONSTANT)
	{

		if ((type()==Data::VARIABLE ) && !aging() && (readvar()))
		{
			//LOOPBACK
			max_end_modulo_cadency=(start()+period+mem_access) % cadency;
			long end_no_modulo_cadency=start()+period+mem_access;
			if (max_end_modulo_cadency==0)
			{
				if (end_no_modulo_cadency>=cadency)
				{
					max_end_modulo_cadency=cadency;
				}
			}
		}
		else if ((type()==Data::INPUT ) && (successors.empty()) && (readvar()))
		{
			//input only used in delay
			max_end_modulo_cadency=(start()+period) % cadency;
			long end_no_modulo_cadency=start()+period;
			if (max_end_modulo_cadency==0)
			{
				if (end_no_modulo_cadency>=cadency)
				{
					max_end_modulo_cadency=cadency;
				}
			}
		}
		else
		{
			//VARIABLE, INPUT, CONSTANT
			for (e_it = successors.begin(); e_it != successors.end(); e_it++)
			{
				e = *e_it;			// get the edge
				n_dst = e->target ;	// get the target node
				if (n_dst->type() != CDFGnode::OPERATION) continue;
				long oper_end=n_dst->start()+n_dst->length();
				if (n_dst->isChained()==-1)
					oper_end=n_dst->start()+n_dst->chainingLength();
				if (oper_end>max_end_no_modulo_cadency)
					max_end_no_modulo_cadency=oper_end;
			}
			max_end_modulo_cadency = max_end_no_modulo_cadency % cadency;//last used			
			/* if ((max_end_modulo_cadency ==0) && 
					(max_end_no_modulo_cadency>=cadency))
			 	max_end_modulo_cadency=cadency;  */
		}
	}
	else
	{
		//OUTPUT
		max_end_modulo_cadency=(start()+period) % cadency;
		long end_no_modulo_cadency=start()+period;
		if (max_end_modulo_cadency==0)
		{
			if (end_no_modulo_cadency>=cadency)
			{
			max_end_modulo_cadency=cadency;
			}
		}
	}
	//lifetime=cadency ?
	if (min_start_modulo_cadency>=max_end_modulo_cadency)
	{
		min_start_modulo_cadency=0;
		max_end_modulo_cadency=cadency;
	}
	set_start_modulo_cadency(min_start_modulo_cadency);
	set_end_modulo_cadency(max_end_modulo_cadency);
}


};

#endif // __DATA_H__
