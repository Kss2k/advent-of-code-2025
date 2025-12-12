from functools import cache

filename = "input.txt"

class Node:
    def __init__(self, name: str):
        self.name = name
        self.connections = []

    def add(self, node):
        self.connections.append(node)

    def __str__(self):
        return f"{self.name}: {[x.name for x in self.connections]}"

    def __repr__(self):
        return self.__str__()


NODES: dict[str, Node] = {}

def readline(line: str) -> None:
    if not line:
        return None

    buffer: list[str] = []
    name: str = ""
    sep: tuple[str] = (":", " ")

    for c in line:
        if c in sep and name:
            buffer.append(name)
            name = ""
        elif c not in sep:
            name += c
    
    if name:
        buffer.append(name)

    if buffer and buffer[0] not in NODES.keys():
        node: Node = Node(name = buffer[0])
        NODES[buffer[0]] = node
        buffer = buffer[1:]
    elif buffer:
        node: Node = NODES[buffer[0]]
        buffer = buffer[1:]
    else:
        return None

    for nm in buffer:
        if nm not in NODES.keys():
            NODES[nm] = Node(nm)            
        node.add(NODES[nm])

    return None

with open(filename, "r") as file:
    input = file.read().split("\n")
    for line in input:
        readline(line)

NODES 
for v in NODES.values():
    print(v)

def printpath(path: list[str]) -> None:
    print("->".join(path))


@cache
def count_paths2target(start: Node,
                       target: Node,
                       passes_0: str | None = None,
                       passes_1: str | None = None,
                       passed_0: bool = False,
                       passed_1: bool = False):
    passed_0 = (
        passed_0 or
        passes_0 is None or
        start.name == passes_0
    )
    
    passed_1 = (
        passed_1 or
        passes_1 is None or
        start.name == passes_1
    )

    if start.name == target.name:
        return 1, int(passed_0 and passed_1)

    count0: int = 0
    count1: int = 0
    for nextnode in start.connections:
        counts = count_paths2target(start    = nextnode,
                                    target   = target,
                                    passes_0 = passes_0,
                                    passes_1 = passes_1,
                                    passed_0 = passed_0,
                                    passed_1 = passed_1)
        count0 += counts[0]
        count1 += counts[1]

    return count0, count1

count_paths2target(start = NODES["you"], target = NODES["out"])
count_paths2target(start = NODES["svr"], target = NODES["out"], 
                   passes_0 = "dac", passes_1 = "fft")
