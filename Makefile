mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir  := $(dir $(mkfile_path))

VENDOR := HI3516DV300

ifeq ($(VENDOR), UBUNTU)
CCOMPILE = gcc
CPPCOMPILE = g++
AR = ar
else ifeq ($(VENDOR), HI3516DV300)
CCOMPILE = arm-himix200-linux-gcc
CPPCOMPILE = arm-himix200-linux-g++
AR = arm-himix200-linux-ar
endif

export CCOMPILE CPPCOMPILE AR

TARGET = image_coversion
OBJS += rgb2yuv420.o

CFLAGS = -Wall -D__LINUX__ -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -mno-unaligned-access -fno-aggressive-loop-optimizations
# 在链接生成最终可执行文件时，如果带有-Wl,--gc-sections参数，并且之前编译目标文件时带有-ffunction-sections、-fdata-sections参数，则链接器ld不会链接未使用的函数，从而减小可执行文件大小；
CFLAGS += -Wl,-gc-sections  

COMPILEOPTION = -c -O3
#	armcc -c --cpu=7 --debug neon.c -o neon.o
#	armlink --entry=EnableNEON neon.o -o neon.axf
#	arm-linux-gnueabi-gcc -flax-vector-conversions -mfloat-abi=softfp -mfpu=neon -o neon neon.c
#	arm-linux-gnueabi-gcc -v -I/usr/lib/gcc/arm-linux-androideabi/4.7.4/include-fixed/ -marm -march=armv7 -mfloat-abi=softfp -mfpu=neon -o neon neon.c
#	arm-linux-gnueabi-gcc -v -I/usr/local/ARMCompiler6.7/include -marm -march=armv7 -mfloat-abi=softfp -mfpu=neon -o neon neon.c
#	arm-linux-gnueabi-gcc -v -I/usr/local/DS-5_v5.27.0/sw/gcc/arm-linux-gnueabihf/ -lrt -marm -march=armv7 -mfloat-abi=softfp -mfpu=neon -o neon neon.c
#	arm-linux-gnueabi-gcc -v -lrt -mfloat-abi=softfp -mfpu=neon -o neon neon.c -static
#	arm-none-eabi-gcc -I/usr/local/DS-5_v5.27.0/sw/gcc/arm-linux-gnueabihf/ -lrt -marm -mfloat-abi=softfp -mfpu=neon -o neon neon.c
#	arm-none-eabi-gcc -v -marm -march=armv7 -mfloat-abi=softfp -mfpu=neon -o neon neon.c
#	arm-linux-gnueabihf-gcc -v -lrt -march=armv7 -mfpu=neon -o neon neon.c -static
#	/usr/local/DS-5_v5.27.0/sw/gcc/bin/arm-linux-gnueabihf-gcc -v -lrt -march=armv7 -mfpu=neon -o neon neon.c -static

#	armcc -O3 -Otime --vectorize --cpu=Cortex-A8 -o neon-func.o -c neon-func.c
#	arm-linux-gnueabi-gcc -O3 -mfloat-abi=softfp -mfpu=neon -o neon-func-gcc.o -c neon-func.c
#	arm-linux-androideabi-gcc -v -lrt -mfloat-abi=softfp -mfpu=neon -o neon neon.c -static
#	$(CC) -v -mfloat-abi=softfp -mfpu=neon -o neon neon.c -static


$(TARGET): $(OBJS)
	rm -rf $(TARGET)
	$(CCOMPILE) -o $@ $(OBJS)
	cp $(TARGET) /share/nfs

clean: 
	rm -f $(OBJS)
	rm -f $(TARGET)

all: clean $(TARGET)
.PRECIOUS:%.cpp %.cc %.cxx %.c %.m %.mm
.SUFFIXES:
.SUFFIXES: .cpp .cc .cxx .c .m .mm .o

.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.cc.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cc

.cxx.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cxx

.c.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(CFLAGS) $*.c

.m.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.m

.mm.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.mm
