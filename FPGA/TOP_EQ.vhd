-------------------------------------------------------------------------------
-- file: TOP_EQ.vhd
-- author: Rene Herthel <rene.herthel@haw-hamburg.de>
-- author: Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
-------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


-------------------------------------------------------------------------------
-- entity of TOP_EQ.vhd
-------------------------------------------------------------------------------
entity TOP_EQ is
	port(
		CLK			: in std_logic;
		ANODES		: out std_logic_vector(3 downto 0);
		CATHODES	: out std_logic_vector(7 downto 0);
		NWE			: in std_logic;
		NE			: in std_logic;
		NOE			: in std_logic;
		DATA		: inout std_logic_vector(15 downto 0);
		RDY			: out std_logic;
		
------------------- OSCILLOPCOPE --------------------
		NWE_OUT		: out std_logic;
		NE_OUT		: out std_logic;
		NOE_OUT		: out std_logic;
		RDY_OUT		: out std_logic
		
---------------------- TIMSIM -----------------------
		-- ;LOCKED_O	: out std_logic
		
------------------------ SIM ------------------------
		-- ;CLK_SYN	: in std_logic;
		-- CLK_PE		: in std_logic;
		-- CLK_ORIG	: in std_logic;
		-- LOCKED		: in std_logic
	);
end TOP_EQ;

architecture TOP_EQ_ARCH of TOP_EQ is


-------------------------------------------------------------------------------
-- component definition
-------------------------------------------------------------------------------
component FREQ_DIV is
	port(
		CLK 		: in std_logic;
		RESET_N		: in std_logic;
		PULSE 		: out std_logic
	);
end component;

component SEVEN_SEG is
	port(
		CLK			: in std_logic;
		RESET_N		: in std_logic;
		CE			: in std_logic;
		DATA_IN		: in std_logic_vector(15 downto 0);
		ANODES		: out std_logic_vector(3 downto 0);
		CATHODES	: out std_logic_vector(7 downto 0)
	);
end component;

component FSMC is
	port (
		CLK_SYN		: in std_logic;
		CLK_PE		: in std_logic;
		NWE			: in std_logic;
		NE			: in std_logic;
		NOE			: in std_logic;
		RESET_N		: in std_logic;
		DATA		: inout std_logic_vector(15 downto 0);
		RDY			: out std_logic
	);
end component;

component clk_wiz_v3_6
	port (
		CLK_IN1		: in std_logic;
		CLK_SYN		: out std_logic;
		CLK_PE		: out std_logic;
		CLK_ORIG	: out std_logic;
		LOCKED		: out std_logic
	);
end component;


-------------------------------------------------------------------------------
-- signal definitions
-------------------------------------------------------------------------------
signal PULSE			: std_logic;
signal SEVEN_SEG_DATA	: std_logic_vector(15 downto 0);

--------------------- PROGRAM -----------------------
signal LOCKED			: std_logic;
signal CLK_SYN			: std_logic;
signal CLK_PE			: std_logic;
signal CLK_ORIG			: std_logic;
-----------------------------------------------------

------------------- OSCILLOPCOPE --------------------
signal RDY_INTERNAL		: std_logic;
-----------------------------------------------------

begin

SEVEN_SEG_DATA <= DATA;

------------------- OSCILLOPCOPE --------------------
NWE_OUT	<= NWE;
NE_OUT	<= NE;
NOE_OUT	<= NOE;
RDY_OUT	<= RDY_INTERNAL;
RDY		<= RDY_INTERNAL;
-----------------------------------------------------

---------------------- TIMSIM -----------------------
-- LOCKED_O <= LOCKED;
-----------------------------------------------------

-------------------------------------------------------------------------------
-- instantiations
-------------------------------------------------------------------------------
FREQ_DIV_C : FREQ_DIV
	port map (
		CLK			=> CLK_ORIG,
		RESET_N		=> LOCKED,
		PULSE	 	=> PULSE
	);

SEVEN_SEG_C : SEVEN_SEG
	port map (
		CLK		 	=> CLK_ORIG,
		RESET_N		=> LOCKED,
		CE		 	=> PULSE,
		DATA_IN 	=> SEVEN_SEG_DATA,
		ANODES	 	=> ANODES,
		CATHODES 	=> CATHODES
	);

FSMC_C : FSMC
	port map (
		CLK_SYN		=> CLK_SYN,
		CLK_PE		=> CLK_PE,
		NWE			=> NWE,
		NE			=> NE,
		NOE			=> NOE,
		RESET_N		=> LOCKED,
		DATA		=> DATA,
		RDY			=> RDY_INTERNAL
	);

COREGEN : clk_wiz_v3_6
	port map (
		CLK_IN1		=> CLK,
		CLK_SYN		=> CLK_SYN,
		CLK_PE		=> CLK_PE,
		CLK_ORIG	=> CLK_ORIG,
		LOCKED		=> LOCKED
	);

end TOP_EQ_ARCH;
