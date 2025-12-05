library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use std.textio.all;
use work.notech.all;

entity debugout is
  port(
-- I/O to connect to operator blocs
    BUS_DONNEES_1_debugout : inout word;
    BUS_DONNEES_2_debugout : inout word;
    enable, rstb, clk : in std_logic
 );
end;
architecture debugout_arch of debugout is
  attribute period:string;
  attribute period of clk : signal is "10 ns";
  -- states of the FSM
  type state_type is (S0,S1,S2,S3,S4,S5,S6);
  --sequential coding for state_type
  --attribute enum_encoding : string;
  --attribute enum_encoding of state_type : type is "sequential";
  --sequentail coding for state_type for synplify
  --attribute syn_encoding : string;
  --attribute syn_encoding of state_type : type is "sequential";
  signal next_state : state_type;
  signal state : state_type;
  -- signals to connect registers to operators
  signal comp_1_mul_op_a : word;
  signal comp_1_mul_op_b : word;
  signal comp_1_mul_op_o : word;
  -- data path registers
  signal reg_0 : word;
  signal reg_1 : word;
  signal reg_2 : word;

begin
  -- instance of operator mul_op to implement function mul
  comp_1_mul_op : mul_op
    port map(
      -- input ports
      a => comp_1_mul_op_a,
      b => comp_1_mul_op_b,
      -- output ports
      o => comp_1_mul_op_o
    );

  -- The synchronous process
  SYNC_PROC: process (clk)
  begin
    if (clk'event and clk = '1') then
      if (rstb = '0') then
        state <= S6;
      elsif (enable = '1') then
        state <= next_state;
      end if;
    end if;
  end process;

  MUX_REGISTER: process (clk)
  begin
  if (clk'event and clk = '1') then
    if (rstb = '0') then
    -- reset value on registers
    reg_0 <= (others=>'0');
    reg_1 <= (others=>'0');
    reg_2 <= (others=>'0');
    elsif (enable = '1') then
    case (state) is
      when S0 => -- time 0
        -- static memory read from buses 
        reg_2 <= BUS_DONNEES_1_debugout;
      when S1 => -- time 10
        -- outputs of operation op_id10
 -- MISSING REGISTER
 -- MISSING PORT COMPONENT DUE TO MISSING REGISTERS
        -- outputs of operation op_op2
        reg_0 <=reg_2;
      when S2 => -- time 20
      when S3 => -- time 30
      when S4 => -- time 40
      when S5 => -- time 50
      when S6 => -- time 60
        -- inputs from buses
        reg_0 <= BUS_DONNEES_1_debugout;
        reg_1 <= BUS_DONNEES_2_debugout;
    end case; end if;
    end if;
  end process;

  MUX_OPERATOR_TRI_BUS_NEXT_STATE: process (state,reg_0,reg_1,reg_2)
  begin
    -- high impedance on buses to avoid bus pollution
    BUS_DONNEES_1_debugout <= (others => 'Z');
    BUS_DONNEES_2_debugout <= (others => 'Z');
    -- signals to connect registers to operators
    comp_1_mul_op_a <= (others => 'X');
    comp_1_mul_op_b <= (others => 'X');
    case (state) is
      when S0 => -- time 0
        -- inputs of operation op_id10
        comp_1_mul_op_a <= reg_0;
        comp_1_mul_op_b <= reg_1;
        next_state <= S1;
      when S1 => -- time 10
        -- inputs of operation op_id10
        comp_1_mul_op_a <= reg_0;
        comp_1_mul_op_b <= reg_1;
        next_state <= S2;
      when S2 => -- time 20
        -- outputs on buses
        BUS_DONNEES_1_debugout <= reg_0;
 -- MISSING REGISTER ASSOCIATED TO "BUS_DONNEES_2_debugout" 
        next_state <= S3;
      when S3 => -- time 30
        next_state <= S4;
      when S4 => -- time 40
        next_state <= S5;
      when S5 => -- time 50
        next_state <= S6;
      when S6 => -- time 60
        next_state <= S0;
    end case;
  end process;

end debugout_arch;

