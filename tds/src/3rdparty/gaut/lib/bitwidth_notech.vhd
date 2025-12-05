library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.fixed_pkg.all;

package bitwidth_notech is
	function word_to_real(x : in std_logic_vector; fractionalFactor : integer) return real;
	function word_to_ureal(x : in std_logic_vector; fractionalFactor : integer) return real;
	function word_to_uint(x : in std_logic_vector) return integer;
	function word_to_int(x : in std_logic_vector) return integer;
	function int_to_word(x : integer; nb_bit : integer) return std_logic_vector;
    	function MAX_INTEGER (LEFT, RIGHT: INTEGER) return INTEGER;
	function MIN_INTEGER (LEFT, RIGHT: INTEGER) return INTEGER;
	function to_slv(x : unsigned; size: INTEGER) return std_logic_vector;
	function to_slv(x : signed; size: INTEGER) return std_logic_vector;

--------------------------------------------------------------------
-- resize
--------------------------------------------------------------------

     component rndconvsatsym_op_s_s
  		port ( a :in sfixed;
		       o :out sfixed
		);
	end component;

     component rndconvsatzero_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndconvsat_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndconvwrap_op_s_s
  		port ( a :in sfixed;
 		       o :out sfixed
 		);
 	end component;

     component rndzerosatsym_op_s_s
  		port ( a :in sfixed;
 		       o :out sfixed
 		);
 	end component;

     component rndzerosatzero_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndzerosat_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndzerowrap_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndmininfsatsym_op_s_s
  		port ( a :in sfixed;
 		       o :out sfixed
 		);
 	end component;

     component rndmininfsatzero_op_s_s
  		port ( a :in sfixed;
 		       o :out sfixed
 		);
 	end component;

     component rndmininfsat_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndmininfwrap_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;


     component trnzerosatsym_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component trnzerosatzero_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component trnzerosat_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component trnzerowrap_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndinfsatsym_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndinfsatzero_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndinfsat_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndinfwrap_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndsatsym_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;


     component rndsatzero_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;


     component rndsat_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component rndwrap_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component trnsatsym_op_s_s
  		port ( a :in sfixed;
 		       o :out sfixed
 		);
 	end component;

     component trnsatzero_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

     component trnsat_op_s_s
  		port ( a :in sfixed;
 		       o :out sfixed
 		);
 	end component;

     component trnwrap_op_s_s
  		port ( a :in sfixed; 
 		       o :out sfixed
 		);
 	end component;

 --   component resize_op_s_s
 -- 		port ( a :in sfixed; 
 --			 cmd : in std_logic_vector(4 downto 0);
 --		       o :out sfixed
 --		);
 --	end component;

     component rndconvsatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndconvsatzero_op_u_u
  		port ( a :in ufixed;
		       o :out ufixed
		);
	end component;

     component rndconvsat_op_u_u
  		port ( a :in ufixed; 
		       o :out ufixed
		);
	end component;

     component rndconvwrap_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerosatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerosatzero_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerosat_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerowrap_op_u_u
  		port ( a :in ufixed;
 		       o :out ufixed
 		);
 	end component;

     component rndmininfsatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndmininfsatzero_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndmininfsat_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndmininfwrap_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;


     component trnzerosatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnzerosatzero_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnzerosat_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnzerowrap_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfsatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfsatzero_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfsat_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfwrap_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndsatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;


     component rndsatzero_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;


     component rndsat_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndwrap_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnsatsym_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnsatzero_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnsat_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnwrap_op_u_u
  		port ( a :in ufixed; 
 		       o :out ufixed
 		);
 	end component;
 --   component resize_op_u_u
 --		port ( a :in ufixed; 
 --			   cmd : in std_logic_vector(4 downto 0);
 --		       o :out ufixed
 --		);
 --	end component;
    component rndconvsatsym_op_s_u
  		port ( a :in sfixed;
 		       o :out ufixed
 		);
 	end component;

     component rndconvsatzero_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndconvsat_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndconvwrap_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerosatsym_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerosatzero_op_s_u
  		port ( a :in sfixed;
 		       o :out ufixed
 		);
 	end component;

     component rndzerosat_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndzerowrap_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndmininfsatsym_op_s_u
  		port ( a :in sfixed;
 		       o :out ufixed
 		);
 	end component;

     component rndmininfsatzero_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndmininfsat_op_s_u
  		port ( a :in sfixed;
 		       o :out ufixed
 		);
 	end component;

     component rndmininfwrap_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;


     component trnzerosatsym_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnzerosatzero_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnzerosat_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnzerowrap_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfsatsym_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfsatzero_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfsat_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndinfwrap_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component rndsatsym_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;


     component rndsatzero_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;


     component rndsat_op_s_u
  		port ( a :in sfixed;
 		       o :out ufixed
 		);
 	end component;

     component rndwrap_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnsatsym_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnsatzero_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnsat_op_s_u
  		port ( a :in sfixed; 
 		       o :out ufixed
 		);
 	end component;

     component trnwrap_op_s_u
  		port ( a :in sfixed;
 		       o :out ufixed
 		);
 	end component;

 --   component resize_op_s_u
 --		port ( a :in sfixed; 
 --		       cmd : in std_logic_vector(4 downto 0);
 --		       o :out ufixed
 --		);
 --	end component;


    component extend_op_s_s
		port ( a :in signed; 
		       o :out signed
		);
	end component;

    component extend_op_u_u
		port ( a :in unsigned; 
		       o :out unsigned
		);
	end component;

    component extend_op_s_u
		port ( a :in signed;
		       o :out unsigned
		);
	end component;

    component extend_op_u_s
		port ( a :in unsigned; 
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- acs4
--------------------------------------------------------------------
	component acs4_op_s_s_s_s_s_s_s_s_s
		port ( a,b,c,d,s,t,u,v : in signed;
				s4 : out signed );
	end component;
--------------------------------------------------------------------
-- min4
--------------------------------------------------------------------
	component min4_op_s_s_s_s_s
		port ( x0,x1,x2,x3 : in signed;
				s3 : out signed );
	end component;
--------------------------------------------------------------------
-- min2
--------------------------------------------------------------------
	component min2_op_s_s_s
		port ( x0,x1 : in signed;
		       s1 : out signed );
	end component;
---------------------------------------

--------------------------------------------------------------------
-- add
--------------------------------------------------------------------

    component add_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;


    component add_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

    component add_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

    component add_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

    component add_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- mul
--------------------------------------------------------------------
    component mul_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component mul_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

    component mul_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

    component mul_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- sub
--------------------------------------------------------------------
    component sub_op_u_u_s
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out signed );
	end component;

    component sub_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component sub_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

    component sub_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

    component sub_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- addsub
