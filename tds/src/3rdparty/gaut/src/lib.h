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

//	File:		lib.h
//	Purpose:	technology dependant operators library
//	Author:		Pierre Bomel, LESTER, UBS

class Lib;

#ifndef __LIB_H__
#define __LIB_H__

#include <string>
#include "parser.h"
#include "operator.h"
#include "function.h"
/* GL 25/10/07 : pattern (add lines)*/
#include "pattern.h"
/* Fin GL */
using namespace std;

//! Library of operators.
class Lib
{
private:
    // objects data members
    long _bits;						//!< number of bits for all operators
    OperatorRefs _operators;		//!< operators
    FunctionRefs *_functions;		//!< functions
    /* GL 25/10/07 : pattern (add lines)*/
    PatternRefs *_patterns;			//!< patterns
    bool _chaining;
    /* Fin GL */
    bool _i_am_a_copy;				//!< to avoid freeing ops and funcs twice
    bool _bitwidthAware;				//!< new flow mode
    bool _verbose;				//!< verbose mode
    /* GL 18/05/2007 : Cluster mode */
    int _clusteringMode;			//!< clustering mode
    /* Fin GL */
    /* GL 27/06/2007 : needed for Clustering mode 2, 3*/
    const Clock *_clk;				//!< clock
    /* Fin GL */
    void *_addr;					//!< Generic pointer to extend easily.

public:
    //! Get number of bits.
    long bits() const
    {
        return _bits;
    }
    //! access to function list
    const FunctionRefs & functions() const
    {
        return (*_functions);
    }
    //! access to operator list
    const OperatorRefs & operators() const
    {
        return _operators;
    }
    /* GL 25/10/07 : pattern (add lines)*/
    //! access to pattern list
    const PatternRefs & patterns() const
    {
        return (*_patterns);
    }
    /* Fin GL */

    //! create me
    Lib(const string & builtins,	// builtins functions
        const string & operators,	// target operators
        const string & userlib,		// user defined functions and operators, if any
        /* GL 25/10/07 : pattern (add lines)*/
        const string & pattern,
        const bool chaining,
        /* Fin GL */
        const bool     bitwidthAware, // new flow mode
        const bool     verbose, // verbose mode
        /* GL 18/05/2007 : Cluster mode */
        const int		clusteringMode,
        /* Fin GL */
        const Clock  *clk			// clock
       )
    {
        _bitwidthAware = bitwidthAware;
        _verbose = verbose;
        /* GL 18/05/2007 : Cluster mode */
        _clusteringMode = clusteringMode;
        /* Fin GL */
        /* GL 27/06/2007 : needed for Clustering mode 2, 3*/
        _clk = clk;
        /* Fin GL */
        _addr = 0;
        _functions = new FunctionRefs();
        /* GL 25/10/07 : pattern (add lines)*/
        _patterns = new PatternRefs();
        _chaining = chaining;
        /* Fin GL */
        vector<string> data_files;
        data_files.push_back(builtins);		// builtin functions
        data_files.push_back(operators);	// target specific operators
        /* GL 25/10/07 : pattern (add lines) */
        if (_chaining) data_files.push_back(pattern);
        /* Fin GL */
        if (userlib.size() != 0) data_files.push_back(userlib);	// user defined functions and operators, if any
        // parse functions and operators
        Parser *parser = new Parser();
        ParseObjects *data = new ParseObjects();	// parse all data and check consistency
        parser->parse(data_files, data);
        delete parser;								// free memory
        // exploit data
		/* DEBUG_DH
        data->info(); FIN DEBUG_DH */
        convert(data, clk);
        // free parsing memory
        delete data;
        //dynamicParsingUsage(); just to check dynamic memory allocation and free
        _i_am_a_copy = false; // set copy toggle to avoid double memory free with delete !
#ifdef CHECK
        lib_create++;
#endif
    }
    //! copy me
    Lib(const Lib & l) : _bits(l._bits)
    {
        //cout << "lib copying" << endl;
        _operators = l._operators;
        _functions = l._functions;
        /* GL 25/10/07 : pattern (add lines)*/
        _patterns = l._patterns;
        /* Fin GL */
        _i_am_a_copy = true;
#ifdef CHECK
        lib_create++;
#endif
    }
    //! delete me
    ~Lib()
    {
        if (_i_am_a_copy)
        {
            // don't free operators and functions, the owner will
            //cout << "lib is a copy" << endl;
            return;
        }
        //cout << "deleting ops" << endl;
        // I'm the owner: really delete all operators
        for (OperatorRefs::iterator op_it = _operators.begin(); op_it != _operators.end(); op_it++)
        {
            OperatorRef op_ref = (*op_it).second;
            Operator *op_ptr = op_ref;
            delete op_ptr;
        }
        //cout << "deleting funcs" << endl;
        // really delete all functions
        for (FunctionRefs::iterator f_it = _functions->begin(); f_it != _functions->end(); f_it++)
        {
            FunctionRef f_ref = (*f_it).second;
            Function *f_ptr = f_ref;
            delete f_ptr;
        }
        /* GL 25/10/07 : pattern (add lines)*/
        // really delete all patterns
        for (PatternRefs::iterator p_it = _patterns->begin(); p_it != _patterns->end(); p_it++)
        {
            PatternRef p_ref = (*p_it).second;
            Pattern *p_ptr = p_ref;
            delete p_ptr;
        }
        /* Fin GL */
        //cout << "delete done" << endl;
#ifdef CHECK
        lib_delete++;
#endif
    }
    //! dump on console
    void info() const
    {
        cout << "bits " << _bits << endl;
        _functions->info();
        _operators.info();
        /* GL 25/10/07 : pattern (add lines)*/
        _patterns->info();
        /* Fin GL */
    }

