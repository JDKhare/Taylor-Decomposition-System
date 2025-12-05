#include "reader.h"
#include <cassert>

namespace tinyxml2
{

bool XMLReader::VisitEnter( const XMLDocument& doc )
{
	processEntities = doc.ProcessEntities();
	return true;
}


bool XMLReader::VisitEnter( const XMLElement& element, const XMLAttribute* attribute )
{
	OpenElement( element.Name() );
	/*
	while ( attribute ) {
		attribute = attribute->Next();
	}
	*/
	return 	ProcessElement(element);
}


bool XMLReader::VisitExit( const XMLElement& )
{
	CloseElement();
	return true;
}


bool XMLReader::Visit( const XMLText& text )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	firstElement = false;
	return true;
}


bool XMLReader::Visit( const XMLComment& comment )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	firstElement = false;	
	return true;
}

bool XMLReader::Visit( const XMLDeclaration& declaration )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	firstElement = false;
	return true;
}


bool XMLReader::Visit( const XMLUnknown& unknown )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	firstElement = false;
	return true;
}

void XMLReader::OpenElement( const char* name )
{
	if ( elementJustOpened ) {
		SealElement();
	}
	stack.Push( name );
	elementJustOpened = true;
	firstElement = false;
	++depth;
}


void XMLReader::CloseElement()
{
	--depth;
	const char* name = stack.Pop();
	elementJustOpened = false;
}

bool XMLReader::ProcessElement(const XMLElement& element)
{
	if(!strcmp(element.Name(),"node")) {
		enum NodeType {DATA,OP,ERROR};
		NodeType nodeT = ERROR;
		const char* nodeName = NULL;
		const char* nodeClass = NULL;
		const char* nodeNature = NULL;
		int id = element.IntAttribute("id");
		for ( const XMLNode* node=element.FirstChild(); node; node=node->NextSibling() )
		{
			const XMLElement* enode = node->ToElement();
			if (enode) {
				assert(!strcmp(enode->Name(),"attr"));	
				if (!strcmp(enode->Attribute("key"),"CLASS")){
					nodeClass = enode->Attribute("value");
				} else if (!strcmp(enode->Attribute("key"),"NAME")){
					nodeName = enode->Attribute("value");
				} else if (!strcmp(enode->Attribute("key"),"NATURE")){
					nodeNature = enode->Attribute("value");
				}
			}
		}
		return false;
	} else if (!strcmp(element.Name(),"edge")) {
		int id_pred = element.IntAttribute("id_pred");
		int id_succ = element.IntAttribute("id_succ");
		const char* edgeW = NULL;
		for ( const XMLNode* node=element.FirstChild(); node; node=node->NextSibling() )
		{
			const XMLElement* enode = node->ToElement();
			if (enode) {
				assert(!strcmp(enode->Name(),"attr"));	
				if (!strcmp(enode->Attribute("key"),"NUM_INPUT")){
					edgeW = enode->Attribute("value");
				}
			}
		}
		return false;
	} else {
		return true;
	}
}

void XMLReader::SealElement()
{
	elementJustOpened = false;
}

}
