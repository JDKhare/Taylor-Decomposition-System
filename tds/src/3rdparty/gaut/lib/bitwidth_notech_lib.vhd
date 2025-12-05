--------------------------------------------------------------------
-- resize
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnwrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnwrap_op_s_s_arch of trnwrap_op_s_s is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN,
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnsat_op_s_s_arch of trnsat_op_s_s is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnsatzero_op_s_s_arch of trnsatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsatsym_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnsatsym_op_s_s_arch of trnsatsym_op_s_s is
	begin
	process (a)
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndwrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndwrap_op_s_s_arch of rndwrap_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_WRAP) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndsat_op_s_s_arch of rndsat_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndsatzero_s_s_arch of rndsatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsatsym_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndsatsym_s_s_arch of rndsatsym_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfwrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndinfwrap_s_s_arch of rndinfwrap_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndinfsat_s_s_arch of rndinfsat_op_s_s is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndinfsatzero_s_s_arch of rndinfsatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsatsym_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndinfsatsym_s_s_arch of rndinfsatsym_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerowrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnzerowrap_s_s_arch of trnzerowrap_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO,
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnzerosat_s_s_arch of trnzerosat_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnzerosatzero_s_s_arch of trnzerosatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosatsym_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture trnzerosatsym_s_s_arch of trnzerosatsym_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfwrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndmininfwrap_s_s_arch of rndmininfwrap_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndmininfsat_s_s_arch of rndmininfsat_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndmininfsatzero_s_s_arch of rndmininfsatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsatsym_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndmininfsatsym_s_s_arch of rndmininfsatsym_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;




library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerowrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndzerowrap_s_s_arch of rndzerowrap_op_s_s is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerosat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndzerosat_s_s_arch of rndzerosat_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_SAT) ;
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerosatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndzerosatzero_s_s_arch of rndzerosatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvwrap_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndconvwrap_s_s_arch of rndconvwrap_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsat_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndconvsat_s_s_arch of rndconvsat_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsatzero_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndconvsatzero_s_s_arch of rndconvsatzero_op_s_s is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsatsym_op_s_s is
		port ( a :in sfixed; 
		       o :out sfixed
		);
	end;
	architecture rndconvsatsym_s_s_arch of rndconvsatsym_op_s_s is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnwrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnwrap_op_u_u_arch of trnwrap_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsat_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnsat_op_u_u_arch of trnsat_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN,
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsatzero_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnsatzero_op_u_u_arch of trnsatzero_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsatsym_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnsatsym_op_u_u_arch of trnsatsym_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndwrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndwrap_op_u_u_arch of rndwrap_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsat_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndsat_op_u_u_arch of rndsat_op_u_u is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsatzero_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndsatzero_u_u_arch of rndsatzero_op_u_u is
	begin
	process (a)
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsatsym_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndsatsym_u_u_arch of rndsatsym_op_u_u is
	begin
	process (a)
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfwrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfwrap_u_u_arch of rndinfwrap_op_u_u is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsat_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfsat_u_u_arch of rndinfsat_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsatzero_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfsatzero_u_u_arch of rndinfsatzero_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF,
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsatsym_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfsatsym_u_u_arch of rndinfsatsym_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerowrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnzerowrap_u_u_arch of trnzerowrap_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosat_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnzerosat_u_u_arch of trnzerosat_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosatzero_op_u_u is
		port ( a :in ufixed;
		       o :out ufixed
		);
	end;
	architecture trnzerosatzero_u_u_arch of trnzerosatzero_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosatsym_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture trnzerosatsym_u_u_arch of trnzerosatsym_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfwrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfwrap_u_u_arch of rndmininfwrap_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsat_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfsat_u_u_arch of rndmininfsat_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsatzero_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfsatzero_u_u_arch of rndmininfsatzero_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsatsym_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfsatsym_u_u_arch of rndmininfsatsym_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;




library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerowrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndzerowrap_u_u_arch of rndzerowrap_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_WRAP) ; 
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerosat_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndzerosat_u_u_arch of rndzerosat_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_SAT) ;
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerosatzero_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndzerosatzero_u_u_arch of rndzerosatzero_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvwrap_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndconvwrap_u_u_arch of rndconvwrap_op_u_u is
	begin
	process (a)
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV,
		       overflow_style => AC_WRAP) ; 
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsat_op_u_u is
		port ( a :in ufixed;
		       o :out ufixed
		);
	end;
	architecture rndconvsat_u_u_arch of rndconvsat_op_u_u is
	begin
	process (a)  
	begin
		o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV,
		       overflow_style => AC_SAT) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsatzero_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndconvsatzero_u_u_arch of rndconvsatzero_op_u_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsatsym_op_u_u is
		port ( a :in ufixed; 
		       o :out ufixed
		);
	end;
	architecture rndconvsatsym_u_u_arch of rndconvsatsym_op_u_u is
	begin
	process (a)
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;



------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnwrap_op_s_u is
		port ( a :in sfixed;
		       o :out ufixed
		);
	end;
	architecture trnwrap_op_s_u_arch of trnwrap_op_s_u is
	begin
	process (a)
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN,
		       overflow_style => AC_WRAP) ; 
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture trnsat_op_s_u_arch of trnsat_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsatzero_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture trnsatzero_op_s_u_arch of trnsatzero_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnsatsym_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture trnsatsym_op_s_u_arch of trnsatsym_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN, 
		       overflow_style => AC_SAT_SYM);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndwrap_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndwrap_op_s_u_arch of rndwrap_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_WRAP);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndsat_op_s_u_arch of rndsat_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND,
		       overflow_style => AC_SAT) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsatzero_op_s_u is
		port ( a :in sfixed;
		       o :out ufixed
		);
	end;
	architecture rndsatzero_s_u_arch of rndsatzero_op_s_u is
	begin
	process (a)
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndsatsym_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndsatsym_s_u_arch of rndsatsym_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND, 
		       overflow_style => AC_SAT_SYM);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfwrap_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfwrap_s_u_arch of rndinfwrap_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_WRAP) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfsat_s_u_arch of rndinfsat_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF,
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsatzero_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfsatzero_s_u_arch of rndinfsatzero_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT_ZERO);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndinfsatsym_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndinfsatsym_s_u_arch of rndinfsatsym_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_INF, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerowrap_op_s_u is
		port ( a :in sfixed;
		       o :out ufixed
		);
	end;
	architecture trnzerowrap_s_u_arch of trnzerowrap_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_WRAP) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture trnzerosat_s_u_arch of trnzerosat_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosatzero_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture trnzerosatzero_s_u_arch of trnzerosatzero_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT_ZERO);
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity trnzerosatsym_op_s_u is
		port ( a :in ufixed;
		       o :out ufixed
		);
	end;
	architecture trnzerosatsym_s_u_arch of trnzerosatsym_op_s_u is
	begin
	process (a)  
	begin
			o <= resize(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_TRN_ZERO, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfwrap_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfwrap_s_u_arch of rndmininfwrap_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_WRAP) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfsat_s_u_arch of rndmininfsat_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsatzero_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfsatzero_s_u_arch of rndmininfsatzero_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndmininfsatsym_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndmininfsatsym_s_u_arch of rndmininfsatsym_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_MIN_INF, 
		       overflow_style => AC_SAT_SYM);
	end process;
	end;




