import os
import subprocess
import sys

if(len(sys.argv) < 3):
    raise RuntimeError("Wrong number of arguments")

folder = sys.argv[1]
prgOptions = sys.argv[2:]
prgOptions[0] = "./" + prgOptions[0]

os.chdir("./" + folder)

n = 0

while(True):
    os.system("clear")
    print(n)
    try:
        subprocess.check_output(prgOptions)
    except subprocess.CalledProcessError as e:                                                                                                   
        print(e.output)
    n += 1