--------------------------------------------------------------------
    component addsub_op_u_u_s
		port ( a :in unsigned;
		       b :in unsigned;
			   cmd : in std_logic;
		       o :out signed );
	end component;

    component addsub_op_s_s_s
		port ( a :in signed;
		       b :in signed;
			   cmd : in std_logic;
		       o :out signed );
	end component;

    component addsub_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
			   cmd : in std_logic;
		       o :out signed );
	end component;

    component addsub_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
			   cmd : in std_logic;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- div
--------------------------------------------------------------------
    component div_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component div_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

    component div_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component div_op_s_u_s 
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;
--------------------------------------------------------------------
-- mod
--------------------------------------------------------------------

    component mod_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component mod_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component mod_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

	component mod_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;
--------------------------------------------------------------------
-- eq
--------------------------------------------------------------------

    component eq_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component eq_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

	component eq_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component eq_op_s_u_u 
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned );
	end component;

--------------------------------------------------------------------
-- ne
--------------------------------------------------------------------

    component ne_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component ne_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

	component ne_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component ne_op_s_u_u
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned );
	end component;
--------------------------------------------------------------------
-- le
--------------------------------------------------------------------

    component le_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component le_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

	component le_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component le_op_s_u_u
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned );
	end component;
--------------------------------------------------------------------
-- ge
--------------------------------------------------------------------

    component ge_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component ge_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

	component ge_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component ge_op_s_u_u
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned );
	end component;
