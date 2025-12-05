/*
 * =====================================================================================
 *
 *        Filename:  CompilerMingWin.cc
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

#if defined(_MINGW)

#include "CompilerMingWin.h"

namespace mingwin_header {

	using namespace std;

	int scandir(const char *dir, struct dirent ***namelist, \
				int (*select)(const struct dirent *), \
				int (*compar)(const void*, const void*)) {
		//int (*compar)(const struct dirent **, const struct dirent **))
		DIR *d;
		struct dirent *entry;
		register int i=0;
		size_t entrysize;

		if ((d=opendir(dir)) == NULL)
			return(-1);

		*namelist=NULL;
		while ((entry=readdir(d)) != NULL)
		{
			if (select == NULL || (select != NULL && (*select)(entry)))
			{
				*namelist= (struct dirent **)realloc((void *)(*namelist),
													 (size_t)((i+1)*sizeof(struct dirent *)));
				if (*namelist == NULL) return(-1);
				entrysize=sizeof(struct dirent)-sizeof(entry->d_name)+strlen(entry->d_name)+1;
				(*namelist)[i]= (struct dirent *)malloc(entrysize);
				if ((*namelist)[i] == NULL) return(-1);
				memcpy((*namelist)[i], entry, entrysize);
				i++;
			}
		}
		if (closedir(d)) {return(-1); }
		if (i == 0) {return(-1); }
		if (compar != NULL) {
			qsort((void *)(*namelist), (size_t)i, sizeof(struct dirent *), compar);
		}
		return(i);
	}

	int alphasort(const void *a, const void *b) {
		const struct dirent** a_ = (const struct dirent**) a;
		const struct dirent** b_ = (const struct dirent**) b;
		return(strcmp((*a_)->d_name, (*b_)->d_name));
	}

}
#else

namespace {
	const bool Avoid_empty_translation_unit = true;
}

#endif
