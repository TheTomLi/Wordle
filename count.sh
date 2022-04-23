#!/bin/bash


result=0

while read province city num
do
    if [$province = $1]
    then 
	result = expr(result + $num)
    fi
done < $2

echo $result
