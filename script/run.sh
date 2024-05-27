gcc -Isrc/include src/*.c -Wall -Wextra -O3 -o bin/scl
if [[ $? -eq 0 ]]; then
    exit 0
else
    exit 1
fi
