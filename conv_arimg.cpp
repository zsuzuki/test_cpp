//
// convert png to arduino uno(ILI8349) 16bit image file
// wave.suzuki.z@gmail.com
//
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <png.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

namespace
{
//
struct PNGPTR
{
  png_structp ptr  = nullptr;
  png_infop   info = nullptr;

  PNGPTR() = default;
  PNGPTR(png_structp p) : ptr(nullptr), info(nullptr) {}

  ~PNGPTR()
  {
    if (ptr && info)
    {
      png_destroy_read_struct(&ptr, &info, nullptr);
    }
  }
};

//
using Buffer = std::vector<uint8_t>;

//
class ImageBase
{
protected:
  int    width;
  int    height;
  Buffer buffer;

public:
  virtual ~ImageBase() = default;

  int getWidth() const { return width; }
  int getHeight() const { return height; }

  virtual const uint8_t* getPixelPtr(int x, int y) const = 0;
  const uint8_t*         getBuffer() const { return buffer.data(); }
  size_t                 getBufferSize() const { return buffer.size(); }
};

//
class Image : public ImageBase
{

public:
  ~Image() override = default;

  const uint8_t* getPixelPtr(int x, int y) const override { return &buffer[(y * width + x) * 3]; }

  void create(uint8_t* buff, int w, int h, int ch)
  {
    width  = w;
    height = h;
    buffer.resize(w * h * 3);
    for (int y = 0; y < h; y++)
    {
      for (int x = 0; x < w; x++)
      {
        int   base = y * w + x;
        auto* src  = &buff[base * ch];
        auto* dst  = &buffer[base * 3];
        int   r    = 0;
        int   g    = 0;
        int   b    = 0;
        if (ch == 3 || ch == 4)
        {
          r = src[0];
          g = src[1];
          b = src[2];
        }
        else
        {
          r = g = b = src[0];
        }
        dst[0] = r;
        dst[1] = g;
        dst[2] = b;
      }
    }
  }
};

//
class Image16 : public ImageBase
{

public:
  ~Image16() override = default;

  const uint8_t* getPixelPtr(int x, int y) const override { return &buffer[(y * width + x) * 2]; }

  void translate(const Image& src)
  {
    width  = src.getWidth();
    height = src.getHeight();
    buffer.resize(width * height * 2);
    for (int y = 0; y < height; y++)
    {
      for (int x = 0; x < width; x++)
      {
        auto* srcPixel = src.getPixelPtr(x, y);
        auto* dstPixel = &buffer[(y * width + x) * 2];
        int   r        = srcPixel[0] >> 3;
        int   g        = srcPixel[1] >> 2;
        int   b        = srcPixel[2] >> 3;
        int   px       = r << 11 | g << 5 | b;
        dstPixel[0]    = px & 0xff;
        dstPixel[1]    = px >> 8;
      }
    }
  }
  void translate(std::shared_ptr<Image> img) { translate(*img); }
};

//
std::shared_ptr<Image>
read_png(const char* fname)
{
  FILE* fp = fopen(fname, "rb");
  if (!fp)
  {
    std::cerr << "error: open failed" << std::endl;
    return {};
  }

  png_byte header[8];
  auto     hsize = sizeof(header);
  if (fread(header, 1, hsize, fp) != hsize)
  {
    std::cerr << "error: read error" << std::endl;
    return {};
  }

  auto is_png = !png_sig_cmp(header, 0, hsize);
  if (!is_png)
  {
    std::cerr << "error: not png" << std::endl;
    return {};
  }

  PNGPTR png;
  auto   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) nullptr, nullptr, nullptr);
  png.ptr        = png_ptr;
  if (!png_ptr)
  {
    std::cerr << "error: png error" << std::endl;
    return {};
  }

  auto info_ptr = png_create_info_struct(png_ptr);
  png.info      = info_ptr;
  if (!info_ptr)
  {
    std::cerr << "error: png info error" << std::endl;
    return {};
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, hsize);
  png_read_info(png_ptr, info_ptr);

  auto w = png_get_image_width(png_ptr, info_ptr);
  auto h = png_get_image_height(png_ptr, info_ptr);

  auto type = png_get_color_type(png_ptr, info_ptr);
  if (type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_RGB_ALPHA && type != PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    std::cerr << "error: no support" << std::endl;
    return {};
  }

  auto rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  auto channels = (int)png_get_channels(png_ptr, info_ptr);

  std::vector<png_byte>  img(rowbytes * h);
  std::vector<png_bytep> row_p(h);

  for (png_uint_32 i = 0; i < h; i++)
  {
    row_p[i] = &img[i * rowbytes];
  }
  png_read_image(png_ptr, row_p.data());

  auto image = std::make_shared<Image>();
  image->create(img.data(), w, h, channels);

  png_read_end(png_ptr, nullptr);

  std::cerr << "read success" << std::endl;
  return image;
}

//
bool
write_img(const char* fname, const Image16& img)
{
  std::cout << "write: " << fname << std::endl;
  std::ofstream file;
  file.open(fname, std::ios::binary | std::ios::trunc);
  int16_t w = img.getWidth();
  int16_t h = img.getHeight();
  file << "ARD0";
  auto write = [&](auto buff, size_t buffsize) { file.write(reinterpret_cast<const char*>(buff), buffsize); };
  write(&w, sizeof(w));
  write(&h, sizeof(h));
  size_t sz = img.getBufferSize();
  write(img.getBuffer(), sz);
  std::cout << "data size: " << sz << std::endl;

  std::cout << "write done: " << file.tellp() << std::endl;
  file.close();
  return true;
}

} // namespace

int
main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << argv[0] << ": <read.png> <write.bin>" << std::endl;
    return 1;
  }

  if (auto image = read_png(argv[1]))
  {
    Image16 img16;
    img16.translate(image);
    int ret = write_img(argv[2], img16) ? 0 : 1;
    return ret;
  }

  return 1;
}
