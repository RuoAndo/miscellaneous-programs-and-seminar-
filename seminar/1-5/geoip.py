import sys 
import commands

argvs = sys.argv
 
f = open(argvs[1])

line = f.readline() 

while line:

    try:
        tmp = line.split(":")

        comstr = "geoiplookup -f /root/GeoLiteCity.dat " + tmp[2].strip()
        check = commands.getoutput(comstr)
        print tmp[2].strip() + "," + check

    except:
        pass
        
    line = f.readline()

f.close

