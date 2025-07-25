/* Generated by https://github.com/corsix/fast-crc32/ using: */
/* ./generate -i neon_eor3 -p crc32c -a v8s2x4_s3 */
/* MIT licensed */

#include "folly/external/fast-crc32/neon_eor3_crc32c_v8s2x4_s3.h"
#include <folly/system/AuxVector.h>
#include <folly/Portability.h>

#include <stdint.h>
#include <stddef.h>

#define CRC_EXPORT extern

namespace folly::detail {
[[maybe_unused]] static ElfHwCaps hwcaps() {
  static ElfHwCaps caps;
  return caps;
}
}

#if !(FOLLY_AARCH64 && FOLLY_NEON && FOLLY_ARM_FEATURE_CRYPTO && FOLLY_ARM_FEATURE_CRC32 && FOLLY_ARM_FEATURE_SHA3)
#include <stdlib.h>
namespace folly::detail {
CRC_EXPORT uint32_t neon_eor3_crc32c_v8s2x4_s3(const uint8_t*, size_t, uint32_t) {
  abort(); // not implemented on this platform
}

CRC_EXPORT bool has_neon_eor3_crc32c_v8s2x4_s3() {
  return false;
}
}
#else
#include <arm_acle.h>
#include <arm_neon.h>

#if defined(_MSC_VER)
#define CRC_AINLINE static __forceinline
#define CRC_ALIGN(n) __declspec(align(n))
#else
#define CRC_AINLINE static __inline __attribute__((always_inline))
#define CRC_ALIGN(n) __attribute__((aligned(n)))
#endif

