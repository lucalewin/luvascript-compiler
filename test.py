#!/bin/python3

import os
import glob
import subprocess

def colored(r, g, b, text):
    return f"\033[38;2;{r};{g};{b}m{text}\x1B[0m"

test_dir = "./tests"

# load all test files
test_files = [file for root, dirs, files in os.walk(test_dir) for file in files if file.lower().endswith(".lv")]

# format: ['test-name', ['test-output', ...], exitcode]
expected_outputs = [
	["Test 1", [], 55],
	["Test 2", [], 26],
	["Test 3", [], 14],
	["Test 4", [], 5],
	["Test 5", [], 69],
	["Test 6", [], 18],
	["Test 7", [], 12],
	["Test 8", [], 3],
	["Test 9", [], 50],
	["Test 10", [], 3],
	["Test 11", [], 40],
	["Test 12", [], 1],
	["Test 13", [], 32],
	["Test 14", [], 8],
	["Test 15", [], 12],
	["Test 16", [], 13],
	["Test 17", [], 0],
	["Test 18", [], 1],
	["Test 19", [], 1],
	["Test 20", [], 0],
	["Test 21", [], 1],
	["Test 22", [], 1],
	["Test 23", [], 20],
	["Test 24", [], 10],
	["Test 25", [], 8],
	["Test 26", [], 55],
	["Test 27", [], 22],
	["Test 28", [], 97],
	["Test 29", [], 63],
	["Test 30", [], ord(',')],
	["Test 31", [], 12],
	["Test 32", ["Hello, World!"], 14],
	["Test 33", [], 22],
	["Test 34", [], 3],
	["Test 35", [], 20],
	["Test 36", [], 19],
	["Test 37", [], ord(',')],
	["Test 38", [], ord('s')]]

index = 0

successfull_tests = 0
failed_builds = 0
failed_tests = 0

print('\n')

#change working directory to test directory
os.chdir(test_dir)

for test in test_files:
	# compile test
	compile_proc = subprocess.Popen(['../bin/debug/lvc', test], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

	stdout, stderr = compile_proc.communicate()

	if compile_proc.returncode != 0 or not os.path.exists('./a.out'):
		print(f'{expected_outputs[index][0]}:', colored(255, 0, 0, 'build failed'))
		print('build output:\n')
		print(stdout.decode() if stdout.decode() != '' else colored(255, 0, 0, 'process was aborted\n'))
		if stderr is not None and stderr.decode() != '':
			print(f'stderr:\n{stderr.decode()}')
		print('[================================]\n')

		failed_builds += 1
		index += 1
		
		continue

	result = subprocess.run(['./a.out'], capture_output=True, text=True)

	# check if test was successful or failed
	if result.returncode == expected_outputs[index][2]:
		print(f'{expected_outputs[index][0]}:', colored(0, 255, 0, 'test finished successfully'))
		successfull_tests += 1
	else:
		print(f'{expected_outputs[index][0]}:', colored(255, 0, 0, 'test failed'))
		print(f'\texpected exitcode \'{expected_outputs[index][2]}\' but got \'{result.returncode}\' instead')
		print()
		failed_tests += 1

	print('\n[================================]\n')
	index += 1

# remove generated binary files
generated_object_files = glob.glob(os.path.join('./*.o'))
for file in generated_object_files:
	os.remove(file)

generated_binary_files = glob.glob(os.path.join('./*.out'))
for file in generated_binary_files:
	os.remove(file)

print('Stats:')
print(' * successfull tests: ', colored(0, 255, 0, successfull_tests))
print(' * failed tests:      ', colored(255, 0, 0, failed_tests) if failed_tests != 0 else colored(0, 255, 0, failed_tests))
print(' * failed builds:     ', colored(255, 0, 0, failed_builds) if failed_builds != 0 else colored(0, 255, 0, failed_builds))

print()

if failed_tests == 0 and failed_builds == 0:
	print('[', colored(0, 255, 0, 'All tests finshed successfully'), ']')
	print()
else:
	exit(1)