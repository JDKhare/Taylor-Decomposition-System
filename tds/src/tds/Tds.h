/*
 * =====================================================================================
 *
 *        Filename:  Tds.h
 *     Description:  Tds class
 *         Created:  04/19/2007 01:56:13 AM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#ifndef __TDS_H__
#define __TDS_H__

#include <string>
#include <list>
#include <set>
#include <vector>
using namespace std;

#include "matrice.h"
#include "Cmatrice.h"

#include "shell.h"
using namespace shell;

namespace ted {
	class TedMan;
	class ATedNode;
	class TedOrderStrategy;
}
using namespace ted;

namespace dfg {
	class DfgMan;
}
using namespace dfg;

namespace network {
	class Netlist;
}
using namespace network;

namespace polyparser {
	class PNode;
}
using namespace polyparser;

/**
 * @namespace tds
 * @brief The Taylor Decomposition System(TDS)interface.
 * @details
 * Serves as a safe-wrapper for the commands at network, poly, ted and dfg
 * packages.
 **/
namespace tds{

	/**
	 * @class Tds
	 * @brief Implements all the commands that will be executed by the shell,
	 *        and has access to all internal data structures.
	 **/
	class Tds {
	private:
		list <Netlist*> _lHistory;
		unsigned int _history_size;
		Shell* _MainShell;
		TedMan* _pTedMan;
		DfgMan* _pDfgMan;
		bool _reconstructNTL;
		bool _manual_purge;

		static Netlist*	_ntlExtracTed;

		Netlist* getCurrentNetlist(void);
		Netlist* getPreviousNetlist(unsigned int);
		template<typename T>
			ATedNode* _ParseTreeToETed_rec(PNode*, bool bChangeConstant);

	public:
		static Netlist* getNtlExtracTed(void) { return _ntlExtracTed; }
		static void setNtlExtracTed(Netlist* ntl) { _ntlExtracTed = ntl; }

		Tds(void);
		~Tds(void);

		TedMan* getTedMan(void);

		void parsePoly(char*, bool intoConstant);
		void buildTransform(Matrice* m);
		void buildTransform(CMatrice* m);
		void printTransform(Matrice* m, vector <string>* vExprs = NULL);
		void printTransform(CMatrice* m, vector <string>* vExprs = NULL);
		void writeTransformToC(Matrice* m, char* file);
		void writeTransformToC(CMatrice* cm, char* file);

		void pushToHistory(Netlist*);
		void clearHistory(void);
		void setHistorySize(unsigned int);
		Netlist* ntl2ted(Netlist*, bool bChangeConstant);
		TedMan* useShifter(void);
		void print_order_report(const char* filename, TedOrderStrategy* orderst);
#if 0
		Netlist* dfg2nl(Netlist* iNl, DfgMan* pDfgMan, bool bChangeConstant);
#endif
		void setShell(Shell*);

