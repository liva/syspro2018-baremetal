include hakase/build_rule.mk

ifeq ($(HOST),)

TESTS = exec
TEST_OBJS := exec.o $(TEST_DIR)test.o
OBJS := $(TEST_OBJS)
DEPS := $(OBJS:%.o=%.d)
.DEFAULT_GOAL:=test
FLAGS = -O0 -Wall -nostdinc -nostdlib -iquote $(ROOT_DIR)
BAREMETAL_CFLAGS = $(FLAGS)
BAREMETAL_CXXFLAGS = $(FLAGS) --std=c++14

.PHONY: test usb image

raw.o: raw.cc
	g++ $(BAREMETAL_CXXFLAGS) -c $^ -o $@

hakase.o: hakase.cc
	g++ $(BAREMETAL_CXXFLAGS) -c $^ -o $@

framebuffer.o: framebuffer.cc
	g++ $(BAREMETAL_CXXFLAGS) -c $^ -o $@

font.o: font.c
	gcc $(BAREMETAL_CFLAGS) -c $^ -o $@

raw.bin: raw.o hakase.o framebuffer.o font.o
	g++ $(BAREMETAL_CXXFLAGS) -T raw.ld $^ -o $@

$(MODULE_DIR)libelf_loader.a:
	$(MAKE) -C $(ROOT_DIR)elf_loader all

exec.bin: $(TEST_OBJS) $(MODULE_DIR)libelf_loader.a
	g++ -pthread -o $@ $^

test: exec.bin raw.bin
	pkill Xvfb 2>&1 || :
	Xvfb :1 -screen 0 1024x768x24 -fbdir /tmp &
	export DISPLAY=:1
	sleep 1
	x11vnc -display :1 -q -bg
	$(MAKE) load
	@echo "info: Starting baremetal program..."
	@$(foreach test, $(TESTS), $(TEST_DIR)test_hakase.sh 0 $(shell pwd)/$(test).bin $(shell pwd)/raw.bin &&) :
	$(MAKE) unload

baremetal.a: raw.o pbaremetal.o framebuffer.o font.o
	ar rcs $@ $^

usb: baremetal.a
	$(MAKE) BAREMETAL_OBJ=$(shell pwd)/baremetal.a -C ../hardware usb

image: baremetal.a
	$(MAKE) BAREMETAL_OBJ=$(shell pwd)/baremetal.a -C ../hardware /tmp/disk.img
	cp /tmp/disk.img .

clean:
	rm -f disk.img *.bin $(OBJS) $(DEPS) baremetal.a
	$(MAKE) -C $(ROOT_DIR) clean
	$(MAKE) -C ../hardware clean

else

.PHONY: vnc vboxtest vboxkill

vnc:
	vncviewer localhost:15900

vboxtest:
	$(MAKE) image
	-VBoxManage controlvm hakase_baremetal_test poweroff
	sleep 1
	-VBoxManage unregistervm hakase_baremetal_test --delete
	rm -f disk.vdi
	VBoxManage createvm --name hakase_baremetal_test --register
	VBoxManage modifyvm hakase_baremetal_test --ioapic on --chipset ich9 --hpet on --firmware efi
	VBoxManage convertfromraw disk.img disk.vdi
	VBoxManage storagectl hakase_baremetal_test --name SATAController --add sata --controller IntelAHCI --bootable on
	VBoxManage storageattach hakase_baremetal_test --storagectl SATAController --port 0 --device 0 --type hdd --medium disk.vdi
	VBoxManage startvm hakase_baremetal_test --type gui

vboxkill:
	-VBoxManage controlvm hakase_baremetal_test poweroff

endif
