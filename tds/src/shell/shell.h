/*
 * =====================================================================================
 *
 *        Filename:  shell.h
 *     Description:  Shell class
 *         Created:  12/09/2005 07:08:31 PM EST
 *          Author:  Daniel Gomez-Prado (from jan 08), Qian Ren (up to dec 07) ()
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 173                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-01-06 18:06:14 -0500 (Fri, 06 Jan 2012)     $: Date of last commit
 * =====================================================================================
 */

//    shell->AddCommand("read", new ShellFunctor <Swift> (this, &Swift::ReadVerilog));
#ifndef __SHELL_H__
#define __SHELL_H__

#include <string>
#include <map>
#include <list>
#include <ctime>
#include <vector>

/**
 * @namespace shell
 * @brief     Access to available commands
 **/
namespace shell {
	using namespace std;

	enum ReturnValue {
		CMD_OK, /**< @brief The command finished its execution successfuly*/
		CMD_BRIEF, /**< @brief The command provided a brief explanation of its functionality*/
		CMD_HELP, /**< @brief The command provided help on its arguments and options*/
		CMD_INFO, /**< @brief The command could not be executed as some arguments are missing or some conditions are not satisfied*/
		CMD_WARN, /**< @brief The command executed but a warning flag has been raised*/
		CMD_ERROR, /**< @brief The command could not finish its execution for a known reason*/
		CMD_FATAL /**< @brief The command could not finish its execution for an unkown reason*/
	};

	/**
	 * @class BaseFunctor
	 * @brief Provides a virtual operator() that can be overload.
	 **/
	class BaseFunctor{
	public:
		virtual ReturnValue operator()(unsigned, char**) = 0;
		virtual bool isVisible(void) = 0;
		virtual ~BaseFunctor() {};
	};

	/**
	 * @class ShellFunctor
	 * @brief Encapsulation at construction of arbitrary class type pointer, so they can be accessed as functors.
	 **/
	template <class TClass> class ShellFunctor : public BaseFunctor {
	public:
		enum TypeOfCommand { publicCmd, privateCmd };
	private:
		enum TypeOfMember { staticMember, nonStaticMember };
		TypeOfCommand _visibility;
		TypeOfMember _memberIs;
		ReturnValue (TClass::*_functorPointer) (unsigned, char**);
		ReturnValue (*_staticFunctorPointer)(unsigned, char**);
		TClass* _objectPointer;
	public:
		/**@brief provides access to non static members of a class*/
		ShellFunctor (TClass* ptr, ReturnValue (TClass::*functor)(unsigned, char**), TypeOfCommand cmdType = publicCmd) {
			_objectPointer = ptr;
			_functorPointer = functor;
			_staticFunctorPointer=NULL;
			_memberIs = nonStaticMember;
			_visibility = cmdType;
		};
		/**@brief provides access to static members of a class*/
		ShellFunctor(ReturnValue (*functor)(unsigned, char**), TypeOfCommand cmdType = publicCmd) {
			_objectPointer = NULL;
			_functorPointer = NULL;
			_staticFunctorPointer=functor;
			_memberIs = staticMember;
			_visibility = cmdType;
		};
		/**@brief sorts out the access method that should be provided*/
		virtual ReturnValue operator () (unsigned argc, char** argv) {
			switch (_memberIs) {
			case staticMember:
				return (*_staticFunctorPointer)(argc, argv);
			case nonStaticMember:
				return (*_objectPointer.*_functorPointer)(argc, argv);
			default:
				break;
			}
			return CMD_FATAL;
		};
		virtual bool isVisible(void) {
			return (_visibility==publicCmd);
		}
	};

	/**
	 * @class Shell
	 * @brief Encapsulates all shell functionality, exits with quit.
	 **/
	class Shell {
	private:
		string _welcome;	 /**< @brief the welcome message to display upon entering the shell*/
		string _prompt;		/**< @brief the format of the prompt line in the shell*/
		unsigned _cmdCounter; /**< @brief number of successfuly executed commands*/

		typedef map<string, BaseFunctor* >  CommandMap;
		CommandMap _tCommands; 	/**< @brief a table linking the name of the command with its BaseFunctor so it can be executed*/
		list<string> _lCommands; 	/**< @brief list of all command's name available*/
		vector<float> _hTimes;
		vector<string> _hCommands;
		clock_t	_startClock; 	/**< @brief time when the last successful command started its execution*/
		clock_t _usedClock; 	/**< @brief time when the last successful command finished its execution*/

		map <string, string> aliasTable;
		void splitline(string&, int&, char** &);

		/**
		 * @brief Implements a delete of a pointer through a constructor.
		 **/
		struct DeleteObject {
			/**
			 * @brief Overloading the operator() makes the constructor of the structure to behave as a function
			 * @param ptr Takes the value type of CommandMap, a pair<const key, T> and deletes T
			 **/
			void operator()(const CommandMap::value_type & cmd2del) const {
				delete cmd2del.second;
			}
		};

	public:
		Shell(void);
		~Shell(void);
		Shell(string prompt);
		Shell(string welcome, string prompt);

		//
		// Adding new methods and members
		//
		bool status;

		void start(void);
		void addCommand(string, BaseFunctor* );
		ReturnValue execCmd(string str);
		void initAliasTable(string aliasFileNames);
		pair <string,bool> getAlias(string alias);

	};
}
#endif
