import subprocess
import os

if os.path.exists('read_only.csv'):
    os.remove('read_only.csv')
if os.path.exists('mostly_reads.csv'):
    os.remove('mostly_reads.csv')
if os.path.exists('mostly_writes.csv'):
    os.remove('mostly_writes.csv')

size = '1000'
duration = '100'
density = '0.75'
mode = '0'
representation = 'ADJACENCY_LIST'

import subprocess

def build():
    # 1. Remove the old build directory inside the parent directory ('../')
    subprocess.run(['rm', '-rf', 'build/'], cwd='../')
    
    # 2. Configure CMake inside the parent directory
    subprocess.run([
        'cmake', '-B', 'build', 
        f"-DIMPLEMENTATION={representation}", 
        '-DCMAKE_BUILD_TYPE=Debug'
    ], cwd='../')
    
    # 3. Compile the project (Fixed the missing .run method)
    subprocess.run(['cmake', '--build', 'build'], cwd='../')
   
def run():
    density = ['0.3', '0.6', '0.75']
    for i in range(3):
        for _ in range(10):
            print('Running 1\n')
            subprocess.run(['../build/benchmark/micro_bench', '-s', size, '-t', duration, '-d', density[i], '-m', mode,
                            '-a', '0', '-b', '0', '-c', '100', '-g', '0', '-w', f"read_only_{representation}_{size}_{density[i]}.csv"])
                
            if representation != 'CSR':
                print('Running 2\n')
                subprocess.run(['../build/benchmark/micro_bench', '-s', size, '-t', duration, '-d', density[i], '-m', mode,
                            '-a', '10', '-b', '10', '-c', '80', '-g', '0', '-w', f"mostly_read_{representation}_{size}_{density[i]}.csv"])
            
                print('Running 3\n')
                subprocess.run(['../build/benchmark/micro_bench', '-s', size, '-t', duration, '-d', density[i], '-m', mode,
                            '-a', '40', '-b', '40', '-c', '20', '-g', '0', '-w', f"mostly_write_{representation}_{size}_{density[i]}.csv"])
            
        

#build()
#run()
#representation = 'ADJACENCY_MATRIX'
#build()
#run()
representation = 'CSR'
build()
run()


