// +--------------------------------------------------------------------------+
// | Fichier   : matrice.cpp                                                   |
// | Utilite   : définition de la Matrice.                                    |
// | Auteurs   : Jeremie Guillot                                              |
// | Creation  : 2 novembre 2005                                              |
// | Remarques : aucune.                                                      |
// +--------------------------------------------------------------------------+
#include <cmath>
#include <cstdio>
#include "matrice.h"
#include "Cmatrice.h"
#include <sstream>
#include <fstream>
#include <map>
//#include <list>
#include <set>
#ifdef _MSC_VER
# define sprintf sprintf_s
#endif

using namespace std;
#define PI 3.14159265358979323
set<float> set_value;
set<float>::iterator iter;

typedef struct {
	double real;
	double imag;
}cpl;

// pour imprimer un container de la stl
// ---------------------------------------
template<typename T>
void  show(char * s,const T & l,const char * separateur="\n")
{
	cout << s << * separateur;
	for (typename T::const_iterator i=l.begin(); i != l.end(); i++)
		cout << '\t' << * i << * separateur;
	cout << endl;
}


CMatrice::CMatrice()
{
	n=0;
	m=0;
	el=NULL;
}

/**Empty constructor of the matrice class for DSP transformation*/
Matrice::Matrice()
{
	n = 0;
	m = 0;
	el = NULL;
}

CMatrice::CMatrice(uint nl, uint nc)
{
	n=nl;
	m=nc;
	el=new el_ligne[n];
	for (uint i=0; i<n; i++)
	{
		el[i]= new celement[m];
		for(uint j=0; j<m; j++)
		{
			el[i][j].real=0;
			el[i][j].imag=0;
			el[i][j].signe=true;
			el[i][j].name="";
		}
	}
}

/**Commonly used constructor of the matrice class for DSP transformation*/
Matrice::Matrice(uint nl, uint nc)
{
	n=nl;
	m=nc;
	el = new el_ligne[n];
	for(uint i=0; i<n; i ++)
	{
		el[i] = new element[m];
		for(uint j=0; j<m; j ++)
		{el[i][j].value =0 ; //initialisation à 'valeur'
			el[i][j].mark= 0;
			el[i][j].signe= true; //signe=true means >0
			el[i][j].name="";   }

	}
}
/**Constructor to initialize the matrix with value only*/
Matrice::Matrice(uint nl, uint nc, float valeur)
{
	n = nl;
	m = nc;

	el = new el_ligne[n];
	for(uint i=0; i<n; i ++)
	{
		el[i] = new element[m];
		for(uint j=0; j<m; j ++)
			el[i][j].value = valeur; //initialisation à 'valeur'
	}
}



Matrice::~Matrice()
{
	for(uint i=0; i<n; i ++) //destruction
		delete[] el[i];

	delete[] el;
}

CMatrice::~CMatrice()
{
	for(uint i=0; i<n; i ++) //destruction
		delete[] el[i];

	delete[] el;
}



Matrice* Matrice::dct(uint nl, uint nc)
{
	//set<float> set_value;
	//set<float>::iterator iter;
	//call the constructor
	Matrice *tmp = new Matrice(nl,nc);
	tmp->n=nl;
	tmp->m=nc;
	//build the matrix with correct value and signe
	for (uint l=0;l<nl;l++)
	{
		for (uint c=0;c<nc;c++)
		{
			tmp->el[l][c].value= (float) cos(((PI/nc)*l)*(c+0.5));
			if (tmp->el[l][c].value>0)
			{
				set_value.insert(tmp->el[l][c].value);
				tmp->el[l][c].signe=true;
			}
			else {set_value.insert(-tmp->el[l][c].value);
				tmp->el[l][c].signe=false;
			}
		}
	}
	show("set S :",  set_value, "  ");
	tmp->set_symb(); //set the symbolic value according to the value
	//tmp.print_symb();
	return tmp;
}

