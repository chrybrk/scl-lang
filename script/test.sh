./script/run.sh
./bin/scl $1
./bin/linker
if [[ $? -eq 0 ]]; then
    ./bin/main
    echo $?
fi
