#include "../hardware/info.h"
#include "common/_memory.h"

void init();
// only available on VM
// cannot use on hardware
void return_value(int rval);
void putchar(char c);

int main() {
  init();
  char buf[] = "booted\n";
  for(char c : buf) {
    putchar(c);
  }

  uint64_t *pml4t = (uint64_t *)0x301000UL;
  uint64_t *pdpt  = (uint64_t *)0x302000UL;
  uint64_t *pd1   = (uint64_t *)0x303000UL;
  uint64_t *pd2   = (uint64_t *)0x304000UL;

  pml4t[0] = ((uint64_t)pdpt + 0x80000000UL) | (1 << 0) | (1 << 1) | (1 << 2);
  pdpt[0]  = ((uint64_t)pd1 + 0x80000000UL) | (1 << 0) | (1 << 1) | (1 << 2);
  for(int i = 0; i < 512; i++) {
    pd1[i]  = (0x80000000UL + 0x200000UL * i) | (1 << 0) | (1 << 1) | (1 << 2) | (1<<7);
  }
  
  asm volatile("movq %0, %%cr3"::"r"((uint64_t)pml4t + 0x80000000UL));

  
  //!!!!!!!!!!!!!!!!!!!!!!
  // add your codes here!
  //!!!!!!!!!!!!!!!!!!!!!!

  return_value(0);
  
  return 0;
}
