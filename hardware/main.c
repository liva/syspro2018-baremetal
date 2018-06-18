/*
 *            main.c
 * written by Shinichi Awamoto, 2017
 * 
 * 各種ハードウェア初期化関数の呼び出し
 */

/*
#include "apic.h"
#include "idt.h"
#include "segment.h"
#include "framebuffer.h"
*/
#include "common.h"
#include "acpi.h"
#include "multiboot2.h"
#include "info.h"

void panic() {
  while(1) {
    __asm__ volatile("cli;hlt;nop;nop;");
  }
}

void cmain() {
  struct rsdp_descriptor *rsdp = multiboot_get_rsdp_desc();
  if (rsdp == NULL) {
    panic();
  }
  acpi_init(rsdp);

  struct multiboot_tag_framebuffer_common *framebuffer = multiboot_get_framebuffer();
  if (framebuffer == NULL) {
    panic();
  }

  hw_info->framebuffer_addr = (uint8_t *)framebuffer->framebuffer_addr;
  hw_info->framebuffer_width = framebuffer->framebuffer_width;
  hw_info->framebuffer_height = framebuffer->framebuffer_height;
  hw_info->framebuffer_bpp = framebuffer->framebuffer_bpp;

  struct hpet_descriptor *hpet = acpi_get_hpet_desc();

  hw_info->hpet_addr = (uint64_t *)hpet->address;

  for(uint64_t i = 0x100000; i < 0x4000000; i++) {
    *(uint64_t *)(i + 0x80000000) = *(uint64_t *)i;
  }

  extern char second_pagetables[];
  uint64_t *pml4t = (uint64_t *)((uint64_t)second_pagetables + 0x80000000);
  uint64_t *pdpt  = (uint64_t *)((uint64_t)second_pagetables + 0x80001000);
  uint64_t *pd    = (uint64_t *)((uint64_t)second_pagetables + 0x80002000);
  pml4t[0] = ((uint64_t)pdpt) | (1 << 0) | (1 << 1) | (1 << 2);
  pdpt[0]  = ((uint64_t)pd) | (1 << 0) | (1 << 1) | (1 << 2);
  for(int i = 0; i < 512; i++) {
    pd[i]  = (0x80000000UL + 0x200000UL * i) | (1 << 0) | (1 << 1) | (1 << 2) | (1<<7);
  }
  __asm__ volatile("movq %0, %%cr3; call main;"::"r"((uint64_t)pml4t));

  /*
  struct apic_descriptor *madt = acpi_get_apic_desc();
  if (madt == NULL) {
    panic();
  }
  apic_init(madt);

  gdt_init();

  idt_init();

  idt_init_for_each_proc();

  apic_enable_lapic();

  apic_start_other_processors();
  */

  while(1) {
    __asm__ volatile("hlt;");
  }
}

/*
void cmain_for_ap() {
  apic_initialize_ap();

  gdt_init();

  idt_init_for_each_proc();

  // TODO ここにコードを追加

  while(1) {
    __asm__ volatile("hlt;");
  }
}
*/
