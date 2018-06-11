#include "elf_loader/hakase.h"
#include "tests/test.h"
#include <assert.h>

int test_main(F2H &f2h, H2F &h2f, int argc, const char **argv) {
  if (argc < 2) {
    return 1;
  }

  auto file = ElfLoader::ElfFile::Load(argv[1]);
  if (!file) {
    return 1;
  }

  {
    auto r = file->Init(h2f);
    if (r.IsError()) {
      r.IgnoreError();
      return 1;
    }
    r.Unwrap();
  }
  
  ElfLoader sl(h2f, std::move(file));

  {
    auto r = sl.Deploy();
    if (r.IsError()) {
      r.IgnoreError();
      return 1;
    }
    r.Unwrap();
  }

  {
    auto r = sl.Execute();
    if (r.IsError()) {
      r.IgnoreError();
      return 1;
    }
    r.Unwrap();
  }

  while(true) {
    switch(f2h.WaitNewSignal()) {
    case 1: {
      uint32_t rval;
      f2h.Read(0, rval);
      return rval;
    }
    case 2: {
      uint8_t data;
      f2h.Read(0, data);
      f2h.Return(0);
      putchar(data);
      break;
    }
    default:
      return 1;
    }
  }
}
