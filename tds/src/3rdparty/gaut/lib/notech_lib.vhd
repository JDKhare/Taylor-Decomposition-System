--------------------------------------------------------------------
-- add
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity add_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture add_op_arch of add_op is
	begin
	process (a, b)
	begin
		o <= int_to_word(word_to_int(a) + word_to_int(b)); -- after 1 ns;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity addsub_op is
		port ( a :in word;
		       b :in word;
			 cmd : in std_logic;
		       o :out word
		);
	end;

	architecture addsub_op_arch of addsub_op is
	begin
	process (a, b)
	begin
		if cmd = '0' then
			o <= int_to_word(word_to_int(a) + word_to_int(b)); -- after 1 ns;
		else
			o <= int_to_word(word_to_int(a) - word_to_int(b)); -- after 1 ns;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- mul
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity mul_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture mul_op_arch of mul_op is
	begin
	process (a, b)
	begin
		o <= int_to_word(word_to_int(a) * word_to_int(b)); -- after 1 ns;
	end process;
	end;

--------------------------------------------------------------------
-- sub
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity sub_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture sub_op_arch of sub_op is
	begin
	process (a, b)
	begin
		o <= int_to_word(word_to_int(a) - word_to_int(b)); -- after 1 ns;
	end process;
	end;

--------------------------------------------------------------------
-- div
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.notech.all;

	entity div_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture div_op_arch of div_op is
	begin
	process (a, b)
	variable i : integer range 0 to nb_bit-1;
	variable isNull : std_logic ;
--	variable abs_a, abs_b, tmp : signed(nb_bit-1 downto 0);
--	variable pos_a, pos_b : boolean;
	begin
		isNull := '0';
		for i in 0 to nb_bit-1 loop
			isNull := b(i) or isNull;
		end loop;

		if isNull = '1' then
--			if signed(a) < 0 then
--				abs_a := 0-signed(a); 
--				pos_a := false;
--			else
--				abs_a := signed(a);
--				pos_a := true;
--			end if;
--			if signed(b) < 0 then
--				abs_b := 0-signed(b);
--				pos_b := false;
--			else
--				abs_b := signed(b);
--				pos_b := true;
--			end if;
--			tmp := abs_a / abs_b;
--			if not (pos_a = pos_b) then tmp := 0-tmp; end if;
			o <= std_logic_vector(signed(a)/signed(b));
		else
			o <= (others => '0');
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- mod
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity mod_op is
		port ( a :in word;
			 b : in word;
		       o :out word
		);
	end;

	architecture mod_op_arch of mod_op is
	begin
	process (a,b)
	variable temp : integer;
	variable i : integer range 0 to nb_bit-1;
	begin
		--o <= int_to_word(word_to_int(a) mod word_to_int(b)); ne marche que si b constant et puissance de 2
		--d'ou en arrondissant b à la valeur de puissance de 2(à vérifier en simulation)
		if (b(nb_bit-1) ='1') then
			o<=a;--on considère que l'on est modulo nb_bit-1	
		else
			for i in 0 to nb_bit-1 loop
				if (b(i)='1') then
					temp := i;
				end if;
			end loop;
			o(temp downto 0)<=a(temp downto 0);
			o(nb_bit-1 downto temp-1)<=(others => a( nb_bit-1));--recopie du bit de signe
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- eq
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity eq_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture eq_op_arch of eq_op is
	begin
	process (a, b)
	begin
		 	o <= (others => '0');
        	if word_to_int(a) = word_to_int(b) then		o(0) <= '1' ;
        	else 						o(0) <= '0' ;
        	end if;
	end process;
	end;

--------------------------------------------------------------------
-- ne
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity ne_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture ne_op_arch of ne_op is
	begin
	process (a, b)
	begin
		 	o <= (others => '0');
        	if word_to_int(a) /= word_to_int(b) then		o(0) <= '1' ;
        	else 						o(0) <= '0' ;
        	end if;
	end process;
	end;

--------------------------------------------------------------------
-- le
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity le_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture le_op_arch of le_op is
	begin
	process (a, b)
	begin
		 	o <= (others => '0');
        	if word_to_int(a) <= word_to_int(b) then		o(0) <= '1' ;
        	else 						o(0) <= '0' ;
        	end if;
	end process;
	end;

