#!/bin/sh

awk '{ $1 = ""; $2 = ""; print }' $1 > `echo "$1"|sed 's/\.txt/-cut\.txt/'`
