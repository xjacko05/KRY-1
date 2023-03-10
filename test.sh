#!/bin/bash

#cat ciphers.csv |
#while read -r line
#do
#echo $line > "test/"${line:0:6}".txt"
#done

#ls test |
#while read -r file
#do
#echo $file
#./kry -c -f test/$file -o out.txt
#cat out.txt | head -c 40
#echo
#echo
#done

cat ciphers.csv |
while read -r line
do
file="test/"${line:0:6}".txt"
echo $line | head -c 1000 > $file
echo $file
./kry -c -f $file -o out.txt
cat out.txt | head -c 40
rm $file
echo
echo
done