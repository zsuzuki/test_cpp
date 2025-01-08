//
// Copyright 2024 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <arm_neon.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <simd/geometry.h>
#include <simd/matrix.h>
#include <simd/vector.h>
#include <vector>

//
//
//
[[maybe_unused]] void
dumpVec32x4(const char* cap, float32x4_t v)
{
  auto x = vgetq_lane_f32(v, 0);
  auto y = vgetq_lane_f32(v, 1);
  auto z = vgetq_lane_f32(v, 2);
  auto w = vgetq_lane_f32(v, 3);
  std::cout << "[" << cap << "]: X=" << x;
  std::cout << " Y=" << y << " Z=" << z << " W=" << w << std::endl;
}

[[maybe_unused]] void
dumpVec64x2(const char* cap, float64x2_t v)
{
  auto x = vgetq_lane_f64(v, 0);
  auto y = vgetq_lane_f64(v, 1);
  std::cout << "[" << cap << "]: " << x << ", " << y << std::endl;
}

[[maybe_unused]] void
dumpVec16x4(const char* cap, float16x4_t v)
{
  auto v8 = vcombine_f16(v, v);
  auto x  = vgetq_lane_f16(v8, 0);
  auto y  = vgetq_lane_f16(v8, 1);
  auto z  = vgetq_lane_f16(v8, 2);
  auto w  = vgetq_lane_f16(v8, 3);
  std::cout << "[" << cap << "]: " << x << ", " << y << ", " << z << ", " << w << std::endl;
}

[[maybe_unused]] void
dumpVecIntx4(const char* cap, int32x4_t v)
{
  auto x = vgetq_lane_s32(v, 0);
  auto y = vgetq_lane_s32(v, 1);
  auto z = vgetq_lane_s32(v, 2);
  auto w = vgetq_lane_s32(v, 3);
  std::cout << "[" << cap << "]: X=" << x;
  std::cout << " Y=" << y << " Z=" << z << " W=" << w << std::endl;
}

//
//
//
float
dotProduct(float x1, float y1, float z1, float x2, float y2, float z2)
{
  return x1 * x2 + y1 * y2 + z1 * z2;
}

float
dotProductNeon(float x1, float y1, float z1, float x2, float y2, float z2)
{
  float32x4_t v1{x1, y1, z1, 0.0f};
  float32x4_t v2{x2, y2, z2, 0.0f};

  auto dp = vmulq_f32(v1, v2);
  dp      = vpaddq_f32(dp, dp);
  dp      = vpaddq_f32(dp, dp);
  return vgetq_lane_f32(dp, 0);
}

double
dotProductNeonD(double x1, double y1, double z1, double x2, double y2, double z2)
{
  float64x2x2_t v1{x1, y1, z1, 0};
  float64x2x2_t v2{x2, y2, z2, 0};

  auto hi = vmulq_f64(v1.val[0], v2.val[0]);
  auto lo = vmulq_f64(v1.val[1], v2.val[1]);
  auto s1 = vpaddq_f64(hi, lo);
  auto r  = vpaddq_f64(s1, s1);
  return vgetq_lane_f64(r, 0);
}

float16x4_t
convertf16(float x, float y, float z, float w)
{
  float32x4_t v{x, y, z, w};
  return vcvt_f16_f32(v);
  //   vcvt_f32_f16(vget_low_f16(vreinterpretq_s16_f16(v)))
}

float16_t
dotProductNeonH(float16x4_t a, float16x4_t b)
{
  auto n  = vmul_f16(a, b);
  auto nc = vcombine_f16(n, n);
  nc      = vpaddq_f16(nc, nc);
  nc      = vpaddq_f16(nc, nc);
  return vget_lane_f16(vget_low_f16(nc), 0);
}

