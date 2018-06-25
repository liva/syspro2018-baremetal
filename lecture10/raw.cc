#include "../hardware/info.h"
#include "common/_memory.h"
#include "int.h"

Idt idt;

void init();
void return_value(int rval);
void putchar(char c);
int16_t get_apicid();

static void handler(Regs *rs, void *arg) {
  //!!!!!!!!!!!!!!!!!!!!!!
  // kadai B
  //!!!!!!!!!!!!!!!!!!!!!!
  // Edit the page table!
  // pd2[???] = (???) | (1 << 0) | (1 << 1) | (1 << 2) | (1<<7);
}

int main() {
  init();


  //!!!!!!!!!!!!!!!!!!!!!!
  // kadai A
  //!!!!!!!!!!!!!!!!!!!!!!
  // Implement spinlock here.
  char buf[] = "CPU 0 booted!\n";
  buf[4] += get_apicid();
  for(char c : buf) {
    putchar(c);
  }

  uint64_t *pml4t = (uint64_t *)0x301000UL;
  uint64_t *pdpt  = (uint64_t *)0x302000UL;
  uint64_t *pd1   = (uint64_t *)0x303000UL;
  uint64_t *pd2   = (uint64_t *)0x304000UL;

  pml4t[0] = ((uint64_t)pdpt + 0x80000000UL) | (1 << 0) | (1 << 1) | (1 << 2);
  pdpt[0]  = ((uint64_t)pd1 + 0x80000000UL) | (1 << 0) | (1 << 1) | (1 << 2);
  pdpt[3]  = ((uint64_t)pd2 + 0xC0000000UL) | (1 << 0) | (1 << 1) | (1 << 2);
  for(int i = 0; i < 512; i++) {
    pd1[i]  = (0x80000000UL + 0x200000UL * i) | (1 << 0) | (1 << 1) | (1 << 2) | (1<<7);
  }
  // Do not edit page tables in main()!
  // Edit them in handler() instead.
  
  asm volatile("movq %0, %%cr3"::"r"((uint64_t)pml4t + 0x80000000UL));


  // interrupt settings
  idt.SetupGeneric();
  idt.SetupProc();
  idt.SetExceptionCallback(14, handler, nullptr);
  
  //!!!!!!!!!!!!!!!!!!!!!!
  // kadai B
  //!!!!!!!!!!!!!!!!!!!!!!
  uint32_t *apic_addr = 0;
  // apic_addr = ???;  // rewrite this
  int apicid = (*apic_addr >> 24);
  
  return_value((apicid == get_apicid()) ? 0 : 1);
  
  return 0;
}
