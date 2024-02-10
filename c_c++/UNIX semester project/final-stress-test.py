#!/usr/bin/env python3

import subprocess
import random
import string
import argparse

parser = argparse.ArgumentParser(
    prog="Tuple space stress tester",
)
parser.add_argument("--log", help="enable shell logs", action="store_true")
parser.add_argument(
    "-n", "--count", type=int, default=100, help="number of operations to do"
)
parser.add_argument(
    "--seed", help="set random seed to 0 (for debugging)", action="store_true"
)


def make_random_tuple_cmds():
    def random_int():
        data = str(random.randint(1, 1000))
        pattern = f"integer:{data}"
        return data, pattern

    def random_string():
        length = random.randint(3, 15)
        data = "".join(random.choices(string.ascii_lowercase, k=length))
        data = f'"{data}"'
        pattern = f"string:{data}"
        return data, pattern

    # Bez floatów, bo nie można użyć ==.
    length = random.randint(1, 5)
    random_funcs = [random_int, random_string]
    segments = [random.choice(random_funcs)() for _ in range(length)]
    data = "TSoutput " + ", ".join(s[0] for s in segments)
    pattern = "TSinput " + ", ".join(s[1] for s in segments) + " 2000"
    return data, pattern


def spawn(cmd, log):
    args = ["./build/shell", "./build/demon", "--clean"]
    if log:
        args.append("--log")
    proc = subprocess.Popen(
        args,
        stdin=subprocess.PIPE,
        stdout=0,
        stderr=0,
        text=True,
    )
    assert proc.stdin is not None
    proc.stdin.write(cmd)
    proc.stdin.close()
    return proc


def main():
    args = parser.parse_args()
    if args.seed:
        random.seed(0)
    cmds = []
    for _ in range(args.count):
        put, get = make_random_tuple_cmds()
        cmds.append(put)
        cmds.append(get)

    random.shuffle(cmds)

    processes = []
    i = 1
    for cmd in cmds:
        print(f"SPAWN {i}: {cmd}")
        i += 1
        processes.append(spawn(cmd, args.log))

    for proc in processes:
        proc.wait()


if __name__ == "__main__":
    main()
