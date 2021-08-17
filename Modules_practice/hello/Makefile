obj-m += hello.o
KSRC = /lib/modules/`uname -r`/build

all:
	make -C ${KSRC} M=${PWD} modules

clean:
	make -C ${KSRC} M=${PWD} clean

