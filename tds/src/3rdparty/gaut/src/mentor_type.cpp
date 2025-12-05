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

//	File:		mentor_type.cpp
//	Purpose:	mentor type ac_int and ac_fixed
//	Author:		Dominique Heller, LAB-STICC, UBS#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mentor_type.h"

/* signed integer : range -(1<<bitLength-1)to(1<<bitLength-1)-1
   n= bitLength
   SignBit = bit[n-1]
   -2^n*SignBit+2^(n-2)*bit[n-2]+2^(n-3)*bit[n-3]+....+bit[0]
   */
/* convert a un signed long in signed bit vector*/

BIT_VECTOR Mentor_type::signedLong_to_signedBitVector(Slong arg,int bitLength)
{

	BIT_VECTOR result;
	int i;
	unsigned char bVal= 0;
	Slong iVal = arg;
	result= (BIT_VECTOR)calloc(bitLength,sizeof(BIT));
	if(arg < 0)
	{
		bVal = 1;
		iVal = -(arg+1);
	}
	for(i=0;i<bitLength;i++)
	{
		if(iVal % 2 == 0)//pas optimal utilise le complément à 2
			result[i] = bVal;
		else
			result[i] = !bVal;
		iVal = iVal>>1;
	}
	return result;
}

/* convert a unsigned long in a unsigned bit vector*/
BIT_VECTOR Mentor_type::unsignedLong_to_unsignedBitVector(Ulong arg,int bitLength)
{
	BIT_VECTOR result;
	int i;
	Ulong iVal = arg;
	long testbit = 1;

		result= (BIT_VECTOR)calloc(bitLength,sizeof(BIT));
	for(i=0;i<bitLength;i++)
	{
		if(iVal % 2 == 0)//pas optimal utilise le complément à 2
			result[i] = 0;
		else
			result[i] = 1;
		iVal = iVal>>1;
	}
	return result;
}

/* convert a unsigned bit vector in unsigned long
   2^(bitLength-1)*bit[bitLength-1]+2^(bitLength-2)*bit[bitLength-2]+....+bit[0]*/
Ulong Mentor_type::unsignedBitVector_to_unsignedLong(BIT_VECTOR bitvector,int bitLength)
{
	int i;
	Ulong result=0;
	for(i=bitLength-1; i>=0;i--)
	{
		result += result;
		if(bitvector[i] == 1)result++;
	}
	return result;
}

/* -2^n*SignBit+2^(n-2)*bit[n-2]+2^(n-3)*bit[n-3]+....+bit[0]*/
Slong Mentor_type::signedBitVector_to_signedLong(BIT_VECTOR bitvector,int bitLength)
{
	int i;
	Slong result=0;
	for(i=bitLength-2; i>=0;i--)
	{
		result += result;
		if(bitvector[i] == 1)result++;
	}
	if(bitvector[bitLength-1]!=0)
		result=result-(1<<(bitLength-1));
	return result;
}

AC_INT Mentor_type::signedLong_to_acInt(Slong arg,int bitLength)
{
	AC_INT result;
	Slong MAX= (1<<(bitLength-1))-1;
	Slong MIN=-1<<(bitLength-1);

	result.bitLength=bitLength;
	result.signedMode=SIGNED_M;
	result.bitvector=signedLong_to_signedBitVector(arg,bitLength);
	if(arg>MAX)
		fprintf(stdout,"!!! Warning : overflow(vector truncated)\n");
	else if(arg<MIN)
		fprintf(stdout,"!!! Warning : underflow(vector truncated)\n");
	return result;
}

/* unsigned integer : range 0 to 1<<bitLength -1*/
AC_INT Mentor_type::unsignedLong_to_acInt(Ulong arg, int bitLength)
{
	AC_INT result;
	Ulong MAX= (1<<bitLength)-1;
	result.bitLength=bitLength;
	result.signedMode=UNSIGNED_M;
	result.bitvector=unsignedLong_to_unsignedBitVector(arg,bitLength);
	if(arg>MAX)fprintf(stdout,"!!! Warning : overflow(vector truncated)\n");
	return result;
}

