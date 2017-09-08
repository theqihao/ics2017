#include "cpu/exec.h"
#include "cpu/rtl.h"

make_EHelper(xchg) {
	if (decoding.is_operand_size_16 == true) {
		rtl_lr_w(&t0, id_dest->reg);
		rtl_lr_w(&t1, R_AX);
		rtl_sr_w(id_dest->reg, &t1);
		rtl_sr_w(R_AX, &t0);
	} else {
		rtl_lr_l(&t0, id_dest->reg);
		rtl_lr_l(&t1, R_AX);
		rtl_sr_l(id_dest->reg, &t1);
		rtl_sr_l(R_AX, &t0);
	}
	print_asm_template1(xchg);
}