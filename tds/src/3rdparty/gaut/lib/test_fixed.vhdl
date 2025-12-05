-- Test for the fixed point package

entity test_fixed is
end entity test_fixed;

use std.textio.all;
use work.fixed_pkg.all;
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

architecture testbench of test_fixed is
    
  subtype ufixed8_3 is ufixed (4 downto -3);   -- 8 bit
  subtype ufixed4_1 is ufixed (2 downto -1);   -- 4 bit
  subtype ufixed6_3 is ufixed (2 downto -3);   -- 6 bit
  subtype sfixed8_3 is sfixed (4 downto -3);   -- 8 bit
  subtype sfixed4_1 is sfixed (2 downto -1);   -- 4 bit

begin  -- architecture testbench

   -- purpose: Basic test for the fixed point package
  -- type   : combinational
  basictest : process is
    variable check8uf3_1              : ufixed8_3;
    variable check8uf3_2              : ufixed8_3;
    variable check4uf1_1              : ufixed4_1;
    variable check4uf1_2              : ufixed4_1;
    variable check6uf3              : ufixed6_3;
    variable check8sf3_1              : sfixed8_3;
    variable check4sf1_1              : sfixed4_1;
    variable check4sf1_2              : sfixed4_1;
    variable check4sf2 : sfixed(1 downto -2);
    variable check3sf1_1 : sfixed(1 downto -1);
    variable check3sf1_2 : sfixed(1 downto -1);
    variable check16sf8 : sfixed(7 downto -8);
    variable check12sf4_1 : sfixed(7 downto -4);
    variable check12sf4_2 : sfixed(7 downto -4);
    variable check14uf5 : sfixed(8 downto -5);
    variable check13sf4_1 : sfixed(8 downto -4);
    variable check13sf4_2 : sfixed(8 downto -4);
    variable result 			: unsigned(15 downto 0);

  begin  -- process basictest
    -- resize test
    check8uf3_1              := "00011000";               -- 3
    check4uf1_1 :=resize (check8uf3_1,ufixed4_1'high, ufixed4_1'low,
                       AC_TRN,AC_WRAP);
    check4uf1_2              := "0110";               -- expected 3
    assert (check4uf1_1 = check4uf1_2) report "error resize" severity error;
    -- overflow & saturation test for unsigned number
    check8uf3_1              := "01000000";               -- 8
    check4uf1_1 :=resize (check8uf3_1,ufixed4_1'high, ufixed4_1'low,
                       AC_TRN,AC_SAT);
    check4uf1_2              := "1111";               -- expected MAX
    assert (check4uf1_1 = check4uf1_2) report "error resize : overflow/sat " severity error;
    check8uf3_1              := "01000000";               -- 8
    check4uf1_1 :=resize (check8uf3_1,ufixed4_1'high, ufixed4_1'low,
                       AC_TRN,AC_SAT_ZERO);
    check4uf1_2              := "0000";               -- expected ZERO
    assert (check4uf1_1 = check4uf1_2) report "error resize : overflow/sat_zero " severity error;
    -- quantization test for unsigned number
    check8uf3_1              := "00011110";               -- 3.75
    check4uf1_1 :=resize (check8uf3_1,ufixed4_1'high, ufixed4_1'low,
                       AC_TRN,AC_WRAP);
    check4uf1_2              := "0111";               -- expected 3.5  -- TRUNC
    assert (check4uf1_1 = check4uf1_2) report "error resize : quantization/trunc " severity error;
    check8uf3_1              := "00011110";               -- 3.75
    check4uf1_1 :=resize (check8uf3_1,ufixed4_1'high, ufixed4_1'low,
                       AC_RND,AC_WRAP);
    check4uf1_2              := "1000";               -- expected 4  -- RND
    assert (check4uf1_1 = check4uf1_2) report "error resize : quantization/rnd " severity error;
    -- overflow & saturation test for signed number
    check8sf3_1              := "11000000";               -- 8
    check4sf1_1 :=resize (check8sf3_1,sfixed4_1'high, sfixed4_1'low,
                       AC_TRN,AC_SAT);
    check4sf1_2              := "1000";               -- expected MIN (-4)
    assert (check4sf1_1 = check4sf1_2) report "error resize : underflow/sat " severity error;
    check8sf3_1              := "11000000";               -- -8
    check4sf1_1 :=resize (check8sf3_1,sfixed4_1'high, sfixed4_1'low,
                       AC_TRN,AC_SAT_SYM);
    check4sf1_2              := "1001";               -- expected -MAX (-3.5)
    assert (check4sf1_1 = check4sf1_2) report "error resize : underflow/sat " severity error;
    -- quantization test for signed number
    check8sf3_1              := "11111111";               -- -16+15.75=-0.25
    check4sf1_1 :=resize (check8sf3_1,sfixed4_1'high, sfixed4_1'low,
                       AC_TRN,AC_WRAP);
    check4sf1_2              := "1111";               -- expected -0.5  -- TRUNC
    assert (check4sf1_1 = check4sf1_2) report "error resize : quantization/trunc " severity error;

    -- test resize and operator for unsigned number
    check8uf3_1              := "00011000";               -- 3
    check4uf1_2              := "0010";               -- 1
    check6uf3              := "0010"&"00"; --align fixed point (done by gaut kernel : assignation)
    --addition on add_u12_u16_u16 bits unsigned : library operator
    result:=resize(to_unsigned(check8uf3_1),12)+resize(to_unsigned(check6uf3),16);
    check8uf3_1:=to_ufixed (result,4,-3,AC_RND,AC_SAT);--temporary variable generated by compiler (length,fixed point rule propagation)
    check4uf1_1:=resize(check8uf3_1,2,-1,AC_RND,AC_SAT);--result variable (initial code c)
    check4uf1_2              := "1000";               -- 4
    assert (check4uf1_1 = check4uf1_2) report "error resize/to_ufixed" severity error;
    --same operation with saturation
    check8uf3_1              := "00011000";               -- 3
    check4uf1_2              := "1010";               -- 5
    check6uf3              := "1010"&"00"; --align fixed point (done by gaut kernel : assignation)
    --addition on add_u12_u16_u16 bits unsigned : library operator
    result:=resize(to_unsigned(check8uf3_1),12)+resize(to_unsigned(check6uf3),16);
    check8uf3_1:=to_ufixed (result,4,-3,AC_RND,AC_SAT);--temporary variable generated by compiler (length,fixed point rule propagation)
    check4uf1_1:=resize(check8uf3_1,2,-1,AC_RND,AC_SAT);--result variable (initial code c)
    check4uf1_2:= "1111"; --SAT
    assert (check4uf1_1 = check4uf1_2) report "error resize/to_ufixed" severity error;
    -- test rounding mode AC_RND
    check4sf2 :="0101"; --  1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND,AC_SAT);
    check3sf1_2:= "011"; --1.5
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND,AC_SAT);
    check3sf1_2:= "110"; -- -1
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd" severity error;
    check16sf8:="0010011001001111"; -- 38.30859375
    check12sf4_1:=resize(check16sf8,7,-4,AC_RND,AC_SAT);
    check12sf4_2:= "001001100101"; -- 38.3125
    assert (check12sf4_1 = check12sf4_2) report "error ac_rnd" severity error;
    -- test rounding mode AC_RND_ZERO
    check4sf2 :="0101"; --  1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_ZERO,AC_SAT);
    check3sf1_2:= "010"; -- 1
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_zero" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_ZERO,AC_SAT);
    check3sf1_2:= "110"; -- -1
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_zero" severity error;
    check14uf5 := "00010011001001"; --38.28125
    check13sf4_1 :=resize(check14uf5,8,-4,AC_RND_ZERO,AC_SAT);
    check13sf4_2 :="0001001100100"; -- 38.25
    assert (check13sf4_1 = check13sf4_2) report "error ac_rnd_zero" severity error;
    -- test rounding mode AC_RND_MIN_INF
    check4sf2 :="0101"; --  1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_MIN_INF,AC_SAT);
    check3sf1_2:= "010"; -- 1
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_min_inf" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_MIN_INF,AC_SAT);
    check3sf1_2:= "101"; -- -1.5
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_min_inf" severity error;
    check14uf5 := "00010011001001"; --38.28125
    check13sf4_1 :=resize(check14uf5,8,-4,AC_RND_MIN_INF,AC_SAT);
    check13sf4_2 :="0001001100100"; -- 38.25
    assert (check13sf4_1 = check13sf4_2) report "error ac_rnd_min_inf" severity error;
    -- test rounding mode AC_RND_MIN_INF
    check4sf2 :="0101"; --  1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_MIN_INF,AC_SAT);
    check3sf1_2:= "010"; -- 1
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_min_inf" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_MIN_INF,AC_SAT);
    check3sf1_2:= "101"; -- -1.5
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_min_inf" severity error;
    check14uf5 := "00010011001001"; --38.28125
    check13sf4_1 :=resize(check14uf5,8,-4,AC_RND_MIN_INF,AC_SAT);
    check13sf4_2 :="0001001100100"; -- 38.25
    assert (check13sf4_1 = check13sf4_2) report "error ac_rnd_min_inf" severity error;
    -- test rounding mode AC_RND_INF
    check4sf2 :="0101"; --  1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_INF,AC_SAT);
    check3sf1_2:= "011"; -- 1.5
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_inf" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_INF,AC_SAT);
    check3sf1_2:= "101"; -- -1.5
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_inf" severity error;
    check14uf5 := "00010011001001"; --38.28125
    check13sf4_1 :=resize(check14uf5,8,-4,AC_RND_INF,AC_SAT);
    check13sf4_2 :="0001001100101"; -- 38.3125
    assert (check13sf4_1 = check13sf4_2) report "error ac_rnd_inf" severity error;
    -- test rounding mode AC_RND_CONV
    check4sf2 :="0011"; --  0.75
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_CONV,AC_SAT);
    check3sf1_2:= "010"; -- 1.0
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_conv" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_RND_CONV,AC_SAT);
    check3sf1_2:= "110"; -- -1
    assert (check3sf1_1 = check3sf1_2) report "error ac_rnd_conv" severity error;
    check14uf5 := "00010011001001"; --38.28125
    check13sf4_1 :=resize(check14uf5,8,-4,AC_RND_CONV,AC_SAT);
    check13sf4_2 :="0001001100100"; -- 38.25
    assert (check13sf4_1 = check13sf4_2) report "error ac_rnd_conv" severity error;
    check14uf5 := "00010011001011"; --38.34375
    check13sf4_1 :=resize(check14uf5,8,-4,AC_RND_CONV,AC_SAT);
    check13sf4_2 :="0001001100110"; -- 38.375
    assert (check13sf4_1 = check13sf4_2) report "error ac_rnd_conv" severity error;

    -- test rounding mode AC_TRN
    check4sf2 :="0101"; --  1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_TRN,AC_SAT);
    check3sf1_2:= "010"; -- 1.0
    assert (check3sf1_1 = check3sf1_2) report "error ac_trn" severity error;
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_TRN,AC_SAT);
    check3sf1_2:= "101"; -- -1.5
    assert (check3sf1_1 = check3sf1_2) report "error ac_trn" severity error;
    check16sf8 := "0010011001001111"; --38.30859375
    check12sf4_1 :=resize(check16sf8,7,-4,AC_TRN,AC_SAT);
    check12sf4_2 :="001001100100"; -- 38.25
    assert (check12sf4_1 = check12sf4_2) report "error ac_trn" severity error;
    -- test rounding mode AC_TRN_ZERO
    check4sf2 :="1011"; --  -1.25
    check3sf1_1:=resize(check4sf2,1,-1,AC_TRN_ZERO,AC_SAT);
    check3sf1_2:= "110"; -- -1.0
    assert (check3sf1_1 = check3sf1_2) report "error ac_trn_zero" severity error;
    check16sf8 := "0010011001001111"; --38.30859375
    check12sf4_1 :=resize(check16sf8,7,-4,AC_TRN_ZERO,AC_SAT);
    check12sf4_2 :="001001100100"; -- 38.25
    assert (check12sf4_1 = check12sf4_2) report "error ac_trn_zero" severity error;

  end process basictest;
  

end architecture testbench;
