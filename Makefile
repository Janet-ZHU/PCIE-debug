pci_debug: pci_debug.o
	gcc -o pci_debug pci_debug.o

pci_debug.o: pci_debug.c #using "ldconfig -p | grep readline" to find the path of readline, /usr/...
	gcc -c pci_debug.c -I usr/lib/x86_64-linux-gnu/libreadline.so.7 -lreadline

clean:
	rm pci_debug pci_debug.o