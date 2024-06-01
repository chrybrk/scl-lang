EXPR:
    number

TERM:
    EXPR "+" TERM | EXPR
    EXPR "-" TERM | EXPR


2 + 4 - 3
