#!/bin/bash

set -e

build_file() {
    gcc -D__GNUC__=0 -E -P $1.c > tmp/$1.c3
    ./cc3 tmp/$1.c3 -c -o tmp/$1.o3
}

build_file cc3
build_file gen
build_file sema
build_file parse
build_file lex
build_file util
cc -no-pie tmp/cc3.o3 tmp/gen.o3 tmp/sema.o3 tmp/parse.o3 tmp/lex.o3 \
    tmp/util.o3 -o tmp/cc3_self

build_file2() {
    gcc -D__GNUC__=0 -E -P $1.c > tmp/$1.c3
    ./tmp/cc3_self tmp/$1.c3 -c -o tmp/$1.o3
}

for i in $(seq 2)
do
    build_file2 cc3
    build_file2 gen
    build_file2 sema
    build_file2 parse
    build_file2 lex
    build_file2 util
    cc -no-pie tmp/cc3.o3 tmp/gen.o3 tmp/sema.o3 tmp/parse.o3 tmp/lex.o3 \
        tmp/util.o3 -o tmp/cc3_self
done

rm tmp/*.{c3,o3}
