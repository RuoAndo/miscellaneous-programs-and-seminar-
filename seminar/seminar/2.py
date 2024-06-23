import sys
import re

argvs = sys.argv
argc = len(argvs)
f = open(argvs[2]) 

line = f.readline() 

ipList = []
while line:
    #print line
    #tmp = line.split(",")
    ipList.append(line.strip())
    line = f.readline() 
f.close()

ipList_uniq = list(set(ipList))
#print ipList

argvs = sys.argv
argc = len(argvs)
f = open(argvs[1]) 

print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

print "<kml xmlns=\"http://www.opengis.net/kml/2.2\"";
print " xmlns:gx=\"http://www.google.com/kml/ext/2.2\"";
print " xmlns:kml=\"http://www.opengis.net/kml/2.2\" ";
print " xmlns:atom=\"http://www.w3.org/2005/Atom\">";

print "<Document>";
print "<name>";
#print $dt->strftime('%Y/%m/%d %H:%M:%S');
print "</name>";
print "<Folder>";

line = f.readline() 

counter = 1
while line:

    tmp = line.split(",")
    tmp2 = tmp[1].split(":")

    flag = 0
    if len(tmp) == 12:
        counter = 0
        print "CHECK:length:12"
        for x in tmp:
            print "CHECK:" + str(counter) + ":" + x
            counter = counter + 1
            flag = 1
            
    if len(tmp) == 13:
        print "CHECK:length:13"
        counter = 0
        for x in tmp:
            print "CHECK:" + str(counter) + ":" + x
            counter = counter + 1
                    
    if len(tmp) == 12:
        #print "HIT:" + str(len(tmp))
        print "<Style id=\"msn_ylw-pushpin1300\">";
        print "<IconStyle>";
        print "<color>ff0000ff</color>";
        print "<scale>6</scale>";
        print "</IconStyle>";
        print "<LabelStyle>";
        print "<color>ff0000ff</color>";
        print "<scale>6</scale>";
        print "</LabelStyle>";
        print "</Style>";

        print "<Placemark>";
        #print "<name>" + str(counter) + ":" +  tmp2[1].strip()  + "</name>";

        if tmp[10].find("AS") > -1:
            astmp = tmp[10].split(":")
            astmp2 = astmp[1].split(" ")
            print "<name>" + astmp2[1] + "</name>";
            print "<description>" + tmp[10] + "</description>";
        elif tmp[11].find("AS") > -1:
            astmp = tmp[11].split(":")
            astmp2 = astmp[1].split(" ")
            print "<name>" + astmp2[1] + "</name>";
            print "<description>" + tmp[11] + "</description>";
        else:
            print "<name>" + "N/A" + "</name>";
            
        #print "<description>" + tmp2[1].strip() + "</description>";
        print "<styleUrl>#msn_ylw-pushpin1300</styleUrl>";
        print "<Point>";

        if str(tmp[7]).find(".") > -1:
            print "<coordinates>" + tmp[8].strip() + "," + tmp[7].strip() + "</coordinates>";
            print "</Point>";
            print "</Placemark>";
        
        elif str(tmp[9]).find(".") > -1:
            print "<coordinates>" + tmp[9].strip() + "," + tmp[8].strip() + "</coordinates>";
            print "</Point>";
            print "</Placemark>";

    if len(tmp) == 13:
        #print "HIT:" + str(len(tmp))
        print "<Style id=\"msn_ylw-pushpin1300\">";
        print "<IconStyle>";
        print "<color>ff0000ff</color>";
        print "<scale>6</scale>";
        print "</IconStyle>";
        print "<LabelStyle>";
        print "<color>ff0000ff</color>";
        print "<scale>6</scale>";
        print "</LabelStyle>";
        print "</Style>";

        print "<Placemark>";
        #print "<name>" + str(counter) + ":" +  tmp2[1].strip()  + "</name>";

        if tmp[10].find("AS") > -1:
            astmp = tmp[10].split(":")
            astmp2 = astmp[1].split(" ")
            print "<name>" + astmp2[1] + "</name>";
            print "<description>" + tmp[10] + "</description>";
        elif tmp[11].find("AS") > -1:
            astmp = tmp[11].split(":")
            astmp2 = astmp[1].split(" ")
            print "<name>" + astmp2[1] + "</name>";
            print "<description>" + tmp[11] + "</description>";
        else:
            print "<name>" + "N/A" + "</name>";
            
        #print "<description>" + tmp2[1].strip() + "</description>";
        print "<styleUrl>#msn_ylw-pushpin1300</styleUrl>";
        print "<Point>";

        if str(tmp[7]).find(".") > -1:
            print "<coordinates>" + tmp[8].strip() + "," + tmp[7].strip() + "</coordinates>";
            print "</Point>";
            print "</Placemark>";
        
        elif str(tmp[9]).find(".") > -1:
            print "<coordinates>" + tmp[9].strip() + "," + tmp[8].strip() + "</coordinates>";
            print "</Point>";
            print "</Placemark>";
                           
    counter  = counter + 1
    line = f.readline()

print "</Folder>";
print "</Document>";
print "</kml>";
