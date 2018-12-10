#! /bin/bash


nasm -felf64 helloworld.asm -o hello.o
ld -o hello hello.o
