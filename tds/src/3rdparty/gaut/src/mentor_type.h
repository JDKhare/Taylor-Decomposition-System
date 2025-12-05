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

//	File:		mentor_type.h
//	Purpose:	mentor type ac_int and ac_fixed
//	Author:		Dominique Heller, LAB-STICC, UBS


class Value;
#ifndef __MENTOR_TYPE_H__
#define __MENTOR_TYPE_H__
#include <iostream>
using namespace std;

#if defined(_MSC_VER)
typedef unsigned __int64 Ulong;
typedef signed   __int64 Slong;
#else
typedef unsigned long long Ulong;
typedef signed   long long Slong;
#endif

typedef unsigned char*BIT_VECTOR;
typedef unsigned char BIT;

//singed mode
typedef enum Signed_mode { UNSIGNED_M, SIGNED_M } SIGNED_MODE;
//quantization mode
typedef enum Ac_q_mode { AC_TRN, AC_RND, AC_TRN_ZERO, AC_RND_ZERO, AC_RND_INF, AC_RND_MIN_INF, AC_RND_CONV } QUANTIZATION_MODE;
//owerflow mode
typedef enum Ac_o_mode { AC_WRAP, AC_SAT, AC_SAT_ZERO, AC_SAT_SYM } OVERFLOW_MODE;

typedef struct ac_fixed
{
	BIT_VECTOR bitvector;
	int bitLength;					//word length
	int fixedPointPositionFromMSB;	//intger word length
	Signed_mode signedMode;			//true = signed, false = unsigned
	Ac_q_mode quantizationMode;		//quantization mode
	Ac_o_mode overflowMode;			//owerflow mode
} AC_FIXED;

typedef struct ac_int
{
	BIT_VECTOR bitvector;
	int bitLength;
	Signed_mode signedMode;
} AC_INT;

//! Mentor Type.
class Mentor_type
{
private:



	//BOOLEAN TYPE
	//enum bool_type { false, true } /*BOOLEAN*/;
	//singed mode
	//typedef enum Signed_mode { UNSIGNED, SIGNED } /*SIGNED_MODE*/;
	//quantization mode
	//typedef enum Ac_q_mode { AC_TRN, AC_RND, AC_TRN_ZERO, AC_RND_ZERO, AC_RND_INF, AC_RND_MIN_INF, AC_RND_CONV } /*QUANTIZATION_MODE*/;
	//owerflow mode
	//typedef enum Ac_o_mode { AC_WRAP, AC_SAT, AC_SAT_ZERO, AC_SAT_SYM } /*OVERFLOW_MODE*/;


	/*
AC_WRAP : Any MSB bits outside the range are deleted(modulo)
AC_SAT : MAX if overflow
MIN if underflow
AC_SAT_SYM : MAX if overflow
-MAX if underflow
AC_SAT_ZEO  O if owerflow
O if underflow
*/

	/* arrondi supérieur ceil
	   arrondi inférieur floor
	   */



	static bool saturate(AC_FIXED*result,long double arg);

	static bool quantization(AC_FIXED result,long double*value);

	static AC_FIXED double_to_acFixed(long double arg, //real
									  int bitLength, //word length
									  int fixedPointPositionFromMSB,
									  Signed_mode signedMode,
									  Ac_q_mode quantizationMode,
									  Ac_o_mode overflowMode);
	static double acFixed_to_double(AC_FIXED arg);

public:

	static BIT_VECTOR signedLong_to_signedBitVector(Slong arg,int bitLength);
	static BIT_VECTOR unsignedLong_to_unsignedBitVector(Ulong arg,int bitLength);
	static Ulong unsignedBitVector_to_unsignedLong(BIT_VECTOR bitvector,int bitLength);
	static Slong signedBitVector_to_signedLong(BIT_VECTOR bitvector,int bitLength);
	static AC_INT signedLong_to_acInt(Slong arg,int bitLength);
	static AC_INT unsignedLong_to_acInt(Ulong arg, int bitLength);
	static Ulong acInt_to_unsignedLong(AC_INT arg);
	static Slong acInt_to_signedLong(AC_INT arg);
	static Ac_q_mode getQuantizationMode(char*mode);
	static Ac_o_mode getOverflowMode(char*mode);
	static BIT_VECTOR atobv(char*value,int bitLength, Signed_mode signedMode, int fixedpoint,Ac_q_mode quantizationMode,Ac_o_mode overflowMode);
	static char*bvtoa(BIT_VECTOR bv,int bitLength);
	static long bvtoi(BIT_VECTOR bv,int bitLength, Signed_mode signedMode);
	static double bvtod(BIT_VECTOR bv,int bitLength,int fixedPointPositionFromMSB,SIGNED_MODE signedMode);
	static char*zerobv(int bitLength);
	static BIT_VECTOR extend(int wordlength,BIT_VECTOR bv, int bvlength,Signed_mode signedMode);
	static long fixed_todec(char*binnumber,int bitLength, Signed_mode signedMode);

		//!
		Mentor_type()
		{
		}

};
#endif // __MENTOR_TYPE_H__
// end of: mentor_type.h
