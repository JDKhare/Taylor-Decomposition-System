/*------------------------------------------------------------------------------
Janvier 2007 - La distribution de GAUT2 est régie par la licence CeCILL-B.

Cette licence est un accord légal conclu entre vous et l'Université de
Bretagne Sud (UBS) concernant l'outil GAUT2.

Cette licence est une licence CeCILL-B dont deux exemplaires (en langue
française et anglaise) sont joints aux codes sources. Plus d'informations
concernant cette licence sont accessibles à http://www.cecill.info.

AUCUNE GARANTIE n'est associée à cette license !

L'Université de Bretagne Sud ne procure aucune garantie concernant l'usage
de GAUT2 et le distribue en l'état à des fins de coopération scientifique
seulement. Tous les utilisateurs sont priés de mettre en oeuvre les mesures
de protection de leurs données qu'il jugeront nécessaires.
------------------------------------------------------------------------------
2007 January - GAUT2 distribution is done under cover of a CeCILL-B license.

This license is a legal agreement between you and the University of
South Britany (UBS) regarding the GAUT2 software tool.

This license is a CeCILL-B license. Two exemplaries (one in French and one in
English) are provided with the source codes. More informations about this
license are available at http://www.cecill.info.

NO WARRANTY is provided with this license.

The University of South Britany does not provide any warranty about
the usage of GAUT2 and distributes it "as is" for scientific cooperation
purpose only. All users are greatly advised to provide all the necessary
protection issues to protect their data.
------------------------------------------------------------------------------
*/

//	File:		ref.h
//	Purpose:	A "smart" pointer
//				that propagates relational ops to the pointed objects.
//	Author:		Pierre Bomel, LESTER, UBS

template <class T> class Ref;

#ifndef __REF_H__
#define __REF_H__

//! Template class for an "smart" pointer to a template T.
//! One can apply the usual operator == != < <= > >= to smart pointers
//! and sure the operators will be propagated to the pointed objects
//! whatever would be thier classes.
template <class T> class Ref
{
	T* _ref;	//!< The pointer itself.
public:
	// canonical contructors and destructor
	//! Create a new smart pointer.
	Ref(T* p = (T*)0)
	{
		_ref = p;
	}
	//! Copy a smart pointer.
	Ref(const Ref& r)
	{
		_ref = r._ref;
	}
	//! Assign a smart pointer.
	Ref& operator=  (const Ref& r)
	{
		_ref = r._ref;
		return *this;
	}
	//! delate a smart pointer.
	~Ref() {}
	//! == operator for smart pointers.
	bool operator== (const Ref &ref)  const
	{
		return (*_ref == *(ref._ref));
	}
	//! != operator for smart pointers.
	bool operator!= (const Ref &ref)  const
	{
		return (*_ref != *(ref._ref));
	}
	//! > operator for smart pointers.
	bool operator>  (const Ref &ref)  const
	{
		return (*_ref >  *(ref._ref));
	}
	//! >= operator for smart pointers.
	bool operator>= (const Ref &ref)  const
	{
		return (*_ref >= *(ref._ref));
	}
	//! < operator for smart pointers.
	bool operator<  (const Ref &ref)  const
	{
		return (*_ref <  *(ref._ref));
	}
	//! <= operator for smart pointers.
	bool operator<= (const Ref &ref)  const
	{
		return (*_ref <= *(ref._ref));
	}
	//! -> operator for smart pointers.
	T*   operator-> ()                const
	{
		return _ref;
	}
	//! * operator for smart pointers.
	operator T* ()				  const
	{
		return _ref;
	}
	//! Explicit delete propagation.
	void del()
	{
		if (_ref) delete _ref;
	}
	//! Validity checking
	bool valid() const
	{
		return _ref != (T*)0;
	}
};

#endif // __REF_H__
