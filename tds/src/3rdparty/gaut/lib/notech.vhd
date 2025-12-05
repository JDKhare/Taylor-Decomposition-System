



-- 09/2007, word

-- add, mul, sub, div, mod, eq, ne, lt, le, gt, ge, eqmux, nemux, lemux, ltmux, gemux, gtmux,
-- and, or, nor, nand, not, xor
-- abs, uminus
-- sll, srl, rol, ror, sla, sra, shr, shl, shr4div



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;

package notech is
	constant nb_bit : integer := 16;
	subtype	word is std_logic_vector(nb_bit-1 downto 0);

	function word_to_int(x : in word) return integer;
	function int_to_word(x : integer) return word;
	function word_to_signed(x : in word) return signed;
	function signed_to_word(x : in signed) return word;
        function max (LEFT, RIGHT: INTEGER) return INTEGER;
	function min (LEFT, RIGHT: INTEGER) return INTEGER;

        function reverse_any_vector (a: in std_logic_vector)
        return std_logic_vector;

       function onehot_to_integer (a: in std_logic_vector)
       return INTEGER;
       function onehot_to_binary (onehot: in std_logic_vector; binary_size : in integer)
       return std_logic_vector;


--------------------------------------------------------------------
-- add
--------------------------------------------------------------------
    component add_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

    component addsub_op
		port ( a :in word;
		       b :in word;
			 cmd : in std_logic;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- mul
--------------------------------------------------------------------
    component mul_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- sub
--------------------------------------------------------------------
	component sub_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;
--------------------------------------------------------------------
-- discriminant
--------------------------------------------------------------------
	component discriminant_op
		port(
		    BUS_DONNEES_1_discriminant : in word;
		    BUS_DONNEES_2_discriminant : in word;
		    BUS_DONNEES_3_discriminant : out word;
		    enable, rstb, clk : in std_logic);
	end component;

--------------------------------------------------------------------
-- div
--------------------------------------------------------------------
	component div_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- mod
--------------------------------------------------------------------
	component mod_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- eq
--------------------------------------------------------------------
	component eq_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- ne
--------------------------------------------------------------------
	component ne_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- le
--------------------------------------------------------------------
	component le_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- ge
--------------------------------------------------------------------
	component ge_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- lt
--------------------------------------------------------------------
	component lt_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- gt
--------------------------------------------------------------------
	component gt_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- eqmux
--------------------------------------------------------------------
	component eqmux_op
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end component;

--------------------------------------------------------------------
-- nemux
--------------------------------------------------------------------
	component nemux_op
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end component;

--------------------------------------------------------------------
-- lemux
--------------------------------------------------------------------
	component lemux_op
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end component;

--------------------------------------------------------------------
-- gemux
--------------------------------------------------------------------
	component gemux_op
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end component;

--------------------------------------------------------------------
-- ltmux
--------------------------------------------------------------------
	component ltmux_op
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end component;

--------------------------------------------------------------------
-- gemux
--------------------------------------------------------------------
	component gtmux_op
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end component;

--------------------------------------------------------------------
-- and
--------------------------------------------------------------------
    component and_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- nand
--------------------------------------------------------------------
    component nand_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- or
--------------------------------------------------------------------
    component or_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- truth_and
--------------------------------------------------------------------
    component truth_and_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- truth_or
--------------------------------------------------------------------
    component truth_or_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- nor
--------------------------------------------------------------------
    component nor_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- xor
--------------------------------------------------------------------
    component xor_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- not
--------------------------------------------------------------------
    component not_op
		port ( a :in word;
		       o :out word );
	end component;


--------------------------------------------------------------------
-- truth_not
--------------------------------------------------------------------
    component truth_not_op
		port ( a :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- abs
--------------------------------------------------------------------
    component abs_op
		port ( a :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- resize
--------------------------------------------------------------------
    component resize_op
		port ( a :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- sll
--------------------------------------------------------------------
    component sll_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- srl
--------------------------------------------------------------------
    component srl_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- rol
--------------------------------------------------------------------
    component rol_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- ror
--------------------------------------------------------------------
    component ror_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- sla
--------------------------------------------------------------------
    component sla_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- sra
--------------------------------------------------------------------
    component sra_op
		port ( a :in word;
		       b :in word;
		       o :out word );
	end component;

--------------------------------------------------------------------
-- shl
--------------------------------------------------------------------

	component shl_op
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end component;

--------------------------------------------------------------------
-- shr
--------------------------------------------------------------------

	component shr_op
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end component;

--------------------------------------------------------------------
-- shr4div
--------------------------------------------------------------------
	component shr4div_op
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end component;

--------------------------------------------------------------------
-- uminus
--------------------------------------------------------------------
    component uminus_op
		port ( a :in word;
		       o :out word );
	end component;


end notech;

package body notech is

	function word_to_int(x : in word) return integer is
	variable i : integer range 0 to nb_bit-1;
	variable tmp : signed(nb_bit-1 downto 0);
	variable r : integer;
	begin
		for i in 0 to nb_bit-1 loop
			tmp(i) := x(i);
		end loop;
		r := to_integer(tmp);
		return r;
	end;

	function int_to_word(x : integer) return word is
	variable i : integer range 0 to nb_bit-1;
	variable tmp : signed(nb_bit-1 downto 0);
	variable r : word;
	begin
		tmp := to_signed(x, nb_bit);
		for i in 0 to nb_bit-1 loop
			r(i) := tmp(i);
		end loop;
		return r;
	end;

	function word_to_signed(x : in word) return signed is
	variable i : integer range 0 to nb_bit-1;
	variable tmp : signed(nb_bit-1 downto 0);
	begin
		for i in 0 to nb_bit-1 loop
			tmp(i) := x(i);
		end loop;
		return tmp;
	end;

	function signed_to_word(x : in signed) return word is
	variable i : integer range 0 to nb_bit-1;
	variable tmp : word;
	begin
		for i in 0 to nb_bit-1 loop
			tmp(i) := x(i);
		end loop;
		return tmp;
	end;

    function MAX (LEFT, RIGHT: INTEGER) return INTEGER is
 	begin
      if LEFT > RIGHT then return LEFT;
      else return RIGHT;
      end if;
    end MAX;

    function MIN (LEFT, RIGHT: INTEGER) return INTEGER is
 	begin
      if LEFT < RIGHT then return LEFT;
      else return RIGHT;
      end if;
    end MIN;

    function reverse_any_vector (a: in std_logic_vector)
return std_logic_vector is
variable result: std_logic_vector(a'REVERSE_RANGE);
begin
for i in a'RANGE loop
result(i) := a(i);
end loop;
return result;
end; -- function reverse_any_vector



function onehot_to_integer (a: in std_logic_vector)
return INTEGER is
variable convert: INTEGER := 0;
begin
for i in a'RANGE loop
    if (a(i)='1')  then convert:= i;
    end if;
end loop;
return convert;
end; -- onehot_to_integer


function onehot_to_binary (onehot: in std_logic_vector; binary_size : in integer)
return std_logic_vector is
variable code: std_logic_vector(binary_size-1 downto 0);
begin
code := (others => '0');
for N in onehot'RANGE loop
if onehot(N) = '1' then
code := code OR std_logic_vector(to_unsigned(N, code'LENGTH));
end if;
end loop;
return code;
end; -- onehot_to_binary

end notech;
