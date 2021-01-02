#!/bin/sh

for file in ./*
do
	./finta -f $file -s $1
done
	
