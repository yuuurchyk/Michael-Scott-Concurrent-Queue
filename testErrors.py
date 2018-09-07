import os
import subprocess

os.chdir("./Debug")

n = 0

while(True):
    os.system("clear")
    print(n)
    try:
        subprocess.check_output(["./tests"])
    except subprocess.CalledProcessError as e:                                                                                                   
        print(e.output)
    n += 1
