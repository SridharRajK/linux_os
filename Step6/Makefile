obj-m += step6.o
KSRC=/lib/modules/`uname -r`/build/ 
all:
	make -C ${KSRC} M=${PWD} modules 
	
in:
	sudo insmod step6.ko
	sudo dmesg
	
rm:
	sudo rmmod step6.ko
	sudo dmesg
	
ds:
	sudo dmesg -c
	
clean:
	make -C ${KSRC} M=${PWD} clean 
	rm -rf *.o

read:
	gcc read.c -o read.o
	sudo ./read.o
	
write:
	gcc write.c -o write.o
	sudo ./write.o
