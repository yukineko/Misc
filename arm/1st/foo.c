#include <stdio.h>

void foo(){
    int val1 = 1;
    int result;
    __asm__ (
             "MOV r0, %[Rs1]\n\t"
             "MOV %[Rd], r0"
             : [Rd] "=r" (result)
             : [Rs1] "r" (val1)
             : "r0"
             );
    printf("result = %d\n", result);
}

int main(){
    foo();
    return 0;
}
