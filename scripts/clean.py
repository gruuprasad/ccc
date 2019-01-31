import os
import os.path
import re
import string
import random


COMMENTS = re.compile(r'(//[^\n]*(?:\n|$)) | (/\*.*?\*/)', re.VERBOSE)

def id_generator():
  return ''.join(random.choice(string.digits) for _ in range(10))

def remove_comments(content):
    return COMMENTS.sub('\n', content)

delete = ["#", "float", "double", "long", "short", "static", "signed ", "enum", "for", "extern ", "typedef", "inline", "const", "0x", "^", "switch", "&&", "__builtin", "<<", ">>", "::", "= {", "union ", " asm "]

i = 0
for filename in os.listdir(os.getcwd()):
  if not filename.endswith(".c"):
    continue;
  f = open(filename, "r")
  ctx = f.read()
  f.close()
  os.remove(filename)
  ctx = remove_comments(ctx)
  b = False
  for d in delete:
    if d in ctx:
      b = True
      break
  if b:
    continue
  ctx  = ctx.replace("/", " * ").replace(" % ", " * ").replace(" > ", " < ").replace("+=", "=").replace("++", "").replace("--", "").replace(" | ", " + ").replace(" & ", " + ").replace("~", "-").replace("\t", " ")
  while "  " in ctx:
    ctx = ctx.replace("  ", " ")
  while "\n\n" in ctx:
    ctx = ctx.replace("\n\n", "\n")
  if ctx and ctx[0] == '\n':
    ctx = ctx[1:]
  i = i + 1
  filename = "ccc_" + str(i).zfill(4) + "_" + id_generator() + ".c";
  while os.path.exists(filename):
    filename = "ccc_" + str(i).zfill(4) + "_" + id_generator() + ".c";
  f = open(filename, "w")
  f.write(ctx)
  f.close()
  print(str(i) + ": " + filename)