--------------------------------------------------------------------
-- lt
--------------------------------------------------------------------

    component lt_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component lt_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

    component lt_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

    component lt_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

    component lt_op_s_u_u
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned );
	end component;


--------------------------------------------------------------------
-- gt
--------------------------------------------------------------------

    component gt_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component gt_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

    component gt_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

    component gt_op_s_u_u
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned );
	end component;


--------------------------------------------------------------------
-- eqmux
--------------------------------------------------------------------

	component eqmux_op_u_u_u_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component eqmux_op_s_s_s_s_s
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component eqmux_op_u_u_s_s_s
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component eqmux_op_u_u_s_s_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out unsigned
		);
	end component;

	component eqmux_op_s_u_s_s_s
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component eqmux_op_u_s_s_s_s
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component eqmux_op_s_u_u_u_u
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component eqmux_op_u_s_u_u_u
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component eqmux_op_u_u_u_s_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in signed;
		       o : out unsigned
		);
	end component;

	component eqmux_op_u_u_s_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

--------------------------------------------------------------------
-- nemux
--------------------------------------------------------------------

	component nemux_op_u_u_u_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component nemux_op_s_s_s_s_s
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;


	component nemux_op_u_u_s_s_s
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;


	component nemux_op_s_u_s_s_s
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component nemux_op_u_s_s_s_s
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component nemux_op_s_u_u_u_u
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component nemux_op_u_s_u_u_u
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

--------------------------------------------------------------------
-- lemux
--------------------------------------------------------------------

	component lemux_op_u_u_u_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component lemux_op_s_s_s_s_s
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component lemux_op_u_u_s_s_s
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component lemux_op_s_u_s_s_s
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component lemux_op_u_s_s_s_s
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component lemux_op_s_u_u_u_u
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component lemux_op_u_s_u_u_u
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

--------------------------------------------------------------------
-- gemux
--------------------------------------------------------------------

	component gemux_op_u_u_u_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component gemux_op_s_s_s_s_s
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gemux_op_u_u_s_s_s
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gemux_op_s_u_s_s_s
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;


	component gemux_op_u_s_s_s_s
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gemux_op_s_u_u_u_u
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component gemux_op_u_s_u_u_u
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

--------------------------------------------------------------------
-- ltmux
--------------------------------------------------------------------

	component ltmux_op_u_u_u_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component ltmux_op_s_s_s_s_s
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component ltmux_op_u_u_s_s_s
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component ltmux_op_s_u_s_s_s
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component ltmux_op_u_s_s_s_s
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component ltmux_op_s_u_u_u_u
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component ltmux_op_u_s_u_u_u
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

--------------------------------------------------------------------
-- gtmux
--------------------------------------------------------------------

	component gtmux_op_u_u_u_u_u
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component gtmux_op_s_s_s_s_s
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gtmux_op_u_u_s_s_s
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gtmux_op_s_u_s_s_s
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gtmux_op_u_s_s_s_s
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end component;

	component gtmux_op_s_u_u_u_u
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

	component gtmux_op_u_s_u_u_u
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end component;

--------------------------------------------------------------------
-- and
--------------------------------------------------------------------

    component and_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component and_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component and_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component and_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;


--------------------------------------------------------------------
-- truth_and
--------------------------------------------------------------------

    component truth_and_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component truth_and_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component truth_and_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component truth_and_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;


--------------------------------------------------------------------
-- nand
--------------------------------------------------------------------

    component nand_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component nand_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component nand_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component nand_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- or
--------------------------------------------------------------------

    component or_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component or_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component or_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component or_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- truth_or
--------------------------------------------------------------------

    component truth_or_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component truth_or_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component truth_or_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component truth_or_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- nor
--------------------------------------------------------------------

    component nor_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component nor_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component nor_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component nor_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- xor
--------------------------------------------------------------------

    component xor_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component xor_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component xor_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component xor_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- not
