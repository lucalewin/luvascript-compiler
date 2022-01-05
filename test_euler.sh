#!/bin/sh

echo ""
echo "--------------- [BUILDING] ---------------"
echo ""

# build the project
make build

cd ./tests/euler

if [ $# -eq 0 ]; then
	echo "No test specified, running all tests"
	
	index=1

	# loop through all the test files and run the generated binary file
	for file in ./*.lvs
	do
		echo ""
		echo "---------------- [Test $index] ----------------"
		echo ""

		# compile the test file with the previously built project binary
		../../bin/lvc -S "$file" "../../lib/math.lv" "../../lib/io.lv" "../../lib/util.lv"

		# execute the generated binary file
		./a.out

		# check if the test passed or failed
		if [ $? -eq 0 ]; then
			echo "Test $index passed"
		else
			echo "Test $index failed with exit code $?"
		fi

		index=$((index+1));
	done
else
	for test in $@; do
		echo ""
		echo "---------------- [Test $test] ----------------"
		echo ""

		# compile the test file with the previously built project binary
		../../bin/lvc -S problem${test}.lvs "../../lib/math.lv" "../../lib/io.lv" "util.lvs"

		# execute the generated binary file
		./a.out

		# check if the test passed or failed
		if [ $? -eq 0 ]; then
			echo "Test ${test} passed"
		else
			echo "Test ${test} failed with exit code $?"
		fi
	done
fi

# remove generated binary files for the test files
rm -rf ./*.o ./*.out