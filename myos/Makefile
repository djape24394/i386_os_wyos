# Bootloader won't set the stack pointer. C++ programs expect SP to be set before they start running.
# That is why we are going to build mykernel.bin from 2 main files, kernel.cpp and loader.s. File loader.s will just set the 
# stack pointer. We will work only in 32 bit mode.

# GPPARAMS need to tell compiler that basically we do not have any support from the operatng system, as we are writing it :-fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions
# -fno-leading-underscore, otherwise it would need to be _kernelMain
GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
ASPARAMS = --32 # 32 bit
LDPARAMS = -melf_i386 #

objects = obj/loader.o \
          obj/common/print_to_console.o \
		  obj/GlobalDescriptorTable.o \
		  obj/memory_management.o \
		  obj/hardware_communication/Port.o \
		  obj/hardware_communication/interruptstubs.o \
		  obj/hardware_communication/InterruptManager.o \
		  obj/hardware_communication/pci.o \
		  obj/multitasking.o \
		  obj/system_calls.o \
		  obj/drivers/Driver.o \
		  obj/drivers/KeyboardDriver.o \
		  obj/drivers/MouseDriver.o \
		  obj/drivers/AdvancedTechnologyAttachment.o \
		  obj/drivers/VideoGraphicsArray.o \
		  obj/drivers/AMD_am79c973.o \
		  obj/gui/Widget.o \
		  obj/gui/Window.o \
		  obj/gui/Desktop.o \
		  obj/kernel.o

.PHONY: run clean_all

obj/%.o: source/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: source/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	ld $(LDMARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
		sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin
		mkdir iso
		mkdir iso/boot
		mkdir iso/boot/grub
		cp mykernel.bin iso/boot/mykernel.bin
		echo 'set timeout=0' > iso/boot/grub/grub.cfg
		echo 'set default=0' >> iso/boot/grub/grub.cfg
		echo '' >> iso/boot/grub/grub.cfg
		echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
		echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
		echo '	boot' >> iso/boot/grub/grub.cfg
		echo '}' >> iso/boot/grub/grub.cfg
		grub-mkrescue --output=mykernel.iso iso
		rm -rf iso

run: mykernel.iso
	(killall virtualboxvm && sleep 1) || true
	virtualboxvm --startvm "MyOs" &

clean_all:
	rm -rf obj mykernel.bin mykernel.iso
