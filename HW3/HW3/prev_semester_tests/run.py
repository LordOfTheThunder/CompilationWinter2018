import os
import re
from subprocess import Popen
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-diff', default='true')
args = parser.parse_args()

failed_tests = []

in_pattern = r'(.*).in'
in_files = []
for file in os.listdir('.'):
    res = re.search(in_pattern, file)
    if res:
        in_files.append(res.group(1))

in_files = sorted(in_files)
for test in in_files:
    test_name = test + '.in'
    print "Running " + test_name,
    output = Popen([r"./../hw3 < " + test_name], stdout=subprocess.PIPE, shell=True).stdout.read().decode('utf-8')
    passed = False
    with open(test + '.out') as fp:
        expected = fp.read().decode('utf-8')

    passed = expected == output
    if passed:
        print('PASSED')
    else:
        print('FAILED <-----')
        failed_tests.append(test_name)
        if args.diff == 'true':
            print ("================= Your output =================")
            print(output)
            print("================================================")
            print("================ Expected output ================ (" + test + ".out)")
            print(expected)
            print("================================================")
            print ("================= In file =================")
            with open(test_name) as fp_in:
                print(fp_in.read())

            print("================================================")
            print("Enter a letter:")
            print(" - x for exit     ")
            print(" - any other letter for continue     ")
            if 'x' == raw_input():
                exit(0)
            else:
                continue


if len(failed_tests) != 0:
    print("Failed tests:")
for failed_test in failed_tests:
    print('- ' + failed_test)
