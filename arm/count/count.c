#include <stdio.h>

void count(int limit){
    int i, total;
    __asm__ (
             "        MOV r0, #0\n\t"
             "        MOV r1, %[Rs1]\n\t"
             "loop:   \n\t"
             "        ADD r0, r0, r1\n\t"
             "        SUBS r1, r1, #1\n\t"
             "        BGE loop\n\t"
             "        MOV %[Rd], r0\n\t"
             : [Rd] "=r" (total)
             : [Rs1] "r" (limit)
             );
    printf("%d\n", total);
}
    
int main()
{
    count(10);
    return 0;
}
