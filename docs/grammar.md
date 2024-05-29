program:
    exit
    let
    extern

extern:
    extern type ident;

exit:
    exit([expr]);

let:
    let ident: [type] = [expr];

type:
    i16
    i32
    i64
    char
    string

expr:
    intlit
    ident
    string
    function call
