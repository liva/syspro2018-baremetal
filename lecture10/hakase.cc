#include "../hardware/info.h"
#include "common/_memory.h"

int16_t get_apicid() {
  unsigned int *channel = reinterpret_cast<unsigned int *>(MemoryMap::kF2h);
  int16_t id;
  asm volatile("movw %%fs:0x0, %0" : "=r"(id));
  return id;
}  

int16_t get_cpuid() {
  unsigned int *channel = reinterpret_cast<unsigned int *>(MemoryMap::kF2h);
  int16_t id;
  asm volatile("movw %%fs:0x4, %0" : "=r"(id));
  return id;
}  

void init() {
  hw_info->framebuffer_addr = (uint8_t *)0x400000UL;
  hw_info->framebuffer_width = 1024;
  hw_info->framebuffer_height = 768;
  hw_info->framebuffer_bpp = 32;
  hw_info->hpet_addr = (uint64_t *)0xfed00000;
  
}

void return_value(int rval) {
  int *channel = reinterpret_cast<int *>(MemoryMap::kF2h);
  int16_t id = get_apicid();
  while(!__sync_bool_compare_and_swap(&channel[0], 0, id << 16)) {
    asm volatile("pause" ::: "memory");
  }
  channel[2] = rval;
  channel[0] = 1 | (id << 16);
  while((channel[0] & 0xFFFF) != 0) {
    asm volatile("pause" ::: "memory");
  }
  channel[0] = 0;
}

// you can create your own printf using this.
void putchar(char c) {
  int *channel = reinterpret_cast<int *>(MemoryMap::kF2h);
  int16_t id = get_apicid();
  while(!__sync_bool_compare_and_swap(&channel[0], 0, id << 16)) {
    asm volatile("pause" ::: "memory");
  }
  channel[2] = c;
  channel[0] = 2 | (id << 16);
  while((channel[0] & 0xFFFF) != 0) {
    asm volatile("pause" ::: "memory");
  }
  channel[0] = 0;
}
