echo $1
dos2unix $1
subject=$(head -n 1 "$1")
echo $subject
mail -s "$subject" ando.ecobiz.kagura.q0lobawit0@blogger.com < $1
 