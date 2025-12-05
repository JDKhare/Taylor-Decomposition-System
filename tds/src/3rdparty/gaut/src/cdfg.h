/*------------------------------------------------------------------------------
  Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

  Cette licence est un accord légal conclu entre vous et l'Université de
  Bretagne Sud(UBS)concernant l'outil GAUT2.

  Cette licence est une licence CeCILL-B dont deux exemplaires(en langue
  française et anglaise)sont joints aux codes sources. Plus d'informations
  concernant cette licence sont accessibles à http://www.cecill.info.

  AUCUNE GARANTIE n'est associée à cette license !

  L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
  de GAUT2 et le distribue en l'état à des fins de coopération scientifique
  seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
  de protection de leurs données qu'il jugeront nécessaires.
  ------------------------------------------------------------------------------
  2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

  This license is a legal agreement between you and the University of
  South Britany(UBS)regarding the GAUT2 software tool.

  This license is a CeCILL-B license. Two exemplaries(one in French and one in
  English)are provided with the source codes. More informations about this
  license are available at http://www.cecill.info.

  NO WARRANTY is provided with this license.

  The University of South Britany does not provide any warranty about
  the usage of GAUT2 and distributes it "as is" for scientific cooperation
  purpose only. All users are greatly advised to provide all the necessary
  protection issues to protect their data.
  ------------------------------------------------------------------------------
  */

//	File:		cdfg.h
//	Purpose:	control + data flow graph
//				First version handles only DFG, CDFG is for the future
//	Author:		Pierre Bomel, LESTER, UBS

class CDFGnode;
class CDFGnodeRef;
class CDFGnodeRefs;
class CDFGedge;
class CDFGedgeRef;
class CDFGedgeRefs;
class CDFG;

#ifndef __CDFG_H__
#define __CDFG_H__

#include <cstdlib>
#include <iostream>
using namespace std;
#include "map.h"
#include "check.h"
#include "cadency.h"

//! A smart pointer to a CDFG node.
class CDFGnodeRef  : public MapRef <CDFGnode>
{
public:
	CDFGnodeRef(CDFGnode*d): MapRef<CDFGnode>(d) {}
};
//! A list of smart pointers to CDFG nodes.
class CDFGnodeRefs : public MapRefs<CDFGnode, CDFGnodeRef> {};

#include "graph.h"
#include "parser.h"
#include "port.h"
#include "selection.h"
/* GL 25/10/07 : pattern(add lines)*/
#include "pattern.h"
/* Fin GL*/

//! CDFG nodes.
class CDFGnode : public Node<CDFGedge>, public MapID  	// specific graph elements
{
public:
	//! Enumerated type to distinguish nodes.
	enum Type {CONTROL, OPERATION, DATA};
private:
	const Type	_type;		//!< Node type.
public:
	void*_addr;			//!< Generic pointer to extend easily.
	void*_addr2;			//!< Generic pointer to extend easily.
	// EJ 14/02/2008 : dynamic memory
	void*_addr3;			//!< Generic pointer to extend easily
	// End EJ
	long _maxCriticalPredecessors; //!<for critical path calculation>
	//! Build a new CDFG node.
	//! @param Input. name is the unique name of the node.
	//! @param Input. type is the nod etype.
	CDFGnode(const string & name, Type type): MapID(name), _type(type)
	{
		_addr = 0;
		_addr2 = 0;
		_addr3 = 0;
		_maxCriticalPredecessors = -1;
#ifdef CHECK
		cdfgnode_create++;
#endif
	}
	~CDFGnode()
	{
#ifdef CHECK
		cdfgnode_delete++;
#endif
	}
	//! Get type.
	Type type()const
	{
		return _type;
	}

	//! Get critical path of predecessor.
	long getMaxCriticalPredecessors()const
	{
		return _maxCriticalPredecessors;
	}

	//! Set critical path of predecessor.
	void setMaxCriticalPredecessors(long maxCriticalPredecessors)
	{
		_maxCriticalPredecessors = maxCriticalPredecessors;
	}

	//! Get start time. All subclasses must define it.
	virtual void start(long start) = 0;
	//! Get asap time. All subclasses must define it.
	virtual void asap(long asap) = 0;
	//! Get alap time. All subclasses must define it.
	virtual void alap(long alap) = 0;
	//! Get lock status of asap time. All subclasses must define it.
	virtual void asap_locked(bool b) = 0;
	//! Get lock status of alap time. All subclasses must define it.
	virtual void alap_locked(bool b) = 0;
	//! Set start time. All subclasses must define it.
	virtual long start()const = 0;
	//! Set asap time. All subclasses must define it.
	virtual long asap()const = 0;
	//! Set lock status of asap time. All subclasses must define it.
	virtual bool asap_locked()const = 0;
	//! Set alap time. All subclasses must define it.
	virtual long alap()const = 0;
	//! Set lock status of alap time. All subclasses must define it.
	virtual bool alap_locked()const = 0;
	//! Get number of cycles. All subclasses must define it.
	virtual long cycles()const = 0;
	//! Get length in time units. All subclasses must define it.
	virtual long length()const = 0;
	//! Get length when chaining is performed in time units.
	virtual long chainingLength()const = 0;
	//! Returns chaining information
	virtual long isChained()const =0;

