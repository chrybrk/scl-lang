gcc -Isrc/include src/*.c -O3 -o bin/scl
if [[ $? -eq 0 ]]; then
    bin/scl example/input000.scl
else
    echo "[ERR]: failed, $?"
fi
