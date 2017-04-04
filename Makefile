.PHONY: all run rund runqemu runqemud load partition unload mount umount lm ulm grubinst mvtoimg builddisk updatedsk clean mrproper dasm count

AS = as
CC = gcc
CXX = g++

SRCDIR = src
SRC32DIR = src32
OUTDIR = out
OUT32DIR = out/loader
DEPDIR = .dep
DEP32DIR = .dep/loader
LIBC = libc
LIBCXX = libc++

LIB32GCC = /usr/lib/gcc/x86_64-*linux-gnu/6.*/32
LIBGCC = /usr/lib/gcc/x86_64-*linux-gnu/6.*

OPTILVL = -O2 -mno-sse

ASFLAGS =
AS32FLAGS = --32
CBASEFLAGS = -nostdlib -ffreestanding -fno-stack-protector -Wall -Wextra \
				 -Wno-packed-bitfield-compat -fno-builtin -mno-red-zone \
				 $(OPTILVL)
C32FLAGS = $(CBASEFLAGS) -m32 -DSUP_OS_LOADER
CFLAGS = $(CBASEFLAGS) -isystem $(LIBC) \
				   -isystem $(LIBCXX)\
				   -DSUP_OS_KERNEL \
					 -mcmodel=kernel # 64 bit high-half kernel

CXXBASEFLAGS = -fno-exceptions -fno-rtti -std=c++14
CXX32FLAGS = $(C32FLAGS) $(CXXBASEFLAGS)
CXXFLAGS = $(CFLAGS) $(CXXBASEFLAGS)

LDFLAGS =  -nostdlib -Wl,--build-id=none
LD32FLAGS = $(LDFLAGS) -m32 -T $(SRC32DIR)/link.ld -Wl,-melf_i386
LIBS32 = -L $(LIB32GCC) -lgcc
LD64FLAGS = $(LDFLAGS) -T $(SRCDIR)/link.ld -Wl,-melf_x86_64
LIBS64 = -L. -L $(LIBGCC) -lc -lgcc -lc++

