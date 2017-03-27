-- ----------------------------------------------------------------------------
-- - file: EQ_PE.vhd
-- - author: Rene Herthel <rene.herthel@haw-hamburg.de>
-- - author: Hauke Sondermann <hauke.sondermann@haw-hamburg.de>
-- ----------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;


-- ----------------------------------------------------------------------------
-- - entity
-- ----------------------------------------------------------------------------
entity EQ_PE is
	port(
		CLK_PE		: in std_logic;
		RESET_N		: in std_logic;
		START		: in std_logic;
		Y			: in std_logic_vector(15 downto 0);
		RDY			: out std_logic;
		W			: out std_logic_vector(15 downto 0)
	);
end EQ_PE;


architecture EQ_PE_ARCH of EQ_PE is


-- ----------------------------------------------------------------------------
-- - states
-- ----------------------------------------------------------------------------
type STATE_TYPE is (IDLE, C1, C2, C3, C4, CW1, C5);
signal CURRENT_Z, FUTURE_Z : STATE_TYPE;


-- ----------------------------------------------------------------------------
-- - control
-- ----------------------------------------------------------------------------
signal SO1, SO2, SO3, SR1, SR2	: std_logic;
signal EN_W, EN_N, EN_QT, EN_R	: std_logic;
signal EN_T1, EN_T2, EN_T3		: std_logic;


-- ----------------------------------------------------------------------------
-- - data
-- ----------------------------------------------------------------------------
signal R			: std_logic_vector(15 downto 0);
signal QT			: std_logic_vector(15 downto 0);
signal T1			: std_logic_vector(15 downto 0);
signal T2			: std_logic_vector(15 downto 0);
signal T3			: std_logic_vector(15 downto 0);
signal N			: integer;

signal DECREMENTER	: integer;
signal ADDER		: std_logic_vector(15 downto 0);
signal SUBTRACTOR	: std_logic_vector(15 downto 0);
signal LEFT_SHIFTER	: std_logic_vector(15 downto 0);

signal COMP1		: std_logic;
signal COMP2		: std_logic;

signal MUX_R1		: std_logic_vector(15 downto 0);
signal MUX_R2		: std_logic_vector(15 downto 0);

signal INIT			: std_logic;


begin


