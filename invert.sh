#!/bin/sh

for file in ./*
do
	convert $file -channel RGB -negate $file
done
	