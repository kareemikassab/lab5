cmd_/root/oslab/ex5/sysCallMod.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000 -T /usr/src/linux-headers-4.19.0-13-common/scripts/module-common.lds  --build-id  -o /root/oslab/ex5/sysCallMod.ko /root/oslab/ex5/sysCallMod.o /root/oslab/ex5/sysCallMod.mod.o ;  true