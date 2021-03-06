#----|-------entry-------|-errorcode-|---irq id---|---handler---|
.globl vecsys;    vecsys:  pushl $0;  pushl $0x80; jmp asm_trap
.globl trapsys;  trapsys:  pushl $0;  pushl $0x81; jmp asm_trap
.globl timesys;  timesys:  pushl $0;  pushl $0x20; jmp asm_trap
.globl vecnull;  vecnull:  pushl $0;  pushl   $-1; jmp asm_trap


asm_trap:
  pushal

  pushl %esp
  call irq_handle

  movl %eax, %esp

  #addl $4, %esp

  popal
  addl $8, %esp

  iret
