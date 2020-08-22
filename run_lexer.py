import os,sys

testName = sys.argv[1]
cmd = f'./etapa1 tests/{testName}/basic_input.txt > tests/{testName}/basic_result.txt'
os.system(cmd)
