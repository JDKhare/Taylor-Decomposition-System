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

//	File:		matrix.cpp
//	Purpose:	Matrix Object
//	Author:	LabSticc, UBS

#include "matrix.h"
#include <algorithm>

 template <class T>
 Matrix<T>::Matrix()
{
	_rows = 0;
	_columns = 0;
	_matrix = NULL;
}

template <class T> 
Matrix<T>::Matrix(const Matrix<T> &source)
{
	if ( source._matrix != NULL )
	{
		// copy matrix
		_matrix = NULL;
		resize(source._rows, source._columns);
		for ( int i = 0 ; i < m_rows ; i++ )
			for ( int j = 0 ; j < m_columns ; j++ )
				_matrix[i][j] = source._matrix[i][j];
	}
	else
	{
		_rows = 0;
		_columns = 0;
		_matrix = NULL;
	}
}

template <class T>
 Matrix<T>::Matrix(int rows, int columns) {
	_matrix = NULL;
	resize(rows, columns);
}

template <class T>
Matrix<T> & Matrix<T>::operator= (const Matrix<T> &other)
{
	if (other._matrix != NULL)
	{
		// copy matrix
		resize(other._rows, other._columns);
		for ( int i = 0 ; i < m_rows ; i++ )
			for ( int j = 0 ; j < m_columns ; j++ )
				_matrix[i][j] = other._matrix[i][j];
	}
	else
	{
		// free matrix
		for (int i = 0 ; i < m_columns ; i++)
			delete [] _matrix[i];
		delete [] _matrix;
		_matrix = NULL;
		_rows = 0;
		_columns = 0;
	}
	return *this;
}

template <class T>
Matrix<T>::~Matrix() {
	if (_matrix != NULL)
	 {
		// free arrays matrix
		for ( int i = 0 ; i < m_rows ; i++ )
			delete [] _matrix[i];
		delete [] _matrix;
	}
	_matrix = NULL;
}

template <class T>
void Matrix<T>::resize(int rows, int columns)
{
	if (_matrix == NULL)
	{
		// alloc rows 
		_matrix = new T*[rows];
		for ( int i = 0 ; i < rows ; i++ )
			// alloc columns
			_matrix[i] = new T[columns];
		_rows = rows;
		_columns = columns;
		clear();
	}
	else
	{
		// save array pointer
		T **new_matrix;
		// alloc new arrays
		new_matrix = new T*[rows]; // rows
		for ( int i = 0 ; i < rows ; i++ ) {
			new_matrix[i] = new T[columns]; // columns
			for ( int j = 0 ; j < columns ; j++ )
				new_matrix[i][j] = 0;
		}
		// copy data from saved pointer to new arrays
		int minrows = std::min<int>(rows, m_rows);
		int mincols = std::min<int>(columns, m_columns);
		for ( int x = 0 ; x < minrows ; x++ )
			for ( int y = 0 ; y < mincols ; y++ )
				new_matrix[x][y] = _matrix[x][y];

		// delete old arrays
		if ( _matrix != NULL ) {
			for ( int i = 0 ; i < _rows ; i++ )
				delete [] _matrix[i];
			delete [] _matrix;
		}
		_matrix = new_matrix;
	}
	_rows = rows;
	_columns = columns;
}

template <class T>
void Matrix<T>::identity()
 {
	if ( _matrix != NULL )
	{
		clear();
		int x = std::min<int>(_rows, _columns);
		for ( int i = 0 ; i < x ; i++ )
			_matrix[i][i] = 1;
	}
}

template <class T>
void Matrix<T>::clear() {
	if (_matrix != NULL )
		for ( int i = 0 ; i < _rows ; i++ )
			for ( int j = 0 ; j < _columns ; j++ )
				_matrix[i][j] = 0;
}

template <class T>
T  Matrix<T>::trace() {

	T value = 0;
	if (_matrix != NULL )
	{
		int x = std::min<int>(_rows, _columns);
		for ( int i = 0 ; i < x ; i++ )
			value += _matrix[i][i];
	}
	return value;
}

template <class T>
Matrix<T> &Matrix<T>::transpose()
{
	if (( _rows > 0 ) &&( _columns > 0 ))
	{
		int new_rows = _columns;
		int new_columns = _rows;
		if ( _rows != _columns )
 		{
			// expand matrix
			int m = std::max<int>(_rows, _columns);
			resize(m,m);
		}
		for ( int i = 0 ; i < _rows ; i++ )
		{
			for ( int j = i+1 ; j < _columns ; j++ ) {
			T tmp = _matrix[i][j];
			_matrix[i][j] = _matrix[j][i];
			_matrix[j][i] = tmp;
		}
	}
	if ( new_columns != new_rows )
	{
		resize(new_rows, new_columns);
	}
	return *this;
}

template <class T>
Matrix<T>  Matrix<T>::product(Matrix<T> &other) {

	Matrix<T> out(_rows, other._columns);

	if ((_matrix != NULL) && ( other._matrix != NULL ) && ( _columns == other._rows ))
	{
		for ( int i = 0 ; i < out._rows ; i++ )
			for ( int j = 0 ; j < out._columns ; j++ )
				for ( int x = 0 ; x < _columns ; x++ )
					out(i,j) += _matrix[i][x] * other._matrix[x][j];
	}
	return out;
}

template <class T>
T& Matrix<T>::operator ()(int x, int y)
{
	if (( _matrix != NULL ) && ( x >= 0 ) && ( y >= 0 )   && ( x < _rows ) && ( y < _columns ))
		return _matrix[x][y];
	else
		return (T&)0;
}

};