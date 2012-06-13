#!/bin/sh

aclocal && 
autoreconf && 
./configure && 
make
