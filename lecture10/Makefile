include hakase/build_rule.mk

ifeq ($(HOST),)

TESTS = exec
TEST_OBJS := exec.o $(TEST_DIR)test.o
OBJS := $(TEST_OBJS)
DEPS := $(OBJS:%.o=%.d)
BAREMETAL_SOURCES := hakase.cc raw.cc int.cc int_asm.S
BAREMETAL_OBJS := $(patsubst %.S,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(BAREMETAL_SOURCES))))
.DEFAULT_GOAL:=test
FLAGS = -O0 -Wall -nostdinc -nostdlib -iquote $(ROOT_DIR)
BAREMETAL_CFLAGS = $(FLAGS)
BAREMETAL_CXXFLAGS = $(FLAGS) --std=c++14

.PHONY: test usb image

define baremetal_cxx_template
$(1:%.cc=%.o): $(1)
	g++ $(BAREMETAL_CXXFLAGS) -c $(1) -o $(1:%.cc=%.o)
endef

define baremetal_c_template
$(1:%.c=%.o): $(1)
	gcc $(BAREMETAL_CFLAGS) -c $(1) -o $(1:%.c=%.o)
endef

$(foreach file,$(filter %.cc,$(BAREMETAL_SOURCES)),$(eval $(call baremetal_cxx_template,$(file))))
$(foreach file,$(filter %.c,$(BAREMETAL_SOURCES)),$(eval $(call baremetal_c_template,$(file))))

int_asm.o: int_asm.S
	gcc $(BAREMETAL_CFLAGS) -c int_asm.S -o int_asm.o

raw.bin: $(filter-out pbaremetal.o,$(BAREMETAL_OBJS))
	g++ $(BAREMETAL_CXXFLAGS) -T raw.ld $^ -o $@

$(MODULE_DIR)libelf_loader.a:
	$(MAKE) -C $(ROOT_DIR)elf_loader all

exec.bin: $(TEST_OBJS) $(MODULE_DIR)libelf_loader.a
	g++ -pthread -o $@ $^

test: exec.bin raw.bin
	$(MAKE) load
	@echo "info: Starting baremetal program..."
	@$(foreach test, $(TESTS), $(TEST_DIR)test_hakase.sh 0 $(shell pwd)/$(test).bin $(shell pwd)/raw.bin &&) :
	$(MAKE) unload

clean:
	rm -f disk.img *.bin *.o *.d
	$(MAKE) -C $(ROOT_DIR) clean
	$(MAKE) -C ../hardware clean

else

.PHONY: vnc vboxtest vboxkill

vboxtest:
	$(MAKE) image
	-VBoxManage controlvm hakase_baremetal_test poweroff
	sleep 1
	-VBoxManage unregistervm hakase_baremetal_test --delete
	rm -f disk.vdi
	VBoxManage createvm --name hakase_baremetal_test --register
	VBoxManage modifyvm hakase_baremetal_test --memory 4096 --ioapic on --chipset ich9 --hpet on --firmware efi
	VBoxManage convertfromraw disk.img disk.vdi
	VBoxManage storagectl hakase_baremetal_test --name SATAController --add sata --controller IntelAHCI --bootable on
	VBoxManage storageattach hakase_baremetal_test --storagectl SATAController --port 0 --device 0 --type hdd --medium disk.vdi
	VBoxManage startvm hakase_baremetal_test --type gui

vboxkill:
	-VBoxManage controlvm hakase_baremetal_test poweroff

endif
