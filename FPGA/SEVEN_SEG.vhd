-------------------------------------------------------------------------------
-- file: SEVEN_SEG.vhd
-- author: Rene Herthel <rene.herthel@haw-hamburg.de>
-- author: Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
-------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-------------------------------------------------------------------------------
-- entity of SEVEN_SEG.vhd
-------------------------------------------------------------------------------
entity SEVEN_SEG is
	port(
		CLK			: in std_logic;
		RESET_N		: in std_logic;
		CE			: in std_logic;
		DATA_IN		: in std_logic_vector(15 downto 0);
		ANODES		: out std_logic_vector(3 downto 0);
		CATHODES	: out std_logic_vector(7 downto 0)
	);
end SEVEN_SEG;


architecture SEVEN_SEG_ARCH of SEVEN_SEG is

-------------------------------------------------------------------------------
-- signal definitions
-------------------------------------------------------------------------------
signal STATE : bit_vector(1 downto 0);

begin
-------------------------------------------------------------------------------
-- FSM
-------------------------------------------------------------------------------
FSM: process(CLK, RESET_N)
variable SRG_IN	: std_logic_vector(3 downto 0);
begin
	if (RESET_N = '0') then
		STATE		<= "00" after 1 ns;
		ANODES		<= "1111" after 1 ns;
		CATHODES	<= "01111111" after 1 ns;
	elsif (CLK = '1' and CLK'event) then
		if (CE = '1') then
			case STATE is
				when "00"	=>	STATE <= "01" after 1 ns;
								ANODES <= "1110" after 1 ns;
								SRG_IN := DATA_IN(3 downto 0);
				when "01"	=>	STATE <= "10" after 1 ns;
								ANODES <= "1101" after 1 ns;
								SRG_IN := DATA_IN(7 downto 4);
				when "10"	=>	STATE <= "11" after 1 ns;
								ANODES <= "1011" after 1 ns;
								SRG_IN := DATA_IN(11 downto 8);
				when "11"	=>	STATE <= "00" after 1 ns;
								ANODES <= "0111" after 1 ns;
								SRG_IN := DATA_IN(15 downto 12);
				when others	=>	null;
			end case;
		end if;
		
		case SRG_IN is
			when "0000" => CATHODES <= "11000000";
			when "0001" => CATHODES <= "11111001";
			when "0010" => CATHODES <= "10100100";
			when "0011" => CATHODES <= "10110000";
			when "0100" => CATHODES <= "10011001";
			when "0101" => CATHODES <= "10010010";
			when "0110" => CATHODES <= "10000010";
			when "0111" => CATHODES <= "11111000";
			when "1000" => CATHODES <= "10000000";
			when "1001" => CATHODES <= "10010000";
			when "1010" => CATHODES <= "10001000"; -- A
			when "1011" => CATHODES <= "10000011"; -- B
			when "1100" => CATHODES <= "11000110"; -- C
			when "1101" => CATHODES <= "10100001"; -- D
			when "1110" => CATHODES <= "10000110"; -- E
			when "1111" => CATHODES <= "10001110"; -- F
			when others	=>	null;
		end case;
		
	end if;
end process;

end SEVEN_SEG_ARCH;
