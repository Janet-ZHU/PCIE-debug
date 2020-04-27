#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
 
#define PCI_CONFIG_ADDR(bus, dev, fn, reg) (0x80000000 | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3))

void usage()
{
    printf("Usage: readpci [-bdfrth]\n\
            -a   addr  :   specify bar address default 0\n\
            -b   bus   :   specify   PCI   bus   number(default   0)\n\
            -d   dev   :   device   number(default   0)\n\
            -f   fn    :   function   number(default   0)\n\
            -r   reg   :   register   address(must   be   multiple   of   4,   default   0)\n\
            -p   port  :   specify port number default 0\n\
            -v   value :   write a integer value into the address\n\
            -h         :   print   this   help   text\n ");
    exit(-1);
}

 

int operaMem(int bar, int offset, int modValue, int isWrite)
{
	int i;
	int fd;
	char* mem;
	
	//open /dev/mem with read and write mode
	if((fd = open("/dev/mem", O_RDWR)) < 0)
	{
		perror ("open error\n");
		return -1;
	}

	//map physical memory 0-10 bytes
	mem = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bar);
	if (mem == MAP_FAILED)
	{
		perror ("mmap error:\n");
		return 1;
	}
	int value = *((int *)(mem + offset));
    printf("The value at 0x%x is 0x%x\n", bar + offset, value);
    if(isWrite == 1)
    {
        printf("Write value 0x%x at 0x%x\n", modValue, bar + offset);
        memcpy(mem + offset, &modValue, 4);
        printf("Reread the value at 0x%x is 0x%x\n", bar + offset, *((int *)(mem + offset)));
    }
	munmap (mem, 4096); //destroy map memory
	close (fd);         //close file
	return 0;
}

int parseInt(char *str)
{
    int value = 0;
    char tmpChar;
    while((tmpChar = *str) != '\0')
    {
        if(tmpChar >= 'A' && tmpChar <= 'F')
        {
            tmpChar = tmpChar - 'A' + 10;
        }
        else if(tmpChar >= 'a' && tmpChar <= 'f')
        {
            tmpChar = tmpChar - 'a' + 10;
        }
        else
        {
            tmpChar -= '0';
        }
        value = value * 16 + tmpChar;
        str += 1;
    }

    return value;

}

 

int main(int argc, char **argv)
{
    unsigned long val = 0;
    char options[] = "a:b:d:f:r:v:p:h";
    int addr = 0, bus = 0, dev = 0, fn = 0, reg = 0, port = 0;
    int opt, value = 0, isWrite = 0, isReadBar = 0;
    while((opt = getopt(argc, argv, options)) != -1)
    {
        switch(opt)
        {
            case 'a':
                addr = parseInt(optarg);
                break;
            case 'b':
                bus = atoi(optarg);
                break;
            case 'd':
                dev = atoi(optarg);
                break;
            case 'f':
                fn = atoi(optarg);
                break;
            case 'r':
                reg = parseInt(optarg);
                break;

            case 'p':
                port = atoi(optarg);
                isReadBar = 1;
                break;
            case 'v':
                value = parseInt(optarg);
                isWrite = 1;
                break;
            case 'h':
            default:
                usage();
                break;
        }
    }
    iopl(3);
    if(isWrite == 0)
    {
        if(isReadBar == 0)
        {
            int i;
            for(i = 0; i < 256; i += 4)
            {
                outl(PCI_CONFIG_ADDR(bus, dev, fn, i), 0xCF8);
                val = inl(0xCFC);
                printf("PCI:Bus %d, DEV %d, FUNC %d, REG %x, Value is %x\n ", bus, dev, fn, i, val);
            }
        }
        else
        {
            outl(PCI_CONFIG_ADDR(bus, dev, fn, 0x10), 0xCF8);
            val = inl(0xCFC) & 0xfffffff0;
            printf("The base address value is 0x%x\n", val);
            int pointAddr = val + port * 0x1000;
            printf("The offset address value is 0x%x\n", pointAddr + addr);
            operaMem(pointAddr, addr, 0, 0);
        }
    }
    else
    {
        outl(PCI_CONFIG_ADDR(bus, dev, fn, 0x10), 0xCF8);
        val = inl(0xCFC) & 0xfffffff0;
        printf("The base address value is 0x%x\n", val);
        int pointAddr = val + port * 0x1000;
        printf("The offset address value is 0x%x\n", pointAddr + addr);
        operaMem(pointAddr, addr, value, 1);
    }
	
    return 0;

}

