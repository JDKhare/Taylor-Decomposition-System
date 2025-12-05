/*
 * =====================================================================================
 *
 *        Filename:  ParseOption.inl
 *     Description:
 *         Created:  30/08/2009 4:06:16 PM EST
 *          Author:   Daniel Gomez-Prado
 *         Company:	 UMASS
 *
 * =====================================================================================
 *  $Revision:: 100                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2009-10-07 22:32:52 -0400 (Wed, 07 Oct 2009)     $: Date of last commit
 * =====================================================================================
 */

#ifndef INLINE
#define inline
#endif

inline
const char* ParseOption::command(void) {
	return _argumentList[0];
}

inline
char* ParseOption::getOptionArgument(void){
	return _argumentValue;
}

inline
void ParseOption::disregardOptionArgument(void){
	--_index;
}

inline
void ParseOption::next(void){
	++_index;
	assert(_index<=_argumentNumber);
}

inline
	char * ParseOption::current(void){
		if(_index<_argumentNumber)
			return _argumentList[_index];
		return NULL;
	}

