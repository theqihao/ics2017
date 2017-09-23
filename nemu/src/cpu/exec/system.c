#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  // TODO();
  // printf("lidt\n");
  // rtl_lm(&cpu.idtr, &id_dest->addr, id_dest->width);
  rtl_li(&cpu.idtr, id_dest->val);
  //printf("id_dest->addr = %08x, id_dest->val  = %08x\n", id_dest->addr, id_dest->val);
  //printf("cpu.idtr = %08x, t2  = %08x\n", cpu.idtr, t2);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();
  if (id_dest->reg == 0) {
    cpu.cr0 = id_src->val;
  } else if (id_dest->reg == 3) {
    cpu.cr3 = id_src->val;
  } else {
    assert(0);
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  if (id_src->reg == 0) {
    operand_write(id_dest, &cpu.cr0);
  } else if (id_src->reg == 3) {
    operand_write(id_dest, &cpu.cr3);
  } else {
    assert(0);
  }

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  // TODO();
  raise_intr(id_dest->val, cpu.eip+2);
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  // TODO();
  decoding.is_jmp = 1;

  rtl_pop(&t2);
  decoding.jmp_eip = t2;
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.flags);

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  // TODO();
  t2 = pio_read(id_src->val, id_dest->width);
  operand_write(id_dest, &t2);
  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  // TODO();
  pio_write(id_dest->val, id_src->width, id_src->val);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
