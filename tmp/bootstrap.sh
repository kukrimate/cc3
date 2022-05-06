#!/bin/sh

set -e

files="cc3 gen sema parse lex util"

# Build first stage
for file in $files
do
    echo "Preprocessing $file"
    cpp -U__GNUC__ -P $file.c > tmp/$file.i
    echo "Compiling $file (stage 1)"
    ./cc3 -c -o tmp/$file.o tmp/$file.i > /dev/null
done

# Link first stage
echo "Linking (stage 1)"
cc -no-pie -o tmp/cc3 tmp/*.o

# Build second stage
for file in $files
do
    echo "Compiling $file (stage 2)"
    ./tmp/cc3 -c -o tmp/$file.o tmp/$file.i > /dev/null
done

# Link first stage
echo "Linking (stage 2)"
cc -no-pie -o tmp/cc3 tmp/*.o

# Cleanup
rm tmp/*.i tmp/*.o
