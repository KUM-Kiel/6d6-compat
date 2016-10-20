#ifndef NUMBER_H
#define NUMBER_H

#include <stdint.h>

#define U ((uint8_t *) x)
#define I ((int8_t *) x)

#define LD2(t, b0, b1) ((((t) (b0)) << 8) | ((t) (b1)))
#define LD4(t, b0, b1, b2, b3) ((LD2(t, b0, b1) << 16) | LD2(t, b2, b3))
#define LD8(t, b0, b1, b2, b3, b4, b5, b6, b7) ((LD4(t, b0, b1, b2, b3) << 32) | LD4(t, b4, b5, b6, b7))

/* Read signed or unsigned 16, 32 or 64 bit integers
 * from a buffer in big endian (aka Motorola format). */
static inline   int8_t  ld_i8_be(const void *x){return I[0];}
static inline  int16_t ld_i16_be(const void *x){return LD2(int16_t, I[0], U[1]);}
static inline  int32_t ld_i32_be(const void *x){return LD4(int32_t, I[0], U[1], U[2], U[3]);}
static inline  int64_t ld_i64_be(const void *x){return LD8(int64_t, I[0], U[1], U[2], U[3], U[4], U[5], U[6], U[7]);}
static inline  uint8_t  ld_u8_be(const void *x){return U[0];}
static inline uint16_t ld_u16_be(const void *x){return LD2(uint16_t, U[0], U[1]);}
static inline uint32_t ld_u32_be(const void *x){return LD4(uint32_t, U[0], U[1], U[2], U[3]);}
static inline uint64_t ld_u64_be(const void *x){return LD8(uint64_t, U[0], U[1], U[2], U[3], U[4], U[5], U[6], U[7]);}

/* Read signed or unsigned 16, 32 or 64 bit integers
 * from a buffer in little endian (aka Intel format). */
static inline   int8_t  ld_i8_le(const void *x){return I[0];}
static inline  int16_t ld_i16_le(const void *x){return LD2(int16_t, I[1], U[0]);}
static inline  int32_t ld_i32_le(const void *x){return LD4(int32_t, I[3], U[2], U[1], U[0]);}
static inline  int64_t ld_i64_le(const void *x){return LD8(int64_t, I[7], U[6], U[5], U[4], U[3], U[2], U[1], U[0]);}
static inline  uint8_t  ld_u8_le(const void *x){return U[0];}
static inline uint16_t ld_u16_le(const void *x){return LD2(uint16_t, U[1], U[0]);}
static inline uint32_t ld_u32_le(const void *x){return LD4(uint32_t, U[3], U[2], U[1], U[0]);}
static inline uint64_t ld_u64_le(const void *x){return LD8(uint64_t, U[7], U[6], U[5], U[4], U[3], U[2], U[1], U[0]);}

#define ST2(v, b0, b1) ((b0) = ((v) >> 8) % 256, (b1) = ((v) % 256))
#define ST4(v, b0, b1, b2, b3) (ST2(v >> 16, b0, b1), ST2(v, b2, b3))
#define ST8(v, b0, b1, b2, b3, b4, b5, b6, b7) (ST4(v >> 32, b0, b1, b2, b3), ST4(v, b4, b5, b6, b7))

/* Write signed or unsigned 16, 32 or 64 bit integers
 * to a buffer in big endian. */
static inline void  st_i8_be(void *x,   int8_t i){I[0] = i;}
static inline void st_i16_be(void *x,  int16_t i){ST2(i, I[0], U[1]);}
static inline void st_i32_be(void *x,  int32_t i){ST4(i, I[0], U[1], U[2], U[3]);}
static inline void st_i64_be(void *x,  int64_t i){ST8(i, I[0], U[1], U[2], U[3], U[4], U[5], U[6], U[7]);}
static inline void  st_u8_be(void *x,  uint8_t u){U[0] = u;}
static inline void st_u16_be(void *x, uint16_t u){ST2(u, U[0], U[1]);}
static inline void st_u32_be(void *x, uint32_t u){ST4(u, U[0], U[1], U[2], U[3]);}
static inline void st_u64_be(void *x, uint64_t u){ST8(u, U[0], U[1], U[2], U[3], U[4], U[5], U[6], U[7]);}

/* Write signed or unsigned 16, 32 or 64 bit integers
 * to a buffer in little endian. */
static inline void  st_i8_le(void *x,   int8_t i){I[0] = i;}
static inline void st_i16_le(void *x,  int16_t i){ST2(i, I[1], U[0]);}
static inline void st_i32_le(void *x,  int32_t i){ST4(i, I[3], U[2], U[1], U[0]);}
static inline void st_i64_le(void *x,  int64_t i){ST8(i, I[7], U[6], U[5], U[4], U[3], U[2], U[1], U[0]);}
static inline void  st_u8_le(void *x,  uint8_t u){U[0] = u;}
static inline void st_u16_le(void *x, uint16_t u){ST2(u, U[1], U[0]);}
static inline void st_u32_le(void *x, uint32_t u){ST4(u, U[3], U[2], U[1], U[0]);}
static inline void st_u64_le(void *x, uint64_t u){ST8(u, U[7], U[6], U[5], U[4], U[3], U[2], U[1], U[0]);}

#endif
