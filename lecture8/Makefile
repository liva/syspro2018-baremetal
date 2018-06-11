include hakase/build_rule.mk

ifeq ($(HOST),)

TESTS = exec
TEST_OBJS := exec.o $(TEST_DIR)test.o
OBJS := $(TEST_OBJS)
DEPS := $(OBJS:%.o=%.d)
.DEFAULT_GOAL:=test

.PHONY: test

raw.bin: raw.cc
	g++ -O0 -Wall --std=c++14 -nostdinc -nostdlib -iquote $(ROOT_DIR) -T raw.ld $^ -o $@

$(MODULE_DIR)libelf_loader.a:
	$(MAKE) -C $(ROOT_DIR)elf_loader all

exec.bin: $(TEST_OBJS) $(MODULE_DIR)libelf_loader.a
	g++ $(CXXFLAGS) -o $@ $^

test: exec.bin raw.bin
	$(MAKE) load
	@echo "info: Starting baremetal program..."
	@$(foreach test, $(TESTS), $(TEST_DIR)test_hakase.sh 0 $(shell pwd)/$(test).bin $(shell pwd)/raw.bin &&) :
	$(MAKE) unload

clean:
	rm -f *.bin $(OBJS) $(DEPS)
	$(MAKE) -C $(ROOT_DIR) clean

endif
