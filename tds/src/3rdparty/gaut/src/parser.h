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

//	File:		parser.h
//	Purpose:	general and VERY simple parser
//	Author:		Pierre Bomel, LESTER, UBS

class Parser;
class ParseAttribute;
class ParseAttributes;
class ParseObject;
class ParseObjects;

#ifndef __PARSER_H__
#define __PARSER_H__

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <strstream>
#include <sstream>
#include <fstream>
#include "check.h"
using namespace std;

//! Parsed attributes.
class ParseAttribute
{
public:
	static int		_created;	//!< to count object lists
	static int		_deleted;	//!< to count object lists
	const string	_name;		//!< attribute name
	vector<string>	_values;	//!< attribute value ordered list
	void*_addr;		//!< to do what you want :-)and extend functionality easily
	//! Create an attribute.
	ParseAttribute(const string & name): _name(name)
	{
		_values.clear();
		_addr = 0;
		_created++;
	}
	//! delete an attribute.
	~ParseAttribute()
	{
		_values.clear();
		_deleted++;
	}
	//! Add a value to an attribute.
	//! @param Input. value is the value to add.
	void add(const string &value)
	{
		_values.push_back(value);
	}
	//! Get attribute unique ID.
	string id()const
	{
		string res = _name + "=";
		int n = _values.size();
		/* DEBUG_DH cout << "id n " << n << endl;*/
		if(n == 1)res += _values[0];
		else
		{
			res += "(";
			for(int i = 0; i < _values.size(); i++)
			{
				if(i)res += ",";
				res+= _values[i];
			}
			res += ")";
		}
		return res;
	}
	//! Print info.
	void info()const
	{
		cout << "  " << id()<< endl;
	}
	//! Get ith value.
	//! @param Input. i is the index value.
	string value(int i)const
	{
		if((i < 0)||(i >= _values.size()))
		{
			cerr << "Error: no value # " << i << " in attribute '" << _name << "'" << endl;
			exit(1);
		}
		return _values[i];
	}
};

//! Set of parsed attributes.
class ParseAttributes : public map<string, ParseAttribute*, less<string> >
{
public:
	static int		_created;	//!< to count object lists
	static int		_deleted;	//!< to count object lists
	//! Create a new set of attributes.
	ParseAttributes()
	{
		_created++;
	}
	//! delete a set of attributes.
	~ParseAttributes()
	{
		for(ParseAttributes::iterator it = begin(); it != end(); it++)delete(*it).second;
		_deleted++;
	}
	//! Search an attribute in a set of attributes.
	//! @param Input. name is the attribute name to search for.
	//! @param Output. pa is a pointer to the attribute found, if any.
	//! @result a boolean, true = attribute found, false otherwise.
	bool search(const string &name, const ParseAttribute**pa)const
	{
		const_iterator it = find(name);
		if(it == end())return false;
		*pa = (*it).second;
		return true;
	}
	//! Add an attribute to a set of attributes.
	//! @param Input. a is a pointer to an attribute.
	void add(ParseAttribute*a)
	{
		pair<iterator, bool> p =
			insert(make_pair(a->_name, a));
		if(!p.second)
		{
			cerr << "Error: attribute '" << a->_name << "' defined twice" << endl;
			exit(1);
		}
	}
	//! Print info.
	void info()const
	{
		for(ParseAttributes::const_iterator it = begin(); it != end(); it++)((*it).second)->info();
	}
};

//! Parsed objects.
class ParseObject  	// object
{
public:
	static int		_created;	//!< to count object lists
	static int		_deleted;	//!< to count object lists
	string			_type;		//!< its type
	string			_name;		//!< its name
	ParseAttributes	_att_list;	//!< its set of attributes
	void*_addr;		//!< to do what you want :-)and extend functionality easily
	//! Create a new parsed object.
	ParseObject(const string &type, const string & name)
	{
		_type = type;
		_name = name;
		_addr = 0;
		_att_list.clear();
		_created++;
	}
	//! Delete a parsed object.
	~ParseObject()
	{
		_deleted++;
	}
	//! Get an attribute 'protected by a "const")from an object.
	//! @param Input. name is the attribute name to search.
	//! @param Input. res is a pointer to an attribute, if found.
	//! @result a booleen, true if attribute is found, false otherwise.
	bool getAttribute(const string &name, const ParseAttribute** res)const
	{
		*res = (ParseAttribute*)0;
		for(ParseAttributes::const_iterator it = _att_list.begin(); it != _att_list.end(); it++)
		{
			const ParseAttribute*a = (*it).second;
			if(a->_name == name)
			{
				*res = a;
				return true;
			}
		}
		return false;
	}
	//! Get an attribute(not protected by a "cosnt")from an object.
	//! @param Input. name is the attribute name to search.
	//! @param Input. res is a pointer to an attribute, if found.
	//! @result a booleen, true if attribute is found, false otherwise.
	bool getAttribute(const string &name, ParseAttribute** res)
	{
		*res = (ParseAttribute*)0;
		for(ParseAttributes::iterator it = _att_list.begin(); it != _att_list.end(); it++)
		{
			ParseAttribute*a = (*it).second;
			if(a->_name == name)
			{
				*res = a;
				return true;
			}
		}
		return false;
	}
	//! Get an attribute 'protected by a "const")from an object.
	//! @param Input. name is the attribute name to search.
	//! @result a pointer to an attribute if found, prints an error and exits otherwise.
	const ParseAttribute* attribute(const string &name)const
	{
		const ParseAttribute*a;
		if(!getAttribute(name, &a))
		{
			cerr << "Error: missing attribute '" << name << "' in '" << _type << "(" << _name << ")'" << endl;
			exit(1);
		}
		return a;
	}
	//! Get unique ID
	string id()const
	{
		return _type+"("+_name+")";
	}
	//! Print info.
	void info()const
	{
		cout << id()<< endl;
		_att_list.info();
	}
};

