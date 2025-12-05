/*
 * =====================================================================================
 *
 *        Filename:  ParseOption.cc
 *     Description:
 *         Created:  30/08/2009 4:06:16 PM EST
 *          Author:   Daniel Gomez-Prado
 *         Company:	 UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-12-10 00:38:18 -0500(Thu, 10 Dec 2009)$: Date of last commit
 * =====================================================================================
 */

#include <iostream>
#include <cstring>

#include "ParseOption.h"
#include "util.h"
namespace util {

#ifndef INLINE
#include "ParseOption.inl"
#endif

#ifdef _MSC_VER
# pragma warning(disable:4996)
#endif
	using namespace std;

	char ParseOption::_optionList[ParseOption::OPTION_LIST_SIZE];

	ParseOption::ParseOption(unsigned int argc, char** argv, const char* options):
		_index(0), _argumentNumber(argc), _argumentList(argv), _argumentValue(NULL), _toScan(NULL) {
			size_t optionLenght = strlen(options);
			if(optionLenght>OPTION_LIST_SIZE) {
				throw(std::string("05001. overflow in the option list"));
			}
			strcpy(_optionList,options);
		}

	/**@brief retrieves any [-option] passed with the command to execute
	 * @return  scanned option is returned as an integer
	 * @warning optind is incremented each time
	 **/
	int ParseOption::getOption(void) {
#ifdef _MSC_VER
		char optionList[OPTION_LIST_SIZE];
#else
		char optionList[strlen(_optionList)];
#endif
		strcpy(optionList,_optionList);
		register int result = 0;
		register bool optionFound = false;
		register bool requireValue = false;
		char* defaultValue = NULL;
		_argumentValue = NULL;
		_toScan = NULL;
		if(_index == 0)
			_index++;
		if(_index >= _argumentNumber)
			return EOF;
		_toScan = _argumentList[_index];
		if(_toScan[0] != '-' || _toScan[1] == '\0')
			return EOF;
		_index++;
		if(_toScan[1] == '-' && _toScan[2] == '\0')
			return EOF;
		_toScan++;
		register unsigned int scanUpTo = strcspn(_toScan,"=");
		char* scanning = new char[scanUpTo+1];
		strncpy(scanning,_toScan,scanUpTo);
		scanning[scanUpTo]='\0';
		register char* currentOption = strtok(optionList, "|");
		while(currentOption != NULL) {
			requireValue=false;
			register unsigned int pos = strcspn(currentOption,":");
			//retrieve the default value, if any.
			if(pos != strlen(currentOption)) {
				requireValue=true;
				char* ptr = currentOption;
				ptr+= (pos+1);
				currentOption[pos]='\0';
				if(*ptr!='\0') {
					defaultValue = Util::strsav(ptr);
				}
			}
			pos = strcspn(currentOption,",");
			while (currentOption) {
				if(!strncmp(currentOption,scanning,pos)) {
					currentOption = NULL;
					optionFound = true;
					break;
				}
				if (pos==strlen(currentOption)) {
					break;
				}
				currentOption+= (pos+1);
				pos = strcspn(currentOption,",");
			}
			if (optionFound) {
				break;
			}
#if 0
			//if current option has 2 calling forms, i.e --help,-h, try the first form
			if(pos != strlen(currentOption)) {
				if(!strncmp(currentOption,scanning,pos)) {
					currentOption=NULL;
					optionFound =  true;
					break;
				}
				currentOption+= (pos+1);
			}
			//if current option has only 1 calling form, or 2 forms but it wasn't the --help, now try the -h
			if(!strcmp(currentOption,scanning)) {
				currentOption=NULL;
				optionFound = true;
				break;
			}
#endif
			//the scanning argument is not equal to the current option, retrieve the next option.
			++result;
			currentOption = strtok(NULL,"|");
		}
		if(!optionFound) {
			cerr << "Error: 05002. UNKOWN option \"-" << scanning << "\" in command " << _argumentList[0] << endl << endl;
			result = '?';
		} else {
			if(requireValue) {
				register char* scanValue = _toScan+scanUpTo;
				if(scanUpTo != strlen(_toScan)&&(*++scanValue!='\0')) {
					//is this dead-code?, is there any case that exercise this path?
					_argumentValue = scanValue;
				} else {
					if(_index >= _argumentNumber && !defaultValue) {
						cerr << "Error: 05003. " << _argumentList[0] << " option \"-" << scanning << "\" requires an argument " << endl << endl;
						result = '?';
					} else if(_index < _argumentNumber) {
						_argumentValue = _argumentList[_index];
						_index++;
					} else {
						_argumentValue = defaultValue;
					}
				}
			}
		}
		delete[] scanning;
		return result;
	}

}
