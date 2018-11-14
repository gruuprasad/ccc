import sys
import matplotlib.pyplot as plt
import os
import datetime
from os import listdir
from timeit import default_timer as timer
from tqdm import tqdm

int = []
val = dict()

ref = []

t = [timer()]

i = 1

files = []
exe = []

for p in sys.argv[1:]:
    if p.endswith("/c4"):
        exe += [p]
    else:
        for path, subdirs, fil in os.walk(p):
            for name in fil:
                if name == "c4":
                    exe += [os.path.join(path, name)]

print("gernating input files...")

for i in tqdm(range(20)):
    size = i * 50000
    if not os.path.exists("./sample"):
        os.makedirs("./sample")
    file = open("./sample/test" + str(size) + ".c", "a+")
    while os.path.getsize("./sample/test" + str(size) + ".c") < size:
        file.write("int a = 0; char* s = \"afsdgdsfg46546&\"; float floatintvoid = 3.46546465;")
    file.close()
    files += ["./sample/test" + str(size) + ".c"]
    int += [size / 1000]
    ref += [0.01 + i * 0.006]

for e in exe:
    val[e] = []

print("lexing...")
for f in tqdm(files):
    for e in exe:
        start = timer()
        os.system("./" + e + " --tokenize " + f + " > /dev/null")
        val[e] += [(timer() - start)]

plt.figure(figsize=(15, 5))

for e in exe:
    plt.plot(int, val[e], label=e + " (" + str(datetime.datetime.fromtimestamp(os.path.getctime(e)).date()) + ")")
#plt.plot(int, ref, "-o", label="ref")
plt.legend()
plt.xlabel("Input size [kByte]")
plt.ylabel('Runtime [seconds]')
plt.savefig("stat.svg", dpi=100)