--------------------------------------------------------------------

    component not_op_u_u
		port ( a :in unsigned;
		       o :out unsigned );
	end component;

    component not_op_s_s
		port ( a :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- truth_not
--------------------------------------------------------------------

    component truth_not_op_u_u
		port ( a :in unsigned;
		       o :out unsigned );
	end component;

    component truth_not_op_s_s
		port ( a :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- abs
--------------------------------------------------------------------


    component abs_op_s_s
		port ( a :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- sll
--------------------------------------------------------------------

    component sll_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component sll_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

    component sll_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end component;

	component sll_op_u_s_u 
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component sll_op_s_u_s 
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;
--------------------------------------------------------------------
-- srl
--------------------------------------------------------------------

    component srl_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component srl_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component srl_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component srl_op_s_u_s 
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- rol
--------------------------------------------------------------------

    component rol_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component rol_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component rol_op_u_s_u 
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component rol_op_s_u_s 
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- ror
--------------------------------------------------------------------

    component ror_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component ror_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component ror_op_u_s_u 
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

	component ror_op_s_u_s 
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- sla
--------------------------------------------------------------------

    component sla_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component sla_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component sla_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

	component sla_op_u_s_u 
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;
--------------------------------------------------------------------
-- sra
--------------------------------------------------------------------

    component sra_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component sra_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component sra_op_s_u_s 
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end component;

	component sra_op_u_s_u 
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end component;

--------------------------------------------------------------------
-- shl
--------------------------------------------------------------------


	component shl_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end component;

	component shl_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed);
	end component;

	component shl_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end component;

	component shl_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed);
	end component;

	component shl_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed);
	end component;

--------------------------------------------------------------------
-- shr
--------------------------------------------------------------------

	component shr_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end component;

	component shr_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed);
	end component;

	component shr_op_u_s_u
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end component;

	component shr_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed);
	end component;

--------------------------------------------------------------------
-- shr4div
--------------------------------------------------------------------

	component shr4div_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end component;

	component shr4div_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end component;

--------------------------------------------------------------------
-- uminus
--------------------------------------------------------------------

    component uminus_op_s_s
		port ( a :in signed;
		       o :out signed );
	end component;

-----------------------------------------------------------------------
-- bitselect
-----------------------------------------------------------------------
    component bitselect_op_s_u_u
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end component;

    component bitselect_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end component;

	component bitselect_op_s_s_u
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end component;

	component bitselect_op_u_s_u 
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end component;
-----------------------------------------------------------------------
-- slicewrite
-----------------------------------------------------------------------
   component slicewrite_op_s_s_u_s
		port ( a :in signed;
		       b :in signed;
			   c : in unsigned;
		       o :out signed );
	end component;
   component slicewrite_op_u_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
			   c : in unsigned;
		       o :out unsigned );
	end component;


-----------------------------------------------------------------------
-- operator whose operands have different size of fractional part
-----------------------------------------------------------------------
--------------------------------------------------------------------
-- add_fp
--------------------------------------------------------------------

    component add_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed );
	end component;

    component add_fp_op_s_s_s
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed);
	end component;

    component add_fp_op_s_u_s
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed);
	end component;

    component add_fp_op_u_s_s
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed);
	end component;
--------------------------------------------------------------------
-- gt_fp
--------------------------------------------------------------------

    component gt_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

    component gt_fp_op_s_s_u
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component gt_fp_op_u_s_u
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component gt_fp_op_s_u_u
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

--------------------------------------------------------------------
-- gtmux_fp
--------------------------------------------------------------------

	component gtmux_fp_op_u_u_u_u_u
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component gtmux_fp_op_s_s_s_s_s
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gtmux_fp_op_u_u_s_s_s
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gtmux_fp_op_s_u_s_s_s
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gtmux_fp_op_u_s_s_s_s
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gtmux_fp_op_s_u_u_u_u
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component gtmux_fp_op_u_s_u_u_u
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;


