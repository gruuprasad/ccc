import sys
import matplotlib.pyplot as plt
import os
from timeit import default_timer as timer
from tqdm import tqdm

int = []
val = dict()

ref = []

t = [timer()]

i = 1

files = []

exe = sys.argv[1:]

print("gernating input files...")

for i in tqdm(range(20)):
    size = i * 50000
    if not os.path.exists("./sample"):
        os.makedirs("./sample")
    file = open("./sample/test" + str(size) + ".c", "a+")
    while os.path.getsize("./sample/test" + str(size) + ".c") < size:
        file.write("int main()\n{\nprintf(\"Hello, World!\");\nint floati = 0.4564616646646;\n"
        + "char* voi = \"asdkjakf46513h...kjsfk\\n\\njaskjf\"\nreturn 0;\n}")
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
    plt.plot(int, val[e], "-o", label=e)
#plt.plot(int, ref, "-o", label="ref")
plt.legend()
plt.xlabel("Input size [kByte]")
plt.ylabel('Runtime [seconds]')
plt.savefig("stat.svg", dpi=100)