Ulong Mentor_type::acInt_to_unsignedLong(AC_INT arg)
{

	Ulong result=0;
	if((arg.signedMode==0)||(arg.bitvector[arg.bitLength-1]==0))
		result=unsignedBitVector_to_unsignedLong(arg.bitvector,arg.bitLength);
	else
		fprintf(stdout,"!!! Warning : Overflow\n");
	return result;
}

Slong Mentor_type::acInt_to_signedLong(AC_INT arg)
{
	Slong result=0;
	result=signedBitVector_to_signedLong(arg.bitvector,arg.bitLength);
	return result;
}

/* signed fixed point :
   signedMode == UNSIGNED
   range O TO(1 - 2 ^(-bitLength))* 2 ^ fixedPointPositionFromMSB
   (b[w-1]....b[1]b[0])2 ^(fixedPointPositionFromMSB-bitLength)
   where b[w-1]....b[1]b[0] is a unsigned integer number
   signedMode == SIGNED
   range(-O.5)2 ^ fixedPointPositionFromMSB  TO(0.5 - 2 ^(-bitLength))* 2 ^ fixedPointPositionFromMSB
   (b[w-1]....b[1]b[0])2 ^(fixedPointPositionFromMSB-bitLength)
   where b[w-1]....b[1]b[0] is a signed integer number
   */
/* AC_WRAP : Any MSB bits outside the range are deleted(modulo)
AC_SAT : MAX if overflow
MIN if underflow
AC_SAT_SYM : MAX if overflow
-MAX if underflow
AC_SAT_ZEO  O if overflow
O if underflow*/

bool Mentor_type::saturate(AC_FIXED*result,long double arg)
{
	int integerPartSize=result->fixedPointPositionFromMSB; //size of integer part
	int fractionalPartSize=result->bitLength-result->fixedPointPositionFromMSB; ///size of fractional part
	bool saturate = false;
	long double overflow_level;
	long double underflow_level;
	BIT_VECTOR MAX,MIN,MOINSMAX;
	char overflow,underflow;
	int i;

	if(result->signedMode==UNSIGNED_M)
	{
		overflow_level= (1<<integerPartSize)-1.0/(1<<(fractionalPartSize));//MAX
		underflow_level=0.0;//MIN
		overflow= (arg > overflow_level);
		underflow= (arg < underflow_level);
		MIN= (BIT_VECTOR)calloc(result->bitLength,sizeof(BIT));
		/* Ulong ulong_arg= (Ulong)(overflow_level*(1<<fractionalPartSize));
		   MAX=unsignedLong_to_unsignedBitVector(ulong_arg,result.bitLength);*/
		MAX= (BIT_VECTOR)calloc(result->bitLength,sizeof(BIT));
		for(i=result->bitLength-1; i>=0;i--)
			MAX[i] =1;
		MOINSMAX=MIN;
	}
	else
	{
		overflow_level= (1<<(integerPartSize-1))-1.0/(1<<(fractionalPartSize)); //MAX
		underflow_level=-(1<<(integerPartSize-1)); //MIN
		overflow= (arg > overflow_level);
			underflow= (arg < underflow_level);
			MIN= (BIT_VECTOR)calloc(result->bitLength,sizeof(BIT));
			MIN[result->bitLength-1]=1;//sign bit
		MAX= (BIT_VECTOR)calloc(result->bitLength,sizeof(BIT));
			MAX[result->bitLength-1]=0;//sign bit
		for(i=result->bitLength-2; i>=0;i--)
			MAX[i] =1;
				MOINSMAX= (BIT_VECTOR)calloc(result->bitLength,sizeof(BIT));
				MOINSMAX[result->bitLength-1]=1;//sign  bit
		MOINSMAX[0]=1; //+1
	}
	if(((overflow)||(underflow))&&(result->overflowMode==AC_SAT_ZERO))
	{
		//ZERO
		result->bitvector= (BIT_VECTOR)calloc(result->bitLength,sizeof(BIT));
			saturate = true;
			fprintf(stderr,"Saturate to ZERO\n");
	}
	else if((overflow)&&((result->overflowMode==AC_SAT)||(result->overflowMode==AC_SAT_SYM)))
	{
		saturate = true;
			//MAX
			result->bitvector=MAX;
			fprintf(stderr,"Saturate to MAX\n");
	}
	else if(underflow)
	{
		saturate = true;
			if(result->overflowMode==AC_SAT)
			{
				//MIN
				result->bitvector=MIN;
				fprintf(stderr,"Saturate to MIN\n");
			}
			else
			{
				//-MAX
				fprintf(stderr,"Saturate to -MAX\n");
				result->bitvector=MOINSMAX;
			}
	}
	return saturate;
}

