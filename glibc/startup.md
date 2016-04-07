
言うまでもないことですがmainに至る道の話は鵜飼さんが以前からネタにしていて
http://ukai.jp/debuan/2002w/elf.html
でも見られますしBinary Hacksにもその記事が掲載されています

最近では「ハロー“Hello,World"OSと標準ライブラリのシゴトとしくみ」という名著があります

以下は鵜飼さんのコンテンツからの引用でありメモです
実行環境はarmhf

- sectionを確認する
readelf -l [実行形式ファイル]

```
readelf -l sample                       

Elf file type is EXEC (Executable file)
Entry point 0x102f4
There are 8 program headers, starting at offset 52

Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  EXIDX          0x0004bc 0x000104bc 0x000104bc 0x00008 0x00008 R   0x4
  PHDR           0x000034 0x00010034 0x00010034 0x00100 0x00100 R E 0x4
  INTERP         0x000134 0x00010134 0x00010134 0x00019 0x00019 R   0x1
      [Requesting program interpreter: /lib/ld-linux-armhf.so.3]
  LOAD           0x000000 0x00010000 0x00010000 0x004c8 0x004c8 R E 0x10000
  LOAD           0x0004c8 0x000204c8 0x000204c8 0x0011c 0x00120 RW  0x10000
  DYNAMIC        0x0004d4 0x000204d4 0x000204d4 0x000e8 0x000e8 RW  0x4
  NOTE           0x000150 0x00010150 0x00010150 0x00044 0x00044 R   0x4
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x10

 Section to Segment mapping:
  Segment Sections...
   00     .ARM.exidx 
   01     
   02     .interp 
   03     .interp .note.ABI-tag .note.gnu.build-id .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rel.dyn .rel.plt .init .plt .text .fini .rodata .ARM.exidx .eh_frame 
   04     .init_array .fini_array .jcr .dynamic .got .data .bss 
   05     .dynamic 
   06     .note.ABI-tag .note.gnu.build-id 
   07     
```


- 読みこまれるldの判別方
objdump -s -j .interp [実行形式ファイル]

- sourceの表示も
objdump -s -j .rel

_dl_start(elf/rtld.c)
  elf_machine_load_address
    _init(init-first.c)
      __libc_init_secure
