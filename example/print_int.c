extern int printf(char* fmt, ...);

void print(char* fmt)
{
    printf("%s\n", fmt);
}

void print_int(int value)
{
    printf("%d\n", value);
}
