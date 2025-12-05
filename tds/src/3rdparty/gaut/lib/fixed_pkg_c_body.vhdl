library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;

package body fixed_pkg is

  function maximum (l, r : INTEGER)
    return INTEGER is
  begin  -- function maximum
    if L > R then return L;
    else return R;
    end if;
  end function maximum;

  function minimum (l, r : INTEGER)
    return INTEGER is
  begin  -- function minimum
    if L > R then return R;
    else return L;
    end if;
  end function minimum;

  function or_reducex (arg : STD_LOGIC_VECTOR)
    return STD_LOGIC is
    variable Result       : STD_LOGIC := '0';
  begin
      for i in arg'range loop
         Result := Result or arg(i);
      end loop;
    return Result;
  end function or_reducex;


  function and_reducex (arg : STD_LOGIC_VECTOR)
    return STD_LOGIC is
    variable Result       : STD_LOGIC := '1';
  begin
      for i in arg'range loop
         Result := Result and arg(i);
      end loop;
    return Result;
  end function and_reducex;


-----------------------------------------------------------------------------
-- only sign extend Functions : (no resize(overflow/quantization)) !!!
-----------------------------------------------------------------------------
function to_slv (arg : ufixed; width : INTEGER ) return STD_LOGIC_VECTOR is
    subtype t is STD_LOGIC_VECTOR (arg'high - arg'low downto 0);
    variable slv : t;
    variable result : STD_LOGIC_VECTOR (width-1 downto 0);
  begin
    slv := t (arg);
    result(t'high downto 0):=slv;
    --result(width downto t'high+1) := (others => '0');
    result(width-1 downto slv'high+1) := (others => '0');
 return result;
  end function to_slv;

function to_slv (arg : sfixed; width : INTEGER ) return STD_LOGIC_VECTOR is
    subtype t is STD_LOGIC_VECTOR (arg'high - arg'low downto 0);
    variable slv : t;
    variable result : STD_LOGIC_VECTOR (width-1 downto 0);
  begin
    slv := t (arg);
    result(t'high downto 0):=slv;
    --result(width downto t'high+1) := (others => slv(t'high));
    result(width-1 downto slv'high+1) := (others => slv(slv'high));
    return result;
  end function to_slv;

-----------------------------------------------------------------------------
-- casting functions
-----------------------------------------------------------------------------

  -- These are needed for synthesis where typically
  -- the only input and output type is a std_logic_vector.
  function to_slv (arg : ufixed)                       -- fixed point vector
    return STD_LOGIC_VECTOR is
    subtype t is STD_LOGIC_VECTOR (arg'high - arg'low downto 0);
    variable slv : t;
  begin
    slv := t (arg);
    return slv;
  end function to_slv;

  function to_slv (
    arg : sfixed)                       -- fixed point vector
    return STD_LOGIC_VECTOR is
    subtype t is STD_LOGIC_VECTOR (arg'high - arg'low downto 0);
    variable slv : t;
  begin
    slv := t (arg);
    return slv;
  end function to_slv;

  function to_ufixed (
    arg                  : STD_LOGIC_VECTOR;  -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return ufixed is
    variable result : ufixed (left_index downto right_index);
  begin
      result := to_fixed (arg         => UNSIGNED(arg),
				left_index  => left_index,
	                  right_index    => right_index);
      return result;
  end function to_ufixed;



  function to_sfixed (
    arg                  : STD_LOGIC_VECTOR;  -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return sfixed is
    variable result : sfixed (left_index downto right_index);
  begin
      result := to_fixed (arg         => SIGNED(arg),
                          left_index  => left_index,
				               right_index    => right_index);
      return result;
  end function to_sfixed;

  -- casted version
  function to_ufixed (
    arg : UNSIGNED)                     -- unsigned
    return ufixed is
    constant ARG_LEFT : INTEGER := ARG'length-1;
    alias XARG        : UNSIGNED(ARG_LEFT downto 0) is ARG;
  begin
    return ufixed(xarg);
  end function to_ufixed;



    -- sign extend and casted version
  function to_ufixed (arg : UNSIGNED;constant size  : INTEGER) -- unsigned
    return ufixed is
  begin
    return ufixed(resize(arg,size));
  end function to_ufixed;


  -- sogne extend and casted version
  function to_sfixed (arg : SIGNED;constant size  : INTEGER)                       -- signed
    return sfixed is
  begin
    return sfixed(resize(arg,size));
  end function to_sfixed;



  -- casted version
  function to_sfixed (
    arg : SIGNED)                       -- signed
    return sfixed is
    constant ARG_LEFT : INTEGER := ARG'length-1;
    alias XARG        : SIGNED(ARG_LEFT downto 0) is ARG;
  begin
    return sfixed(xarg);
  end function to_sfixed;


-----------------------------------------------------------------------------
-- Saturate functions
-----------------------------------------------------------------------------
  -- purpose: returns a saturated number for an unsigned number
  -- AC_WRAP (Wrap-around): 	Any MSB bits outside the range are deleted (modulo)
  -- AC_SAT (Saturation) : 	MAX if overflow
  --			MIN if underflow
  -- AC_SAT_SYM (Symmetrical Saturation): MAX if overflow
  --			-MAX if underflow
  -- AC_SAT_ZERO (Saturation to Zero) : O if owerflow
  --			O if underflow
  function saturate (
    constant number: ufixed;
    constant overflow_style : overflow_mode;
    constant overflow_flag : BOOLEAN;
    constant underflow_flag : BOOLEAN )
    return ufixed is
    constant left_index : INTEGER := number'high;
    constant right_index: INTEGER := number'low;
    constant MAX : ufixed (left_index downto right_index) := (others => '1');
    constant ZERO : ufixed (left_index downto right_index) := (others => '0');
    variable sat : ufixed (left_index downto right_index):=number;
  begin
    if (overflow_flag =TRUE) then
	   if ((overflow_style=AC_SAT) OR (overflow_style=AC_SAT_SYM)) then
		   sat:=MAX;
	   elsif (overflow_style/=AC_WRAP) then
		   sat:=ZERO ;
      end if;
    elsif ((underflow_flag =TRUE) AND (overflow_style/=AC_WRAP)) then
		sat:=ZERO ;	
    end if;	
    return sat;
  end function saturate;


  -- purpose: returns a saturated number for an signed number
  -- AC_WRAP (Wrap-around): 	Any MSB bits outside the range are deleted (modulo)
  -- AC_SAT (Saturation) : 	MAX if overflow
  --			MIN if underflow
  -- AC_SAT_SYM (Symmetrical Saturation): MAX if overflow
  --			-MAX if underflow
  -- AC_SAT_ZERO (Saturation to Zero) : O if owerflow
  --			O if underflow
  function saturate (
    constant number: sfixed;
    constant overflow_style : overflow_mode;
    constant overflow_flag : BOOLEAN;
    constant underflow_flag : BOOLEAN )
    return sfixed is
    constant left_index : INTEGER := number'high;
    constant right_index: INTEGER := number'low;
    constant MAX : sfixed (left_index downto right_index) := '0'&(left_index-1 downto right_index  => '1');
    constant ZERO : sfixed (left_index downto right_index) := (others => '0');
    constant MIN : sfixed (left_index downto right_index) := '1'&(left_index-1 downto right_index => '0');
   constant MOINSMAX : sfixed (left_index downto right_index) := '1'&((left_index-1) downto (right_index+1) => '0')&'1';
    variable sat : sfixed (left_index downto right_index):=number (left_index downto right_index);
  begin
    if (overflow_flag =TRUE) then
	   if ((overflow_style=AC_SAT) OR (overflow_style=AC_SAT_SYM)) then
		   sat:=MAX ;
	   elsif (overflow_style/=AC_WRAP) then
		   sat:=ZERO ;
	   end if;
   elsif (underflow_flag =TRUE)  then
	   if (overflow_style=AC_SAT) then
		   sat:=MIN ;	
	   elsif (overflow_style=AC_SAT_SYM )then
		   sat:=MOINSMAX ;	
	   elsif (overflow_style=AC_SAT_ZERO) then
		   sat:=ZERO ;
	   end if; 		
    end if;	
    return sat;
  end function saturate;

-----------------------------------------------------------------------------
-- Rounding functions
-----------------------------------------------------------------------------
  -- Type cast a "unsigned" into a "ufixed", used internally
  function to_fixed (
    arg                  : UNSIGNED;    -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return ufixed is
    variable result : ufixed (left_index downto right_index);
  begin  -- function to_fixed
    result := ufixed(arg);
    return result;
  end function to_fixed;

  -- Type cast a "signed" into an "sfixed", used internally
  function to_fixed (
    arg                  : SIGNED;      -- shifted vector
    constant left_index  : INTEGER;
    constant right_index : INTEGER)
    return sfixed is
    variable result : sfixed (left_index downto right_index);
  begin  -- function to_fixed
    result := sfixed(arg);
    return result;
  end function to_fixed;

  -- Type cast a "ufixed" into an "unsigned"
  function to_unsigned (
    arg : ufixed)                       -- fp vector
    return UNSIGNED is
    subtype  t is UNSIGNED(arg'high - arg'low downto 0);
    variable slv : t;
  begin  -- function to_uns
    slv := t(arg);
    --return UNSIGNED(to_X01(std_logic_vector(slv)));
    return slv;
  end function to_unsigned;

  -- Type cast an "sfixed" into a "signed"
  function to_signed (
    arg : sfixed)                       -- fp vector
    return SIGNED is
    subtype  t is SIGNED(arg'high - arg'low downto 0);
    variable slv : t;
  begin  -- function to_s
    slv := t(arg);
    --return SIGNED(to_X01(std_logic_vector(slv)));
    return slv;
  end function to_signed;


  -- Type cast an "ufixed" into a "signed"
  function to_signed (
    arg : ufixed)                       -- fp vector
    return SIGNED is
    subtype  t is SIGNED(arg'high - arg'low downto 0);
    variable slv : t;
  begin  -- function to_s
    slv := t(arg);
    --return SIGNED(to_X01(std_logic_vector(slv)));
    return slv;
  end function to_signed;

  -- adds 1 to the LSB of the number
  procedure round_up (arg       : in  ufixed;
                      result    : out ufixed;
                      overflowx : out BOOLEAN) is
    variable arguns, resuns : UNSIGNED (arg'high-arg'low+1 downto 0) :=
      (others => '0');--use temporary vector of size+1 to detect overflow with additional MSB 
  begin  -- round_up
    arguns (arguns'high-1 downto 0) := to_unsigned (arg);
    resuns                          := arguns + 1;
    result := to_fixed(resuns(arg'high-arg'low
                              downto 0), arg'high, arg'low);
    overflowx := (resuns(resuns'high) = '1');
  end procedure round_up;


  -- sub 1 to the LSB of the number
  procedure round_down (arg       : in  ufixed;
                      result    : out ufixed;
                      underflowx : out BOOLEAN) is
    variable arguns, resuns : UNSIGNED (arg'high-arg'low downto 0) :=
      (others => '0');
  begin  -- round_down
    arguns (arguns'high-1 downto 0) := to_unsigned (arg);
    resuns                          := arguns -1;
    result := to_fixed(resuns(arg'high-arg'low
                              downto 0), arg'high, arg'low);
    underflowx := (resuns(resuns'high) = '1');--true if arg is 0
  end procedure round_down;


  -- adds 1 to the LSB of the number
  procedure round_up (arg       : in  sfixed;
                      result    : out sfixed;
                      overflowx : out BOOLEAN) is
    variable args, ress : SIGNED (arg'high-arg'low+1 downto 0); --use temporary vector of size+1 (wuth sign extend) to detect overflow with additional MSB 
  begin  -- round_up
    args (args'high-1 downto 0) := to_signed (arg);
    args(args'high)             := arg(arg'high);  -- sign extend
    ress                        := args + 1;
    result := to_fixed(ress (ress'high-1
                             downto 0), arg'high, arg'low);
    overflowx := ((arg(arg'high) /= ress(ress'high-1))--ress sign bitn != arg sign msb
                  and (or_reducex (STD_LOGIC_VECTOR(ress)) /= '0')); --all bits except sign bits are 0
  end procedure round_up;


  -- sub 1 to the LSB of the number
  procedure round_down (arg       : in  sfixed;
                      result    : out sfixed;
                      underflowx : out BOOLEAN) is
    variable args, ress : SIGNED (arg'high-arg'low downto 0) := (others => '0');
  begin  -- round_down
    args := to_signed (arg);
    ress                          := args -1;
    result := to_fixed(ress(arg'high-arg'low
                              downto 0), arg'high, arg'low);
    underflowx := ((arg(arg'high) /= ress(ress'high-1))--ress sign bitn != arg sign msb
                  and (and_reducex (STD_LOGIC_VECTOR(ress)) /= '1')); --all bits except sign bits are 1
  end procedure round_down;


  -- if the most significant deleted bit is one, and either the least significant of the
  -- remaining bist or at least one of the other delected bits is one, then add one
  -- to the remaining bits
  function convergent_rounding (arg            : ufixed;
                        remainder      : ufixed;
                        overflow_style : overflow_mode)
    return ufixed is
    variable rounds         : BOOLEAN;
    variable round_overflow : BOOLEAN;
    variable result         : ufixed (arg'range);
  begin
    rounds := false;
    if (remainder'length > 1) then
      if (remainder (remainder'high) = '1') then
        rounds := (arg(arg'low) = '1')
                  or (or_reducex (to_slv(remainder(remainder'high-1 downto
                                                   remainder'low))) = '1');
      end if;
    else
      rounds := (arg(arg'low) = '1') and (remainder (remainder'high) = '1');
    end if;
    if rounds then
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overflow,false);
    return result;
  end function convergent_rounding;

  -- if the most significant deleted bit is one, and either the least significant of the
  -- remaining bist or at least one of the other delected bits is one, then add one
  -- to the remaining bits
  function convergent_rounding (arg            : sfixed;
                        remainder      : sfixed;
                        overflow_style : overflow_mode)
    return sfixed is
    variable rounds         : BOOLEAN;
    variable round_overflow : BOOLEAN;
    variable result         : sfixed (arg'range);
  begin
    rounds := false;
    if (remainder'length > 1) then
      if (remainder (remainder'high) = '1') then
        rounds := (arg(arg'low) = '1')
                  or (or_reducex (to_slv(remainder(remainder'high-1 downto
                                                   remainder'low))) = '1');
      end if;
    else
      rounds := (arg(arg'low) = '1') and (remainder (remainder'high) = '1');
    end if;
    if rounds then
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overflow,false);
    return result;
  end function convergent_rounding;


  --toward Positive Infinity 
  function round_toward_plus_infinity (arg            : ufixed;
                        remainder      : ufixed;
                        overflow_style : overflow_mode)
    return ufixed is
    variable rounds         : BOOLEAN;
    variable round_overflow : BOOLEAN;
    variable result         : ufixed (arg'range);
  begin
    --The MSB of the deleted bits is nonzero (x>=0.5)  
    rounds := remainder (remainder'high) = '1';
    if rounds then --- add the MSB of the removed bits to the remaining bits
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overflow,false);
    return result;
  end function round_toward_plus_infinity;

  -- Rounding toward plus infinity 
  function round_toward_plus_infinity(arg            : sfixed;
                        remainder      : sfixed;
                        overflow_style : overflow_mode)
    return sfixed is
    variable rounds         : BOOLEAN;
    variable round_overflow : BOOLEAN;
    variable result         : sfixed (arg'range);
  begin
    --The MSB of the deleted bits is nonzero (x>=0.5)  
    rounds := remainder(remainder'high)='1';
    if rounds then  --- add the MSB of the removed bits to the remaining bits
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overflow,false);
    return result;
  end function round_toward_plus_infinity;

   
  -- truncate toward zero (for postive number trunc and  for  negative number round_toward_Zero) 
  function trunc_toward_zero(arg            : sfixed;
                        remainder      : sfixed;
                        overflow_style : overflow_mode)
    return sfixed is
    variable rounds         : BOOLEAN;
    variable round_overflow : BOOLEAN;
    variable result         : sfixed (arg'range);
  begin
    --at least one of the deleted bits is nonzero  
    if (remainder'length > 1) then
      rounds := or_reducex (to_slv(remainder(remainder'high downto remainder'low)))='1';
    else  
      rounds := remainder(remainder'high)='1';
    end if;
    if rounds then  --- add the sign bit to the LSBs on the remaining bits
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overflow,false);
    return result;
  end function trunc_toward_zero;


  -- round toward negative infinity  
  function round_toward_minus_infinity(arg            : sfixed;
                        remainder      : sfixed;
                        overflow_style : overflow_mode)
    return sfixed is
    variable rounds         : BOOLEAN;
    variable round_overderflow : BOOLEAN;
    variable result         : sfixed (arg'range);
  begin
  if (remainder'length>1) then 
     --if (x>0.5)or (x>-0.5) round_up else trunc 
    rounds := (remainder(remainder'high)='1') and
                  (or_reducex (to_slv(remainder(remainder'high-1 downto
                                                   remainder'low))) = '1');
   else rounds := false;
   end if;   
  if rounds then  --- round_up
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overderflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overderflow,false);
    return result;
  end function round_toward_minus_infinity;

  -- round toward negative infinity  
  function round_toward_minus_infinity(arg            : ufixed;
                        remainder      : ufixed;
                        overflow_style : overflow_mode)
    return ufixed is
    variable rounds         : BOOLEAN;
    variable round_overderflow : BOOLEAN;
    variable result         : ufixed (arg'range);
  begin
  if (remainder'length>1) then 
     --if (x>0.5)or (x>-0.5) round_up else trunc 
    rounds := (remainder(remainder'high)='1') and
                  (or_reducex (to_slv(remainder(remainder'high-1 downto
                                                   remainder'low))) = '1');
   else rounds := false;
   end if;   
   if rounds then  --- round_up
      round_up(arg       => arg,
               result    => result,
               overflowx => round_overderflow);
    else
      result := arg;
    end if;
    result := saturate (result,overflow_style,round_overderflow,false);
    return result;
  end function round_toward_minus_infinity;

  function quantization (
    arg                     : ufixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant overflow_style : overflow_mode;  -- overflow
    constant round_style    : quantization_mode)     -- rounding
    return ufixed is
    variable result  : ufixed(left_index downto right_index) :=(others => '0');
    begin    
       case round_style is
          when AC_TRN => --the number is truncated (truncation)
                result:=arg(left_index downto right_index);
          when AC_TRN_ZERO => --the number is truncated to zero (truncation to zero)
                result:=arg(left_index downto right_index);
          when AC_RND => --rounding to plus infinity
                 result:=round_toward_plus_infinity (arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
          when AC_RND_ZERO =>--Rounding to zero 
                   result:=round_toward_minus_infinity(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
          when AC_RND_INF =>--rounding to infinity
                 result:=round_toward_plus_infinity (arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);              
          when AC_RND_MIN_INF =>--rounding to minus infinity
                 result:=round_toward_minus_infinity (arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);              
          when AC_RND_CONV =>--convergent rounding
              result:=convergent_rounding(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
     end case;
    return result;
  end function quantization;

  function quantization (
    arg                     : sfixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant overflow_style : overflow_mode;  -- overflow
    constant round_style    : quantization_mode)     -- rounding
    return sfixed is
    variable result  : sfixed(left_index downto right_index) :=(others => '0');
    constant sign_bit : std_logic := arg(arg'high);     
    variable overflow_flag : BOOLEAN := false;--for positive number reduced
    variable underflow_flag : BOOLEAN := false;--for negative number reduced
    begin    
      case round_style is
            when AC_TRN =>  --truncation 
               result:=arg(left_index downto right_index); --the number is truncated
            when AC_TRN_ZERO =>--truncation to zero
                if (sign_bit='0') then --positive number
                   result:=arg(left_index downto right_index); --the number is truncated
                else 
                   result:=trunc_toward_zero(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
               end if;   
            when AC_RND => --rounding to plus infinity
                 result:=round_toward_plus_infinity (arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);  
           when AC_RND_INF =>--rounding to infinity
                if (sign_bit='0') then --positive number
                    result:=round_toward_plus_infinity (arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);  
                else 
                   result:=round_toward_minus_infinity(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
               end if;   
          when AC_RND_CONV =>--convergent rounding
              result:=convergent_rounding(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
          when AC_RND_ZERO =>--Rounding to zero
                 if (sign_bit='0') then --positive number
                   result:=round_toward_minus_infinity(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
                else 
                   result:=round_toward_plus_infinity(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
            end if;   
          when AC_RND_MIN_INF =>--rounding to minus infinity
                   result:=round_toward_minus_infinity(arg(left_index downto right_index),arg(right_index-1 downto arg'low),overflow_style);
     end case;
    return result;
  end function quantization;

-----------------------------------------------------------------------------
-- Resizing functions
-----------------------------------------------------------------------------
-- For unsigned results, overflow occurs if any bits beyond the most significant bit of result are
-- non-zero.
  function resize (
    arg                     : ufixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant round_style    : quantization_mode;     -- rounding
    constant overflow_style : overflow_mode)  -- overflow
    return ufixed is
    constant indexhigh : INTEGER := maximum (arg'high, left_index);
    constant indexlow  : INTEGER := minimum (right_index, arg'low);
    constant INTEGER_PART_SIZE_OUTPUT : INTEGER := maximum(0,left_index);
    constant INTEGER_PART_SIZE_INPUT : INTEGER := maximum(0,arg'high);
    constant FRACTIONAL_PART_SIZE_OUTPUT : INTEGER := ABS(minimum(0,right_index));
    constant FRACTIONAL_PART_SIZE_INPUT : INTEGER := ABS(minimum(0,arg'low));
    variable result  : ufixed(indexhigh downto indexlow) := (others => '0');
    variable overflow_flag : BOOLEAN := false;--underflow always false for unsigned number (always >= 0)
    
  begin   
   
  if (INTEGER_PART_SIZE_OUTPUT<INTEGER_PART_SIZE_INPUT) then --decrease integer part : need saturate ?
        --overflow ?
        overflow_flag:=(or_reducex(to_slv(arg(arg'high downto left_index+1))) = '1');
        if (overflow_flag) then
           return saturate (result(left_index downto right_index),overflow_style,true,false); --overflow and satuaration
        end if;
        if (FRACTIONAL_PART_SIZE_INPUT>FRACTIONAL_PART_SIZE_OUTPUT) then --decrease fractional part : need rounding
            return quantization(arg(left_index downto arg'low),left_index,right_index,overflow_style,round_style);
        else --increase fractional part : add zero for additional bit
           result(left_index downto  arg'low):=arg(left_index downto  arg'low);
           --result(arg'low-1 donwto right_index):=(others=>'0'); done by initialization in declaration   
           return result(left_index downto right_index);
        end if;   
  else --increase integer part : add zero for additional bit
        --result(left_index donwto arg'high+1):=(others=>'0'); done by initialization in declaration 
        result(arg'high downto  arg'low):=arg(arg'high downto  arg'low);
        if (FRACTIONAL_PART_SIZE_INPUT>FRACTIONAL_PART_SIZE_OUTPUT) then --decrease fractional part : need rounding
            return quantization(result ,left_index,right_index,overflow_style,round_style);
        else --increase fractional part : add zero for additional bit
           --result(larg'low-1 donwto right_index):=(others=>'0'); done by initialization in declaration 
           return result(left_index downto right_index);
        end if;   
  end if;
  end function resize;

  function resize_s2u (
    arg                     : sfixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant round_style    : quantization_mode;     -- rounding
    constant overflow_style : overflow_mode)  -- overflow
    return ufixed is
    constant sign_bit : std_logic := arg(arg'high);
    variable convert_arg  : ufixed(arg'high downto arg'low) := (others => '0');
    variable zero  : ufixed(left_index downto right_index) := (others => '0');
  begin
	if (sign_bit='0') then --positive number
		convert_arg  := ufixed(arg); --cast sfixed to ufixed
		return resize(convert_arg,left_index,right_index,round_style,overflow_style);
	else --negative number return 0
		return zero;
	end if;

  end function resize_s2u;

-- For signed results, overflow occurs if any bits beyond the most significant bit of result are
--  different from the most significant bit of the result.
  function resize (
    arg                     : sfixed;   -- input
    constant left_index     : INTEGER;  -- integer portion
    constant right_index    : INTEGER;  -- size of fraction
    constant round_style    : quantization_mode;     -- rounding
    constant overflow_style : overflow_mode)  -- overflow
    return sfixed is
    constant indexhigh : INTEGER := maximum (arg'high, left_index);
    constant indexlow  : INTEGER := minimum (right_index, arg'low);
    constant sign_bit : std_logic := arg(arg'high);   
    variable result  : sfixed(indexhigh downto indexlow) := (others => '0');
    variable overflow_flag : BOOLEAN := false;--for positive number reduced
    variable underflow_flag : BOOLEAN := false;--for negative number reduced
    constant INTEGER_PART_SIZE_OUTPUT : INTEGER := maximum(0,left_index);
    constant INTEGER_PART_SIZE_INPUT : INTEGER := maximum(0,arg'high);
    constant FRACTIONAL_PART_SIZE_OUTPUT : INTEGER := ABS(minimum(0,right_index));
    constant FRACTIONAL_PART_SIZE_INPUT : INTEGER := ABS(minimum(0,arg'low));
  begin   
  if (INTEGER_PART_SIZE_OUTPUT<INTEGER_PART_SIZE_INPUT) then --decrease integer part : need saturate ?
        --overflow/underflow ?
        if (sign_bit='0') then --positive number
           overflow_flag:= (or_reducex(to_slv(arg(arg'high-1 downto left_index))) = '1');--at least one bit is '1' (different of sign bit) ?
        else -- negative number            
           underflow_flag:= (and_reducex(to_slv(arg(arg'high-1 downto left_index))) = '0'); --at least one bit is '0' (different of sign bit) ?
        end if;           
        if (overflow_flag) then
           return saturate (result(left_index downto right_index),overflow_style,true,false); --saturate to MAX or ZERO
        end if;
        if (underflow_flag) then
           return saturate (result(left_index downto right_index),overflow_style,false,true); --saturate to -MAX, MIN or ZERO
        end if;
        -- no saturation necessary for integer part reduction
        result(left_index downto arg'low):=sign_bit&arg(left_index-1 downto arg'low);
        if (FRACTIONAL_PART_SIZE_OUTPUT <FRACTIONAL_PART_SIZE_INPUT ) then --decrease fractional part : need rounding
            return quantization(result(left_index downto arg'low),left_index,right_index,overflow_style,round_style);
        else --increase fractional part : add zero for additional bit
           --result(arg'low-1 downto right_index):=(others=>'0'); done by initialization in declaration   
           return result(left_index downto right_index);
        end if;   
  elsif(INTEGER_PART_SIZE_OUTPUT>INTEGER_PART_SIZE_INPUT) then  --increase integer part : add sign bit for additional bit
        result(left_index downto arg'high+1):=(others=>sign_bit); 
        result(arg'high downto  arg'low):=arg(arg'high downto  arg'low);
        if (FRACTIONAL_PART_SIZE_OUTPUT <FRACTIONAL_PART_SIZE_INPUT )  then --decrease fractional part : need rounding
            return quantization(result ,left_index,right_index,overflow_style,round_style);
        else --increase fractional part : add zero for additional bit
           --result(larg'low-1 donwto right_index):=(others=>'0'); done by initialization in declaration 
           return result(left_index downto right_index);
        end if;
   else
	  result(arg'high downto  arg'low):=arg(arg'high downto  arg'low);
        if (FRACTIONAL_PART_SIZE_OUTPUT <FRACTIONAL_PART_SIZE_INPUT)  then --decrease fractional part : need rounding
            return quantization(result ,left_index,right_index,overflow_style,round_style);
        else --increase fractional part : add zero for additional bit
           --result(larg'low-1 donwto right_index):=(others=>'0'); done by initialization in declaration 
           return result(left_index downto right_index);
        end if;
  end if;
  end function resize;

 -----------------------------------------------------------------------------
-- Conversion functions
-----------------------------------------------------------------------------
 function to_slv (arg : ufixed; -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return STD_LOGIC_VECTOR is 
    variable resized  : ufixed(left_index downto right_index);
   begin       
    resized:= resize (arg,left_index,right_index,round_style,overflow_style);
    return to_slv(resized);
end function to_slv;

function to_slv (arg : sfixed; -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return STD_LOGIC_VECTOR is
       variable resized  : sfixed(left_index downto right_index);
   begin       
    resized:= resize (arg,left_index,right_index,round_style,overflow_style);
    return to_slv(resized);
end function to_slv;
       
function to_sfixed (
       arg                  : STD_LOGIC_VECTOR;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction (negatif)
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return sfixed is
constant arg_size  : integer :=arg'high-arg'low;
constant arg_left  : integer :=arg_size + right_index;
variable arg_cast  : sfixed(arg_left downto right_index);
variable resized  : sfixed(left_index downto right_index);
begin       
    arg_cast:=to_sfixed(arg,arg_left,right_index);
    resized:= resize (arg_cast,left_index,right_index,round_style,overflow_style);
    return resized;
end function to_sfixed;
       
function to_sfixed (
       arg                  : SIGNED;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode; 
       constant overflow_style : overflow_mode)
       return sfixed is
constant arg_size  : integer :=arg'high-arg'low;
constant arg_left  : integer :=arg_size + right_index;
variable arg_cast  : sfixed(arg_left downto right_index);
variable resized  : sfixed(left_index downto right_index);
begin       
    arg_cast:=to_fixed(arg,arg_left,right_index);
    resized:= resize (arg_cast,left_index,right_index,round_style,overflow_style);
    return resized;
end function to_sfixed;
 
function to_ufixed (
       arg                  : STD_LOGIC_VECTOR;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode;
       constant overflow_style : overflow_mode)
       return ufixed is
constant arg_size  : integer :=arg'high-arg'low;
constant arg_left  : integer :=arg_size + right_index;
variable arg_cast  : ufixed(arg_left downto right_index);
variable resized  : ufixed(left_index downto right_index);
begin
    arg_cast:=to_ufixed(arg,arg_left,right_index);
    resized:= resize (arg_cast,left_index,right_index,round_style,overflow_style);
    return resized;
end function to_ufixed;

function to_ufixed (
       arg                  : UNSIGNED;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode;
       constant overflow_style : overflow_mode)
       return ufixed is
constant arg_size  : integer :=arg'high-arg'low;
constant arg_left  : integer :=arg_size + right_index;
variable arg_cast  : ufixed(arg_left downto right_index);
variable resized  : ufixed(left_index downto right_index);
begin
    arg_cast:=to_fixed(arg,arg_left,right_index);
    resized:= resize (arg_cast,left_index,right_index,round_style,overflow_style);
    return resized;
end function to_ufixed;


function to_ufixed (
       arg                  : SIGNED;  -- input
       constant left_index     : INTEGER;  -- integer portion
       constant right_index    : INTEGER;  -- size of fraction
       constant round_style    : quantization_mode;
       constant overflow_style : overflow_mode)
       return ufixed is
variable arg_signed  : signed(arg'high+1 downto arg'low);
begin
          arg_signed:="0"&arg;
          return to_ufixed(arg_signed,left_index,right_index,round_style,overflow_style);
end function to_ufixed;

function to_unsigned (
    arg                     : ufixed;   -- ufixed point input
    size : INTEGER  -- size >=arg'length : add 0
    )
    return UNSIGNED is
    variable uns :     UNSIGNED(arg'high - arg'low downto 0);
    variable result :     UNSIGNED(size-1 downto 0);
begin
	uns:=to_unsigned(arg);
	result:=resize(uns,size);
	return result;	
end function to_unsigned;

  function to_signed (
    arg                     : sfixed;   -- sfixed point input
    size : INTEGER  -- size >=arg'length : sign extend only
    )
    return SIGNED is
    variable sgn :     SIGNED(arg'high - arg'low downto 0);
    variable result :     SIGNED(size-1 downto 0);
begin
	sgn:=to_signed(arg);
	result:=resize(sgn,size);
	return result;	
end function to_signed;

  function to_signed (
    arg                     : ufixed;   -- sfixed point input
    size : INTEGER  -- size >=arg'length : sign extend only
    )
    return SIGNED is
    variable sgn :     SIGNED(arg'high - arg'low downto 0);
    variable result :     SIGNED(size-1 downto 0);
begin
	sgn:=to_signed(arg);
	result:=resize(sgn,size);
	return result;	
end function to_signed;

function sfixed_to_real(x : in sfixed; fractionalFactor : integer) return real is
variable tmp : integer;
variable r : real;
begin
	tmp:= TO_INTEGER(to_signed(x));
	r:=real(tmp)/real(fractionalFactor);
	return r;
end function sfixed_to_real;

function ufixed_to_real(x : in ufixed; fractionalFactor : integer) return real is
variable tmp : integer;
variable r : real;
begin
	tmp:= TO_INTEGER(to_unsigned(x));
	r:=real(tmp)/real(fractionalFactor);
	return r;
end function ufixed_to_real;

end package body fixed_pkg;
