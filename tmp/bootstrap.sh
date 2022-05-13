#!/bin/sh

set -e

files="cc3 gen sema parse lex debug util"

##
# Stage 0: Build cc3 using cc3 built with the system compiler
##

for file in $files
do
    echo "Preprocessing $file"
    cpp -U__GNUC__ -P $file.c > tmp/$file.i
    echo "Compiling $file (stage 0)"
    ./cc3 -c -o tmp/$file.o tmp/$file.i
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
        ./tmp/cc3 -c -o tmp/$file.o tmp/$file.i
    done

    echo "Linking (stage $N)"
    cc -o tmp/cc3 tmp/*.o
done

# Cleanup
rm tmp/*.i tmp/*.o