		//Commands
		//TED functionality
		ReturnValue CmdParsePoly(unsigned int argc, char** argv);
		ReturnValue CmdBuildTransforms(unsigned int argc, char** argv);
		ReturnValue CmdSub(unsigned int argc, char** argv);
		ReturnValue CmdCompute(unsigned int argc, char** argv);
		//TED Variable
		ReturnValue CmdVars(unsigned int argc, char** argv);
		ReturnValue CmdListVars(unsigned int argc, char** argv);
		ReturnValue CmdLinearize(unsigned int argc, char** argv);
		//TED Ordering
		ReturnValue CmdBblup(unsigned int argc, char** argv);
		ReturnValue CmdBbldown(unsigned int argc, char** argv);
		ReturnValue CmdBottom(unsigned int argc, char** argv);
		ReturnValue CmdTop(unsigned int argc, char** argv);
		ReturnValue CmdReloc(unsigned int argc, char** argv);
		ReturnValue CmdErase(unsigned int argc, char** argv);
		ReturnValue CmdSift(unsigned int argc, char** argv);
		ReturnValue CmdFlip(unsigned int argc, char** argv);
		ReturnValue CmdReorder(unsigned int argc, char** argv);
		ReturnValue CmdCustomReorder(unsigned int argc, char** argv);
		ReturnValue CmdFixOrder(unsigned int argc, char** argv);
		ReturnValue CmdJumpBelow(unsigned int argc, char** argv);
		ReturnValue CmdJumpAbove(unsigned int argc, char** argv);
		ReturnValue CmdExchange(unsigned int argc, char** argv);
		//TED Decomposition, Factorization and SCE
		ReturnValue CmdDecompose(unsigned int argc, char** argv);
		ReturnValue CmdDFactor(unsigned int argc, char** argv);
		ReturnValue CmdCandidate(unsigned int argc, char** argv);
		ReturnValue CmdCost(unsigned int argc, char** argv);
		ReturnValue Cmdscse(unsigned int argc, char** argv);
		ReturnValue Cmdbottomscse(unsigned int argc, char** argv);
		ReturnValue Cmddcse(unsigned int argc, char** argv);
		ReturnValue Cmdlcse(unsigned int argc, char** argv);
		ReturnValue Cmdbottomdcse(unsigned int argc, char** argv);
		ReturnValue Cmdese(unsigned int argc, char** argv);
		// multi database commands
		ReturnValue CmdPrint(unsigned int argc, char** argv);
		ReturnValue CmdShow(unsigned int argc, char** argv);
		ReturnValue CmdPurge(unsigned int argc, char** argv);
		ReturnValue CmdRead(unsigned int argc, char** argv);
		ReturnValue CmdWrite(unsigned int argc, char** argv);
		// TED, DFG, NTL transformations
		ReturnValue CmdNTL2TED(unsigned int argc, char** argv);
		ReturnValue CmdDFG2NTL(unsigned int argc, char** argv);
		ReturnValue CmdDFG2TED(unsigned int argc, char** argv);
		ReturnValue CmdTED2DFG(unsigned int argc, char** argv);
		ReturnValue CmdQuartus(unsigned int argc, char** argv);

		ReturnValue CmdInfo(unsigned int argc, char** argv);
		ReturnValue CmdEval(unsigned int argc, char** argv);
		ReturnValue CmdRetime(unsigned int argc, char** argv);
#if 0
		ReturnValue CmdTED2NTK(unsigned int argc, char** argv);
#endif
		//DFG functionality
#ifdef FUNCTIONS_DEPRECATED
		ReturnValue CmdDfgUnlinearize(unsigned int argc, char** argv);
		ReturnValue CmdShiftreplace(unsigned int argc, char** argv);
#endif
		ReturnValue CmdShifter(unsigned int argc, char** argv);
		ReturnValue CmdBalance(unsigned int argc, char** argv);
		ReturnValue CmdDfgArea(unsigned int argc, char** argv);
		ReturnValue CmdDfgSchedule(unsigned int argc, char** argv);
		ReturnValue CmdPrintNTL(unsigned int argc, char** argv);
		ReturnValue CmdExtract(unsigned int argc, char** argv);
		ReturnValue CmdDfgRemapShifter(unsigned int argc, char** argv);
		ReturnValue CmdOptimize(unsigned int argc, char** argv);
		ReturnValue CmdExplore(unsigned int argc, char** argv);
		ReturnValue CmdEvaluateConstants(unsigned int argc, char** argv);
		ReturnValue CmdDfgRelink(unsigned int argc, char** argv);
		//Environments
		ReturnValue CmdSet(unsigned int argc, char** argv);
		ReturnValue CmdPrintEnv(unsigned int argc, char** argv);
		ReturnValue CmdSetEnv(unsigned int argc, char** argv);
		ReturnValue CmdLoadEnv(unsigned int argc, char** argv);
		ReturnValue CmdSaveEnv(unsigned int argc, char** argv);
		//Hidden Commands
		ReturnValue CmdEcho(unsigned int argc, char** argv);
		ReturnValue CmdRegressionTest(unsigned int argc, char** argv);
		ReturnValue CmdVerify(unsigned int argc, char** argv);

		ReturnValue update_ted_manager(TedMan* pManNew,string errMsg);
	};

}
#endif
