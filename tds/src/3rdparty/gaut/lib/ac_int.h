#ifndef __AC_INT_H
#define __AC_INT_H

#ifndef __cplusplus
#error C++ is required to include this header file
#endif

#if (defined(__GNUC__) && __GNUC__ < 3 && !defined(__EDG__))
#error GCC version 3 or greater is required to include this header file
#endif

#if (defined(_MSC_VER) && _MSC_VER < 1400 && !defined(__EDG__))
#error Microsoft Visual Studio 8 or newer is required to include this header file
#endif

#if (defined(_MSC_VER) && !defined(__EDG__))
#pragma warning( push )
#pragma warning( disable: 4127 4100 4244 4307 4554 )
#endif

// for safety
#if (defined(N) || defined(N2))
#error One or more of the following is defined: N, N2. Definition conflicts with their usage as template parameters. 
#endif

// for safety
#if (defined(W) || defined(I) || defined(S) || defined(W2) || defined(I2) || defined(S2))
#error One or more of the following is defined: W, I, S, W2, I2, S2. Definition conflicts with their usage as template parameters. 
#endif

#if defined(_MSC_VER)
#if !defined(__EDG__) && _MSC_VER < 1400 && !defined(for)
# define for if(0);else for
#endif
typedef unsigned __int64 Ulong;
typedef signed   __int64 Slong;
#else
typedef unsigned long long Ulong;
typedef signed   long long Slong;
#endif

enum ac_base_mode { AC_BIN=2, AC_OCT=8, AC_DEC=10, AC_HEX=16 };
enum ac_special_val {AC_VAL_DC, AC_VAL_0};
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

namespace ac_private {
#if defined(__SYNTHESIS__) && !defined(AC_IGNORE_BUILTINS)
#pragma builtin
#endif

  //////////////////////////////////////////////////////////////////////////////
  //  Integer Vector class: iv 
  //////////////////////////////////////////////////////////////////////////////
  template<int N>
  class iv {
  protected:
    int v[N];
  public:
    template<int N2> friend class iv;
    iv() {}
    template<int N2>
    iv ( const iv<N2> &b );
    iv ( Slong t);
    iv ( Ulong t);
    iv ( int t);
    iv ( unsigned int t);
    iv ( long t);
    iv ( unsigned long t);
    iv ( double d );
  
    // Explicit conversion functions to C built-in types -------------
      Slong to_int64() const;
      Ulong to_uint64() const;
      double to_double() const;
      void conv_from_fraction(double d, bool *qb, bool *rbits, bool *o);
  
    template<int N2, int Nr>
      void mult(const iv<N2> &op2, iv<Nr> &r) const;
    template<int N2, int Nr>
    void add(const iv<N2> &op2, iv<Nr> &r) const;
    template<int N2, int Nr>
    void sub(const iv<N2> &op2, iv<Nr> &r) const;
    template<int N2, int Nr>
    void div(const iv<N2> &op2, iv<Nr> &r) const ;
    template<int N2, int Nr>
    void rem(const iv<N2> &op2, iv<Nr> &r) const;
    template<int Nr>
    void shift_l(unsigned op2, iv<Nr> &r) const;
    template<int Nr>
    void shift_l2(signed op2, iv<Nr> &r) const;
    template<int Nr>
    void shift_r(unsigned op2, iv<Nr> &r) const;
    template<int Nr>
    void shift_r2(signed op2, iv<Nr> &r) const;
    template<int Nr, int B>
    void const_shift_l(iv<Nr> &r) const;
    template<int Nr, int B>
    void const_shift_r(iv<Nr> &r) const;
    template<int Nr>
    void bitwise_complement(iv<Nr> &r) const;
    template<int N2, int Nr>
    void bitwise_and(const iv<N2> &op2, iv<Nr> &r) const;
    template<int N2, int Nr>
    void bitwise_or(const iv<N2> &op2, iv<Nr> &r) const;
    template<int N2, int Nr>
    void bitwise_xor(const iv<N2> &op2, iv<Nr> &r) const;
    template<int N2>
    bool equal(const iv<N2> &op2) const;
    template<int N2>
    bool greater_than(const iv<N2> &op2) const;
    template<int N2>
    bool less_than(const iv<N2> &op2) const;
    bool equal_zero() const;
    template<int N2>
    void set_slc(unsigned lsb, int WS, const iv<N2> &op2);
  };
  