--------------------------------------------------------------------
-- ge
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity ge_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture ge_op_arch of ge_op is
	begin
	process (a, b)
	begin
		 	o <= (others => '0');
        	if word_to_int(a) >= word_to_int(b) then		o(0) <= '1' ;
        	else 						o(0) <= '0' ;
        	end if;
	end process;
	end;

--------------------------------------------------------------------
-- lt
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity lt_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture lt_op_arch of lt_op is
	begin
	process (a, b)
	begin
		 	o <= (others => '0');
        	if word_to_int(a) < word_to_int(b) then		o(0) <= '1' ;
        	else 						o(0) <= '0' ;
        	end if;
	end process;
	end;

--------------------------------------------------------------------
-- gt
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity gt_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture gt_op_arch of gt_op is
	begin
	process (a, b)
	begin
		 	o <= (others => '0');
        	if word_to_int(a) > word_to_int(b) then		o(0) <= '1' ;
        	else 						o(0) <= '0' ;
        	end if;
	end process;
	end;

--------------------------------------------------------------------
-- eqmux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity eqmux_op is
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end;

	architecture eqmux_op_arch of eqmux_op is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= c;
		else
			o <= d;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- nemux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity nemux_op is
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end;

	architecture nemux_op_arch of nemux_op is
	begin
	process (a,b,c,d)
	begin
		if (a /= b) then
			o <= c;
		else
			o <= d;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- lemux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity lemux_op is
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end;

	architecture lemux_op_arch of lemux_op is
	begin
	process (a,b,c,d)
	begin
		if (a <= b) then
			o <= c;
		else
			o <= d;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- gemux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity gemux_op is
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end;

	architecture gemux_op_arch of gemux_op is
	begin
	process (a,b,c,d)
	begin
		if (a >= b) then
			o <= c;
		else
			o <= d;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- ltmux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity ltmux_op is
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end;

	architecture ltmux_op_arch of ltmux_op is
	begin
	process (a,b,c,d)
	begin
		if (a < b) then
			o <= c;
		else
			o <= d;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- gtmux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity gtmux_op is
		port ( a : in word;
		       b : in word;
		       c : in word;
		       d : in word;
		       o : out word
		);
	end;

	architecture gtmux_op_arch of gtmux_op is
	begin
	process (a,b,c,d)
	begin
		if (a > b) then
			o <= c;
		else
			o <= d;
		end if;
	end process;
	end;

--------------------------------------------------------------------
-- and
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity and_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture and_op_arch of and_op is
	begin
	process (a, b)
	begin
		o <= a and b;
	end process;
	end;

--------------------------------------------------------------------
-- truth_and
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity truth_and_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture truth_and_op_arch of truth_and_op is
	begin
	process (a, b)
	begin
		o <= a and b;
	end process;
	end;

--------------------------------------------------------------------
-- nand
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity nand_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture nand_op_arch of nand_op is
	begin
	process (a, b)
	begin
		o <= a nand b;
	end process;
	end;

--------------------------------------------------------------------
-- or
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity or_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture or_op_arch of or_op is
	begin
	process (a, b)
	begin
		o <= a or b;
	end process;
	end;

--------------------------------------------------------------------
-- truth_or
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity truth_or_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture truth_or_op_arch of truth_or_op is
	begin
	process (a, b)
	begin
		o <= a or b;
	end process;
	end;

--------------------------------------------------------------------
-- xor
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity xor_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture xor_op_arch of xor_op is
	begin
	process (a, b)
	begin
		o <= a xor b;
	end process;
	end;

--------------------------------------------------------------------
-- nor
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity nor_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture nor_op_arch of nor_op is
	begin
	process (a, b)
	begin
		o <= a nor b;
	end process;
	end;

--------------------------------------------------------------------
-- abs
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity abs_op is
		port ( a :in word;
		       o :out word );
	end;

	architecture abs_op_arch of abs_op is
	begin
	process (a)
	begin
		o <= int_to_word(abs(word_to_int(a)));
	end process;
	end;

--------------------------------------------------------------------
-- uminus
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity uminus_op is
		port ( a :in word;
		       o :out word
		);
	end;

	architecture uminus_op_arch of uminus_op is
	begin
	process (a)
	begin
		o <= int_to_word(-word_to_int(a));
	end process;
	end;