Matrice* Matrice::bidct(uint nl, uint nc)
{
	//set<float> set_value;
	//set<float>::iterator iter;
	//call the constructor
	Matrice *tmp = new Matrice(nl*nc,nc*nl);
	tmp->n=nl*nc;
	tmp->m=nc*nl;
	uint M=nl;
	uint N=nc;
	//float value=0;
	for (uint p=0;p<M;p++)
	{
		for(uint q=0; q<N;q++)
		{
			for (uint m=0;m<M;m++)
			{
				for (uint n=0;n<N;n++)
				{
					tmp->el[(p*M)+q][(m*M)+n].value=((float)cos((PI*(2*m+1)*p)/(2*M))* (float)cos((PI*(2*n+1)*q)/(2*N)));
					if (tmp->el[(p*M)+q][(m*M)+n].value>0)
					{
						set_value.insert(tmp->el[(p*M)+q][(m*M)+n].value);
						tmp->el[(p*M)+q][(m*M)+n].signe=true;
					}
					else {
						set_value.insert(-tmp->el[(p*M)+q][(m*M)+n].value);
						tmp->el[(p*M)+q][(m*M)+n].signe=false;
					}

				}
			}
		}
	}
	/*for (uint p=0;p<M*N;p++)
	  {
	  for(uint q=0; q<N*M;q++)
	  {
	  cout<<tmp->el[p][q].value<<" ";
	  }
	  cout<<endl;
	  }*/
	//show("set S :",  set_value, "  ");
	tmp->set_symb(); //set the symbolic value (Cx) according to the real value
	//tmp.print_symb();
	return tmp;
}



/******BUILD THE DFT MATRIX AND SET THE SYMBOLIC VALUES
 * It returns the entire matrix*/


