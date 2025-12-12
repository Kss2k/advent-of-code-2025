# Trick questions today...

filepath = "input.txt"

import numpy as np
import re

class Shape:
    def __init__(self, shape: str):
        arr = np.array([list(row) for row in shape.split("\n") if row], dtype = "U1")

        
        self.shape = shape
        self.arr = arr
        self.n   = (arr=="#").sum()
        self.m   = (arr!="#").sum()
        self.N   = self.n + self.m


    def __repr__(self):
        return self.arr.__repr__()

class Box:
    def __init__(self, dim: tuple[int, int], counts: list[int]):
        self.dim    = dim
        self.n      = dim[0] * dim[1]
        self.counts = counts

SHAPES = {}
BOXES = []

with open(filepath, "r") as file:
    input = file.read()
    lines = input.split("\n")

    i   = -1
    shape = ""

    pat0 = "([0-9]):"
    pat1 = "([0-9]+)x([0-9]+): (.*)"
    for line in lines:
        if re.search(pat1, line):
            rx = re.search(pat1, line)
            dim = (int(rx.group(1)), int(rx.group(2)))
            counts = [int(x) for x in rx.group(3).split(" ") if x]
            BOXES.append(Box(dim = dim, counts = counts))        
        elif re.search(pat0, line):
            i = int(line[0])
        elif line and i >= 0:
            shape += "\n" + line
        elif line == '' and i >= 0:
            SHAPES[i] = Shape(shape)
            shape = ""

count_t1 = 0
for box in BOXES:
    m = box.dim[0] - box.dim[0] % 3
    k = box.dim[1] - box.dim[1] % 3
    N = m * k


    nmin = 0
    nmax = 0
    for i, count in enumerate(box.counts):
        shape = SHAPES[i]
        nmin += shape.n * count
        nmax += 9 * count

    if nmin > N:
        continue
    elif nmax <= N:
        count_t1 += 1

print(count_t1)
