cat log | cut -d ":" -f2 | grep "0" | grep -v time | grep -v accurecy | grep -v loss | shuf -n 5000 > tmp

