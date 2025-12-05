/*
 * =====================================================================================
 *
 *        Filename:  SystemLinux.cc
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#if !defined(_WIN32)
#define _LINUX
#endif

#if defined(_LINUX)

#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>

#include "util.h"
namespace util {
	using namespace std;

	char* Util::fileSeparator = "/";

	int Util::makeDir(const char *path, int umode) {
		return mkdir(path, umode);
	}

}

#else

namespace {
	const bool Avoid_empty_translation_unit = true;
}

#endif
