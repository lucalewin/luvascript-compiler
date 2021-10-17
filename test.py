#!/bin/python3

import os
import sys
import glob
import subprocess

def colored(r, g, b, text):
    return "\033[38;2;{};{};{}m{} \x1B[0m".format(r, g, b, text)

test_dir = "./tests"

# load all test files
test_files = [file for root, dirs, files in os.walk(test_dir) for file in files if file.lower().endswith(".lv")]

# format: 'test-name', ['test-output', ...], exitcode
expected_outputs = [
	["Test 1", [], 55], 
	["Test 2", [], 26], 
	["Test 3", [], 14],
	["Test 4", [], 5],
	["Test 5", [], 69]]

index = 0

successfull_tests = 0
failed_builds = 0
failed_tests = 0

print('\n')

#change working directory to test directory
os.chdir(test_dir)

for test in test_files:
	# compile test
	compile_proc = subprocess.Popen(['../bin/lvc', test], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

	stdout, stderr = compile_proc.communicate()

	if compile_proc.returncode != 0:
		print(f'{expected_outputs[index][0]}:', colored(255, 0, 0, 'build failed'))
		print('build output:\n')
		print(stdout if stdout != '' else colored(255, 0, 0, 'process was aborted\n'))
		if stderr is not None and stderr != '':
			print(f'stderr:\n{stderr}')
		print('[==============================]\n')

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
		print('\texpected exitcode \'{}\' but got \'{}\' instead'.format(expected_outputs[index][2], result.returncode))
		print()
		failed_tests += 1

	print('\n[==============================]\n')
	index += 1

# remove generated binary files
generated_object_files = glob.glob(os.path.join('./*.o'))
for file in generated_object_files:
	os.remove(file)

generated_binary_files = glob.glob(os.path.join('./*.out'))
for file in generated_binary_files:
	os.remove(file)

print()

if failed_tests == 0 and failed_builds == 0:
	print('All tests finshed successfully')
else:
	print('Stats:')
	print(' * successfull tests: ', colored(0, 255, 0, successfull_tests))
	print(' * failed tests:      ', colored(255, 0, 0, failed_tests))
	print(' * failed builds:     ', colored(255, 0, 0, failed_builds))
	# print(failed_tests, 'out of', len(expected_outputs), 'tests failed')
	
print('\n')
