/*------------------------------------------------------------------------------
  Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

  Cette licence est un accord légal conclu entre vous et l'Université de
  Bretagne Sud(UBS)concernant l'outil GAUT2.

  Cette licence est une licence CeCILL-B dont deux exemplaires(en langue
  française et anglaise)sont joints aux codes sources. Plus d'informations
  concernant cette licence sont accessibles à http://www.cecill.info.

  AUCUNE GARANTIE n'est associée à cette license !

  L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
  de GAUT2 et le distribue en l'état à des fins de coopération scientifique
  seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
  de protection de leurs données qu'il jugeront nécessaires.
  ------------------------------------------------------------------------------
  2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

  This license is a legal agreement between you and the University of
  South Britany(UBS)regarding the GAUT2 software tool.

  This license is a CeCILL-B license. Two exemplaries(one in French and one in
  English)are provided with the source codes. More informations about this
  license are available at http://www.cecill.info.

  NO WARRANTY is provided with this license.

  The University of South Britany does not provide any warranty about
  the usage of GAUT2 and distributes it "as is" for scientific cooperation
  purpose only. All users are greatly advised to provide all the necessary
  protection issues to protect their data.
  ------------------------------------------------------------------------------
  */

//	File:		map.h
//	Purpose:	MAP generic template
//	Author:		Pierre Bomel, LESTER, UBS

class MapID;
template <class ID> class MapRef;
template <class ID, class Ref> class MapRefs;

#ifndef __MAP_H__
#define __MAP_H__

#include <cstdlib>
#include <map>
#include "ref.h"
#include "check.h"
using namespace std;

//! Minimum of information to identify anything.
class MapID
{
private:
	const string _name;		//!< a unique name.
public:
	//! Create a new ID
	//! @param Input. name is the unique name.
	MapID(const string &name): _name(name)
	{
#ifdef CHECK
		mapid_create++;	// debug
#endif
	}
	//! Delete a unique ID.
	~MapID()
	{
#ifdef CHECK
		mapid_delete++;	// debug
#endif
	}
	//! Get unique name.
	string name()const
	{
		return _name;
	}
	//! Virtual print info.
	virtual void info()const = 0;	// must be defined by all daughter classes
	//! Virtual serialize.
	virtual void serialize(ofstream &f)const = 0;
};

//! Smart pointers to Data.
template <class ID>
class MapRef : public Ref<ID>
{
public:
	MapRef(ID*id): Ref<ID>(id) {} //!< all daughter classes must have the same constructor.
};

//! Data bases of MapIDs = STL maps.
//! @param Template. ID is the target class.
//! @param Template. Ref is a smart pointer to the target class.
template <class ID, class Ref>
class MapRefs : public map<string, Ref, less<string> >
{
public:
	//! Search an object in a smart map.
	//! @param Input. name is the unique name.
	//! @param Output. id will be a pointer to the object found.
	//!
	//! @result a boolean telling if the object exists(true)or not(false).
	bool search(const string &name, ID**id)const
	{
		typename map<string, Ref, less<string> >::const_iterator it;
		it =  map<string, Ref, less<string> >::find(name);
		if(it == map<string, Ref, less<string> >::end())return false;
		*id = (*it).second;
		return true;
	}
	//! Add a smart pointer to a smart map.
	//! @param Input. ref is the smart pointer.
	void add(Ref ref)
	{
		pair<typename map<string, Ref, less<string> >::iterator, bool> p = insert(make_pair(ref->name(), ref));
		if(!p.second)
		{
			cerr << "Error: MapRefs: data '" << ref->name()<< "' defined twice" << endl;
			exit(1);
		}
	}
	//! Print info.
	void info()const
	{
		for(typename map<string, Ref, less<string> >::const_iterator it = map<string, Ref, less<string> >::begin(); it != map<string, Ref, less<string> >::end(); it++)
		{
			const ID*id = (*it).second;
			id->info();
		}
	}
	//! Serialize into a file.
	void serialize(ofstream &f)const
	{
		for(typename map<string, Ref, less<string> >::const_iterator it = map<string, Ref, less<string> >::begin(); it != map<string, Ref, less<string> >::end(); it++)
		{
			const ID*id = (*it).second;
			id->serialize(f);
		}
	}
};

//! Data bases of MapIDs = STL multi-maps.
//! @param Template. ID is the target class.
//! @param Template. Ref is a smart pointer to the target class.
template <class ID, class Ref>
class MMapRefs : public multimap<string, Ref, less<string> >
{
public:
	//! Search an object in a smart map.
	//! @param Input. name is the unique name.
	//! @param Output. id will be a pointer to the object found.
	//!
	//! @result a boolean telling if the object exists(true)or not(false).
	bool search(const string &name, ID**id)const
	{
		typename multimap<string, Ref, less<string> >::const_iterator it = multimap<string, Ref, less<string> >::find(name);
		if(it == multimap<string, Ref, less<string> >::end())return false;
		*id = (*it).second;
		return true;
	}
	//! Add a smart pointer to a smart map.
	//! @param Input. ref is the smart pointer.
	void add(Ref ref)
	{
		pair<typename multimap<string, Ref, less<string> >::iterator, bool> p = insert(make_pair(ref->name(), ref));
	}
	//! Print info.
	void info()const
	{
		for(typename multimap<string, Ref, less<string> >::const_iterator it = multimap<string, Ref, less<string> >::begin(); it != multimap<string, Ref, less<string> >::end(); it++)
		{
			const ID*id = (*it).second;
			id->info();
		}
	}
	//! Serialize into a file.
	void serialize(ofstream &f)const
	{
		for(typename multimap<string, Ref, less<string> >::const_iterator it = multimap<string, Ref, less<string> >::begin(); it != multimap<string, Ref, less<string> >::end(); it++)
		{
			const ID*id = (*it).second;
			id->serialize(f);
		}
	}
};

#endif // __DATA_H__