--------------------------------------------------------------------
-- eq_fp
--------------------------------------------------------------------

    component eq_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

    component eq_fp_op_s_s_u
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component eq_fp_op_u_s_u
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component eq_fp_op_s_u_u
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

--------------------------------------------------------------------
-- eqmux_fp
--------------------------------------------------------------------

	component eqmux_fp_op_u_u_u_u_u
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component eqmux_fp_op_s_s_s_s_s
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component eqmux_fp_op_u_u_s_s_s
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component eqmux_fp_op_s_u_s_s_s
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component eqmux_fp_op_u_s_s_s_s
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component eqmux_fp_op_s_u_u_u_u
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component eqmux_fp_op_u_s_u_u_u
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

--------------------------------------------------------------------
-- ne_fp
--------------------------------------------------------------------

    component ne_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

    component ne_fp_op_s_s_u
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component ne_fp_op_u_s_u
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component ne_fp_op_s_u_u
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

--------------------------------------------------------------------
-- nemux_fp
--------------------------------------------------------------------

	component nemux_fp_op_u_u_u_u_u
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component nemux_fp_op_s_s_s_s_s
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component nemux_fp_op_u_u_s_s_s
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component nemux_fp_op_s_u_s_s_s
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component nemux_fp_op_u_s_s_s_s
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component nemux_fp_op_s_u_u_u_u
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component nemux_fp_op_u_s_u_u_u
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;


--------------------------------------------------------------------
-- lt_fp
--------------------------------------------------------------------

    component lt_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

    component lt_fp_op_s_s_u
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component lt_fp_op_u_s_u
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component lt_fp_op_s_u_u
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

--------------------------------------------------------------------
-- ltmux_fp
--------------------------------------------------------------------

	component ltmux_fp_op_u_u_u_u_u
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component ltmux_fp_op_s_s_s_s_s
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component ltmux_fp_op_u_u_s_s_s
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component ltmux_fp_op_s_u_s_s_s
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component ltmux_fp_op_u_s_s_s_s
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component ltmux_fp_op_s_u_u_u_u
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component ltmux_fp_op_u_s_u_u_u
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;


--------------------------------------------------------------------
-- le_fp
--------------------------------------------------------------------

    component le_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

    component le_fp_op_s_s_u
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component le_fp_op_u_s_u
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component le_fp_op_s_u_u
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

--------------------------------------------------------------------
-- lemux_fp
--------------------------------------------------------------------

	component lemux_fp_op_u_u_u_u_u
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component lemux_fp_op_s_s_s_s_s
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component lemux_fp_op_u_u_s_s_s
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component lemux_fp_op_s_u_s_s_s
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component lemux_fp_op_u_s_s_s_s
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component lemux_fp_op_s_u_u_u_u
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component lemux_fp_op_u_s_u_u_u
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

--------------------------------------------------------------------
-- ge_fp
--------------------------------------------------------------------

    component ge_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

    component ge_fp_op_s_s_u
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component ge_fp_op_u_s_u
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end component;

    component ge_fp_op_s_u_u
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end component;

--------------------------------------------------------------------
-- gemux_fp
--------------------------------------------------------------------

	component gemux_fp_op_u_u_u_u_u
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component gemux_fp_op_s_s_s_s_s
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gemux_fp_op_u_u_s_s_s
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gemux_fp_op_s_u_s_s_s
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gemux_fp_op_u_s_s_s_s
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end component;

	component gemux_fp_op_s_u_u_u_u
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;

	component gemux_fp_op_u_s_u_u_u
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end component;
--------------------------------------------------------------------
-- sub_fp
--------------------------------------------------------------------

    component sub_fp_op_u_u_u
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed );
	end component;

    component sub_fp_op_s_s_s
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed);
	end component;

    component sub_fp_op_s_u_s
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed);
	end component;

    component sub_fp_op_u_s_s
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed);
	end component;
--------------------------------------------------------------------
-- not_fp
--------------------------------------------------------------------

    component not_fp_op_u_u
		port ( a :in unsigned;
		       o :out unsigned );
	end component;

    component not_fp_op_s_s
		port ( a :in signed;
		       o :out signed );
	end component;

