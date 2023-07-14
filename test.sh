#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input -> $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"

assert 21 "5+20-4;"

assert 41 " 12 + 34 - 5; "

assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3 + 5) / 2;'

assert 10 '-10 + 20;'
assert 2 '-20 / -10;'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 1 '10 - 5 < 6 == 1;'

assert 3 'a = 3; a;'
assert 8 'a = 3; z = 5; a + z;'
assert 6 'a= b=3; a + b;'

assert 3 'aa = 3; aa;'
assert 8 'aa = 3; zz = 5; aa + zz;'
assert 6 'aa= bb=3; aa + bb;'

assert 6 'return 6;'
assert 5 'returnx = 5; return returnx;'
assert 6 'aa= bb = 3; return aa + bb;'

assert 3 'a = 0; if (3 < 4) a = 3; return a;'
assert 0 'a = 0; if (3 > 4) a = 3; return a;'

assert 3 'if (3 < 4) a = 3; else a = 5; return a;'
assert 5 'if (3 > 4) a = 3; else a = 5; return a;'
assert 5 'if (4 > 4) a = 3; else a = 5; return a;'
assert 3 'if (1) a = 3; else a = 5; return a;'
assert 5 'if (0) a = 3; else a = 5; return a;'

assert 0 'a = 0; while(0) a=1; return a;'
assert 5 'a = 0; while(a < 5) a=a+1; return a;'

assert 10 'x = 0; for(a = 0;a < 5; a = a + 1) x = x + a; return x;'
assert 10 'x = 10; for(;0;) x = x + 1; return x;'

echo OK