#pragma once
#include "common.h"

struct HardwareInfo {
  uint64_t *hpet_addr;
  uint8_t *framebuffer_addr;
  int framebuffer_width;
  int framebuffer_height;
  int framebuffer_bpp;
};

static struct HardwareInfo * const hw_info = (struct HardwareInfo *)0x300000UL;
