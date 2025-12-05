/*
 * =====================================================================================
 *
 *       Filename:  Network.h
 *    Description:
 *        Created:  05/07/2007 11:56:18 AM EDT
 *         Author:  Daniel Gomez-Prado(from jan 08), Qian Ren(up to dec 07)qren@ren-chao.com
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision::                                              $: Revision of last commit
 *  $Author::                                                $: Author of last commit
 *  $Date::                                                  $: Date of last commit
 * =====================================================================================
 */

#ifndef __NETWORK_NTK_H__
#define __NETWORK_NTK_H__

#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <Node.h>

#define GAUT_VERSION "2.4.3"

/**
 * @namespace network
 * @brief A network data structure to hold structural elements of an GAUT CDFG file
 **/
namespace network {

	using namespace std;

	/**
	 * @class ltpChar
	 * @brief compare two const char*, returning true if the second string is greater
	 **/
	struct ltpChar {
		bool operator()(const char* s1, const char* s2)const {
			return(strcmp(s1, s2)<0);
		}
	};

	class Node;

	/**
	 * @class Netlist
	 * @brief Implements a structural netlist for CDFG or GC file formats.
	 **/
	class Netlist {
	private:
		static const char* temporary_prefix;
		string _name;
		long _maxLevel;
		map <const char*, Node*, ltpChar> _mNodes;
		map <Node*, vector<Node*> > _supportMap;
		map <Node*, vector<Node*> > _coneMap;

		void _collectDFS_rec(Node*, vector<Node*> &);
		Node* _AddOp(const char* name, const char* type, const char* pFanin1, const char* pFanin2);

		//Read a row of a matrix
		void _ReadMatrixRow(size_t r, string & line, const char* fv, const char* cv);

		bool _bAddingConstPrefix;
		bool _extracTed;

		class Decomposed {
		private:
			Netlist& _myself;
			void run(void);
		public:
			vector <Node*> vNodes, vConsts, vInputs, vOutputs, vVariables, vOperator, vTed, vAgings, vSource;
			map<Node*,Node*> outputTable;
			Decomposed(Netlist& that) : _myself(that) { run(); }
			~Decomposed() {
				vNodes.clear();
				vConsts.clear();
				vInputs.clear();
				vOutputs.clear();
				vVariables.clear();
				vOperator.clear();
				vTed.clear();
				vAgings.clear();
				vSource.clear();
				outputTable.clear();
			}
		};
	public:
		Netlist(const Netlist& other): _name(other._name), _maxLevel(other._maxLevel), _mNodes(other._mNodes), \
									   _supportMap(other._supportMap), _coneMap(other._coneMap), \
															_bAddingConstPrefix(other._bAddingConstPrefix), _extracTed(false) {}
		Netlist(const char* _name);
		~Netlist(void);
		const char* getName(void);

		map <const char*, Node*, ltpChar> & mapNodes(void);

		void add(Node* node);
		void remove(Node* node);
		Node* get(const char* name);
		Node* getOrCreate(const char* name, NODE_TYPE t = OPERATOR);
		Node* addOpAdd(const char* name, const char* pFanin1, const char* pFanin2);
		Node* addOpSub(const char* name, const char* pFanin1, const char* pFanin2);
		Node* addOpMul(const char* name, const char* pFanin1, const char* pFanin2);

		void updateLevel(const vector<Node*>& vNodes);
		unsigned int getMaxLevel(void);
		vector <Node*> & collectDFS(vector <Node*> &);
		void getPoPis(vector <Node*>* vPos, vector <Node*>* vPis = NULL);
		size_t size(void);
		bool empty(void) { return _mNodes.empty(); }

		//IO Commands
		CDFGData readCDFGData(ifstream& in);
		static Netlist* readCDFG(const char* file, bool inferFF = true);
		void writeCDFG(const char* file);
		void writeOpCDFG(ofstream& ofs, Node* pNode);
		void writeC(const char* file);
		void writeOpC(ofstream& ofs, Node* pNode);

		void toCombinationalNetlist(void);
		void toSequentialnetlist(void);

		void writeDot(ofstream& ofile, bool bVerbose = false);

		//Read Matrix format
		static Netlist* readMatrix(const char* filename);

		//Balancing the Network
		Netlist* balance(void);
		void getStats(int & nAdd, int & nSub, int & nMul, int & nStructural, int & nPi, int & nPo, int & nLatency, int &nReg);

		Netlist* duplicateExtract(vector<Node*>* vPos);
		Netlist* duplicateExtract(void);
		int updateDelayAndRefs(const vector<Node*>& vNodes);

		bool isExtracTed(void) { return _extracTed; }
		void setExtracTed(void) { _extracTed = true; }
		void clearExtracTed(void) { _extracTed = false; }
	};

}

#endif

