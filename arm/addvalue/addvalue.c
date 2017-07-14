#include <stdio.h>
int main(){

    int a = 1, b = 2, total;
    __asm__ (
             "        MOV r0, %[Rs1]\n\t"
             "        MOV r1, %[Rs2]\n\t"
             "        BL addvalues  \n\t"
             "        MOV %[Rd], r0\ n\t"
             "        B finish      \n\t"
             "addvalues:            \n\t"
             "        ADD r0, r0, 01\n\t"
             "        BX lr         \n\r"
             "finish:               \n\t"
             : [Rd] "=r" (total)
             : [Rs1] "r" (a), [Rs2] "r" (b)
             : "r0", "r1", "lr"
             );
    printf("%d\n", total);
    return 0;
}