    //! serialize
    void serialize(const string &file_name) const
    {
        // open
        ofstream f(file_name.c_str(), ios::out);
        f << "bits " << _bits << "\n";
        _functions->serialize(f);
        _operators.serialize(f);
        /* GL 25/10/07 : pattern (add lines)*/
        _patterns->serialize(f);
        /* Fin GL */
        // close
        f.close();
    }

    /* EJ 18/04/2007 : suppression des deux fonctions suivantes
    /* GL 19/05/2007 : les deux fonctions serviront pour le clusteringMode 1 */
    //! get new function name in new flow mode
    /* GL 27/06/07 : clustering mode 2, 3
    				 Replace line
    string newFunctionName(const ParseObject *object, string &functionName)
    /* By */
    string newFunctionName(const ParseObject *object, string functionName, long propagation_time )
    {
        const ParseAttribute *att_inputs_bitwidth = object->attribute("inputs_bitwidth");
        const ParseAttribute *att_inputs_signed = object->attribute("inputs_signed");
        const ParseAttribute *att_outputs_bitwidth = object->attribute("outputs_bitwidth");
        const ParseAttribute *att_outputs_signed = object->attribute("outputs_signed");
        /* GL 27/06/07 : clustering mode 2, 3 */
        const ParseAttribute *att_area = object->attribute("area");
        /* Fin GL */
        // new name
        string newName=functionName;
        int i;
		if (_bitwidthAware)
        {
            for (i = 0; i < att_inputs_bitwidth->_values.size(); i++)
            {
                string Signed = att_inputs_signed->value(i);
                string bitwidth = att_inputs_bitwidth->value(i);
				if ((_clusteringMode == 1) || (_clusteringMode == 0))
					newName += "$"+Signed+bitwidth;
				else
					newName += "$"+Signed;
            }
            for (i = 0; i < att_outputs_bitwidth->_values.size(); i++)
            {
                string Signed = att_outputs_signed->value(i);
                string bitwidth = att_outputs_bitwidth->value(i);
				if ((_clusteringMode == 1) || (_clusteringMode == 0))
					newName += "$"+Signed+bitwidth;
				else
					newName += "$"+Signed;
            }
		    if (_clusteringMode == 2)
			{
				newName += "$"+ Parser::itos((long) ceil((double)(propagation_time/_clk->period()))+1) + "cycle";
			}
        }
        return newName;
    }

