-------------------------------------------------------------------------------
-- file: FSMC.vhd
-- author: Rene Herthel <rene.herthel@haw-hamburg.de>
-- author: Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
-------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


-------------------------------------------------------------------------------
-- entity
-------------------------------------------------------------------------------
entity FSMC is
	port(
		CLK_SYN		: in std_logic;
		CLK_PE		: in std_logic;
		NWE			: in std_logic;
		NE			: in std_logic;
		NOE			: in std_logic;
		RESET_N		: in std_logic;
		DATA		: inout std_logic_vector(15 downto 0);
		RDY			: out std_logic
	);
end FSMC;


architecture FSMC_ARCH of FSMC is


-------------------------------------------------------------------------------
-- component definition
-------------------------------------------------------------------------------
component EQ_PE is
	port(
		CLK_PE		: in std_logic;
		RESET_N		: in std_logic;
		START		: in std_logic;
		Y			: in std_logic_vector(15 downto 0);
		RDY			: out std_logic;
		W			: out std_logic_vector(15 downto 0)
	);
end component;


-------------------------------------------------------------------------------
-- signals
-------------------------------------------------------------------------------
signal EN		: std_logic;
signal W		: std_logic_vector(15 downto 0);
signal Y		: std_logic_vector(15 downto 0);
signal TRISTATE	: std_logic;
signal START	: std_logic;

signal NWE_Q1	: std_logic;
signal NWE_Q2	: std_logic;
signal NWE_Q3	: std_logic;
signal NWE_AND1	: std_logic;
signal NWE_AND2	: std_logic;
signal NWE_Q4	: std_logic;

signal NE_Q1	: std_logic;
signal NE_Q2	: std_logic;

signal NOE_Q1	: std_logic;
signal NOE_Q2	: std_logic;

signal DATA_Q1	: std_logic_vector(15 downto 0);
signal DATA_Q2	: std_logic_vector(15 downto 0);

signal W_CHECK	: std_logic_vector(15 downto 0);
signal Y_CHECK	: std_logic_vector(15 downto 0);


begin

-------------------------------------------------------------------------------
-- P_NWE
-------------------------------------------------------------------------------
P_NWE: process(CLK_SYN, RESET_N)
begin
	if (RESET_N = '0') then
		NWE_Q1 <= '0' after 1 ns;
		EN <= '0' after 1 ns;
	elsif (CLK_SYN = '1' and CLK_SYN'event) then
		NWE_Q1 <= NWE after 1 ns;  -- IOB
		EN <= NWE_AND2 after 1 ns; -- Pulse-Stretcher EN
	end if;
end process;


-------------------------------------------------------------------------------
-- Puls-Shorter
-------------------------------------------------------------------------------
P_Shorter: process(CLK_SYN, RESET_N)
begin
	if (RESET_N = '0') then
		NWE_Q2 <= '0' after 1 ns;
		NWE_Q3 <= '0' after 1 ns;
	elsif (CLK_SYN = '1' and CLK_SYN'event) then
		NWE_Q2 <= NWE_Q1 after 1 ns;
		NWE_Q3 <= NWE_Q2 after 1 ns;
	end if;
end process;


NWE_AND1 <= NWE_Q3 and not NWE_Q2 after 1 ns;


-------------------------------------------------------------------------------
-- Puls-Stretcher
-------------------------------------------------------------------------------
P_Stretcher1: process(EN, START)
begin
	if (START = '1') then
		NWE_Q4 <= '0' after 1 ns;
	elsif (EN = '1' and EN'event) then
		NWE_Q4 <= '1' after 1 ns;
	end if;
end process;


P_Stretcher2: process(CLK_PE, RESET_N)
begin
	if (RESET_N = '0') then
		START <= '0' after 1 ns;
	elsif (CLK_PE = '1' and CLK_PE'event) then
		START <= NWE_Q4 after 1 ns;
	end if;
end process;


-------------------------------------------------------------------------------
-- P_NE
-------------------------------------------------------------------------------
P_NE: process(CLK_SYN, RESET_N)
begin
	if (RESET_N = '0') then
		NE_Q1 <= '0' after 1 ns;
		NE_Q2 <= '0' after 1 ns;
	elsif (CLK_SYN = '1' and CLK_SYN'event) then
		NE_Q1 <= NE after 1 ns; -- IOB
		NE_Q2 <= NE_Q1 after 1 ns;
	end if;
end process;


NWE_AND2 <= NWE_AND1 and not NE_Q2 after 1 ns;


-------------------------------------------------------------------------------
-- P_NOE
-------------------------------------------------------------------------------
P_NOE: process(CLK_SYN, RESET_N)
begin
	if (RESET_N = '0') then
		NOE_Q1 <= '0' after 1 ns;
		NOE_Q2 <= '0' after 1 ns;
	elsif (CLK_SYN = '1' and CLK_SYN'event) then
		NOE_Q1 <= NOE after 1 ns; --IOB
		NOE_Q2 <= NOE_Q1 after 1 ns;
	end if;
end process;


TRISTATE <= NE_Q2 or NOE_Q2 after 1 ns;


-------------------------------------------------------------------------------
-- P_DATA
-------------------------------------------------------------------------------
P_DATA: process(TRISTATE, W_CHECK)
begin
	if (TRISTATE = '1') then
		DATA <= (others => 'Z');
	else
     	DATA <= W_CHECK;
	end if;
end process;

P_DATA_Q: process(CLK_SYN, RESET_N)
begin
	if (RESET_N = '0') then
		DATA_Q1 <= (others => '0') after 1 ns;
		DATA_Q2 <= (others => '0') after 1 ns;
		Y <= (others => '0') after 1 ns;
	elsif (CLK_SYN = '1' and CLK_SYN'event) then
		DATA_Q1 <= DATA after 1 ns; -- IOB
		DATA_Q2 <= DATA_Q1 after 1 ns;
		if (EN = '1') then
			Y <= DATA_Q2 after 1 ns;
		end if;
	end if;
end process;

-------------------------------------------------------------------------------
-- 2's Complement
-------------------------------------------------------------------------------
P_CMPLMNT: process(W, Y)
begin
	if (Y(15) = '1') then
		Y_CHECK <= not(Y) + 1;
		W_CHECK <= not(W) + 1;
	else
		Y_CHECK <= Y;
		W_CHECK <= W;
	end if;
end process;

-------------------------------------------------------------------------------
-- EQ_PE instantiation
-------------------------------------------------------------------------------
EQ_PE_C : EQ_PE
	port map (
		CLK_PE	=> CLK_PE,
		RESET_N	=> RESET_N,
		START	=> START,
		Y		=> Y_CHECK,
		RDY		=> RDY,
		W		=> W
	);


end FSMC_ARCH;
