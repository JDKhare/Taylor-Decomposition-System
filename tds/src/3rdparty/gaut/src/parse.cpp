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

//	File:		parser.cpp
//	Purpose:	general and simple parser
//	Author:		Pierre Bomel, LESTER, UBS

#include <vector>
#include <set>
#include "parser.h"
using namespace std;

// PARSEOBJECTS section
void ParseObjects::add(ParseObject *o)
{
	string full = Parser::buildFull(o->_type, o->_name);
	pair<Obase::iterator, bool> p = insert(make_pair(full, o));
	if (!p.second)
	{
		cerr << "Error: object '" << full << "' defined twice" << endl;
		exit(1);
	}
}

bool ParseObjects::search(const string &type, const string &name, ParseObject **po)
{
	iterator it = find(Parser::buildFull(type, name));
	if (it == end()) return false;
	*po = (*it).second;
	return true;
}

bool ParseObjects::search(const string &type, const string &name, const ParseObject **po) const
{
	const_iterator it = find(Parser::buildFull(type, name));
	if (it == end()) return false;
	*po = (*it).second;
	return true;
}

// PARSER section

const char   Parser::PARSER_COLON				= ',';
const string Parser::PARSER_COLON_STR			= ",";
const char   Parser::PARSER_COMMENT				= '#';
const char   Parser::PARSER_LEFTPAR				= '(';
const char   Parser::PARSER_RIGHTPAR			= ')';
const char   Parser::PARSER_LEFTACC				= '{';
const char   Parser::PARSER_PLUS				= '+';
const char   Parser::PARSER_RIGHTACC			= '}';
const char   Parser::PARSER_SEMICOLON			= ';';
const string Parser::PARSER_ANY_STR				= "*";
const string Parser::PARSER_LEFTACC_STR			= "{";
const string Parser::PARSER_RIGHTACC_STR		= "}";
const string Parser::PARSER_SEMICOLON_STR		= ";";

// BASIC SYNTAX ANALYSIS

// extract type and short name from full name
void Parser::parseFull(const string &full_name, string &type, string &name, bool &extends, const string &deftype)
{
	type = name = ""; // default values
	char typec[100];
	int typec_pos = 0;
	char namec[100];
	int namec_pos = 0;
	int pos = 0;
	const char *str = full_name.c_str();
	int len = full_name.size();
	extends = false;

	// read type part of full name
	while ((str[pos] != PARSER_LEFTPAR) && (pos < len))
		typec[typec_pos++] = str[pos++];
	pos++; // eat "("
	typec[typec_pos] = 0; // EOS
	// convert to strings
	type = typec;
	if (pos >= len)
	{
		name = type;
		type = deftype;
		return;
	}

	// read id part of full name
	while ((str[pos] != PARSER_RIGHTPAR) && (pos < len))
		namec[namec_pos++] = str[pos++];
	namec[namec_pos] = 0; // EOS
	// convert to strings
	name = namec;

	// add default type if required
	if (type.size() == 0) type = deftype;

	// detect extension, if any
	while (type[0] == PARSER_PLUS)
	{
		extends = true;
		type.erase(0, 1);
	}
}

// search for a char and split text in two parts
void Parser::split(const char c, const string &txt, string &first, string &second)
{
	int len = txt.size();
	int wpos = 0;
	int rpos = 0;
	char first_str[100];
	char second_str[100];
	while ((txt[rpos] != c) && (rpos < len)) first_str[wpos++] = txt[rpos++];
	rpos++; // eat c
	first_str[wpos] = 0; // EOS
	wpos = 0;
	while (rpos < len) second_str[wpos++] = txt[rpos++];
	second_str[wpos] = 0; // EOS
	first = first_str;
	second = second_str;
}

void Parser::parseFull(const string &full_name, string & type, string & name)
{
	bool extends;
	return parseFull(full_name, type, name, extends);
}

// increment line number when parsing files
void Parser::oneMoreLine()
{
	_line++;
	//cout << "one more line" << endl;
}

// tell if char is a blank
bool Parser::isABlank() const
{
	switch (_cur_char)
	{
	case ' ':
	case '\a':
	case '\b':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v':
		return true;
	default:
		return false;
	}
}

