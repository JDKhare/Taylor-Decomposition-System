/*
 * =====================================================================================
 *
 *        Filename:  Util.h
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 199                                          $: Revision of last commit
 *  $Author:: daniel@linux                                   $: Author of last commit
 *  $Date:: 2010-10-15 11:14:18 -0400(Fri, 15 Oct 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <map>
#include <string>
#include <vector>
#include <set>
#include <fstream>

#ifdef __cplusplus
#define EXTERN extern
#else
#define EXTERN extern "C"
#endif

#define MAX(p, q)((p)>(q) ?(p):(q))
#define MIN(p, q)((p)<(q) ?(p):(q))

#include "types.h"

/**
 * @namespace util
 * @brief utility functions
 **/
namespace util {

	/**
	 * @class Util
	 * @brief string utilities for parsing lines, scaping string sequences.
	 **/
	class Util {
	public:
		static std::string strprintf(const char* format, ...);
		static std::vector<std::string> split(std::string str, std::string tokens);
		static std::vector<std::string> splitbystr(std::string & str, std::string & substr);
		static std::string getToken(std::string str, std::string tokens, unsigned i);
		static std::string & trim(std::string & str);
		static std::string vescape(std::string);
		static std::string replaceFileExtension(std::string fname, std::string ext);
		static std::string mergeWrap(std::vector<std::string> & strs, std::string tok, unsigned length);
		static std::string & strSubstitute(std::string & s, const char* s1, const char* s2);
		static std::string & strSubstitute(std::string & s, const char* s1);
		static void splitCmdLineToArgcArgv(std::string& line, int& argc, char**& argv);

		static bool fileExist(std::string filename);
		static bool copyFile(std::string source, std::string destine);
		static bool isFileExtension(std::string & str, std::string ext);
		static bool isGc(std::string & str);
		static bool isVerilog(std::string & str);
		static bool isBlif(std::string & str);

		static std::string baseChar(std::string);
		static std::string itoa(wedge);
		static std::pair<bool, wedge> atoi(const char*);
		static std::string conformName(wedge value);

		static unsigned int readline(std::ifstream & in, std::string & str,  const char* keyword_comment);
		static char*strsav(const char*);

		template <typename T>
			static T max(T t1, T t2) {
				return(t1>t2) ? t1 : t2;
			}

		static void progressBar(int d, int max, std::string msg = std::string());
		static std::string getPrefix(const char* filename);

		//Compiler dependent
		static char* tempFileName(const char*dirpath, const char*prefix, const char*ext);
		static double getCpuTime(void);
		static int launchProc(const char* cmd, bool wait=false);

		static void changemode(int dir);
		static int kbhit(void);

		//OS dependent
		static char* fileSeparator;
		static int makeDir(const char*,int);
		static void showDotFile(const char* filename);

		typedef std::map<std::string,std::string> SystemEnvironment;
		static SystemEnvironment getEnvironment(void);
	};

}

#endif

