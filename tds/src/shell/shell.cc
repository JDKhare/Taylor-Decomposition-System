/*
 * =====================================================================================
 *
 *        Filename:  shell.cc
 *     Description:  Shell class
 *         Created:  12/09/2005 07:13:03 PM EST
 *          Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07), Daniel Gomez-Prado
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 198                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-06-26 23:31:32 -0400 (Tue, 26 Jun 2012)     $: Date of last commit
 * =====================================================================================
 */

#include <fstream>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#ifdef _MSC_VER
#	define sprintf sprintf_s
# pragma warning(disable:4996)
#endif
#include <vector>
using namespace std;

//
// If the following line is uncommented
// the features of shell autocomplete as well as other readline properties will be ignored
//
//#define NO_READLINE


#include "completer.h"

#include "util.h"
using util::Util;

#include "shell.h"
namespace shell {

	Shell::Shell(void) {
		_cmdCounter = 1;
	}

	Shell::Shell(string prompt) {
		_prompt = prompt;
		_cmdCounter = 1;
	}

	Shell::Shell(string welcome, string prompt) {
		_welcome = welcome;
		_prompt = prompt;
		_cmdCounter = 1;
	}

	Shell::~Shell(void) {
		for_each(_tCommands.begin(), _tCommands.end(), DeleteObject());
	}

