/* -*-mode:c++-*- *********************************************************
 *                                                                        *
 *  Algorithmic C (tm) Datatypes                                          *
 *                                                                        *
 *  Software Version: 1.1                                                 *
 *                                                                        *
 *  Release Date    : Fri Jun 30 15:34:19 PDT 2006                        *
 *  Release Type    : Production                                          *
 *  Release Build   : 1.1.1                                               *
 *                                                                        *
 *  Copyright 1996-2006, Mentor Graphics Corporation,                     *
 *                                                                        *
 *  All Rights Reserved.                                                  *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package can be downloaded from:       *
 *     http://www.mentor.com/products/c-based_design/ac_datatypes         *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 *  IMPORTANT - THIS SOFTWARE IS COPYRIGHTED AND SUBJECT TO LICENSE       *
 *  RESTRICTIONS                                                          *
 *                                                                        *
 *  THE LICENSE THAT CONTROLS YOUR USE OF THE SOFTWARE IS:                *
 *     ALGORITHMIC C DATATYPES END-USER LICENSE AGREEMENT                 *
 *                                                                        *
 *  THESE COMMENTS ARE NOT THE LICENSE.  PLEASE CONSULT THE FULL LICENSE  *
 *  FOR THE ACTUAL TERMS AND CONDITIONS WHICH IS LOCATED AT THE BOTTOM    *
 *  OF THIS FILE.                                                         *
 *                                                                        *  
 *  CAREFULLY READ THE LICENSE AGREEMENT BEFORE USING THE SOFTWARE.       *
 *                                                                        *  
 *       *** MODIFICATION OF THE SOFTWARE IS NOT AUTHORIZED ***           *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 *  YOUR USE OF THE SOFTWARE INDICATES YOUR COMPLETE AND UNCONDITIONAL    *
 *  ACCEPTANCE OF THE TERMS AND CONDITIONS SET FORTH IN THE LICENSE. IF   *
 *  YOU DO NOT  AGREE TO THE LICENSE TERMS AND CONDITIONS, DO NOT USE THE *
 *  SOFTWARE, REMOVE IT FROM YOUR SYSTEM, AND DESTROY ALL COPIES.         *
 *                                                                        *
 *************************************************************************/

/*
//  Source:         ac_fixed.h
//  Description:    fixed class for fixed point operation handling in C++
*/

#ifndef __AC_FIXED_H
#define __AC_FIXED_H

#include "ac_int.h"

#if (defined(__GNUC__) && __GNUC__ < 3 && !defined(__EDG__))
#error GCC version 3 or greater is required to include this header file
#endif

#if (defined(_MSC_VER) && _MSC_VER < 1400 && !defined(__EDG__))
#error Microsoft Visual Studio 8 or newer is required to include this header file
#endif

#if (defined(_MSC_VER) && !defined(__EDG__))
#pragma warning( push )
#pragma warning( disable: 4127 )
#endif

//////////////////////////////////////////////////////////////////////////////
//  ac_fixed 
//////////////////////////////////////////////////////////////////////////////

enum ac_q_mode { AC_TRN, AC_RND, AC_TRN_ZERO, AC_RND_ZERO, AC_RND_INF, AC_RND_MIN_INF, AC_RND_CONV };
enum ac_o_mode { AC_WRAP, AC_SAT, AC_SAT_ZERO, AC_SAT_SYM };

template<int W, int I, bool S=true, ac_q_mode Q=AC_TRN, ac_o_mode O=AC_WRAP>
class ac_fixed : private ac_private::iv<(W+31+!S)/32> {
#if defined(__SYNTHESIS__) && !defined(AC_IGNORE_BUILTINS)
#pragma builtin
#endif

  enum {N=(W+31+!S)/32};
  typedef ac_private::iv<N> Base;

  void bit_adjust();
  Base &base();
  const Base &base() const;

  void overflow_adjust(bool underflow, bool overflow);

  bool quantization_adjust(bool qb, bool r, bool s);

