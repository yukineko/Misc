#include <stdio.h>
#include <stdint.h>

#define SHIFT 28

char* display_flags(uint32_t flags){
    static char buffer[256];
    sprintf(buffer, "n=%d, z=%d, c=%d, v=%d", (flags & 0b1000) >> 3, (flags & 0b0100) >> 2, (flags & 0b0010) >> 1, flags & 0b0001);
    return buffer;
}


void arm_add_sample1(){
    uint32_t cpsr1, cpsr2, rd1, rd2;
    __asm__ (
             "MOV   r0, #0          \n\t"
             "MSR   cpsr_f, r0      \n\t"
             "LDR   r0, =1          \n\t"
             "LDR   r1, =-1         \n\t"
             "ADD   r2, r0, r1      \n\t"
             "MOV   %[Rd1], r2     \n\t"
             "MRS   %[CPSR1], cpsr  \n\t"
             
             "LDR   r0, =1          \n\t"
             "LDR   r1, =2          \n\t"
             "ADD   r3, r0, r1      \n\t"
             "MOV   %[Rd2], r3      \n\t"
             "MRS   %[CPSR2], cpsr  \n\t"

             : [Rd1] "=r" (rd1), [Rd2] "=r" (rd2), [CPSR1] "=r" (cpsr1), [CPSR2] "=r" (cpsr2)
             :
             : "r0","r1","r2","r3"
             );
    printf("r1 %d\n", rd1);
    printf("r2 %d\n", rd2);
    printf("flang1: %s\n", display_flags(cpsr1 >> SHIFT));
    printf("flang2: %s\n", display_flags(cpsr2 >> SHIFT));    
}


void arm_add_sample2(){
    uint32_t cpsr1, cpsr2, rd1, rd2;
    __asm__ (
             "MOV   r0, #0          \n\t"
             "MSR   cpsr_f, r0      \n\t"
             "LDR   r0, =1          \n\t"
             "LDR   r1, =-1         \n\t"
             "ADDS  r2, r0, r1      \n\t"
             "MOV   %[Rd1], r2     \n\t"
             "MRS   %[CPSR1], cpsr  \n\t"
             
             "LDR   r0, =1          \n\t"
             "LDR   r1, =2          \n\t"
             "ADDS  r3, r0, r1      \n\t"
             "MOV   %[Rd2], r3      \n\t"
             "MRS   %[CPSR2], cpsr  \n\t"

             : [Rd1] "=r" (rd1), [Rd2] "=r" (rd2), [CPSR1] "=r" (cpsr1), [CPSR2] "=r" (cpsr2)
             :
             : "r0","r1","r2","r3"
             );
    printf("r1 %d\n", rd1);
    printf("r2 %d\n", rd2);
    printf("flang1: %s\n", display_flags(cpsr1 >> SHIFT));
    printf("flang2: %s\n", display_flags(cpsr2 >> SHIFT));    
}

void arm_add_sample3(){
    uint32_t cpsr, d;
    __asm__ (
             "MRS r0, cpsr              \n\t"
             "ORR r0, r0, #0x20000000   \n\t"
             
             "LDR r0, =1                \n\t"
             "LDR r1, =2                \n\t"
             "ADC r2, r0, r1            \n\t"
             "MOV %[Rd], r2             \n\t"
             : [CPSR] "=r" (cpsr), [Rd] "=r" (d)
             :
             : "r0", "r1", "r2", "r3"
             );
    printf("r2 %d\n", d);
    printf("flang1: %s\n", display_flags(cpsr >> SHIFT));

}



int main(){
    arm_add_sample1();
    printf("\n");
    arm_add_sample2();
    printf("\n");
    arm_add_sample3();    
    return 0;
}