	/**
	 * @brief setups and starts the main loop of the shell, reading and executing commands, till "quit" or "q" is entered.
	 **/
	void Shell::start(void) {
		string cmd;
		char* line;
		string strline;

		char prompt[255];
#ifndef USE_READLINE
		char lineBuffer[255];
#endif
		map<string, BaseFunctor* >::iterator p;
		int argc;
		char** argv;
		ReturnValue ret;


		argv = (char**) malloc(255);
		assert(argv);

		printf("%s\n", _welcome.c_str());

		initAliasTable("tds.aliases tds.alias aliases.tds alias.tds");

		_lCommands.push_back("quit");
		_lCommands.push_back("exit");
		_lCommands.push_back("help");
		_lCommands.push_back("time");
		_lCommands.push_back("man");
#ifdef USE_READLINE
		setlistof(_lCommands);  /* set the strings to use by the completer*/
		initialize_readline();	/* Bind completer to readline.*/
#endif


		while (1) {
			sprintf(prompt, "%s %02d> ", _prompt.c_str(), _cmdCounter);

#ifndef USE_READLINE
			std::cout << prompt;
			fgets(lineBuffer,255,stdin);
			lineBuffer[strlen(lineBuffer)-1]=0;
			line=lineBuffer;
#else
			line = readline(prompt);
			add_history(line);
#endif

			strline = string(line);;
			if (strline == "q" || !strncmp(strline.c_str(),"quit",4) || \
				strline == "e" || !strncmp(strline.c_str(),"exit",4) ) {
				execCmd("purge");
				break;
			}
			if (!strncmp(strline.c_str(),"#",1)) {
				continue;
			}
			if (strline == "man") {
				cout << "Type \"man command\" for the manual of the command." << endl;
				continue;
			}
			if (!strncmp(strline.c_str(),"man",3)) {
				strline.erase(0,4);
				strline+=" --help";
				ret = execCmd(strline);
				if(  ret != CMD_HELP ) {
					strline.erase(strline.size()-7);
					if(strline == "man") {
						cout << "Type \"man command\" for the manual of the command." << endl;
					} else if(strline == "time") {
						cout << "Type \"time\" to obtain the elapsed time of the last succsessful comand." << endl;
					} else if(strline == "help") {
						cout << "Type \"help\" to obtain the list of available commands." << endl;
					} else if(strline == "quit") {
						cout << "Type \"quit\" to exit the shell." << endl;
					} else if(!strncmp(strline.c_str(),"!",1)) {
						strline.erase(0,1);
						cout << "Pass the command \"" << strline << "\" to the OS for its execution." << endl;
					} else {
						cout << "Info: UNKNOWN command \"" << strline << "\"." << endl;
					}
				}
				continue;
			}
			if (strline == "h" || !strncmp(strline.c_str(),"help",4)) {
				cout.setf(ios_base::left);
				for (map <string, BaseFunctor*>::iterator p = _tCommands.begin(); p != _tCommands.end(); p++) {
					if ((*(p->second)).isVisible()) {
						argv[0] = Util::strsav(p->first.c_str());
						argv[1] = Util::strsav("--brief");
						cout << " - ";
						cout.width(21);
						cout.fill(' ');
						cout << p->first.c_str();
						(*(p->second))(2, argv);
						free (argv[0]);
						free (argv[1]);
					}
				}	
				if (!aliasTable.empty()) {
					cout << " -------- ALIASES" << endl;
					for (map<string,string>::iterator it = aliasTable.begin(); it != aliasTable.end(); it++) {
						cout << " - ";
						cout.width(21);
						cout.fill(' ');
						cout << (*it).first;
						cout << (*it).second << endl;
					}
				}
				cout.unsetf(ios_base::adjustfield);
				cout << " -------- GENERAL" <<endl;
				cout << " - ![bin]\t\tSystem call to execute bin" << endl;
				cout << " - h[elp]\t\tPrint this help" << endl;
				cout << " - e[xit]\t\tExit the shell" << endl;
				cout << " - q[uit]\t\tQuit the shell" << endl;
				cout << " - history\t\tPrints all successfully executed commands and its execution times" << endl;
				cout << " - time\t\t\tShow the elapsed time for the last command executed" << endl;
				cout << " - man\t\t\tPrints the manual of the given command" << endl;
#ifdef USE_READLINE
				cout << " - the shell accepts command completion. i.e. type dec<TAB> for decompose" << endl;
#endif
				continue;
			}
			if (strline == "history") {
				for(int i =0; i < _cmdCounter-1; i++) {
					cout << "Tds[" << i << "] " << _hTimes[i] << "ms \t|\t" << _hCommands[i] << endl;
				}
				continue;
			}
			if (strline == "time") {
				cout << "Instruction [";
				cout.width(3);
				cout.fill('0');
				cout << _cmdCounter-1 << "] took " << _hTimes[_cmdCounter-1] << " ms." << endl;
				continue;
			}
			if (strline.find("!",0) == 0 ) {
				strline.erase(0,1);
				system(strline.c_str());
#if 0
				bool wait = false;
				if (strline.find("&") != string::npos) {
					wait = true;
					strline.erase(strline.find("&"));
				}
				Util::launchProc(strline.c_str(),wait);
#endif
				continue;
			}

			memset(argv, 0, 255);
			splitline(strline, argc, argv);

			if (argc == 0) continue;
			p = _tCommands.find(string(argv[0]));
			if (p != _tCommands.end()) {
				_startClock = clock();
				ret = (*(p->second))(argc, argv);
				if (ret == CMD_OK) {
					_usedClock = clock() - _startClock;
					_hTimes.push_back( (float)_usedClock/CLOCKS_PER_SEC );
					_hCommands.push_back(line);
					_cmdCounter ++;
				}
			} else if (getAlias(string(argv[0])).second) {
				string aliasLine = getAlias(string(argv[0])).first;
				vector<string> vToks = Util::split(aliasLine, ";");
				for (unsigned int i = 0; i < vToks.size(); i++) {
					ret = execCmd(vToks[i]);
					if (ret != CMD_OK) {
						cerr << "Error: Execution of \"" << vToks[i] << "\" in alias command \"" << argv[0] << "\" failed." << endl;
					}
				}
			} else {
				cout << "Info: UNKOWN command \"" << argv[0] << "\", check the spelling." << endl;
			}
		}
		free(argv);
	}

