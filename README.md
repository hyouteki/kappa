> [!IMPORTANT]
> Work in progress

## Quick Start
``` console
cargo run -- compile --felf64 -f ./eg/main.K
nasm -felf64 ./eg/main.asm -o ./eg/main.o
ld ./eg/main.o -o ./eg/main
./eg/main
```
