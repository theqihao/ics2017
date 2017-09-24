#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);

  printf("pcb addr = %08X, ptr = %08X\n", &pcb[i], pcb[i].as.ptr);
}

// static uint32_t flag = 0;
bool current_game = 0;

_RegSet* schedule(_RegSet *prev) {

  // printf("schedule\n");
  // int i = 0;
  // for (i = 0; i < 2; i++) {
  //   printf("pcb addr = %08X, ptr = %08X\n", &pcb[i], pcb[i].as.ptr);
  // }

  // save the context pointer
  current->tf = prev;

  // always select pcb[0] as the new process
  current = &pcb[0];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // flag++;
  // if (current_game == 0) {
  //   if (flag % 0x100 == 0) {
  //     current = &pcb[1];
  //   } else {
  //     current = &pcb[0];
  //   }
  // } else {
  //   if (flag % 0x100 == 0) {
  //     current = &pcb[1];
  //   } else {
  //     current = &pcb[2];
  //   }
  // }


  // TODO: switch to the new address space,
  // then return the new context
  // printf("schedule pcb addr = %08X, ptr = %08X\n", current, current->as.ptr);
  _switch(&current->as);
  // printf("schedule pcb addr = %08X, ptr = %08X\n", current, current->as.ptr);

  return current->tf;
}
