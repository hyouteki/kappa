nasm -felf64 dev.asm
ld dev.o -o dev
./dev
echo $?
