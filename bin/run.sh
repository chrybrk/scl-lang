nasm -felf main.s -o main.o
ld main.o -o main
./main
echo $?