--------------------------------------------------------------------
-- sll
--------------------------------------------------------------------
library IEEE;
use IEEE.numeric_std.all;
library work;
use work.notech.all;

	entity sll_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture sll_op_arch of sll_op is
	begin
	process (a, b)
	begin
		o <=signed_to_word(word_to_signed(a) sll word_to_int(b));
	end process;
	end;

--------------------------------------------------------------------
-- srl
--------------------------------------------------------------------
library IEEE;
use IEEE.numeric_std.all;
library work;
use work.notech.all;

	entity srl_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture srl_op_arch of srl_op is
	begin
	process (a, b)
	begin
		o <=signed_to_word(word_to_signed(a) srl word_to_int(b));
	end process;
	end;

--------------------------------------------------------------------
-- rol
--------------------------------------------------------------------
library IEEE;
use IEEE.numeric_std.all;
library work;
use work.notech.all;

	entity rol_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture rol_op_arch of rol_op is
	begin
	process (a, b)
	begin
		o <=signed_to_word(word_to_signed(a) rol word_to_int(b));
	end process;
	end;

--------------------------------------------------------------------
-- ror
--------------------------------------------------------------------
library IEEE;
use IEEE.numeric_std.all;
library work;
use work.notech.all;

	entity ror_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture ror_op_arch of ror_op is
	begin
	process (a, b)
	begin
		o <=signed_to_word(word_to_signed(a) ror word_to_int(b));
	end process;
	end;

--------------------------------------------------------------------
-- sla
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity sla_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture sla_op_arch of sla_op is
	begin
	process (a, b)
	begin
		o <=To_StdLogicVector  (To_bitvector (a) sla word_to_int(b));
	end process;
	end;

--------------------------------------------------------------------
-- sra
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity sra_op is
		port ( a :in word;
		       b :in word;
		       o :out word );
	end;

	architecture sra_op_arch of sra_op is
	begin
	process (a, b)
	begin
		o <=To_StdLogicVector  (To_bitvector (a) sra word_to_int(b));
	end process;
	end;

--------------------------------------------------------------------
-- shl
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_signed.all;
library work;
use work.notech.all;

	entity shl_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture shl_op_arch of shl_op is
	begin
	process (a, b)
	begin
	     o<=SHL(a,b);
	end process;
	end;

--------------------------------------------------------------------
-- shr
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use ieee.numeric_std.all;
library work;
use work.notech.all;

	entity shr_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture shr_op_arch of shr_op is
	begin
	process (a, b)
	begin
	     o<=SHR(a,b);
	end process;
	end;

--------------------------------------------------------------------
-- shr4div
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use ieee.numeric_std.all;
library work;
use work.notech.all;

	entity shr4div_op is
		port ( a :in word;
		       b :in word;
		       o :out word
		);
	end;

	architecture shr4div_op_arch of shr4div_op is
	begin
	process (a, b)
	variable sa, tmp : signed(nb_bit-1 downto 0);
	begin
		sa := signed(a);
		if (sa < 0) then tmp := sa+1; else tmp := sa; end if;
	     	o<=SHR(std_logic_vector(tmp),b);
	end process;
	end;

--------------------------------------------------------------------
-- not
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity not_op is
		port ( a :in word;
		       o :out word
		);
	end;

	architecture not_op_arch of not_op is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

--------------------------------------------------------------------
-- truth_not
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
library work;
use work.notech.all;

	entity truth_not_op is
		port ( a :in word;
		       o :out word
		);
	end;

	architecture truth_not_op_arch of truth_not_op is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

--------------------------------------------------------------------
-- discriminant
--------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.notech.all;

entity discriminant_op is
  port(
-- I/O to connect to operator blocs
    BUS_DONNEES_1_discriminant : in word;
    BUS_DONNEES_2_discriminant : in word;
    BUS_DONNEES_3_discriminant : out word;
    enable, rstb, clk : in std_logic
 );
