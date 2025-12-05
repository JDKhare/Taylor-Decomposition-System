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

//	File:		os_tools.h
//	Purpose:	operating system dependant basic tools
//				Used to hide OS software interface and ease
//				future support of various OSes.
//	Author:		Pierre Bomel, LESTER, UBS

// forward refs
class Tools;

#ifndef __OS_TOOLS_H__
#define __OS_TOOLS_H__

#include <cstdlib>
#include <cstring>
#include <strstream>
#include <fstream>
#include <iostream>
#include <string>
#include "check.h"
using namespace std;

//! Some OS/lib dependant tools.
class Tools
{
private:
	//! Print an error message and exit.
	static void errStop(const string &header, const string & txt)
	{
		cerr<<header<<txt<<endl;
		exit(1);
	}
public:
	//! Create a new Tools.
	Tools()
	{
#ifdef CHECK
		tools_create++;	// debug
#endif
	}
	//! Delete a Tools.
	~Tools()
	{
#ifdef CHECK
		tools_delete++;	// debug
#endif
	}
	// ERRORS
	//! To signal an internal error(a bug)
	static void internalErrStop(const string &txt)
	{
		errStop("Internal error: ",txt);
	}
	//! To signal a user error(bad specification of something)
	static void userErrStop(const string &txt)
	{
		errStop("User error: ",txt);
	}
	//! To signal an error coming from the library file
	static void libErrStop(const string &txt)
	{
		errStop("Library error: ",txt);
	}
	// to check if a file exists

	// FILE SYSTEM
	//! Check if a file exists. If it does not exist, prints an error message and stops.
	static void check_file_exists(const string &name)
	{
		if(name.size() == 0)
		{
			userErrStop("Invalid file name");
		}
		ifstream f(name.c_str());
		if(!f)
		{
			userErrStop("Unable to open file "+name);
		}
	}
	//! Check if a file does not exist. If it exists, prints an error message and stops.
	static void check_file_does_not_exist(const string &name)
	{
		if(name.size() == 0)
		{
			userErrStop("Invalid file name");
		}
		ifstream f(name.c_str());
		if(f)
		{
			userErrStop("File "+name+" already exist");
		}
	}

	//! Check if a file exist.
	static bool fileExists(const string &name)
	{
		if(name.size() == 0)return false;
		ifstream f(name.c_str());
		return f != 0;
	}
	//! Get prefix out of a file name. Prefix is the part before the dot ".".
	static string prefix(const string &txt)
	{
		// search for '/' or '\'
		const char*str = txt.c_str();			// get pointer to chat array
		int slash_pos = -1;
		int i;
		for(i = 0; i < strlen(str); i++)// scan all characters
		{
			if((str[i] == '/')||(str[i] == '\\'))
			{
				slash_pos = i;
			}
		}
		char*res = new char[strlen(str)];		// allocate a new char array
		int pos = 0;
		for(i = slash_pos+1; i < strlen(str)&& str[i] != '.'; i++)// scan all characters
		{
			res[pos++] = str[i];				// copy char
		}
		res[pos] = 0;							// end of string
		string str_res(res);					// build a new string
		delete [] res;							// free char array space
		return str_res;							// return copy of string
	}

	// BASIC SYNTAX ANALYSIS
	//! Get a string from an input stream
	static bool getString(ifstream &f, string &w)
	{
		return !(f>>w).fail();
	}
	//! Get a constant string
	static bool getConst(ifstream &f, const string &ref)
	{
		string w;
		if((f>>w).fail())return false;
		return(w == ref);
	}
	//! Get a number from an input stream
	static bool getLong(ifstream &f, long &l)
	{
		string w;
		if((f>>w).fail())return false;
		istrstream txt(w.c_str());
		if((txt>>l).fail())return false;
		return true;
	}
	//! Read all up to end of line
	static void getEndOfLine(ifstream &f)
	{
		char c;
		do
		{
			if(f.get(c).fail())return;
		}
		while(c != '\n');
	}

	// TAG files
	//! Tag a file
	static void tag(ofstream &f, const string start, const string name, const string gaut, const string version)
	{
		f << start << "File " << name << " generated by " << gaut << " version " << version << endl;
	}
};


#endif // __OS_TOOLS_H__

//	End of:		os_tools.h
