#!/bin/sh

echo ""
echo "--------------- [BUILDING] ---------------"
echo ""
make build

cd ./tests

index=1

for file in ./*.lv
do
    echo ""
    echo "---------------- [Test $index] ----------------"
    echo ""
    ../bin/lvc "$file"

    ./a.out
    echo $?

    index=$((index+1));
done