end;
architecture discriminant_arch of discriminant_op is
  -- states of the FSMD
  type state_type is (S0,S1,S2,S3);
  signal state, next_state : state_type;
  -- signals to connect registers to operators
  signal comp_0_add_op_a : word;
  signal comp_0_add_op_b : word;
  signal comp_0_add_op_o : word;
  signal comp_1_mul_op_a : word;
  signal comp_1_mul_op_b : word;
  signal comp_1_mul_op_o : word;
  signal comp_2_mul_op_a : word;
  signal comp_2_mul_op_b : word;
  signal comp_2_mul_op_o : word;
  -- data path registers
  signal reg_0 : word;
  signal reg_1 : word;
  signal reg_2 : word;
  -- hardwire constant
  constant const_moins_2 : word := std_logic_vector( to_signed(-2, 16));

begin
  -- instance of operator add_op to implement function add
  comp_0_add_op : add_op
    port map(
      -- input ports
      a => comp_0_add_op_a,
      b => comp_0_add_op_b,
      -- output ports
      o => comp_0_add_op_o
    );
  -- instance of operator mul_op to implement function mul
  comp_1_mul_op : mul_op
    port map(
      -- input ports
      a => comp_1_mul_op_a,
      b => comp_1_mul_op_b,
      -- output ports
      o => comp_1_mul_op_o
    );
  -- instance of operator mul_op to implement function mul
  comp_2_mul_op : mul_op
    port map(
      -- input ports
      a => comp_2_mul_op_a,
      b => comp_2_mul_op_b,
      -- output ports
      o => comp_2_mul_op_o
    );

  -- The synchronous process
  SYNC_PROC: process (clk)
  begin
    if (clk'event and clk = '1') then
      if (rstb = '0') then
        state <= S3;
      elsif (enable = '1') then
        state <= next_state;
      end if;
    end if;
  end process;

  OUTPUT_REGISTER: process (clk)
  begin
  if (clk'event and clk = '1') then
    if (rstb = '0') then
    -- reset value on registers
    reg_0 <= const_moins_2;
    reg_1 <= (others=>'0');
    reg_2 <= (others=>'0');
    elsif (enable = '1') then
    case (state) is
      when S0 => -- time 0
      when S1 => -- time 10
        -- outputs of operation op0
        reg_2 <= comp_2_mul_op_o;
        -- outputs of operation op1
        reg_1 <= comp_1_mul_op_o;
      when S2 => -- time 20
        -- outputs of operation op2
        reg_1 <= comp_0_add_op_o;
      when S3 => -- time 30
        -- inputs from buses 
        reg_1 <= BUS_DONNEES_1_discriminant;
        reg_2 <= BUS_DONNEES_2_discriminant;
    end case; end if;
    end if;
  end process;

  OUTPUT_OPERATOR_BUS: process (state)
  begin
    -- high impedance on buses to avoid bus pollution
    BUS_DONNEES_3_discriminant <= (others => 'Z');
    -- signals to connect registers to operators
    comp_0_add_op_a <= (others => '0');
    comp_0_add_op_b <= (others => '0');
    comp_1_mul_op_a <= (others => '0');
    comp_1_mul_op_b <= (others => '0');
    comp_2_mul_op_a <= (others => '0');
    comp_2_mul_op_b <= (others => '0');
    case (state) is
      when S0 => -- time 0
        -- inputs of operation op0
        comp_2_mul_op_a <= reg_1;
        comp_2_mul_op_b <= reg_1;
        -- inputs of operation op1
        comp_1_mul_op_a <= reg_2;
        comp_1_mul_op_b <= reg_0;
      when S1 => -- time 10
        -- inputs of operation op0
        comp_2_mul_op_a <= reg_1;
        comp_2_mul_op_b <= reg_1;
        -- inputs of operation op1
        comp_1_mul_op_a <= reg_2;
        comp_1_mul_op_b <= reg_0;
      when S2 => -- time 20
        -- inputs of operation op2
        comp_0_add_op_a <= reg_2;
        comp_0_add_op_b <= reg_1;
      when S3 => -- time 30
        -- outputs on buses 
        BUS_DONNEES_3_discriminant <= reg_1;
    end case;
  end process;

  NEXT_STATE_DECODE: process (state)
  begin
    next_state <= state;  -- default is to stay in current state
    case (state) is
      when S0 => -- time 0
        next_state <= S1;
      when S1 => -- time 10
        next_state <= S2;
      when S2 => -- time 20
        next_state <= S3;
      when S3 => -- time 30
        next_state <= S0;
    end case;
  end process;

end discriminant_arch;
