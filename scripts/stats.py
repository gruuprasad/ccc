import sys
import matplotlib.pyplot as plt
import os
import datetime
import numpy as np
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

for i in tqdm(range(21)):
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

for e in exe:
  print("benchmark " + e)
  timeout = 0;
  for f in tqdm(files):
      if timeout < 0.15:
        start = timer()
        os.system("./" + e + " --tokenize " + f + " > /dev/null")
        timeout = timer() - start;
      val[e] += [min(timeout, 0.15)]

plt.figure(figsize=(16, 9))

plt.plot(int, val[exe[0]], "-o", label=exe[0].replace("./build/", "").replace("/c4", ""))

for e in exe[1:]:
  try:
    file = open(e + ".flags", "r")
    plt.plot(int, val[e], ":", label=e.replace("./build/", "").replace("/c4", "")[:14] + " [" + file.read().strip() + "]")
  except FileNotFoundError:
    plt.plot(int, val[e], ":", label=e.replace("./build/", "").replace("/c4", "")[:14])
#plt.plot(int, ref, "-o", label="ref")
plt.legend(frameon=False)
plt.xlabel("Input size [kByte]")
plt.xticks(np.arange(0, 1050, step=50))
plt.ylabel('Runtime [seconds]')
plt.savefig("benchmark.svg", dpi=100, bbox_inches="tight")