  template<>   iv<1>::iv(double d);
  template<>   iv<2>::iv(double d);
  template<>   Slong iv<1>::to_int64() const;
  template<>   Ulong iv<1>::to_uint64() const;
  
  template<>   Slong iv<2>::to_int64() const ;
  template<>   Ulong iv<2>::to_uint64() const;
  
  template<> template<>   void iv<1>::set_slc(unsigned lsb, int WS, const iv<1> &op2);
  template<> template<>   void iv<2>::set_slc(unsigned lsb, int WS, const iv<1> &op2);
  template<> template<>   void iv<2>::set_slc(unsigned lsb, int WS, const iv<2> &op2);

  /*  add automatic conversion to Slong/Ulong depending on S and C */
  template<int N, bool S, bool C>
  class iv_conv  : public iv<N> {
  protected:
    iv_conv();
    template<class T> iv_conv(const T& t);
  }; 

  template<int N>
  class iv_conv<N,false,true> : public iv<N> {
  public:
    operator Ulong () const { return iv<N>::to_uint64(); };
  protected:
    iv_conv();
    template<class T> iv_conv(const T& t);
  };

  template<int N>
  class iv_conv<N,true,true> : public iv<N> {
  public:
    operator Slong () const { return iv<N>::to_int64(); };
  protected:
    iv_conv();
    template<class T> iv_conv(const T& t);
  }; 
} // namespace ac_private


template<int W, bool S=true>
class ac_int : public ac_private::iv_conv<(W+31+!S)/32, S, W<=64>   {
#if defined(__SYNTHESIS__) && !defined(AC_IGNORE_BUILTINS)
#pragma builtin
#endif
  enum {N=(W+31+!S)/32};
  typedef ac_private::iv_conv<N, S, W <= 64> ConvBase;
  typedef ac_private::iv<N>                  Base;

  void bit_adjust();

  bool is_neg() const;

public:
template<int W2, bool S2>
  struct rt {
    enum {
      mult_w = W+W2,
      mult_s = S||S2,
      plus_w = ((W+(S2&&!S)) > (W2+(S&&!S2)) ? (W+(S2&&!S)) : (W2+(S&&!S2)))+1,
      plus_s = S||S2,
      minus_w = ((W+(S2&&!S)) > (W2+(S&&!S2)) ? (W+(S2&&!S)) : (W2+(S&&!S2)))+1,
      minus_s = true,
      div_w = W+S2,
      div_s = S||S2,
      mod_w = ((W) < (W2+(!S2&&S)) ? (W) : (W2+(!S2&&S))),
      mod_s = S,
      logic_w = ((W+(S2&&!S)) > (W2+(S&&!S2)) ? (W+(S2&&!S)) : (W2+(S&&!S2))),
      logic_s = S||S2
    };
    typedef ac_int<mult_w, mult_s> mult;
    typedef ac_int<plus_w, plus_s> plus;
    typedef ac_int<minus_w, minus_s> minus;
    typedef ac_int<logic_w, logic_s> logic;
    typedef ac_int<div_w, div_s> div;
    typedef ac_int<mod_w, mod_s> mod;
    typedef ac_int<W, S> arg1;
  };
  template<int W2, bool S2> friend class ac_int;
  ac_int();

  template<int W2, bool S2>
  ac_int (const ac_int<W2,S2> &op);
  
