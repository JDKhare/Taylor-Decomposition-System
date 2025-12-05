// +--------------------------------------------------------------------------+
// | Fichier   : Cmatrice.h                                                     |
// | Utilite   : déclaration de la Matrice.                                   |
// | Auteurs   : Guillot Jérémie                                              |
// | Creation  : 11.03.2005                                                   |
// | Remarques : aucune.                                                      |
// +--------------------------------------------------------------------------

#ifndef __CMATRIX_H__
#define __CMATRIX_H__
#include <iostream>
#define uint	unsigned int


using namespace std;
///Spcifies the type of elements used by the matrix during DSP transformations
typedef struct CElement_Mat {
    string name;
    float real;
    float imag;
    int mark;
    bool signe;
} celement;

///Matrix class used to construct the DSP transformation
/**This Class provides a mean to create TED representation of
 * commonly used DSP transformations
 */
class CMatrice
{
   private:
      typedef celement *el_ligne;
      uint n; // Nombre de lignes (1er paramètre)
      uint m; // Nombre de colonnes (2ème paramètre)

   public:
      el_ligne *el;
      //les constructeurs/destructeur
      CMatrice();
    CMatrice(uint nl,uint nc);
    CMatrice(uint nl, uint nc, float valeur_re, float valeur_im);
//      Matrice(uint size);
      ~CMatrice(void);
//les operateurs
    CMatrice &operator=(const CMatrice &mat);
    uint nb_colones() const;
    uint nb_lignes() const;
    celement &operator()(uint nl, uint nc);
    friend ostream & operator<<(ostream &os,const CMatrice &mat);
    void set_symb();
    void print_symb();
    void clean_set();

    static CMatrice* dft(uint, uint);
    static CMatrice* dftfftw(uint, uint);
};
#endif