/* AC_TRN : delete bits
   AC_RND,
   AC_TRN_ZERO,
   AC_RND_ZERO,
   AC_RND_INF,
   AC_RND_MIN_INF,
   AC_RND_CONV*/

bool Mentor_type::quantization(AC_FIXED result,long double*value)
{
	int fractionalPartSize=result.bitLength-result.fixedPointPositionFromMSB; ///size of fractional part
	long scale = 1<<fractionalPartSize;
	long double val= (*value)*scale;
	double integerPart;
	double fractionalPart=modf(val,&integerPart);
	bool needRounding= (fractionalPart != 0.0);
	if(needRounding)
	{
		if(result.quantizationMode==AC_TRN)

		{
			//truncation
			if(*value<0.0)val-=1.0;
		}
		else if(result.quantizationMode==AC_RND)
		{
			//rounding to plus infinity
			if(fractionalPart >=0.5)
				val+=1.0;
			else if(fractionalPart <=-0.5)
				val-=1.0;
		}
		else if(result.quantizationMode==AC_TRN_ZERO)
		{
			//truncation to zero
			//nothing todo
		}
		else if(result.quantizationMode==AC_RND_INF)
		{
			//rounding to infinity
			if(fractionalPart >=0.5)
				val+=1.0;
			else if(fractionalPart <=-0.5)
				val-=1.0;
		}
		else if(result.quantizationMode==AC_RND_CONV)
		{
			//convergent rounding
			if(fractionalPart > 0.5 ||
			   fractionalPart == 0.5 && fmod(integerPart, 2.0)!= 0.0)
				val += 1.0;
			else if(fractionalPart < -0.5 ||
					fractionalPart == -0.5 && fmod(integerPart, 2.0)!= 0.0)
				val -= 1.0;
		}
		else if(result.quantizationMode==AC_RND_ZERO)
		{
			//rounding to zero
			if(fractionalPart >0.5)
				val+=1.0;
			else if(fractionalPart <-0.5)
				val-=1.0;
		}
		else if(result.quantizationMode==AC_RND_MIN_INF)
		{
			//rounding to minus infinity
			if(fractionalPart > 0.5)
				val += 1.0;
			else if(fractionalPart <= -0.5)
				val -= 1.0;
		}
		val /= scale;
		*value= val;
	}
	return needRounding;
}

