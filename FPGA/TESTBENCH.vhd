-------------------------------------------------------------------------------
-- file: TESTBENCH.vhd
-- author: Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
-- author: Rene Herthel <rene.herthel@haw-hamburg.de>
-------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;


entity TESTBENCH is
	generic (
		HCLK		: time		:= 6 ns;	-- Clockzyklus des Microcontrollers
		
		HOLD		: positive	:= 1;		-- Haltezyklus beim Schreibzugriff
		PAUSE		: positive	:= 2;		-- Pausenzyklen mit deaktivierten Steuersignalen
		
		ADDSET_W	: positive	:= 1;
		DATAST_W	: positive	:= 2;
		
		ADDSET_R	: positive	:= 1;
		DATAST_R	: positive	:= 2
	);
end TESTBENCH;


architecture BEHAVIORAL_TESTBENCH of TESTBENCH is


component TOP_EQ is
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
		;LOCKED_O	: out std_logic
		
------------------------ SIM ------------------------
		;CLK_SYN	: in std_logic;
		CLK_PE		: in std_logic;
		CLK_ORIG	: in std_logic;
		LOCKED		: in std_logic
	);
end component;
for all : TOP_EQ use entity work.TOP_EQ(TOP_EQ_ARCH);
--for all : TOP_EQ use entity work.TOP_EQ(Structure);

-------------------------------------------------------------------------------
-- signal initialization
-------------------------------------------------------------------------------	
signal NWE			: std_logic := '1';
signal NE			: std_logic := '1';
signal NOE			: std_logic := '1';
signal RESET_N		: std_logic := '0';
signal DATA			: std_logic_vector(15 downto 0) := (others => 'Z');
signal RDY			: std_logic;

signal COUNTER		: std_logic_vector(15 downto 0) := "1000000000000000";

signal CLK_FPGA		: std_logic := '0';
signal FPGA_IN		: std_logic_vector(15 downto 0);
signal FPGA_OUT		: std_logic_vector(15 downto 0);
signal FPGA_IN_EN	: std_logic := '0';
signal FPGA_OUT_EN	: std_logic := '0';

signal CLK			: std_logic := '0';
signal EN_N			: std_logic := '0';
signal ANODES		: std_logic_vector(3 downto 0) := (others => '0');
signal CATHODES		: std_logic_vector(7 downto 0) := (others => '0');

------------------- OSCILLOPCOPE --------------------
signal NWE_OUT		: std_logic;
signal NE_OUT		: std_logic;
signal NOE_OUT		: std_logic;
signal RDY_OUT		: std_logic;
-----------------------------------------------------

------------------------ SIM ------------------------
signal CLK_PE			: std_logic := '0';
signal CLK_SYN			: std_logic := '0';
signal CLK_ORIG			: std_logic := '0';
signal LOCKED			: std_logic := '0';
-----------------------------------------------------

---------------------- TIMSIM -----------------------
signal LOCKED_O		: std_logic;
-----------------------------------------------------

begin

-------------------------------------------------------------------------------
-- signals stimuli
-------------------------------------------------------------------------------
READ_WRITE_P: process
begin

	wait until LOCKED_O'event and LOCKED_O = '1';
	
	WHILE (LOCKED_O = '1') LOOP
		
		--WRITE
		NE <= '0';
		wait for (HCLK * ADDSET_W);
		
		NWE <= '0';
		DATA <= COUNTER;
		wait for (HCLK * DATAST_W);
		
		FPGA_IN_EN <= '1';
		NWE <= '1';
		wait for (HCLK * HOLD);
		
		FPGA_IN_EN <= '0';
		
		--PAUSE
		DATA <= (others => 'Z');
		NE <= '1';
		wait for (HCLK * PAUSE);
		
		--WAIT
		wait until RDY = '1';
		
		--READ
		NE <= '0';
		wait for (HCLK * ADDSET_R);
		
		NOE <= '0';
		wait for (HCLK * DATAST_R);
		
		--PAUSE
		FPGA_OUT_EN <= '1';
		NOE <= '1';
		NE <= '1';
		wait for (HCLK * PAUSE/2);
		FPGA_OUT_EN <= '0';
		wait for (HCLK * PAUSE/2);
		
		COUNTER <= COUNTER + 1;
		
	END LOOP;
end process;

CLK_P: process
begin
	CLK <= '0';
	wait for 20 ns;
	CLK <= '1';
	wait for 20 ns;
end process;

RESET_N_P: process
begin
	wait for 20 ns;
	RESET_N <= '1';
end process;

CLK_FPGA_P: process
begin
	CLK_FPGA <= '0';
	wait for 3 ns;
	CLK_FPGA <= '1';
	wait for 3 ns;
end process;

FPGA_IN_P: process(CLK_FPGA, RESET_N)
begin
	if (RESET_N = '0') then
		FPGA_IN <= (others => '0');
	elsif (CLK_FPGA = '1' and CLK_FPGA'event) then
		if (FPGA_IN_EN = '1') then
			FPGA_IN <= DATA;
		end if;
	end if;
end process;

FPGA_OUT_P: process(CLK_FPGA, RESET_N)
begin
	if (RESET_N = '0') then
		FPGA_OUT <= (others => '0');
	elsif (CLK_FPGA = '1' and CLK_FPGA'event) then
		if (FPGA_OUT_EN = '1') then
			FPGA_OUT <= DATA;
		end if;
	end if;
end process;

------------------------ SIM ------------------------
CLK_SYN_P: process
begin
	CLK_SYN <= '0';
	wait for 1500 ps;
	CLK_SYN <= '1';
	wait for 1500 ps;
end process;

CLK_PE_P: process
begin
	CLK_PE <= '0';
	wait for 3 ns;
	CLK_PE <= '1';
	wait for 3 ns;
end process;

CLK_ORIG_P: process
begin
	CLK_ORIG <= '0';
	wait for 20 ns;
	CLK_ORIG <= '1';
	wait for 20 ns;
end process;

LOCKED_P: process
begin
	wait for 100 ns;
	LOCKED <= '1';
end process;
-----------------------------------------------------

DUT : TOP_EQ
port map (
	CLK			=> CLK,
	ANODES		=> ANODES,
	CATHODES	=> CATHODES,
	NWE			=> NWE,
	NE			=> NE,
	NOE			=> NOE,
	DATA		=> DATA,
	RDY			=> RDY,
	
------------------- OSCILLOPCOPE --------------------
	NWE_OUT		=> NWE_OUT,
	NE_OUT		=> NE_OUT,
	NOE_OUT		=> NOE_OUT,
	RDY_OUT		=> RDY_OUT
	
---------------------- TIMSIM -----------------------
	,LOCKED_O	=> LOCKED_O
	
------------------------ SIM ------------------------
	
	,CLK_SYN	=> CLK_SYN,
	CLK_PE		=> CLK_PE,
	CLK_ORIG	=> CLK_ORIG,
	LOCKED		=> LOCKED
);
	
end BEHAVIORAL_TESTBENCH;
