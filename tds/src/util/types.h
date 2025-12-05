/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *    Description:
 *        Created:  04/12/2011 05:58:47 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */
#ifndef __TYPES_H__
#define __TYPES_H__ 1

#if 1
typedef long long wedge;
const wedge wedge_max = 0xFFFFFFFF;
#else
typedef int wedge;
const wedge wedge_max = 0xFFFF;
#endif


#endif


