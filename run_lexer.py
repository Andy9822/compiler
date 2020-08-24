import os,sys

testName = sys.argv[1]
cmd = f'./etapa1 tests/{testName}/basic_input.txt > tests/{testName}/basic_result.txt'

if len(sys.argv) > 2:
    testType = sys.argv[2]
    cmd = f'./etapa1 tests/{testName}/{testType}_input.txt > tests/{testName}/{testType}_result.txt'
    
os.system(cmd)
