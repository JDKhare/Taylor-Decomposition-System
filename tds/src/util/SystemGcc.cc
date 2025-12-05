/*
 * =====================================================================================
 *
 *        Filename:  SystemGcc.cc
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#if !defined(_MSC_VER)

#include "util.h"

#if defined(_MINGWIN)&& defined(_WIN32)
#include "CompilerMingWin.h"
using namespace mingwin_header;
#endif

#include <dirent.h>
#include <cstring>
#include <cstdlib>

#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

namespace util {
	using namespace std;

	char* Util::tempFileName(const char* path, const char* prefix, const char* ext) {
		int  n, index, k;
		const char* filename;
		char buf[200];
		struct dirent**namelist;
		n = scandir(path, &namelist, 0, alphasort);
		if(n<0) {
			n = makeDir(path, 0775);
			if(n<0)
				printf("failed to create \"show directory\"... assuming it already exist\n");
			n = 0;
		}
		index = 0;
		for(int i = 0; i < n; i++) {
			filename = namelist[i]->d_name;
			if(strncmp(filename, prefix, strlen(prefix))!= 0)continue;
			if((k = strlen(filename)- strlen(ext)-1)< 0)continue;
			if(*(filename + k)!= '.')continue;
			if(strncmp(filename+k+1, ext, strlen(ext))!= 0)continue;
			index++;
			free(namelist[i]);
		}
		//is this a leak? if we uncommented next line glibd complains about double free and crashes
		//free(namelist);
		sprintf(buf, "%s%s%s%03d.%s", path, Util::fileSeparator ,prefix, index, ext);
		return strsav(buf);
	}

	/* Get the process cpu time.*/
	double Util::getCpuTime(void) {
		/* UNIX CLOCKS_PER_SEC is 1000000, so it wraps after 2100 secs.*/
		double c = ((float)clock())/ CLOCKS_PER_SEC;
		return(c);
	}

	int Util::launchProc(const char* cmd, bool wait) {
		std::string str;
		str = std::string(cmd);
		if(!wait)
			str += string(" &");
		return system(str.c_str());
	}


	void Util::changemode(int dir) {
		static struct termios old_term_att, new_term_att;
		if(dir == 1) {
			int i = tcgetattr(STDIN_FILENO, &old_term_att);
			if (i<0) {
				printf("terminal attribute returned %d for %d\n",i,STDIN_FILENO);
				return;
			}
			new_term_att = old_term_att;
			new_term_att.c_lflag &= ~(ICANON | ECHO);
			tcsetattr(STDIN_FILENO, TCSANOW, &new_term_att);
		}
		else {
			tcsetattr(STDIN_FILENO, TCSANOW, &old_term_att);
		}
	}

	int Util::kbhit(void) {
		struct timeval tv;
		fd_set rdfs;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&rdfs);
		FD_SET(STDIN_FILENO, &rdfs);
		select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
		return FD_ISSET(STDIN_FILENO, &rdfs);
	}
}

#else

namespace {
	const bool Avoid_empty_translation_unit = true;
}

#endif

