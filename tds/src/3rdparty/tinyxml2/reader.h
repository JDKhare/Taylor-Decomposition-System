#ifndef READER_INCLUDED
#define READER_INCLUDED

#include "tinyxml2.h"

namespace tinyxml2
{

class XMLReader : public XMLVisitor {
private:
	bool elementJustOpened;
	bool firstElement;
	int depth;
	bool processEntities;
	DynArray< const char*, 10 > stack;

	void SealElement();
public:
	XMLReader(void) : XMLVisitor(), elementJustOpened( false ), firstElement( true ), depth( 0 ), processEntities( true ) {}
	~XMLReader() {}

	virtual bool VisitEnter( const XMLDocument& /*doc*/ );

	/// Visit an element.
	virtual bool VisitEnter( const XMLElement& /*element*/, const XMLAttribute* /*firstAttribute*/ );
	/// Visit an element.
	virtual bool VisitExit( const XMLElement& /*element*/ );

	/// Visit a declaration.
	virtual bool Visit( const XMLDeclaration& /*declaration*/ );
	/// Visit a text node.
	virtual bool Visit( const XMLText& /*text*/ );
	/// Visit a comment node.
	virtual bool Visit( const XMLComment& /*comment*/ );
	/// Visit an unknown node.
	virtual bool Visit( const XMLUnknown& /*unknown*/ );

	void CloseElement();
	bool ProcessElement(const XMLElement& );
	void OpenElement( const char* name );

};

}

#endif