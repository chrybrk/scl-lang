# SCL: Simple Compiled Language
It is `in progress` programming language that i am working on as a fun project to learn about `compiler design`.

# Install
$ `git clone https://github.com/chrybrk/scl-lang.git`
$ `./script/run.sh`

Now, it will create a binary file called, `scl` in the `bin/` folder.

$ `./script/install.sh` you can choose to install it in `opt/` folder.

# How to use?
```
extern i32 puts;
extern i32 atoi;

puts("Hello, World!");

let x: i32 = atoi("10");
exit(x);
```
$ `scl main.scl -o main -lc`

It uses `puts` from `libc`.
In `scl`, `exit();` is important because it does not generate or leave function on its own.

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
