/*
 * =====================================================================================
 *
 *        Filename:  system.cc
 *     Description:  utilities for better c++ string class handling
 *         Created:  12/05/2005 10:52:53 PM EST
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 199                                          $: Revision of last commit
 *  $Author:: daniel@linux                                   $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>

// on _MSCVER the include of util has to come here, if we put this include at
// the end of the includes, multiple errors occur. Once we learn the reason
// annotate it properly and put it on the TWiki.
#include "util.h"
#include "Environment.h"
using namespace tds;

namespace util {

	using namespace std;

	std::string Util::baseChar(std::string requestedVar) {
		size_t pos = requestedVar.find_first_of("0123456789");
		if(pos < requestedVar.size())
			requestedVar.erase(requestedVar.find_first_of("0123456789"));
		return requestedVar;
	}

	std::string Util::conformName(wedge value) {
		if(value >= 0) {
			return Util::itoa(value);
		} else {
			return string("_")+ Util::itoa(value*-1);
		}
	}

	std::string Util::itoa(wedge n) {
#if 0
		string ret = "";
		unsigned int len, m, d;
		char* p;
		bool bNeg = false;
		if(n == 0) {
			ret = "0";
			return ret;
		}
		if(n < 0) {
			n = -1*n;
			bNeg = true;
		}
		m = n;
		len = 0;
		while(m > 0) {
			m = m/10;
			len ++;
		}
		p = (char*)malloc(sizeof(char)*len+1);
		p[len] = 0;
		for(unsigned int i = 0; i < len; i ++) {
			d = n - n/10*10;
			n = n/10;
			p[len-1-i] = d + '0';
		}
		ret = string(p);
		free(p);
		if(bNeg)ret = string("-")+ ret;
		return ret;
#else
		std::stringstream ss;
		ss << n;
		return ss.str();
#endif
	}

	std::pair<bool, wedge > Util::atoi(const char* a) {
		bool bErr = false;
		bool bNeg = false;
		wedge ret  = 0;
		if(a[0] == '-') {
			bNeg = true;
			a++;
		}
		for(unsigned int i = 0; i < strlen(a); i++) {
			if(a[i] > '9' || a[i] < '0') {
				bErr = true;
				break;
			}
			ret = ret* 10 + a[i] - '0';
		}
		if(bErr) {
			return pair<bool, wedge>(false, 0);
		}
		if(bNeg) {
			ret*= -1;
		}
		return pair<bool, wedge>(true, ret);
	}

	unsigned int Util::readline(ifstream & in, string & line, const char* keyword_comment) {
		line = "";
		size_t start;
		if(!in.is_open()) {
			return 0;
		}
		while(in.good()) {
			getline(in, line);
			line = trim(line);
			start = line.find(keyword_comment);
			if(start != string::npos) {
				line.erase(start, line.size()- start);
			}
			if(line.size()!= 0) {
				break;
			}
		}
		return line.size();
	}


	void Util::showDotFile(const char* tname) {
		string cmd = Environment::getStr("dot_bin");
		if(cmd.empty()|| tname==NULL)
			return;
		cmd += string(" -Tps ")+tname + " -o " + tname + ".ps";
		//cmd += string(" 2>/dev/null");
		if(-1 == Util::launchProc(cmd.c_str(),true))
			return;
		cmd = Environment::getStr("ps_bin");
		cmd += string(" ")+ tname + ".ps";
		//cmd += string(" 2>/dev/null");
		Util::launchProc(cmd.c_str(),false);
	}

	void Util::progressBar(int current, int max, std::string msg) {
		//[0123456789012345678901234567890123456789] 100%
		if(msg.empty())
			msg = "                                         ";
		std::cout << '\xd';
		if(current>=max) {
			std::cout << "[========================================] 100% " << msg;
		} else {
			int upto = (40*current)/max;
			int pad = 40-upto;
			std::cout << "[";
			while(upto-->0) {
				std::cout << "=";
			}
			while(pad-->0) {
				std::cout << " ";
			}
			std::cout << "] ";
			std::cout.width(3);
			std::cout.fill(' ');
			std::cout <<(100*current)/max << "% " << msg;
		}
		std::flush(std::cout);
	}

	std::string Util::getPrefix(const char* str) {
		// search for '/' or '\'
		int slash_pos = -1;
		int i;
		int len = strlen(str);
		for(i = 0; i < len; i++) {
			if((str[i] == '/')||(str[i] == '\\')) {
				slash_pos = i;
			}
		}
		char*res = new char[len+1];		// allocate a new char array
		int pos = 0;
		for(i = slash_pos+1; (i < len) && (str[i] != '.'); i++) {
			res[pos++] = str[i];
		}
		assert(pos<=len);
		res[pos] = 0;
		std::string str_res(res);
		delete[] res;
		return str_res;
	}

	bool Util::fileExist(std::string filename) {
		ifstream file;
		file.open(filename.c_str());
		if (file.is_open()) {
			file.close();
			return true;
		}
		return false;
	}

	bool Util::copyFile(std::string source, std::string destine) {
		ifstream inFile;
		inFile.open(source.c_str());
		if (inFile.is_open()) {
			ofstream outFile;
			outFile.open(destine.c_str());
			if (outFile.is_open()) {
				outFile << inFile.rdbuf();
				outFile.close();
				inFile.close();
				return true;
			}
			inFile.close();
		}
		return false;
	}
}
