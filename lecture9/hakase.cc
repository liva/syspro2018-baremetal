#include "../hardware/info.h"
#include "common/_memory.h"

void init() {
  hw_info->framebuffer_addr = (uint8_t *)0x400000UL;
  hw_info->framebuffer_width = 1024;
  hw_info->framebuffer_height = 768;
  hw_info->framebuffer_bpp = 32;
  hw_info->hpet_addr = (uint64_t *)0xfed00000;
  
}

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
