// +--------------------------------------------------------------------------+
// | Fichier   : matrix.h                                                     |
// | Utilite   : déclaration de la Matrice.                                   |
// | Auteurs   : Guillot Jérémie                                              |
// | Creation  : 11.03.2005                                                   |
// | Remarques : aucune.                                                      |
// +--------------------------------------------------------------------------

#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <iostream>
#define uint	unsigned int


using namespace std;
///Spcifies the type of elements used by the matrix during DSP transformations
typedef struct Element_Mat {
	string name;
	float value;
	int mark;
	bool signe;
} element;

///Matrix class used to construct the DSP transformation
/**This Class provides a mean to create TED representation of
 * commonly used DSP transformations
 */
class Matrice
{
private:
	typedef element *el_ligne;
	uint n; // Nombre de lignes (1er paramètre)
	uint m; // Nombre de colonnes (2ème paramètre)

public:
	el_ligne *el;
	//les constructeurs/destructeur
	Matrice();
	Matrice(uint nl,uint nc);
	Matrice(uint nl, uint nc,  float valeur);
	//      Matrice(uint size);
	~Matrice(void);
	//les operateurs
	Matrice &operator=(const Matrice &mat);
	uint nb_colones() const;
	uint nb_lignes() const;
	element &operator()(uint nl, uint nc);
	friend ostream & operator<<(ostream &os,const Matrice &mat);
	void set_symb();
	void print_symb();
	void clean_set();

	static Matrice* dct(uint, uint);
	static Matrice* bidct(uint, uint);
	static Matrice* dst(uint, uint);
	static Matrice* dht(uint);
	static Matrice* wht(uint);
};
#endif