  bool is_neg() const;

public:
  template<int W2, int I2, bool S2>
  struct rt {
    enum {
      F=W-I, 
      F2=W2-I2,
      mult_w = W+W2,
      mult_i = I+I2,
      mult_s = S||S2,
      plus_w = MAX(I+(S2&&!S),I2+(S&&!S2))+1+MAX(F,F2),
      plus_i = MAX(I+(S2&&!S),I2+(S&&!S2))+1,
      plus_s = S||S2,
      minus_w = MAX(I+(S2&&!S),I2+(S&&!S2))+1+MAX(F,F2),
      minus_i = MAX(I+(S2&&!S),I2+(S&&!S2))+1,
      minus_s = true,
      div_w = W+MAX(W2-I2,0)+S2,
      div_i = I+(W2-I2)+S2,
      div_s = S||S2,
      logic_w = MAX(I+(S2&&!S),I2+(S&&!S2))+MAX(F,F2),
      logic_i = MAX(I+(S2&&!S),I2+(S&&!S2)),
      logic_s = S||S2
    };
    typedef ac_fixed<mult_w, mult_i, mult_s> mult;
    typedef ac_fixed<plus_w, plus_i, plus_s> plus;
    typedef ac_fixed<minus_w, minus_i, minus_s> minus;
    typedef ac_fixed<logic_w, logic_i, logic_s> logic;
    typedef ac_fixed<div_w, div_i, div_s> div;
    typedef ac_fixed<W, I, S> arg1;
  };

  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2> friend class ac_fixed;
  ac_fixed();
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed (const ac_fixed<W2,I2,S2,Q2,O2> &op);

  template<int W2, bool S2>
  ac_fixed (const ac_int<W2,S2> &op);

  template<int W2>
  ac_fixed<MAX(W+W2,1),I,S> shiftl() const;

  ac_fixed( bool b );
  ac_fixed( char b );
  ac_fixed( signed char b );
  ac_fixed( unsigned char b );
  ac_fixed( signed short b );
  ac_fixed( unsigned short b );
  ac_fixed( signed int b );
  ac_fixed( unsigned int b );
  ac_fixed( signed long b );
  ac_fixed( unsigned long b );
  ac_fixed( Slong b );
  ac_fixed( Ulong b );

   ac_fixed( double d );

  // Explicit conversion functions to ac_int that captures all integer bits (bits are truncated)
  ac_int<MAX(I,1),S> to_ac_int() const;

  // Explicit conversion functions to C built-in integral types -------------
  int to_int(); 
  unsigned to_uint();
  long to_long() const; 
  unsigned long to_ulong() const; 
  Slong to_int64() const; 
  Ulong to_uint64() const; 
  double to_double() const; 

  int length() const;

