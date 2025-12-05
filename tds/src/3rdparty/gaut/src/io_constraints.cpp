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

//	File:		io_constraints.cpp
//	Purpose:	IO constraints
//	Author:		Pierre Bomel, LESTER, UBS

#include <climits>
#include <math.h>
#include "io_constraints.h"
#include "operation.h"
using namespace std;





CDFG* IOC::_cdfg;
Clock* IOC::_clk;
Cadency* IOC::_cadency;

void IOC::asap_shift(CDFG*cdfg, const Clock*clk, const Cadency*cadency, bool scheduled)
{
	// All inputs with no io constraints on the asap field
	// have an asap value of 0 with a regular ASAP().
	// They must be delayed until the first operation needing them.

	_cdfg = cdfg;
	_clk = (Clock*)clk;
	_cadency = (Cadency*)cadency;
	// scan all CDFG nodes
	if(_debug)cout << "IOC::asap_shift scheduled=" << scheduled << endl;
	//for(CDFG::NODES::const_iterator n_it = cdfg->nodes().begin(); n_it != cdfg->nodes().end(); n_it++) {
	for(CDFG::EDGES::const_iterator e_it = cdfg->source()->successors.begin(); e_it != cdfg->source()->successors.end(); e_it++)
	{
		CDFGedge*e =*e_it;						// get current edge
		CDFGnode*n = e->target;					// get current node
		if(n->type()!= CDFGnode::DATA)continue;	// process only DATA nodes
		if(n->asap_locked())continue;				// process only DATA nodes without asap constraints
		const Data*d = (const Data*)n;				// get Data
		set_min_start(n, scheduled);				// set to minimum of operation using the Data
	}
}

void IOC::set_min_start(CDFGnode*n, bool scheduled)
{
	long min = LONG_MAX;
	CDFGnode*succ,*assign_target;
	if(_debug)cout << "set min start:" << n->name()<< endl;
	// search for min start/asap of all successors
	for(vector<CDFGedge*>::iterator e_it = n->successors.begin(); e_it != n->successors.end(); e_it++)
	{
		const CDFGedge*edge =*e_it;			// get edge
		succ = edge->target;					// get node
		const Operation*o = (const Operation*)succ;	// get operation

		/* DH: 22/11/2008 sert a quoi ? et pose problème avec le levinson op2 b(2) =r_m_0_re assign decalé en 130
		// alors que b(2)utilisé en 20 pour écriture mémoire
		// special case when successor is a "="
		if((succ->type() == CDFGnode::OPERATION)&&(o->function()->passThrough()))// "=" operation/operator
		{
		if(_debug)cout << "set min start:" << n->name()<< " op " << o->name()<< " is a '=' with length=" << o->length()<< endl;
		assign_target = (*succ->successors.begin())->target;
		set_min_start(assign_target, scheduled);		// recursively propagate
		if(scheduled)succ->start(assign_target->start()- o->length());
		else			succ->asap(assign_target->asap()- o->length());
		if(_debug)cout << "set min start:" << n->name()<< " succ.min = " << min << endl;
		}*/

		// compute min
		if(scheduled)
		{
			if(succ->start()< min)min = succ->start();
		}
		else
		{
			if(succ->asap()< min)min = succ->asap();
		}
		if(_debug)cout << "set min start:" << n->name()<< " min=" << min << endl;
	}
	// check for a min found
	if(min == LONG_MAX)// no successors, this is a "dead code", should never appen.
	{
		if(_debug)cout << "set min start:" << n->name()<< " dead code" << endl;
		min = scheduled ? _cdfg->latency(): _cdfg->estimated_best_latency();
	}

	// EJ 30/11/2006: Inputs must be delayed until the first cadency too(%cadency).
	Data*d = (Data*)n;
	if(d->type() == Data::INPUT)
	{
		if(min >= _cadency->length())min = _cadency->length()- _clk->period();
		if(_debug)cout << "set min start:" << n->name()<< "(Input)" << endl;
	}

	// compute my min
	min -= n->length();
	// update me
	if(scheduled)
	{
		n->start(min);
		if(_debug)cout << "set min start:" << n->name()<< " new start = " << n->start()<< endl;
	}
	else
	{
		n->asap(min);
		if(_debug)cout << "set min start:" << n->name()<< " new asap = "  << n->asap()<< endl;
	}
}

void IOC::alap_shift(CDFG*cdfg, bool scheduled)
{
	// All outputs with no io constraints on the alap field
	// have an alap value of latency with a regular ALAP().
	// They must be advanced to reach the alap of operation producing them.

	if(_debug)cout << "IOC::alap_shift scheduled=" << scheduled << endl;
	//for(CDFG::NODES::const_iterator n_it = cdfg->nodes().begin(); n_it != cdfg->nodes().end(); n_it++) {
	for(CDFG::EDGES::const_iterator e_it = cdfg->sink()->predecessors.begin(); e_it != cdfg->sink()->predecessors.end(); e_it++)
	{
		CDFGedge*e =*e_it;						// get current edge
		CDFGnode*n = e->source;					// get current node
		if(n->type()!= CDFGnode::DATA)continue;		// process only DATA nodes
		if(n->alap_locked())continue;			// process only DATA nodes without alap constraints
		//const Data*d = n->data();			// get Data
		// dynamic link here
		const Data*d = (Data*)n;				// get Data
		// set to maximum of operation producing the Data(a single one)
		long max = max_end(n, scheduled);		// remember there is a single operation producing the Data
		if(scheduled)
		{
			long start = n->start();
			if(_debug)cout << "IOC::data " << d->name()<< " max=" << max << " start=" << start << endl;
			if(max < start)
			{
				if(_debug)cout << " data " << d->name()<< " start(" << start << ")<- " << max << endl;
				n->start(max);
			}
		}
		else
		{
			long alap = n->alap();
			if(_debug)cout << "IOC::data " << d->name()<< " max=" << max << " alap=" << alap << endl;
			if(max < alap)
			{
				if(_debug)cout << " data " << d->name()<< " alap(" << alap << ")<- " << max << endl;
				n->alap(max);
			}
		}
	}
}

long IOC::max_end(CDFGnode*n, bool scheduled)
{
	long max = 0;
	for(vector<CDFGedge*>::iterator e_it = n->predecessors.begin(); e_it != n->predecessors.end(); e_it++)
	{
		const CDFGedge*edge =*e_it;			// get edge
		CDFGnode*node = edge->source;			// get node
		long time = node->length();
		if(scheduled)time += node->start();
		else				time += node->alap();
		if(time > max)max = time;
	}
	return max;
}

// end of: io_constraints.cpp

