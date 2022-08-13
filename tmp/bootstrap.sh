#!/bin/sh

set -e

files="cc3 gen sema parse lex util"

##
# Stage 0: Build cc3 using cc3 built with the system compiler
##

for file in $files
do
    echo "Compiling $file (stage 0)"
    ./cc3 -c -o tmp/$file.o $file.c
done

echo "Linking (stage 0)"
cc -o tmp/cc3_stage0 tmp/*.o

##
# Stage N: Build cc3 using cc3 built with itself
##

for N in $(seq 2)
do
    for file in $files
    do
        echo "Compiling $file (stage $N)"
        ./tmp/cc3_stage$((N-1)) -c -o tmp/$file.o $file.c
    done

    echo "Linking (stage $N)"
    cc -o tmp/cc3_stage$N tmp/*.o
done
