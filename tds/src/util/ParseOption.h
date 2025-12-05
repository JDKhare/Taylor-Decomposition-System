/*
 * =====================================================================================
 *
 *        Filename:  ParseOption.h
 *     Description:
 *         Created:  30/08/2009 4:06:16 PM EST
 *          Author:   Daniel Gomez-Prado
 *         Company:	 UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __PARSEOPTION_H__
#define __PARSEOPTION_H__


#include <cassert>

namespace util {

	/**
	 * @param _argumetNumber the number of arguments passed
	 * @param _arugmentList the arguments passed
	 * @param _optionList is a list with valid parameters, |option1,alias1:|option2|option3,alias3|option4:|
	 * @details A valid option has the format optionN,aliasN[:,#]where option is the usually the full
	 *          name of the option, i.e for option "--help" optionN = "-help". The alias is the
	 *          abbreviated form of the option, i.e for option "-h" aliasN = "h". There can be AT MOST
	 *          one alias. If the string option ends with the character ":", it means that the option
	 *          requires an additional input. Any information after ":" and before the next "|" is
	 *          considered the default value of the requested additional input. Examples:
	 *          "-brief|-help,h|-row,r:|-file,f:tmpfile|-verbose:|
	 **/
	class ParseOption {
	private:
		static const unsigned int OPTION_LIST_SIZE = 512;
		static char _optionList[OPTION_LIST_SIZE];
		unsigned int _index;
		unsigned int _argumentNumber;
		char** _argumentList;
		char* _argumentValue;
		char* _toScan;
	public:
		explicit ParseOption(unsigned int argc, char** argv, const char* options);
		~ParseOption(void) {};
		int getOption(void);
		const char* command(void);
		char* getOptionArgument(void);
		void disregardOptionArgument(void);
		void next(void);
		char* current(void);
	};

#ifdef INLINE
#include "ParseOption.inl"
#endif

}
#endif

