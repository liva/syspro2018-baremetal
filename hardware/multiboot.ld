OUTPUT_FORMAT(elf64-x86-64)

/* ブートローダが呼び出すラベル */
ENTRY (entry)

SECTIONS{
  /* multibootの都合上、各セクションは0x100000以降に置かれなければならない */
  . = 0x100000;
  .boot : {
    *(.bootstrap*)
    boot.o (.text*)
    boot.o (.rodata*)
    boot.o (.data*)
    boot.o (.bss*)
    . = ALIGN(0x1000);
  }

  . = 0x8000;
  .boot16 : AT(LOADADDR(.boot) + SIZEOF(.boot)) {
    /*
    bootap.o (.text*)
    bootap.o (.rodata*)
    bootap.o (.data*)
    bootap.o (.bss*)
    */
    . = ALIGN(0x1000);
  }
  boot16_start = LOADADDR(.boot16);
  boot16_end = LOADADDR(.boot16) + SIZEOF(.boot16);

  . = LOADADDR(.boot16) + SIZEOF(.boot16);
  .text : AT(LOADADDR(.boot16) + SIZEOF(.boot16)) {
    *(EXCLUDE_FILE (*boot.o *bootap.o) .text*)
    *(.gnu.linkonce.t*)
  }

  .rodata : {
    *(EXCLUDE_FILE (*boot.o *bootap.o) .rodata*)
    *(.gnu.linkonce.r*)
  }

  .data : {
    *(EXCLUDE_FILE (*boot.o *bootap.o) .data*)
    *(.gnu.linkonce.d*)
  }

  .bss : {
    *(COMMON*)
    *(.COMMON*)
    *(EXCLUDE_FILE (*boot.o *bootap.o) .bss*)
    *(.gnu.linkonce.b*)
  }

  .note : {
    *(.note*)
    *(.eh_frame .eh_frame_hdr)
    *(.comment*)
  }

  .unsupported : {
    *(*)
    ASSERT(0, "Found non-empty unsupported section in object files.");
  }

  . = ALIGN(0x1000);

  second_pagetables = .;
  . += 0x3000;

  ASSERT(.<0x300000, "too huge binary")
}
