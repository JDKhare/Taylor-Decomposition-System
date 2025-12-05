/*
 * =====================================================================================
 *
 *       Filename:  poly.cc
 *    Description:
 *        Created:  02/12/2010 09:36:26 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */

#include <cmath>
#include <list>
#include <set>
#include <cassert>
#include <stack>
using namespace std;

#include "poly.h"

namespace polyparser {

	unsigned int PolyParser::getOpPriority(char c) {
		unsigned int ret;
		switch(c) {
		case REG:
			ret = 5;
			break;
		case LSH:
			ret = 4;
			break;
		case POW:
			ret = 3;
			break;
		case MUL:
		case DIV:
			ret = 2;
			break;
		case ADD:
		case SUB:
			ret = 1;
			break;
		case ERR:
		default:
			ret = 0;
			break;
		}
		return ret;
	}

	void PolyParser::fixSyntax(void) {
		size_t index = _infixPoly.find('=');
		if(index != string::npos) {
			_name = _infixPoly.substr(0, index);
			_infixPoly  = _infixPoly.substr(index+1);
		}
		if(_infixPoly[0] == '-')_infixPoly = "0" + _infixPoly;
		index=_infixPoly.find("<<");
		while(index!= string::npos) {
			_infixPoly.replace(index,2,"<");
			index=_infixPoly.find("<<",index+1);
		}
		if(_infixPoly.find_first_of("+-*^<") == string::npos) {
			_infixPoly = "1*"+ _infixPoly;
		}
		index = _infixPoly.find(")(");
		while(index != string::npos) {
			_infixPoly.replace(index,2,")*(");
			index = _infixPoly.find(")(",index+2);
		}
		index=_infixPoly.find("(-");
		while(index != string::npos) {
			_infixPoly.replace(index,2,"((0-1)*");
			index=_infixPoly.find("(-",index+6);
		}
		index=_infixPoly.find("*-");
		while(index != string::npos) {
			_infixPoly.replace(index,2,"*((0-1)*");
			index = _infixPoly.find_first_of("+-*/^<",index+8);
			if(index!=string::npos) {
				_infixPoly.insert(index,")");
				index = _infixPoly.find("*-",index+1);
			} else {
				_infixPoly = _infixPoly + ")";
			}
		}
		index=_infixPoly.find("/-");
		while(index != string::npos) {
			_infixPoly.replace(index,2,"/((0-1)*");
			index = _infixPoly.find_first_of("+-*/^<",index+8);
			if(index!=string::npos) {
				_infixPoly.insert(index,")");
				index = _infixPoly.find("/-",index+1);
			} else {
				_infixPoly = _infixPoly + ")";
			}
		}
		index=_infixPoly.find("^-");
		while(index != string::npos) {
			_infixPoly.replace(index,2,"^((0-1)*");
			index = _infixPoly.find_first_of("+-*/^<",index+8);
			if(index!=string::npos) {
				_infixPoly.insert(index,")");
				index = _infixPoly.find("^-",index+1);
			} else {
				_infixPoly = _infixPoly + ")";
			}
		}
	}
	/**
	  @brief
	  @param infix the input polynomial
	  @param postfix when there are no errors, it holds the operations to do.
	  If there is any error, it holds the error message.
	  @return 0 if no error occurred, 1 otherwise
	 **********************************************/
	bool PolyParser::infixToPostfix(void) {
		stack <char> stOp;

		string errmess(_infixPoly);
		unsigned int bracketOpen = 0;
		unsigned int bracketClose = 0;
		bool beforeBracket = false; // watch for varName(
		bool afterBracket = false;  // watch for)varName

		_infixPoly += '\n';
		stOp.push('\n');

		for(unsigned int i = 0; i < _infixPoly.size(); i++) {
			switch(_infixPoly[i]) {
			case '[' : // fall through
			case '{' : // fall through
			case '(' :
				if(beforeBracket) {
					errmess.insert(i,"\"?\"",3);
					_postfixPoly = string("Missing operator at ?, in ");
					_postfixPoly += errmess;
					return false;
				}
				bracketOpen++;
				_postfixPoly += ' ';
				//stOp.push(_infixPoly[i]);
				stOp.push('(');
				break;
			case ']' : // fall through
			case '}' : // fall through
			case ')' :
				afterBracket=true;
				bracketClose++;
				_postfixPoly += ' ';
				while(stOp.top()!= '(') {
					_postfixPoly += stOp.top();
					stOp.pop();
					if(stOp.empty()) {
						errmess.insert(i+1,"\"",1);
						errmess.insert(i,"\"",1);
						_postfixPoly = string("There is no \"(\" for ");
						_postfixPoly += errmess;
						return false;
					}
				}
				if(stOp.size() == 0)
					return false;
				stOp.pop();
				break;
			case REG : // '@' fall through
			case LSH : // '<' fall through
			case POW : // '^' fall through
			case MUL : // '*' fall through
			case DIV : // '/' fall through
			case ADD : // '+' fall through
			case SUB : // '-' fall through
				beforeBracket=afterBracket=false;
				_postfixPoly += ' ';
				if(stOp.size() == 0 || getOpPriority(_infixPoly[i])> getOpPriority(stOp.top())) {
					stOp.push(_infixPoly[i]);
				} else {
					while(getOpPriority(_infixPoly[i])<= getOpPriority(stOp.top())) {
						_postfixPoly += stOp.top();
						stOp.pop();
					}
					stOp.push(_infixPoly[i]);
				}
				break;
			case '`' : // fall through
			case '#' : // fall through
			case '$' : // fall through
			case '\\': // fall through
			case ';' : // fall through
			case '\"': // fall through
			case '\'': // fall through
			case ',' : // fall through
			case '?' :
				_postfixPoly = string("Symbol \"");
				_postfixPoly +=_infixPoly[i];
				_postfixPoly +=string("\" not allowed as part of a variable name");
				return false;
				//case ':' : // fall through
			case '~' : // fall through
			case '!' : // fall through
			case '|' : // fall through
			case '&' : // fall through
			case '>' : // fall through
			case ERR : // '%'
				_postfixPoly = string("UNKOWN operator \"");
				_postfixPoly +=_infixPoly[i];
				_postfixPoly +=string("\"");
				return false;
			case ' ':
				break;
			case '\n':
				_postfixPoly += ' ';
				while(stOp.top()!= '\n') {
					_postfixPoly += stOp.top();
					stOp.pop();
				}
				break;
			default:
				if(afterBracket) {
					errmess.insert(i,"\"?\"",3);
					_postfixPoly = string("Missing operator at ?, in ");
					_postfixPoly += errmess;
					return false;
				}
				beforeBracket=true;
				_postfixPoly += _infixPoly[i];
				break;
			}
		}
		if(bracketClose!=bracketOpen) {
			_postfixPoly = "Parenthesis mismatch, there were ";
			_postfixPoly += (bracketClose > bracketOpen) ? "more closures" : "more openings";
			return false;
		}

		// A space ' ' in the postfix string, means that an operation or operand follows.
		// therefore there should be none trailing ' ' characters.
		while(' '==_postfixPoly[_postfixPoly.size()-1]) {
			_postfixPoly.erase(_postfixPoly.size()-1,1);
		}
		return true;
	}

}