library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerowrap_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndzerowrap_s_u_arch of rndzerowrap_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_WRAP) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerosat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndzerosat_s_u_arch of rndzerosat_op_s_u is
	begin
	process (a)
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_SAT) ;
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndzerosatzero_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndzerosatzero_s_u_arch of rndzerosatzero_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_ZERO, 
		       overflow_style => AC_SAT_ZERO);
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsat_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndconvsat_s_u_arch of rndconvsat_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT) ;
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsatzero_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndconvsatzero_s_u_arch of rndconvsatzero_op_s_u is
	begin
	process (a)  
	begin
			o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT_ZERO) ;
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.fixed_pkg.all;
library work;
use work.bitwidth_notech.all;

	entity rndconvsatsym_op_s_u is
		port ( a :in sfixed; 
		       o :out ufixed
		);
	end;
	architecture rndconvsatsym_s_u_arch of rndconvsatsym_op_s_u is
	begin
	process (a)  
	begin
		o <= resize_s2u(   arg            => a ,
                       left_index     => o'high,
                       right_index    => o'low,
                       round_style    => AC_RND_CONV, 
		       overflow_style => AC_SAT_SYM) ;
	end process;
	end;

----------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity extend_op_u_u is
		port ( a :in unsigned;
		       o :out unsigned );
	end;

	architecture extend_op_u_u_arch of extend_op_u_u is
	begin
	process (a)
	begin
		o <= resize(a,o'length);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity extend_op_s_s is
		port ( a :in signed;
		       o :out signed );
	end;

	architecture extend_op_s_s_arch of extend_op_s_s is
	begin
	process (a)
	begin
		o <= resize(a,o'length);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity extend_op_s_u is
		port ( a :in signed;
		       o :out unsigned );
	end;

	architecture extend_op_s_u_arch of extend_op_s_u is
	begin
	process (a)
	begin
		o <= unsigned(resize(a,o'length));
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity extend_op_u_s is
		port ( a :in unsigned;
		       o :out signed );
	end;

	architecture extend_op_u_s_arch of extend_op_u_s is
	begin
	process (a)
	begin
		o <= resize(signed("0"&a),o'length);
	end process;
end;


--
--------------------------------------------------------------------
-- acs4
--------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.bitwidth_notech.all;

entity acs4_op_s_s_s_s_s_s_s_s_s is
		port ( 
				a,b,c,d,s,t,u,v : in signed;
				s4 : out signed
		);
	end;

architecture acs4_op_s_s_s_s_s_s_s_s_s_arch of acs4_op_s_s_s_s_s_s_s_s_s is
	begin
		process (a,b,c,d,s,t,u,v)
			variable x0: signed(MAX_INTEGER(a'length, s'length) downto 0);
			variable x1: signed(MAX_INTEGER(b'length, t'length) downto 0);
			variable x2: signed(MAX_INTEGER(c'length, u'length) downto 0);
			variable x3: signed(MAX_INTEGER(d'length, v'length) downto 0);
			variable x : signed(MAX_INTEGER(x0'length, x1'length) downto 0);
			variable y	: signed(MAX_INTEGER(x2'length, x3'length) downto 0);

		begin
			x0:=a+s;
			x1:=b+t;
			x2:=c+u;
			x3:=d+v;


				if (x0<x1)then
					x:=x0;
				else
					x:=x1;
				end if;

				if (x2<x3)then
					y:=x2;
				else
					y:=x3;
				end if;
	
				if (x<y)then
					s4<=x;
				else
					s4<=y;
		end if;
		
	end process;
	
	end;
--------------------------------------------------------------------
-- min4
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity min4_op_s_s_s_s_s is
		port ( 
				x0,x1,x2,x3 : in signed;
				s3 : out signed
		);
	end;

	architecture min4_op_s_s_s_s_s_arch of min4_op_s_s_s_s_s is
		
	begin
		process (x0,x1,x2,x3)
			variable x : signed(MAX_INTEGER(x0'length, x1'length) downto 0);
			variable y	: signed(MAX_INTEGER(x2'length, x3'length) downto 0);
	begin

				if (x0<x1)then
					x:=x0;
				else
					x:=x1;
				end if;

				if (x2<x3)then
					y:=x2;
				else
					y:=x3;
				end if;

				if (x<y)then
					s3<=x;
		else
					s3<=y;
		end if;
		
	end process;
	end;

--------------------------------------------------------------------
-- min2
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity min2_op_s_s_s is
		port ( x0,x1 : in signed;
		       s1 : out signed
		);
	end;

	architecture min2_op_s_s_s_arch of min2_op_s_s_s is
	begin
		process (x0,x1)
		begin
				if (x0<x1)then
					s1<=x0;
				else
					s1<=x1;
				end if;
		end process;
	end;
--------------------------------------------------------------------
-- add
-- Sr = S1 | S2
-- Wr = MAX_INTEGER(W1+!S1&S2,W2+!S2&S1)+1
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity add_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture add_op_u_u_u_arch of add_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity add_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture add_op_s_u_s_arch of add_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b), b_tmp'length);
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity add_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture add_op_u_s_s_arch of add_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity add_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture add_op_s_s_s_arch of add_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity add_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned
		);
	end;

	architecture add_op_s_s_u_arch of add_op_s_s_u is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= unsigned(tmp(o'length-1 downto 0)); 
	end process;
	end;

-----------------------------------------------------------------------
-- sub
-- Sr = true (signed)
-- Wr = MAX_INTEGER(W1+!S1&S2,W2+!S2&S1)+1
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity sub_op_u_u_s is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture sub_op_u_u_s_arch of sub_op_u_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length);
		tmp := a_tmp - b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity sub_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture sub_op_u_u_u_arch of sub_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length);
		tmp := unsigned(a_tmp - b_tmp); -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity sub_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture sub_op_s_u_s_arch of sub_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length);
		tmp := a_tmp - b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity sub_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture sub_op_u_s_s_arch of sub_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp - b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity sub_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture sub_op_s_s_s_arch of sub_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp - b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- mul
-- Sr = S1 | S2
-- Wr = W1 + W2
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity mul_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture mul_op_u_u_u_arch of mul_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length+b'length-1 downto 0);
	begin
		 tmp := a * b;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity mul_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture mul_op_s_s_s_arch of mul_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length+b'length-1 downto 0);
	begin
		 tmp := a * b;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity mul_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture mul_op_s_u_s_arch of mul_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length+b'length downto 0);
	variable b_tmp : signed(b'length downto 0);
	begin
		 b_tmp := signed("0"&b);
		 tmp := a * b_tmp;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity mul_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture mul_op_u_s_s_arch of mul_op_u_s_s is
	begin
	process (a, b)
	variable a_tmp : signed(a'length downto 0);
	variable tmp : signed(a'length+b'length downto 0);
	begin
		 a_tmp := signed("0"&a);
		 tmp := a_tmp * b;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- addsub
-- Sr = 1
-- Wr = MAX_INTEGER(W1+!S1&S2,W2+!S2&S1)+1
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity addsub_op_u_u_s is
		port ( a :in unsigned;
		       b :in unsigned;
			   cmd : in std_logic;
		       o :out signed
		);
	end;

	architecture addsub_op_u_u_s_arch of addsub_op_u_u_s is
	begin
	process (a, b, cmd)
	variable tmp : signed(MAX_INTEGER(a'length, b'length) downto 0);
   	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length);
		if (cmd = '0') then
		   tmp := a_tmp + b_tmp; -- after 1 ns;
		else
		   tmp := a_tmp - b_tmp; -- after 1 ns;
		end if;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity addsub_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
			   cmd : in std_logic;
		       o :out signed
		);
	end;

	architecture addsub_op_s_s_s_arch of addsub_op_s_s_s is
	begin
	process (a, b, cmd)
	variable tmp : signed(MAX_INTEGER(a'length, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		if (cmd = '0') then
		   tmp := a_tmp + b_tmp; -- after 1 ns;
		else
		   tmp := a_tmp - b_tmp; -- after 1 ns;
		end if;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity addsub_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
			   cmd : in std_logic;
		       o :out signed
		);
	end;

	architecture addsub_op_s_u_s_arch of addsub_op_s_u_s is
	begin
	process (a, b, cmd)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length);
		if (cmd = '0') then
		   tmp := a_tmp + b_tmp; -- after 1 ns;
		else
		   tmp := a_tmp - b_tmp; -- after 1 ns;
		end if;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity addsub_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
			   cmd : in std_logic;
		       o :out signed
		);
	end;

	architecture addsub_op_u_s_s_arch of addsub_op_u_s_s is
	begin
	process (a, b, cmd)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length) downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		if (cmd = '0') then
		   tmp := a_tmp + b_tmp; -- after 1 ns;
		else
		   tmp := a_tmp - b_tmp; -- after 1 ns;
		end if;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- div
