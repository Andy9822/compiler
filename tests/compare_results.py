import filecmp
import sys
from os import listdir
from os.path import isfile, join

tests_dir = "./tests/"

OKGREEN = '\033[92m'
REDFAIL = '\033[91m'
ENDCOLOR ='\033[0m'

def test_directory(rule_name):
    path = tests_dir + rule_name
    prefixes = list(dict.fromkeys([f.split("_")[0] for f in listdir(path) if isfile(join(path, f))]))
    for prefix in prefixes:
        resultFile =  tests_dir + rule_name + "/" + prefix + "_result.txt"
        expectedFile =  tests_dir + rule_name + "/" + prefix + "_expected.txt"
        test_files(resultFile, expectedFile)

def test_files(file1, file2):
    print(file1,file2)
    comp = filecmp.cmp(file1, file2)
    if comp:
        print(OKGREEN + str(comp) + ENDCOLOR)
        sys.exit(0)
    else:
        print(REDFAIL + str(comp) + ENDCOLOR)
        sys.exit(-1)



if len(sys.argv) == 2:
    test_directory(sys.argv[1])
if len(sys.argv) == 3:
    test_files(sys.argv[1], sys.argv[2])
