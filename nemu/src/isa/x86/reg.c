#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include "local-include/reg.h"

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  word_t sample[8];
  word_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  for(int i=0;i<8;i++){
    printf("%s:0x%x\n",regsl[i],cpu.gpr[i]._32);
  }
  for(int i=0;i<8;i++){
    printf("%s:0x%x\n",regsw[i],cpu.gpr[i]._16);
  }
  for(int i=0;i<8;i++){
    printf("%s:0x%x\n",regsb[i],(i<4)?cpu.gpr[i]._8[0]:cpu.gpr[i]._8[1]);
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  for(int i=0;i<8;i++){
    if(strcmp(s,regsl[i])==0) return cpu.gpr[i]._32;
  }
  for(int i=0;i<8;i++){
    if(strcmp(s,regsw[i])==0) return cpu.gpr[i]._16;
  }
  for(int i=0;i<8;i++){
    if(strcmp(s,regsb[i])==0) return (i<4)?cpu.gpr[i]._8[0]:cpu.gpr[i]._8[1];
  }
  *success=false;
  return 0;
}