SRC32ASM = $(wildcard $(SRC32DIR)/*.s)
SRC32CXX = $(wildcard $(SRC32DIR)/*.cpp)

SRCCONTENT = $(shell find $(SRCDIR) -type f)

#SRCASM = $(SRCDIR)/Interrupts/Interrupt.s
#SRCCXX = $(SRCDIR)/kmain.cpp $(SRCDIR)/IO/FrameBuffer.cpp $(SRCDIR)/utility.cpp \
				 $(SRCDIR)/Interrupts/Interrupt.cpp $(SRCDIR)/Interrupts/Pic.cpp \
				 $(SRCDIR)/Memory/PhysicalMemoryAllocator.cpp \
				 $(SRCDIR)/Memory/Paging.cpp \
				 $(SRCDIR)/Memory/Heap.cpp

SRCASM = $(filter %.s,$(SRCCONTENT))
SRCC = $(filter %.c,$(SRCCONTENT))
SRCCXX = $(filter %.cpp,$(SRCCONTENT))

DEPF = $(shell find $(DEPDIR) -type f)

OBJ32 = $(patsubst $(SRC32DIR)/%, $(OUT32DIR)/%.o, $(SRC32ASM)) \
      $(patsubst $(SRC32DIR)/%, $(OUT32DIR)/%.o, $(SRC32CXX)) \

OBJ = $(patsubst $(SRCDIR)/%, $(OUTDIR)/%.o, $(SRCASM)) \
      $(patsubst $(SRCDIR)/%, $(OUTDIR)/%.o, $(SRCC))   \
      $(patsubst $(SRCDIR)/%, $(OUTDIR)/%.o, $(SRCCXX)) \
      $(OUTDIR)/InterruptInt.o


LOOPDEV = /dev/loop0
MNTPATH = /mnt/test# must be absolute and not relative

LIBCSRC = $(wildcard $(LIBC)/src/*.c)
LIBCSRCXX = $(wildcard $(LIBC)/src/*.cpp)
LIBCOBJ = $(patsubst $(LIBC)/src/%.c,$(OUTDIR)/$(LIBC)/%.o,$(LIBCSRC)) \
	$(patsubst $(LIBC)/src/%.cpp,$(OUTDIR)/$(LIBC)/%.o,$(LIBCSRCXX))
LIBCH = $(wildcard $(LIBC)/*.h)

LIBCXXSRC = $(wildcard $(LIBCXX)/src/*.cpp)
LIBCXXOBJ = $(patsubst $(LIBCXX)/src/%.cpp,$(OUTDIR)/$(LIBCXX)/%.o,$(LIBCXXSRC))
LIBCXXH = $(wildcard $(LIBCXX)/*) $(wildcard $(LIBCXX)/include/*.h)

FSTYPE = ext2
MKFSARGS = -b 2048
#FSTYPE = fat
#MKFSARGS = -F 32

all: loader.elf kernel.elf

os.iso: all
	cp loader.elf iso/boot/loader.elf
	cp kernel.elf iso/boot/kernel.elf
	grub-mkrescue -o os.iso iso

run: os.iso
	bochs -q -f bochsrc.txt

rund: updatedisk
	bochs -f bochsrcd.txt -q

runqemu: os.iso
	qemu-system-x86_64 -boot d -cdrom os.iso -m 512 -s -serial file:logqemu.txt

runqemud: updatedisk
	qemu-system-x86_64 -boot c -drive format=raw,file=disk.img -m 512 -s -serial file:logqemu.txt

runqemuu: updatedisk
	qemu-system-x86_64 -boot c -drive format=raw,file=disk.img -m 512 -s -nographic

connect : all
	gdb -ex "set arch i386:x86-64" -ex "symbol-file kernel.elf" -ex "target remote localhost:1234"



#-------------------------------kernel rules------------------------------------

kernel.elf: $(OBJ) $(SRCDIR)/link.ld libc.a libc++.a
	@echo Linking kernel
	@g++ $(LD64FLAGS) $(OBJ) -o kernel.elf $(LIBS64) -Xlinker --print-map > ld_mapping_full
	@cat ld_mapping_full | sed -e '1,/text/d' -e '/rodata/,$$d' > ld_mapping
	@echo --------------------------kernel built.------------------------------
	@echo
	@echo

$(OUTDIR)/%.s.o: $(SRCDIR)/%.s libc.a libc++.a Makefile
	@mkdir -p `dirname $@`
	@$(AS) $(ASFLAGS) $< -o $@
	@echo Assembling kernel file : $<

$(OUTDIR)/%.c.o: $(SRCDIR)/%.c libc.a libc++.a Makefile
	@mkdir -p `dirname $@`
	@mkdir -p `dirname $(patsubst $(SRCDIR)/%.c, $(DEPDIR)/%.c.d, $<)`
	@echo Compiling kernel file : $<
	@$(CC) $(CFLAGS) -MMD -MT '$@' -MF	$(patsubst $(SRCDIR)/%.c, $(DEPDIR)/%.c.d, $<) -c $< -o $@

$(OUTDIR)/%.cpp.o: $(SRCDIR)/%.cpp libc.a libc++.a Makefile
	@mkdir -p `dirname $@`
	@mkdir -p `dirname $(patsubst $(SRCDIR)/%.cpp, $(DEPDIR)/%.cpp.d, $<)`
	@echo Compiling kernel file : $<
	@$(CXX) $(CXXFLAGS) -MMD -MT '$@' -MF $(patsubst $(SRCDIR)/%.cpp, $(DEPDIR)/%.cpp.d, $<) -c $< -o $@


$(OUTDIR)/InterruptInt.o : $(SRCDIR)/Interrupts/Interrupt.py Makefile
	@python3 $(SRCDIR)/Interrupts/Interrupt.py > $(OUTDIR)/InterruptInt.s
	@$(AS) $(ASFLAGS) $(OUTDIR)/InterruptInt.s -o $(OUTDIR)/InterruptInt.o
	@echo Generating and assembling from kernel file : $<



#----------------------------------loader rules---------------------------------

loader.elf: $(OBJ32) $(SRC32DIR)/link.ld
	@echo Linking loader
	@g++ $(LD32FLAGS) $(OBJ32) -o loader.elf $(LIBS32) -Xlinker --print-map > ld_mapping_loader_full
	@cat ld_mapping_loader_full | sed -e '1,/text/d' -e '/rodata/,$$d' > ld_mapping_loader
	@echo --------------------------loader built.------------------------------
	@echo
	@echo



$(OUT32DIR)/%.s.o: $(SRC32DIR)/%.s Makefile
	@mkdir -p $(OUT32DIR)
	@$(AS) $(AS32FLAGS) $< -o $@
	@echo Assembling loader file : $<

$(OUT32DIR)/%.cpp.o: $(SRC32DIR)/%.cpp Makefile
	@mkdir -p $(OUT32DIR)
	@mkdir -p `dirname $(patsubst $(SRC32DIR)/%.cpp, $(DEP32DIR)/%.cpp.d, $<)`
	@echo Compiling loader file : $<
	@$(CXX) $(CXX32FLAGS) -MMD -MT '$@' -MF$(patsubst $(SRC32DIR)/%.cpp, $(DEP32DIR)/%.cpp.d, $<) -c $< -o $@


#----------------------------------libc rules-----------------------------------

$(OUTDIR)/$(LIBC)/%.o: $(LIBC)/src/%.c Makefile
	@mkdir -p $(OUTDIR)/libc
	@mkdir -p $(DEPDIR)/libc
	@echo Compiling libc file : $<
	@$(CC) $(CFLAGS) -MD -MT '$@' -MF	$(patsubst $(LIBC)/src/%.c, $(DEPDIR)/$(LIBC)/%.c.d, $<) -c $< -o $@

$(OUTDIR)/$(LIBC)/%.o: $(LIBC)/src/%.cpp Makefile
	@mkdir -p $(OUTDIR)/libc
	@mkdir -p $(DEPDIR)/libc
	@echo Compiling libc file : $<
	@$(CXX) $(CXXFLAGS) -MD -MT '$@' -MF	$(patsubst $(LIBC)/src/%.cpp, $(DEPDIR)/$(LIBC)/%.cpp.d, $<) -c $< -o $@

libc.a: $(LIBCOBJ) $(LIBCH) Makefile

	ar rcs libc.a $(LIBCOBJ)
	@echo --------------------------libc built.------------------------------
	@echo
	@echo


#----------------------------------libc++ rules---------------------------------

$(OUTDIR)/$(LIBCXX)/%.o: $(LIBCXX)/src/%.cpp libc.a Makefile
	@mkdir -p $(OUTDIR)/libc++
	@mkdir -p $(DEPDIR)/libc++
	@echo Compiling libcpp file : $<
	@$(CXX) $(CXXFLAGS) -MD -MT '$@' -MF	$(patsubst $(LIBCXX)/src/%.cpp, $(DEPDIR)/$(LIBCXX)/%.cpp.d, $<) -c $< -o $@

libc++.a: $(LIBCXXOBJ) $(LIBCXXH) libc.a Makefile
	ar rcs libc++.a $(LIBCXXOBJ)
	@echo --------------------------libc++ built.------------------------------
	@echo
	@echo



#---------------------------------Unit tests-------------------------------

unittest:
	@./unitTests.sh


buildunit: $(OBJ) $(SRCDIR)/link.ld libc.a libc++.a
	@$(CXX) $(CXXFLAGS) -DUNITTEST -c $(SRCDIR)/kmain.cpp -o $(OUTDIR)/kmain.cpp.o
	@echo Linking kernel for unit test
	@g++ $(LD64FLAGS) $(OBJ) $(OUTDIR)/unittest.o -o kernel.elf $(LIBS64) -Xlinker --print-map > ld_mapping_full
	@cat ld_mapping_full | sed -e '1,/text/d' -e '/rodata/,$$d' > ld_mapping
	@echo ----------------------unittest kernel built.------------------------------
	@echo
	@echo

getCompileLine:
	@echo $(CXX) $(CXXFLAGS)





disk.img :
	dd if=/dev/zero of=disk.img bs=512 count=131072

load: disk.img
	sudo losetup $(LOOPDEV) disk.img
	sudo partprobe $(LOOPDEV)

partition:
	sudo echo "," | sudo sfdisk $(LOOPDEV)
	sudo partprobe $(LOOPDEV)
	sudo mkfs.$(FSTYPE) $(MKFSARGS) $(LOOPDEV)p1

unload:
	sudo losetup -d $(LOOPDEV)
  #sudo partprobe "$(LOOPDEV)"

mount:
	sudo mkdir -p $(MNTPATH)
	sudo mount $(LOOPDEV)p1 $(MNTPATH)

umount:
	sudo umount $(MNTPATH)
	sudo rm -rf $(MNTPATH)

lm: load mount

ulm: umount unload


grubinst:
	sudo grub-install --root-directory=$(MNTPATH) --no-floppy --modules="normal part_msdos $(FSTYPE) multiboot" --target=i386-pc $(LOOPDEV)

mvtoimg: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	cp loader.elf iso/boot/loader.elf
	sudo rsync -r iso/ $(MNTPATH)/

builddisk: load partition mount grubinst mvtoimg ulm

updatedisk: kernel.elf loader.elf lm mvtoimg ulm





clean:
	rm -rf $(OUTDIR)
	rm -rf $(DEPDIR)
	rm -f disassembly
	rm -f disassemblyl
	rm -f ld_mapping
	rm -f ld_mapping_full
	rm -f ld_mapping_loader
	rm -f ld_mapping_loader_full
	rm -f os.iso
	rm -f kernel.elf
	rm -f loader.elf
	rm -f bochslog.txt
	rm -f libc.a
	rm -f libc++.a
	rm -f log.txt
	rm -f logqemu.txt
	rm -f logqemu_*.txt

mrproper: clean
	rm -f disk.img
	rm -f iso/boot/kernel.elf
	rm -f iso/boot/loader.elf

dasm:
	objdump -D -C kernel.elf > disassembly

dasml:
	objdump -D -C loader.elf > disassemblyl

count:
	cloc libc libc++ src src32 unitTests -lang-no-ext="C/C++ Header"

include $(DEPF)

