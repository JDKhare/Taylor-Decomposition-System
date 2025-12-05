/*
 * =====================================================================================
 *
 *        Filename:  CompilerHeader.h
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

#ifndef __COMPILERMSC_H__
#define __COMPILERMSC_H__

#include <vector>
#include <string>
#include <windows.h>

namespace msc_header {

	int scandir(const char* dirname, std::vector<std::string>* namelist, int indent);
	double winFileTimeToSeconds(LPFILETIME ftp);


}
#endif
