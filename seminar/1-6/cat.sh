TESTFILE=$1
rm -rf logcat
touch logcat 
while read line; do
    cat $line >> logcat
done < $TESTFILE
