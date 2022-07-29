test "this is a string"

[ ] ( ) { } . ->
++ -- & * + - ~ !
/ % << >> < > <= >= == != ^ | && ||
? : ; ...
= *= /= %= += -= <<= >>= &= ^= |=
, # ##

// previous expansion bug source
#define X X Y
#define Y X
#define GLUE(x, y) x  y
GLUE(X X, 5)

// gcc "troublesome" example
#define foo(x) bar x
foo(foo) (2)

// using actual parameters twice
#define dup(x) x x
dup(this will be duplicated)

#define obj a ## b
a dup()obj

#define EMPTY

EMPTY#define LOL xy

LOL

__DATE__
