#!/bin/python3

"""
This module contains the test functions for the luvascript compiler
"""

import os
import sys
import glob
import subprocess

TEST_DIR = "./tests/"

# format: ['test-name', ['test-output', ...], exitcode]
EXPECTED_OUTPUTS: list = [
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

# ------------------------------------------------------------------------

def main():
    '''main function'''

    # build debug version of the luvascript compiler
    build_debug()

    # change working directory to test directory
    os.chdir(TEST_DIR)

    # parse command line args
    files, options = parse_args()

    index = 0
    successfull_tests = 0
    failed_builds = 0
    failed_tests = 0

    for file in files:
        # built the test file
        if not build_test(file, options):
            failed_builds += 1
            index += 1
            continue

        #execute the generated file
        if run_test('./a.out', EXPECTED_OUTPUTS[index]):
            successfull_tests += 1
        else:
            failed_tests += 1
        index += 1

    # print summary
    print('Stats:')
    print(' - successfull tests: ', colored(0, 255, 0, successfull_tests))
    print(' - failed tests:      ', colored(255, 0, 0, failed_tests)
        if failed_tests != 0 else colored(0, 255, 0, failed_tests))
    print(' - failed builds:     ', colored(255, 0, 0, failed_builds)
        if failed_builds != 0 else colored(0, 255, 0, failed_builds))

    print()

    if failed_tests == 0 and failed_builds == 0:
        print('[', colored(0, 255, 0, 'All tests finshed successfully'), ']')
        print()
    else:
        sys.exit(1)

# ------------------------------------------------------------------------

def parse_args():
    '''
    Parsing the command line arguments
    returns a list of files to build/run + a list of options
    '''

    all_test_files = sorted(filter(os.path.isfile, glob.glob('./*.lv')))

    if len(sys.argv) > 1:
        # look for options
        options = []
        files = []

        for arg in sys.argv[1:]:
            if arg.startswith('-'):
                options.append(arg)
            elif arg in all_test_files:
                files.append(arg)
            else:
                print(colored(255, 0, 0, f'{arg} is not a valid test file'))
                sys.exit(1)

        return files, options

    return all_test_files, []

# ------------------------------------------------------------------------

def build_debug():
    """builds the debug version"""
    print()
    print(colored(0, 255, 0, 'Building debug version...'))
    subprocess.run(['make', 'debug'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=True)

# ------------------------------------------------------------------------

def build_test(file, options):
    """builds the test file"""

    # build the test file
    compile_proc = subprocess.Popen(
        ['../bin/debug/lvc', file],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT)

    stdout, stderr = compile_proc.communicate()

    if compile_proc.returncode != 0 or not os.path.exists('./a.out'):
        # the build failed
        # print error message
        build_failed_str = colored(255, 0, 0, 'build failed')
        stdout_msg = stdout.decode() if stdout else ''

        print(f"{file}: {build_failed_str}")
        print(f'{file}: build output:\n{stdout_msg}')

        # print error if stderr is not empty
        if stderr is not None and stderr.decode() != '':
            print(f'stderr:\n{stderr.decode()}')

        return False

    if stdout and ('--verbose' in options or '-v' in options):
        print(f'{file}: build output:\n{stdout.decode()}')

    return True

# ------------------------------------------------------------------------

def run_test(file, expected):
    """runs the test file"""

    result = subprocess.run([file], capture_output=True, text=True, check=False)

    # check if test was successful or failed
    if result.returncode == expected[2]:
        print(
            f'{expected[0]}:',
            colored(0, 255, 0, 'test finished successfully'))
        return True
    else:
        print(
            f'{expected[0]}:',
            colored(255, 0, 0, 'test failed'))
        print(
            f"\texpected exitcode \'{expected[2]}\'"
            f" but got \'{result.returncode}\' instead")
        print()
        return False

# ------------------------------------------------------------------------

def colored(red: int, green: int, blue: int, text: str):
    """colors the text with the given RGB values"""
    return f'\033[38;2;{red};{green};{blue}m{text}\033[0m'

# ------------------------------------------------------------------------

if __name__ == '__main__':
    main()
