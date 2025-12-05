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

#ifndef READER_INCLUDED
#define READER_INCLUDED

#include <cassert>

#include "DfgMan.h"

#if _WIN32
#include "../3rdparty/tinyxml2/tinyxml2.h"
#else
#include "tinyxml2.h"
#endif

using namespace tinyxml2;

namespace dfg {

	class XMLImport : public XMLVisitor {
	private:
		static const char* XML_NODE;
		static const char* XML_EDGE;
		static const char* XML_ATTR;
		static const char* XML_KEY;
		static const char* XML_VALUE;

		static const char* ATTR_CLASS;
		static const char* ATTR_NAME;
		static const char* ATTR_NATURE;
		static const char* ATTR_PROPERTY;
		static const char* ATTR_NUM_INPUT;

		int depth;
		DynArray< const char*, 10 > stack;
		DfgMan* _dMan;

		typedef map<int, DfgNode*> DfgMapID;
		typedef map<int, string> DfgMapPOs;
		typedef map<string,DfgNode*> DfgMapName;
		DfgMapPOs PoTable;
		DfgMapID IDTable;
		DfgMapName NameTable;

		void ProcessNode(const XMLElement&);
		void ProcessEdge(const XMLElement&);

	public:
		XMLImport(DfgMan* dMan) : XMLVisitor(), depth( 0 ), _dMan(dMan) { assert(_dMan); }
		~XMLImport() { _dMan->updateDelayAndNumRefs(); }

		virtual bool VisitEnter( const XMLDocument&);

		/// Visit an element.
		virtual bool VisitEnter( const XMLElement&, const XMLAttribute*);
		virtual bool VisitExit( const XMLElement&);

		void CloseElement();
		bool ProcessElement(const XMLElement& );
		void OpenElement( const char* name );

	};

}

#endif
