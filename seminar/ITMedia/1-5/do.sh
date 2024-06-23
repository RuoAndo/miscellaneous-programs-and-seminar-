#!/bin/sh

TESTFILE=$1
while read line; do
    #echo $line
    python abuseip.py $line --TOKEN-- 30
    sleep 1m
done < $TESTFILE