  ac_int( bool b );
  ac_int( char b );
  ac_int( signed char b );
  ac_int( unsigned char b );
  ac_int( signed short b );
  ac_int( unsigned short b );
  ac_int( signed int b );
  ac_int( unsigned int b );
  ac_int( signed long b );
  ac_int( unsigned long b );
  ac_int( Slong b );
  ac_int( Ulong b );
  ac_int( double d );

  int to_int();
  unsigned to_uint();
  long to_long();
  unsigned long to_ulong();
  Slong to_int64();
  Ulong to_uint64();
  double to_double();
  int length();
  
   //  Arithmetic : Binary ----------------------------------------------------
  template<int W2, bool S2>
  typename rt<W2,S2>::mult operator *( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  typename rt<W2,S2>::plus operator +( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  typename rt<W2,S2>::minus operator -( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  typename rt<W2,S2>::div operator /( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  typename rt<W2,S2>::mod operator %( const ac_int<W2,S2> &op2) const ; 
  
    // Arithmetic assign  ------------------------------------------------------
  template<int W2, bool S2>
  ac_int &operator *=( const ac_int<W2,S2> &op2);
  template<int W2, bool S2>
  ac_int &operator +=( const ac_int<W2,S2> &op2);
  template<int W2, bool S2>
  ac_int &operator -=( const ac_int<W2,S2> &op2);
  template<int W2, bool S2>
  ac_int &operator /=( const ac_int<W2,S2> &op2);
  template<int W2, bool S2>
  ac_int &operator %=( const ac_int<W2,S2> &op2);
  
  // Arithmetic prefix increment, decrement ----------------------------------
  ac_int &operator ++();
  ac_int &operator --();
  // Arithmetic postfix increment, decrement ---------------------------------
  const ac_int operator ++(int);
  const ac_int operator --(int);
  // Arithmetic Unary --------------------------------------------------------
  ac_int operator +();
  ac_int<W+1,true> operator -() const;
  // ! ------------------------------------------------------------------------
  bool operator ! () const ;
  // Bitwise (arithmetic) unary: complement  -----------------------------
  ac_int<W+!S, true> operator ~() const ;

  // Bitwise (arithmetic): and, or, xor ----------------------------------
  template<int W2, bool S2>
  typename rt<W2,S2>::logic operator & ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  typename rt<W2,S2>::logic operator | ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  typename rt<W2,S2>::logic operator ^ ( const ac_int<W2,S2> &op2) const;
  // Bitwise assign (not arithmetic): and, or, xor ----------------------------
  template<int W2, bool S2>
  ac_int &operator &= ( const ac_int<W2,S2> &op2 );
  template<int W2, bool S2>
  ac_int &operator |= ( const ac_int<W2,S2> &op2 );
  template<int W2, bool S2>
  ac_int &operator ^= ( const ac_int<W2,S2> &op2 );
    // Shift (result constrained by left operand) -------------------------------
  template<int W2>
  ac_int operator << ( const ac_int<W2,true> &op2 ) const;
  template<int W2>
  ac_int operator << ( const ac_int<W2,false> &op2 ) const;
  template<int W2>
  ac_int operator >> ( const ac_int<W2,true> &op2 ) const;
  template<int W2>
  ac_int operator >> ( const ac_int<W2,false> &op2 ) const;
  // Shift assign ------------------------------------------------------------
  template<int W2>
  ac_int &operator <<= ( const ac_int<W2,true> &op2 );
  template<int W2>
  ac_int &operator <<= ( const ac_int<W2,false> &op2 );
  template<int W2>
  ac_int &operator >>= ( const ac_int<W2,true> &op2 );
  template<int W2>
  ac_int &operator >>= ( const ac_int<W2,false> &op2 );

    // Relational ---------------------------------------------------------------
  template<int W2, bool S2>
  bool operator == ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  bool operator != ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  bool operator < ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  bool operator >= ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  bool operator > ( const ac_int<W2,S2> &op2) const;
  template<int W2, bool S2>
  bool operator <= ( const ac_int<W2,S2> &op2) const;

  // Bit and Slice Select -----------------------------------------------------
  template<int WS, int WX, bool SX>
    ac_int<WS,S> slc(const ac_int<WX,SX> &index) const;

  template<int WS>
    ac_int<WS,S> slc(signed index) const;
  template<int WS>
    ac_int<WS,S> slc(unsigned uindex) const;

  template<int W2, bool S2, int WX, bool SX>
    ac_int &set_slc(const ac_int<WX,SX> lsb, const ac_int<W2,S2> &slc);
  template<int W2, bool S2>
    ac_int &set_slc(signed lsb, const ac_int<W2,S2> &slc);
  template<int W2, bool S2>
    ac_int &set_slc(unsigned ulsb, const ac_int<W2,S2> &slc);

  bool operator [] ( unsigned int uindex) const; 
  bool operator [] ( int index) const;
  template<int W2, bool S2>
  bool operator [] ( const ac_int<W2,S2> &index) const;
};

  
//  Binary Operators with Integers --------------------------------------------

#define BIN_OP_WITH_INT(BIN_OP, C_TYPE, WI, SI, RTYPE)  \
  template<int W, bool S> \
    typename ac_int<WI,SI>::template rt<W,S>::RTYPE operator BIN_OP ( C_TYPE i_op, const ac_int<W,S> &op) {  \
    return ac_int<WI,SI>(i_op).operator BIN_OP (op);  \
  } \
  template<int W, bool S>   \
    typename ac_int<W,S>::template rt<WI,SI>::RTYPE operator BIN_OP ( const ac_int<W,S> &op, C_TYPE i_op) {  \
    return op.operator BIN_OP (ac_int<WI,SI>(i_op));  \
  }

#define REL_OP_WITH_INT(REL_OP, C_TYPE, W2, S2)  \
  template<int W, bool S>   \
    bool operator REL_OP ( const ac_int<W,S> &op, C_TYPE op2) {  \
    return op.operator REL_OP (ac_int<W2,S2>(op2));  \
  }  \
  template<int W, bool S> \
    bool operator REL_OP ( C_TYPE op2, const ac_int<W,S> &op) {  \
    return ac_int<W2,S2>(op2).operator REL_OP (op);  \
  }

#define ASSIGN_OP_WITH_INT(ASSIGN_OP, C_TYPE, W2, S2)  \
  template<int W, bool S>   \
    ac_int<W,S> &operator ASSIGN_OP ( ac_int<W,S> &op, C_TYPE op2) {  \
    return op.operator ASSIGN_OP (ac_int<W2,S2>(op2));  \
  }

#define OPS_WITH_INT(C_TYPE, WI, SI) \
  BIN_OP_WITH_INT(*, C_TYPE, WI, SI, mult) \
  BIN_OP_WITH_INT(+, C_TYPE, WI, SI, plus) \
  BIN_OP_WITH_INT(-, C_TYPE, WI, SI, minus) \
  BIN_OP_WITH_INT(/, C_TYPE, WI, SI, div) \
  BIN_OP_WITH_INT(%, C_TYPE, WI, SI, mod) \
  BIN_OP_WITH_INT(>>, C_TYPE, WI, SI, arg1) \
  BIN_OP_WITH_INT(<<, C_TYPE, WI, SI, arg1) \
  BIN_OP_WITH_INT(&, C_TYPE, WI, SI, logic) \
  BIN_OP_WITH_INT(|, C_TYPE, WI, SI, logic) \
  BIN_OP_WITH_INT(^, C_TYPE, WI, SI, logic) \
  \
  REL_OP_WITH_INT(==, C_TYPE, WI, SI) \
  REL_OP_WITH_INT(!=, C_TYPE, WI, SI) \
  REL_OP_WITH_INT(>, C_TYPE, WI, SI) \
  REL_OP_WITH_INT(>=, C_TYPE, WI, SI) \
  REL_OP_WITH_INT(<, C_TYPE, WI, SI) \
  REL_OP_WITH_INT(<=, C_TYPE, WI, SI) \
  \
  ASSIGN_OP_WITH_INT(+=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(-=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(*=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(/=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(%=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(>>=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(<<=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(&=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(|=, C_TYPE, WI, SI) \
  ASSIGN_OP_WITH_INT(^=, C_TYPE, WI, SI)

OPS_WITH_INT(bool, 1, false)
OPS_WITH_INT(char, 8, true)
OPS_WITH_INT(signed char, 8, true)
OPS_WITH_INT(unsigned char, 8, false)
OPS_WITH_INT(short, 16, true)
OPS_WITH_INT(unsigned short, 16, false)
OPS_WITH_INT(int, 32, true)
OPS_WITH_INT(unsigned int, 32, false)
OPS_WITH_INT(long, 32, true)
OPS_WITH_INT(unsigned long, 32, false)
OPS_WITH_INT(Slong, 64, true)
OPS_WITH_INT(Ulong, 64, false)

namespace ac_intN {
  ///////////////////////////////////////////////////////////////////////////////
  //  Predefined for ease of use
  ///////////////////////////////////////////////////////////////////////////////
  typedef ac_int<1,          true>   int1;
  typedef ac_int<1,          false>  uint1;
  typedef ac_int<2,          true>   int2;
  typedef ac_int<2,          false>  uint2;
  typedef ac_int<3,          true>   int3;
  typedef ac_int<3,          false>  uint3;
  typedef ac_int<4,          true>   int4;
  typedef ac_int<4,          false>  uint4;
  typedef ac_int<5,          true>   int5;
  typedef ac_int<5,          false>  uint5;
  typedef ac_int<6,          true>   int6;
  typedef ac_int<6,          false>  uint6;
  typedef ac_int<7,          true>   int7;
  typedef ac_int<7,          false>  uint7;
  typedef ac_int<8,          true>   int8;
  typedef ac_int<8,          false>  uint8;
  typedef ac_int<9,          true>   int9;
  typedef ac_int<9,          false>  uint9;
  typedef ac_int<10,         true>   int10;
  typedef ac_int<10,         false>  uint10;
  typedef ac_int<11,         true>   int11;
  typedef ac_int<11,         false>  uint11;
  typedef ac_int<12,         true>   int12;
  typedef ac_int<12,         false>  uint12;
  typedef ac_int<13,         true>   int13;
  typedef ac_int<13,         false>  uint13;
  typedef ac_int<14,         true>   int14;
  typedef ac_int<14,         false>  uint14;
  typedef ac_int<15,         true>   int15;
  typedef ac_int<15,         false>  uint15;
  typedef ac_int<16,         true>   int16;
  typedef ac_int<16,         false>  uint16;
  typedef ac_int<17,         true>   int17;
  typedef ac_int<17,         false>  uint17;
  typedef ac_int<18,         true>   int18;
  typedef ac_int<18,         false>  uint18;
  typedef ac_int<19,         true>   int19;
  typedef ac_int<19,         false>  uint19;
  typedef ac_int<20,         true>   int20;
  typedef ac_int<20,         false>  uint20;
  typedef ac_int<21,         true>   int21;
  typedef ac_int<21,         false>  uint21;
  typedef ac_int<22,         true>   int22;
  typedef ac_int<22,         false>  uint22;
  typedef ac_int<23,         true>   int23;
  typedef ac_int<23,         false>  uint23;
  typedef ac_int<24,         true>   int24;
  typedef ac_int<24,         false>  uint24;
  typedef ac_int<25,         true>   int25;
  typedef ac_int<25,         false>  uint25;
  typedef ac_int<26,         true>   int26;
  typedef ac_int<26,         false>  uint26;
  typedef ac_int<27,         true>   int27;
  typedef ac_int<27,         false>  uint27;
  typedef ac_int<28,         true>   int28;
  typedef ac_int<28,         false>  uint28;
  typedef ac_int<29,         true>   int29;
  typedef ac_int<29,         false>  uint29;
  typedef ac_int<30,         true>   int30;
  typedef ac_int<30,         false>  uint30;
  typedef ac_int<31,         true>   int31;
  typedef ac_int<31,         false>  uint31;
  typedef ac_int<32,         true>   int32;
  typedef ac_int<32,         false>  uint32;
  typedef ac_int<33,         true>   int33;
  typedef ac_int<33,         false>  uint33;
  typedef ac_int<34,         true>   int34;
  typedef ac_int<34,         false>  uint34;
  typedef ac_int<35,         true>   int35;
  typedef ac_int<35,         false>  uint35;
  typedef ac_int<36,         true>   int36;
  typedef ac_int<36,         false>  uint36;
  typedef ac_int<37,         true>   int37;
  typedef ac_int<37,         false>  uint37;
  typedef ac_int<38,         true>   int38;
  typedef ac_int<38,         false>  uint38;
  typedef ac_int<39,         true>   int39;
  typedef ac_int<39,         false>  uint39;
  typedef ac_int<40,         true>   int40;
  typedef ac_int<40,         false>  uint40;
  typedef ac_int<41,         true>   int41;
  typedef ac_int<41,         false>  uint41;
  typedef ac_int<42,         true>   int42;
  typedef ac_int<42,         false>  uint42;
  typedef ac_int<43,         true>   int43;
  typedef ac_int<43,         false>  uint43;
  typedef ac_int<44,         true>   int44;
  typedef ac_int<44,         false>  uint44;
  typedef ac_int<45,         true>   int45;
  typedef ac_int<45,         false>  uint45;
  typedef ac_int<46,         true>   int46;
  typedef ac_int<46,         false>  uint46;
  typedef ac_int<47,         true>   int47;
  typedef ac_int<47,         false>  uint47;
  typedef ac_int<48,         true>   int48;
  typedef ac_int<48,         false>  uint48;
  typedef ac_int<49,         true>   int49;
  typedef ac_int<49,         false>  uint49;
  typedef ac_int<50,         true>   int50;
  typedef ac_int<50,         false>  uint50;
  typedef ac_int<51,         true>   int51;
  typedef ac_int<51,         false>  uint51;
  typedef ac_int<52,         true>   int52;
  typedef ac_int<52,         false>  uint52;
  typedef ac_int<53,         true>   int53;
  typedef ac_int<53,         false>  uint53;
  typedef ac_int<54,         true>   int54;
  typedef ac_int<54,         false>  uint54;
  typedef ac_int<55,         true>   int55;
  typedef ac_int<55,         false>  uint55;
  typedef ac_int<56,         true>   int56;
  typedef ac_int<56,         false>  uint56;
  typedef ac_int<57,         true>   int57;
  typedef ac_int<57,         false>  uint57;
  typedef ac_int<58,         true>   int58;
  typedef ac_int<58,         false>  uint58;
  typedef ac_int<59,         true>   int59;
  typedef ac_int<59,         false>  uint59;
  typedef ac_int<60,         true>   int60;
  typedef ac_int<60,         false>  uint60;
  typedef ac_int<61,         true>   int61;
  typedef ac_int<61,         false>  uint61;
  typedef ac_int<62,         true>   int62;
  typedef ac_int<62,         false>  uint62;
  typedef ac_int<63,         true>   int63;
  typedef ac_int<63,         false>  uint63;
}  // namespace ac_intN

#ifndef AC_NOT_USING_INTN
using namespace ac_intN;
#endif
// Global templatized functions for easy initialization to special values
template<ac_special_val V, int W, bool S>
ac_int<W,S> value(ac_int<W,S>);



namespace ac {
#if defined(__SYNTHESIS__) && !defined(AC_IGNORE_BUILTINS)
#pragma builtin
#endif
// PUBLIC FUNCTIONS 
// function to initialize (or uninitialize) arrays 
  template<ac_special_val V, int W, bool S>
  bool init_array(ac_int<W,S> *a, int n);
}

#endif // __AC_INT_H