    void updateParamPort(const ParseObject *object, Function *f)
    {
        const ParseAttribute *att_inputs_bitwidth = object->attribute("inputs_bitwidth");
        const ParseAttribute *att_inputs_signed = object->attribute("inputs_signed");
        const ParseAttribute *att_outputs_bitwidth = object->attribute("outputs_bitwidth");
        const ParseAttribute *att_outputs_signed = object->attribute("outputs_signed");
        /* GL 10/10/07 : add lines..fixedpoint */
        const ParseAttribute *att_inputs_fixedpoint ;
        const ParseAttribute *att_outputs_fixedpoint ;
        /* Fin GL */
        int i;
        for (i = 0; i < att_inputs_bitwidth->_values.size(); i++)
        {
            Port *p = (Port*) f->getInputPortByPos(i);

			if ((!p) && (f->symbolic_function()=="mem_read"))
			{
				ostringstream myStream;
				myStream << "depedency_" << i;
				f->addPort(myStream.str(), true, i);
				p = (Port*) f->getInputPortByPos(i);
			}

            string _signed = att_inputs_signed->value(i);
            /* GL 10/10/07 : add lines..fixedpoint */
            if (object->getAttribute("inputs_fixedpoint", &att_inputs_fixedpoint))
            {
                p->fixedPoint(Parser::atol(att_inputs_fixedpoint->value(i)));
                if (_signed=="s") p->setSigned(Port::SFIXED);
                else if (_signed=="u") p->setSigned(Port::UFIXED);
                else
                {
                    cerr << "Error: unknown value '" << _signed << "' in attribute '" << att_inputs_bitwidth->_name << "'" << endl;
                    exit(1);
                }
            }
            else
            {
                /* Fin GL */
                if (_signed=="s") p->setSigned(Port::SIGNED);
                else if (_signed=="u") p->setSigned(Port::UNSIGNED);
                else
                {
                    cerr << "Error: unknown value '" << _signed << "' in attribute '" << att_inputs_bitwidth->_name << "'" << endl;
                    exit(1);
                }
                /* GL 10/10/07 : add lines..fixedpoint */
            }
            /* Fin GL */
            /* GL 27/06/07 : Clustering modes 0, 2, 3 */
            // update only if bitwidth is larger than old one if exists
            if (p->bitwidth() < Parser::atol(att_inputs_bitwidth->value(i)))
            {
                /* Fin GL */
                p->bitwidth(Parser::atol(att_inputs_bitwidth->value(i)));
            }
        }
        for (i = 0; i < att_outputs_bitwidth->_values.size(); i++)
        {
            Port *p = (Port*) f->getOutputPortByPos(i);
            string _signed = att_outputs_signed->value(i);
            /* GL 10/10/07 : add lines..fixedpoint */
            if (object->getAttribute("outputs_fixedpoint", &att_outputs_fixedpoint))
            {
                p->fixedPoint(Parser::atol(att_outputs_fixedpoint->value(i)));
                if (_signed=="s") p->setSigned(Port::SFIXED);
                else if (_signed=="u") p->setSigned(Port::UFIXED);
                else
                {
                    cerr << "Error: unknown value '" << _signed << "' in attribute '" << att_outputs_bitwidth->_name << "'" << endl;
                    exit(1);
                }
            }
            else
            {
                /* Fin GL */
                if (_signed=="s") p->setSigned(Port::SIGNED);
                else if (_signed=="u") p->setSigned(Port::UNSIGNED);
                else
                {
                    cerr << "Error: unknown value '" << _signed << "' in attribute '" << att_inputs_bitwidth->_name << "'" << endl;
                    exit(1);
                }
                /* GL 27/06/07 : Clustering modes 1, 2, 3 */
            }
            /* Fin GL */
            /* GL 27/06/07 : Clustering modes 1, 2, 3 */
            // update only if bitwidth is larger than old one if exists
            if (p->bitwidth() < Parser::atol(att_outputs_bitwidth->value(i)))
            {
                /* Fin GL*/
                p->bitwidth(Parser::atol(att_outputs_bitwidth->value(i)));
                /* GL 10/10/07 : add lines..fixedpoint */
            }
            /* Fin GL */
        }
    }
    /* Fin GL
     */
    /* Fin EJ */


    // convert parsed data into internal data

    /* CONVERSION =
    	1/ translate parsed data to internal instances of classes
    	2/ compute specific values:
    		- clock_cycles of operators
    	3/ Final coherency checks

      Hypothesis:
    	Parsed data base is 100% correct
    	All functions are defined before operators

      1/ Conversion rules are:

    	for each parsed object P

    		if type of P is "bits"
    			_bits				= int(name of P)
    		endif

    		if type of P is "function"
    			create a new C++ Function instance X in _functions
    			X._name						= string(name of P)
    			X._inputs					= vector of FunctionPort
    			X._outputs					= vector of FunctionPort
    		endif

    		if type of P is "operator"
    			create a new C++ Operator instance X in _operators
    			X._name						= string(name of P)
    			X._area						= long(value of attribute "area" of P)
    			for each 3-uple of values of attributes ("propagation_time", "power", "function") U
    				create a new C++ ImplementedFunction IF
    				IF._propagation_time	= U.1
    				IF._power				= U.2
    				IF._function			= U.3
    				X._implementedFunctionRefs	+= Ref<IF>
    			endfor
    		endif

    	endfor

      2/ build implemented functions list

      3/ Post data-read values to create

    	for each Function instance F
    		F._implemented_by		= Refs<address of Operator instances with _functionRef = address of F>
    	endfor

      4/ Final checks are:

    	each Function F must have at least an implementation operator P.

    */
    //! Conversion from parsed objects to a library.
    void convert(ParseObjects *objects, const Clock *clk);

};

#endif // __LIB_H__
