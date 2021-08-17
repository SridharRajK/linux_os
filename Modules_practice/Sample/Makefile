obj-m += sample.o
KSRC = /home/sumanth/EmbdLinux/KSRC/linux-4.14.238

all:
	make -C ${KSRC} M=${PWD} modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

clean:
	make -C ${KSRC} M=${PWD} clean ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