	/**
	 * @brief mechanism to add commands to the shell
	 **/
	void Shell::addCommand(string cmd, BaseFunctor* pFunc ) {
		_tCommands.insert(pair<string, BaseFunctor*>(cmd, pFunc));
		_lCommands.push_back(cmd);
	}


	/**
	 * @brief private function to split a string into argc argv
	 *
	 * @param line the line to split
	 * @param argc number of arguments found
	 * @param argv array of char* with the arguments
	 **/
	void Shell::splitline(string& line, int& argc, char**& argv) {

		bool bLastSpace = true;
		bool bThisSpace;
		argc = 0;
		char* p = (char*) line.c_str();

		for (unsigned int i = 0; i < line.size(); i ++) {
			bThisSpace = (isspace(line[i]) != 0);
			if (bThisSpace && !bLastSpace) {
				line[i] = 0;
			}
			if (!bThisSpace && bLastSpace) {
				argv[argc] = p + i;
				argc++;
			}
			bLastSpace = bThisSpace;
		}
	}

	/**
	 * @brief Execute the passed command
	 *
	 * @param str command line to execute
	 *
	 * @return 0 if the execution of the command succeded, otherwise different than 0.
	 **/
	ReturnValue Shell::execCmd(string str) {
		map<string, BaseFunctor* >::iterator p;
		int argc;
		char** argv;
		ReturnValue  ret = CMD_FATAL;

		argv = (char**) malloc(255);
		assert(argv);

		if (str == "q")
			return CMD_OK;

		memset (argv, 0, 255);
		string line = str + "\n";
		splitline(line, argc, argv);

		if (argc == 0)
			return CMD_OK;

		p = _tCommands.find(string(argv[0]));

		if (p != _tCommands.end()) {
			ret = (*(p->second))(argc, argv);
			if (ret == CMD_OK)
				_cmdCounter ++;
		}
		free(argv);
		return ret;
	}

	//filenames: file1;file2;file3;...
	//filenames: file1 file2 file3 ...
	void  Shell::initAliasTable(string fileNames) {
		ifstream inFile;
		string line;
		string alias, cmd;
		vector <string> vTokens = Util::split(fileNames, "; \n");

		for (vector<string>::iterator p = vTokens.begin(); p != vTokens.end(); p++) {
			inFile.open(p->c_str());
			if (inFile.is_open()) {
				while (!inFile.eof()) {
					getline(inFile, line);
					while (!line.empty() && line[0] == ' ') {
						line.erase(0);
					}
					if (!line.empty() && line[0] == '#') {
						continue;
					}
					if (Util::getToken(line, " ", 0) != "alias") {
						continue;
					}
					alias = Util::getToken(line, " ", 1);
					cmd = line.erase(0, line.find(alias) + alias.size());
					for (unsigned i = 0; i < cmd.size(); i++) {
						if (cmd[i] == '"') {
							cmd[i] = ' ';
						}
					}
					alias = Util::trim(alias);
					cmd   = Util::trim(cmd);
					if (aliasTable.find(alias) == aliasTable.end()) {
						aliasTable.insert(pair<string, string>(alias, cmd));
						_lCommands.push_front(alias);
					}
				}
				inFile.close();
			}
		}
	}

	pair <string, bool> Shell::getAlias(string cmd) {
		map<string, string>::iterator p;
		vector <string> vTokens;
		string alias;
		bool found;
		/*
		   for (p = aliasTable.begin(); p != aliasTable.end(); p++) {
		   printf("%s:%s\n", p->first.c_str(), p->second.c_str());
		   }
		   */
		vTokens = Util::split(cmd, " ");

		if (vTokens.size() == 0) return pair<string, bool>("", false);

		alias = vTokens[0];
		alias = Util::trim(alias);
		if ((p = aliasTable.find(alias)) != aliasTable.end()) {
			alias = p->second;
			found = true;
		} else {
			found = false;
		}

		for (unsigned i = 1; i < vTokens.size(); i++) {
			alias += " " + vTokens[i];
		}
		return  pair <string, bool> (alias, found);
	}

}