  // Arithmetic : Binary ----------------------------------------------------
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::mult operator *( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::plus operator +( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::minus operator -( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::div operator /( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  // Arithmetic assign  ------------------------------------------------------
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator *=( const ac_fixed<W2,I2,S2,Q2,O2> &op2);
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator +=( const ac_fixed<W2,I2,S2,Q2,O2> &op2);
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator -=( const ac_fixed<W2,I2,S2,Q2,O2> &op2);
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator /=( const ac_fixed<W2,I2,S2,Q2,O2> &op2);
  // increment/decrement by quantum (smallest difference that can be represented)
  // Arithmetic prefix increment, decrement ---------------------------------
  ac_fixed &operator ++();
  ac_fixed &operator --();
  // Arithmetic postfix increment, decrement ---------------------------------
  const ac_fixed operator ++(int);
  const ac_fixed operator --(int);
  // Arithmetic Unary --------------------------------------------------------
  ac_fixed operator +();
  ac_fixed<W+1,I+1,true> operator -() const;
  // ! ------------------------------------------------------------------------
  bool operator ! () const;

  // Bitwise (not arithmetic) unary: complement  -----------------------------
  ac_fixed<W+!S, I+!S, true> operator ~() const;
  // Bitwise (not arithmetic): and, or, xor ----------------------------------
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::logic operator &( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::logic operator |( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  typename rt<W2,I2,S2>::logic operator ^( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  // Bitwise assign (not arithmetic): and, or, xor ----------------------------
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator &= ( const ac_fixed<W2,I2,S2,Q2,O2> &op2 );
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator |= ( const ac_fixed<W2,I2,S2,Q2,O2> &op2 );
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  ac_fixed &operator ^= ( const ac_fixed<W2,I2,S2,Q2,O2> &op2 );
  // Shift (result constrained by left operand) -------------------------------
  template<int W2>
  ac_fixed operator << ( const ac_int<W2,true> &op2 ) const;
  template<int W2>
  ac_fixed operator << ( const ac_int<W2,false> &op2 ) const;
  template<int W2>
  ac_fixed operator >> ( const ac_int<W2,true> &op2 ) const;
  template<int W2>
  ac_fixed operator >> ( const ac_int<W2,false> &op2 ) const;
  // Shift assign ------------------------------------------------------------
  template<int W2>
  ac_fixed &operator <<= ( const ac_int<W2,true> &op2 );
  template<int W2>
  ac_fixed &operator <<= ( const ac_int<W2,false> &op2 );
  template<int W2>
  ac_fixed &operator >>= ( const ac_int<W2,true> &op2 );
  template<int W2>
  ac_fixed &operator >>= ( const ac_int<W2,false> &op2 );
  // Relational ---------------------------------------------------------------
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  bool operator == ( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  bool operator != ( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  bool operator < ( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  bool operator >= ( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  bool operator > ( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  template<int W2, int I2, bool S2, ac_q_mode Q2, ac_o_mode O2>
  bool operator <= ( const ac_fixed<W2,I2,S2,Q2,O2> &op2) const;
  bool operator == ( double d) const;
  bool operator != ( double d) const;
  bool operator < ( double d) const;
  bool operator >= ( double d) const;
  bool operator > ( double d) const;
  bool operator <= ( double d) const;
  // Bit and Slice Select -----------------------------------------------------
  template<int WS, int WX, bool SX>
  ac_int<WS,S> slc(const ac_int<WX,SX> &index) const;
  template<int WS>
  ac_int<WS,S> slc(signed index) const;
  template<int WS>
  ac_int<WS,S> slc(unsigned uindex) const;
  template<int W2, bool S2, int WX, bool SX>
  ac_fixed &set_slc(const ac_int<WX,SX> lsb, const ac_int<W2,S2> &slc);
  template<int W2, bool S2>
  ac_fixed &set_slc(signed lsb, const ac_int<W2,S2> &slc);
  template<int W2, bool S2>
  ac_fixed &set_slc(unsigned ulsb, const ac_int<W2,S2> &slc);

  bool operator [] ( unsigned int uindex) const;
  bool operator [] ( int index) const;
  template<int W2, bool S2>
  bool operator [] ( const ac_int<W2,S2> &index) const;
};

// Specializations for constructors on integers that bypass bit adjusting
//  and are therefore more efficient
template<> ac_fixed<1,1,true,AC_TRN,AC_WRAP>::ac_fixed( bool b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( bool b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( signed char b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned char b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( signed short b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned short b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( signed int b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned int b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( signed long b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned long b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( Ulong b );
template<> ac_fixed<1,1,false,AC_TRN,AC_WRAP>::ac_fixed( Slong b );

template<> ac_fixed<8,8,true,AC_TRN,AC_WRAP>::ac_fixed( bool b );
template<> ac_fixed<8,8,false,AC_TRN,AC_WRAP>::ac_fixed( bool b );
template<> ac_fixed<8,8,true,AC_TRN,AC_WRAP>::ac_fixed( signed char b );
template<> ac_fixed<8,8,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned char b );
template<> ac_fixed<8,8,true,AC_TRN,AC_WRAP>::ac_fixed( unsigned char b );
template<> ac_fixed<8,8,false,AC_TRN,AC_WRAP>::ac_fixed( signed char b );

template<> ac_fixed<16,16,true,AC_TRN,AC_WRAP>::ac_fixed( bool b );
template<> ac_fixed<16,16,false,AC_TRN,AC_WRAP>::ac_fixed( bool b );
template<> ac_fixed<16,16,true,AC_TRN,AC_WRAP>::ac_fixed( signed char b );
template<> ac_fixed<16,16,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned char b );
template<> ac_fixed<16,16,true,AC_TRN,AC_WRAP>::ac_fixed( unsigned char b );
template<> ac_fixed<16,16,false,AC_TRN,AC_WRAP>::ac_fixed( signed char b );
template<> ac_fixed<16,16,true,AC_TRN,AC_WRAP>::ac_fixed( signed short b );
template<> ac_fixed<16,16,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned short b );
template<> ac_fixed<16,16,true,AC_TRN,AC_WRAP>::ac_fixed( unsigned short b );
template<> ac_fixed<16,16,false,AC_TRN,AC_WRAP>::ac_fixed( signed short b );

template<> ac_fixed<32,32,true,AC_TRN,AC_WRAP>::ac_fixed( signed int b );
template<> ac_fixed<32,32,true,AC_TRN,AC_WRAP>::ac_fixed( unsigned int b );
template<> ac_fixed<32,32,false,AC_TRN,AC_WRAP>::ac_fixed( signed int b );
template<> ac_fixed<32,32,false,AC_TRN,AC_WRAP>::ac_fixed( unsigned int b );

template<> ac_fixed<32,32,true,AC_TRN,AC_WRAP>::ac_fixed( Slong b );
template<> ac_fixed<32,32,true,AC_TRN,AC_WRAP>::ac_fixed( Ulong b );
template<> ac_fixed<32,32,false,AC_TRN,AC_WRAP>::ac_fixed( Slong b );
template<> ac_fixed<32,32,false,AC_TRN,AC_WRAP>::ac_fixed( Ulong b );

template<> ac_fixed<64,64,true,AC_TRN,AC_WRAP>::ac_fixed( Slong b );
template<> ac_fixed<64,64,true,AC_TRN,AC_WRAP>::ac_fixed( Ulong b );
template<> ac_fixed<64,64,false,AC_TRN,AC_WRAP>::ac_fixed( Slong b );
template<> ac_fixed<64,64,false,AC_TRN,AC_WRAP>::ac_fixed( Ulong b );


// Binary Operators with Integers --------------------------------------------

#define FX_BIN_OP_WITH_INT_2I(BIN_OP, C_TYPE, WI, SI, RTYPE)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  typename ac_fixed<W,I,S>::template rt<WI,WI,SI>::RTYPE operator BIN_OP ( const ac_fixed<W,I,S,Q,O> &op, C_TYPE i_op);

#define FX_BIN_OP_WITH_INT(BIN_OP, C_TYPE, WI, SI, RTYPE)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  typename ac_fixed<WI,WI,SI>::template rt<W,I,S>::RTYPE operator BIN_OP ( C_TYPE i_op, const ac_fixed<W,I,S,Q,O> &op);\
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  typename ac_fixed<W,I,S>::template rt<WI,WI,SI>::RTYPE operator BIN_OP ( const ac_fixed<W,I,S,Q,O> &op, C_TYPE i_op);

#define FX_REL_OP_WITH_INT(REL_OP, C_TYPE, W2, S2)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  bool operator REL_OP ( const ac_fixed<W,I,S,Q,O> &op, C_TYPE op2); \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  bool operator REL_OP ( C_TYPE op2, const ac_fixed<W,I,S,Q,O> &op);

#define FX_ASSIGN_OP_WITH_INT_2(ASSIGN_OP, C_TYPE, W2, S2)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  ac_fixed<W,I,S,Q,O> &operator ASSIGN_OP ( ac_fixed<W,I,S,Q,O> &op, C_TYPE op2);
  
#define FX_ASSIGN_OP_WITH_INT_2I(ASSIGN_OP, C_TYPE, W2, S2)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O> \
  ac_fixed<W,I,S,Q,O> &operator ASSIGN_OP ( ac_fixed<W,I,S,Q,O> &op, C_TYPE op2);

#define FX_OPS_WITH_INT(C_TYPE, WI, SI) \
  FX_BIN_OP_WITH_INT(*, C_TYPE, WI, SI, mult) \
  FX_BIN_OP_WITH_INT(+, C_TYPE, WI, SI, plus) \
  FX_BIN_OP_WITH_INT(-, C_TYPE, WI, SI, minus) \
  FX_BIN_OP_WITH_INT(/, C_TYPE, WI, SI, div) \
  FX_BIN_OP_WITH_INT_2I(>>, C_TYPE, WI, SI, arg1) \
  FX_BIN_OP_WITH_INT_2I(<<, C_TYPE, WI, SI, arg1) \
  FX_BIN_OP_WITH_INT(&, C_TYPE, WI, SI, logic) \
  FX_BIN_OP_WITH_INT(|, C_TYPE, WI, SI, logic) \
  FX_BIN_OP_WITH_INT(^, C_TYPE, WI, SI, logic) \
  \
  FX_REL_OP_WITH_INT(==, C_TYPE, WI, SI) \
  FX_REL_OP_WITH_INT(!=, C_TYPE, WI, SI) \
  FX_REL_OP_WITH_INT(>, C_TYPE, WI, SI) \
  FX_REL_OP_WITH_INT(>=, C_TYPE, WI, SI) \
  FX_REL_OP_WITH_INT(<, C_TYPE, WI, SI) \
  FX_REL_OP_WITH_INT(<=, C_TYPE, WI, SI) \
  \
  FX_ASSIGN_OP_WITH_INT_2(+=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(-=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(*=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(/=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(%=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2I(>>=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2I(<<=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(&=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(|=, C_TYPE, WI, SI) \
  FX_ASSIGN_OP_WITH_INT_2(^=, C_TYPE, WI, SI)

FX_OPS_WITH_INT(bool, 1, false)
FX_OPS_WITH_INT(char, 8, true)
FX_OPS_WITH_INT(signed char, 8, true)
FX_OPS_WITH_INT(unsigned char, 8, false)
FX_OPS_WITH_INT(short, 16, true)
FX_OPS_WITH_INT(unsigned short, 16, false)
FX_OPS_WITH_INT(int, 32, true)
FX_OPS_WITH_INT(unsigned int, 32, false)
FX_OPS_WITH_INT(long, 32, true)
FX_OPS_WITH_INT(unsigned long, 32, false)
FX_OPS_WITH_INT(Slong, 64, true)
FX_OPS_WITH_INT(Ulong, 64, false)


#define FX_BIN_OP_WITH_AC_INT_1(BIN_OP, RTYPE)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, int WI, bool SI> \
  typename ac_fixed<WI,WI,SI>::template rt<W,I,S>::RTYPE operator BIN_OP ( const ac_int<WI,SI> &i_op, const ac_fixed<W,I,S,Q,O> &op);

#define FX_BIN_OP_WITH_AC_INT_2(BIN_OP, RTYPE)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, int WI, bool SI> \
  typename ac_fixed<W,I,S>::template rt<WI,WI,SI>::RTYPE operator BIN_OP ( const ac_fixed<W,I,S,Q,O> &op, const ac_int<WI,SI> &i_op);

#define FX_BIN_OP_WITH_AC_INT(BIN_OP, RTYPE)  \
  FX_BIN_OP_WITH_AC_INT_1(BIN_OP, RTYPE) \
  FX_BIN_OP_WITH_AC_INT_2(BIN_OP, RTYPE)

#define FX_REL_OP_WITH_AC_INT(REL_OP)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, int WI, bool SI> \
  bool operator REL_OP ( const ac_fixed<W,I,S,Q,O> &op, const ac_int<WI,SI> &op2); \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, int WI, bool SI> \
  bool operator REL_OP ( ac_int<WI,SI> &op2, const ac_fixed<W,I,S,Q,O> &op);

#define FX_ASSIGN_OP_WITH_AC_INT(ASSIGN_OP)  \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, int WI, bool SI> \
  ac_fixed<W,I,S,Q,O> &operator ASSIGN_OP ( ac_fixed<W,I,S,Q,O> &op, const ac_int<WI,SI> &op2); \
  template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O, int WI, bool SI> \
  ac_int<WI,SI> &operator ASSIGN_OP ( ac_int<WI,SI> &op, const ac_fixed<W,I,S,Q,O> &op2);

FX_BIN_OP_WITH_AC_INT(*, mult)
FX_BIN_OP_WITH_AC_INT(+, plus)
FX_BIN_OP_WITH_AC_INT(-, minus)
FX_BIN_OP_WITH_AC_INT(/, div)
FX_BIN_OP_WITH_AC_INT(&, logic)
FX_BIN_OP_WITH_AC_INT(|, logic)
FX_BIN_OP_WITH_AC_INT(^, logic)

FX_REL_OP_WITH_AC_INT(==)
FX_REL_OP_WITH_AC_INT(!=)
FX_REL_OP_WITH_AC_INT(>)
FX_REL_OP_WITH_AC_INT(>=)
FX_REL_OP_WITH_AC_INT(<)
FX_REL_OP_WITH_AC_INT(<=)

FX_ASSIGN_OP_WITH_AC_INT(+=)
FX_ASSIGN_OP_WITH_AC_INT(-=)
FX_ASSIGN_OP_WITH_AC_INT(*=)
FX_ASSIGN_OP_WITH_AC_INT(/=)
FX_ASSIGN_OP_WITH_AC_INT(%=)
FX_ASSIGN_OP_WITH_AC_INT(&=)
FX_ASSIGN_OP_WITH_AC_INT(|=)
FX_ASSIGN_OP_WITH_AC_INT(^=)

// Relational Operators with double 
template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool operator == ( double op, const ac_fixed<W,I,S,Q,O> &op2);
template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool operator != ( double op, const ac_fixed<W,I,S,Q,O> &op2);
template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool operator > ( double op, const ac_fixed<W,I,S,Q,O> &op2);
template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool operator < ( double op, const ac_fixed<W,I,S,Q,O> &op2);
template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool operator <= ( double op, const ac_fixed<W,I,S,Q,O> &op2);
template<int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
bool operator >= ( double op, const ac_fixed<W,I,S,Q,O> &op2);

// Global templatized functions for easy initialization to special values
template<ac_special_val V, int W, int I, bool S, ac_q_mode Q, ac_o_mode O>
ac_fixed<W,I,S,Q,O> value(ac_fixed<W,I,S,Q,O>);

namespace ac {
// PUBLIC FUNCTIONS
// function to initialize (or uninitialize) arrays
  template<ac_special_val V, int W, int I, bool S, ac_q_mode Q, ac_o_mode O> 
  bool init_array(ac_fixed<W,I,S,Q,O> *a, int n);
}


///////////////////////////////////////////////////////////////////////////////

#if (defined(_MSC_VER) && !defined(__EDG__))
#pragma warning( pop )
#endif

#endif // __AC_FIXED_H

/**************************************************************************
 *                                                                        *
 *  ALGORITHMIC C DATATYPES END-USER LICENSE AGREEMENT                    *
 *                                                                        *
 *                                                                        *
 *  IMPORTANT - USE OF SOFTWARE IS SUBJECT TO LICENSE RESTRICTIONS        *
 *  CAREFULLY READ THIS LICENSE AGREEMENT BEFORE USING THE SOFTWARE       *
 *                                                                        *
 *  YOU MAY USE AND DISTRIBUTE UNMODIFIED VERSIONS OF THIS SOFTWARE AS    *
 *  STATED BELOW, YOU MAY NOT MODIFY THE SOFTWARE This license is a       *
 *  legal Agreement between you, the end user, either individually or     *
 *  as an authorized representative of a company acquiring the license,   *
 *  and Mentor Graphics Corporation ("Mentor Graphics"). YOUR USE OF      *
 *  THE SOFTWARE INDICATES YOUR COMPLETE AND UNCONDITIONAL ACCEPTANCE     *
 *  OF THE TERMS AND CONDITIONS SET FORTH IN THIS AGREEMENT. If you do    *
 *  not agree to these terms and conditions, promptly return or, if       *
 *  received electronically, delete the Software and all accompanying     *
 *  items.                                                                *
 *                                                                        *
 *                                                                        *
 *  1. GRANT OF LICENSE. YOU MAY USE AND DISTRIBUTE THE SOFTWARE, BUT     *
 *  YOU MAY NOT MODIFY THE SOFTWARE. The Software you are installing,     *
 *  downloading, or otherwise acquired, under this Agreement, including   *
 *  source code, binary code, updates, modifications, revisions,          *
 *  copies, or documentation pertaining to Algorithmic C Datatypes        *
 *  (collectively the "Software") is a copyrighted work owned by Mentor   *
 *  Graphics. Mentor Graphics grants to you, a nontransferable,           *
 *  nonexclusive, limited copyright license to use and distribute the     *
 *  Software, but you may not modify the Software. Use of the Software    *
 *  consists solely of reproduction, performance, and display.            *
 *                                                                        *
 *  2. RESTRICTIONS; NO MODIFICATION. Modifying the Software is           *
 *  prohibited. Each copy of the Software you create must include all     *
 *  notices and legends embedded in the Software.  Modifying the          *
 *  Software means altering, enhancing, editing, deleting portions or     *
 *  creating derivative works of the Software.  You may append other      *
 *  code to the Software, so long as the Software is not otherwise        *
 *  modified. Mentor Graphics retains all rights not expressly granted    *
 *  by this Agreement. The terms of this Agreement, including without     *
 *  limitation, the licensing and assignment provisions, shall be         *
 *  binding upon your successors in interest and assigns.  The            *
 *  provisions of this section 2 shall survive termination or             *
 *  expiration of this Agreement.                                         *
 *                                                                        *
 *  3. USER COMMENT AND SUGGESTIONS.  You are not obligated to provide    *
 *  Mentor Graphics with comments or suggestions regarding the            *
 *  Software.  However, if you do provide to Mentor Graphics comments     *
 *  or suggestions for the modification, correction, improvement or       *
 *  enhancement of (a) the Software or (b) Mentor Graphics products or    *
 *  processes which may embody the Software ("Comments"), you grant to    *
 *  Mentor a non-exclusive, irrevocable, worldwide, royalty-free          *
 *  license to disclose, display, perform, copy, make, have made, use,    *
 *  sublicense, sell, and otherwise dispose of the Comments, and Mentor   *
 *  Graphics' products embodying such Comments, in any manner which       *
 *  Mentor Graphics chooses, without reference to the source.             *
 *                                                                        *
 *  4. NO WARRANTY. MENTOR GRAPHICS EXPRESSLY DISCLAIMS ALL WARRANTY      *
 *  FOR THE SOFTWARE. TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE       *
 *  LAW, THE SOFTWARE AND ANY RELATED DOCUMENTATION IS PROVIDED "AS IS"   *
 *  AND WITH ALL FAULTS AND WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
 *  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT LIMITATION, THE   *
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR       *
 *  PURPOSE, OR NONINFRINGEMENT. THE ENTIRE RISK ARISING OUT OF USE OR    *
 *  DISTRIBUTION OF THE SOFTWARE REMAINS WITH YOU.                        *
 *                                                                        *
 *  5. LIMITATION OF LIABILITY. IN NO EVENT WILL MENTOR GRAPHICS OR ITS   *
 *  LICENSORS BE LIABLE FOR INDIRECT, SPECIAL, INCIDENTAL, OR             *
 *  CONSEQUENTIAL DAMAGES (INCLUDING LOST PROFITS OR SAVINGS) WHETHER     *
 *  BASED ON CONTRACT, TORT OR ANY OTHER LEGAL THEORY, EVEN IF MENTOR     *
 *  GRAPHICS OR ITS LICENSORS HAVE BEEN ADVISED OF THE POSSIBILITY OF     *
 *  SUCH DAMAGES.                                                         *
 *                                                                        *
 *  6.  LIFE ENDANGERING ACTIVITIES. NEITHER MENTOR GRAPHICS NOR ITS      *
 *  LICENSORS SHALL BE LIABLE FOR ANY DAMAGES RESULTING FROM OR IN        *
 *  CONNECTION WITH THE USE OR DISTRIBUTION OF SOFTWARE IN ANY            *
 *  APPLICATION WHERE THE FAILURE OR INACCURACY OF THE SOFTWARE MIGHT     *
 *  RESULT IN DEATH OR PERSONAL INJURY.                                   *
 *                                                                        *
 *  7.  INDEMNIFICATION.  YOU AGREE TO INDEMNIFY AND HOLD HARMLESS        *
 *  MENTOR GRAPHICS AND ITS LICENSORS FROM ANY CLAIMS, LOSS, COST,        *
 *  DAMAGE, EXPENSE, OR LIABILITY, INCLUDING ATTORNEYS' FEES, ARISING     *
 *  OUT OF OR IN CONNECTION WITH YOUR USE OR DISTRIBUTION OF SOFTWARE.    *
 *                                                                        *
 *  8. TERM AND TERMINATION. This Agreement terminates immediately if     *
 *  you exceed the scope of the license granted or fail to comply with    *
 *  the provisions of this License Agreement.  If you institute patent    *
 *  litigation against Mentor Graphics (including a cross-claim or        *
 *  counterclaim in a lawsuit) alleging that the Software constitutes     *
 *  direct or contributory patent infringement, then any patent           *
 *  licenses granted to you under this License for that Software shall    *
 *  terminate as of the date such litigation is filed. Upon termination   *
 *  or expiration, you agree to cease all use of the Software and         *
 *  delete all copies of the Software.                                    *
 *                                                                        *
 *  9. EXPORT. Software may be subject to regulation by local laws and    *
 *  United States government agencies, which prohibit export or           *
 *  diversion of certain products, information about the products, and    *
 *  direct products of the products to certain countries and certain      *
 *  persons. You agree that you will not export any Software or direct    *
 *  product of Software in any manner without first obtaining all         *
 *  necessary approval from appropriate local and United States           *
 *  government agencies.                                                  *
 *                                                                        *
 *  10. RESTRICTED RIGHTS NOTICE. Software was developed entirely at      *
 *  private expense and is commercial computer software provided with     *
 *  RESTRICTED RIGHTS. Use, duplication or disclosure by the U.S.         *
 *  Government or a U.S. Government subcontractor is subject to the       *
 *  restrictions set forth in the license agreement under which           *
 *  Software was obtained pursuant to DFARS 227.7202-3(a) or as set       *
 *  forth in subparagraphs (c)(1) and (2) of the Commercial Computer      *
 *  Software - Restricted Rights clause at FAR 52.227-19, as              *
 *  applicable. Contractor/manufacturer is Mentor Graphics Corporation,   *
 *  8005 SW Boeckman Road, Wilsonville, Oregon 97070-7777 USA.            *
 *                                                                        *
 *  11. CONTROLLING LAW AND JURISDICTION. THIS AGREEMENT SHALL BE         *
 *  GOVERNED BY AND CONSTRUED UNDER THE LAWS OF THE STATE OF OREGON,      *
 *  USA. All disputes arising out of or in relation to this Agreement     *
 *  shall be submitted to the exclusive jurisdiction of Multnomah         *
 *  County, Oregon. This section shall not restrict Mentor Graphics'      *
 *  right to bring an action against you in the jurisdiction where your   *
 *  place of business is located.  The United Nations Convention on       *
 *  Contracts for the International Sale of Goods does not apply to       *
 *  this Agreement.                                                       *
 *                                                                        *
 *  12. SEVERABILITY. If any provision of this Agreement is held by a     *
 *  court of competent jurisdiction to be void, invalid, unenforceable    *
 *  or illegal, such provision shall be severed from this Agreement and   *
 *  the remaining provisions will remain in full force and effect.        *
 *                                                                        *
 *  13. MISCELLANEOUS.  This Agreement contains the parties' entire       *
 *  understanding relating to its subject matter and supersedes all       *
 *  prior or contemporaneous agreements. This Agreement may only be       *
 *  modified in writing by authorized representatives of the parties.     *
 *  Waiver of terms or excuse of breach must be in writing and shall      *
 *  not constitute subsequent consent, waiver or excuse. The prevailing   *
 *  party in any legal action regarding the subject matter of this        *
 *  Agreement shall be entitled to recover, in addition to other          *
 *  relief, reasonable attorneys' fees and expenses.                      *
 *                                                                        *
 **************************************************************************/
