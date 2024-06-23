import sys                                                                                                        
import re

argvs = sys.argv                                                                                                  
argc = len(argvs)                                                                                                 
                                                                                                                  
f = open(argvs[1])                                                                                                
line = f.readline()                                                                                               
                                                                                                                  
while line:                                                                                                       
    tmp = re.split(' +', line) #=> ['a', 'c', 'ef', 'hi']
    #words = re.split(" +", time)
    #print(tmp)                                                                                                   
    print("break " + tmp[2] + ":" + tmp[1].strip())                                                               
    line = f.readline()                                                                                               
    
# global -f function.cpp | grep forward > tmp
# python gdb.py tmp > tmp2
