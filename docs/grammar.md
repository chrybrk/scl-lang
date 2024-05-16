program:
    exit
    let
    extern

extern:
    extern ident;

exit:
    exit([expr]);

let:
    let ident: [type] = [expr];

type:
    i16
    i32
    i64
    char

expr:
    intlit
    ident
