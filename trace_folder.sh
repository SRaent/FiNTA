#!/bin/sh

for file in ./*
do
	./atafut -f $file -s $1
done
	