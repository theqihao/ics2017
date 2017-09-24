#include "cpu/exec.h"
#include "memory/mmu.h"
/*
依次将EFLAGS, CS, EIP寄存器的值压入堆栈
从IDTR中读出IDT的首地址
根据异常(中断)号在IDT中进行索引, 找到一个门描述符
将门描述符中的offset域组合成目标地址
跳转到目标地址
*/
void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // TODO();
  //printf("raise_intr\n");
	rtl_push(&cpu.flags);  cpu.IF = 0;
	rtl_push(&cpu.cs);
	rtl_li(&t2, ret_addr);
	rtl_push(&t2);
	rtl_li(&t2, cpu.idtr);
	//printf("cpu.idtr = %08x, no = %08x\n", cpu.idtr, NO);
	rtl_addi(&t2, &t2, NO*8);
	rtl_lm(&t3, &t2, 4);
	//printf("t3 = %08x\n", t3);
	rtl_addi(&t2, &t2, 4);
	rtl_lm(&t1, &t2, 4);
	//t0 = (((t2 & 0xff000000) >> 16) | (t2 & 0x000000ff)) & 0x0000ffff;
	//t0 = t2 & 0x0000ffff;
	//printf("t1 = %08x\n", t1);
	t0 = ((t1 & 0xffff0000) | (t3 & 0x0000ffff));
	//printf("t0 = %08x\n", t0);
  decoding.jmp_eip = t0;
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
	cpu.INTR = 0x1;
}