namespace folly::detail {
CRC_AINLINE uint64x2_t clmul_lo(uint64x2_t a, uint64x2_t b) {
  return vreinterpretq_u64_p128(vmull_p64(a[0], b[0]));
}

CRC_AINLINE uint64x2_t clmul_hi(uint64x2_t a, uint64x2_t b) {
  return vreinterpretq_u64_p128(vmull_high_p64(vreinterpretq_p64_u64(a), vreinterpretq_p64_u64(b)));
}

CRC_AINLINE uint64x2_t clmul_scalar(uint32_t a, uint32_t b) {
  return vreinterpretq_u64_p128(vmull_p64(a, b));
}

static uint32_t xnmodp(uint64_t n) /* x^n mod P, in log(n) time */ {
  uint64_t stack = ~(uint64_t)1;
  uint32_t acc, low;
  for (; n > 191; n = (n >> 1) - 16) {
    stack = (stack << 1) + (n & 1);
  }
  stack = ~stack;
  acc = ((uint32_t)0x80000000) >> (n & 31);
  for (n >>= 5; n; --n) {
    acc = __crc32cw(acc, 0);
  }
  while ((low = stack & 1), stack >>= 1) {
    poly8x8_t x = vreinterpret_p8_u64(vmov_n_u64(acc));
    uint64_t y = vgetq_lane_u64(vreinterpretq_u64_p16(vmull_p8(x, x)), 0);
    acc = __crc32cd(0, y << low);
  }
  return acc;
}

CRC_AINLINE uint64x2_t crc_shift(uint32_t crc, size_t nbytes) {
  return clmul_scalar(crc, xnmodp(nbytes * 8 - 33));
}

FOLLY_TARGET_ATTRIBUTE("+crc")
CRC_EXPORT bool has_neon_eor3_crc32c_v8s2x4_s3() {
  auto caps = hwcaps();

  return caps.aarch64_fp() && caps.aarch64_asimd() && caps.aarch64_pmull() &&
      caps.aarch64_crc32() && caps.aarch64_sha3();
}

CRC_EXPORT uint32_t neon_eor3_crc32c_v8s2x4_s3(const uint8_t* buf, size_t len, uint32_t crc0) {
  for (; len && ((uintptr_t)buf & 7); --len) {
    crc0 = __crc32cb(crc0, *buf++);
  }
  if (((uintptr_t)buf & 8) && len >= 8) {
    crc0 = __crc32cd(crc0, *(const uint64_t*)buf);
    buf += 8;
    len -= 8;
  }
  if (len >= 192) {
    size_t blk = (len - 0) / 192;
    size_t klen = blk * 32;
    const uint8_t* buf2 = buf + klen * 2;
    uint32_t crc1 = 0;
    uint64x2_t vc0;
    uint64x2_t vc1;
    uint64_t vc;
    /* First vector chunk. */
    uint64x2_t x0 = vld1q_u64((const uint64_t*)buf2), y0;
    uint64x2_t x1 = vld1q_u64((const uint64_t*)(buf2 + 16)), y1;
    uint64x2_t x2 = vld1q_u64((const uint64_t*)(buf2 + 32)), y2;
    uint64x2_t x3 = vld1q_u64((const uint64_t*)(buf2 + 48)), y3;
    uint64x2_t x4 = vld1q_u64((const uint64_t*)(buf2 + 64)), y4;
    uint64x2_t x5 = vld1q_u64((const uint64_t*)(buf2 + 80)), y5;
    uint64x2_t x6 = vld1q_u64((const uint64_t*)(buf2 + 96)), y6;
    uint64x2_t x7 = vld1q_u64((const uint64_t*)(buf2 + 112)), y7;
    uint64x2_t k;
    { static const uint64_t CRC_ALIGN(16) k_[] = {0x6992cea2, 0x0d3b6092}; k = vld1q_u64(k_); }
    buf2 += 128;
    len -= 192;
    /* Main loop. */
    while (len >= 192) {
      y0 = clmul_lo(x0, k), x0 = clmul_hi(x0, k);
      y1 = clmul_lo(x1, k), x1 = clmul_hi(x1, k);
      y2 = clmul_lo(x2, k), x2 = clmul_hi(x2, k);
      y3 = clmul_lo(x3, k), x3 = clmul_hi(x3, k);
      y4 = clmul_lo(x4, k), x4 = clmul_hi(x4, k);
      y5 = clmul_lo(x5, k), x5 = clmul_hi(x5, k);
      y6 = clmul_lo(x6, k), x6 = clmul_hi(x6, k);
      y7 = clmul_lo(x7, k), x7 = clmul_hi(x7, k);
      x0 = veor3q_u64(x0, y0, vld1q_u64((const uint64_t*)buf2));
      x1 = veor3q_u64(x1, y1, vld1q_u64((const uint64_t*)(buf2 + 16)));
      x2 = veor3q_u64(x2, y2, vld1q_u64((const uint64_t*)(buf2 + 32)));
      x3 = veor3q_u64(x3, y3, vld1q_u64((const uint64_t*)(buf2 + 48)));
      x4 = veor3q_u64(x4, y4, vld1q_u64((const uint64_t*)(buf2 + 64)));
      x5 = veor3q_u64(x5, y5, vld1q_u64((const uint64_t*)(buf2 + 80)));
      x6 = veor3q_u64(x6, y6, vld1q_u64((const uint64_t*)(buf2 + 96)));
      x7 = veor3q_u64(x7, y7, vld1q_u64((const uint64_t*)(buf2 + 112)));
      crc0 = __crc32cd(crc0, *(const uint64_t*)buf);
      crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen));
      crc0 = __crc32cd(crc0, *(const uint64_t*)(buf + 8));
      crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen + 8));
      crc0 = __crc32cd(crc0, *(const uint64_t*)(buf + 16));
      crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen + 16));
      crc0 = __crc32cd(crc0, *(const uint64_t*)(buf + 24));
      crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen + 24));
      buf += 32;
      buf2 += 128;
      len -= 192;
    }
    /* Reduce x0 ... x7 to just x0. */
    { static const uint64_t CRC_ALIGN(16) k_[] = {0xf20c0dfe, 0x493c7d27}; k = vld1q_u64(k_); }
    y0 = clmul_lo(x0, k), x0 = clmul_hi(x0, k);
    y2 = clmul_lo(x2, k), x2 = clmul_hi(x2, k);
    y4 = clmul_lo(x4, k), x4 = clmul_hi(x4, k);
    y6 = clmul_lo(x6, k), x6 = clmul_hi(x6, k);
    x0 = veor3q_u64(x0, y0, x1);
    x2 = veor3q_u64(x2, y2, x3);
    x4 = veor3q_u64(x4, y4, x5);
    x6 = veor3q_u64(x6, y6, x7);
    { static const uint64_t CRC_ALIGN(16) k_[] = {0x3da6d0cb, 0xba4fc28e}; k = vld1q_u64(k_); }
    y0 = clmul_lo(x0, k), x0 = clmul_hi(x0, k);
    y4 = clmul_lo(x4, k), x4 = clmul_hi(x4, k);
    x0 = veor3q_u64(x0, y0, x2);
    x4 = veor3q_u64(x4, y4, x6);
    { static const uint64_t CRC_ALIGN(16) k_[] = {0x740eef02, 0x9e4addf8}; k = vld1q_u64(k_); }
    y0 = clmul_lo(x0, k), x0 = clmul_hi(x0, k);
    x0 = veor3q_u64(x0, y0, x4);
    /* Final scalar chunk. */
    crc0 = __crc32cd(crc0, *(const uint64_t*)buf);
    crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen));
    crc0 = __crc32cd(crc0, *(const uint64_t*)(buf + 8));
    crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen + 8));
    crc0 = __crc32cd(crc0, *(const uint64_t*)(buf + 16));
    crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen + 16));
    crc0 = __crc32cd(crc0, *(const uint64_t*)(buf + 24));
    crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen + 24));
    vc0 = crc_shift(crc0, klen + blk * 128);
    vc1 = crc_shift(crc1, 0 + blk * 128);
    vc = vgetq_lane_u64(veorq_u64(vc0, vc1), 0);
    /* Reduce 128 bits to 32 bits, and multiply by x^32. */
    crc0 = __crc32cd(0, vgetq_lane_u64(x0, 0));
    crc0 = __crc32cd(crc0, vc ^ vgetq_lane_u64(x0, 1));
    buf = buf2;
  }
  if (len >= 32) {
    size_t klen = ((len - 8) / 24) * 8;
    uint32_t crc1 = 0;
    uint32_t crc2 = 0;
    uint64x2_t vc0;
    uint64x2_t vc1;
    uint64_t vc;
    /* Main loop. */
    do {
      crc0 = __crc32cd(crc0, *(const uint64_t*)buf);
      crc1 = __crc32cd(crc1, *(const uint64_t*)(buf + klen));
      crc2 = __crc32cd(crc2, *(const uint64_t*)(buf + klen * 2));
      buf += 8;
      len -= 24;
    } while (len >= 32);
    vc0 = crc_shift(crc0, klen * 2 + 8);
    vc1 = crc_shift(crc1, klen + 8);
    vc = vgetq_lane_u64(veorq_u64(vc0, vc1), 0);
    /* Final 8 bytes. */
    buf += klen * 2;
    crc0 = crc2;
    crc0 = __crc32cd(crc0, *(const uint64_t*)buf ^ vc), buf += 8;
    len -= 8;
  }
  for (; len >= 8; buf += 8, len -= 8) {
    crc0 = __crc32cd(crc0, *(const uint64_t*)buf);
  }
  for (; len; --len) {
    crc0 = __crc32cb(crc0, *buf++);
  }
  return crc0;
}
} // namespace folly::detail
#endif
