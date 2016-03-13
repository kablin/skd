
APP = simplesqlite3

CROSS=/home/user/ARMLinux/buildroot/build_arm/staging_dir/bin/arm-linux-
ARCH=arm


LIBS = /home/user/ARMLinux/libnpe/lib/libhwnpe.so.0.0.1 /home/user/ARMLinux/buildroot/build_arm/staging_dir/usr/lib/libpq.a  /home/user/ARMLinux/buildroot/build_arm/staging_dir/usr/lib/libpq.so.5.2
INCLUDE = .  /home/user/ARMLinux/libnpe/lib/libhwnpe

CC = $(CROSS)gcc
#CC = $(CROSS)-g
CCP = $(CROSS)g++

CFLAGS = -Wall -Os -pthread -fomit-frame-pointer -DLINUX -D_GNU_SOURCE 
#LDFLAGS = -lpthread -s -Wl,-warn-common -lc -lssl -lcrypto
LDFLAGS = -lpthread -lrt -s -Wl,-warn-common -lc -lpq 
#LDFLAGS = -s -Wl,-warn-common --static -lc

SRC := $(APP).c crc16ccit.c comroutines.c comroutines.h LocalDB.c MainBaseroutines.c com.c
OBJ := $(APP).o crc16ccit.o comroutines.o LocalDB.o MainBaseroutines.o com.o

.c.o:
	@echo Compile... $<
#	@$(CC) $(CFLAGS) -I. -I/home/user/ARMLinux/buildroot/build_arm/staging_dir/usr/include/   -c -o $@ $<
	@$(CC) $(CFLAGS) -I. -I/home/user/ARMLinux/buildroot/build_arm/staging_dir/usr/include/  -I/home/user/ARMLinux/libnpe/lib/libhwnpe  -c -o $@ $<	

all: $(APP)

$(APP): $(OBJ)
	@echo Link... $@
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo Strip... $@
	@$(CROSS)strip $@
#	@$(CROSS)readelf -d $@
	@$(CROSS)size $@
	@echo Copy to nfs...
	@cp $@ /export/nfs/
	@echo Done.
test: $(APP)_test

$(APP)_test: $(OBJ)
	@echo Link... $@
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo Strip... $@
	@$(CROSS)strip $@
	@echo Copy to nfs...
	@cp $@ /export/nfs/
	@$(CROSS)size $@
	@echo Done.
clean:
	@rm -f *.o *.gdb $(APP)

install:
	@echo Copy to initrd...
	@cp $(APP) ../../buildroot/target/generic/target_skeleton/bin/