-- Sr = S1 | S2
-- Wr = W1 + S2
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity div_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture div_op_u_u_u_arch of div_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		 tmp := a / b;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity div_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture div_op_s_s_s_arch of div_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length downto 0);
	begin
		 tmp := a / b;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity div_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture div_op_u_s_s_arch of div_op_u_s_s is
	begin
	process (a, b)
	variable a_tmp : signed(a'length downto 0);
	variable tmp : signed(a'length downto 0);
	begin
		 a_tmp := signed("0"&a);
		 tmp := a_tmp / b;
		 o <= tmp(o'length-1 downto 0);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity div_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture div_op_s_u_s_arch of div_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	variable b_tmp : signed(b'length downto 0);
	begin
	    	 b_tmp := signed("0" & b);
		 tmp := a / b_tmp;
		 o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- mod
-- Sr = S1
-- Wr = Min(W1,W2+!S2&S1)
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity mod_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture mod_op_u_u_u_arch of mod_op_u_u_u is
	begin
	process (a, b)
	--variable tmp : unsigned(MIN_INTEGER(a'length,b'length)-1 downto 0);
	variable tmp : unsigned(b'length-1 downto 0); -- cf numeric_std.vhd
	begin
		tmp := a mod b;
		o <= tmp(o'length-1 downto 0);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity mod_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture mod_op_s_s_s_arch of mod_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(b'length-1 downto 0);
	begin
		tmp := a mod b;
		o <= tmp(o'length-1 downto 0);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity mod_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture mod_op_s_u_s_arch of mod_op_s_u_s is
	begin
	process (a, b)
	variable b_tmp : signed(b'length downto 0);
	variable tmp : signed(b'length-1 downto 0);
	begin
		b_tmp := signed("0"&b);
		tmp := a mod b_tmp;
		o <= tmp(o'length-1 downto 0);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity mod_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture mod_op_u_s_u_arch of mod_op_u_s_u is
	begin
	process (a, b)
	variable b_tmp : signed(b'length downto 0);
	variable tmp : unsigned(b'length-1 downto 0);
	begin
		b_tmp := abs(b);
		tmp := a mod unsigned(b_tmp);
		o <= tmp(o'length-1 downto 0);
	end process;
end;

--------------------------------------------------------------------
-- eq
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eq_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture eq_op_u_u_u_arch of eq_op_u_u_u is
	begin
	process (a, b)
	begin
   	     o <= to_unsigned(0,o'length);
 	     if a = b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eq_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture eq_op_s_s_u_arch of eq_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if a = b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eq_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture eq_op_u_s_u_arch of eq_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if signed("0"&a) = b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eq_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture eq_op_s_u_u_arch of eq_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a = signed("0"&b) then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


--------------------------------------------------------------------
-- ne
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ne_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture ne_op_u_u_u_arch of ne_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if a /= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ne_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture ne_op_s_s_u_arch of ne_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a /= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ne_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture ne_op_u_s_u_arch of ne_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if signed("0"&a) /= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ne_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture ne_op_s_u_u_arch of ne_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a /= signed("0"&b) then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

--------------------------------------------------------------------
-- le
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity le_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture le_op_u_u_u_arch of le_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a <= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity le_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture le_op_s_s_u_arch of le_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a <= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity le_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture le_op_u_s_u_arch of le_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if signed("0"&a) <= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity le_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture le_op_s_u_u_arch of le_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a <= signed("0"&b) then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

--------------------------------------------------------------------
-- ge
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ge_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture ge_op_u_u_u_arch of ge_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a >= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ge_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture ge_op_s_s_u_arch of ge_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a >= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ge_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture ge_op_u_s_u_arch of ge_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if signed("0"&a) >= b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ge_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture ge_op_s_u_u_arch of ge_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a >= signed("0"&b) then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


--------------------------------------------------------------------
-- gt
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gt_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture gt_op_u_u_u_arch of gt_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a > b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gt_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture gt_op_s_s_u_arch of gt_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if a > b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gt_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture gt_op_u_s_u_arch of gt_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if signed("0"&a) > b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gt_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture gt_op_s_u_u_arch of gt_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);	
		if a > signed("0"&b) then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

--------------------------------------------------------------------
-- eqmux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_u_u_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture eqmux_op_u_u_u_u_u_arch of eqmux_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_u_s_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture eqmux_op_u_u_s_u_u_arch of eqmux_op_u_u_s_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= unsigned(resize(c,o'length));
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_u_u_s_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in signed;
		       o : out unsigned
		);
	end;

	architecture eqmux_op_u_u_u_s_u_arch of eqmux_op_u_u_u_s_u is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= resize(c,o'length);
		else
			o <= unsigned(resize(d,o'length));
		end if;
	end process;
end;




library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_s_s_s_s_s is
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture eqmux_op_s_s_s_s_s_arch of eqmux_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_u_s_s_s is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture eqmux_op_u_u_s_s_s_arch of eqmux_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_u_s_s_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out unsigned
		);
	end;

	architecture eqmux_op_u_u_s_s_u_arch of eqmux_op_u_u_s_s_u is
	begin
	process (a,b,c,d)
	begin
		if (a = b) then
			o <= resize(unsigned("0"&c),o'length);
		else
			o <= resize(unsigned("0"&d),o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_s_u_s_s_s is
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture eqmux_op_s_u_s_s_s_arch of eqmux_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a = signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_s_s_s_s is
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture eqmux_op_u_s_s_s_s_arch of eqmux_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) = b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_s_u_u_u_u is
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture eqmux_op_s_u_u_u_u_arch of eqmux_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a = signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity eqmux_op_u_s_u_u_u is
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture eqmux_op_u_s_u_u_u_arch of eqmux_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) = b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;
--------------------------------------------------------------------
-- nemux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_u_u_u_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture nemux_op_u_u_u_u_u_arch of nemux_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a /= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_s_s_s_s_s is
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture nemux_op_s_s_s_s_s_arch of nemux_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a /= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_u_u_s_s_s is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture nemux_op_u_u_s_s_s_arch of nemux_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a /= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_s_u_s_s_s is
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture nemux_op_s_u_s_s_s_arch of nemux_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a /= signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_u_s_s_s_s is
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture nemux_op_u_s_s_s_s_arch of nemux_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) /= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_s_u_u_u_u is
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture nemux_op_s_u_u_u_u_arch of nemux_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a /= signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity nemux_op_u_s_u_u_u is
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture nemux_op_u_s_u_u_u_arch of nemux_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) /= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

--------------------------------------------------------------------
-- lemux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_u_u_u_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture lemux_op_u_u_u_u_u_arch of lemux_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a <= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_s_s_s_s_s is
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture lemux_op_s_s_s_s_s_arch of lemux_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a <= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_u_u_s_s_s is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture lemux_op_u_u_s_s_s_arch of lemux_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a <= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_s_u_s_s_s is
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture lemux_op_s_u_s_s_s_arch of lemux_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a <= signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_u_s_s_s_s is
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture lemux_op_u_s_s_s_s_arch of lemux_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) <= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_s_u_u_u_u is
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture lemux_op_s_u_u_u_u_arch of lemux_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a <= signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lemux_op_u_s_u_u_u is
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture lemux_op_u_s_u_u_u_arch of lemux_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) <= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;
--------------------------------------------------------------------
-- gemux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_u_u_u_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture gemux_op_u_u_u_u_u_arch of gemux_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a >= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_s_s_s_s_s is
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gemux_op_s_s_s_s_s_arch of gemux_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a >= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_u_u_s_s_s is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gemux_op_u_u_s_s_s_arch of gemux_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a >= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_s_u_s_s_s is
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gemux_op_s_u_s_s_s_arch of gemux_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a >= signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_u_s_s_s_s is
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gemux_op_u_s_s_s_s_arch of gemux_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) >= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_s_u_u_u_u is
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture gemux_op_s_u_u_u_u_arch of gemux_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a >= signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gemux_op_u_s_u_u_u is
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture gemux_op_u_s_u_u_u_arch of gemux_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) >= b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


--------------------------------------------------------------------
-- ltmux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_u_u_u_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture ltmux_op_u_u_u_u_u_arch of ltmux_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a < b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_s_s_s_s_s is
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture ltmux_op_s_s_s_s_s_arch of ltmux_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a < b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_u_u_s_s_s is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture ltmux_op_u_u_s_s_s_arch of ltmux_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a < b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_s_u_s_s_s is
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture ltmux_op_s_u_s_s_s_arch of ltmux_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (a < signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_u_s_s_s_s is
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture ltmux_op_u_s_s_s_s_arch of ltmux_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) < b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_s_u_u_u_u is
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture ltmux_op_s_u_u_u_u_arch of ltmux_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (a < signed("0"&b)) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ltmux_op_u_s_u_u_u is
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture ltmux_op_u_s_u_u_u_arch of ltmux_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	begin
		if (signed("0"&a) < b) then
			o <= resize(c,o'length);
		else
			o <= resize(d,o'length);
		end if;
	end process;
end;

--------------------------------------------------------------------
-- gtmux
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_u_u_u_u_u is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture gtmux_op_u_u_u_u_u_arch of gtmux_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (a > b) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
                o<=resize(tmp,o'length);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_s_s_s_s_s is
		port ( a : in signed;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gtmux_op_s_s_s_s_s_arch of gtmux_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (a > b) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
                o<=resize(tmp,o'length);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_u_u_s_s_s is
		port ( a : in unsigned;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gtmux_op_u_u_s_s_s_arch of gtmux_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (a > b) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
                o<=resize(tmp,o'length);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_s_u_s_s_s is
		port ( a : in signed;
		       b : in unsigned;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gtmux_op_s_u_s_s_s_arch of gtmux_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (a > signed("0"&b)) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
                o<=resize(tmp,o'length);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_u_s_s_s_s is
		port ( a : in unsigned;
		       b : in signed;
		       c : in signed;
		       d : in signed;
		       o : out signed
		);
	end;

	architecture gtmux_op_u_s_s_s_s_arch of gtmux_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (signed("0"&a) > b) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
                o<=resize(tmp,o'length);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_s_u_u_u_u is
		port ( a : in signed;
		       b : in unsigned;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture gtmux_op_s_u_u_u_u_arch of gtmux_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (a > signed("0"&b)) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
                o<=resize(tmp,o'length);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity gtmux_op_u_s_u_u_u is
		port ( a : in unsigned;
		       b : in signed;
		       c : in unsigned;
		       d : in unsigned;
		       o : out unsigned
		);
	end;

	architecture gtmux_op_u_s_u_u_u_arch of gtmux_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	begin
		c_tmp := resize(c, c_tmp'length);
		d_tmp := resize(d, d_tmp'length);

		if (signed("0"&a) > b) then
			tmp:= c_tmp;
		else
			tmp:= d_tmp;
		end if;
		o<=resize(tmp,o'length);
	end process;
end;

--------------------------------------------------------------------
-- logic_w = MAX_INTEGER(W1+(S2&&!S1),W2+(S1&&!S2)),
-- logic_s = S1|S2
--------------------------------------------------------------------
--------------------------------------------------------------------
-- and
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity and_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture and_op_u_u_u_arch of and_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity and_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture and_op_s_s_s_arch of and_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity and_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture and_op_u_s_s_arch of and_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity and_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture and_op_s_u_s_arch of and_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- truth_and
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_and_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture truth_and_op_u_u_u_arch of truth_and_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_and_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture truth_and_op_s_s_s_arch of truth_and_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_and_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture truth_and_op_u_s_s_arch of truth_and_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_and_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture truth_and_op_s_u_s_arch of truth_and_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- nand
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nand_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture nand_op_u_u_u_arch of nand_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nand_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture nand_op_s_s_s_arch of nand_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nand_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture nand_op_u_s_s_arch of nand_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nand_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture nand_op_s_u_s_arch of nand_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;


--------------------------------------------------------------------
-- or
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity or_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture or_op_u_u_u_arch of or_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity or_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture or_op_s_s_s_arch of or_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity or_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture or_op_u_s_s_arch of or_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity or_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture or_op_s_u_s_arch of or_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- truth_or
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_or_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture truth_or_op_u_u_u_arch of truth_or_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_or_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture truth_or_op_s_s_s_arch of truth_or_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_or_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture truth_or_op_u_s_s_arch of truth_or_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_or_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture truth_or_op_s_u_s_arch of truth_or_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- xor
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity xor_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture xor_op_u_u_u_arch of xor_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity xor_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture xor_op_s_s_s_arch of xor_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity xor_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture xor_op_u_s_s_arch of xor_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity xor_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture xor_op_s_u_s_arch of xor_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;
--------------------------------------------------------------------
-- nor
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nor_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture nor_op_u_u_u_arch of nor_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : unsigned(tmp'range);
	variable b_tmp : unsigned(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nor_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture nor_op_s_s_s_arch of nor_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nor_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture nor_op_u_s_s_arch of nor_op_u_s_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length+1, b'length)-1 downto 0);
	variable a_tmp : signed(tmp'range);
	variable b_tmp : signed(tmp'range);
	begin
		a_tmp := resize(signed("0"&a),a_tmp'length); 
		b_tmp := resize(b, b_tmp'length);
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity nor_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed
		);
	end;

	architecture nor_op_s_u_s_arch of nor_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(MAX_INTEGER(a'length, b'length+1)-1 downto 0);
	variable b_tmp : signed(tmp'range);
	variable a_tmp : signed(tmp'range);
	begin
		a_tmp := resize(a, a_tmp'length);
		b_tmp := resize(signed("0"&b),b_tmp'length); 
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		o <= tmp(o'length-1 downto 0);
	end process;
	end;
--------------------------------------------------------------------
-- abs
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity abs_op_s_s is
		port ( a :in signed;
		       o :out signed
		);
	end;

	architecture abs_op_s_s_arch of abs_op_s_s is
	begin
	process (a)
	begin
		o <= abs(a);
	end process;
	end;

--------------------------------------------------------------------
-- uminus
-- Sr = -
-- Wr = W1+1 (not done in numeric_std !!!)
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity uminus_op_s_s is
		port ( a :in signed;
		       o :out signed
		);
	end;

	architecture uminus_op_s_s_arch of uminus_op_s_s is
	begin
	process (a)
	variable tmp : signed(a'length-1 downto 0);

	begin
		tmp := - a;
		o <= resize(tmp,o'length);
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity uminus_op_u_s is
		port ( a :in unsigned;
		       o :out signed
		);
	end;

	architecture uminus_op_u_s_arch of uminus_op_u_s is
	begin
	process (a)
	begin
		o <= - signed("0"&a);
	end process;
	end;

--------------------------------------------------------------------
-- sll
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sll_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture sll_op_u_u_u_arch of sll_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a sll TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sll_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture sll_op_u_s_u_arch of sll_op_u_s_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a sll TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sll_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture sll_op_s_s_s_arch of sll_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a sll TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sll_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture sll_op_s_s_u_arch of sll_op_s_s_u is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a sll TO_INTEGER(b);
		o <= unsigned(tmp(o'length-1 downto 0));
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sll_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture sll_op_s_u_s_arch of sll_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a sll TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- srl
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity srl_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture srl_op_u_u_u_arch of srl_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a srl TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity srl_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture srl_op_u_s_u_arch of srl_op_u_s_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a srl TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity srl_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture srl_op_s_s_s_arch of srl_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a srl TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity srl_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture srl_op_s_u_s_arch of srl_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a srl TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- rol
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity rol_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture rol_op_u_u_u_arch of rol_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a rol TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity rol_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture rol_op_u_s_u_arch of rol_op_u_s_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a rol TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity rol_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture rol_op_s_s_s_arch of rol_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a rol TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity rol_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture rol_op_s_u_s_arch of rol_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a rol TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- ror
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ror_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture ror_op_u_u_u_arch of ror_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a ror TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ror_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture ror_op_u_s_u_arch of ror_op_u_s_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp := a ror TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ror_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture ror_op_s_s_s_arch of ror_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a ror TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity ror_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture ror_op_s_u_s_arch of ror_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp := a ror TO_INTEGER(b);
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- sla
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sla_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture sla_op_s_s_s_arch of sla_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp :=signed  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sla TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sla_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture sla_op_s_u_s_arch of sla_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp :=signed  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sla TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sla_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture sla_op_u_u_u_arch of sla_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp :=unsigned  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sla TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sla_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture sla_op_u_s_u_arch of sla_op_u_s_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp :=unsigned  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sla TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

--------------------------------------------------------------------
-- sra
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sra_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed );
	end;

	architecture sra_op_s_s_s_arch of sra_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp :=signed  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sra TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sra_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed );
	end;

	architecture sra_op_s_u_s_arch of sra_op_s_u_s is
	begin
	process (a, b)
	variable tmp : signed(a'length-1 downto 0);
	begin
		tmp :=signed  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sra TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sra_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned );
	end;

	architecture sra_op_u_u_u_arch of sra_op_u_u_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp :=unsigned  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sra TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity sra_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned );
	end;

	architecture sra_op_u_s_u_arch of sra_op_u_s_u is
	begin
	process (a, b)
	variable tmp : unsigned(a'length-1 downto 0);
	begin
		tmp :=unsigned  (To_StdLogicVector (To_bitvector(std_logic_vector(a)) sra TO_INTEGER(b)));
		o <= tmp(o'length-1 downto 0);
	end process;
	end;



--------------------------------------------------------------------
-- shl
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shl_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture shl_op_u_u_u_arch of shl_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= unsigned(SHL(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shl_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture shl_op_u_s_u_arch of shl_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= unsigned(SHL(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shl_op_u_s_s is
		port ( a :in unsigned;
		       b :in signed;
		       o :out signed);
	end;

	architecture shl_op_u_s_s_arch of shl_op_u_s_s is
	begin
	process (a, b)
	begin
		o <= signed(SHL(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shl_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed);
	end;

	architecture shl_op_s_s_s_arch of shl_op_s_s_s is
	begin
	process (a, b)
	begin
		o <= signed(SHL(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shl_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed);
	end;

	architecture shl_op_s_u_s_arch of shl_op_s_u_s is
	begin
	process (a, b)
	begin
		o <= signed(SHL(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;

--------------------------------------------------------------------
-- shr
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shr_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture shr_op_u_u_u_arch of shr_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= unsigned(SHR(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shr_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture shr_op_u_s_u_arch of shr_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= unsigned(SHR(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shr_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed);
	end;

	architecture shr_op_s_s_s_arch of shr_op_s_s_s is
	begin
	process (a, b)
	begin
		o <= signed(SHR(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shr_op_s_u_s is
		port ( a :in signed;
		       b :in unsigned;
		       o :out signed);
	end;

	architecture shr_op_s_u_s_arch of shr_op_s_u_s is
	begin
	process (a, b)
	begin
		o <= signed(SHR(std_logic_vector(a),std_logic_vector(b)));
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
use work.bitwidth_notech.all;

	entity shr4div_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed
		);
	end;

	architecture shr4div_op_s_s_s_arch of shr4div_op_s_s_s is
	begin
	process (a, b)
	variable tmp : signed(a'LENGTH-1 downto 0);
	begin
		if (a < 0) then tmp := a+1; else tmp := a; end if;
	     	o<=signed(SHR(std_logic_vector(tmp),std_logic_vector(b)));
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use ieee.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity shr4div_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned
		);
	end;

	architecture shr4div_op_u_u_u_arch of shr4div_op_u_u_u is
	begin
	process (a, b)
	begin
	     	o<=unsigned(SHR(std_logic_vector(a),std_logic_vector(b)));
	end process;
	end;

-----------------------------------------------------------------------
-- not
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity not_op_u_u is
		port ( a :in unsigned;
		       o :out unsigned
		);
	end;

	architecture not_op_u_u_arch of not_op_u_u is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity not_op_s_s is
		port ( a :in signed;
		       o :out signed
		);
	end;

	architecture not_op_s_s_arch of not_op_s_s is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

-----------------------------------------------------------------------
-- truth_not
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_not_op_u_u is
		port ( a :in unsigned;
		       o :out unsigned
		);
	end;

	architecture truth_not_op_u_u_arch of truth_not_op_u_u is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_not_op_s_s is
		port ( a :in signed;
		       o :out signed
		);
	end;

	architecture truth_not_op_s_s_arch of truth_not_op_s_s is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

-----------------------------------------------------------------------
-- bitselect
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity bitselect_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture bitselect_op_s_u_u_arch of bitselect_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		o(0 downto 0) <= unsigned(a(TO_INTEGER(b) downto TO_INTEGER(b)));
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity bitselect_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture bitselect_op_s_s_u_arch of bitselect_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		o(0 downto 0) <= unsigned(a(TO_INTEGER(b) downto TO_INTEGER(b)));
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity bitselect_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture bitselect_op_u_u_u_arch of bitselect_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		o(0 downto 0) <= unsigned(a(TO_INTEGER(b) downto TO_INTEGER(b)));
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity bitselect_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture bitselect_op_u_s_u_arch of bitselect_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		o(0 downto 0) <= unsigned(a(TO_INTEGER(b) downto TO_INTEGER(b)));
	end process;
end;
-----------------------------------------------------------------------
-- slicewrite
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity slicewrite_op_s_s_u_s is
		port ( a :in signed;
		       b :in signed;
			 c :in unsigned;
		       o :out signed );
	end;

	architecture slicewrite_op_s_s_u_s_arch of slicewrite_op_s_s_u_s is
	begin
	process (a, b, c)
	variable lsb : integer;
	variable rang : integer;
	begin
		lsb := TO_INTEGER(b);
		rang:= TO_INTEGER(c);
		o <= a;
		o(rang+lsb-1 downto lsb) <= b;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity slicewrite_op_u_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
			   c :in unsigned;
		       o :out unsigned );
	end;

	architecture slicewrite_op_u_u_u_u_arch of slicewrite_op_u_u_u_u is
	begin
	process (a, b, c)
	variable lsb : integer;
	variable rang : integer;
	begin
		lsb := TO_INTEGER(b);
		rang:= TO_INTEGER(c);
		o <= a;
		o(rang+lsb-1 downto lsb) <= b;
	end process;
	end;

--------------------------------------------------------------------
-- add_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity add_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture add_fp_op_u_u_u_arch of add_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size add_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp + b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity add_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture add_fp_op_s_u_s_arch of add_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size add_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity add_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture add_fp_op_u_s_s_arch of add_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size add_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp + b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity add_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture add_fp_op_s_s_s_arch of add_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size add_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp + b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

--------------------------------------------------------------------
-- gt_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gt_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture gt_fp_op_u_u_u_arch of gt_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) > to_unsigned(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_unsigned(a)&ADDZERO > to_unsigned(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gt_fp_op_s_s_u is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture gt_fp_op_s_s_u_arch of gt_fp_op_s_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) > to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO > to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gt_fp_op_u_s_u is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture gt_fp_op_u_s_u_arch of gt_fp_op_u_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if "0"&to_signed(a) > to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if "0"&to_signed(a)&ADDZERO > to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gt_fp_op_s_u_u is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture gt_fp_op_s_u_u_arch of gt_fp_op_s_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) > "0"&to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO > "0"&to_signed(b)  then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;



--------------------------------------------------------------------
-- gtmux_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_u_u_u_u_u is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture gtmux_fp_op_u_u_u_u_u_arch of gtmux_fp_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) > to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO > to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_s_s_s_s_s is
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gtmux_fp_op_s_s_s_s_s_arch of gtmux_fp_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);

		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) > to_signed(b)&ADDZERO then
			   tmp:= c_tmp;
                        else
                            tmp:= d_tmp;
                        end if;
		else
			if to_signed(a)&ADDZERO > to_signed(b) then
                           tmp:= c_tmp;
                         else
                           tmp:= d_tmp;
                         end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_u_u_s_s_s is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gtmux_fp_op_u_u_s_s_s_arch of gtmux_fp_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) > to_unsigned(b)&ADDZERO 
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO > to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_s_u_s_s_s is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gtmux_fp_op_s_u_s_s_s_arch of gtmux_fp_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) > "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO > "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_u_s_s_s_s is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gtmux_fp_op_u_s_s_s_s_arch of gtmux_fp_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) > to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO > to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_s_u_u_u_u is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture gtmux_fp_op_s_u_u_u_u_arch of gtmux_fp_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);

        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) > "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO > "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gtmux_fp_op_u_s_u_u_u is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture gtmux_fp_op_u_s_u_u_u_arch of gtmux_fp_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gp_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) > to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO > to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

--------------------------------------------------------------------
-- lt
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lt_op_u_u_u is
		port ( a :in unsigned;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture lt_op_u_u_u_arch of lt_op_u_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if a < b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lt_op_s_s_s is
		port ( a :in signed;
		       b :in signed;
		       o :out signed);
	end;

	architecture lt_op_s_s_s_arch of lt_op_s_s_s is
	begin
	process (a, b)
	begin
		o <= to_signed(0,o'length);
		if a < b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lt_op_s_s_u is
		port ( a :in signed;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture lt_op_s_s_u_arch of lt_op_s_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if a < b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lt_op_u_s_u is
		port ( a :in unsigned;
		       b :in signed;
		       o :out unsigned);
	end;

	architecture lt_op_u_s_u_arch of lt_op_u_s_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if signed(a) < b then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;

	entity lt_op_s_u_u is
		port ( a :in signed;
		       b :in unsigned;
		       o :out unsigned);
	end;

	architecture lt_op_s_u_u_arch of lt_op_s_u_u is
	begin
	process (a, b)
	begin
		o <= to_unsigned(0,o'length);
		if a < signed("0"&b) then o(0) <= '1'; else o(0) <= '0'; end if;
	end process;
end;


--------------------------------------------------------------------
-- eq_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eq_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture eq_fp_op_u_u_u_arch of eq_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eq_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) = to_unsigned(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_unsigned(a)&ADDZERO = to_unsigned(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eq_fp_op_s_s_u is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture eq_fp_op_s_s_u_arch of eq_fp_op_s_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eq_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) = to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO = to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eq_fp_op_u_s_u is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture eq_fp_op_u_s_u_arch of eq_fp_op_u_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eq_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if "0"&to_signed(a) = to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if "0"&to_signed(a)&ADDZERO = to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eq_fp_op_s_u_u is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture eq_fp_op_s_u_u_arch of eq_fp_op_s_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eq_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) = "0"&to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO = "0"&to_signed(b)  then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;



--------------------------------------------------------------------
-- eqmux_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_u_u_u_u_u is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture eqmux_fp_op_u_u_u_u_u_arch of eqmux_fp_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) = to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO = to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_s_s_s_s_s is
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture eqmux_fp_op_s_s_s_s_s_arch of eqmux_fp_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);

		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) = to_signed(b)&ADDZERO then
			   tmp:= c_tmp;
                        else
                            tmp:= d_tmp;
                        end if;
		else
			if to_signed(a)&ADDZERO = to_signed(b) then
                           tmp:= c_tmp;
                         else
                           tmp:= d_tmp;
                         end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_u_u_s_s_s is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture eqmux_fp_op_u_u_s_s_s_arch of eqmux_fp_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) = to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO = to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_s_u_s_s_s is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture eqmux_fp_op_s_u_s_s_s_arch of eqmux_fp_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) = "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO = "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_u_s_s_s_s is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture eqmux_fp_op_u_s_s_s_s_arch of eqmux_fp_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) = to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO = to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_s_u_u_u_u is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture eqmux_fp_op_s_u_u_u_u_arch of eqmux_fp_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);

        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) = "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO = "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity eqmux_fp_op_u_s_u_u_u is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture eqmux_fp_op_u_s_u_u_u_arch of eqmux_fp_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size eqmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) = to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO = to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


--------------------------------------------------------------------
-- ne_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ne_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture ne_fp_op_u_u_u_arch of ne_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ne_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) /= to_unsigned(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_unsigned(a)&ADDZERO /= to_unsigned(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ne_fp_op_s_s_u is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture ne_fp_op_s_s_u_arch of ne_fp_op_s_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ne_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) /= to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO /= to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ne_fp_op_u_s_u is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture ne_fp_op_u_s_u_arch of ne_fp_op_u_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ne_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if "0"&to_signed(a) /= to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if "0"&to_signed(a)&ADDZERO /= to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ne_fp_op_s_u_u is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture ne_fp_op_s_u_u_arch of ne_fp_op_s_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ne_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) /= "0"&to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO /= "0"&to_signed(b)  then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;



--------------------------------------------------------------------
-- nemux_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_u_u_u_u_u is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture nemux_fp_op_u_u_u_u_u_arch of nemux_fp_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) /= to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO /= to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_s_s_s_s_s is
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture nemux_fp_op_s_s_s_s_s_arch of nemux_fp_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);

		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) /= to_signed(b)&ADDZERO then
			   tmp:= c_tmp;
                        else
                            tmp:= d_tmp;
                        end if;
		else
			if to_signed(a)&ADDZERO /= to_signed(b) then
                           tmp:= c_tmp;
                         else
                           tmp:= d_tmp;
                         end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_u_u_s_s_s is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture nemux_fp_op_u_u_s_s_s_arch of nemux_fp_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) /= to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO /= to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_s_u_s_s_s is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture nemux_fp_op_s_u_s_s_s_arch of nemux_fp_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) /= "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO /= "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_u_s_s_s_s is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture nemux_fp_op_u_s_s_s_s_arch of nemux_fp_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) /= to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO /= to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_s_u_u_u_u is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture nemux_fp_op_s_u_u_u_u_arch of nemux_fp_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);

        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) /= "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO /= "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nemux_fp_op_u_s_u_u_u is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture nemux_fp_op_u_s_u_u_u_arch of nemux_fp_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) /= to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO /= to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

--------------------------------------------------------------------
-- lt_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lt_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture lt_fp_op_u_u_u_arch of lt_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lt_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) < to_unsigned(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_unsigned(a)&ADDZERO < to_unsigned(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lt_fp_op_s_s_u is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture lt_fp_op_s_s_u_arch of lt_fp_op_s_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lt_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) < to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO < to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lt_fp_op_u_s_u is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture lt_fp_op_u_s_u_arch of lt_fp_op_u_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lt_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if "0"&to_signed(a) < to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if "0"&to_signed(a)&ADDZERO < to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lt_fp_op_s_u_u is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture lt_fp_op_s_u_u_arch of lt_fp_op_s_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lt_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) < "0"&to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO < "0"&to_signed(b)  then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;



--------------------------------------------------------------------
-- ltmux_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_u_u_u_u_u is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture ltmux_fp_op_u_u_u_u_u_arch of ltmux_fp_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) < to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO < to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_s_s_s_s_s is
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture ltmux_fp_op_s_s_s_s_s_arch of ltmux_fp_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);

		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) < to_signed(b)&ADDZERO then
			   tmp:= c_tmp;
                        else
                            tmp:= d_tmp;
                        end if;
		else
			if to_signed(a)&ADDZERO < to_signed(b) then
                           tmp:= c_tmp;
                         else
                           tmp:= d_tmp;
                         end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_u_u_s_s_s is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture ltmux_fp_op_u_u_s_s_s_arch of ltmux_fp_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) < to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO < to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_s_u_s_s_s is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture ltmux_fp_op_s_u_s_s_s_arch of ltmux_fp_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) < "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO < "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_u_s_s_s_s is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture ltmux_fp_op_u_s_s_s_s_arch of ltmux_fp_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) < to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO < to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_s_u_u_u_u is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture ltmux_fp_op_s_u_u_u_u_arch of ltmux_fp_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);

        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) < "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO < "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ltmux_fp_op_u_s_u_u_u is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture ltmux_fp_op_u_s_u_u_u_arch of ltmux_fp_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ltmux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) < to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO < to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

--------------------------------------------------------------------
-- le_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity le_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture le_fp_op_u_u_u_arch of le_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size le_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) <= to_unsigned(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_unsigned(a)&ADDZERO <= to_unsigned(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity le_fp_op_s_s_u is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture le_fp_op_s_s_u_arch of le_fp_op_s_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size le_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) <= to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO <= to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity le_fp_op_u_s_u is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture le_fp_op_u_s_u_arch of le_fp_op_u_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size le_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if "0"&to_signed(a) <= to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if "0"&to_signed(a)&ADDZERO <= to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity le_fp_op_s_u_u is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture le_fp_op_s_u_u_arch of le_fp_op_s_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size le_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) <= "0"&to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO <= "0"&to_signed(b)  then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;



--------------------------------------------------------------------
-- lemux_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_u_u_u_u_u is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture lemux_fp_op_u_u_u_u_u_arch of lemux_fp_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) <= to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO <= to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_s_s_s_s_s is
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture lemux_fp_op_s_s_s_s_s_arch of lemux_fp_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);

		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) <= to_signed(b)&ADDZERO then
			   tmp:= c_tmp;
                        else
                            tmp:= d_tmp;
                        end if;
		else
			if to_signed(a)&ADDZERO <= to_signed(b) then
                           tmp:= c_tmp;
                         else
                           tmp:= d_tmp;
                         end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_u_u_s_s_s is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture lemux_fp_op_u_u_s_s_s_arch of lemux_fp_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) <= to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO <= to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_s_u_s_s_s is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture lemux_fp_op_s_u_s_s_s_arch of lemux_fp_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) <= "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO <= "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_u_s_s_s_s is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture lemux_fp_op_u_s_s_s_s_arch of lemux_fp_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) <= to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO <= to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_s_u_u_u_u is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture lemux_fp_op_s_u_u_u_u_arch of lemux_fp_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);

        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) <= "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO <= "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity lemux_fp_op_u_s_u_u_u is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture lemux_fp_op_u_s_u_u_u_arch of lemux_fp_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size lemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) <= to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO <= to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


