obj-m += wrsample.o
KSRC = /home/sumanth/EmbdLinux/KSRC/linux-4.14.238

all:
	make -C ${KSRC} M=${PWD} modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

clean:
	make -C ${KSRC} M=${PWD} clean ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

mount:
	sudo mount -o loop,rw,sync rootfs.img /mnt/rootfs/
	
unmount:
	sudo umount /mnt/rootfs

copy:
	sudo cp step4.ko /mnt/rootfs/home/root/