CMatrice* CMatrice::dft(uint sizel, uint sizec)
{
	CMatrice * tmp =new CMatrice(sizel,sizec);
	double coeftwid=2*PI/sizec;
	double coef_el, re, im;
	int i_re, i_im, index;
	cpl * motif = NULL;
	ofstream ofile;
	ofstream ofilesqr;
	ofile.open("coef_file.txt");
	ofilesqr.open("coef_sqr_file.txt");

	try{motif =new cpl[sizec];}
	catch (bad_alloc xa){	cout << "Allocation Failure for the DFT motif\n";}

	/*	try {tmp= new CMatrice(sizel,sizec);}
		catch (bad_alloc xa) {cout << "Allocation failure for the matrix\n";}
		*/

	/*construction of the pattern*/
	for (uint col=0;col<sizec;col++)
	{
		coef_el=coeftwid*col;
		//double re=((1./1000000.)*((int)(cos(coef_el))*1000000)); double im=((1./1000000.)*((int)(-sin(coef_el))*1000000));
		i_re=(int)((cos(coef_el))*10000000);
		i_im=(int)((-sin(coef_el))*10000000);
		re=(1./10000000.)*i_re;
		im=(1./10000000.)*i_im;
		//if (re<0.0000001 && re>-0.0000001) re=0;  //here to solve precision issue
		//if (im<0.0000001 && im>-0.0000001) im=0;
		motif[col].real=re;
		motif[col].imag=-im;
		//cout<<motif[col].real()<<"   "<<motif[col].imag()<<endl;
	}
	/*end of pattern's construction*/

	/*Fill in the matrix with the complex value*/
	index=0;
	for (uint row=0;row<sizel;row++){
		for (uint col=0;col<sizec;col++)
		{
			//index=row*sizel+col;
			tmp->el[row][col].real=(float)motif[(col*row)%sizec].real; //operateur modulo
			tmp->el[row][col].imag=(float)motif[(col*row)%sizec].imag; //operateur modulo
		}
	}
	/*End of fill in*/

	//show the matrix
	/*for (uint row=0;row<sizel;row++){
	  for (uint col=0;col<sizec;col++)
	  {
	  cout<<tmp->el[row][col].real<<"+j"<<tmp->el[row][col].imag<<" ";
	  }cout <<endl;
	  }*/


	std::ostringstream oss;
	map<int,cpl>symb_list;
	map<int,cpl>::iterator p;
	cpl val2insert;
	index=0;
	val2insert.real=tmp->el[0][0].real;
	val2insert.imag=tmp->el[0][0].imag;
	symb_list.insert(pair<int,cpl>(index,val2insert));
	//remplissage du map avec les valeurs de element de la matrice
	for (uint row=0;row<sizel;row++)
	{
		for (uint col=0;col<sizec;col++)
		{	//cout<<row<<" "<<col<<endl;
			bool el_find=false;
			//unsigned int indice=row*sizel+col;
			for (p=symb_list.begin();p!=symb_list.end()&&el_find!=true;p++)
			{
				if ((((p->second.real==tmp->el[row][col].real&&p->second.imag==tmp->el[row][col].imag)||(p->second.real==-tmp->el[row][col].real&&p->second.imag==-tmp->el[row][col].imag)))) //if the value already exist in the map
				{
					el_find=true;oss<<p->first;
					if((tmp->el[row][col].real!=0)&&(p->second.real==tmp->el[row][col].real))
					{
						tmp->el[row][col].name="C"+ oss.str();
						tmp->el[row][col].signe=true;
					}
					if((tmp->el[row][col].real==0)&&(p->second.imag==tmp->el[row][col].imag))
					{
						tmp->el[row][col].name="C"+ oss.str();
						tmp->el[row][col].signe=true;
					}
					if((tmp->el[row][col].real!=0)&&(p->second.real==-tmp->el[row][col].real))
					{
						tmp->el[row][col].name="C"+oss.str();
						tmp->el[row][col].signe=false;
					}
					if((tmp->el[row][col].real==0)&&(p->second.imag==-tmp->el[row][col].imag))
					{
						tmp->el[row][col].name="C"+oss.str();
						tmp->el[row][col].signe=false;
					}

					oss.str("");
				}
				if (((tmp->el[row][col].real==1)&&(tmp->el[row][col].imag==0))||((tmp->el[row][col].real==-1)&&(tmp->el[row][col].imag==0)))
				{
					el_find=true;
					if((tmp->el[row][col].real)==1)
					{tmp->el[row][col].name="1";tmp->el[row][col].signe=true;}
					else {tmp->el[row][col].name="1";tmp->el[row][col].signe=false;}
				}
			}
			if (el_find==false)
			{
				index++;
				val2insert.real=tmp->el[row][col].real;
				val2insert.imag=tmp->el[row][col].imag;
				symb_list.insert(pair<int,cpl>(index,val2insert));
				oss<<index;
				tmp->el[row][col].name="C"+oss.str();
				ofile<<"C"+oss.str()<<"\t"<< val2insert.real<<"\t"<<val2insert.imag<<endl;
				ofilesqr<<"C"+oss.str()<<"\t"<<(val2insert.real*val2insert.real)-(val2insert.imag*val2insert.imag)<<"\t"<<2*val2insert.real*val2insert.imag<<endl;
				tmp->el[row][col].signe=true;
				oss.str("");
				//cout<<"new value added"<<endl;
			}
		}
	}
	/*for (p=symb_list.begin();p!=symb_list.end();p++)
	  {
	  cout << p->first <<" "<<p->second.real<<" "<<p->second.imag<<endl;
	  }*/
	return tmp;
}

Matrice* Matrice::dst(uint nl, uint nc)
{
	//set<float> set_value;
	//set<float>::iterator iter;
	//call the constructor
	Matrice *tmp = new Matrice(nl,nc);
	tmp->n=nl;
	tmp->m=nc;
	//build the matrix with correct value and signe
	for (uint l=0;l<nl;l++)
	{
		for (uint c=0;c<nc;c++)
		{
			tmp->el[l][c].value= (float) sin(((PI/nc)*(l+1))*(c+0.5));
			if (tmp->el[l][c].value>0)
			{
				set_value.insert(tmp->el[l][c].value);
				tmp->el[l][c].signe=true;
			}
			else {set_value.insert(-tmp->el[l][c].value);
				tmp->el[l][c].signe=false;
			}
		}
	}
	//show("set S :",  set_value, "  ");
	tmp->set_symb(); //set the symbolic value according to the value
	//tmp.print_symb();
	cout<<endl;
	//cout<<tmp;
	return tmp;
}

