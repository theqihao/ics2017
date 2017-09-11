#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };
  // printf("%08X\n", subcode);
  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
  // switch (subcode & 0xf) {
    case CC_O:
    case CC_NO:
    case CC_B: *dest = cpu.CF; break;
    case CC_NB: *dest = (cpu.CF == 0 ? 1 : 0); break;
    case CC_E: *dest = cpu.ZF; break; 
    case CC_NE: *dest = !cpu.ZF; break;
    case CC_BE: *dest = cpu.CF || cpu.ZF; break;
    case CC_NBE: *dest = (cpu.CF == 0) && (cpu.ZF == 0); break;
    case CC_S: *dest = cpu.SF; break;
    case CC_NS: *dest = (cpu.SF == 0 ? 1 : 0); break;
    case CC_NP:
    case CC_L: *dest = (cpu.OF == cpu.SF ? 0 : 1); break;
    case CC_NL: *dest = (cpu.OF == cpu.SF ? 1 : 0); break;
    case CC_LE: if (cpu.ZF == 1 || cpu.SF != cpu.OF) {
                  *dest = 1;
                } else {
                  *dest = 0;
                }
                break;
    case CC_NLE: if (cpu.ZF == 0 && cpu.SF == cpu.OF) {
                   *dest = 1;
                 } else {
                   *dest = 0;
                 }
                 break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
