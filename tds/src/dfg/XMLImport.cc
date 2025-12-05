/*
 * =====================================================================================
 *
 *       Filename:  XMLImport.h
 *    Description:
 *        Created:  7/4/2012 10:50:27 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 203                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-11-18 08:13:53 -0500(Thu, 18 Nov 2012)$: Date of last commit
 * =====================================================================================
 */
#include <cassert>
#include <iostream>

#include "XMLImport.h"
#include "DfgOperator.h"
#include "DfgNode.h"


namespace dfg {

		const char* XMLImport::XML_NODE = "node";
		const char* XMLImport::XML_EDGE = "edge";
		const char* XMLImport::XML_ATTR = "attr";
		const char* XMLImport::XML_KEY = "key";
		const char* XMLImport::XML_VALUE = "value";

		const char* XMLImport::ATTR_CLASS = "CLASS";
		const char* XMLImport::ATTR_NAME = "NAME";
		const char* XMLImport::ATTR_NATURE = "NATURE";
		const char* XMLImport::ATTR_PROPERTY = "PROPERTY";
		const char* XMLImport::ATTR_NUM_INPUT = "NUM_INPUT";

		namespace {
			string generateName(string& suggestedName) {
				string name(suggestedName);
				Util::strSubstitute(name,"[","_");
				Util::strSubstitute(name,"]","_");
				return name;
			}
		}

		bool XMLImport::VisitEnter( const XMLDocument& doc ) {
			return true;
		}

		bool XMLImport::VisitEnter( const XMLElement& element, const XMLAttribute* attribute ) {
			OpenElement( element.Name() );
				return 	ProcessElement(element);
		}


		bool XMLImport::VisitExit( const XMLElement& ) {
			CloseElement();
				return true;
		}

		void XMLImport::OpenElement( const char* name ) {
			stack.Push( name );
				++depth;
		}

		void XMLImport::CloseElement() {
			--depth;
				const char* name = stack.Pop();
		}

		bool XMLImport::ProcessElement(const XMLElement& element) {
			if(!strcmp(element.Name(),XML_NODE)) {
				ProcessNode(element);
					return false;
			} else if (!strcmp(element.Name(),XML_EDGE)) {
				ProcessEdge(element);
					return false;
			} else {
				return true;
			}
		}