	//! Set cycles. All subclasses must define it.
	virtual void cycles(const SelectionOutRefs*mdos,long clk_period,long memory_access_time) = 0;
	//! Serialize. All subclasses must define it.
	virtual void serialize(ofstream &f)const = 0;

	//! Special.
	bool backTo(const CDFGnode*n, long &measure)const;
	//! Special.
	void forceAlapBackward(const CDFGnode*n, long limit);
	//! propagateLock alap
	void propagateAlapLock(long alap)const;
};

//! CDFG edges.
class CDFGedge : public Edge<CDFGnode>
{
private:
	const Port* _source_port;		//!< points to a Function Port if source is a Function, otherwise = 0
	const Port* _target_port;		//!< points to a Function Port if target is a Function, otherwise = 0
public:
	//! Build a new edge without source and target.
	CDFGedge(): _source_port(0), _target_port(0)
	{
#ifdef CHECK
		cdfgedge_create++;
#endif
	}
	//! Delete an edge.
	~CDFGedge()
	{
#ifdef CHECK
		cdfgedge_delete++;
#endif
	}
	//! Set source port.
	void sourcePort(const Port*source)
	{
		_source_port = source;
	}
	//! Get source port.
	const Port* sourcePort()const
	{
		return _source_port;
	}
	//! Set target port.
	void targetPort(const Port*target)
	{
		_target_port = target;
	}
	//! Get target port.
	const Port* targetPort()const
	{
		return _target_port;
	}
	//! get name.
	string name()const
	{
		return source->name()+ "->" + target->name();
	}
	//! Print info. Required by mother class.
	void info()const
	{
		cout << "  " << source->name();
		if(_source_port)cout << "." << _source_port->name();
		cout << " -> " << target->name();
		if(_target_port)cout << "." << _target_port->name();
		cout << endl;
	}
};

#include "data.h"
#include "switches.h"


//! GAUT's CDFG. Is a subclass of Graph with specific CDFGnode and CDGGedge.
class CDFG : public Graph<CDFGnode, CDFGedge>
{
private:
	void checkBank(const ParseObject*object, Data*d);// check bank info for DATA only
	/* GL 18/09/08 : add lines*/
	void checkPort(const ParseObject*object, Data*d);// check port info for DATA only
	/* End GL*/
	CDFGnode* _source;		// keep a pointer to the DAG start
	CDFGnode* _sink;		// keep a pointer to the DAG end
	void setFunction(CDFGnodeRefs &cdfg_nodes_by_name, CDFGnode*node, const ParseObject*object, const FunctionRefs &functions)const;
	void connectIO(CDFGnodeRefs &cdfg_nodes_by_name, CDFGnode*node, const ParseObject*object, const string &io, const string &io_map, bool input);
	void convert(const ParseObjects*data, const FunctionRefs &functions);
	bool _io_constraints_detected;
	const Switches*_sw;
	/* GL 25/10/07 : pattern(add lines)*/
	const PatternRefs* _patterns;			//!< patterns
	/* Fin GL*/
	/* GL 20/11/07 : pour les fixed*/
	bool _fixed_detected;
	/* Fin GL*/
public:
	bool _mapping_constraints;
	bool _bitwidth_Aware;
	/* GL 18/05/2007 : Cluster mode*/
	int _clusteringMode;			//!< clustering mode
	/* Fin GL*/
	long _clk;
	static bool _debug;		//!< For debug.
	//! Create a new CDFG.
	//! @param Input. cdfg_file_name is the name of the file to parse.
	//! @param Input. functions is the set of predefined functions.
	CDFG(
		 const string & cdfg_file_name,
		 const string io_file,
		 const string map_file,
		 const FunctionRefs &functions,
		 /* GL 25/10/07 : pattern(add lines)*/
		 const PatternRefs &patterns,
		 /* Fin GL*/
		 const bool io_constraints,
		 const bool mapping_constraints,
		 const bool bitwidth_Aware,
		 /* GL 18/05/2007 : Cluster mode*/
		 const int		clusteringMode,
		 /* Fin GL*/
		 const Switches*sw,
		 const Clock*clk): _sw(sw)
	{
		/* GL 20/11/07 : pour les fixed*/
		_fixed_detected = false;
		/* Fin GL*/
		_source = 0;
		_sink = 0;
		_io_constraints_detected = io_constraints;
		_mapping_constraints = mapping_constraints;
		_bitwidth_Aware = bitwidth_Aware;
		/* GL 25/10/07 : pattern(add lines)*/
		_patterns = &patterns;
		/* Fin GL*/
		/* GL 18/05/2007 : Cluster mode*/
		_clusteringMode = clusteringMode;
		/* Fin GL*/
		_clk = clk->period();

		vector<string> cdfg_files;
		cdfg_files.push_back(cdfg_file_name);		// CDFG file name
		if(io_file.size())cdfg_files.push_back(io_file);	// add IO constraints file
		if(map_file.size())cdfg_files.push_back(map_file);	// add mapping constraints file
		Parser*parser = new Parser();				// build temporary parser
		ParseObjects*objs = new ParseObjects();	// build temporary objects
		parser->parse(cdfg_files, objs);			// parse file
		delete parser;								// free memory
		//objs->info();								// dump parsed data
		convert(objs, functions);					// exploit data
		/* GL 11/06/07 : propagate bitwidth info to operation node*/
		propagateBitwidth();
		//info();
		delete objs;								// free parsing memory
#ifdef CHECK
		cdfg_create++;
#endif
	}
	//! Delete a CDFG.
	~CDFG()
	{
		// delete all nodes: Graph will do it !
		CDFGnode*n;
		//for(vector<CDFGnode*>::iterator n_it = nodes().begin(); n_it != nodes().end(); n_it++)delete*n_it;
#ifdef CHECK
		cdfg_delete++;
#endif
	}

