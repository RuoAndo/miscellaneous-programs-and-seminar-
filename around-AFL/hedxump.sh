#!/bin/sh

ls id* > list

while read line; do
    echo $line
    hexdump -C $line
done < list
