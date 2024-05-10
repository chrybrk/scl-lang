rm ./bin/*
gcc -Isrc/include src/*.c -O3 -o bin/scl
if [[ $? -eq 0 ]]; then
    bin/scl example/input000.scl >> bin/main.s
    nasm -felf64 bin/main.s -o bin/main.o
    ld bin/main.o -o bin/main
else
    echo "[ERR]: failed, $?"
fi