--------------------------------------------------------------------
-- ge_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ge_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture ge_fp_op_u_u_u_arch of ge_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ge_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) >= to_unsigned(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_unsigned(a)&ADDZERO >= to_unsigned(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ge_fp_op_s_s_u is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture ge_fp_op_s_s_u_arch of ge_fp_op_s_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ge_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) >= to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO >= to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ge_fp_op_u_s_u is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out ufixed);
	end;

	architecture ge_fp_op_u_s_u_arch of ge_fp_op_u_s_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ge_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if "0"&to_signed(a) >= to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if "0"&to_signed(a)&ADDZERO >= to_signed(b) then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity ge_fp_op_s_u_u is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out ufixed);
	end;

	architecture ge_fp_op_s_u_u_arch of ge_fp_op_s_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size ge_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
	        o <= to_ufixed(to_unsigned(0,o'length));
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) >= "0"&to_signed(b)&ADDZERO then o(0) <= '1'; else o(0) <= '0'; end if;
		else
			if to_signed(a)&ADDZERO >= "0"&to_signed(b)  then o(0) <= '1'; else o(0) <= '0'; end if;
		end if;
	end process;
end;



--------------------------------------------------------------------
-- gemux_fp
--------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_u_u_u_u_u is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture gemux_fp_op_u_u_u_u_u_arch of gemux_fp_op_u_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) >= to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO >= to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_s_s_s_s_s is
		port ( a : in sfixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gemux_fp_op_s_s_s_s_s_arch of gemux_fp_op_s_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);

		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_signed(a) >= to_signed(b)&ADDZERO then
			   tmp:= c_tmp;
                        else
                            tmp:= d_tmp;
                        end if;
		else
			if to_signed(a)&ADDZERO >= to_signed(b) then
                           tmp:= c_tmp;
                         else
                           tmp:= d_tmp;
                         end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_u_u_s_s_s is
		port ( a : in ufixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gemux_fp_op_u_u_s_s_s_arch of gemux_fp_op_u_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			if to_unsigned(a) >= to_unsigned(b)&ADDZERO
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		else
			if to_unsigned(a)&ADDZERO >= to_unsigned(b)
                        then tmp:= c_tmp;
                        else tmp:= d_tmp;
                        end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_s_u_s_s_s is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gemux_fp_op_s_u_s_s_s_arch of gemux_fp_op_s_u_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) >= "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO >= "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;



