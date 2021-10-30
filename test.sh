#!/bin/sh

echo ""
echo "--------------- [BUILDING] ---------------"
echo ""

# build the project
make build

cd ./tests

index=1

# loop through all the test files and run the generated binary file
for file in ./*.lv
do
    echo ""
    echo "---------------- [Test $index] ----------------"
    echo ""

	# compile the test file with the previously built project binary
    ../bin/lvc -S "$file"

	# execute the generated binary file
    ./a.out

	# print the exit code of the generated binary file
    echo $?

    index=$((index+1));
done

# remove generated binary files for the test files
rm -rf ./*.o ./*.out