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

//	File:		ic.h
//	Purpose:	IC creation of VHDL RTL synthesizable model
//	Author:		Pierre Bomel, LESTER, UBS

class IC;

#ifndef __IC_H__
#define __IC_H__

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
#include "parser.h"
#include "switches.h"
#include "cdfg.h"
#include "reg.h"
#include "clock.h"
#include "operation.h"
#include "bus.h"

//! IC clock cycle. To sort operations among cycles.
class IC_Cycle
{
public:
	vector<const Operation*>	start;			//!< The list of starting operations
	vector<const Operation*>	end;			//!< The list of ending operations at the previous cycle
	// Ghizlane 22/10/08 */
	vector<const Operation*>	run;			//!< The list of runing operations
	// Fin GL */
};
//! IC clock cycles is a vector of IC_Cycle. To sort operations among cycles.
class IC_Cycles : public vector<IC_Cycle> {};

//! To create VHDL RTL synthesizable models.
class IC
{
private:
	//! Generate the VHDL.
	//! @param Input. f is an output stream.
	void  gen_vhdl(ofstream &f);
	const Switches *_sw;						//!< local copy of sw
	const Mem *_mem;							//!< local copy of mem
	const Cadency *_cadency;					//!< local copy of cadency
	const CDFG *_cdfg;							//!< local copy of cdfg
	const Clock *_clk;							//!< local copy of clk
	const BusOut *_buses;						//!< local copy of buses
	const BusOut *_abuses;						//!< local copy of address bus. // EJ 14/02/2008 : dynamic memory
	const RegOut *_regs;						//!< local copy of regs
	const SchedulingOut *_sched;				//!< local copy of sched
	long _bits;									//!< local copy of bits
	/* Caaliph 27/06/2007 */
	string _vhdl_name;
	/* End Caaliph */
	bool _debug_vhdl; // DH: 27/11/2008 add trace generation on vhdl for debug

public:
	string instance_name(const OperatorInstance *inst) const ;
	string signal_name(const OperatorInstance *inst, const Port *port) const ;
	string bus_name(const BusAccess *ba) const ;
	string bus_name(long id) const ;
	string address_bus_name(const BusAccess *ba) const ;
	string address_bus_name(long id) const ; 
	string port_name(const Port *port) const ;
	void	debug_vhdl(ofstream &f,const string &tabul,const Data *dest_data) const;
	void map_register_to_component_port(ofstream &f,const string &tabul,const string &signal_name,Reg *reg, const Data * data,const Port *port,const Operation*oper,const long current_cycle) const;
	void map_component_port_to_register(ofstream &f,const string &tabul,const string &signal_name,Reg *reg, const Data * data,const Port *port,const Operation *oper) const;
	void assign_register_to_register(ofstream &f,const string &tabul,const Data *dest_data, const Operation *oper,const long cycle_op) const;
	void sliceread_register_to_register(ofstream &f,const string &tabul,const Data *dest_data, const Operation *oper) const;
	void gen_input_reg_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const;
	void gen_cmd_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const;
	void gen_enable_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const;
	void map_component_port_to_component_port(ofstream &f,const string &tabul, const Port *port_out, const string &signal_port_out,
											  const Data * data,const Port *port_in,const string &signal_port_in) const;
	void gen_input_from_buses(ofstream &f, long cycle_read);
	void gen_static_memory_read(ofstream &f, long cycle_read);
	void gen_dynamic_memory_read(ofstream &f, long cycle_read);
	void gen_output_on_buses(ofstream &f, long cycle_write);
	void gen_static_write_to_memory(ofstream &f, long cycle_write);
	void gen_dynamic_address(ofstream &f, long cycle_adress);
	void gen_dynamic_memory_write(ofstream &f, long cycle_write);
	void gen_output_op(ofstream &f, long cycle_output_op,long cycles);
	void gen_reg_tri_vhdl(ofstream &f, bool reg_fifo);
	void gen_rom_fsm(long cycles,long period,bool one_hot_coding);
	void trace_input_vhdl(ofstream &f,const string &tabul,const OperatorInstance *inst,const Data *data,long current_cycle) const;
	void trace_input_reg_operator(ofstream &f,const string &tabul,const Operation *oper,const long current_cycle) const;
	void trace_output_vhdl(ofstream &f,const string &tabul,const OperatorInstance *inst,const Data *data,long current_cycle,string trace_name) const;


