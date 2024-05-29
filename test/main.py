a = "2+3+4+5+1"
i = 0
c = a[i]

def n_tok():
    global i, c
    i += 1
    if i < len(a): c = a[i]
    else: c = None

    return c

def expr(v):
    l = c

    n_tok()

    while c in ["+"]:
        n_tok()
        print(f"{l} + ", end="")
        return expr(v[i:])

    return l

tree = expr(a)
print(tree)