--------------------------------------------------------------------
-- truth_not_fp
--------------------------------------------------------------------

    component truth_not_fp_op_u_u
		port ( a :in unsigned;
		       o :out unsigned );
	end component;

    component truth_not_fp_op_s_s
		port ( a :in signed;
		       o :out signed );
	end component;                         
--------------------------------------------------------------------
-- and_fp
--------------------------------------------------------------------

    component and_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component and_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component and_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component and_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;


--------------------------------------------------------------------
-- truth_and_fp
--------------------------------------------------------------------

    component truth_and_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component truth_and_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component truth_and_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component truth_and_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;


--------------------------------------------------------------------
-- nand_fp
--------------------------------------------------------------------

    component nand_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component nand_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component nand_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component nand_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- or
--------------------------------------------------------------------

    component or_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component or_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component or_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component or_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- truth_or_fp
--------------------------------------------------------------------

    component truth_or_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component truth_or_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component truth_or_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component truth_or_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- nor_fp
--------------------------------------------------------------------

    component nor_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component nor_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component nor_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component nor_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

--------------------------------------------------------------------
-- xor_fp
--------------------------------------------------------------------

    component xor_fp_op_u_u_u
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end component;

    component xor_fp_op_s_s_s
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end component;

	component xor_fp_op_u_s_s
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end component;

	component xor_fp_op_s_u_s
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end component;

end bitwidth_notech;

package body bitwidth_notech is

	function word_to_real(x : in std_logic_vector; fractionalFactor : integer) return real is
	variable tmp : integer;
	variable r : real;
	begin
		tmp:= word_to_int(x);
		r:=real(tmp)/real(fractionalFactor);
		return r;
	end;


	function word_to_ureal(x : in std_logic_vector; fractionalFactor : integer) return real is
	variable tmp : integer;
	variable r : real;
	begin
		tmp:= word_to_uint(x);
		r:=real(tmp)/real(fractionalFactor);
		return r;
	end;

	function word_to_uint(x : in std_logic_vector) return integer is
	variable i : integer range 0 to x'high;
	variable tmp : unsigned(x'high downto 0);
	variable r : integer;
	begin
		for i in 0 to x'high loop
			tmp(i) := x(i);
		end loop;
		r := to_integer(tmp);
		return r;
	end;
	function word_to_int(x : in std_logic_vector) return integer is
	variable i : integer range 0 to x'high;
	variable tmp : signed(x'high downto 0);
	variable r : integer;
	begin
		for i in 0 to x'high loop
			tmp(i) := x(i);
		end loop;
		r := to_integer(tmp);
		return r;
	end;

	function int_to_word(x : integer; nb_bit : integer) return std_logic_vector is
	variable i : integer range 0 to nb_bit-1;
	variable tmp : signed(nb_bit-1 downto 0);
	variable r : std_logic_vector(nb_bit-1 downto 0);
	begin
		tmp := to_signed(x, nb_bit);
		for i in 0 to nb_bit-1 loop
			r(i) := tmp(i);
		end loop;
		return r;
	end;

    function MAX_INTEGER (LEFT, RIGHT: INTEGER) return INTEGER is
 	begin
      if LEFT > RIGHT then return LEFT;
      else return RIGHT;
      end if;
    end MAX_INTEGER;

    function MIN_INTEGER (LEFT, RIGHT: INTEGER) return INTEGER is
 	begin
      if LEFT < RIGHT then return LEFT;
      else return RIGHT;
      end if;
    end MIN_INTEGER;


    function to_slv(x : unsigned; size: INTEGER) return std_logic_vector is
    variable r : std_logic_vector(size-1 downto 0);
    begin
         r:=std_logic_vector(resize(x,size));
         return r;
    end to_slv;

    function to_slv(x : signed; size: INTEGER) return std_logic_vector is
    variable r : std_logic_vector(size-1 downto 0);
    begin
         r:=std_logic_vector(resize(x,size));
         return r;
    end to_slv;

end bitwidth_notech;