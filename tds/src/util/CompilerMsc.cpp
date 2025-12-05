/*
 * =====================================================================================
 *
 *        Filename:  CompilerMsc.cpp
 *     Description:
 *         Created:  08/01/2009 07:14:00 PM EST
 *          Author:  Daniel F. Gomez-Prado
 *         Company:
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#if defined(_MSC_VER)

#include "CompilerMsc.h"

#include <process.h>
#define sprintf sprintf_s
//# define strncpy strncpy_s
//#	define mbstowcs mbstowcs_s
#pragma warning(disable:4996)

using namespace std;
namespace msc_header {

	int msc_header::scandir(const char* dirname, std::vector<std::string>* namelist, int indent) {
		BOOL fFinished;
		HANDLE hList;
		TCHAR szDir[MAX_PATH+1];
		WIN32_FIND_DATA FileData;
		char szDir_char[MAX_PATH+1];
		// Get the proper directory path
		sprintf(szDir_char, "%s\\*", dirname);
		int szDirLength = strlen(szDir_char);
#if !defined(_DEBUG)
		strncpy(szDir,dirname,strlen(dirname));
#else
		TCHAR* pszDir = szDir;
		mbstowcs(pszDir,szDir_char,szDirLength);
		szDir[szDirLength]=NULL;
#endif
		// Get the first file
		hList = FindFirstFile(szDir, &FileData);
		if(hList != INVALID_HANDLE_VALUE) {
			// Traverse through the directory structure
			fFinished = false;
			while(!fFinished) {
				// Check the object is a directory or not
				if(!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
#if !defined(_DEBUG)
					namelist->push_back(FileData.cFileName);
#else
					char filename[MAX_PATH+1];
					//mbtowc(FileData.cFileName,filename,MB_CUR_MAX);
					wcstombs(filename,FileData.cFileName,MAX_PATH); //MB_CUR_MAX);
					filename[MAX_PATH] = '\0';
					namelist->push_back(filename);
#endif
					//printf("%*s%s\n", indent, "", FileData.cFileName);
				}
				if(!FindNextFile(hList, &FileData)) {
					if(GetLastError() == ERROR_NO_MORE_FILES) {
						fFinished = true;
					}
				}
			}
		}
		FindClose(hList);
		int ret = -1;
		if(!namelist->empty())
			ret = namelist->size();
		return ret;
	}

	/** @brief  Converts from FILETIME to number seconds*/
	double msc_header::winFileTimeToSeconds(LPFILETIME ftp) {
		static bool doinit = true;
		static double hscale, lscale;
		if(doinit) {
			doinit = false;
			hscale = 1.0;
			for(int i = 0; i < 32; i++)
				hscale*= 2;
			lscale = 100e-9; /* 100 ns*/
			hscale*= lscale;
		}
		double t = hscale* ftp->dwHighDateTime + lscale* ftp->dwLowDateTime;
		return(t);
	}
}

#else

namespace {
	const bool Avoid_empty_translation_unit = true;
}

#endif
