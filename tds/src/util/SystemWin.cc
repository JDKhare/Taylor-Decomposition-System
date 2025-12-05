/*
 * =====================================================================================
 *
 *        Filename:  SystemWin.cc
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 200                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2012-07-02 14:28:27 -0400 (Mon, 02 Jul 2012)     $: Date of last commit
 * =====================================================================================
 */

#if defined(_WIN32)

// on _MSCVER the include of util has to come here, if we put this include at
// the end of the includes, multiple errors occur. Once we learn the reason
// annotate it properly and put it on the TWiki.
#include "util.h"

//#include <iostream>
#include <windows.h>
namespace util {

#include <direct.h>
#pragma warning(disable:4996)
#define mkdir _mkdir

	using namespace std;

	char* Util::fileSeparator = "\\";

	int Util::makeDir(const char *path, int umode) {
		(void)sizeof(umode);
		return mkdir(path);
	}

	Util::SystemEnvironment Util::getEnvironment(void) {
		SystemEnvironment env;
		LPTCH env_block = GetEnvironmentStrings();
		for (LPTCH i = env_block; *i != '\0'; ++i) {
			std::string key;
			std::string value;
			for (; *i != '='; ++i) {
				key += *i;
			}
			++i;
			for (; *i != '\0'; ++i) {
				value += *i;
			}
			if (key.compare("Path")==0 || key.compare("path")==0) {
				env["PATH"] = value;
			} else {
				env[key] = value;
			}
			//std::cout << "env[" << key << "] = " << value << std::endl;
		}
		return env;
	}

}
#else

namespace {
	const bool Avoid_empty_translation_unit = true;
}

#endif

