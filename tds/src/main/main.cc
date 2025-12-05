/*
 * =====================================================================================
 *
 *        Filename:  main.cc
 *     Description:
 *         Created:  04/17/2007 12:45:43 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer), qren@ren-chao.com
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 206                                          $: Revision of last commit
 *  $Author:: daniel@win                                     $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#include <ctime>
#include <iostream>

#include "shell.h"
#include "Tds.h"
#include "ParseOption.h"
#include "util.h"

#ifdef _MSC_VER
# define _WIN32_WINNT 0x0500
# include <windows.h>
# include <WinDef.h>
# include <conio.h>
# define sprintf sprintf_s
#endif

using namespace std;
using namespace util;
using namespace shell;
using namespace tds;

int main(int argc, char** argv) {
#ifdef _MSC_VER
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console,&r);
	MoveWindow(console,r.left,r.top,1400,700,true);
#endif
	int c;
	Tds gTds;
	gTds.setHistorySize(10);
	string strHello;
	char buf[200];
	char* filename,* commandline;
	ifstream* in;
	string line;
	vector <string> vToks;

	filename = NULL;
	commandline = NULL;

#ifdef CURDATE
	time_t ctime = CURDATE;
	struct tm* ctm = gmtime(&ctime);
	sprintf(buf, "Taylor Decomposition System\nCompiled at: %02d:%02d(GMT), %02d-%02d-%04d", ctm->tm_hour, ctm->tm_min, ctm->tm_mon+1, ctm->tm_mday, ctm->tm_year + 1900);
#else
	sprintf(buf, "Taylor Decomposition System\nCompiled at: %s(GMT), %s", __TIME__, __DATE__);
#endif

	Shell mainShell(buf, "Tds");
	gTds.setShell(&mainShell);

	//
	// CONSTRUCTION & MANIPULATION
	//
	mainShell.addCommand("vars", new ShellFunctor<Tds>(&gTds, &Tds::CmdVars));
	mainShell.addCommand("poly", new ShellFunctor<Tds>(&gTds, &Tds::CmdParsePoly));
	mainShell.addCommand("read", new ShellFunctor<Tds>(&gTds, &Tds::CmdRead));
	mainShell.addCommand("write",	new ShellFunctor<Tds>(&gTds, &Tds::CmdWrite));
	mainShell.addCommand("purge", new ShellFunctor<Tds>(&gTds, &Tds::CmdPurge));
	mainShell.addCommand("tr", new ShellFunctor<Tds>(&gTds, &Tds::CmdBuildTransforms));
	mainShell.addCommand("extract",	new ShellFunctor<Tds>(&gTds, &Tds::CmdExtract));
	mainShell.addCommand("erase", new ShellFunctor<Tds>(&gTds, &Tds::CmdErase));
	//mainShell.addCommand("ted2ntk", new ShellFunctor<Tds>(&gTds, &Tds::CmdTED2NTK));
	mainShell.addCommand("dfg2ntl",	new ShellFunctor<Tds>(&gTds, &Tds::CmdDFG2NTL));
	mainShell.addCommand("ntl2ted", new ShellFunctor<Tds>(&gTds, &Tds::CmdNTL2TED));
	mainShell.addCommand("dfg2ted",	new ShellFunctor<Tds>(&gTds, &Tds::CmdDFG2TED));
	mainShell.addCommand("ted2dfg",	new ShellFunctor<Tds>(&gTds, &Tds::CmdTED2DFG));
	//
	// TED ORDER
	//
	mainShell.addCommand("bbldown",	new ShellFunctor<Tds>(&gTds, &Tds::CmdBbldown));
	mainShell.addCommand("bblup",	new ShellFunctor<Tds>(&gTds, &Tds::CmdBblup));
	mainShell.addCommand("bottom",new ShellFunctor<Tds>(&gTds, &Tds::CmdBottom));
	mainShell.addCommand("exchange",	new ShellFunctor<Tds>(&gTds, &Tds::CmdExchange));
	mainShell.addCommand("fixorder", new ShellFunctor<Tds>(&gTds, &Tds::CmdFixOrder));
	mainShell.addCommand("flip", new ShellFunctor<Tds>(&gTds, &Tds::CmdFlip));
	mainShell.addCommand("jumpAbove",	new ShellFunctor<Tds>(&gTds, &Tds::CmdJumpAbove));
	mainShell.addCommand("jumpBelow",	new ShellFunctor<Tds>(&gTds, &Tds::CmdJumpBelow));
	mainShell.addCommand("reloc",	new ShellFunctor<Tds>(&gTds, &Tds::CmdReloc));
	mainShell.addCommand("reorder",	new ShellFunctor<Tds>(&gTds, &Tds::CmdReorder));
	mainShell.addCommand("reorder*",	new ShellFunctor<Tds>(&gTds, &Tds::CmdCustomReorder));
	mainShell.addCommand("sift", new ShellFunctor<Tds>(&gTds, &Tds::CmdSift));
	mainShell.addCommand("top",	new ShellFunctor<Tds>(&gTds, &Tds::CmdTop));
	//
	// TED FACTORIZATION & DECOMPOSITON
	//
	mainShell.addCommand("decompose", new ShellFunctor<Tds>(&gTds, &Tds::CmdDecompose));
	mainShell.addCommand("candidate",	new ShellFunctor<Tds>(&gTds, &Tds::CmdCandidate));
	mainShell.addCommand("scse", new ShellFunctor<Tds>(&gTds, &Tds::Cmdscse));
	mainShell.addCommand("bottomscse", new ShellFunctor<Tds>(&gTds, &Tds::Cmdbottomscse));
	mainShell.addCommand("dcse", new ShellFunctor<Tds>(&gTds, &Tds::Cmddcse));
	mainShell.addCommand("lcse", new ShellFunctor<Tds>(&gTds, &Tds::Cmdlcse));
	mainShell.addCommand("bottomdcse", new ShellFunctor<Tds>(&gTds, &Tds::Cmdbottomdcse));
	mainShell.addCommand("sub", new ShellFunctor<Tds>(&gTds, &Tds::CmdSub));
	mainShell.addCommand("dfactor",	new ShellFunctor<Tds>(&gTds, &Tds::CmdDFactor));
	//
	// TED OPERATIONS
	//
	mainShell.addCommand("shifter",	new ShellFunctor<Tds>(&gTds, &Tds::CmdShifter));
	mainShell.addCommand("linearize",	new ShellFunctor<Tds>(&gTds, &Tds::CmdLinearize));
	//mainShell.addCommand("unlinearizedfg", new ShellFunctor<Tds>(&gTds, &Tds::CmdDfgUnlinearize));
	mainShell.addCommand("set", new ShellFunctor<Tds>(&gTds, &Tds::CmdSet));
	mainShell.addCommand("compute", new ShellFunctor<Tds>(&gTds, &Tds::CmdCompute));
	mainShell.addCommand("optimize", new ShellFunctor<Tds>(&gTds, &Tds::CmdOptimize));
	mainShell.addCommand("explore", new ShellFunctor<Tds>(&gTds, &Tds::CmdExplore));
	mainShell.addCommand("retime", new ShellFunctor<Tds>(&gTds, &Tds::CmdRetime));
	mainShell.addCommand("eval", new ShellFunctor<Tds>(&gTds, &Tds::CmdEval));
	//
	// TED, DFG & NTL INFO & VISUALIZATION
	//
	mainShell.addCommand("cost",	new ShellFunctor<Tds>(&gTds, &Tds::CmdCost));
	mainShell.addCommand("show", new ShellFunctor<Tds>(&gTds, &Tds::CmdShow));
	mainShell.addCommand("listvars", new ShellFunctor<Tds>(&gTds, &Tds::CmdListVars));
	mainShell.addCommand("print",	new ShellFunctor<Tds>(&gTds, &Tds::CmdPrint));
	mainShell.addCommand("printntl", new ShellFunctor<Tds>(&gTds, &Tds::CmdPrintNTL));
	mainShell.addCommand("info",	new ShellFunctor<Tds>(&gTds, &Tds::CmdInfo));
	//
	// DFG & NTL OPERATIONS
	//
	mainShell.addCommand("remapshift", new ShellFunctor<Tds>(&gTds, &Tds::CmdDfgRemapShifter));
	mainShell.addCommand("balance", new ShellFunctor<Tds>(&gTds, &Tds::CmdBalance));
	mainShell.addCommand("dfgarea",	new ShellFunctor<Tds>(&gTds, &Tds::CmdDfgArea));
	mainShell.addCommand("dfgschedule", new ShellFunctor<Tds>(&gTds, &Tds::CmdDfgSchedule));
	mainShell.addCommand("dfgflatten", new ShellFunctor<Tds>(&gTds, &Tds::CmdDfgRelink));
	//mainShell.addCommand("balancenl", new ShellFunctor<Tds>(&gTds, &Tds::CmdBalanceNL));
	//mainShell.addCommand("extractnl", new ShellFunctor<Tds>(&gTds, &Tds::CmdNLExtract));
	mainShell.addCommand("dfgevalconst", new ShellFunctor<Tds>(&gTds, &Tds::CmdEvaluateConstants));
	mainShell.addCommand("quartus", new ShellFunctor<Tds>(&gTds, &Tds::CmdQuartus));
	//
	// ENVIRONMET
	//
	mainShell.addCommand("printenv", new ShellFunctor<Tds>(&gTds, &Tds::CmdPrintEnv));
	mainShell.addCommand("setenv", new ShellFunctor<Tds>(&gTds, &Tds::CmdSetEnv));
	mainShell.addCommand("load", new ShellFunctor<Tds>(&gTds, &Tds::CmdLoadEnv));
	mainShell.addCommand("save", new ShellFunctor<Tds>(&gTds, &Tds::CmdSaveEnv));
	//
	// OTHERS
	//
	mainShell.addCommand("echo", new ShellFunctor<Tds>(&gTds, &Tds::CmdEcho, ShellFunctor<Tds>::privateCmd));
	mainShell.addCommand("regressionTest", new ShellFunctor<Tds>(&gTds, &Tds::CmdRegressionTest, ShellFunctor<Tds>::privateCmd));
	mainShell.addCommand("verify", new ShellFunctor<Tds>(&gTds, &Tds::CmdVerify));

	//mainShell.addCommand("underTest", new ShellFunctor<Tds>(&gTds, &Tds::CmdUnderTesting, ShellFunctor<Tds>::privateCmd));

	ParseOption opt(argc,argv,"-help,h|-file,f:|-command,c:");
	enum switchcase {case_help,case_file,case_command};
	while((c=opt.getOption())!= EOF) {
		switch(c) {
		case case_file:
			filename = opt.getOptionArgument();
			break;
		case case_command:
			commandline = opt.getOptionArgument();
			break;
		case case_help:
		default:
			cout << "NAME" << endl;
			cout << "\t[T]aylor [D]ecomposition [S]ystem - tds" << endl;
			cout << "SYNOPSIS" << endl;
			cout << "\ttds [options]" << endl;
			cout << "\tIf no option is given, enters to the TDS shell." << endl;
			cout << "OPTIONS" << endl;
			cout << "\t-f,--file filename" << endl;
			cout << "\t\truns a script with no user interaction." << endl;
			cout << "\t-c,--command \"cmd1; cmd2; ...; cmdn\"" << endl;
			cout << "\t\truns a list of commands(semicolon separated)with no user interaction." << endl;
			return 1;
		}
	}

	if(filename != NULL) {
		in = new ifstream(filename);
		if(!in->is_open()) {
			delete in;
			cerr << "Error: Can't read script file " << filename << endl;
			return 2;
		}
		int lineNumber = 0;
		clock_t	startCmd = 0;
		clock_t finishCmd = 0;
		while(Util::readline(*in, line, "#")!= 0) {
			lineNumber++;
			startCmd = clock();
			ReturnValue ret = mainShell.execCmd(line);
			finishCmd += (clock()- startCmd);
			if(ret != CMD_OK) {
				cerr << "Error: Aborting script, can't understand script line " << lineNumber << endl;
				cerr << "       \"" << line << "\"" << endl;
				delete in;
				return 2;
			}
		}
		cout <<(float)finishCmd/CLOCKS_PER_SEC << " ms" << endl;
		in->close();
		delete in;
	} else if(commandline != NULL) {
		line = commandline;
		vToks = Util::split(line, ";");
		for(unsigned int i = 0; i < vToks.size(); i++) {
			if(mainShell.execCmd(vToks[i])!= 0) {
				cerr << "Error: Can't understand line:" << endl << vToks[i] <<  endl;
				//no need to delte, exiting
				return 2;
			}
		}

	} else {
		mainShell.start();
	}

	return 0;
}
