HOST=$(shell if [ ! -e /etc/hakase_installed ]; then echo "host"; fi)
OBJS=boot.o main.o multiboot2.o acpi.o
MOUNT_DIR=/mnt

FLAGS = -O0 -Wall -Werror=return-type -Werror=unused-result -nostdinc -nostdlib -fno-builtin -MMD -MP
ASFLAGS += $(FLAGS)
CFLAGS += $(FLAGS) -std=c99

ifneq ($(HOST),)
# host environment
default:
	@echo "Please run on guest environment."
	@exit 1

else
# guest environment
.DEFAULT_GOAL:=default

ifneq ($(BAREMETAL_OBJ),)
.PHONY: default FORCE mount unmount usb

default:

FORCE:

baremetal.o: $(BAREMETAL_OBJ) FORCE
	@if [ ! -e baremetal.o ] || [ "`sum $(BAREMETAL_OBJ)`" != "`sum baremetal.o`" ]; then cp $(BAREMETAL_OBJ) baremetal.o; fi

multiboot.elf: $(OBJS) multiboot.ld baremetal.o
	g++ -nostdinc -nostdlib -Tmultiboot.ld -Wl,--nmagic $(OBJS) baremetal.o -o $@

$(MOUNT_DIR)/boot/grub/grub.cfg: grub.cfg
	sudo cp grub.cfg $(MOUNT_DIR)/boot/grub/grub.cfg

$(MOUNT_DIR)/boot/multiboot.elf: multiboot.elf
	sudo cp multiboot.elf $(MOUNT_DIR)/boot/multiboot.elf

/tmp/disk.img: disk.img.tar.xz multiboot.elf grub.cfg
	$(MAKE) unmount
	tar xf disk.img.tar.xz
	mv disk.img /tmp
	$(MAKE) mount
	$(MAKE) $(MOUNT_DIR)/boot/grub/grub.cfg
	$(MAKE) $(MOUNT_DIR)/boot/multiboot.elf
	$(MAKE) unmount

usb: /tmp/disk.img
	$(if $(shell if [ ! -e /dev/sdb ]; then echo "no usb"; fi), \
	  @echo "error: insert usb memory!"; exit 1, \
	)
	sudo dd if=/tmp/disk.img of=/dev/sdb bs=1M

mount:
	sudo kpartx -avs /tmp/disk.img
	sleep 1
	sudo mount -t ext2 /dev/mapper/loop0p3 $(MOUNT_DIR)

unmount:
	$(if $(shell mountpoint -q $(MOUNT_DIR); if [ $$? = 0 ]; then echo "mounted"; fi), sudo umount $(MOUNT_DIR))
	sudo kpartx -d /tmp/disk.img || return 0
	sudo losetup -d /dev/loop[0-9] > /dev/null 2>&1 || return 0

else
default:
	@echo "Please set 'BAREMETAL_OBJ' variable."
	@exit 1
endif
endif

clean:
	rm -f /tmp/disk.img multiboot.elf *.o

