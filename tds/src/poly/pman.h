/*
 * =====================================================================================
 *
 *       Filename:  pman.h
 *    Description:
 *        Created:  02/12/2010 11:32:38 AM
 *         Author:  Daniel F. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 181                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2010)$: Date of last commit
 * =====================================================================================
 */
#ifndef __PMAN_H__
#define __PMAN_H__

#include <stack>
#include <cstdlib>
#include <climits>
using namespace std;

#include "util.h"

#include "Environment.h"
#include "TedMan.h"
#include "ATedNode.h"
#include "ETedNode.h"

namespace polyparser {

	using namespace ted;
	using tds::Environment;
	using util::Util;

	class PNode;

	class PMan {
	private:
		static unsigned int _ps_count;
		PNode* _root;
		TedMan* _pTedMan;
		bool _intoConstant;
		string _constant;
		bool _hasRegisters;

		template<typename T>
			ATedNode* ParseTreeTo(PNode* pRoot);

	public:
		explicit PMan(TedMan* pTedMan, string constprefix = ""): \
			_root(NULL), _pTedMan(pTedMan), \
			_intoConstant(false), _constant(constprefix), _hasRegisters(false) {
				if(!_constant.empty()) {
					_intoConstant = true;
				}
			}
		~PMan(void) { _pTedMan=NULL; deleteParseTree(); }

		bool hasRegisters(void) { return _hasRegisters; }
		void buildParseTree(string postfix);
		void deleteParseTree(void);
		template<typename T>
			ATedNode* convertTo(string postfix);
	};

	template<typename T>
		ATedNode* PMan::convertTo(string postfix) {
			buildParseTree(postfix);
			return ParseTreeTo<T>(_root);
		}


	template<typename T>
		ATedNode* PMan::ParseTreeTo(PNode* pRoot) {
			assert(ATedNode::checkContainer(_pTedMan->getContainer()));
			ATedNode* t,* t1,* t2;
			pair<bool, long long> p;
			double fxp = 0.0;
			switch(pRoot->getType()) {
			case PNode::ADD:
				t1 = ParseTreeTo<T>(pRoot->getLeft());
				t2 = ParseTreeTo<T>(pRoot->getRight());
				t = t1->add(t2);
				break;
			case PNode::SUB:
				t1 = ParseTreeTo<T>(pRoot->getLeft());
				t2 = ParseTreeTo<T>(pRoot->getRight());
				t = t1->sub(t2);
				break;
			case PNode::MUL:
				t1 = ParseTreeTo<T>(pRoot->getLeft());
				t2 = ParseTreeTo<T>(pRoot->getRight());
				t = t1->mul(t2);
				break;
			case PNode::EXP:
				t1 = ParseTreeTo<T>(pRoot->getLeft());
				t2 = ParseTreeTo<T>(pRoot->getRight());
				t = t1->exp(t2);
				break;
			case PNode::DIV:
				throw(string("03005. Currently there is no support for division"));
				break;
			case PNode::REG:
				t1 = ParseTreeTo<T>(pRoot->getLeft());
				t2 = ParseTreeTo<T>(pRoot->getRight());
				//t2 has been already checked to be of constant value
				if(!t1->isConst()) {
					t = t1->reg(t2);
					_hasRegisters = true;
				} else {
					std::cerr << "Warning: Check the operator precedence of ..." << t1->getName()<< PolyParser::REG << t2->getName()<< std::endl;
					std::cerr << "         Retiming of a constant value is equivalent to disregarding entirely the operator " << PolyParser::REG << std::endl;
					t = t1;
				}
				break;
			case PNode::VAR:
				{
					string name = pRoot->getName();
					string prefix = Environment::getStr("const_prefix");
					p = Util::atoi(name.c_str());
					fxp = atof(name.c_str());
					if(p.first) {
						//if it's integer
						if(_intoConstant && p.second != 0 && p.second != 1 && p.second != -1) {
							//extract the integer into a variable
							string s = prefix;
							s += name;
							t = new T(s,p.second);
						} else {
							//use the constant as a weight
							t = new T(p.second);
						}
					} else {
						if(0.0!=fxp && !p.first && name[0]!='e' && name[0]!='E') {
							//extract the fixed point into a variable
							string s = prefix;
							s+=name;
							t = new T(s,name);
						} else {
							int last = strlen(name.c_str());
							if(name[last-1]==PolyParser::BIN) {
								string bin_name = name;
								bin_name.erase(last-1);
								t = new T(bin_name,true);
							} else {
								if (name.compare(0,prefix.size(),prefix) ==0) {
									//default constant value is 1
									t = new T(name, (wedge)1);
								} else {
									t = new T(name);
								}
							}
						}
					}
					break;
				}
			default:
				throw(string("03006. UNKOWN operation"));
			}
			return t;
		}


}

#endif