//! Set of parsed objects.
class ParseObjects : public map<string, ParseObject*, less<string> >  	// set of parsed objects
{
public:
	static int		_created;	//!< to count object lists
	static int		_deleted;	//!< to count object lists
	//! Create a new set of parsed objects.
	ParseObjects()
	{
		_created++;
	}
	typedef map<string, ParseObject*, less<string> > Obase;	//!< local type for maps of parsed objects.
	//! Print info.
	void info()const
	{
		for(Obase::const_iterator it = begin(); it != end(); it++)(*it).second->info();
	}
	//! Find a parsed object(type,name)in a parsed objects list.
	//! The object will not be protected by a "const".
	//! @param Input. type string.
	//! @param Input. name string.
	//! @param Output. po is a pointer to the parsed object if found.
	//! @result a boolean, true if object found, false otherwise.
	bool search(const string &type, const string &name, ParseObject**po);
	//! Find a parsed object(type,name)in a parsed objects list.
	//! The object will be protected by a "const".
	//! @param Input. type string.
	//! @param Input. name string.
	//! @param Output. po is a pointer to the parsed object if found.
	//! @result a boolean, true if object found, false otherwise.
	bool search(const string &type, const string &name, const ParseObject**po)const;
	//! Delete set of parsed objects.
	~ParseObjects()
	{
		for(Obase::const_iterator it = begin(); it != end(); it++)delete(*it).second;
		_deleted++;
	}
	//! Add a parsed object.
	void add(ParseObject*o);
	//! get number of objects.
	int count(const string &type)const
	{
		int n;
		if(type != "")
		{
			n = 0;
			for(ParseObjects::Obase::const_iterator it = begin(); it != end(); it++)
			{
				if(((*it).second)->_type == type)n++;
			}
		}
		else
		{
			n = size();
		}
		return n;
	}
};

//! A generic parser of the(object, attribute, value)syntax.
class Parser  	// meta-model driven Parser(2 stages parsing)
{
private:
	string					_file_name;		//!< current file being parsed
	ParseObjects*_spec;			//!< input = parsing rules
	ParseObjects*_objects;		//!< output = list of parsed objects
	int						_line;			//!< line position while parsing
	char					_cur_char;		//!< current read char from file to parse
	bool					_debug;			//!< debug toggle
	bool					_debug_word;	//!< debug toggle for word reading
	string					_back_word;		//!< for pushBackWord()
public:
	//! Create a parser.
	Parser()
	{
		_debug = _debug_word = false;
		_back_word = "";
#ifdef CHECK
		parser_create++;
#endif
	}
	//! delete a parser.
	~Parser()
	{
#ifdef CHECK
		parser_delete++;
#endif
	}
	//! Parse files.
	//! @param Input. file_names is a vector of file names to parse in order.
	//! @param Output. data is the set of parsed objects.
	void parse(const vector<string> & file_names, ParseObjects*data);
	//! Print info.
	void info()const
	{
		_objects->info();
	}
	//! Build a full name from a type and a name.
	static string buildFull(const string &type, const string &name)
	{
		return type+"("+name+")";
	}
	//! Get type and name parts of a "full name"
	static void parseFull(const string &full_name, string &type, string &name, bool &extends, const string &deftype = "");
	//! Get type and name parts of a "full name"
	static void parseFull(const string &full_name, string &type, string &name);
	//! atoi()like
	static int atoi(const string &txt);
	//! From integer to string.
	static string itos(int num);
	//! From long to string.
	static string ltos(long num);
	//! atol()like
	static long atol(const string &txt);
	//! Split a string in two parts.
	//! @param Input. c is the splitting char.
	//! @param Input. txt is the string to split.
	//! @param Output. first is the first part of the string.
	//! @param Output. second is the second part of the string if a "c" has been found, or "" otherwise.
	static void split(const char c, const string &txt, string &first, string &second);
	// FILE SYSTEM
	//! Check if a file exists
	static void check_file_exists(const string &name);
	//! Check if a file does not exist
	static void check_file_nexists(const string &name);

private:
	// some private constants
	static const string	PARSER_ANY_STR;
	static const string PARSER_COLON_STR;
	static const char   PARSER_COLON;
	static const char   PARSER_COMMENT;
	static const string	PARSER_LEFTACC_STR;
	static const char   PARSER_LEFTPAR;
	static const char   PARSER_RIGHTPAR;
	static const char	PARSER_LEFTACC;
	static const char	PARSER_PLUS;
	static const char	PARSER_RIGHTACC;
	static const string	PARSER_RIGHTACC_STR;
	static const char	PARSER_SEMICOLON;
	static const string	PARSER_SEMICOLON_STR;

	// BASIC SYNTAX ANALYSIS
	//! Extract type and name from full name
	void oneMoreLine();
	//! Tell if char is a blank
	bool isABlank()const;
	//! Tell if char is a separator
	bool isASeparator()const;
	//! Eat blanks
	void eatBlanks(ifstream &f);
	//! Read a word
	bool getWord(ifstream &f, string &s);
	//! Push back a word
	void pushBackWord(const string &word);
	//! Get a string from an input stream
	bool getString(ifstream &f, string &w);
	//! Get a constant string
	bool getConst(ifstream &f, const string &ref);
	//! Get a number from an input stream
	bool getLong(ifstream &f, long &l);
	//! Read all characters up to end of line
	void getEndOfLine(ifstream &f);
	// ERRORS
	//! To signal an error
	void parseErrStop(const string &txt)const;
};

//! To check dynamic behavior
void dynamicParsingUsage();

#endif // __PARSER_H__

//  end of: parser.h

