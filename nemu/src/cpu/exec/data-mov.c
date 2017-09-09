#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();
  rtl_pop((rtlreg_t *)&(cpu.gpr[id_dest->reg]));
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();
  rtl_sr_l(R_ESP, &cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    rtl_lr_w(&t0, R_AX);
    rtl_sext(&t0, &t0, 2);
    if ((int)t0 < 0) {
      rtl_li(&t1, 0xffff);
      rtl_sr_w(R_DX, &t1);
    } else {
      rtl_sr_w(R_DX, &tzero);
    }
  }
  else {
    // TODO();
    rtl_lr_l(&t0, R_EAX);
    if ((int)t0 < 0) {
      rtl_li(&t1, 0xffffffff);
      rtl_sr_l(R_EDX, &t1);
    } else {
      rtl_sr_l(R_EDX, &tzero);
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