library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_u_s_s_s_s is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in sfixed;
		       d : in sfixed;
		       o : out sfixed
		);
	end;

	architecture gemux_fp_op_u_s_s_s_s_arch of gemux_fp_op_u_s_s_s_s is
	begin
	process (a,b,c,d)
	variable tmp : signed(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : signed(tmp'length-1 downto 0);
	variable d_tmp : signed(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_signed(c), c_tmp'length);
		d_tmp := resize(to_signed(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) >= to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO >= to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_s_u_u_u_u is
		port ( a : in sfixed;
		       b : in ufixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture gemux_fp_op_s_u_u_u_u_arch of gemux_fp_op_s_u_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);

        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if to_signed(a) >= "0"&to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if to_signed(a)&ADDZERO >= "0"&to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity gemux_fp_op_u_s_u_u_u is
		port ( a : in ufixed;
		       b : in sfixed;
		       c : in ufixed;
		       d : in ufixed;
		       o : out ufixed
		);
	end;

	architecture gemux_fp_op_u_s_u_u_u_arch of gemux_fp_op_u_s_u_u_u is
	begin
	process (a,b,c,d)
	variable tmp : unsigned(MAX_INTEGER(c'length, d'length)-1 downto 0);
	variable c_tmp : unsigned(tmp'length-1 downto 0);
	variable d_tmp : unsigned(tmp'length-1 downto 0);
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size gemux_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	begin
		c_tmp := resize(to_unsigned(c), c_tmp'length);
		d_tmp := resize(to_unsigned(d), d_tmp'length);
        	if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
		   if "0"&to_signed(a) >= to_signed(b)&ADDZERO then
			tmp:= c_tmp;
                   else
			tmp:= d_tmp;
                   end if;
		else
		    if "0"&to_signed(a)&ADDZERO >= to_signed(b) then
			tmp:= c_tmp;
                    else
			tmp:= d_tmp;
                    end if;
		end if;
		o<=to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
	end process;
end;


--------------------------------------------------------------------
-- sub_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity sub_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture sub_fp_op_u_u_u_arch of sub_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size sub_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp - b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity sub_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture sub_fp_op_s_u_s_arch of sub_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size sub_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp - b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity sub_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture sub_fp_op_u_s_s_arch of sub_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size sub_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp - b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity sub_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture sub_fp_op_s_s_s_arch of sub_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size sub_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX+1;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp - b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;

-----------------------------------------------------------------------
-- not_fp
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity not_fp_op_u_u is
		port ( a :in unsigned;
		       o :out unsigned
		);
	end;

	architecture not_fp_op_u_u_arch of not_fp_op_u_u is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity not_fp_op_s_s is
		port ( a :in signed;
		       o :out signed
		);
	end;

	architecture not_fp_op_s_s_arch of not_fp_op_s_s is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

-----------------------------------------------------------------------
-- truth_not_fp
-----------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_not_fp_op_u_u is
		port ( a :in unsigned;
		       o :out unsigned
		);
	end;

	architecture truth_not_fp_op_u_u_arch of truth_not_fp_op_u_u is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;

	entity truth_not_fp_op_s_s is
		port ( a :in signed;
		       o :out signed
		);
	end;

	architecture truth_not_fp_op_s_s_arch of truth_not_fp_op_s_s is
	begin
	process (a)
	begin
		o <= not a;
	end process;
	end;
--------------------------------------------------------------------
-- and_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity and_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture and_fp_op_u_u_u_arch of and_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size and_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity and_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture and_fp_op_s_u_s_arch of and_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size and_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity and_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture and_fp_op_u_s_s_arch of and_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size and_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity and_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture and_fp_op_s_s_s_arch of and_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size and_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;

--------------------------------------------------------------------
-- or_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity or_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture or_fp_op_u_u_u_arch of or_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size or_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity or_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture or_fp_op_s_u_s_arch of or_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size or_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity or_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture or_fp_op_u_s_s_arch of or_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size or_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity or_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture or_fp_op_s_s_s_arch of or_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size or_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;


--------------------------------------------------------------------
-- truth_and_fp_op
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity truth_and_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture truth_and_fp_op_u_u_u_arch of truth_and_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_and_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity truth_and_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture truth_and_fp_op_s_u_s_arch of truth_and_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_and_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity truth_and_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture truth_and_fp_op_u_s_s_arch of truth_and_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_and_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity truth_and_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture truth_and_fp_op_s_s_s_arch of truth_and_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_and_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp and b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;

--------------------------------------------------------------------
-- truth_or_fp_op
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity truth_or_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture truth_or_fp_op_u_u_u_arch of truth_or_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_or_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity truth_or_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture truth_or_fp_op_s_u_s_arch of truth_or_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_or_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity truth_or_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture truth_or_fp_op_u_s_s_arch of truth_or_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_or_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity truth_or_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture truth_or_fp_op_s_s_s_arch of truth_or_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size truth_or_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp or b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;




--------------------------------------------------------------------
-- nor_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity nor_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture nor_fp_op_u_u_u_arch of nor_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nor_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nor_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture nor_fp_op_s_u_s_arch of nor_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nor_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nor_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture nor_fp_op_u_s_s_arch of nor_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nor_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nor_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture nor_fp_op_s_s_s_arch of nor_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nor_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp nor b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;


--------------------------------------------------------------------
-- nand_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity nand_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture nand_fp_op_u_u_u_arch of nand_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nand_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nand_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture nand_fp_op_s_u_s_arch of nand_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nand_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nand_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture nand_fp_op_u_s_s_arch of nand_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nand_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity nand_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture nand_fp_op_s_s_s_arch of nand_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size nand_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp nand b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;



--------------------------------------------------------------------
-- xor_fp
--------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;
--USE STD.TEXTIO.ALL;

	entity xor_fp_op_u_u_u is
		port ( a :in ufixed;
		       b :in ufixed;
		       o :out ufixed
		);
	end;

	architecture xor_fp_op_u_u_u_arch of xor_fp_op_u_u_u is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size xor_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : unsigned (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : unsigned(TMP_SIZE downto 0);
	variable a_tmp : unsigned(tmp'length-1 downto 0);
	variable b_tmp : unsigned(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_unsigned(a), a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b)&ADDZERO, b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_unsigned(a)&ADDZERO, a_tmp'length);
			--assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_unsigned(b), b_tmp'length);
			--assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		--assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		--assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity xor_fp_op_s_u_s is
		port ( a :in sfixed;
		       b :in ufixed;
		       o :out sfixed
		);
	end;

	architecture xor_fp_op_s_u_s_arch of xor_fp_op_s_u_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size xor_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			b_tmp := resize("0"&to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize("0"&to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity xor_fp_op_u_s_s is
		port ( a :in ufixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture xor_fp_op_u_s_s_arch of xor_fp_op_u_s_s is
	begin
	process (a, b)
	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size xor_op component is used instead
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize("0"&to_signed(a), a_tmp'length);
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
		else
			a_tmp := resize("0"&to_signed(a)&ADDZERO, a_tmp'length);
			b_tmp := resize(to_signed(b), b_tmp'length);
		end if;
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
	end process;
	end;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
library work;
use work.bitwidth_notech.all;
library work;
use work.fixed_pkg.all;

	entity xor_fp_op_s_s_s is
		port ( a :in sfixed;
		       b :in sfixed;
		       o :out sfixed
		);
	end;

	architecture xor_fp_op_s_s_s_arch of xor_fp_op_s_s_s is
	begin
	process (a, b)

	-- FRACT_PART_DIFF >=1 because if  a_fp_size == b_fp_size xor_op component is used instead
	constant TMP_MAX_INDEX : INTEGER := MAX_INTEGER(a'high,b'high);
	constant TMP_MIN_INDEX : INTEGER := ABS(MIN_INTEGER(a'low,b'low));
	constant FRACT_PART_DIFF : INTEGER := ABS(a'low-b'low);
	constant ADDZERO : signed (FRACT_PART_DIFF-1 downto 0) := (others => '0');
	constant TMP_SIZE : INTEGER := TMP_MAX_INDEX+TMP_MIN_INDEX;
	variable tmp : signed(TMP_SIZE downto 0);
	variable a_tmp : signed(tmp'length-1 downto 0);
	variable b_tmp : signed(tmp'length-1 downto 0);
	begin
		if (a'low < b'low) then --size of fractional part of a > size of fractional part of b
			a_tmp := resize(to_signed(a), a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b)&ADDZERO, b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		else
			a_tmp := resize(to_signed(a)&ADDZERO, a_tmp'length);
			----assert (1=0) report "a_tmp = "&integer'image(TO_INTEGER(a_tmp)) severity note;
			b_tmp := resize(to_signed(b), b_tmp'length);
			----assert (1=0) report "b_tmp = "&integer'image(TO_INTEGER(b_tmp)) severity note;
		end if;
		tmp := a_tmp xor b_tmp; -- after 1 ns;
		----assert (1=0) report "tmp = "&integer'image(TO_INTEGER(tmp)) severity note;
		o <= to_fixed (tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low),o'high ,o'low);
		--o <= to_fixed (tmp(o'length-1 downto 0),o'high ,o'low);
		----assert (1=0) report "o = "&integer'image(TO_INTEGER(tmp(TMP_MIN_INDEX+o'low+o'length-1 downto TMP_MIN_INDEX+o'low))) severity note;
	end process;
        end;