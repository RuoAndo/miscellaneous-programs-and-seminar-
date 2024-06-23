while read line; do
    # echo $line
    #result=`geoiplookup -f GeoLiteCity.dat $line`
    result=`geoiplookup $line`
    echo $result,$line
done < $1
