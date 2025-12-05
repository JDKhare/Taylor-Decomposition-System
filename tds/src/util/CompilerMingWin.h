/*
 * =====================================================================================
 *
 *        Filename:  CompilerMingWin.h
 *     Description:
 *         Created:  09/01/2009 11:06:16 PM EST
 *          Author:  Daniel F. Gomez Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __COMPILERMINGWIN_H__
#define __COMPILERMINGWIN_H__

namespace mingwin_header {

	int scandir(const char *dir, struct dirent ***namelist,
				int (*select)(const struct dirent *),
				int (*compar)(const void*, const void*));

	int alphasort(const void *a, const void *b)


}
#endif
