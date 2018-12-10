#! /bin/bash


nasm -felf64 print_rax.asm -o print_rax.o
ld -o hello hello.o
