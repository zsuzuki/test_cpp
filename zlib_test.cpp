//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <zlib.h>

unsigned char msg[] = "Hello,World\n"
                      "My name is POPOPOPO.\n"
                      "私の名前は中野です\n"
                      "play for futsal on today.\n"
                      "take your picture\n"
                      "明日は明日の風が吹く\n"
                      "convert on server\n"
                      "put on your mark\n"
                      "get set\n"
                      "ready\n"
                      "go";

unsigned char buffer[1024];
unsigned char outbuffer[1024];

class worker
{
  std::vector<uint8_t> buffer;
  size_t               size;

public:
  worker(size_t sz)
  {
    buffer.resize(sz);
    size = sz;
  }
  void* alloc(size_t sz)
  {
    size -= sz;
    printf("allocate: %zu - %zu\n", size, sz);
    return &buffer[size];
  }
  void free(void*) {}
};

int
main(int argc, char** argv)
{
#if __cplusplus > 199711L || _MSC_VER >= 1900
  printf("new c++ version: %ld\n", __cplusplus);
#endif
  worker   w(400 * 1024);
  z_stream z;
  z.zalloc    = [](auto opq, auto items, auto sz) { return reinterpret_cast<worker*>(opq)->alloc(items * sz); };
  z.zfree     = [](auto opq, auto ptr) { reinterpret_cast<worker*>(opq)->free(ptr); };
  z.opaque    = &w;
  z.next_in   = msg;
  z.avail_in  = sizeof(msg);
  z.next_out  = buffer;
  z.avail_out = sizeof(buffer);

  deflateInit2(&z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
  // deflateInit(&z, Z_DEFAULT_COMPRESSION);
  deflate(&z, Z_FINISH);
  deflateEnd(&z);
  printf("size: %zu/%zu\n", sizeof(buffer) - z.avail_out, sizeof(msg));

  z.next_in   = buffer;
  z.avail_in  = sizeof(buffer);
  z.next_out  = outbuffer;
  z.avail_out = sizeof(outbuffer);
  // inflateInit(&z);
  inflateInit2(&z, MAX_WBITS);
  inflate(&z, Z_FINISH);
  inflateEnd(&z);

  printf("msg(%d):\n%s\n", z.avail_in, outbuffer);

  printf("\ndone.\n");
  return 0;
}
//
// End
//
