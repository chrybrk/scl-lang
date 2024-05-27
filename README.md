# SCL: Simple Compiled Language
It is `in progress` programming language that i am working on as a fun project to learn about `compiler design`.

# Install
```
$ git clone https://github.com/chrybrk/scl-lang.git
$ ./script/run.sh
```

Now, it will create a binary file called, `scl` in the `bin/` folder.

`$ ./script/install.sh` you can choose to install it in `opt/` folder.

# How to use?

### Help in SCL

```
$ scl
scl: help
            	scl <inputfile> <arguments>
            	-o: output
            	-a: generate assembly
            	-O: generate object
            	-r: compile and run
            	-l: linker input, -l<libs>
            	-L: linker libs path, -L<path>
            	-I: linker libs include path, -I<path>
            	-C: compile `.c` file and link
            	-S: compile `.s` file and link
            	-E: lazy evaluation, for variable, function, externs and loops
```

### Classic Example of Hello, World! and little more.

```
extern i32 puts;
extern i32 atoi;

puts("Hello, World!");

let x: i32 = atoi("10");
exit(x);
```

```
$ scl main.scl -o main -lc
$ ./main
```

- It uses `puts` from `libc`.
- In `scl`, `exit();` is important because it does not generate or leave function on its own.

# Features
- [x] Variable
- [x] Extern
- [x] Function Calling
- [x] Interacting with `C`
- [ ] Loops
- [ ] Function
- [ ] Structs
- [ ] Lazy Evaluation
- More you can find in `docs/notes`