AC_FIXED Mentor_type::double_to_acFixed(long double arg, //real
										int bitLength, //word length
										int fixedPointPositionFromMSB,
										Signed_mode signedMode,
										Ac_q_mode quantizationMode,
										Ac_o_mode overflowMode)
{
	int integerPartSize=fixedPointPositionFromMSB; //size of integer part
	int fractionalPartSize=bitLength-fixedPointPositionFromMSB; ///size of fractional part
	char overflow,underflow,needRounded;
	Slong slong_arg;
	Ulong ulong_arg;
	int i;
	AC_FIXED result;
	bool overflowQuantization=false;
	long double value=arg;
	/* 	fprintf(stdout, "%.20Lf\n",1E-18);
		fprintf(stdout, "%.20Lf\n",atof("3.800000000000000001"));
		fprintf(stdout, "size of float = %d\n",sizeof(float));
	//23 bits pour la MANTISSE
	//8 bits pour l'expossant
	//1 bit de signed
	//atod pas défino sous VC++ fprintf(stdout, "%.20Lf\n",atod("3.800000000000000001"));
	fprintf(stdout, "%.20Lf\n",3.800000000000000001l);
	fprintf(stdout, "%.20Lf\n",arg);*/
	//init
	result.bitLength=bitLength;
	result.fixedPointPositionFromMSB=fixedPointPositionFromMSB;
	result.signedMode=signedMode;
	result.quantizationMode=quantizationMode;
	result.overflowMode=overflowMode;
	if(!saturate(&result,arg))
	{
		//WRAP or nor overflow : rounding ???
		if(quantization(result,&value))
		{
			overflowQuantization=saturate(&result,value);//saturation after rounding
		}
		if(overflowQuantization==false)
		{
			if(signedMode==UNSIGNED_M)
			{
				ulong_arg= (Ulong)(value*(1<<fractionalPartSize));
				result.bitvector=unsignedLong_to_unsignedBitVector(ulong_arg,bitLength);
			}
			else
			{
				//signedMode==SIGNED
				slong_arg= (Slong)(value*(1<<fractionalPartSize));
				result.bitvector=signedLong_to_signedBitVector(slong_arg,bitLength);
			}
		}
	}
	return result;
}

double Mentor_type::acFixed_to_double(AC_FIXED arg)
{
	long integerPartSize=arg.fixedPointPositionFromMSB; //size of integer part
	long fractionalPartSize=arg.bitLength-arg.fixedPointPositionFromMSB; ///size of fractional part
	long scale = 1<<fractionalPartSize;
	double result;

	if(arg.signedMode==UNSIGNED_M)
		result= (double)unsignedBitVector_to_unsignedLong(arg.bitvector,arg.bitLength)/(double)scale;
	else
		result= (double)signedBitVector_to_signedLong(arg.bitvector,arg.bitLength)/(double)scale;
	return result;
}

Ac_q_mode Mentor_type::getQuantizationMode(char*mode)
{
	Ac_q_mode result=AC_TRN;

	if(strcmp(mode,"AC_RND") ==0)
		result=AC_RND;
	else if(strcmp(mode,"AC_TRN_ZERO") ==0)
		result=AC_TRN_ZERO;
	else if(strcmp(mode,"AC_RND_ZERO") ==0)
		result=AC_RND_ZERO;
	else if(strcmp(mode,"AC_RND_INF") ==0)
		result=AC_RND_INF;
	else if(strcmp(mode,"AC_RND_MIN_INF") ==0)
		result=AC_RND_MIN_INF;
	else if(strcmp(mode,"AC_RND_CONV") ==0)
		result=AC_RND_CONV;
	return result;
}

Ac_o_mode Mentor_type::getOverflowMode(char*mode)
{
	Ac_o_mode result=AC_WRAP;

		if(strcmp(mode,"AC_SAT") ==0)
			result=AC_SAT;
		else if(strcmp(mode,"AC_SAT_ZERO") ==0)
			result=AC_SAT_ZERO;
		else if(strcmp(mode,"AC_SAT_SYM") ==0)
			result=AC_SAT_SYM;
	return result;

}

char*Mentor_type::zerobv(int bitLength)
{
	int i;
	char*result= (char*)calloc(bitLength+3,sizeof(char));
	result[0]='"';
	for(i=1;i<=bitLength;i++)
		result[i]='0';
	result[bitLength+1]='"';
	result[bitLength+2]='\0';
	return result;
}



