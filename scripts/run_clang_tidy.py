#!/usr/bin/env python3
#
# run-clang-tidy を使用して並列実行する.
# compile_commands.json に含まれるファイルを対象に clang-tidy を実行する.
# サードパーティコード (_deps/) は除外する.
#

import argparse
import os
import subprocess
import sys

from linter_config import BUILD_DIR

parser = argparse.ArgumentParser()
parser.add_argument("--fix", action="store_true", help="Apply fixes automatically")
args = parser.parse_args()

cmd = [
    "run-clang-tidy",
    f"-j{os.cpu_count() or 1}",
    "-p", BUILD_DIR,
    "-header-filter=.*/lib/(inc|src)/.*",
    *(["-fix"] if args.fix else []),
    r"^(?!.*/_deps/).*/(lib)/.*\.(cpp|cc|c|cxx)$",
]

print(" ".join(cmd))
result = subprocess.run(cmd)
sys.exit(result.returncode)
