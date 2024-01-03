nasm -felf64 asm.asm
ld asm.o -o asm
./asm
echo $?