// tell if char is a separator
bool Parser::isASeparator() const
{
	return
	    (
	        (_cur_char == PARSER_LEFTACC)  ||
	        (_cur_char == PARSER_RIGHTACC) ||
	        (_cur_char == PARSER_SEMICOLON)||
	        (_cur_char == PARSER_COLON)
	    );
}

// eat blanks
void Parser::eatBlanks(ifstream &f)
{
	do
	{
		if (f.get(_cur_char).fail())
		{
			_cur_char = ' ';
			return;
		}
		if ((_cur_char == '\n') || (_cur_char == '\r') || (_cur_char == '\f')) oneMoreLine();
	}
	while (isABlank());
}

// read a word
bool Parser::getWord(ifstream &f, string &s)
{
	if (_back_word.size() != 0)
	{
		s = _back_word;
		if (_debug_word) cout << "word = '" << s << "'" << endl;
		_back_word = "";
		return true;
	}
	// else normal read
	char word[100];
	if (isASeparator())
	{
		word[0] = _cur_char;
		word[1] = 0;
		eatBlanks(f);
		s = string(word);
		if (_debug_word) cout << "word = '" << s << "'" << endl;
		return true;
	}
	while (_cur_char == PARSER_COMMENT) getEndOfLine(f);
	int pos = 1;
	word[0] = _cur_char;
	do
	{
		if (f.get(_cur_char).fail())
		{
			_cur_char = ' ';
			return false;
		}
		if (_cur_char == '\n') oneMoreLine();
		if (_cur_char == '@') word[pos++] = '_';
		else if (!isABlank() && !isASeparator()) word[pos++] = _cur_char;
	}
	while (!isABlank() && !isASeparator());
	word[pos] = 0; // EOS
	if (!isASeparator()) eatBlanks(f);
	s = string(word);
	if (_debug_word) cout << "word = '" << s << "'" << endl;
	return true;
}

// push back word for next read
void Parser::pushBackWord(const string & word)
{
	if (_debug_word) cout << "back=" << word << endl;
	_back_word = word;
}

// get a string from an input stream
bool Parser::getString(ifstream &f, string &w)
{
	return getWord(f, w);
}

// get a constant string
bool Parser::getConst(ifstream &f, const string &ref)
{
	string w;
	if (!getWord(f, w)) return false;
	return (w == ref);
}

// get a number from an input stream
bool Parser::getLong(ifstream &f, long &l)
{
	string w;
	if (!getWord(f, w)) return false;
	istrstream txt(w.c_str());
	if ((txt>>l).fail()) return false;
	return true;
}

int Parser::atoi(const string &txt)
{
	istrstream is(txt.c_str());
	int n;
	is>>n;
	return n;
}

string Parser::itos(int num)
{
	ostringstream myStream;
	myStream << num << flush;
	return(myStream.str());
}


string Parser::ltos(long num)
{
	ostringstream myStream;
	myStream << num << flush;
	return(myStream.str());
}

long Parser::atol(const string &txt)
{
	istrstream is(txt.c_str());
	long n;
	is>>n;
	return n;
}

// read all characters up to end of line
void Parser::getEndOfLine(ifstream &f)
{
	do
	{
		if (f.get(_cur_char).fail())
		{
			_cur_char = ' ';
			return;
		}
		if (_cur_char == '\n') oneMoreLine();
	}
	while (_cur_char != '\n');
	eatBlanks(f);
}

// ERRORS
// to signal an error
void Parser::parseErrStop(const string &txt) const
{
	cout << "In file: " << _file_name << endl;
	cout << "Parsing error at line " << _line << endl;
	cerr << "Error: " << txt << endl;
	exit(1);
}

// FILE SYSTEM
// to check if a file exists
void Parser::check_file_exists(const string &name)
{
	if (name.size() == 0)
	{
		cerr << "Invalid file name" << name << endl;
		exit(1);
	}
	ifstream f(name.c_str());
	if (!f)
	{
		cerr << "File " << name << " does not exist" << endl;
		exit(1);
	}
}

// to check if a file does not exist
void Parser::check_file_nexists(const string &name)
{
	if (name.size() == 0)
	{
		cerr << "Invalid file name" << name << endl;
		exit(1);
	}
	ifstream f(name.c_str());
	if (f)
	{
		cerr << "File " << name << " already exists" << endl;
		exit(1);
	}
}

