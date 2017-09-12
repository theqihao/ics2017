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
  // TODO(); 
  // Push EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI
  t2 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t2);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t2);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
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
    // TODO();
    rtl_lr_b(&t1, R_AL);
    rtl_sext(&t2, &t1, 1);
    rtl_sr_w(R_AX, &t2);
  }
  else {
    // TODO();
    rtl_lr_w(&t1, R_AX);
    rtl_sext(&t2, &t1, 2);
    rtl_sr_l(R_EAX, &t2);
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