	long get_bits() const
	{
		return _bits;
	}

	/* GL 27/04/2007 :	compute reg bitwidth */
	void reg_width(const Reg *r) ;
	/* Fin GL */
	/* GL 08/10/07 : add lines
	string fixedAdapt (Reg *r, long width, const Operation *o) const;
	/* Fin GL */
	/* GL 20/11/07 */
	string signal_name(const OperatorInstance *inst, const string port_name) const ;
	/* Fin GL */

	//! Create a new VHDL generation process.
	IC(
	    const Switches *sw,						//!< @param Input. sw is a pointer to the user switches.
	    const Mem *mem,							//!< @param Input. mem is a pointer to the memory model.
	    const Cadency *cadency,					//!< @param Input. cadency is a pointer to the cadency constraint.
	    const CDFG *cdfg,						//!< @param Input. cdfg is a pointer to the CDFG.
	    const Clock *clk,				 		//!< @param Input. clk is a pointer to the system clock.
	    const BusOut *buses,					//!< @param Input. buses is a pointer to the result of the bus synthesis.
	    const BusOut *abuses,					//!< @param Input. address buses is a pointer to the result of the address bus synthesis. // EJ 14/02/2008 : dynamic memory
	    const RegOut *regs,						//!< @param Input. regs is a pointer to the result of the regs synthesis.
	    const SchedulingOut *sched,				//!< @param Input. sched is a pointer to the result of the scheduling.
	    long bits								//!< @param Input. bits
	)
	{
		// copy parameters
		_sw = sw;
		_mem = mem;
		_cadency = cadency;
		_cdfg = cdfg;
		_clk = clk;
		_buses = buses;
		_abuses = abuses; // EJ 14/02/2008 : dynamic memory
		_regs = regs;
		_sched = sched;
		_bits = bits;
		_debug_vhdl = false;
		// check for overwrite
		if (!sw->force_overwrite()) 
			Parser::check_file_nexists(sw->vhdl_file_name());
		// open output VHDL file
		string output_vhdl_file = sw->vhdl_file_name();
		ofstream f(output_vhdl_file.c_str(), ios::out);
		//DH : 22/20/2008 : try with a stream buffer
		char *mybuffer = (char *)calloc(131072,sizeof(char));;
		f.rdbuf()->pubsetbuf(mybuffer,131072);
		// generate VHDL
		gen_vhdl(f);
		// output VHDL file  close
		f.close();
		free(mybuffer);
	}

	/* Caaliph 27/06/2007 */
	//! Create a new VHDL generation process.
	IC(
	    const Switches *sw,						//!< @param Input. sw is a pointer to the user switches.
	    string vhdl_name,
	    const Mem *mem,							//!< @param Input. mem is a pointer to the memory model.
	    const Cadency *cadency,					//!< @param Input. cadency is a pointer to the cadency constraint.
	    const CDFG *cdfg,						//!< @param Input. cdfg is a pointer to the CDFG.
	    const Clock *clk,				 		//!< @param Input. clk is a pointer to the system clock.
	    const BusOut *buses,					//!< @param Input. buses is a pointer to the result of the bus synthesis.
	    const BusOut *abuses,					//!< @param Input. address buses is a pointer to the result of the address bus synthesis. // EJ 14/02/2008 : dynamic memory
	    const RegOut *regs,						//!< @param Input. regs is a pointer to the result of the regs synthesis.
	    const SchedulingOut *sched,				//!< @param Input. sched is a pointer to the result of the scheduling.
	    long bits								//!< @param Input. bits
	)
	{
		// copy parameters
		_sw = sw;
		_mem = mem;
		_cadency = cadency;
		_cdfg = cdfg;
		_clk = clk;
		_buses = buses;
		_abuses = abuses; // EJ 14/02/2008 : dynamic memory
		_regs = regs;
		_sched = sched;
		_bits = bits;
		_vhdl_name = vhdl_name;
		_debug_vhdl = false;
		// check for overwrite
		string vhd_name = _vhdl_name+".vhd";
		if (!sw->force_overwrite()) Parser::check_file_nexists(vhd_name);
		// open output VHDL file
		ofstream f(vhd_name.c_str(), ios::out);
		// generate VHDL
		gen_vhdl(f);
		// output VHDL file  close
		f.close();
	}
	/* End Caaliph */
};

#endif // __IC_H__
