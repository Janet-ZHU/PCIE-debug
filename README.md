# PCIE-debug
## debug  tool for pci device or pcie device
This tool has been verified in ubuntun OS.
PCIE bar addr specify the physical addr, but when we want to get the content of the memory mapped to PCI device, we must use mmap to tranfer the phy addr to kernel addr(virtual addr). 
I also using the devmem2, but can not get the data, I don't understand why. 
