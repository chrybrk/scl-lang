gcc -Isrc/include src/*.c -O3 -o bin/scl
if [[ $? -eq 0 ]]; then
    echo "done"
else
    exit 1
fi
