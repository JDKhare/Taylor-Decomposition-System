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

//File: Munkres.h
//Purpose: Munkres 
//Author: Dominique Heller, LabSticc, UBS

#include "munkres.h"

#include <iostream>

#define Z_NORMAL 0
#define Z_STAR 1
#define Z_PRIME 2

bool 
Munkres::find_uncovered_in_matrix(double item, int &row, int &col) {
	for ( row = 0 ; row < matrix.rows() ; row++ )
		if ( !row_mask[row] )
			for ( col = 0 ; col < matrix.columns() ; col++ )
				if ( !col_mask[col] )
					if ( matrix(row,col) == item )
						return true;

	return false;
}

bool 
Munkres::pair_in_list(const std::pair<int,int> &needle, const std::list<std::pair<int,int> > &haystack) {
	for ( std::list<std::pair<int,int> >::const_iterator i = haystack.begin() ; i != haystack.end() ; i++ ) {
		if ( needle == *i )
			return true;
	}
	
	return false;
}

int 
Munkres::step1(void) {
	for ( int row = 0 ; row < matrix.rows() ; row++ )
		for ( int col = 0 ; col < matrix.columns() ; col++ )
			if ( matrix(row,col) == 0 ) {
				bool isstarred = false;
				for ( int nrow = 0 ; nrow < matrix.rows() ; nrow++ )
					if ( mask_matrix(nrow,col) == Z_STAR )
						isstarred = true;

				if ( !isstarred ) {
					for ( int ncol = 0 ; ncol < matrix.columns() ; ncol++ )
						if ( mask_matrix(row,ncol) == Z_STAR )
							isstarred = true;
				}
							
				if ( !isstarred ) {
					mask_matrix(row,col) = Z_STAR;
				}
			}

	return 2;
}

