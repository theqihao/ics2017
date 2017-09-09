#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

/* ----pa2.1---- */
make_EHelper(call);
make_EHelper(push);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(pop);
make_EHelper(ret);


/* ----pa2.2---- */
/* add.c */
make_EHelper(and);
make_EHelper(xchg);
make_EHelper(lea);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(inc);

/* add-longlong */
make_EHelper(adc);
make_EHelper(or);

/* bubble-sort */
make_EHelper(dec);
make_EHelper(jmp);
make_EHelper(leave);

/* shuixianhuashu */
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);
make_EHelper(sar);
make_EHelper(cltd);

/* load-store */
make_EHelper(movsx);
make_EHelper(shl);
make_EHelper(not);

/* sub-longlong */
make_EHelper(sbb);

/* recursion */
make_EHelper(call_rm);
make_EHelper(jmp_rm);
make_EHelper(shr);