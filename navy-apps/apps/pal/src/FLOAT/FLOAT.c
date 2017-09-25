#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  // assert(0);
  /*
  FLOAT up = (((a >> 16) * (b >> 16)) << 16);// + (((((a&0xffff)*(b&0xffff)) >> 16) & 0xffff0000));
  FLOAT down = (((a*b) >> 16) & 0x0000ffff);
  FLOAT ans = up | down;
  */
  /*
  FLOAT ans = (a >> 8) * (b >> 8);
  return ans;
  */
  
  long long A = a;
  long long B = b;
  long long C = A*B;
  FLOAT res = (C >> 16);
  return res;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  volatile FLOAT x1 = a >> 16;
  volatile FLOAT x2 = a << 16;
  FLOAT y;
  FLOAT temp;
  asm volatile ("idivl %2" : "=a"(y), "=d"(temp) :"r"(b), "a"(x2), "d"(x1));
  return y;

  // assert(0);
  FLOAT ans = (a/b) << 16;
  return ans;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  // assert(0);
  unsigned uf = *(unsigned *)(&a);
  unsigned sign = uf >> 31;  //取符号位
  int exp = (uf >>23) & 0xFF;   //取阶码
  unsigned frac = uf & 0x7FFFFF;  //取尾数


  exp -= 127;
  if(exp > 30 || exp == 0xFF) //超出int表示范围
     return 0x80000000u;
  if(exp < 0)
     return 0;

  int ans = 0;
  frac |= (1 << 23); //frac前面填上1
  //printf("%08X\n", frac);

  int shift = (23 - exp) - 16;
  if (shift >= 0) {
    ans = frac >> shift;
  } else {
    frac = -frac;
    ans = frac << shift;
    ans &= 0x7fffffff;
  }

  /*
  if(exp >= 23)  {
    // printf("exp > 23\n");
    frac <<= (exp - 23);
  } else if(exp >= 0) {
    if (23 - exp >= 16) {
        // ans = ((frac >> (23 - exp)) << 16) | ((frac >> (23 - exp - 16)) & 0x0000ffff);
        ans = frac >>= (7 - exp);
    } else {
        ans = ((frac >> (23 - exp)) << 16) | ((frac << (16 - (23 - exp))) & 0x0000ffff);
    }
  }
  */
  //printf("ans = %08X\n", ans);
  if(sign == 1) //判断是否为负数
     ans = (~ans) + 1;
  return ans;
}

FLOAT Fabs(FLOAT a) {
  // assert(0);
  FLOAT ans = 0;
  if ((a & 0x80000000) == 0) {
    ans = a;
  } else {
    ans = -a;
  }
  return ans;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
