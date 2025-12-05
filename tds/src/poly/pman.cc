/*
 * =====================================================================================
 *
 *       Filename:  pman.cc
 *    Description:
 *        Created:  02/12/2010 12:56:06 PM
 *         Author:  Daniel F. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-05-11 09:39:43 -0400 (Fri, 11 May 2012)     $: Date of last commit
 * =====================================================================================
 */

#include "pnode.h"
#include "pman.h"
#include "poly.h"

namespace polyparser {

	unsigned int PMan::_ps_count = 0;

	void PMan::deleteParseTree(void) {
		if (_root != NULL) {
			vector <PNode *> vNodes;
			_root->collectDFS(vNodes);
			for (unsigned int i = 0; i < vNodes.size(); i++) {
				delete vNodes[i];
			}
		}
	}

	void PMan::buildParseTree(string postfix) {
		PNode *  opResult, * opRight, * opLeft;
		stack <PNode *> s;

		for (unsigned int i = 0; i < postfix.size(); i ++) {
			i = postfix.find_first_not_of(" ", i);
			if (postfix[i] == PolyParser::REG ||
				postfix[i] == PolyParser::LSH ||
				postfix[i] == PolyParser::POW ||
				postfix[i] == PolyParser::MUL ||
				postfix[i] == PolyParser::DIV ||
				postfix[i] == PolyParser::ADD ||
				postfix[i] == PolyParser::SUB) {
				if (s.size() < 2) {
					throw (string("02004. One of the terms in operation ") + postfix[i] + string(" is missing or incorrect"));
				}
				opRight = s.top();
				s.pop();
				opLeft = s.top();
				s.pop();
				switch (postfix[i]) {
				case PolyParser::ADD:
					opResult = opLeft->add(opRight);
					break;
				case PolyParser::SUB:
					opResult = opLeft->sub(opRight);
					break;
				case PolyParser::MUL:
					opResult = opLeft->mul(opRight);
					break;
				case PolyParser::DIV:
					opResult = opLeft->div(opRight);
					break;
				case PolyParser::POW:
					opResult = opLeft->exp(opRight);
					break;
				case PolyParser::LSH:
					{
						pair <bool, long> valid = Util::atoi(opRight->getName());
						if ( !valid.first ) {
							throw(string("02005. Right hand side of left shift operator, must be constant, not ") +string(opRight->getName()));
						}
						if(valid.second < 0) {
							throw(string("02006. Overflow on operand ") + string(opRight->getName()));
						}
						double val = pow(2,static_cast<double>(valid.second));
						if(val>= UINT_MAX) {
							throw(string("02007. Overflow computing shift value ") + string(" 2^") + string(opRight->getName()));
						}
						unsigned int result = static_cast<unsigned int>(val);
						delete opRight;
						opRight = new PNode(Util::itoa(result));
						opResult = opLeft->mul(opRight);
						break;
					}
				case PolyParser::REG:
					{
						pair <bool, long> valid = Util::atoi(opRight->getName());
						if( !valid.first ) {
							throw(string("02008. Right hand side of register operator must be an integer value"));
						} else {
							opResult = opLeft->reg(opRight);
						}
						break;
					}
				default:
					break;
				}
				s.push(opResult);
			} else {
				unsigned int k = postfix.find(' ', i);
				string newName = postfix.substr(i, k-i);
				opResult = new PNode(newName);
				s.push(opResult);
				i = k;
			}
		}
		_root = s.top();
	};

}
