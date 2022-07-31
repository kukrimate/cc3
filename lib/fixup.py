#!/usr/bin/python3

import sys

START   = "$$\n"
END     = "$$\n"

if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} FILE", file=sys.stderr)
    exit(1)

# Target path
path = sys.argv[1]

# Read file
with open(path, "r") as f:
    data = f.read()

# Output data
outdata = ""

# Find all macros
previdx = 0

while True:
    startidx = data.find(START, previdx)
    if startidx < 0:
        break

    endidx = data.find(END, startidx + len(START))
    if endidx < 0:
        break

    # Append preceeding text
    outdata += data[previdx:startidx]

    # Get macro lines
    lines = data[startidx + len(START):endidx].split("\n")
    if lines[-1] == "":
        lines.pop(-1)

    # Append each line properly padded
    for line in lines:
        outdata += f"{line.rstrip().ljust(79)}\\\n"

    previdx = endidx + len(END)

# Rest of the file
outdata += data[previdx:]

# Write file
with open(path, "w") as f:
    f.write(outdata)
