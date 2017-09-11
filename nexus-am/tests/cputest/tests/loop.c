#include "trap.h"

int add(int a, int b) {
	int c = a + b;
	return c;
}

int main() {
	/*
	int i, j = 0;
	for (i = 0; i < 0x03ffffff; i++) {
		j = add(i, i);
		nemu_assert(j == 2*i);
	}
	*/
	return 0;
}
