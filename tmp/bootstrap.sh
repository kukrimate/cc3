#!/bin/sh

set -e

files="cc3 gen sema parse lex debug util"

##
# Stage 0: Build cc3 using cc3 built with the system compiler
##

for file in $files
do
    echo "Compiling $file (stage 0)"
    valgrind ./cc3 -c -o tmp/$file.o $file.c
done

echo "Linking (stage 0)"
cc -o tmp/cc3 tmp/*.o

##
# Stage N: Build cc3 using cc3 built with itself
##

for N in $(seq 2)
do
    for file in $files
    do
        echo "Compiling $file (stage $N)"
        ./tmp/cc3 -c -o tmp/$file.o $file.c
    done

    echo "Linking (stage $N)"
    cc -o tmp/cc3 tmp/*.o
done
