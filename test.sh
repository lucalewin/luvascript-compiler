#!/bin/sh

echo ""
echo "--------------- [BUILDING] ---------------"
echo ""
make build

index=1

for file in "tests"/*.lv
do
    echo ""
    echo "---------------- [Test $index] ----------------"
    echo ""
    ./bin/lvc "$file"
    index=$((index+1));
done