// THE PARSER !!!!
void Parser::parse(const vector<string> & file_names, ParseObjects *data)
{
	_objects = data;

	for (int ifile = 0; ifile < file_names.size(); ifile++)
	{
		_file_name = file_names[ifile];
		check_file_exists(_file_name);
		if (_debug) cout << "parsing " << _file_name << " file" << endl;
		ifstream f(_file_name.c_str());
		_line = 1;

		string word;
		eatBlanks(f);	// align on first non blank char
		// process all objects: read object full name 'type(name)'
		while (getString(f,word))
		{
			bool extends_object;
			string obj_type, obj_name, attribute_name, value;
			if (_debug) cout << "read word=" << word << endl;
			// extract type and name from full name
			parseFull(word, obj_type, obj_name, extends_object);
			if (_debug) cout << "type=" << obj_type << " name=" << obj_name << endl;
			// create object
			ParseObject *object;
			if (extends_object)
			{
				if (!_objects->search(obj_type, obj_name, &object))
				{
					parseErrStop("trying to extend an unknown object '"+buildFull(obj_type, obj_name)+"'");
				}
				if (_debug) cout << "extending object '" << object->id() << "'" << endl;
			}
			else
			{
				object = new ParseObject(obj_type, obj_name);
				if (_debug) cout << "creating object " << object->id() << endl;
			}
			// eat "{"
			getString(f, word);
			if (word != PARSER_LEFTACC_STR)   // object without attributes
			{
				pushBackWord(word);
			}
			else while (getString(f, attribute_name))
				{
					if (_debug) cout << "read word=" << attribute_name << endl;
					// detect end of attribute list : "}"
					if (attribute_name == PARSER_RIGHTACC_STR) break;
					if (_debug) cout << " creating attribute '" << attribute_name << "'" << endl;
					// extract type and name from full name
					bool extends_attribute;
					string att_type, att_name;
					parseFull(attribute_name, att_type, att_name, extends_attribute);
					ParseAttribute *att;
					if (extends_attribute)
					{
						if (!object->getAttribute(att_name, &att))
						{
							parseErrStop("attribute '"+att_name+"' unknown in object '"+object->id()+"'");
						}
						if (_debug) cout << "extending attribute '" << att->id() << "' of object '" << object->id() << "'" << endl;
					}
					else
					{
						att = new ParseAttribute(att_name);
						if (_debug) cout << "creating attribute '" << att->id() << "'" << endl;
					}
					if (!getString(f,value)) parseErrStop("bad value");
					if (value != PARSER_SEMICOLON_STR)
					{
						if (_debug) cout << " value '" << value << "'" << endl;
						do
						{
							// this is a value
							att->add(value);
							if (_debug) cout << " adding value'" << value << "'" << endl;
							// read next token: ";" or ","
							getString(f, value);
							if (value == PARSER_COLON_STR)   //another value follows, read next value
							{
								if (!getString(f, value)) parseErrStop("bad value");
							}
							else if (value != PARSER_SEMICOLON_STR) parseErrStop("bad value separator");
						}
						while (value != PARSER_SEMICOLON_STR);
					}
					// add into attribute list of object
					if (_debug) cout << " add attribute into object attribute list" << endl;
					object->_att_list.add(att);
				}
			// add object into parse data base
			if (!extends_object)
			{
				if (_debug) cout << "adding object into data base" << endl;
				_objects->add(object);
			}
		}
	}
}

// To check dynamic behavior
int ParseObject::_created = 0;
int ParseObject::_deleted = 0;
int ParseObjects::_created = 0;
int ParseObjects::_deleted = 0;
int ParseAttribute::_created = 0;
int ParseAttribute::_deleted = 0;
int ParseAttributes::_created = 0;
int ParseAttributes::_deleted = 0;

void dynamicParsingUsage()
{
	cout << ParseObjects::_created    << " object lists created" << endl;
	cout << ParseObject::_created     << " objects created" << endl;
	cout << ParseAttributes::_created << " attribute lists created" << endl;
	cout << ParseAttribute::_created  << " attributess created" << endl;
	cout << ParseObjects::_deleted    << " object lists deleted" << endl;
	cout << ParseObject::_deleted     << " objects deleted" << endl;
	cout << ParseAttributes::_deleted << " attribute lists deleted" << endl;
	cout << ParseAttribute::_deleted  << " attributess deleted" << endl;
}

//	end of: parser.cpp