char*Mentor_type::bvtoa(BIT_VECTOR bv,int bitLength)
{
	int i;

		char*result= (char*)calloc(bitLength+3,sizeof(char));
	result[0]='"';
	for(i=1;i<=bitLength;i++)
	{
		if(bv[i-1]==0)result[bitLength-i+1]='0';
		else result[bitLength-i+1]='1';
	}
	result[bitLength+1]='"';
	result[bitLength+2]='\0';
	return result;

}

long Mentor_type::bvtoi(BIT_VECTOR bv,int bitLength,Signed_mode signedMode)
{
	long result;

		if(signedMode==UNSIGNED_M)
			result=unsignedBitVector_to_unsignedLong(bv,bitLength);
		else
			result=signedBitVector_to_signedLong(bv,bitLength);
	return result;
}

double Mentor_type::bvtod(BIT_VECTOR bv,int bitLength,int fixedPointPositionFromMSB,SIGNED_MODE signedMode)
{
	long fractionalPartSize=bitLength-fixedPointPositionFromMSB; ///size of fractional part
	long scale = 1<<fractionalPartSize;
	double result;

	if(signedMode==UNSIGNED_M)
		result= (double)unsignedBitVector_to_unsignedLong(bv,bitLength)/(double)scale;
	else
		result= (double)signedBitVector_to_signedLong(bv,bitLength)/(double)scale;
	return result;
}


/* convert 0b000.0001 to dec to simulation debug*/
long Mentor_type::fixed_todec(char*binnumber,int bitLength, Signed_mode signedMode)
{

		//initialize bit vector with binnumber
		BIT_VECTOR bitvector;
		bitvector= (BIT_VECTOR)calloc(bitLength,sizeof(BIT));
		long result=0;

		int i;
		//skip 0b prefix
		while((*binnumber)&&(*binnumber!='b'))binnumber++;
			if((*binnumber)&&(*binnumber=='b'))binnumber++;
				//convert binary number ascii representation to bitvector
				if(*binnumber)
				{
					for(i=bitLength-1;i>=0;i--)
					{
						//skip point
						if((*binnumber)&&(*binnumber=='.'))binnumber++;
							if((*binnumber)&&(*binnumber=='1'))
							{
								bitvector[i]=1;
									binnumber++;
							}
							else if((*binnumber)&&(*binnumber=='0'))
							{
								bitvector[i]=0;
									binnumber++;
							}
							else
							{
								bitvector[i]=0;
							}
					}
					result=Mentor_type::bvtoi(bitvector,bitLength,signedMode);
				}
	return result;
}

BIT_VECTOR Mentor_type::atobv(char*value,int bitLength, Signed_mode signedMode, int fixedpoint,Ac_q_mode quantizationMode,Ac_o_mode overflowMode)
{
	BIT_VECTOR result;

	if(fixedpoint==-1)
	{
		//AC_INT
		AC_INT ac_int;
		if(signedMode==SIGNED_M)
		{
			ac_int=signedLong_to_acInt(atol(value),bitLength);
		}
		else
		{
			char*p;

				unsigned long ul = strtoul(value,&p,0);
			ac_int=unsignedLong_to_acInt(ul,bitLength);
		}
		result=ac_int.bitvector;
	}
	else
	{
		//AC_FIXED
		long double ld;

		AC_FIXED ac_fixed;
		sscanf(value,"%Lg",&ld);
		ac_fixed=double_to_acFixed(ld,bitLength,fixedpoint,signedMode,quantizationMode,overflowMode);
		result=ac_fixed.bitvector;
	}
	return result;
}

BIT_VECTOR Mentor_type::extend(int wordlength,BIT_VECTOR bv, int bvlength,Signed_mode signedMode)
{
	int i;
	BIT_VECTOR result;

		result= (BIT_VECTOR)calloc(wordlength,sizeof(BIT));;
	for(i=0;i<bvlength && i<wordlength;i++)
	{
		result[i]=bv[i];
	}
	if(signedMode==SIGNED_M)
	{
		for(i=bvlength;i<wordlength;i++)
		{
			result[i]=bv[bvlength-1];//sign bit duplication
		}
	}
	return result;
}
