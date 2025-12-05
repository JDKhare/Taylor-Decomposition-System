/*
 * =====================================================================================
 *
 *        Filename:  SystemMsc.cc
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#if defined(_MSC_VER)

// on _MSCVER the include of util has to come here, if we put this include at
// the end of the includes, multiple errors occur. Once we learn the reason
// annotate it properly and put it on the TWiki.
#include "util.h"

#include "CompilerMsc.h"
using namespace msc_header;

#include <process.h>
#include <windows.h>
#include <conio.h>
#define sprintf sprintf_s
#pragma warning(disable:4996)

using namespace std;
namespace util {

	char* Util::tempFileName(const char* path, const char* prefix, const char* ext) {
		int  n, index, k;
		const char* filename;
		char buf[200];
		std::vector<std::string> namelist;
		n = scandir(path, &namelist, 0);
		if(n<0) {
			n = makeDir(path, 0775);
			if(n<0 && errno==2)
				return NULL;
			n = 0;
		}
		index = 0;
		for(int i = 0; i < n; i++) {
			filename = namelist[i].c_str();
			if(strncmp(filename, prefix, strlen(prefix))!= 0)continue;
			if((k = strlen(filename)- strlen(ext)-1)< 0)continue;
			if(*(filename + k)!= '.')continue;
			if(strncmp(filename+k+1, ext, strlen(ext))!= 0)continue;
			index++;
		}
		sprintf(buf, "%s%s%s%03d.%s", path, Util::fileSeparator, prefix, index, ext);
		return strsav(buf);
	}

	/* Get the process cpu time.*/
	double Util::getCpuTime(void) {
		FILETIME creationTime, exitTime, kernelTime, userTime;
		GetThreadTimes(GetCurrentThread(), &creationTime, &exitTime, &kernelTime, &userTime);
		double c = winFileTimeToSeconds(&kernelTime)+ winFileTimeToSeconds(&userTime);
		return(c);
	}

	int Util::launchProc(const char* cmd, bool wait) {
		int argc, ret;
		char** argv;
		std::string strcmd;
		argv = (char**)malloc(255);
		memset(argv, 0, 255);
		strcmd = std::string(cmd);
		splitCmdLineToArgcArgv(strcmd, argc, argv);
		if(wait)
			ret = _spawnvp(_P_WAIT,argv[0], argv);
		else
			ret = _spawnvp(_P_NOWAIT,argv[0], argv);
		free(argv);
		return ret;
	}

	void Util::changemode(int dir) { }
	int Util::kbhit(void) { return _kbhit(); }

}

#else

namespace {
	const bool Avoid_empty_translation_unit = true;
}

#endif