	//! Get source node.
	CDFGnode* source()
	{
		if(!_source)
		{
			cerr << "Error in CDFG: no source node" << endl;
			exit(1);
		}
		return _source;
	}
	//! Get source node(const version).
	const CDFGnode* source()const   // const version
	{
		if(!_source)
		{
			cerr << "Error in CDFG: no source node" << endl;
			exit(1);
		}
		return _source;
	}
	//! Get sink node.
	CDFGnode* sink()
	{
		if(!_sink)
		{
			cerr << "Error in CDFG: no sink node" << endl;
			exit(1);
		}
		return _sink;
	}
	//! Get sink node(const version).
	const CDFGnode* sink()const   // const version
	{
		if(!_sink)
		{
			cerr << "Error in CDFG: no sink node" << endl;
			exit(1);
		}
		return _sink;
	}
	// basic algorithms on CDFGs
	//! Run an ASAP on the CDFG.
	bool ASAP(long start);
	//! Run an ALAP on the CDFG.
	bool ALAP(long stop);
	//! Get CDFG latency = alap of the sink.
	long latency()const
	{
		return sink()->start()+/* DH: 26/10/2009*/ _clk /* FIN DH: 26/10/2009*/;
	}
	//! Get CDFG estimated best latency = alap of the sink.
	long estimated_best_latency()const
	{
		return sink()->asap()+/* DH: 26/10/2009*/ _clk /* DH: 26/10/2009*/;
	}
	//! Reset CDFG.
	void reset(const SelectionOutRefs*mdos);
	//! Print info.
	void info()const
	{
		//cout << "Latency=" << latency();
		if(io_constraints_detected())cout << " +IO constraints detected";
		cout << endl;
		Graph<CDFGnode, CDFGedge>::info();
	}
	//! tell if there are IO constraints in the CDFG.
	bool io_constraints_detected()const
	{
		return _io_constraints_detected;
	}
	//! Get critical loop.
	long criticalLoop(const Data**critical)const;
	//! Extract list of used functions.
	void extract_list_of_used_functions(FunctionRefs*used_functions)const;
	//! Serialize.
	void serialize(const string &file_name)const;
	/* Caaliph 27/06/2007*/
	void serialize_STAR(const string &file_name, const string &prefix_name, Cadency*cadency)const;
	void serialize_STAR(const string &file_name, const string &prefix_name, Cadency*cadency, const string &mode)const;
	void serialize_STAR(const string &file_name, const string &prefix_name, Cadency*cadency, vector<const CDFGnode*>*v, const string &mode)const;
	void serialize(const string &file_name, const string &mode)const;
	void serialize(const string &file_name, vector<const CDFGnode*>*v, const string &mode)const;
	/* End Caaliph*/
	//! propagateLocks
	void propagateLocks()const;
	//! locked sink node
	void lockedSinkNode(long cadency, long clk, long stages, Switches::Scheduling_strategy sty)const;
	/* GL 11/06/07 : propagate bitwidth info to operation node*/
	void propagateBitwidth()const;
	/* Fin GL*/
	/* GL 25/10/07 : pattern(add lines)*/
	const PatternRefs & patterns()const
	{
		return(*_patterns);
	}
	/* Fin GL*/
	/* GL 20/11/07 : pour les fixed*/
	bool fixed_detected()const
	{
		return _fixed_detected;
	}

	void show(void) const;
	int launchProc(const char* cmd, bool wait) const;
	void splitCmdLineToArgcArgv(std::string& line, int& argc, char ** &argv) const;
	/* Fin GL*/

};

extern void CDFGstat();

#endif // __CDFG_H__