		void XMLImport::ProcessNode(const XMLElement& element) {
			DfgOperator::Type nodeType = DfgOperator::ERROR;
				const char* attrName = NULL;
				const char* attrTypeStr = NULL;
				const char* attrProperty = NULL;
				const char* attrIndexes = NULL;
				const char* nodeName = element.Attribute("name");
				int nodeID = element.IntAttribute("id");
				assert(nodeName);
				assert(nodeID>0);
				string bracket_name(Util::strsav(nodeName));
				bracket_name += Util::strsav(element.Attribute("id"));
				string node_name = generateName(bracket_name);

				for ( const XMLNode* node=element.FirstChild(); node; node=node->NextSibling() ) {
					const XMLElement* enode = node->ToElement();
						if (enode) {
							assert(!strcmp(enode->Name(),XML_ATTR));
								if (!strcmp(enode->Attribute(XML_KEY),ATTR_CLASS)) {
									const char* node_type = enode->Attribute(XML_VALUE);
										if (!strcmp(node_type,"DATA")) {
											nodeType = DfgOperator::VAR;
										} else {
											assert(!strcmp(node_type,"OP"));
										}
								} else if (!strcmp(enode->Attribute(XML_KEY),ATTR_NAME)) {
									attrName = enode->Attribute(XML_VALUE);
								} else if (!strcmp(enode->Attribute(XML_KEY),ATTR_NATURE)) {
									attrTypeStr = enode->Attribute(XML_VALUE);
								} else if (!strcmp(enode->Attribute(XML_KEY),ATTR_PROPERTY)) {
									attrProperty = enode->Attribute(XML_VALUE);
								} else if (!strcmp(enode->Attribute(XML_KEY),"INDEXES")) {
									attrIndexes = enode->Attribute(XML_VALUE);
								}
						}
				}
			DfgNode* pNode = NULL;
				if (nodeType==DfgOperator::VAR) {
					assert(attrName);
						DfgMapID::iterator IDIterator = IDTable.find(nodeID);
						if(IDIterator != IDTable.end()) {
							throw(string("09001. Duplicated unique ID for node ")+ node_name);
						}
					DfgMapName::iterator NameIterator = NameTable.find(node_name);
#if 1
						assert(NameIterator==NameTable.end());
#else
						if (NameIterator != NameTable.end()) {
							if (attrIndexes) {
								node_name += attrIndexes;
									NameIterator = NameTable.find(node_name);
									if (NameIterator!=NameTable.end()) {
										pNode = NameIterator->second;
									}
							} else {
								pNode = NameIterator->second;
							}
						}
					if (!pNode) {
						pNode = new DfgNode(_dMan,node_name,nodeType);
							_dMan->registerNode(pNode);
							NameTable.insert(pair<string,DfgNode*>(node_name,pNode));
					}
#endif


						if (attrProperty && !strcmp(attrProperty,"OUTPUT")) {
#if 1
							PoTable.insert(pair<int,string>(nodeID,node_name));
#else
								if(_dMan->getPos()->find(node_name) == _dMan->getPos()->end()) {
									_dMan->getPos()->insert(pair<string,DfgNode*>(node_name,pNode));
								} else {
									//if PO already exist load it as PO_read
									//and generate its difference PO_diff = PO - PO_read
									throw(string("09001. PO") + string(attrName) + string("already exist"));
								}
#endif
						} else {
							pNode = new DfgNode(_dMan,node_name,nodeType);
								_dMan->registerNode(pNode);
								NameTable.insert(pair<string,DfgNode*>(node_name,pNode));
								IDTable.insert(pair<int,DfgNode*>(nodeID,pNode));
						}
				} else {
					// node has to be a OP
					DfgMapID::iterator IDIterator = IDTable.find(nodeID);
						assert(IDIterator==IDTable.end());
						if(!strcmp(attrTypeStr,"ADD")) {
							nodeType=DfgOperator::ADD;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else if(!strcmp(attrTypeStr,"SUB")) {
							nodeType=DfgOperator::SUB;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else if(!strcmp(attrTypeStr,"MUL")) {
							nodeType=DfgOperator::MUL;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else if(!strcmp(attrTypeStr,"LSH")) {
							nodeType=DfgOperator::LSH;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else if(!strcmp(attrTypeStr,"RSH")) {
							nodeType=DfgOperator::RSH;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else if(!strcmp(attrTypeStr,"DIV")) {
							nodeType=DfgOperator::RSH;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else if(!strcmp(attrTypeStr,"DELAY")) {
							nodeType=DfgOperator::REG;
								string regamount = "1";
								regamount += DfgNode::TOKEN_REG;
								pNode = new DfgNode(_dMan,regamount,nodeType);
						} else if(!strcmp(attrTypeStr,"EQ")) {
							nodeType=DfgOperator::EQ;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						} else {
							std::cout << "Warning: unknown operator detected, proceed at your own risk" << std::endl;
								pNode = new DfgNode(_dMan,nodeType,NULL,NULL);
						}
					IDTable.insert(pair<int,DfgNode*>(nodeID,pNode));
				}
		}

		void XMLImport::ProcessEdge(const XMLElement& element) {
			int id_pred = element.IntAttribute("id_pred");
				int id_succ = element.IntAttribute("id_succ");
				const char* edgeW = NULL;
				for ( const XMLNode* node=element.FirstChild(); node; node=node->NextSibling() ) {
					const XMLElement* enode = node->ToElement();
						if (enode) {
							assert(!strcmp(enode->Name(),XML_ATTR));
								if (!strcmp(enode->Attribute(XML_KEY),ATTR_NUM_INPUT)) {
									edgeW = enode->Attribute(XML_VALUE);
								}
						}
				}
			assert(IDTable.find(id_pred)!=IDTable.end());
				DfgNode* pred = IDTable.find(id_pred)->second;
				if (IDTable.find(id_succ) ==IDTable.end()) {
					assert(PoTable.find(id_succ)!=PoTable.end());
						_dMan->getPos()->insert(pair<string,DfgNode*>(PoTable.find(id_succ)->second,pred));
				} else {
					DfgNode* succ = IDTable.find(id_succ)->second;
						switch (succ->getOp()) {
						case DfgOperator::REG:
						case DfgOperator::EQ:
											 {
												 assert(succ->getLeft() ==NULL && succ->getRight() ==NULL);
													 succ->setKids(pred,pred);
													 break;
											 }
						case DfgOperator::ADD:
						case DfgOperator::MUL:
											  {
												  assert(succ->getRight() ==NULL);
													  if (succ->getLeft()) {
														  succ->setKids(succ->getLeft(),pred);
													  } else {
														  assert(succ->getLeft() ==NULL);
															  succ->setKids(pred,NULL);
													  }
												  break;
											  }
						case DfgOperator::SUB:
						case DfgOperator::LSH:
						case DfgOperator::RSH:
											  {
												  assert(edgeW);
													  assert( (!strcmp(edgeW,"0")) ? (succ->getLeft() ==NULL) : (succ->getRight() ==NULL));
													  (!strcmp(edgeW,"0")) ? succ->setKids(pred,succ->getRight()) : succ->setKids(succ->getLeft(),pred);
													  break;
											  }
						default:
								{
									// Operator is a VAR, VARCONT or CONST type but it is being assigned a value by another operand
									// that is, our variable is in a loopback!!
									std::cout << "Info: implicit loopback by id_pre=" << id_pred << " and id_succ=" << id_succ << std::endl;
										std::cout << "      separating loopback cycle " << succ->getName() << " -> " << succ->getName() << "_loopback" << std::endl;
										string loopback_name = Util::strsav(succ->getName());
										loopback_name += "_loopback";
										//DfgNode* loopback_node = new DfgNode(_dMan,loopback_name,succ->getOp());
										//_dMan->registerNode(loopback_node);
										_dMan->getPos()->insert(pair<string,DfgNode*>(loopback_name,pred));
										break;
								}
						}
				}
		}

}
