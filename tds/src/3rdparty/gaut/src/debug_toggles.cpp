/*------------------------------------------------------------------------------
  Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

  Cette licence est un accord légal conclu entre vous et l'Université de
  Bretagne Sud(UBS)concernant l'outil GAUT2.

  Cette licence est une licence CeCILL-B dont deux exemplaires(en langue
  française et anglaise)sont joints aux codes sources. Plus d'informations
  concernant cette licence sont accessibles à http://www.cecill.info.

  AUCUNE GARANTIE n'est associée à cette license !

  L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
  de GAUT2 et le distribue en l'état à des fins de coopération scientifique
  seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
  de protection de leurs données qu'il jugeront nécessaires.
  ------------------------------------------------------------------------------
  2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

  This license is a legal agreement between you and the University of
  South Britany(UBS)regarding the GAUT2 software tool.

  This license is a CeCILL-B license. Two exemplaries(one in French and one in
  English)are provided with the source codes. More informations about this
  license are available at http://www.cecill.info.

  NO WARRANTY is provided with this license.

  The University of South Britany does not provide any warranty about
  the usage of GAUT2 and distributes it "as is" for scientific cooperation
  purpose only. All users are greatly advised to provide all the necessary
  protection issues to protect their data.
  ------------------------------------------------------------------------------
  */

#include "cdfg.h"
#include "scheduling.h"
#include "selection.h"
#include "resizing.h"
#include "io_constraints.h"
#include "mapping_constraints.h"
#include "bus.h"
#include "dot_mem.h"
#include "dot_gantt.h"
#include "local_search.h"

bool CDFG::_debug						= true;
bool Selection::_debug					= false;
bool SelectionOutRefs::_debug			= false;
bool Allocation::_debug					= false;
bool Scheduling::_debug					= true;
/* GL 10/04/2007 : add following line for Resizing process
   Only useful with NewFlowMode*/
bool Resizing::_debug					= false;
/* Fin GL*/
/* KT 01/2008*/
bool Solution::_debug					= false;
bool Local_search::_debug				= false;
/* End KT*/
bool IOC::_debug						= false;
bool MEMC::_debug						= false;
bool BusSynthesis::_debug				= true;
bool DotMem::_debug						= false;
bool DotGantt::_debug					= false;




