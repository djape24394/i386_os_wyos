GPPPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		  obj/GlobalDescriptorTable.o \
		  obj/memory_management.o \
		  obj/hardware_communication/Port.o \
		  obj/hardware_communication/interruptstubs.o \
		  obj/hardware_communication/InterruptManager.o \
		  obj/hardware_communication/pci.o \
		  obj/multitasking.o \
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
