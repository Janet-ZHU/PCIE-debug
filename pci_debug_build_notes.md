##This tool just for pcie device which map mem by barx.

#source code using readline.h, so we should install this libary.
sudo apt install libreadline6-dev 

#gcc -o pcie_debug pcie_debug.c -lreadline

./pcie_debug -s 00:00.0

d32 addr_offset len(by bytes)