#include "elf_loader/hakase.h"
#include "tests/test.h"
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <thread>

// thanks to hibari(@lcst_topevx)
float depth = 24.0; //1024 x 768 x "24"
int fb_size;

void* init_xvfb(){

	const char *filename = "/tmp/Xvfb_screen0";
	FILE *fp;

	if((fp = fopen(filename, "r"))==NULL){
		perror("open error");
		perror("please check if \"Xvfb_screen0\" exists");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	fb_size = ftell(fp);
	fclose(fp);

	// mmap Xvfb framebuffer
	int fd;
	if((fd = open(filename, O_RDWR)) < 0){
		perror("open error");
		exit(1);
	}
	void *addr = mmap(NULL, fb_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);


	//https://samy.pl/packet/MISC/tru64/usr/include/X11/XWDFile.h
	//big endian...??
	uint8_t header_size = static_cast<uint8_t*>(addr)[3];

	// 256 ... 0x00 ~ 0xff
	uint32_t offset = header_size + 256 * static_cast<uint32_t>(16 * (depth/32.0));

	// fb1 is a start address of framebuffer
	uint8_t *fb1 = static_cast<uint8_t*>(addr) + offset;

	return fb1;
}

void draw() {
  uint8_t *fb1 = static_cast<uint8_t *>(init_xvfb());

  while(true) {
    FILE *fp;
    uint8_t buf[1024 * 768 * 4];

    if ((fp = fopen("/sys/kernel/debug/friend_loader/memory", "rb")) == NULL) {
      exit(1);
    }

    if (fseek(fp, 0x400000UL, SEEK_SET) != 0) {
      exit(1);
    }

    {
      size_t buf_read = 0;
      while(buf_read < sizeof(buf)) {
        buf_read += fread(buf + buf_read, 1, sizeof(buf) - buf_read, fp);
      }
    }

    fclose(fp);

    memcpy(fb1, buf, 1024 * 768 * 4);

    usleep(1000 * 100);
  }

  // never called but should be
	munmap(fb1, fb_size);
}

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

  // drawer thread
  std::thread(draw).detach();

  while(true) {
    switch(f2h.WaitNewSignal()) {
    case 1: {
      uint32_t rval;
      f2h.Read(0, rval);
      sleep(1); // to wait drawer thread
      // return rval;
      break; // TODO
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
