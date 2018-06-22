include hakase/build_rule.mk

ifeq ($(HOST),)

TESTS = exec
TEST_OBJS := exec.o $(TEST_DIR)test.o
OBJS := $(TEST_OBJS)
DEPS := $(OBJS:%.o=%.d)
.DEFAULT_GOAL:=test
CXXFLAGS += -pthread

.PHONY: test usb image

raw.bin: raw.cc hakase.cc framebuffer.cc font.c
	g++ -O0 -Wall --std=c++14 -nostdinc -nostdlib -iquote $(ROOT_DIR) -T raw.ld $^ -o $@

$(MODULE_DIR)libelf_loader.a:
	$(MAKE) -C $(ROOT_DIR)elf_loader all

exec.bin: $(TEST_OBJS) $(MODULE_DIR)libelf_loader.a
	g++ $(CXXFLAGS) -o $@ $^

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

baremetal.o: raw.cc pbaremetal.cc framebuffer.cc font.c
	g++ -O0 -Wall --std=c++14 -nostdinc -nostdlib -iquote $(ROOT_DIR) $^ -o $@

usb: baremetal.o
	$(MAKE) BAREMETAL_OBJ=$(shell pwd)/baremetal.o -C ../hardware usb

image: baremetal.o
	$(MAKE) BAREMETAL_OBJ=$(shell pwd)/baremetal.o -C ../hardware /tmp/disk.img
	cp /tmp/disk.img .

clean:
	rm -f disk.img *.bin $(OBJS) $(DEPS) baremetal.o
	$(MAKE) -C $(ROOT_DIR) clean
	$(MAKE) -C ../hardware clean

else

.PHONY: vnc vboxtest vboxkill

vnc:
	vncviewer localhost:15900

vboxtest:
	$(MAKE) image
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
