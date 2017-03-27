restart
view signals
view wave
radix decimal
config wave -signalnamewidth 1

delete wave *

# TESTBENCH
add wave -noupdate -divider "TESTBENCH"
add wave -height 30 -label COUNTER sim:/TESTBENCH/COUNTER
add wave -height 30 -label FPGA_IN sim:/TESTBENCH/FPGA_IN
add wave -height 30 -label FPGA_OUT sim:/TESTBENCH/FPGA_OUT

# TOP_EQ
add wave -noupdate -divider "TOP_EQ"
add wave -height 30 -label DATA sim:/TESTBENCH/DUT/DATA

# FSMC
add wave -noupdate -divider "FSMC"
add wave -height 30 -label NE sim:/TESTBENCH/DUT/FSMC_C/NE
add wave -height 30 -label NOE sim:/TESTBENCH/DUT/FSMC_C/NOE
add wave -height 30 -label NWE sim:/TESTBENCH/DUT/FSMC_C/NWE
add wave -height 30 -label RDY sim:/TESTBENCH/DUT/FSMC_C/RDY

# EQ_PE
add wave -height 40 -noupdate -divider "EQ_PE"
add wave -height 30 sim:/testbench/DUT/FSMC_C/EQ_PE_C/START
add wave -height 30 sim:/testbench/DUT/FSMC_C/EQ_PE_C/CURRENT_Z
add wave -height 30 sim:/testbench/DUT/FSMC_C/W_CHECK

run 500000ns

wave zoomfull
