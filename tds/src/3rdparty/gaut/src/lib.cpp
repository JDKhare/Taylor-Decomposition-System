
#include <string>
#include "parser.h"
#include "operator.h"
#include "function.h"
#include "pattern.h"
#include "lib.h"
using namespace std;


//! Conversion from parsed objects to a library.
void Lib::convert(ParseObjects *objects, const Clock *clk)
{
    ParseObjects::Obase::const_iterator it;

    /* Fin GL */
    if (_bitwidthAware) _addr = new FunctionRefs();
    /* Fin GL */

    // 1
    // conversion

    // all parsed objects must be duplicated with their internal equivalent data structures
    for (_bits = 0, it = objects->begin(); it != objects->end(); it++)
    {
        const ParseObject *object = (*it).second;
        if (object->_type == "bits")
        {
            _bits					= Parser::atoi(object->_name);
            break;
        }
    }

    // all parsed objects must be duplicated with their internal equivalent data structures
    for (it = objects->begin(); it != objects->end(); it++)
    {

        const ParseObject *object = (*it).second;

        if (object->_type == "function")
        {
            int i;
            string name				= object->_name;
            Function *f = new Function(name);

            // EJ 07/07/2008 : levinson
            if (name == "mem_read" || name == "mem_write")
                f->mem_access(true);
            // End EJ

            /* GL 27 /06 /07 : add symbolic function name */
            f->symbolic_function(name);
            /* Fin GL */
            // add to set of functions
            FunctionRef f_ref(f);
            _functions->add(f_ref);
            const ParseAttribute *att_inputs = object->attribute("inputs");
            for (i = 0; i < att_inputs->_values.size(); i++)
            {
                string port_name = att_inputs->value(i);
                f->addPort(port_name, true, i);
                Port *p = (Port*) f->getInputPortByPos(i);

                if (_bitwidthAware) 
					p->bitwidth(_bits); // default value
            }
            const ParseAttribute *att_outputs = object->attribute("outputs");
            for (i = 0; i < att_outputs->_values.size(); i++)
            {
                string port_name = att_outputs->value(i);
                f->addPort(port_name, false, i);
                Port *p = (Port*) f->getOutputPortByPos(i);
                if (_bitwidthAware) 
					p->bitwidth(_bits); // default value
            }
            /* GL 25/10/07 : pattern (add lines)*/
        }
        else if ( (_chaining)&&(object->_type == "pattern"))
        {
            int i;
            string name;
            const ParseAttribute *att_sources         = object->attribute("sources");
            const ParseAttribute *att_targets         = object->attribute("targets");
            for (i = 0; i < att_sources->_values.size(); i++)
            {
                name += att_sources->_values[i];
                if (i != att_sources->_values.size()-1) name += ".";
            }
            name += "_";
            for (i = 0; i < att_targets->_values.size(); i++)
            {
                name += att_targets->_values[i];
                if (i != att_targets->_values.size()-1) name += ".";
            }
            const long cycle = Parser::atol(object->attribute("cycle")->_values[0]);
            Pattern *p = new Pattern(name, cycle);
            PatternRef p_ref(p);
            _patterns->add(p_ref);
            /* Fin GL */
        }
        else if (object->_type == "operator")
        {
            long i;
            string name = object->_name;

            long area = Parser::atol(object->attribute("area")->_values[0]);
            string type = object->attribute("type")->_values[0];
            bool synchronous = (type == "synchronous");

            long cadency, latency;
            latency = 1;
            cadency = 1;
            if (synchronous)
            {
	            const ParseAttribute *att_latency;
	            const ParseAttribute *att_cadency;
	            if (object->getAttribute("latency", &att_latency))
					latency = Parser::atol(att_latency->_values[0]);

				/* DEBUG_DH cout << "latency " << latency << endl; */
	            if (object->getAttribute("cadency", &att_cadency))
					cadency = Parser::atol(att_cadency->_values[0]);
				/* DEBUG_DH cout << "cadency " << cadency << endl; */
            }

            // ctrl port name
            const ParseAttribute *att_ctrl_port;
            string ctrl_name;
            if (object->getAttribute("ctrl_port", &att_ctrl_port))
            {
                ctrl_name = att_ctrl_port->_values[0];
            }
            else  	// implicit naming: "ctrl"
            {
                ctrl_name = /*GL 20/11/07 : change name */ "cmd"; // /* Fin GL */"ctrl";	// default value
            }

            // clk port name
            const ParseAttribute *att_clk_port;
            string clk_name;
            if (object->getAttribute("clk_port", &att_clk_port))
            {
                clk_name = att_clk_port->_values[0];
            }
            else  	// implicit naming: "clk"
            {
                clk_name = "clk"; // default value
            }

            // reset port name
            const ParseAttribute *att_rst_port;
            string rst_name;
            if (object->getAttribute("rst_port", &att_rst_port))
            {
                rst_name = att_rst_port->_values[0];
            }
            else  	// implicit naming: "rst"
            {
                rst_name = "rst"; // default value
            }

            // reset polarity
            const ParseAttribute *att_rst_pol;
            string polarity;
            if (object->getAttribute("rst_pol", &att_rst_pol))
            {
                polarity = att_rst_pol->_values[0];
            }
            else  	// implicit value: '0' (active low)
            {
                polarity = "low";	// default value
            }
            Operator::Polarity pol = polarity == "high" ? Operator::HIGH : Operator::LOW;
			/* GL 18/06/08 :  enable signal */
			// enable port name
			const ParseAttribute *att_enable_port;
			string enable_name;
			if (object->getAttribute("enable_port", &att_enable_port))
			{
				enable_name = att_enable_port->_values[0];
			}
			else  	// implicit naming: "enable"
			{
				enable_name = "enable"; // default value
			}

			/* DEBUG_DH cout << "enable_name " << enable_name << endl;*/
			// enable polarity
			const ParseAttribute *att_enable_pol;
			string e_polarity;
			if (object->getAttribute("enable_pol", &att_enable_pol))
			{
				e_polarity = att_enable_pol->_values[0];
			}
			else  	// implicit value: '0' (active low)
			{
				e_polarity = "high";	// default value
			}
			Operator::Polarity e_pol = e_polarity == "high" ? Operator::HIGH : Operator::LOW;

            // component
            string component;
            const ParseAttribute *att_component;
            if (object->getAttribute("component", &att_component))
            {
                component = att_component->value(0);
            }
            else
            {
                component = name;
            }
            /* GL 14/05/07 : Add bitwidths and signeds info */
            vector<long> inputsBitwidth, outputsBitwidth;
            vector<bool> inputsSigned, outputsSigned;
			inputsBitwidth.clear();
			outputsBitwidth.clear();
			inputsSigned.clear();
			outputsSigned.clear();
            if (_bitwidthAware)
            {
                // inputsBitwidth
                const ParseAttribute *att_inputs_bitwidth;
				if (object->getAttribute("inputs_bitwidth", &att_inputs_bitwidth))
				{
					for (i = 0; i < att_inputs_bitwidth->_values.size(); i++)
						inputsBitwidth.push_back(Parser::atol(att_inputs_bitwidth->value(i)));
				}
                // inputsSigned
                const ParseAttribute *att_inputs_signed;
				if (object->getAttribute("inputs_signed", &att_inputs_signed))
				{
					for (i = 0; i < att_inputs_signed->_values.size(); i++)
				    {
		                if (att_inputs_signed->value(i)=="s")	inputsSigned.push_back(true);
			            else									inputsSigned.push_back(false);
	                }
				}
                // outputsBitwidth
                const ParseAttribute *att_outputs_bitwidth;
				if (object->getAttribute("outputs_bitwidth", &att_outputs_bitwidth))
				{
					for (i = 0; i < att_outputs_bitwidth->_values.size(); i++)
						outputsBitwidth.push_back(Parser::atol(att_outputs_bitwidth->value(i)));
				}
                // outputsSigned
                const ParseAttribute *att_outputs_signed;
				if (object->getAttribute("outputs_signed", &att_outputs_signed))
				{
					for (i = 0; i < att_outputs_signed->_values.size(); i++)
					{
						if (att_outputs_signed->value(i)=="s")	outputsSigned.push_back(true);
						else									outputsSigned.push_back(false);
					}
				}
            }
            // create new operator
            /* Replace following line
            Operator *o = new Operator(name, component, area, synchronous, cadency, latency,
            						   pol, rst_name, clk_name, ctrl_name);
            /* By */
			Operator *o = new Operator(name, component, area, synchronous, cadency, latency,
			                           pol, rst_name, clk_name, ctrl_name,
			                           /* GL 18/06/08 */
			                           e_pol, enable_name,
			                           /* Fin GL */
			                           inputsBitwidth, inputsSigned, outputsBitwidth, outputsSigned);
			/* Fin GL */
			/* GL 18/06/08 :  synchronous without fsm operators */
			const ParseAttribute *att_noFSM;
			string noFSM;
			if (object->getAttribute("noFSM", &att_noFSM))
			{
				noFSM = att_noFSM->_values[0];
				if (noFSM == "yes")
				{
					o->noFSM(true);
				}
				else if (noFSM == "no")
				{
					; // nothing to do
				}
				else
				{
					cerr << "Error: unknown value  '" << noFSM << "' in attribute '" << "noFSM" << "'" << endl;
					exit(1);
				}
			}
            /* Fin GL */
				/* GL 21/10/08 : clock ration (divided by) */
				const ParseAttribute *att_ratio; 
				int ratio;
				if (object->getAttribute("clock_ratio", &att_ratio)) {
					ratio = Parser::atoi(att_ratio->_values[0]);
					o->clock_ratio(ratio);
				}
				/* Fin GL */
				/* GL 10/06/09 : pour specifier une clock min pour les opr sequentiels*/
				long min_period;
				/*	if (synchronous)
					min_period = Parser::atol(object->attribute("minimum_period")->_values[0]);
				else */
					min_period = clk->period();
				o->minimum_period(min_period);
				/* Fin GL */
				const ParseAttribute *att_communication;
				bool communication;
				if (object->getAttribute("communication", &att_communication))
				{
					communication = (att_communication->_values[0] == "synchronous");
					o->synCOM(communication);
				}
				/* Fin GL */
            // passThrough
            const ParseAttribute *att_passThrough;
            string passThrough;
            if (object->getAttribute("passThrough", &att_passThrough) || object->getAttribute("passthrough", &att_passThrough))
            {
                passThrough = att_passThrough->_values[0];
                if (passThrough == "yes")
                {
                    o->passThrough(true);
                }
                else if (passThrough == "no")
                {
                    ; // nothing to do
                }
                else
                {
                    cerr << "Error: unknown value  '" << passThrough << "' in attribute '" << "passThrough" << "'" << endl;
                    exit(1);
                }
            }

            const ParseAttribute *att_function          = object->attribute("function");
            string function_name						= att_function->_values[0];
            long size;
            Function *f;

            if (!_functions->search(function_name, &f))
            {
                cerr << "Error: unknown function '" << function_name << "' in operator '" << name << "'" << endl;
                exit(1);
            }
            size = f->inputs();
            vector<string> ports(size);					// operator ports
            vector<long> symbolic_ports_no(size);		// mapped function ports

			const ParseAttribute *att_latency=NULL;
			const ParseAttribute *att_propagation_time  = NULL;
			long propagation_time=1;
			if (o->synchronous())
			{
				if (object->getAttribute("latency", &att_latency))
					propagation_time = Parser::atol(att_latency->_values[0])* clk->period();
			}
			else
			{
				if (object->getAttribute("propagation_time", &att_propagation_time))
					propagation_time = Parser::atol(att_propagation_time->_values[0])* clk->period();
			}
				/* Fin GL */				
            if (_bitwidthAware) 
            {
                int j;
                for (j = 0; j < att_function->_values.size(); j++)
                {
                    if (!_functions->search(att_function->value(j), &f))
                    {
                        cerr << "Error: unknown function '" << function_name << "' in operator '" << name << "'" << endl;
                        exit(1);
                    }
						/* GL 10/06/09 : correction bug temps de release
										 vu avec Philippe et Dominique 						
										 Replace line 
                    long propagation_time	= Parser::atol(att_propagation_time->value(j));
						/* By */

					long propagation_time=0;
					if (o->synchronous())
					{
						if (object->getAttribute("latency", &att_latency))
							if (att_latency->_values.size()>j)
								propagation_time = Parser::atol(att_latency->_values[j])* clk->period();
					}
					else
					{
						if (object->getAttribute("propagation_time", &att_propagation_time))
							if (att_propagation_time->_values.size()>j)
								propagation_time = Parser::atol(att_propagation_time->_values[j]);
					}
                    string new_name = newFunctionName(object, att_function->value(j), propagation_time);
                    /* Fin GL */
                    Function *function = new Function(new_name);
				   if (f->name() == "mem_read" || f->name() == "mem_write")
			           f->mem_access(true);
	                    function->symbolic_function(f->name());
                    for (i = 0; i < f->inputPorts().size(); i++)
                    {
                        Port *p = (Port*)f->getInputPortByPos(i);
                        p->bitwidth(0);
                        function->addPort(p->name(), true, i);
                    }
                    for (i = 0; i < f->outputPorts().size(); i++)
                    {
                        Port *p = (Port*)f->getOutputPortByPos(i);
                        p->bitwidth(0);
                        function->addPort(p->name(), false, i);
                    }
                    /* GL 27/06/07 : no need for if
                    				replace line
                    /* GL 06/06/07 if (_bitwidthAware) /* Fin GL updateParamPort(object, function); // bitwidth and signed
                    /* By */
                    if (((FunctionRefs*)_addr)->search(function->name(), &f))function = f;
                    else
                    {
                        for (i = 0; i < function->inputPorts().size(); i++)
                            ((Port*)function->getInputPortByPos(i))->bitwidth(0);
                        for (i = 0; i < function->outputPorts().size(); i++)
                            ((Port*)function->getOutputPortByPos(i))->bitwidth(0);
                    }
                    updateParamPort(object, function); // bitwidth and signed
                    /* End GL */
                    // add to set of functions
                    FunctionRef f_new_ref(function);
                    if (!((FunctionRefs*)_addr)->search(function->name(), &f))
                    {
                        ((FunctionRefs*)_addr)->add(f_new_ref);
                    }
                    f = function;
                } /* Fin GL */
            }
            /* Fin GL */

            // input port names
            const ParseAttribute *att_input_ports;
            if (object->getAttribute("input_ports", &att_input_ports))
            {
                if (size != att_input_ports->_values.size())
                {
                    cerr << "Error: number of input ports of operator " << name << " does not match number of function ports " << size << endl;
                    exit(1);
                }
 
                for (i = 0; i < size; i++)
                {
                    ports[i] = att_input_ports->_values[i];
                }
            }
            else  	// implicit naming: same names than function's symbolic ports
            {
                // all functions have the same "signature"
                for (i = 0; i < size; i++)
                {
                    ports[i] = f->getInputPortByPos(i)->name();
                }
            }
            // input ports mapping
            const ParseAttribute *att_input_map;
            if (object->getAttribute("input_map", &att_input_map))
            {
                if (size != att_input_map->_values.size())
                {
                    cerr << "Error: number of input ports maps of operator " << name << " does not match number of function ports " << size << endl;
                    exit(1);
                }
                for (i = 0; i < size; i++)
                {
                    string port_name = att_input_map->_values[i];
                    const Port *port;
                    if (!(port = f->getInputPortByName(port_name)))
                    {
						/* GL 18/06/08 :  synchronous without fsm operators */
						if (o->noFSM())
						{
							for (i = 0; i< size; i++)
							{
								for (int j = 0; j< size; j++)
								{
									if (port_name == ports[j])
									{
										symbolic_ports_no[i] = j;
										break;
									}
								}
							}
						}
						else
						{
							/* Fin GL */
                        cerr << "Error: operator " << o->name() << " contains an unknown input port " << port_name << " for function " << f->name() << endl;
                        exit(1);
							/* GL 18/06/08 :  synchronous without fsm operators */
						}
						/* Fin GL */
                    }
                    symbolic_ports_no[i] = port->pos();
                }
            }
            else  	// implicit naming: same order than function's symbolic ports
            {
                for (i = 0; i < size; i++)
                {
                    symbolic_ports_no[i] = i;
                }
            }
            // set input port mapping
            o->resizeInputPorts(size);
            for (i = 0; i < size; i++)
            {
                const Port *p = new Port(ports[i], Port::IN, symbolic_ports_no[i], f, o);
                o->addPort(i, p);
                Port *port = (Port*)o->getInputPort(i);
					/* GL 26/04/09 : Timing behavior */
					// Timing behavior of inputs
					const ParseAttribute *att_input_timings;
					if (object->getAttribute("input_timings", &att_input_timings)) {
						if (size != att_input_timings->_values.size())
                    {
							cerr << "Error: Input timing behavior has to be specified for all ports " << endl;
							exit(1);
						}
						if (att_input_timings->_values[i] == "*") {
							port->hasVariableTime(true);
							o->synCOM(false);
						}
					}
					/* Fin GL*/						
                if (_bitwidthAware)
                    {
                    /* EJ 18/04/2007 : modification des deux lignes suivantes
                    port->bitwidth(f->getInputPortByPos(i)->bitwidth());
                    port->setSigned(f->getInputPortByPos(i)->getSigned());
                    par */
                    const ParseAttribute *att_inputs_bitwidth = object->attribute("inputs_bitwidth");
                    const ParseAttribute *att_inputs_signed = object->attribute("inputs_signed");
                    // EJ 07/07/2008 : levinson
                    if (function_name == "mem_read" || function_name == "mem_write")
                    {
                        port->bitwidth(Parser::atol(att_inputs_bitwidth->value(0)));
                    }
                    else
                    {
                        port->bitwidth(Parser::atol(att_inputs_bitwidth->value(i)));
                    }
                    /* GL 09/11/07 : add fixed info */
                    const ParseAttribute *att_inputs_fixedpoint ;
                    if (object->getAttribute("inputs_fixedpoint", &att_inputs_fixedpoint))
                    {
                        // EJ 07/07/2008 : levinson
                        if (function_name == "mem_read")
                        {
                            port->fixedPoint(Parser::atol(att_inputs_fixedpoint->value(0)));
                            if (att_inputs_signed->value(0)=="s")	port->setSigned(Port::SFIXED);
                            else									port->setSigned(Port::UFIXED);
                        }
                        else if (function_name == "mem_write")
                        {
                            port->fixedPoint(Parser::atol(att_inputs_fixedpoint->value(0)));
                            if (att_inputs_signed->value(0)=="s")	port->setSigned(Port::SFIXED);
                            else									port->setSigned(Port::UFIXED);
                        }
                        else
                        {
                            port->fixedPoint(Parser::atol(att_inputs_fixedpoint->value(i)));
                            if (att_inputs_signed->value(i)=="s")	port->setSigned(Port::SFIXED);
                            else									port->setSigned(Port::UFIXED);
                        }
                    }
                    else
                        /* Fin GL*/
                        // EJ 07/07/2008 : levinson
                        if (function_name == "mem_read")
                        {
                            if (att_inputs_signed->value(0)=="s")	port->setSigned(Port::SIGNED);
                            else									port->setSigned(Port::UNSIGNED);
                        }
                        else if (function_name == "mem_write")
                        {
                            if (att_inputs_signed->value(0)=="s")	port->setSigned(Port::SIGNED);
                            else									port->setSigned(Port::UNSIGNED);
                        }
                        else
                        {
                            if (att_inputs_signed->value(i)=="s")	port->setSigned(Port::SIGNED);
                            else									port->setSigned(Port::UNSIGNED);
                        }
                    /* Fin EJ */
                }
                else
                {
                    port->bitwidth(_bits);
                }
            }
				/* GL 20/12/08 : Complex operators */
				vector<long> inputBegins;
				// Arrival time of inputs
				const ParseAttribute *att_input_begins;
				if (object->getAttribute("input_begins", &att_input_begins)) {
					for (i = 0; i < att_input_begins->_values.size(); i++)
					/* GL 03/07/09 : Verification 
					Replace line
						inputBegins.push_back(Parser::atol(att_input_begins->value(i)));
					/* By*/
					{
						long time = Parser::atol(att_input_begins->value(i));
						if(time/* /_clk->period() */>latency) {
							cerr << "Error: operator " << o->name() << " has an input arrival time greater than his latency " << endl;
							exit(1);
						}
						inputBegins.push_back(time);
					}
					/*Fin GL*/
				}
				else inputBegins.assign(size, 0);
				o->input_begins(inputBegins);
				/* Fin GL*/				
            // free memory
            ports.clear();
            symbolic_ports_no.clear();

            // reset arrays
            size = f->outputs();
            ports.resize(size);
            symbolic_ports_no.resize(size);

            // output port names
            const ParseAttribute *att_output_ports;
            if (object->getAttribute("output_ports", &att_output_ports))
            {
                if (size != att_output_ports->_values.size())
                {
                    cerr << "Error: number of output ports of operator " << name << " does not match number of function ports " << size << endl;
                    exit(1);
                }
                for (i = 0; i < size; i++)
                {
                    ports[i] = att_output_ports->_values[i];
                }
            }
            else  	// implicit naming: same names than function's symbolic ports
            {
                // all functions have the same "signature"
                for (i = 0; i < size; i++)
                {
                    ports[i] = f->getOutputPortByPos(i)->name();
                }
            }
            // output ports mapping
            const ParseAttribute *att_output_map;
            if (object->getAttribute("output_map", &att_output_map))
            {
                if (size != att_output_map->_values.size())
                {
                    cerr << "Error: number of output ports maps of operator " << name << " does not match number of function ports " << size << endl;
                    exit(1);
                }
                for (i = 0; i < size; i++)
                {
                    string port_name = att_output_map->_values[i];
                    const Port *port;
                    if (!(port = f->getOutputPortByName(port_name)))
                    {
						/* GL 18/06/08 :  synchronous without fsm operators */
						if (o->noFSM())
						{
							for (i = 0; i< size; i++)
							{
								for (int j = 0; j< size; j++)
								{
									if (port_name == ports[j])
									{
										symbolic_ports_no[i] = j;
										break;
									}
								}
							}
						}
						else
						{
							/* Fin GL */
                        cerr << "Error: operator " << o->name() << " contains an unknown output port " << port_name << " for function " << f->name() << endl;
                        exit(1);
						}
                    }
                    symbolic_ports_no[i] = port->pos();
                }
            }
            else  	// implicit naming: same order than function's symbolic ports
            {
                for (i = 0; i < size; i++)
                {
                    symbolic_ports_no[i] = i;
                }
            }
            // set output port mapping
            o->resizeOutputPorts(size);
            for (i = 0; i < size; i++)
            {
                const Port *p = new Port(ports[i], Port::OUT, symbolic_ports_no[i], f, o);
                o->addPort(i, p);
                Port *port = (Port*)o->getOutputPort(i);
					/* GL 26/04/09 : Timing behavior */
					// Timing behavior of outputs
					const ParseAttribute *att_output_timings;
					if (object->getAttribute("output_timings", &att_output_timings)) {
						if (size != att_output_timings->_values.size())
						{
							cerr << "Error: Output timing behavior has to be specified for all ports " << endl;
							exit(1);
						}
						if (att_output_timings->_values[i] == "*") {
							port->hasVariableTime(true);
							o->synCOM(false);
						}
					}
					/* Fin GL*/						
                if (_bitwidthAware)
                {
                    /* EJ 18/04/2007 : modification des deux lignes suivantes
                    port->bitwidth(f->getOutputPortByPos(i)->bitwidth());
                    port->setSigned(f->getOutputPortByPos(i)->getSigned());
                    par */
                    const ParseAttribute *att_outputs_bitwidth = object->attribute("outputs_bitwidth");
                    const ParseAttribute *att_outputs_signed = object->attribute("outputs_signed");
                    port->bitwidth(Parser::atol(att_outputs_bitwidth->value(i)));
                    /* GL 09/11/07 : add fixed info */
                    const ParseAttribute *att_outputs_fixedpoint ;
                    if (object->getAttribute("outputs_fixedpoint", &att_outputs_fixedpoint))
                    {
                        port->fixedPoint(Parser::atol(att_outputs_fixedpoint->value(i)));
                        if (att_outputs_signed->value(i)=="s")	port->setSigned(Port::SFIXED);
                        else									port->setSigned(Port::UFIXED);
                    }
                    else
                        /* Fin GL*/
                        if (att_outputs_signed->value(i)=="s")	port->setSigned(Port::SIGNED);
                        else									port->setSigned(Port::UNSIGNED);
                    /* Fin EJ */
                }
                else
                {
                    port->bitwidth(_bits);
                }
            }

				/* GL 20/12/08 : Complex operators */
				vector<long> outputBegins;
				// Arrival time of outputs
				const ParseAttribute *att_output_begins;
				/* GL 03/07/09 : Verification */
				bool ok = false;
				/* Fin GL*/
				if (object->getAttribute("output_begins", &att_output_begins)) {
					for (i = 0; i < att_output_begins->_values.size(); i++)
					/* GL 03/07/09 : Verification 
					Replace line
						outputBegins.push_back(Parser::atol(att_output_begins->value(i)));
					/* By*/
					{
						long time = Parser::atol(att_output_begins->value(i));
						if (time/*/_clk->period()*/ > latency){
							cerr << "Error: operator " << o->name() << " has an output arrival time greater than his latency " << endl;
							exit(1);
						}
						if (!ok)
							ok = (time/*/_clk->period()*/ == latency);
						outputBegins.push_back(time);
					}
					/* Fin GL*/
				}
				else {
					if (!synchronous) latency = (long)ceil((long double)(propagation_time/_clk->period()))+1;
					outputBegins.assign(size, latency);
					/* GL 03/07/09 : Verification */
					ok = true;
					/* Fin GL */
				}
				/* GL 03/07/09 : Verification */
				if(!ok){
					cerr << "Error: operator " << o->name() << " must have at least one output arrival time equal to his latency " << endl;
					exit(1);
				}
				/*Fin GL*/
				o->output_begins(outputBegins);
			
				/* Fin GL*/				
            // free memory
            ports.clear();
            symbolic_ports_no.clear();

				/* GL 22/10/08 :  synchronous without fsm operators */
				if (o->noFSM()){
					// cmd port size
					const ParseAttribute *att_cmd_size;
					if (object->getAttribute("cmd_size", &att_cmd_size))
					{
						o->command_size( Parser::atol(att_cmd_size->_values[0]));
					}
					else  	
					{
						cerr << "Error: Need to specify command port size when defining operator with exernal FSM" <<  endl;
						exit(1);
					}
					// cmd port name
					const ParseAttribute *att_cmd_port;
					string cmd_name;
					if (object->getAttribute("cmd_port", &att_cmd_port))
					{
						cmd_name = att_cmd_port->_values[0];
					}
					else  	// implicit naming: "COMMAND"
					{
						cmd_name = "COMMAND";	// default value
					}
					o->command_name(cmd_name);
			// command code values
			const ParseAttribute *att_cmd_codes;
			if (object->getAttribute("cmd_codes", &att_cmd_codes))
			{
						vector<string> codes;
				for (i = 0; i < att_cmd_codes->_values.size(); i++)
				{
							if (i>= o->cadency()) 
							{
								cerr << "Error: Operator FSM codes are more than its number of cycles" <<  endl;
								exit(1);
							}
							codes.push_back(att_cmd_codes->_values[i]);
						}
						o->addCode(f, codes);
				}
			}
			/* Fin GL */
            // resize control codes vector
            size = att_function->_values.size();
            o->resizeCtrlCodes(size);
            // ctrl code values
            const ParseAttribute *att_ctrl_codes;
            if (object->getAttribute("ctrl_codes", &att_ctrl_codes))
            {
                for (i = 0; i < att_ctrl_codes->_values.size(); i++)
                {
                    o->setCtrlCode(i, Parser::atol(att_ctrl_codes->_values[i]));
                }
            }
            else  	// implicit codes: 0..N-1
            {
                for (i = 0; i < size; i++)
                {
                    o->setCtrlCode(i, i);
                }
            }
            /* GL 25/10/07 : combinatorial delay */
            const ParseAttribute *att_combinational_delay;
            o->resizeCombinationalDelay(size);
            if (object->getAttribute("combinational_delay", &att_combinational_delay))
            {
                for (i = 0; i < size; i++)
                {
                    o->setCombinationalDelay(i, Parser::atol(att_combinational_delay->value(i)));
                }
            }
            else
            {
                for (i = 0; i < size; i++)
                {
                    o->setCombinationalDelay(i, -1);
                }
            }
            /* Fin GL */
            /* GL 20/11/07 : function list */
            o->resizeFunctionList(size);
            for (i = 0; i < size; i++)
            {
                o->setFunctionList(i, att_function->value(i));
            }
            /* Fin GL */
            // add to set of operators
            OperatorRef o_ref(o);
            _operators.add(o_ref);

        }
        else if (object->_type == "bits")
        {
            _bits					= Parser::atoi(object->_name);
        }
        else
        {
            cerr << "Error: unexpected object '" << object->_type << "(" << object->_name << ")'" << endl;
            exit(1);
        }
    }

    // 2
    // build implemented functions list
    if (_bitwidthAware)  _functions = (FunctionRefs*)_addr;
    for (it = objects->begin(); it != objects->end(); it++)
    {
        const ParseObject *object = (*it).second;
        if (object->_type == "operator")
        {
            Operator *o;
            _operators.search(object->_name, &o);
            string name = o->name();
            int i;
            const ParseAttribute *att_propagation_time = object->attribute("propagation_time");
				/* GL 10/06/09 : correction bug temps de release
								 vu avec Philippe et Dominique */
				/* Fin GL */
            const ParseAttribute *att_power            = object->attribute("power");
            const ParseAttribute *att_function         = object->attribute("function");
            for (i = 0; i < att_function->_values.size(); i++)
            {
					/* GL 10/06/09 : correction bug temps de release
									 vu avec Philippe et Dominique 
									 Replace line 
                long propagation_time	= Parser::atol(att_propagation_time->value(i));
					/* By */
					long propagation_time;
					if (o->synchronous())
					{
						const ParseAttribute *att_latency = object->attribute("latency");
						propagation_time = Parser::atol(att_latency->value(i))* clk->period();
					}
					else
						propagation_time = Parser::atol(att_propagation_time->value(i));
					/* Fin GL */
                long power				= Parser::atol(att_power->value(i));
                string function_name;
                if (_bitwidthAware)
                    function_name	= newFunctionName(object, att_function->value(i), propagation_time);
                else
                    /* Fin GL */
                    function_name	= att_function->value(i);

                Function *f;
                if (!_functions->search(function_name, &f))
                {
                    cerr << "Error: unknown function '" << function_name << "' in operator '" << name << "'" << endl;
                    exit(1);
                }
                FunctionRef f_ref(f);
                ImplementedFunction *ifunc = new ImplementedFunction(propagation_time, power, f_ref);
                ImplementedFunctionRef if_ref(ifunc);
                o->functions().add(if_ref);
            }
        }
    }

    // 3
    // post-read values to create

    // build _implemented_by lists
    FunctionRefs::const_iterator f_it;
    for (f_it = _functions->begin(); f_it != _functions->end(); f_it++)
    {
        const FunctionRef f_ref = (*f_it).second;
        Function *f = f_ref;
        for (OperatorRefs::const_iterator o_it = _operators.begin(); o_it != _operators.end(); o_it++)
        {
            OperatorRef o_ref = (*o_it).second;
            Operator *o = o_ref;
            for (ImplementedFunctionRefs::const_iterator if_it = o->functions().begin(); if_it != o->functions().end(); if_it++)
            {
                ImplementedFunction *ifunc = (*if_it);
                if (ifunc->function() == f)
                {
                    /* GL 16/07/07 : operators with the same component must have same propagation time (max of all)*/
                    /* GL 18/09/07 : correction bug add_sub */
                    if (_bitwidthAware /*GL 22/11 : keep the largest for all modes && _clusteringMode!=0 */)
                    {
                        /* Fin GL */
                        bool biggest = false;
                        /* GL 18/09/07 : correction bug multifunction */
                        bool first = true;
                        /* Fin GL */
                        OperatorRefs::iterator f_o_it;
                        for (f_o_it = f->implemented_by().begin() ; f_o_it != f->implemented_by().end(); f_o_it++)
                        {
                            OperatorRef f_o_ref = (*f_o_it).second;
                            Operator *f_o = f_o_ref;
                            if (f_o->component() == o->component())
                            {
                                first = false;

                                // EJ 07/07/2008 : levinson
                                if (f->name() == "mem_read" || f->name() == "mem_write")
                                {
                                    if (f_o->inputsBitwidth()[0] < o->inputsBitwidth()[0])
                                    {
                                        biggest = true;
                                        break;
                                    }
                                }
                                else
                                    // End EJ

                                    if (f_o->sumOfwidths() < o->sumOfwidths())
                                    {
                                        biggest = true;
                                        break;
                                    }
                            }
                        }
                        if (biggest)
                        {
                            if (f->implemented_by().size() != 0) f->implemented_by().erase(f_o_it);
                            f->implemented_by().add(o_ref);
                        }
                        if (first)
                            f->implemented_by().add(o_ref);
                    }
                    else
                        f->implemented_by().add(o_ref);
                    /* Fin GL */
                }
            }
        }
    }

    // 4
    // final checks

    // all functions must have at least one implementation operator
    for (f_it = _functions->begin(); f_it != _functions->end(); f_it++)
    {
        const Function *f = (*f_it).second;
        if ((f->implemented_by().size() == 0) && (_verbose))

        {
            cout << "Warning: function '" << f->name() << "' has no implementation operator" << endl;
            //exit(1);
        }
    }
}
