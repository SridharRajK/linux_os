obj-m += step5.o
KSRC=/lib/modules/`uname -r`/build/ 
all:
	make -C ${KSRC} M=${PWD} modules 
	
clean:
	make -C ${KSRC} M=${PWD} clean 

mount:
	sudo mount -o loop,rw,sync rootfs.img /mnt/rootfs/
	
unmount:
	sudo umount /mnt/rootfs

copy:
	sudo cp step5.ko /mnt/rootfs/home/root/
