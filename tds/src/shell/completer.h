/*
 * =====================================================================================
 *
 *        Filename:  completer.cc
 *     Description:  Provides command completion at tds prompt shell in Unix
 *         Created:  12/09/2005 07:13:03 PM EST
 *          Author:  Daniel Gomez-Prado
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __H_COMPLETER
#define __H_COMPLETER

#if defined(__GNUC__) || defined(_UNIX) || !defined(_MSC_VER)
#	define USE_READLINE	ISACTIVE
#else
#	undef	USE_READLINE
#endif

#if defined(NO_READLINE)
#undef USE_READLINE
#endif

#ifdef USE_READLINE

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <list>
#include <string>
using std::string;
using std::list;

#include <readline/readline.h>
#include <readline/history.h>

/**@brief  safely duplicates a string
 * @return a pointer to the new char* where the string was copied
 * @author Daniel Gomez-Prado
 **********************************************************************/
static char* duplicateStr(const char* s) {
	char *r;
	r = (char*) malloc(strlen(s) + 1);
	strcpy(r, s);
	return (r);
}

/**@def oldSchool
 * when undefined the implementation is made using list<string>
 * @author Daniel Gomez-Prado
 **********************************************************************/
static list<string> listOfcommands;
static list<string>::iterator indexend;

/**@brief  Generator function for command completion.
 * @param  text the stream or line buffer to read
 * @param  state let us know whether to start from scratch; state==0 start at the top of the list.
 * @return the next name which partially matches from the command list, NULL if nothing is matched
 * @author Daniel Gomez-Prado
 **********************************************************************/
static char* commandSearch(const char *text, int state) {
	static int length;
	char *name;
	static list<string>::iterator index=listOfcommands.begin();
	if (!state) {
		index = listOfcommands.begin();
		length = strlen(text);
	}
	while(index != indexend) {
		name=duplicateStr((*index).c_str());
		index++;
		if (strncmp(name, text, length) == 0)
			return name;
		else
			free(name);
	}
	return ((char *)NULL);
}

/**@brief  Attempt to complete on the contents of TEXT.
 * @param  text the stream or line buffer to read
 * @param  start shows the begining of the region of text that contains the word to complete
 * @param  end shows the ending of the region of text that contains the word to complete
 * @note   We can use the entire line in case we want to do some simple parsing.
 * @return the array of matches, or NULL if there aren't any.
 * @author Daniel Gomez-Prado
 *
 * @details
 * If this word is at the start of the line, then it is a command to complete.
 * Otherwise it is the name of a file in the current directory.
 **********************************************************************/
static char** commandCompletion (const char* text, int start, int end) {
	end = end; //lint
	char **matches;
	matches = (char **)NULL;
	if (start == 0)
		matches = rl_completion_matches(text, commandSearch);
	return (matches);
}


/**@brief  set the listOfcommands variable used for completion.
 * @param  lCommands the list of commands to use for completation
 * @note   This function is accessible from outside this file.
 * @author Daniel Gomez-Prado
 * @date   06-15-07
 **********************************************************************/
void setlistof(list<string> lCommands) {
	listOfcommands = lCommands;
	indexend = listOfcommands.end();
}

/**@brief  Tell the GNU Readline library how to complete.
 * @note   This function is accessible from outside this file.
 * @author Daniel Gomez-Prado
 * @date   06-15-07
 **********************************************************************/
void initialize_readline() {
	// Allow conditional parsing of the ~/.inputrc file.
	rl_readline_name = "tds";
	// Tell the completer that we want a crack first.
	rl_attempted_completion_function = commandCompletion;
}

#endif
#endif