Matrice* Matrice::dht(uint t)
{
	//set<float> set_value;
	//set<float>::iterator iter;
	//call the constructor
	Matrice *tmp = new Matrice(t,t);
	tmp->n=t;
	tmp->m=t;
	//build the matrix with correct value and signe
	for (uint l=0;l<t;l++)
	{
		for (uint c=0;c<t;c++)
		{

			tmp->el[l][c].value=(float) (cos(2*PI*l*c/t)+sin(2*PI*l*c/t));
			if (tmp->el[l][c].value>0)
			{
				set_value.insert(tmp->el[l][c].value);
				tmp->el[l][c].signe=true;
			}
			else {set_value.insert(-tmp->el[l][c].value);
				tmp->el[l][c].signe=false;
			}
		}
	}
	//show("set S :",  set_value, "  ");
	tmp->set_symb(); //set the symbolic value according to the value
	//tmp.print_symb();
	cout<<endl;
	//cout<<tmp;
	return tmp;
}


/**This method implements the Walsh Hadamard transform*/
Matrice* Matrice::wht(uint size)
{
	//calcul de la taille de la matrice
	int t = 1;
	for (uint u = 0; u < size; u++)
	{//printf("t=%d",t);
		t = t * 2;}

		Matrice *tmp = new Matrice(t,t); //appel du constructeur

		tmp->n=t; //mise à jour du champ n
		tmp->m=t; //mise à jour du champ m
		// creation of the butterfly
		tmp->el[0][0].value = 1;     tmp->el[0][0].mark = 1;
		tmp->el[0][0].signe = true;tmp->el[0][0].name ="1";

		tmp->el[0][1].value = 1;tmp->el[0][1].mark = 1;
		tmp->el[0][1].signe = true;tmp->el[0][1].name ="1";

		tmp->el[1][0].value = 1;tmp->el[1][0].mark = 1;
		tmp->el[1][0].signe = true;tmp->el[1][0].name ="1";

		tmp->el[1][1].value = -1;tmp->el[1][1].mark = 1;
		tmp->el[1][1].signe = false;tmp->el[1][1].name ="1";
		//end of butterfly

		for (uint round = 2; round <= size; round++)
		{ //printf ("i =%d", n);
			//printf("\n tour %d",round);
			//calcul de la taille de la matrice considéré par le round
			// affectation au round 1 effectué par init
			uint size_round = 1;
			for (uint count = 0; count < round; count++)
			{
				size_round = size_round * 2;
			}
			// printf("\n matrice en cours de taille 2^%d",round);

			//remplissage haut droit
			uint row = 0;
			uint col = 0;
			while (tmp->el[row][col].mark != 0 && row < size_round && col < size_round)
			{
				while (tmp->el[row][col].mark != 0)
				{
					//printf("Size_round=%d",size_round);
					tmp->el[row][col + size_round / 2].value = tmp->el[row][col].value;
					tmp->el[row][col + size_round / 2].signe = tmp->el[row][col].signe;
					tmp->el[row][col + size_round / 2].name  = tmp->el[row][col].name;

					//printf("\n mark[%d][%d]=%d",row,col,Mat[row][col].mark);
					col++;
				}
				col = 0;		//retour de l'indice colonne à 0
				while (col < size_round)
				{
					tmp->el[row][col].mark = 1;
					if (tmp->el[row][col].value == 0)
						tmp->el[row][col].mark = 0;
					col++;
				}
				col = 0;
				row++;
			}

			//remplissage bas droit et gauche
			row = 0;
			col = 0;
			while (tmp->el[row][col].mark != 0 && row < size_round && col < size_round)
			{
				while (tmp->el[row][col].mark != 0)
				{
					//printf("Size_round=%d",size_round);
					if (col < size_round / 2)
					{
						tmp->el[row + size_round / 2][col].value = tmp->el[row][col].value;
						tmp->el[row + size_round / 2][col].signe = tmp->el[row][col].signe;
						tmp->el[row + size_round / 2][col].name = tmp->el[row][col].name;

					}
					else{
						tmp->el[row + size_round / 2][col].value = -tmp->el[row][col].value;
						tmp->el[row + size_round / 2][col].signe = !tmp->el[row][col].signe;
						tmp->el[row + size_round / 2][col].name = tmp->el[row][col].name;
					}

					//printf("\n mark[%d][%d]=%d",row,col,Mat[row][col].mark);
					row++;
				}
				row = 0;		//retour de l'indice ligne à 0
				while (row < size_round)
				{
					tmp->el[row][col].mark = 1;
					if (tmp->el[row][col].value == 0)
						tmp->el[row][col].mark = 0;
					row++;
				}
				row = 0;
				col++;
			}
		}

		return tmp;

}

