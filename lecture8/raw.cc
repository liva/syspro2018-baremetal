using int8_t   = __INT8_TYPE__;
using uint8_t  = __UINT8_TYPE__;
using int16_t  = __INT16_TYPE__;
using uint16_t = __UINT16_TYPE__;
using int32_t  = __INT32_TYPE__;
using uint32_t = __UINT32_TYPE__;
using int64_t  = __INT64_TYPE__;
using uint64_t = __UINT64_TYPE__;
#include "common/_memory.h"

void return_value(int rval) {
  int *channel = reinterpret_cast<int *>(MemoryMap::kF2h);
  channel[2] = rval;
  channel[0] = 1;
}

// you can create your own printf using this.
void putchar(char c) {
  int *channel = reinterpret_cast<int *>(MemoryMap::kF2h);
  channel[2] = c;
  channel[0] = 2;
  while(channel[0] != 0) {
    asm volatile("pause" ::: "memory");
  }
}

int main() {
  putchar('b');
  putchar('o');
  putchar('o');
  putchar('t');
  putchar('e');
  putchar('d');
  putchar('\n');

  uint64_t *pml4t = (uint64_t *)0x201000UL;
  uint64_t *pdpt  = (uint64_t *)0x202000UL;

  
  pml4t[0] = ((uint64_t)pdpt + 0x80000000UL) | (1 << 0) | (1 << 1) | (1 << 2);
  pdpt[0]  = 0x80000000UL | (1 << 0) | (1 << 1) | (1 << 2) | (1 << 7);
  //!!!!!!!!!!!!!!!!!!!!!!
  // add your codes here!
  //!!!!!!!!!!!!!!!!!!!!!!
  
  asm volatile("movq %0, %%cr3"::"r"((uint64_t)pml4t + 0x80000000UL));

  if (*((uint64_t *)0x200000UL) == *((uint64_t *)0x4000000000UL)) {
    return_value(0);
  } else {
    return_value(1);
  }
  
  return 0;
}
