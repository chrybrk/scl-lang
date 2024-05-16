./script/run.sh
./bin/scl $1
if [[ $? -eq 0 ]]; then
    ./bin/main
    echo $?
fi
