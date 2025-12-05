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

//	File:		function.h
//	Purpose:	Functions.
//	Author:		Pierre Bomel, LESTER, UBS

class Connection;
class ConnectionRef;
class ConnectionRefs;

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <iostream>
#include <string>
using namespace std;
#include "map.h"

//! Smart pointer on a Connection.
class ConnectionRef		: public MapRef<Connection>
{
public:
	ConnectionRef(Connection *c) : MapRef<Connection>(c) {}
};
//! List of smart pointers to Connections.
class ConnectionRefs : public MapRefs<Connection, ConnectionRef>
{
};


//! Connection.
class Connection : public MapID
{
private:
	long _nb;						//!< each Connection has a number

public:
	void *_addr;	//!< for extensions
	//! Create a new Connection.
	//! @param Input. name is the uniqu eof the Connection.
	Connection(const string & name) : MapID(name)
	{
		_addr = 0;
		_nb = 1;
	}
	~Connection();

	//! Add a symbolic port to a function.
	//! @param Input. name is the unique name of the port.
	//! @param Input. input is a boolean, true = input port, false = output port.
	//! @param Input. pos is the position of the port (0..N-1).
	void addPort(const string &name, bool input, int pos);

	long NumberOfConnections ()
	{
		return _nb;
	}
	//! ++ operator for smart pointers.
	void operator++ ()
	{
		_nb++;
	}
	//! Serialize into a file.
	void info() const
	{
		cout << name() << " : " << _nb << endl;
	}
	//! Serialize into a file.
	void serialize(ofstream &f) const
	{
		f << _nb << "\n";
	}

};
#endif // __CONNECTION_H__
