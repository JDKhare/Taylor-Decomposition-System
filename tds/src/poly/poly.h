/*
 * =====================================================================================
 *
 *        Filename:  poly.h
 *     Description:  Polynomial Evaluation
 *         Created:  04/15/2007 07:14:18 PM EDT
 *          Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)(R&D Engineer)
 *         Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */


#ifndef __INFIXPOLY_H__
#define __INFIXPOLY_H__

#include <string>
using std::string;

/**
 * @namespace polyparser
 * @brief Parses algebraic expressions and produce a tree with the operations
 **/
namespace polyparser {
	using namespace std;

	/**
	 * @class PolyParser
	 * @brief Main parser of algebraic expressions.
	 **/
	class PolyParser {
	private:
		unsigned int getOpPriority(char c);
		string _infixPoly;
		string _postfixPoly;
		string _name;
	public:
		PolyParser(string poly): _infixPoly(poly), _postfixPoly(""), _name("") {
			fixSyntax();
			bool ok = infixToPostfix();
			if(!ok) {
				//infixToPostfix annotates the error message in _postfixPoly
				throw(string("02003.")+_postfixPoly);
			}
		}
		~PolyParser(void) {};

		string getPostfix(void) { return _postfixPoly; }
		string getName(void) { return _name; }
		void fixSyntax(void);
		bool infixToPostfix(void);
		//T evaluate(string & poly);

		static const char ERR='%';
		static const char REG='@';
		static const char LSH='<';
		static const char ADD='+';
		static const char SUB='-';
		static const char MUL='*';
		static const char DIV='/';
		static const char POW='^';
		static const char BIN=':';

	};



#if 0
	template<typename T>
		T PolyParser<T>::evaluate(string & poly) {
			T opResult, opRight, opLeft;
			stack <T> s;

			string postfix;
			if(infixToPostfix(poly,postfix)) {
				//infixToPostfix annotates in postfix any error message
				throw(string("02001. ")+postfix);
			}
			for(unsigned int i = 0; i < postfix.size(); i ++) {
				i = postfix.find_first_not_of(" ", i);
				if(postfix[i] == '|' ||
				   postfix[i] == '@' ||
				   postfix[i] == '^' ||
				   postfix[i] == '*' ||
				   postfix[i] == '/' ||
				   postfix[i] == '+' ||
				   postfix[i] == '-') {
					if(s.size()< 2) {
						throw(string("02002. One of the terms in operation ")+ postfix[i] + string(" is missing or incorrect"));
					}
					opRight = s.top();
					s.pop();
					opLeft = s.top();
					s.pop();
					switch(postfix[i]) {
					case '^':
						opResult = opLeft ^ opRight;
						break;
					case '*':
						opResult = opLeft* opRight;
						break;
					case '/':
						opResult = opLeft / opRight;
						break;
					case '+':
						opResult = opLeft + opRight;
						break;
					case '-':
						opResult = opLeft - opRight;
						break;
					case '@':
						opResult = opLeft*(2 ^ opRight);
						break;
					case '|':
						break;
					default:
						break;
					}
					s.push(opResult);
				} else {
					unsigned int k = postfix.find(' ', i);
					s.push(T(postfix.substr(i, k-i)));
					i = k;

				}
			}
			return s.top();
		};
#endif



};

#endif
