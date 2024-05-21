#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    system("fasm bin/main.s");
    system("gcc bin/main.o -lc -nostdlib -nostartfiles -nolibc -nostdinc -no-pie -o bin/main");

    return 0;
}
