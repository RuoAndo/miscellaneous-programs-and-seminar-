import json
from json import load
import requests

import sys 
argvs = sys.argv 

str1="https://www.abuseipdb.com/check/"
str2=argvs[1]
str3="/json?key="
str4=argvs[2]
str5="&days="
str6=argvs[3]

url = str1 + str2 + str3 + str4 + str5 + str6
r = requests.get(url)

#print r.status_code
j = r.json()
print str(str2) + ";" + str(j)