-- ----------------------------------------------------------------------------
-- - FSM_ZR
-- ----------------------------------------------------------------------------
FSM_ZR: process(CLK_PE, RESET_N)
begin
	if (RESET_N = '0') then
		CURRENT_Z <= IDLE after 1 ns;
	elsif (CLK_PE = '1' and CLK_PE'event) then
		CURRENT_Z <= FUTURE_Z after 1 ns;
	end if;
end process;


-- ----------------------------------------------------------------------------
-- - FSM_ASN
-- ----------------------------------------------------------------------------
FSM_ASN: process(CURRENT_Z, START, COMP1, COMP2)
begin
	FUTURE_Z	<= IDLE	after 1 ns;
	RDY			<= '0'	after 1 ns;
	EN_W		<= '0'	after 1 ns;
	EN_N		<= '0'	after 1 ns;
	EN_QT		<= '0'	after 1 ns;
	EN_R		<= '0'	after 1 ns;
	EN_T1		<= '0'	after 1 ns;
	EN_T2		<= '0'	after 1 ns;
	EN_T3		<= '0'	after 1 ns;
	SO1			<= '0'	after 1 ns;
	SO2			<= '0'	after 1 ns;
	SO3			<= '0'	after 1 ns;
	SR1			<= '0'	after 1 ns;
	SR2			<= '0'	after 1 ns;
	INIT		<= '0'	after 1 ns;
	case CURRENT_Z is -----------------------------------------------------
		when IDLE		=>	RDY <= '1' after 1 ns;
							if (START = '1') then 
								FUTURE_Z <= C1 after 1 ns;
								INIT <= '1' after 1 ns;
								EN_R <= '1' after 1 ns;
							end if;		
							-----------------------------------------------
		when C1			=>	EN_N	<= '1' 						after 1 ns;
							FUTURE_Z <= C2 						after 1 ns;
							-----------------------------------------------
		when C2			=>	EN_T1	<= '1' 						after 1 ns;
							FUTURE_Z <= C3						after 1 ns;
							-----------------------------------------------
		when C3			=>	EN_T2	<= '1' 						after 1 ns;
							SO1		<= '1' 						after 1 ns;
							SO2		<= '1' 						after 1 ns;
							FUTURE_Z <= C4 						after 1 ns;
							-----------------------------------------------
		when C4			=>	EN_T3	<= '1' 						after 1 ns;
							SO3		<= '1' 						after 1 ns;
							FUTURE_Z <= CW1 					after 1 ns;
							-----------------------------------------------
		when C5			=>	EN_R	<= '1' 						after 1 ns;
							SO2		<= '1' 						after 1 ns;
							SR1		<= '1' 						after 1 ns;
							if (COMP1 = '1') then
								FUTURE_Z <= C1 after 1 ns;
								EN_QT <= '1' after 1 ns;
							else
								FUTURE_Z <= IDLE after 1 ns;
								EN_W <= '1' after 1 ns;
							end if;
							-----------------------------------------------
		when CW1		=>	if (COMP2 = '1') then
								FUTURE_Z <= C5 after 1 ns;
							else
								if (COMP1 = '1') then
									FUTURE_Z <= C1 after 1 ns;
								else
									FUTURE_Z <= IDLE after 1 ns;
									EN_W <= '1' after 1 ns;
									SR2 <= '1' after 1 ns;
								end if;
							end if;
							-----------------------------------------------
		when others		=>	null;
	end case; -- CURRENT_Z
end process; -- FSM_ASN


-- ----------------------------------------------------------------------------
-- - SQRT_OPERATORS
-- ----------------------------------------------------------------------------
SQRT_OPERATORS: process(QT, R, T1, T2, T3, SR1, SR2, SO1, SO2, SO3, N, ADDER, SUBTRACTOR, Y, DECREMENTER)
begin

	DECREMENTER <= N - 1 after 1 ns;
	LEFT_SHIFTER <= (others => '0') after 1 ns;
	
	
	if (SO1 = '0' and SO2 = '0') then
		ADDER <= QT + QT after 1 ns;
	elsif (SO1 = '0' and SO2 = '1') then
		ADDER <= QT + T1 after 1 ns;
	elsif (SO1 = '1' and SO2 = '1') then
		ADDER <= (QT(14 downto 0) & '0') + T1 after 1 ns;
	else
		ADDER <= (QT(14 downto 0) & '0') + QT after 1 ns;
	end if;
	
	
	SUBTRACTOR <= R - T3 after 1 ns;
	
	
	if (SO3 = '0') then
		LEFT_SHIFTER(N) <= '1' after 1 ns;
	else
		LEFT_SHIFTER <= std_logic_vector(SHIFT_LEFT(unsigned(T2), N)) 	after 1 ns;
	end if;


	if 		(N > 0) 	then	COMP1 <= '1' 						after 1 ns;
	else				 		COMP1 <= '0' 						after 1 ns; 
																	end if;
	
	if 		(R >= T3) 	then	COMP2 <= '1' 						after 1 ns;
	else						COMP2 <= '0' 						after 1 ns; 
																	end if;
	
	if 		(SR1 = '0')	then	MUX_R1 <= Y 						after 1 ns;
	else						MUX_R1 <= SUBTRACTOR 				after 1 ns; 
																	end if;
																	
	if 		(SR2 = '0')	then	MUX_R2 <= ADDER 					after 1 ns;
	else						MUX_R2 <= QT		 				after 1 ns; 
																	end if;
end process; --SQRT_OPERATORS


-- ----------------------------------------------------------------------------
-- SQRT_REGISTERS
-- ----------------------------------------------------------------------------
SQRT_REGISTERS: process(CLK_PE, RESET_N, INIT)
begin
	if (RESET_N = '0' or INIT = '1') then
		QT	<= (others => '0') 									after 1 ns;
		T1	<= (others => '0') 									after 1 ns;
		T2	<= (others => '0') 									after 1 ns;
		T3	<= (others => '0') 									after 1 ns;
		N	<= 8												after 1 ns;
	elsif (CLK_PE = '1' and CLK_PE'event) then --------------------------------	
		
		if 	(EN_N = '1') 		then 	N <= DECREMENTER 		after 1 ns; 
		end if;

		if	(EN_QT = '1')		then	QT <= ADDER				after 1 ns; 
		end if;

		if 		(EN_T1 = '1') 	then	T1 <= LEFT_SHIFTER 		after 1 ns; 
		end if;
		
		if 		(EN_T2 = '1') 	then	T2 <= ADDER 			after 1 ns; 
		end if;

		if 		(EN_T3 = '1') 	then	T3 <= LEFT_SHIFTER 		after 1 ns; 
		end if;
	end if; --RESET_N and CLK_PE
end process; -- SQRT_REGISTERS

SQRT_W: process(CLK_PE, RESET_N)
begin
	if (RESET_N = '0') then
		W <= (others => '0') after 1 ns;
		R <= (others => '0') after 1 ns;
	elsif (CLK_PE = '1' and CLK_PE'event) then
		if (EN_W = '1') then
			W <= MUX_R2 after 1 ns;
		end if;
		if (EN_R = '1') then
			R <= MUX_R1 after 1 ns; 
		end if;
	end if;
end process;


end EQ_PE_ARCH;