int
main(int argc, char** argv)
{
  float x1 = 1.0f;
  float y1 = 3.0f;
  float z1 = 5.0f;
  float x2 = 2.0f;
  float y2 = 4.0f;
  float z2 = 6.0f;
  std::cout << "normal: " << dotProduct(x1, y1, z1, x2, y2, z2) << std::endl;
  std::cout << "neon: " << dotProductNeon(x1, y1, z1, x2, y2, z2) << std::endl;
  std::cout << "neonD: " << dotProductNeonD(x1, y1, z1, x2, y2, z2) << std::endl;

  auto a16 = convertf16(x1, y1, z1, 0.0f);
  auto b16 = convertf16(x2, y2, z2, 0.0f);
  std::cout << "neonH: " << dotProductNeonH(a16, b16) << std::endl;

  float32x4_t aa{x1, y1, z1, 0.0f};
  auto        pan = vpaddq_f32(aa, aa);
  pan             = vpaddq_f32(pan, pan);
  dumpVec32x4("PADD", pan);

  int32x4_t n{-1, 0x01234567, -0x01234567, 0x00000000};
  dumpVecIntx4("Int", n);
  auto np = vshrq_n_s32(n, 31);
  dumpVecIntx4("Int", np);
  auto nm = vmovq_n_s32(1);
  auto mv = vandq_s32(np, nm);
  dumpVecIntx4("And", mv);
  auto bit0 = vgetq_lane_s32(mv, 0);
  auto bit1 = vgetq_lane_s32(mv, 1);
  auto bit2 = vgetq_lane_s32(mv, 2);
  auto bit3 = vgetq_lane_s32(mv, 3);
  auto mask = bit0 | (bit1 << 1) | (bit2 << 2) | (bit3 << 3);
  std::cout << "Mask: " << std::hex << mask << std::dec << std::endl;

  auto clz = vclzq_s32(n);
  dumpVecIntx4("Clz", clz);
  auto sbt = vpaddlq_s16(vpaddlq_s8(vcntq_s8(vreinterpretq_s8_s32(n))));
  dumpVecIntx4("Bit", sbt);
  auto tsb  = vmovq_n_s32(0x1f);
  auto sbt2 = vpaddlq_s16(vpaddlq_s8(vcntq_s8(vreinterpretq_s8_s32(tsb))));
  dumpVecIntx4("Bit", sbt2);

#define SAME_CHECK(a, b)                             \
  {                                                  \
    auto exp = (std::is_same_v<a, b>) ? "==" : "!="; \
    std::cout << #a << exp << #b << std::endl;       \
  }

  SAME_CHECK(size_t, uint64_t);
  SAME_CHECK(size_t, unsigned int);
  SAME_CHECK(size_t, unsigned long);
  SAME_CHECK(uint64_t, unsigned int);
  SAME_CHECK(uint64_t, unsigned long long);

  std::vector<uint64_t> vec64{0, 1, 2, 3, 4, 5};
  std::vector<long>&    vecl = reinterpret_cast<std::vector<long>&>(vec64);
  std::vector<long>*    vecp = (std::vector<long>*)(&vec64);

  for (auto lv : vecl)
  {
    std::cout << lv << std::endl;
  }
  for (auto lv : *vecp)
  {
    std::cout << lv << std::endl;
  }

  std::cout << "SIMD utility" << std::endl;
  auto eye    = simd_make_float3(10.0f, 0.0f, 0.0f);
  auto tgt    = simd_make_float3(0.0f, 0.0f, 0.0f);
  auto up     = simd_make_float3(0.0f, 1.0f, 0.0f);
  auto frontV = simd_normalize(tgt - eye);
  auto sideV  = simd_normalize(simd_cross(up, frontV));
  auto upV    = simd_normalize(simd_cross(frontV, sideV));

  simd::float4x4 viewMtx{};
  viewMtx.columns[0] = simd_make_float4(sideV[0], sideV[1], sideV[2], 0.0f);
  viewMtx.columns[1] = simd_make_float4(upV[0], upV[1], upV[2], 0.0f);
  viewMtx.columns[2] = simd_make_float4(frontV[0], frontV[1], frontV[2], 0.0f);
  viewMtx            = simd_transpose(viewMtx);
  viewMtx.columns[3] = simd_mul(viewMtx, simd_make_float4(-eye[0], -eye[1], -eye[2], 1.0f));
  for (auto& c : viewMtx.columns)
  {
    std::cout << " " << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << std::endl;
  }
  auto pos0 = simd_mul(viewMtx, simd_make_float4(0, 0, 10, 1));
  std::cout << "pos: " << pos0[0] << ", " << pos0[1] << ", " << pos0[2] << std::endl;

  std::cout << "SizeT:" << sizeof(size_t) << "," << sizeof(long) << std::endl;

  std::filesystem::path tp{"/file/./src/test.cpp"};
  std::cout << "path: " << tp.lexically_normal() << std::endl;

  return 0;
}
