<pre>
python cut.py list > list-cut
./geoip.sh list-cut  > tmp
python 2.py tmp list-cut > 100.kml
</pre>

list
<pre>
X.X.X.X	2017-11-23 12:26:35
Y.Y.Y.Y	2017-11-23 12:26:35
</pre>

tmp
<pre>
GeoIP Country Edition: US, United States GeoIP City Edition, Rev 1: US, TX, Texas, Houston, 77072, 29.699699, -95.585899, 618, 281 GeoIP ASNum Edition: IP Address not found,X.X.X.X
</pre>