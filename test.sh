#!/bin/sh

make clean ; make test

./test

rm test
reset
