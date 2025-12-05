------------------------------------------------------------------------------
-- "fixed_pkg" package contains functions for fixed point math.
-- Fixed point type is defined as follows:
-- 0000000000
-- 4321012345
-- 4   0   -5
-- The decimal point is assumed between the "0" and "-1" index
-- Thus "0011010000" = 6.5 and would be written as 00110.10000
-- (SignBit) -2**4*0+2**3*0+2**2*1=2**2*1+2**0*0+2**(-1)*1+2**(-2)*0+...+2**(-5)*0
-- All types are assumed to be in the "downto" direction.
--  create from  VHDL-200X 
------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;

package fixed_pkg is
  type quantization_mode is (AC_TRN,-- Defauts truncature
				AC_RND,   -- nearest LSB '0'
				AC_RND_INF, -- Round to positive
				AC_TRN_ZERO, --
           	AC_RND_MIN_INF, -- Round to negate
            AC_RND_ZERO, -- Round towards zero
			 	AC_RND_CONV);

  type overflow_mode is (AC_WRAP,-- Defauts wrap
				   AC_SAT, --
             	AC_SAT_ZERO, -- 
			    	AC_SAT_SYM); --

  -- base Unsigned fixed point type, downto direction assumed
  type     ufixed is array (INTEGER range <>) of STD_LOGIC;
  -- base Signed fixed point type, downto direction assumed
  type     sfixed is array (INTEGER range <>) of STD_LOGIC;


  --===========================================================================
  --   saturate Functions
  --===========================================================================
  
  -- purpose: returns a saturated number 
  -- AC_WRAP : 	Any MSB bits outside the range are deleted (modulo)
  -- AC_SAT : 	MAX if overflow
  --			MIN if underflow
  -- AC_SAT_SYM : MAX if overflow
  --			-MAX if underflow
  -- AC_SAT_ZERO  O if owerflow
  --			O if underflow
  function saturate (
    constant number: ufixed;
    constant overflow_style : overflow_mode;
    constant overflow_flag : BOOLEAN;
    constant underflow_flag : BOOLEAN )
    return ufixed;
  function saturate (
    constant number: sfixed;
    constant overflow_style : overflow_mode;
    constant overflow_flag : BOOLEAN;
    constant underflow_flag : BOOLEAN )
    return sfixed;

  --===========================================================================
  --   RESIZE Functions
  --===========================================================================
 function resize (
    arg                     : ufixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant round_style    : quantization_mode; 
    constant overflow_style : overflow_mode)
    return ufixed;

  function resize (
    arg                     : sfixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant round_style    : quantization_mode; 
    constant overflow_style : overflow_mode)
    return sfixed;

  function resize_s2u (
    arg                     : sfixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant round_style    : quantization_mode;
    constant overflow_style : overflow_mode)
    return ufixed;

  --===========================================================================
  --   conversion Functions :  resize(overflow/quantization)) !!!
  --===========================================================================
  function to_slv (arg : ufixed; -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
     return STD_LOGIC_VECTOR;

  function to_slv (arg : sfixed; -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return STD_LOGIC_VECTOR;

  function to_sfixed (
       arg                  : STD_LOGIC_VECTOR;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return sfixed;
 
  function to_sfixed (
       arg                  : SIGNED;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return sfixed;
 
  function to_ufixed (
       arg                  : STD_LOGIC_VECTOR;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode;
       constant overflow_style : overflow_mode)
       return ufixed;

  function to_ufixed (
       arg                  : UNSIGNED;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return ufixed;

  function to_ufixed (
       arg                  : SIGNED;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode;
       constant overflow_style : overflow_mode)
       return ufixed;

  function to_unsigned (
    arg                     : ufixed;   -- ufixed point input
    size : INTEGER  -- size >=arg'length : add 0
    )
    return UNSIGNED;

  function to_signed (
    arg                     : sfixed;   -- sfixed point input
    size : INTEGER  -- size >=arg'length : sign extend only
    )
    return SIGNED;


  function to_signed (
    arg                     : ufixed;   -- ufixed point input
    size : INTEGER  -- size >=arg'length : sign extend only
    )
    return SIGNED;
  --===========================================================================
  --   only sign extend Functions : (no resize(overflow/quantization)) !!!
  --===========================================================================
  function to_slv (arg : ufixed; width : INTEGER ) return STD_LOGIC_VECTOR;
  function to_slv (arg : sfixed; width : INTEGER ) return STD_LOGIC_VECTOR;

  --===========================================================================
  --   only casting Functions : no conversion function (no resize(overflow/quantization)) !!!
  --===========================================================================
  function to_slv (arg : ufixed) return STD_LOGIC_VECTOR;
  function to_slv (arg : sfixed) return STD_LOGIC_VECTOR;

  function to_fixed (
    arg                  : UNSIGNED;    -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER) return ufixed;
    
  function to_fixed (
    arg                  : SIGNED;      -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return sfixed;

  function to_ufixed (
    arg                  : STD_LOGIC_VECTOR;  -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return ufixed;

  function to_sfixed (
    arg                  : STD_LOGIC_VECTOR;  -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return sfixed;


  -- casted version
  function to_ufixed (arg : UNSIGNED) -- unsigned
    return ufixed;


  -- casted version
  function to_sfixed (arg : SIGNED)                       -- signed
    return sfixed;



  -- sign extend and casted version
  function to_ufixed (arg : UNSIGNED;constant size  : INTEGER) -- unsigned
    return ufixed;


  -- sogne extend and casted version
  function to_sfixed (arg : SIGNED;constant size  : INTEGER)                       -- signed
    return sfixed;


  function to_unsigned (
    arg                     : ufixed   -- ufixed point input
    )
    return UNSIGNED;

  function to_signed (
    arg                     : sfixed   -- sfixed point input
    )
    return SIGNED;

  function to_signed (
    arg                     : ufixed   -- ufixed point input
    )
    return SIGNED;

  function sfixed_to_real(x : in sfixed; fractionalFactor : integer) return real;
  function ufixed_to_real(x : in ufixed; fractionalFactor : integer) return real;

end package fixed_pkg;