void Matrice::set_symb() {
	ofstream ofile;
	ofstream ofilesqr;
	ofile.open("coef_file.txt");
	//ofilesqr.open("coef_sqr_file.txt");
	int index;
	for (uint l=0;l<n;l++) {
		for (uint c=0;c<m;c++) {
			float val;
			if (el[l][c].value >=0) val=el[l][c].value;
			else val=-el[l][c].value; //return the absolut value
			index= 0;
			iter=set_value.begin();
			while ((*iter!=val)&&(iter!=set_value.end())) {
				index++;
				iter++;
			}
			char Coeffstring[10];
			if ( val == 1) sprintf(Coeffstring,"1");
			else if (val < 0.000000000001) sprintf(Coeffstring,"0");
			else {
				sprintf(Coeffstring,"C%d",index);
				ofile<<"C"<< index <<"\t"<< val <<endl;
				//printf("cons %s = %8.32f\n",Coeffstring,val);
			}
			el[l][c].name=Coeffstring;
		}
	}
ofile.close();
}

void Matrice::print_symb()
{
	uint i,j;
	for (i=0;i<n;i++)
	{
		for(j=0;j<m;j++) {
			if (el[i][j].signe)
				//cout << "  " << (string) el[i][j].name;
				printf("  %s",el[i][j].name.c_str());
			else
				//cout << " -" << (string) el[i][j].name;
				printf(" -%s",el[i][j].name.c_str());
		}
		cout <<endl;
	}
}

void Matrice::clean_set()
{
	set_value.clear();
}


celement & CMatrice::operator()(uint nl,uint nc)
{
	if ( nl>n || nc > m) //if out of range
	{
		cout<<" OUT OF RANGE"<< endl;
		return el[0][0];
	}
	else return el[nl][nc];
}

element & Matrice::operator()(uint nl,uint nc)
{
	if ( nl>n || nc > m) //if out of range
	{
		cout<<" OUT OF RANGE"<< endl;
		return el[0][0];
	}
	else return el[nl][nc];
}

Matrice & Matrice::operator=(const Matrice &mat)
{
	if(this != &mat) //assignation M = M impossible
	{
		if(mat.n != n || mat.m != m) // si tailles pas compatibles
		{
			for(uint i=0; i<n; i++) // on détruit...
				delete[] el[i];
			delete[] el;

			n = mat.n;
			m = mat.m;

			  el = new el_ligne[n];   // on réalloue
			for(uint i=0; i<n; i++)
				el[i] = new element[m];
		}

		for(uint i=0; i<n; i ++)
			for(uint j=0; j<m; j ++)
				el[i][j] = mat.el[i][j]; // et on copie...
	}
	return *this;
}

ostream & operator<<(ostream &os,const Matrice &mat)
{cout<<endl;
	for(uint i=0; i<mat.n; i++)
	{
		for(uint j=0; j<mat.m; j++)
			if(mat.el[i][j].value>=0)
				os <<" "<< mat.el[i][j].value << " ";
			else os << mat.el[i][j].value << " ";


		os << endl;
	}
	return os;
}

uint CMatrice::nb_colones() const
{
	return m;
}
uint CMatrice::nb_lignes() const
{
	return n;
}

uint Matrice::nb_colones() const
{
	return m;
}
uint Matrice::nb_lignes() const
{
	return n;
}
