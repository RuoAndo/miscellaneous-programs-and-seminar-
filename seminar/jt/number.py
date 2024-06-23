#!/usr/bin/python
# coding: UTF-8
 
import sys
 
argvs = sys.argv
argc = len(argvs)
if (argc != 3):
    print 'Usage: $ python %s target_file making_file' % argvs[0]
    quit()
f = open(argvs[1])
lines2 = f.readlines()
f.close()
nf = open(argvs[2], 'w')
i = 1
for line in lines2:
    nf.write('%3d: %s' % (i, line))
    i = i + 1
nf.close()
