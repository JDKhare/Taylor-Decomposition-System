/*
 * =====================================================================================
 *
 *       Filename:  TedManCSE.cc
 *    Description:
 *        Created:  12/8/2008 10:54:04 AM
 *         Author:  Daniel f. Gomez-Prado(dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 182                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2010-10-15 11:14:18 -0400(Fri, 15 Oct 2010)$: Date of last commit
 * =====================================================================================
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <list>

#include "TedMan.h"
#include "TedNode.h"
#include "TedKids.h"
#include "TedVar.h"
#include "ETedNode.h"
#include "TedVarGroup.h"
#include "TedVarMan.h"

#include "DfgNode.h"
using dfg::DfgNode;
using dfg::DfgStat;

#include "util.h"
using util::Util;


namespace ted {


	/*****Function that create mpy reference list*****/
	/***** author Guillot Jeremie***************/
	/**************    Date 13 mai 2008**************/
	void TedMan::searchmpyclass(vector<pair<TedNode*, TedNode*> > & list_decomp_node) {
		TedNode* pNode = NULL,* pKid = NULL;
		wedge weight = 0;
		unsigned int index = 0;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;
		map <TedNode*, vector<pair <TedNode*, pair< unsigned int, wedge> > > > linkofnodes;
		map <TedNode*, vector<pair <TedNode*, pair< unsigned int, wedge> > > >::iterator it_linkofnodes;
		vector<pair< TedNode*, pair<unsigned int,wedge> > >::iterator it_parent;
		vector<pair<TedNode*, TedNode*> > it_vector;
		list_decomp_node.clear();	 //nettoyage de la liste
		/*creation d'une liste de noeud avec enfants pour references*/
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				cout<<"-----------------------------------------------"<<endl;
				cout<<pNode->getName()<<" goes to "<<endl;
				//pour chacun des enfants je met le noeud et dans la liste associs ses enfants
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					weight = _iterkids.getWeight();
					linkofnodes[pKid].push_back(pair<TedNode*, pair<unsigned int,wedge> >(pNode, pair<unsigned int,wedge>(index,weight)));
					cout<<pKid->getName()<<" is referenced by "<<pNode->getName()<<" with a "<<index<<"-edge"<<endl;
				}
			}
		}
		//	it_vector=arg_candidate_list.begin();
		/* pour chacun des noeud de  la liste*/
		string name_first_child;
		/*	for(it_linkofnodes=linkofnodes.begin();it_linkofnodes!=linkofnodes.end();it_linkofnodes++)
			{
			if(it_linkofnodes->first==_pOne)
			continue;
			if(it_linkofnodes->second.size()<2)
			continue;
			samename=true;
			name_first_child=it_linkofnodes->second.begin()->first->getName();
		//lister les parents
		for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++)
		{
		if(name_first_child!=it_parent->first->getName())
		{samename=false; break; }
		}
		if(samename==false) {
		cout<<"------------------------------------------"<<endl;
		cout<<it_linkofnodes->first->getName()<<" "<<it_linkofnodes->first<<" is referenced by"<<endl;

		for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++)
		{
		cout<<it_parent->first->getName()<<" "<< it_parent->first<<" with a "<<it_parent->second.first<<"-edge"<<endl;
		}}

		}
		pNode=NULL;*/
	}

	//Return a vector of pointeur to TedNode that are purely additive
	void TedMan::searchadditiveNode(vector<TedNode*>  & add_candidate_list) {
		TedNode* pNode = NULL,* pKid = NULL;
		wedge weight = 0;
		unsigned int index = 0;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;
		map <TedNode*, vector<pair <TedNode*, pair< unsigned int, wedge> > > > linkofnodes;
		map <TedNode*, vector<pair <TedNode*, pair< unsigned int, wedge> > > >::iterator it_linkofnodes;
		vector<TedNode* > add_candidate_list_temp;
		vector<pair< TedNode*, pair<unsigned int,wedge> > >::iterator it_parent;
		vector<pair<TedNode*, TedNode*> > it_vector;
		add_candidate_list.clear();	 //nettoyage de la liste final
		/*creation des references*/
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					weight = _iterkids.getWeight();
					linkofnodes[pKid].push_back(pair<TedNode*, pair<unsigned int,wedge> >(pNode, pair<unsigned int,wedge>(index,weight)));
				}
			}
		}
		string parent;
		TedNode* node_parent = NULL;
		TedNode* node_children = NULL;
		bool addonly;
		bool sameparent;
		unsigned int type_edge = 0;
		string children;
		int nbref;
		/* pour chacun des enfants*/
		for(it_linkofnodes=linkofnodes.begin();it_linkofnodes!=linkofnodes.end();it_linkofnodes++) {
			if(it_linkofnodes->first==TedNode::getOne())
				continue;

			children=it_linkofnodes->first->getName();
			node_children=it_linkofnodes->first;
			/*cond initiale*/
			addonly=true;
			nbref=0;
			it_parent=it_linkofnodes->second.begin();
			parent=it_parent->first->getName();
			node_parent=it_parent->first;
			type_edge=it_parent->second.first;

			/*lister les parents*/
			for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++) {
				nbref++;
				//to print all the dependancies
				if(it_parent->second.first!=0) {
					addonly=false;
					break;
				}
			}
			//if the node is purely additive and referenced more than once
			if(addonly==true && nbref>1) {
				sameparent=true;
				//Dump dans add_candidate_list des noeuds additifs dont on peut rduire le nbre de parents

				/*recherche si les parents du noeud sont de mme nom et si leur arcs mpy vont vers le mme noeud*/
				for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++) {
					//cout<<it_parent->first->getName()<<" "<< it_parent->first<<" with a "<<it_parent->second.first<<"-edge"<<endl;
					//regle  modifier
					if((it_parent->first->getName()!=parent)||(it_parent->first->getKidNode(1) ==TedNode::getOne())) {
						sameparent=false;
						break;
					}
				}
				/*s'ils ont le mme nom et que le first edge des parents est le mme alors ajout du noeud  la liste temporaire*/
				if(sameparent==true) {
					//il faut en plus pour qu'il soit additif pure qu'il ne soit pas multipli par autre chose que noeu terminal 1
					if(it_linkofnodes->first->getKidNode(1) ==TedNode::getOne()) {
						add_candidate_list.push_back(it_linkofnodes->first);
					}
				}
			}
		}
		pNode=NULL;

	}

	void TedMan::searchFactorizableNode(vector<pair<TedNode*, TedNode*> > & arg_candidate_list) {
		TedNode* pNode = NULL,* pKid = NULL;
		wedge weight = 0;
		unsigned int index = 0;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;
		map <TedNode*, vector<pair <TedNode*, pair<unsigned int, wedge> > > > linkofnodes;
		map <TedNode*, vector<pair <TedNode*, pair<unsigned int, wedge> > > >::iterator it_linkofnodes;
		vector<pair< TedNode*, pair<unsigned int,wedge> > >::iterator it_parent;
		vector<pair<TedNode*, TedNode*> > it_vector;

		arg_candidate_list.clear();	 //nettoyage de la liste

		/*creation des references*/
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					weight = _iterkids.getWeight();
					linkofnodes[pKid].push_back(pair<TedNode*, pair<unsigned int,wedge> >(pNode, pair<unsigned int,wedge>(index,weight)));
				}
			}
		}
		string parent;
		TedNode* node_parent;
		TedNode* node_children;
		bool sameparent;
		unsigned int type_edge;
		string children;
		int nbref;
		/* pour chacun des enfants*/
		for(it_linkofnodes=linkofnodes.begin();it_linkofnodes!=linkofnodes.end();it_linkofnodes++) {
			children=it_linkofnodes->first->getName();
			node_children=it_linkofnodes->first;
			/*cond initiale*/
			sameparent=true;
			nbref=0;
			it_parent=it_linkofnodes->second.begin();
			parent=it_parent->first->getName();
			node_parent=it_parent->first;
			type_edge=it_parent->second.first;
			/*lister les parents*/
			for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++) {
				nbref++;
				if(it_parent->first->getName()!=parent||it_parent->second.first!=type_edge) {
					sameparent=false;
					break;
				}
			}
			//si tout les parents sont de mme var et avec le mm arc:
			if(sameparent==true&&nbref>1&&type_edge!=0) {
				//reprendre tt les parents pour dump dans vector
				for(it_parent=it_linkofnodes->second.begin();it_parent!=it_linkofnodes->second.end();it_parent++) {
					node_parent=it_parent->first;
					arg_candidate_list.push_back(pair<TedNode*, TedNode*>(node_parent,node_children));
				}

			}
		}
		pNode=NULL;

	}

	void TedMan::getAssociatedNodes(TedNode* parentnode, vector<string> &AssociatedNodes) {
		//GetAssociated node is a recursive function the AssociatedNodes.clear()has to be called
		//before calling this function, not here
		wedge weight = 0;
		unsigned int index = 0;
		TedNode* pKid = NULL;
		if(parentnode!=TedNode::getOne()) {
			AssociatedNodes.push_back(parentnode->getName());
			FOREACH_KID_OF_NODE(parentnode) {
				index = _iterkids.getIndex();
				pKid = _iterkids.Node<TedNode>();
				weight = _iterkids.getWeight();
				getAssociatedNodes(pKid,AssociatedNodes);
			}
		}
	}

	/** @brief Return the list of candidates to CSE*/
	void TedMan::getCandidateList(TedCandidates& arg_candidate_list) {
		TedNode* pNode,* pKid;
		//int weight;
		//unsigned int index;
		map <TedNode*, vector<TedNode* > > mParents;
		map <TedNode*, vector<TedNode* > >::iterator itmParents;

		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;

		arg_candidate_list.clear();
		/*creating the references*/
		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					//index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					//weight = _iterkids.getWeight();
					mParents[pKid].push_back(pNode);
				}
			}
		}

		for(itmParents= mParents.begin(); itmParents!=mParents.end();itmParents++)
		{
			if(itmParents->second.size()<= 1) {continue; }
			else if(itmParents->first == TedNode::getOne()|| itmParents->first->isBasic()) {continue; }
			//else if(itmParents->first == TedNode::getOne()|| isSimpleVariable(itmParents->first)) {continue; }
			else {    arg_candidate_list.insert(pair<TedNode*, unsigned int>(itmParents->first, itmParents->second.size())); }
		}
		//To print the list of candidates
		/*map <TedNode*, unsigned int>::iterator it_list;
		  for(it_list=arg_candidate_list.begin();it_list!=arg_candidate_list.end();it_list++)
		  {
		  cout<<"Node added to the list"<< it_list->first->getName()<<endl;
		  }*/
	}

	TedMan* TedMan::bottomscse(void) {
		TedMan* pMan = this->duplicate();
		static unsigned int indexsubexpr = 0;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;
		vector<pair< TedNode*, TedNode* > > list_factor_cand;
		TedCandidates::iterator it_best_found;
		/*used to store the parents of a candidate*/
		vector <TedNode*> vparentnodes;

		unsigned int maxreference=1;
		unsigned int minlevel = 0;
		TedNode* bestnodefound = NULL,* pNodeNew = NULL,*pKid = NULL,*tempnode = NULL;
		string candidatename;
		unsigned int toplevel = getMaxLevelWoConst();
		wedge weight = 0;
		unsigned int index = 0;
		vector <string> AssociatedNodes;
		vector <string> creatednodeS;
		cout<<"Performing bottom static CSE into the TED"<<endl;
		cout<<"Find multiple parent nodes and substitute"<<endl<<"the subgraph by intermediate node S"<<endl;

		pMan->getCandidateList(list_cand);
		while(list_cand.size()!=0) { //tant qu'il y a des facteurs possible:
			cout<<"Still "<<list_cand.size()<<"candidate to factorize"<<endl;
			/*Remise  zero compteur et ptr bestnodefound*/
			bestnodefound=NULL;
			maxreference=1;
			minlevel=toplevel;
			/*recherche du candidat qui se trouve le plus bas sans faire attention au nombre de parent*/
			for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
				tempnode=it_list_cand->first;
				if((pMan->getLevel(tempnode)<minlevel)) {
					bestnodefound= it_list_cand->first;
					minlevel=pMan->getLevel(tempnode);
					it_best_found=it_list_cand;
				}
			}
			cout<<"The best candidate found is: "<<bestnodefound->getName()<<endl;

			if(bestnodefound!=NULL) {
				candidatename=bestnodefound->getName();
				//For all the candidate
				for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
					TedNode* pcandid;
					pcandid=it_list_cand->first;
					//look for its parents
					pMan->getParentNodes(pcandid, vparentnodes);
					/*These nodes are strored into vparentnodes*/
					indexsubexpr++;
					//create an indexed string S
					string strNewName = string("S")+ Util::itoa(indexsubexpr);
					// add the indexed S nodes to the manager
					pNodeNew = pMan->getNode(strNewName);
					//vecteur de noeud S cr afin de pouvoir faire le realoc
					creatednodeS.push_back(strNewName);
					//Pour tout les noeuds parents
					for(unsigned int i=0;i<vparentnodes.size();i++) {				 //pour chaque enfant
						FOREACH_KID_OF_NODE(vparentnodes.at(i)) {
							index = _iterkids.getIndex();
							pKid = _iterkids.Node<TedNode>();
							weight = _iterkids.getWeight();
							//si l'arc pointe vers le candidat
							if(pKid==pcandid) {
								// le faire pointer vers le nouveau noeud sous expression
								weight=vparentnodes.at(i)->getKidWeight(index);
								vparentnodes.at(i)->replaceKid(index, pNodeNew, weight);
							}
						}
					}
					//lier la sousexpression au noeud S
					cout<<"Subexpression "<<strNewName<<"="<<pcandid->getName();
					FOREACH_KID_OF_NODE(pcandid) {
						index = _iterkids.getIndex();
						pKid = _iterkids.Node<TedNode>();
						weight = _iterkids.getWeight();
						if(index==0&&weight>0)
							cout<<"+";
						if(index==0&&weight<0)
							cout<<"-";
						if(index==1&&weight>0)
							cout<<"*";
						if(index==1&&weight<0)
							cout<<"*-";
						cout<<pKid->getName();
					}
					cout<<endl;
					//to add the subexpression to the TED please remove the //
					pMan->linkTedToPO(pcandid, 1, strNewName);
					toplevel++;		 //remove the // if subexpression have to be added to the TED
					// remove also // for the same reason than above
					pMan->setConstLimit(toplevel);
					vparentnodes.clear();
				}

			}
			//placer les noeud S entre const et variable
			/*for(unsigned int nb=0;nb<creatednodeS.size();nb++)
			  {
			  pManNew=pMan->relocateVariable(creatednodeS.at(nb).c_str(),toplevel-nb);
			  delete pMan;
			  pMan=pManNew;
			  }*/
			pMan->getCandidateList(list_cand);
		}
		return pMan;
	}


	TedMan* TedMan::scse(void) {
		TedMan* pMan=this->duplicate();
		static unsigned int indexsubexpr = 0;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;
		vector<pair< TedNode*, TedNode* > > list_factor_cand;
		TedCandidates::iterator it_best_found;
		/*used to store the parents of a candidate*/
		vector <TedNode*> vparentnodes;

		unsigned int maxreference=1;
		unsigned int minlevel = 0;
		TedNode* bestnodefound = NULL,*pNodeNew = NULL,*pKid=NULL ,*tempnode=NULL;
		string candidatename;
		unsigned int toplevel = getMaxLevelWoConst();
		wedge weight = 0;
		unsigned int index = 0;
		vector <string> AssociatedNodes;
		vector <string> creatednodeS;
		cout<<"Performing static CSE into the TED"<<endl;
		cout<<"Find multiple parent nodes and substitute"<<endl<<"the subgraph by intermediate node S"<<endl;

		pMan->getCandidateList(list_cand);
		while(list_cand.size()!=0) { //tant qu'il y a des facteurs possible:
			cout<<"Still "<<list_cand.size()<<"candidate to factorize"<<endl;
			/*Remise  zero compteur et ptr bestnodefound*/
			bestnodefound=NULL;
			maxreference=1;
			minlevel=toplevel;
			/*recherche du candidat qui a le plus de reference dans la liste*/
			for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
				tempnode=it_list_cand->first;
				if((it_list_cand->second>maxreference)||((it_list_cand->second>=maxreference)&&(pMan->getLevel(tempnode)<minlevel))) {
					maxreference=it_list_cand->second;
					bestnodefound= it_list_cand->first;
					minlevel=pMan->getLevel(tempnode);
					it_best_found=it_list_cand;
				}
			}
			cout<<"The best candidate found is: "<<bestnodefound->getName()<<endl;

			if(bestnodefound!=NULL) {
				candidatename=bestnodefound->getName();
				//For all the candidate
				for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
					TedNode* pcandid;
					pcandid=it_list_cand->first;
					//look for its parents
					pMan->getParentNodes(pcandid, vparentnodes);
					/*These nodes are strored into vparentnodes*/
					indexsubexpr++;
					//create an indexed string S
					string strNewName = string("S")+ Util::itoa(indexsubexpr);
					// add the indexed S nodes to the manager
					pNodeNew = pMan->getNode(strNewName);
					//vecteur de noeud S cr afin de pouvoir faire le realoc
					creatednodeS.push_back(strNewName);
					//Pour tout les noeuds parents
					for(unsigned int i=0;i<vparentnodes.size();i++) {				 //pour chaque enfant
						FOREACH_KID_OF_NODE(vparentnodes.at(i)) {
							index = _iterkids.getIndex();
							pKid = _iterkids.Node<TedNode>();
							weight = _iterkids.getWeight();
							//si l'arc pointe vers le candidat
							if(pKid==pcandid) {
								// le faire pointer vers le nouveau noeud sous expression
								weight=vparentnodes.at(i)->getKidWeight(index);
								vparentnodes.at(i)->replaceKid(index, pNodeNew, weight);
								//vparentnodes.at(i)->setKid(index, pNodeNew, weight);
							}
						}
					}
					//lier la sousexpression au noeud S
					cout<<"Subexpression "<<strNewName<<"="<<pcandid->getName();
					FOREACH_KID_OF_NODE(pcandid) {
						index = _iterkids.getIndex();
						pKid = _iterkids.Node<TedNode>();
						weight = _iterkids.getWeight();
						if(index==0&&weight>0)
							cout<<"+";
						if(index==0&&weight<0)
							cout<<"-";
						if(index==1&&weight>0)
							cout<<"*";
						if(index==1&&weight<0)
							cout<<"*-";
						cout<<pKid->getName();
					}
					cout<<endl;
					//to add the subexpression to the TED please remove the //
					pMan->linkTedToPO(pcandid, 1, strNewName);
					toplevel++;		 //remove the // if subexpression have to be added to the TED
					// remove also // for the same reason than above
					pMan->setConstLimit(toplevel);
					vparentnodes.clear();
				}

			}
			//placer les noeud S entre const et variable
			/*for(unsigned int nb=0;nb<creatednodeS.size();nb++)
			  {
			  pManNew=pMan->relocateVariable(creatednodeS.at(nb).c_str(),toplevel-nb);
			  delete pMan;
			  pMan=pManNew;
			  }*/
			pMan->getCandidateList(list_cand);
		}
		return pMan;
	}


	/** @brief Print the candidate to CSE*/
	void TedMan::printCandidates(void) {
		cout<<"Looking for candidate to CSE into the TED"<<endl;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;

		//call the function that look for multiple referenced nodes
		this->getCandidateList(list_cand);
		//print the list
		for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
			cout<<it_list_cand->first->getName()<<" # reference: "<<it_list_cand->second<<endl;
		}

	}


	TedMan* TedMan::bottomdcse(void) {
		TedMan* pMan = NULL,* pManNew = NULL;
		pMan=this->duplicate();
		static unsigned int indexsubexpr = 0;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;
		vector<pair< TedNode*, TedNode* > > list_factor_cand;
		TedCandidates::iterator it_best_found;
		/*used to store the parents of a candidate*/
		vector <TedNode*> vparentnodes;

		unsigned int maxreference=1;
		unsigned int minlevel = 0;
		TedNode* bestnodefound = NULL,* pNodeNew=NULL,*pKid=NULL,*tempnode=NULL;
		string candidatename;
		unsigned int toplevel = getMaxLevelWoConst();
		wedge weight = 0;
		unsigned int index = 0;
		vector <string> AssociatedNodes;
		vector <string> creatednodeS;
		cout<<"Performing CSE into the TED"<<endl;

		//call the function that look for multiple referenced nodes
		pMan->getCandidateList(list_cand);
		//print the list
		/*for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
		  cout<<it_list_cand->first->getName()<<" # reference: "<<it_list_cand->second<<endl;
		  }*/
		bestnodefound=NULL;
		while(list_cand.size()!=0) { //tant qu'il y a des facteurs possible:
			cout<<"Still "<<list_cand.size()<<"candidate to extract"<<endl;
			/*Remise  zero compteur et ptr bestnodefound*/
			bestnodefound=NULL;
			maxreference=1;
			minlevel=toplevel;
			/*recherche du candidat qui a le plus de reference dans la liste*/
			for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
				tempnode=it_list_cand->first;
				if((pMan->getLevel(tempnode)<minlevel)) {
					bestnodefound= it_list_cand->first;
					minlevel=pMan->getLevel(tempnode);
					it_best_found=it_list_cand;
				}
			} //cout<<"The best candidate found is: "<<bestnodefound->getName()<<endl;

			if(bestnodefound!=NULL) {
				candidatename=bestnodefound->getName();

				/*search for associated node*/
				//clean the vector as specified in the comment for the next function
				AssociatedNodes.clear();
				pMan->getAssociatedNodes(bestnodefound, AssociatedNodes);
				for(unsigned int i=0; i<AssociatedNodes.size();i++) {
					string var=AssociatedNodes.at(i);
					pManNew=pMan->relocateVariable(var.c_str(), 1);
					delete pMan;
					pMan = pManNew;
				}
				/*Look for new candidate*/
				pMan->getCandidateList(list_cand);
				/*print the candidate*/
				creatednodeS.clear();
				//For all the candidate
				for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
					//if the candidate has its variable equal to the previous name
					if(it_list_cand->first->getName() ==candidatename) {
						TedNode* pcandid;
						pcandid=it_list_cand->first;
						//look for its parents
						pMan->getParentNodes(pcandid, vparentnodes);
						/*These nodes are strored into vparentnodes*/
						indexsubexpr++;
						//create an indexed string S
						string strNewName = string("S")+ Util::itoa(indexsubexpr);
						// add the indexed S nodes to the manager
						pNodeNew = pMan->getNode(strNewName);
						//vecteur de noeud S cree afin de pouvoir faire le realoc
						creatednodeS.push_back(strNewName);

						//Pour tout les noeuds parents
						for(unsigned int i=0;i<vparentnodes.size();i++) {			 //pour chaque enfant
							FOREACH_KID_OF_NODE(vparentnodes.at(i)) {//si l'arc pointe vers le candidat
								index = _iterkids.getIndex();
								pKid = _iterkids.Node<TedNode>();
								weight = _iterkids.getWeight();
								if(pKid==pcandid) {// le faire pointer vers le nouveau noeud sous expression
									weight=vparentnodes.at(i)->getKidWeight(index);
									vparentnodes.at(i)->replaceKid(index, pNodeNew, weight);
								}
							}
						}
						//lier la sousexpression au noeud S
						cout<<"Subexpression "<<strNewName<<"="<<pcandid->getName();
						FOREACH_KID_OF_NODE(pcandid) {
							index = _iterkids.getIndex();
							pKid = _iterkids.Node<TedNode>();
							weight = _iterkids.getWeight();
							if(index==0&&weight>0)
								cout<<"+";
							if(index==0&&weight<0)
								cout<<"-";
							if(index==1&&weight>0)
								cout<<"*";
							if(index==1&&weight<0)
								cout<<"*-";
							cout<<pKid->getName();
						}
						cout<<endl;
						// add the subexpression to the TED please remove the

						toplevel++;	 //remove the // if subexpression have to be added to the TED
						// remove also // for the same reason than above
						pMan->setConstLimit(toplevel);
						vparentnodes.clear();
						cout<<"subexpression "<<strNewName<<" completed"<<endl;
					}
				}

			}

			//placer les noeud S entre const et variable
			for(unsigned int nb=0;nb<creatednodeS.size();nb++) {
				pManNew=pMan->relocateVariable(creatednodeS.at(nb).c_str(),toplevel-nb);
				delete pMan;
				pMan=pManNew;
			}
			pMan->getCandidateList(list_cand);
			if(list_cand.size() ==0) { //si aucune sous expressions alors on recherche des factorisations
				cout<<"End of CSE"<<endl;
				cout<<"Searching factorization"<<endl;
				list_factor_cand.clear();
				//look for multiple MPY edges or multiple ADD edges
				pMan->searchFactorizableNode(list_factor_cand);
				for(unsigned int nb=0; nb<list_factor_cand.size(); nb++) {
					cout<<list_factor_cand[nb].first->getName()<<" reference: "<<list_factor_cand[nb].second->getName()<<endl;
				}

				if(list_factor_cand.size() ==0) {
					cout<<"No factorization found"<<endl;
				}
				if(list_factor_cand.size()!=0) {
					cout<<"Can factorize"<<endl;
					//pMan->writeDot();
					AssociatedNodes.clear();
					pMan->getAssociatedNodes(list_factor_cand[0].first, AssociatedNodes);
					cout<<" Associated nodes of: "<<list_factor_cand[0].first->getName()<<endl;
					for(unsigned int i=0; i<AssociatedNodes.size();i++) {
						cout<<AssociatedNodes.at(i)<<endl;
						string var=AssociatedNodes.at(i);
						pManNew=pMan->relocateVariable(var.c_str(), 1);
						delete pMan;
						pMan = pManNew;
					}
					cout<<"End of factorization reordering"<<endl;
					//pMan->writeDot();

				}
				list_cand.clear();
				assert(pMan);
				pMan->getCandidateList(list_cand);
			}
		}
		cout<<"top level"<<toplevel<<endl;
		return pMan;
	}


	/** @brief Performs CSE on a TED*/
	TedMan* TedMan::dcse(void) {
		TedMan* pMan=NULL,* pManNew=NULL;
		pMan=this->duplicate();
		static unsigned int indexsubexpr = 0;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;
		vector<pair< TedNode*, TedNode* > > list_factor_cand;
		vector<pair< TedNode*, TedNode* > > list_decomp_node;//not in jer
		list<TedNode*> listcycle; //added
		vector<TedNode* > list_add_cand;
		TedCandidates::iterator it_best_found;
		/*used to compute relocation time*/
		double clkin;
		double clkdiff=0;
		/*used to store the parents of a candidate*/
		vector <TedNode*> vparentnodes;
		vector <TedNode*> vparentnodes1;
		vector <TedNode*> vparentnodes2;
		ofstream ofile;
		ofile.open("subfile.txt");
		unsigned int minlevelfound;
		TedNode* bestnodefound=NULL,* pNodeNew=NULL,*pKid=NULL,*tempnode=NULL;
		string candidatename;
		unsigned int toplevel = getMaxLevelWoConst();
		wedge weight=0;
		unsigned int index=0;
		vector <string> AssociatedNodes;
		vector <string> creatednodeS;
		cout<<"Performing CSE into the TED"<<endl;

/*		TedNode* D11;
		TedNode* D12;
		TedNode* D21;
		TedNode* D22;
		TedNode* X;
		TedNode* Y;
*/
		string D11str, D12str, D21str,D22str, Xstr, Ystr;
		//call the function that look for multiple referenced nodes
		pMan->getCandidateList(list_cand);
		//print the list
		/*for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
		  cout<<it_list_cand->first->getName()<<" # reference: "<<it_list_cand->second<<endl;
		  }*/
		bestnodefound=NULL;
		while(list_cand.size()!=0)//tant qu'il y a des facteurs possible:
		{							 //cout<<"Still "<<list_cand.size()<<"candidate to factorize"<<endl;
			/*Remise  zero compteur et ptr bestnodefound*/
			bestnodefound=NULL;
			minlevelfound=pMan->getLevel(list_cand.begin()->first);
			/*recherche du candidat qui a le plus de reference dans la liste*/
			for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
				tempnode=it_list_cand->first;
				if((pMan->getLevel(tempnode)<=minlevelfound)) {
					bestnodefound= it_list_cand->first;
					minlevelfound=pMan->getLevel(tempnode);
					it_best_found=it_list_cand;
				}
			}

			if(bestnodefound==NULL)
				cout<<"Can't choose the best candidate"<<endl;

			if(bestnodefound!=NULL) {
				candidatename=bestnodefound->getName();

				/*search for associated node*/
				//clean the vector as specified in the comment for the next function
				AssociatedNodes.clear();
				pMan->getAssociatedNodes(bestnodefound, AssociatedNodes);
				for(unsigned int i=0; i<AssociatedNodes.size();i++) {
					string var=AssociatedNodes.at(i);
					//relocate at the bottom
					clkin=Util::getCpuTime();
					pManNew=pMan->relocateVariable(var.c_str(), 1);
					clkdiff=clkdiff+(Util::getCpuTime()-clkin);
					delete pMan;
					pMan = pManNew;
				}
				/*Look for new candidate*/
				pMan->getCandidateList(list_cand);
				creatednodeS.clear();
				//For all the candidate
				for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
					//if the candidate has its variable equal to the previous name
					if(it_list_cand->first->getName() ==candidatename) {
						TedNode* pcandid;
						pcandid=it_list_cand->first;
						//look for its parents
						pMan->getParentNodes(pcandid, vparentnodes);
						/*These nodes are strored into vparentnodes*/
						indexsubexpr++;
						//create an indexed string S
						string strNewName = string("S")+ Util::itoa(indexsubexpr);
						// add the indexed S nodes to the manager
						pNodeNew = pMan->getNode(strNewName);
						//vecteur de noeud S cr afin de pouvoir faire le realoc
						creatednodeS.push_back(strNewName);

						//Pour tout les noeuds parents
						for(unsigned int i=0;i<vparentnodes.size();i++) {			 //pour chaque enfant
							FOREACH_KID_OF_NODE(vparentnodes.at(i)) {
								index = _iterkids.getIndex();
								pKid = _iterkids.Node<TedNode>();
								weight = _iterkids.getWeight();
								//si l'arc pointe vers le candidat
								if(pKid==pcandid) {
									// le faire pointer vers le nouveau noeud sous expression
									weight=vparentnodes.at(i)->getKidWeight(index);
									//vparentnodes.at(i)->setKid(index, pNodeNew, weight);
									vparentnodes.at(i)->replaceKid(index, pNodeNew, weight);
								}
							}
						}
						//lier la sousexpression au noeud S
						cout<<"Subexpression "<<strNewName<<"="<<pcandid->getName();
						ofile<<strNewName<<"="<<pcandid->getName();
						FOREACH_KID_OF_NODE(pcandid) {
							index = _iterkids.getIndex();
							pKid = _iterkids.Node<TedNode>();
							weight = _iterkids.getWeight();
							if(index==0&&weight>0) {
								ofile<<"+";
								cout<<"+";
							}
							if(index==0&&weight<0) {
								ofile<<"-";
								cout<<"-";
							}
							if(index==1&&weight>0) {
								ofile<<"*";
								cout<<"*";
							}
							if(index==1&&weight<0) {
								ofile<<"*-";
								cout<<"*-";
							}
							ofile<<pKid->getName();
							cout<<pKid->getName();
						}
						ofile<<endl;
						cout<<endl;
						//to add the subexpression to the TED please remove the //
						pMan->linkTedToPO(pcandid, 1, strNewName);
						toplevel++;	 //remove the // if subexpression have to be added to the TED
						// remove also // for the same reason than above
						pMan->setConstLimit(toplevel);
						vparentnodes.clear();
						//cout<<"subexpression "<<strNewName<<" completed"<<endl;
					}
				}

			}
			//placer les noeud S entre const et variable
			for(unsigned int nb=0;nb<creatednodeS.size();nb++) {
				clkin=Util::getCpuTime();
				pManNew=pMan->relocateVariable(creatednodeS.at(nb).c_str(),toplevel-nb);
				clkdiff=clkdiff+(Util::getCpuTime()-clkin);
				delete pMan;
				pMan=pManNew;
			}
			pMan->getCandidateList(list_cand);
			if(list_cand.size() ==0) { //si aucune sous expressions alors on recherche des factorisations
				cout<<"End of CSE"<<endl;
				cout<<"Searching redundant multiplication"<<endl;
				list_factor_cand.clear();
				//look for multiple MPY edges or multiple ADD edges
				pMan->searchFactorizableNode(list_factor_cand);
				if(list_factor_cand.size() ==0)cout<<"No redundant multiplication found"<<endl;
				for(unsigned int nb=0; nb<list_factor_cand.size(); nb++) {
					cout<<list_factor_cand[nb].first->getName()<<" reference: "<<list_factor_cand[nb].second->getName()<<endl;
				}

				if(list_factor_cand.size()!=0) {
					clkin=Util::getCpuTime();
					string strnode1,strnode2;
					strnode1=list_factor_cand[0].first->getName();
					strnode2=list_factor_cand[0].second->getName();
					pManNew=pMan->relocateVariable(strnode1.c_str(), 1);
					clkdiff=clkdiff+(Util::getCpuTime()-clkin);
					delete pMan;
					pMan = pManNew;
					assert(pMan);
					clkin=Util::getCpuTime();
					pManNew=pMan->relocateVariable(strnode2.c_str(), 1);
					clkdiff=clkdiff+(Util::getCpuTime()-clkin);
					delete pMan;
					pMan = pManNew;
					assert(pMan);
				}
				list_cand.clear();
				assert(pMan);

				//the following two lines allows to enter in the next if condition
				pMan->getCandidateList(list_cand);
				pMan->searchFactorizableNode(list_factor_cand);
				cout<<"End of reduction of redundant multiplications"<<endl;
			}
			if((list_factor_cand.size() ==0)&&(list_cand.size() ==0)) {
				unsigned int absolute_max_level=pMan->_container.getMaxLevel();
				cout<<"Start searching pure additive nodes"<<endl;
				pMan->searchadditiveNode(list_add_cand);
				if(list_add_cand.size() ==0)cout<<"No purely additive nodes found"<<endl;
				else {
					cout<<" there is "<<list_add_cand.size()<<" purely additive nodes"<<endl;
					//dump des string dans un tableaux pour le relocate
					vector<string> tempstring;
					tempstring.resize(list_add_cand.size());
					for(unsigned int i=0;i<list_add_cand.size();i++) {
						tempstring[i]=list_add_cand.at(i)->getName();
						cout<<tempstring[i]<<" is purely additive and will be placed on top"<<endl;
					}
					for(unsigned int i=0;i<list_add_cand.size();i++) {
						clkin=Util::getCpuTime();
						pManNew=pMan->relocateVariable(tempstring[i].c_str(), absolute_max_level);
						clkdiff=clkdiff+(Util::getCpuTime()-clkin);
						delete pMan;
						pMan = pManNew;
						assert(pMan);
					}
					cout<<"End of relocation of purely additive nodes."<<endl;

					list_add_cand.clear();
					pMan->searchadditiveNode(list_add_cand);

					if(list_add_cand.size() ==0)cout<<"No more purely additive node found"<<endl;
					else {
						cout<<"WARNING: other additive node detected please report it with complete description to jguillot@univ-ubs.fr"<<endl;
					}
				}

			}
			if((list_factor_cand.size() ==0)&&(list_cand.size() ==0)&&(list_add_cand.size() ==0)) {
				cout<<"Start to search multiplication reference classes"<<endl;
				pMan->searchmpyclass(list_decomp_node);
			}
		}
		cout<<"top level"<<toplevel<<endl;
		ofile.close();
		cout<<"CLK DIFF"<<clkin<<endl;
		return pMan;
	}

	TedMan* TedMan::maxdcse(void) {
		TedMan* pMan=NULL,* pManNew=NULL;
		pMan=this->duplicate();
		static unsigned int indexsubexpr = 0;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;
		vector<pair< TedNode*, TedNode* > > list_factor_cand;
		TedCandidates::iterator it_best_found;
		/*used to store the parents of a candidate*/
		vector <TedNode*> vparentnodes;

		unsigned int maxreference=1;
		unsigned int minlevel=0;
		TedNode* bestnodefound=NULL,* pNodeNew=NULL,*pKid=NULL,*tempnode=NULL;
		string candidatename;
		unsigned int toplevel = getMaxLevelWoConst();
		wedge weight=0;
		unsigned int index=0;
		vector <string> AssociatedNodes;
		vector <string> creatednodeS;
		cout<<"Performing CSE into the TED"<<endl;

		//call the function that look for multiple referenced nodes
		pMan->getCandidateList(list_cand);
		//print the list
		/*for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
		  cout<<it_list_cand->first->getName()<<" # reference: "<<it_list_cand->second<<endl;
		  }*/
		bestnodefound=NULL;
		while(list_cand.size()!=0)//tant qu'il y a des facteurs possible:
		{							 //cout<<"Still "<<list_cand.size()<<"candidate to factorize"<<endl;
			/*Remise  zero compteur et ptr bestnodefound*/
			bestnodefound=NULL;
			maxreference=1;
			minlevel=toplevel;
			/*recherche du candidat qui a le plus de reference dans la liste*/
			for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
				tempnode=it_list_cand->first;
				if((it_list_cand->second>maxreference)) {
					maxreference=it_list_cand->second;
					bestnodefound= it_list_cand->first;
					minlevel=pMan->getLevel(tempnode);
					it_best_found=it_list_cand;
				}
			}						 //cout<<"The best candidate found is: "<<bestnodefound->getName()<<endl;

			if(bestnodefound!=NULL) {
				candidatename=bestnodefound->getName();

				/*search for associated node*/
				//clean the vector as specified in the comment for the next function
				AssociatedNodes.clear();
				pMan->getAssociatedNodes(bestnodefound, AssociatedNodes);
				for(unsigned int i=0; i<AssociatedNodes.size();i++) {
					string var=AssociatedNodes.at(i);
					pManNew=pMan->relocateVariable(var.c_str(), 1);
					delete pMan;
					pMan = pManNew;
				}
				/*Look for new candidate*/
				pMan->getCandidateList(list_cand);
				creatednodeS.clear();
				//For all the candidate
				for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
					//if the candidate has its variable equal to the previous name
					if(it_list_cand->first->getName() ==candidatename) {
						TedNode* pcandid;
						pcandid=it_list_cand->first;
						pMan->getParentNodes(pcandid, vparentnodes);
						/*These nodes are strored into vparentnodes*/
						indexsubexpr++;
						string strNewName = string("S")+ Util::itoa(indexsubexpr);
						// add the indexed S nodes to the manager
						pNodeNew = pMan->getNode(strNewName);
						//vecteur de noeud S cr afin de pouvoir faire le realoc
						creatednodeS.push_back(strNewName);

						//Pour tout les noeuds parents
						for(unsigned int i=0;i<vparentnodes.size();i++) {			 //pour chaque enfant
							FOREACH_KID_OF_NODE(vparentnodes.at(i)) {
								index = _iterkids.getIndex();
								pKid = _iterkids.Node<TedNode>();
								weight = _iterkids.getWeight();
								//si l'arc pointe vers le candidat
								if(pKid==pcandid) {
									// le faire pointer vers le nouveau noeud sous expression
									weight=vparentnodes.at(i)->getKidWeight(index);
									vparentnodes.at(i)->setKid(index, pNodeNew, weight);
									//vparentnodes.at(i)->ReplaceKid(index, pNodeNew, weight);
								}
							}
						}
						//lier la sousexpression au noeud S
						cout<<"Subexpression "<<strNewName<<"="<<pcandid->getName();
						FOREACH_KID_OF_NODE(pcandid) {
							index = _iterkids.getIndex();
							pKid = _iterkids.Node<TedNode>();
							weight = _iterkids.getWeight();
							if(index==0&&weight>0)
								cout<<"+";
							if(index==0&&weight<0)
								cout<<"-";
							if(index==1&&weight>0)
								cout<<"*";
							if(index==1&&weight<0)
								cout<<"*-";
							cout<<pKid->getName();
						}
						cout<<endl;
						//to add the subexpression to the TED please remove the //
						pMan->linkTedToPO(pcandid, 1, strNewName);
						toplevel++;	 //remove the // if subexpression have to be added to the TED
						// remove also // for the same reason than above
						pMan->setConstLimit(toplevel);
						vparentnodes.clear();
					}
				}

			}
			//placer les noeud S entre const et variable
			for(unsigned int nb=0;nb<creatednodeS.size();nb++) {
				pManNew=pMan->relocateVariable(creatednodeS.at(nb).c_str(),toplevel-nb);
				delete pMan;
				pMan=pManNew;
			}
			pMan->getCandidateList(list_cand);
			if(list_cand.size() ==0) { //si aucune sous expressions alors on recherche des factorisations
				cout<<"End of CSE"<<endl;
				cout<<"Searching factorization"<<endl;
				list_factor_cand.clear();
				//look for multiple MPY edges or multiple ADD edges
				pMan->searchFactorizableNode(list_factor_cand);

				for(unsigned int nb=0; nb<list_factor_cand.size(); nb++) {
					cout<<list_factor_cand[nb].first->getName()<<" reference: "<<list_factor_cand[nb].second->getName()<<endl;
				}

				if(list_factor_cand.size()!=0) {
					cout<<"-----------------------------1"<<endl;
					string strnode1,strnode2;
					strnode1=list_factor_cand[0].first->getName();
					strnode2=list_factor_cand[0].second->getName();
					cout<<strnode1<<"  "<<strnode2<<endl;
					cout<<"Bottom "<<strnode1<<endl;
					pManNew=pMan->relocateVariable(strnode1.c_str(), 1);
					delete pMan;
					pMan = pManNew;
					assert(pMan);
					cout<<"Bottom "<<strnode2<<endl;
					pManNew=pMan->relocateVariable(strnode2.c_str(), 1);
					delete pMan;
					pMan = pManNew;
					assert(pMan);
				}
				list_cand.clear();
				cout<<"la7bis"<<endl;
				assert(pMan);
				pMan->getCandidateList(list_cand);
				cout<<"la8"<<endl;
			}
		}
		cout<<"top level"<<toplevel<<endl;
		return pMan;
	}

	TedMan* TedMan::lcse(void) {
		TedMan* pMan=NULL,* pManNew=NULL;
		pMan=this->duplicate();
		static unsigned int indexsubexpr = 0;
		TedCandidates list_cand;
		TedCandidates::iterator it_list_cand;
		vector<pair< TedNode*, TedNode* > > list_factor_cand;
		vector<pair< TedNode*, TedNode* > > list_decomp_node;
		vector<TedNode* > list_add_cand;
		TedCandidates::iterator it_best_found;
		/*used to store the parents of a candidate*/
		vector <TedNode*> vparentnodes;
		ofstream ofile;
		ofile.open("subfile.txt");
		unsigned int minlevelfound;
		TedNode* bestnodefound=NULL,* pNodeNew=NULL,*pKid=NULL,*tempnode=NULL;
		string candidatename;
		unsigned int toplevel = getMaxLevelWoConst();
		wedge weight=0;
		unsigned int index=0;
		vector <string> AssociatedNodes;
		vector <string> creatednodeS;
		cout<<"Performing special CSE for linearized TED"<<endl;

		//call the function that look for multiple referenced nodes
		pMan->getCandidateList(list_cand);
		//print the list
		/*for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
		  cout<<it_list_cand->first->getName()<<" # reference: "<<it_list_cand->second<<endl;
		  }*/
		bestnodefound=NULL;
		while(list_cand.size()!=0)//tant qu'il y a des facteurs possible:
		{							 //cout<<"Still "<<list_cand.size()<<"candidate to factorize"<<endl;
			/*Remise  zero compteur et ptr bestnodefound*/
			bestnodefound=NULL;
			minlevelfound=pMan->getLevel(list_cand.begin()->first);
			/*recherche du candidat qui a le plus de reference dans la liste*/
			for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
				tempnode=it_list_cand->first;
				if((pMan->getLevel(tempnode)<=minlevelfound)) {
					bestnodefound= it_list_cand->first;
					minlevelfound=pMan->getLevel(tempnode);
					it_best_found=it_list_cand;
				}
			}

			if(bestnodefound==NULL)
				cout<<"Can't choose the best candidate"<<endl;

			if(bestnodefound!=NULL) {
				candidatename=bestnodefound->getName();

				/*search for associated node*/
				AssociatedNodes.clear();
				pMan->getAssociatedNodes(bestnodefound, AssociatedNodes);
				for(unsigned int i=0; i<AssociatedNodes.size();i++) {
					string var=AssociatedNodes.at(i);
					pManNew=pMan->relocateVariable(var.c_str(), 1);
					delete pMan;
					pMan = pManNew;
				}
				/*Look for new candidate*/
				pMan->getCandidateList(list_cand);
				creatednodeS.clear();
				//For all the candidate
				for(it_list_cand=list_cand.begin();it_list_cand!=list_cand.end();it_list_cand++) {
					//if the candidate has its variable equal to the previous name
					if(it_list_cand->first->getName() ==candidatename) {
						TedNode* pcandid;
						pcandid=it_list_cand->first;
						pMan->getParentNodes(pcandid, vparentnodes);
						/*These nodes are strored into vparentnodes*/
						indexsubexpr++;
						//create an indexed string S
						string strNewName = string("S")+ Util::itoa(indexsubexpr);
						pNodeNew = pMan->getNode(strNewName);
						//vecteur de noeud S cr afin de pouvoir faire le realoc
						creatednodeS.push_back(strNewName);

						//Pour tout les noeuds parents
						for(unsigned int i=0;i<vparentnodes.size();i++) {			 //pour chaque enfant
							FOREACH_KID_OF_NODE(vparentnodes.at(i)) {
								index = _iterkids.getIndex();
								pKid = _iterkids.Node<TedNode>();
								weight = _iterkids.getWeight();
								//si l'arc pointe vers le candidat
								if(pKid==pcandid) {
									// le faire pointer vers le nouveau noeud sous expression
									weight=vparentnodes.at(i)->getKidWeight(index);
									//vparentnodes.at(i)->setKid(index, pNodeNew, weight);
									vparentnodes.at(i)->replaceKid(index, pNodeNew, weight);
								}
							}
						}
						//lier la sousexpression au noeud S
						cout<<"Subexpression "<<strNewName<<"="<<pcandid->getName();
						ofile<<strNewName<<"="<<pcandid->getName();
						FOREACH_KID_OF_NODE(pcandid) {
							index = _iterkids.getIndex();
							pKid = _iterkids.Node<TedNode>();
							weight = _iterkids.getWeight();
							if(index==0&&weight>0) {
								ofile<<"+";
								cout<<"+";
							}
							if(index==0&&weight<0) {
								ofile<<"-";
								cout<<"-";
							}
							if(index==1&&weight>0) {
								ofile<<"*";
								cout<<"*";
							}
							if(index==1&&weight<0) {
								ofile<<"*-";
								cout<<"*-";
							}
							ofile<<pKid->getName();
							cout<<pKid->getName();
						}
						ofile<<endl;
						cout<<endl;
						//to add the subexpression to the TED please remove the //
						pMan->linkTedToPO(pcandid, 1, strNewName);
						toplevel++;	 //remove the // if subexpression have to be added to the TED
						// remove also // for the same reason than above
						pMan->setConstLimit(toplevel);
						vparentnodes.clear();
					}
				}

			}
			//placer les noeud S entre const et variable
			/*for(unsigned int nb=0;nb<creatednodeS.size();nb++)
			  {
			  pManNew=pMan->relocateVariable(creatednodeS.at(nb).c_str(),toplevel-nb);
			  delete pMan;
			  pMan=pManNew;
			  }*/
			pMan->getCandidateList(list_cand);
			if(list_cand.size() ==0) { //si aucune sous expressions alors on recherche des factorisations
				cout<<"End of CSE"<<endl;
				cout<<"Searching redundant multiplication"<<endl;
				list_factor_cand.clear();
			}
		}
		cout<<"top level"<<toplevel<<endl;
		ofile.close();
		return pMan;
	}


	/** @brief Perform Dynamic factorization of this manager, the result is returned as a new manager*/
	TedMan* TedMan::dynFactorize(void) {
		TedMan* pMan,* pManNew;
		TedNode* pNode,* pNodeNew;
		wedge weight;
		string strNewName;
		size_t toplevel;
		TedContainer::iterator msp;
		vector <pair<TedNode*, unsigned int> >* vParents = new vector <pair<TedNode*, unsigned int> >;
		vector <string> vVars;
		pMan = this->duplicate();
		pMan->getVars(vVars);
		toplevel = getMaxLevelWoConst();
		for(unsigned int i = pMan->_container.getMaxLevel()-toplevel; i < vVars.size(); i++) {
			while((pNode = pMan->getFirstCandidate(vParents))!= NULL) {
				strNewName = string("DF")+ Util::itoa(_factor_counter++);
				TedVar* newVar = _vars.createVar(strNewName);
				//TedVar* newVar = pMan->_vars.getIfExist(strNewName);
				//if(NULL==newVar) {
				//	newVar = new TedVar(strNewName);
				//	pMan->_vars.add(newVar);
				//}
				pNodeNew = pMan->getNode(*newVar);
				//pNodeNew = pMan->getNode(strNewName.c_str());
				toplevel++;
				for(unsigned int j = 0; j < vParents->size(); j++) {
					weight = vParents->at(j).first->getKidWeight(vParents->at(j).second);
					vParents->at(j).first->setKid(vParents->at(j).second, pNodeNew, weight);
				}
				pMan->linkTedToPO(pNode, 1, strNewName);
#if 0
				// commented for consistency with print -e DFG algorithm
				// moving the variables to the top hinder us, from observing other
				// factorizations for the current ordering
				pManNew = pMan->RelocateVariable(strNewName.c_str(), toplevel);
				assert(pManNew);
				delete pMan;
				pMan = pManNew;
#endif
				vParents->clear();
			}
			pManNew = pMan->relocateVariable(vVars[vVars.size()-1-i].c_str(), toplevel);
			assert(pManNew);
			delete pMan;
			pMan = pManNew;
		}
		delete vParents;
		pMan->setConstLimit(toplevel);
		return pMan;
	}


	/** @brief Get the first Factor Candidate*/
	TedNode* TedMan::getFirstCandidate(vector <pair<TedNode*, unsigned int> >* vParients) {
		TedNode* pNode = NULL,* pKid = NULL;
		wedge weight = 0;
		unsigned int index = 0;

		map <TedNode*, vector<pair<TedNode*, unsigned int> > > mParients;
		TedContainer::iterator msp;
		set <TedNode*>::iterator sp;

		map <unsigned int, set <TedNode*> > msNodes;
		map <unsigned int, set <TedNode*> >::iterator msp2;

		for(msp = _container.begin(); msp != _container.end(); msp ++) {
			for(sp = msp->second.second->begin(); sp != msp->second.second->end(); sp++) {
				pNode =*sp;
				FOREACH_KID_OF_NODE(pNode) {
					index = _iterkids.getIndex();
					pKid = _iterkids.Node<TedNode>();
					weight = _iterkids.getWeight();
					mParients[pKid].push_back(pair<TedNode*, unsigned int>(pNode, index));
					msNodes[getLevel(pNode)].insert(pNode);
				}
			}
		}

		pNode = NULL;
		for(msp2 = msNodes.begin(); msp2 != msNodes.end(); msp2++) {
			for(sp = msp2->second.begin(); sp != msp2->second.end(); sp++) {
				if(mParients[*sp].size()<= 1) {
					continue;
				} else if(*sp == TedNode::getOne()||(*sp)->isBasic()) {
					//} else if(*sp == TedNode::getOne()|| isSimpleVariable(*sp)) {
					continue;
			} else {
				pNode =*sp;
				break;
			}
			}
			if(pNode != NULL)break;
		}

		if(pNode != NULL && vParients != NULL) {
			*vParients = mParients[pNode];
		}

		return pNode;
	}

}
