# $@ = target file
# $< = first dependency
# $^ = all dependencies

C_SOURCES = $(wildcard kernel/*.c kernel/*/*.c kernel/*/*/*.c kernel/*/*/*/*.c)
CPP_SOURCES = $(wildcard kernel/*.cpp kernel/*/*.cpp kernel/*/*/*.cpp kernel/*/*/*/*.cpp)
S_SOURCES = $(wildcard kernel/hardwarecommunication/interrupts/*.s )
HEADERS = $(wildcard include/*.h)
C_OBJ = ${C_SOURCES:.c=.o} 
CPP_OBJ = ${CPP_SOURCES:.cpp=.o} 
S_OBJ = ${S_SOURCES:.s=.o} 

all: run

products/kernel.bin: products/kernel_entry.o ${C_OBJ} ${S_OBJ} ${CPP_OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

products/kernel_entry.o: kernel/boot/kernel_entry.s
	/usr/local/bin/nasm $< -f elf -o $@

%.o: %.c ${HEADERS}
	i386-elf-gcc -ffreestanding -c $< -o $@ -I./include

%.o: %.cpp ${HEADERS}
	i386-elf-g++ -ffreestanding -c $< -o $@ -I./include

%.o: %.s
	/usr/local/bin/nasm $< -f elf -o $@

debug/kernel.dis: products/kernel.bin
	ndisasm -b 32 $< > $@

products/boot.bin: kernel/boot/boot.s
	/usr/local/bin/nasm $< -f bin -o $@

products/os-image.bin: products/boot.bin products/kernel.bin
	cat $^ > $@

run: products/os-image.bin
	./qemu/programs/qemu-system-i386 -d int -fda $< -device piix3-ide,id=ide -drive id=disk,file=one.img,if=none -device ide-drive,drive=disk,bus=ide.0

clean:
	rm -rf products/*.bin products/*.o debug/*.dis
	rm -rf kernel/*.bin kernel/*.o kernel/*.dis
	rm -rf kernel/*/*.bin kernel/*/*.o
	rm -rf kernel/*/*/*.bin kernel/*/*/*.o
	rm -rf kernel/*/*/*/*.bin kernel/*/*/*/*.o
	rm -rf drivers/*/*.bin drivers/*/*.o drivers/*/*.dis
	rm -rf *.bin *.o *.dis

one.img:
	qemu-img create -f raw one.img 1M