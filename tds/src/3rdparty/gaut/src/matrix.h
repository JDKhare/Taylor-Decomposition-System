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

//	File:		Matrix.h
//	Purpose:	Matrix
//	Author: 	Dominique Heller, LabSticc, UBS


#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cstdlib>
#include <algorithm>
#include <iostream>
using namespace std;


template <class T>
class Matrix {
private:
	T**_matrix;
	int _rows;
	int _columns;
public:
	inline Matrix()
	{
		_rows = 0;
		_columns = 0;
		_matrix = NULL;
	}

	inline Matrix(int rows, int columns)
	{
		_matrix = NULL;
		resize(rows, columns);
	}

	inline Matrix(const Matrix<T> &source)
	{
		if(source._matrix != NULL)
		{
			// copy matrix
			_matrix = NULL;
			resize(source._rows, source._columns);
			for(int i = 0; i < _rows; i++)
				for(int j = 0; j < _columns; j++)
					_matrix[i][j] = source._matrix[i][j];
		}
		else
		{
			_rows = 0;
			_columns = 0;
			_matrix = NULL;
		}
	}

	inline ~Matrix()
	{
		if(_matrix != NULL)
		{
			// free arrays matrix
			for(int i = 0; i < _rows; i++)
				delete [] _matrix[i];
			delete [] _matrix;
		}
		_matrix = NULL;
	}

	inline Matrix<T> & operator= (const Matrix<T> &other)
	{
		if(other._matrix != NULL)
		{
			// copy matrix
			resize(other._rows, other._columns);
			for(int i = 0; i < _rows; i++)
				for(int j = 0; j < _columns; j++)
					_matrix[i][j] = other._matrix[i][j];
		}
		else
		{
			// free matrix
			for(int i = 0; i < _columns; i++)
				delete [] _matrix[i];
			delete [] _matrix;
			_matrix = NULL;
			_rows = 0;
			_columns = 0;
		}
		return*this;
	}

	void resize(int rows, int columns)
	{
		if(_matrix == NULL)
		{
			// alloc rows
			_matrix = new T*[rows];
			for(int i = 0; i < rows; i++)
				// alloc columns
				_matrix[i] = new T[columns];
			_rows = rows;
			_columns = columns;
			clear();
		}
		else
		{
			// save array pointer
			T**new_matrix;
			// alloc new arrays
			new_matrix = new T*[rows]; // rows
			for(int i = 0; i < rows; i++) {
				new_matrix[i] = new T[columns]; // columns
				for(int j = 0; j < columns; j++)
					new_matrix[i][j] = 0;
			}
			// copy data from saved pointer to new arrays
			int minrows = rows;
			if(minrows>_rows)
				minrows=_rows;
			int mincols = columns;
			if(mincols>_columns)
				mincols=_columns;
			for(int x = 0; x < minrows; x++)
				for(int y = 0; y < mincols; y++)
					new_matrix[x][y] = _matrix[x][y];

			// delete old arrays
			if(_matrix != NULL) {
				for(int i = 0; i < _rows; i++)
					delete [] _matrix[i];
				delete [] _matrix;
			}
			_matrix = new_matrix;
		}
		_rows = rows;
		_columns = columns;
	}

	void identity(void)
	{
		if(_matrix != NULL)
		{
			clear();
			int x = std::min<int>(_rows, _columns);
			for(int i = 0; i < x; i++)
				_matrix[i][i] = 1;
		}
	}

	void clear(void)
	{
		if(_matrix != NULL)
			for(int i = 0; i < _rows; i++)
				for(int j = 0; j < _columns; j++)
					_matrix[i][j] = 0;
	}

	T& operator()(int x, int y)
	{
		//if((_matrix != NULL)&&(x >= 0)&&(y >= 0)&&(x < _rows)&&(y < _columns))
		return _matrix[x][y];
		//else
		//	return(T)0;
	}

	T trace(void)
	{
		T value = 0;
		if(_matrix != NULL)
		{
			int x = std::min<int>(_rows, _columns);
			for(int i = 0; i < x; i++)
				value += _matrix[i][i];
		}
		return value;
	}

	Matrix<T>& transpose(void)
	{
		if((_rows > 0)&&(_columns > 0))
		{
			int new_rows = _columns;
			int new_columns = _rows;
			if(_rows != _columns)
			{
				// expand matrix
				int m = std::max<int>(_rows, _columns);
				resize(m,m);
			}
			for(int i = 0; i < _rows; i++)
			{
				for(int j = i+1; j < _columns; j++)
				{
					T tmp = _matrix[i][j];
					_matrix[i][j] = _matrix[j][i];
					_matrix[j][i] = tmp;
				}
			}
			if(new_columns != new_rows)
			{
				resize(new_rows, new_columns);
			}
		}
		return*this;
	}


	Matrix<T> product(Matrix<T> &other)
	{
		Matrix<T> out(_rows, other._columns);
		if((_matrix != NULL)&&(other._matrix != NULL)&&(_columns == other._rows))
		{
			for(int i = 0; i < out._rows; i++)
				for(int j = 0; j < out._columns; j++)
					for(int x = 0; x < _columns; x++)
						out(i,j)+= _matrix[i][x]* other._matrix[x][j];
		}
		return out;
	}

	void info(void)
	{
		if(_matrix != NULL)
			for(int i = 0; i < _rows; i++)
			{
				for(int j = 0; j < _columns; j++)
				{
					cout << _matrix[i][j] << " ";
				}
				cout << endl;
			}
	}

	int minsize(void)
	{
		return((_rows < _columns) ? _rows : _columns);
	}

	int columns(void)
	{
		return _columns;
	}

	int rows(void)
	{
		return _rows;
	}

	T** matrix(void)
	{
		return _matrix;
	}

};
#endif /* !defined(_MATRIX_H_)*/