int 
Munkres::step2(void) {
	int covercount = 0;
	for ( int row = 0 ; row < matrix.rows() ; row++ )
		for ( int col = 0 ; col < matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_STAR ) {
				col_mask[col] = true;
				covercount++;
			}
			
	int k = matrix.minsize();

	if ( covercount >= k ) {
#ifdef DEBUG
		std::cout << "Final cover count: " << covercount << std::endl;
#endif
		return 0;
	}

#ifdef DEBUG
	std::cout << "Munkres matrix has " << covercount << " of " << k << " Columns covered:" << std::endl;
	for ( int row = 0 ; row < matrix.rows() ; row++ ) {
		for ( int col = 0 ; col < matrix.columns() ; col++ ) {
			std::cout.width(8);
			std::cout << matrix(row,col) << ",";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif


	return 3;
}

int 
Munkres::step3(void) {
	/*
	Main Zero Search

   1. Find an uncovered Z in the distance matrix and prime it. If no such zero exists, go to Step 5
   2. If No Z* exists in the row of the Z', go to Step 4.
   3. If a Z* exists, cover this row and uncover the column of the Z*. Return to Step 3.1 to find a new Z
	*/
	if ( find_uncovered_in_matrix(0, saverow, savecol) ) {
		mask_matrix(saverow,savecol) = Z_PRIME; // prime it.
	} else {
		return 5;
	}

	for ( int ncol = 0 ; ncol < matrix.columns() ; ncol++ )
		if ( mask_matrix(saverow,ncol) == Z_STAR ) {
			row_mask[saverow] = true; //cover this row and
			col_mask[ncol] = false; // uncover the column containing the starred zero
			return 3; // repeat
		}

	return 4; // no starred zero in the row containing this primed zero
}

int 
Munkres::step4(void) {
	std::list<std::pair<int,int> > seq;
	// use saverow, savecol from step 3.
	std::pair<int,int> z0(saverow, savecol);
	std::pair<int,int> z1(-1,-1);
	std::pair<int,int> z2n(-1,-1);
	seq.insert(seq.end(), z0);
	int row, col = savecol;
	/*
	Increment Set of Starred Zeros

   1. Construct the ``alternating sequence'' of primed and starred zeros:

         Z0 : Unpaired Z' from Step 4.2 
         Z1 : The Z* in the column of Z0
         Z[2N] : The Z' in the row of Z[2N-1], if such a zero exists 
         Z[2N+1] : The Z* in the column of Z[2N]

      The sequence eventually terminates with an unpaired Z' = Z[2N] for some N.
	*/
	bool madepair;
	do {
		madepair = false;
		for ( row = 0 ; row < matrix.rows() ; row++ )
			if ( mask_matrix(row,col) == Z_STAR ) {
				z1.first = row;
				z1.second = col;
				if ( pair_in_list(z1, seq) )
					continue;
				
				madepair = true;
				seq.insert(seq.end(), z1);
				break;
			}

		if ( !madepair )
			break;

		madepair = false;

		for ( col = 0 ; col < matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_PRIME ) {
				z2n.first = row;
				z2n.second = col;
				if ( pair_in_list(z2n, seq) )
					continue;
				madepair = true;
				seq.insert(seq.end(), z2n);
				break;
			}
	} while ( madepair );

	for ( std::list<std::pair<int,int> >::iterator i = seq.begin() ;
		  i != seq.end() ;
		  i++ ) {
		// 2. Unstar each starred zero of the sequence.
		if ( mask_matrix(i->first,i->second) == Z_STAR )
			mask_matrix(i->first,i->second) = Z_NORMAL;

		// 3. Star each primed zero of the sequence,
		// thus increasing the number of starred zeros by one.
		if ( mask_matrix(i->first,i->second) == Z_PRIME )
			mask_matrix(i->first,i->second) = Z_STAR;
	}

	// 4. Erase all primes, uncover all columns and rows, 
	for (row = 0 ; row < mask_matrix.rows() ; row++ )
		for (col = 0 ; col < mask_matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_PRIME )
				mask_matrix(row,col) = Z_NORMAL;
	int index;
	for (index = 0 ; index < matrix.rows() ; index++ ) {
		row_mask[index] = false;
	}

	for (index = 0 ; index < matrix.columns() ; index++ ) {
		col_mask[index] = false;
	}

	// and return to Step 2. 
	return 2;
}

int 
Munkres::step5(void) {
	/*
	New Zero Manufactures

   1. Let h be the smallest uncovered entry in the (modified) distance matrix.
   2. Add h to all covered rows.
   3. Subtract h from all uncovered columns
   4. Return to Step 3, without altering stars, primes, or covers. 
	*/
	int row,col; 

	double h = 0;
	for (row = 0 ; row < matrix.rows() ; row++ ) {
		if ( !row_mask[row] ) {
			for (col = 0 ; col < matrix.columns() ; col++ ) {
				if ( !col_mask[col] ) {
					if ( (h > matrix(row,col) && matrix(row,col) != 0) || h == 0 ) {
						h = matrix(row,col);
					}
				}
			}
		}
	}

	for (row = 0 ; row < matrix.rows() ; row++ )
		for (col = 0 ; col < matrix.columns() ; col++ ) {
			if ( row_mask[row] )
				matrix(row,col) += h;

			if ( !col_mask[col] )
				matrix(row,col) -= h;
		}

	return 3;
}

void 
Munkres::solve(Matrix<double> &m) {
	int i,row,col;
	// Linear assignment problem solution
	// [modifies matrix in-place.]
	// matrix(row,col): row major format assumed.

	// Assignments are remaining 0 values
	// (extra 0 values are replaced with -1)
#ifdef DEBUG
	std::cout << "Munkres input matrix:" << std::endl;
	for (row = 0 ; row < m.rows() ; row++ ) {
		for ( col = 0 ; col < m.columns() ; col++ ) {
			std::cout.width(8);
			std::cout << m(row,col) << ",";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif

	bool notdone = true;
	int step = 1;

	this->matrix = m;
	// Z_STAR == 1 == starred, Z_PRIME == 2 == primed
	mask_matrix.resize(matrix.rows(), matrix.columns());

	row_mask = new bool[matrix.rows()];
	col_mask = new bool[matrix.columns()];
	for ( i = 0 ; i < matrix.rows() ; i++ ) {
		row_mask[i] = false;
	}

	for ( i = 0 ; i < matrix.columns() ; i++ ) {
		col_mask[i] = false;
	}

	while ( notdone ) {
		switch ( step ) {
			case 0:
				notdone = false;
				break;
			case 1:
				step = step1();
				break;
			case 2:
				step = step2();
				break;
			case 3:
				step = step3();
				break;
			case 4:
				step = step4();
				break;
			case 5:
				step = step5();
				break;
		}
	}

	// Store results
	for (row = 0 ; row < matrix.rows() ; row++ )
		for (col = 0 ; col < matrix.columns() ; col++ )
			if ( mask_matrix(row,col) == Z_STAR )
				matrix(row,col) = 0;
			else
				matrix(row,col) = -1;

#ifdef DEBUG
	std::cout << "Munkres output matrix:" << std::endl;
	for ( row = 0 ; row < matrix.rows() ; row++ ) {
		for ( col = 0 ; col < matrix.columns() ; col++ ) {
			std::cout.width(1);
			std::cout << matrix(row,col) << ",";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif

	m = matrix;

	delete [] row_mask;
	delete [] col_mask;
}
