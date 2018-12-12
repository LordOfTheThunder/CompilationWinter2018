import os
import re
from subprocess import Popen
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('-stop_on_fail', default='true')
parser.add_argument('-diff', default='true')
args = parser.parse_args()

in_pattern = r'(.*).in'
in_files = []
for file in os.listdir('.'):
    res = re.search(in_pattern, file)
    if res:
        in_files.append(res.group(1))

for test in in_files:
    test_name = test + '.in'
    print "Running " + test_name,
    output = Popen([r"./../hw3 < " + test_name], stdout=subprocess.PIPE, shell=True).stdout.read()
    with open(test + '.out') as fp:
        expected = fp.read()
        if expected == output:
            print('PASSED')
        else:
            if args.diff == 'true':
                print('FAILED')
                print ("================= Your output =================")
                print(output)
                print("================================================")
                print("================ Expected output ================ (" + test + ".out)")
                print(expected)
                print("================================================")
                print ("================= In file =================")
                print("================================================")
                with open(test_name) as fp_in:
                    print(fp_in.read())

            if args.stop_on_fail == 'true':
                break
