-------------------------------------------------------------------------------
-- file: FREQ_DIV.vhd
-- author: Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
-- author: Rene Herthel <rene.herthel@haw-hamburg.de>
-------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-------------------------------------------------------------------------------
-- entity of FREQ_DIV.vhd
-------------------------------------------------------------------------------
entity FREQ_DIV is
	generic(N : natural := 17); -- 17 Bit
	port(
		CLK 	: in std_logic;
		RESET_N	: in std_logic;
		PULSE 	: out std_logic
	);
end FREQ_DIV;


architecture FREQ_DIV_ARCH of FREQ_DIV is

-------------------------------------------------------------------------------
-- signal definitions
-------------------------------------------------------------------------------
signal COUNTER	: std_logic_vector(N-1 downto 0);
signal P1		: std_logic;
signal P2		: std_logic;


begin
	
-------------------------------------------------------------------------------
-- CNTR
-------------------------------------------------------------------------------
CNTR: process(CLK, RESET_N)
begin
	if (RESET_N = '0') then
		COUNTER <= (others => '0') after 1 ns;
	elsif (CLK = '1' and CLK'event) then
		COUNTER <= COUNTER + 1 after 1 ns;
	end if;
end process;

-------------------------------------------------------------------------------
-- PULSE_SHORTER
-------------------------------------------------------------------------------
PULSE_SHORTER: process(CLK, RESET_N)
begin
	if (RESET_N = '0') then
		P1 <= '0' after 1 ns;
		P2 <= '0' after 1 ns;
	elsif (CLK = '1' and CLK'event) then
		P1 <= COUNTER(N-1) after 1 ns;
		P2 <= P1 after 1 ns;
	end if;
end process;

PULSE <= P1 and not P2 after 1 ns;


end FREQ_DIV_ARCH;
