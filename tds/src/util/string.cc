/*
 * =====================================================================================
 *
 *        Filename:  string.cc
 *     Description:  utilities for better c++ string class handling
 *         Created:  12/05/2005 10:52:53 PM EST
 *          Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07)
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <sstream>
#include <cassert>
#ifdef _MSC_VER
# define vsprintf vsprintf_s
# pragma warning(disable:4996)
//#	define strncpy strncpy_s
#endif

#include "util.h"
namespace util {

	std::string Util::strprintf(const char * format, ...) {
		char buf[65535];
		memset(buf, 0, 65535);

		va_list ap;
		va_start(ap, format);
		vsprintf(buf, format, ap);

		return std::string(buf);
	}
	std::vector<std::string> Util::split(std::string str, std::string tokens) {
		int begin, end;
		std::vector<std::string> ret;

		while (1) {
			begin = str.find_first_not_of(tokens);
			if (begin == std::string::npos) break;
			end   = str.find_first_of(tokens, begin);

			if (end == std::string::npos) {
				ret.push_back(str.substr(begin, str.size()-begin));
				break;
			}
			ret.push_back(str.substr(begin, end-begin));
			str = str.substr(end, str.size()-(end - begin));
		}

		return ret;
	}

	std::vector<std::string> Util::splitbystr(std::string & str, std::string & substr) {
		int begin, end;
		std::vector<std::string> ret;

		while (1) {
			begin = str.find_first_not_of(substr);
			if (begin == std::string::npos) break;
			end   = str.find(substr, begin);

			if (end == std::string::npos) {
				ret.push_back(str.substr(begin, str.size()-begin));
				break;
			}
			ret.push_back(str.substr(begin, end-begin));
			str = str.substr(end, str.size()-(end - begin));
		}

		return ret;
	}
	std::string Util::getToken(std::string str, std::string tokens, unsigned index) {
		//    int begin, end;
		std::vector<std::string> vTokens;

		vTokens = split(str, tokens);
		if (index >= vTokens.size()) {
			return "";
		}
		return vTokens[index];
	}
	/*
	   std::string Util::trim(std::string str) {
	   unsigned i, j;

	   i = str.find_first_not_of(" \t\r");
	   j = str.find_last_not_of(" \t\r");

	   if (i == std::string::npos || j == std::string::npos || i > j) {
	   return "";
	   }
	   return str.substr(i, j-i+1);
	   } */

	std::string & Util::trim(std::string & str) {
		size_t i;
		if ((i = str.find_first_not_of(" \t\r")) != std::string::npos) {
			str.erase(0, i);
		}
		if ((i = str.find_last_not_of(" \t\r")) != std::string::npos) {
			str.erase(i+1, str.size()-i-1);
		}
		return str;
	}

#define VERILOG_VALID_CHARACTORS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_&"
	std::string Util::vescape(std::string str) {
		if (str.find_first_not_of(VERILOG_VALID_CHARACTORS) != std::string::npos) {
			return ("\\"+str+" ");
		} else if (str[0] == '$' || (str[0]>= '0' && str[0] <='9')) {
			return ("\\"+str+" ");
		}  else {
			return  str;
		}
	}
	std::string Util::replaceFileExtension(std::string fname, std::string ext) {
		unsigned i;
		if ((i = fname.rfind(".")) == std::string::npos) {
			return (fname + "."+ext);
		} else {
			return (fname.substr(0, i)+"."+ext);
		}
	}
	std::string Util::mergeWrap(std::vector<std::string> & vecstr, std::string tok, unsigned length) {
		std::string str;
		std::vector<std::string>::iterator p;
		unsigned size = 0;

		for (p=vecstr.begin(); (p+1) != vecstr.end(); p++) {
			str += (*p)  + tok;
			size += p->size() + tok.size();

			if (size > length) {
				size = 0;
				str += "\n\t";
			}
		}
		return str + (*(vecstr.end()-1));
	}

	std::string & Util::strSubstitute(std::string & s, const char * s1, const char * s2) {
		unsigned i = 0, len = strlen(s1);
		while ((i = s.find(s1)) != std::string::npos) {
			s.replace(i, len, s2);
		}
		return s;
	}

	std::string & Util::strSubstitute(std::string & s, const char * s1) {
		unsigned i = 0, len = strlen(s1);
		while ((i = s.find(s1)) != std::string::npos) {
			s.erase(i, len);
		}
		return s;
	}

	bool Util::isFileExtension(std::string & str, std::string ext) {
		std::vector<std::string> vecstr = split(str, ".");
		if (vecstr.size() == 0) return false;
		if (vecstr.size() == 1  && ext== "") return true;
		if (vecstr[vecstr.size()-1] == ext) return true;
		return false;
	}


	bool Util::isGc(std::string &str) {
		std::vector<std::string> vecstr = split(str, ".");
		if (vecstr[vecstr.size()-1] == "gc") {
			return true;
		} else {
			return false;
		}
	}

	bool Util::isVerilog(std::string &str) {
		std::vector<std::string> vecstr = split(str, ".");
		if (vecstr[vecstr.size()-1] == "v") {
			return true;
		} else {
			return false;
		}
	}

	bool Util::isBlif(std::string &str) {
		std::vector<std::string> vecstr = split(str, ".");
		if (vecstr[vecstr.size()-1] == "blif") {
			return true;
		} else {
			return false;
		}
	}

	void Util::splitCmdLineToArgcArgv(std::string& line, int& argc, char ** &argv) {
		bool bLastSpace = true;
		bool bThisSpace;
		argc = 0;
		char * p = (char *) line.c_str();

		for (unsigned int i = 0; i < line.size(); i ++) {
			bThisSpace = (isspace(line[i])!=0);
			if (bThisSpace == true && bLastSpace == false) {
				line[i] = 0;
			}
			if (bLastSpace == true && bThisSpace == false) {
				argv[argc] = p + i;
				argc++;
			}
			bLastSpace = bThisSpace;
		}
	}

	char * Util::strsav(const char * p) {
		size_t len = strlen(p);
		char * r = (char *) calloc(sizeof(char), len+1);
		//    char * r = (char *) malloc(len+1);
		return strncpy(r, p, len);
	}

}
