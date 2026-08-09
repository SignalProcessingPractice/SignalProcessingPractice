#!/usr/bin/env python3
#
# run-clang-tidy を使用して並列実行する.
# compile_commands.json に含まれるファイルを対象に clang-tidy を実行する.
# サードパーティコード (_deps/) は除外する.
#
# --diff-base を指定した場合, 指定した git ref との差分に含まれる lib/ 配下の
# .cpp ファイルのみを対象にする. なお, .cpp を伴わないヘッダ単体の変更は,
# それを include する .cpp が差分に含まれていない限り検査対象にならない.
#

import argparse
import os
import re
import subprocess
import sys

from linter_config import BUILD_DIR, TARGET_DIR, get_diff_files

parser = argparse.ArgumentParser()
parser.add_argument("--fix", action="store_true", help="Apply fixes automatically")
parser.add_argument(
    "--diff-base",
    help="指定した git ref との差分に含まれる lib/ 配下の .cpp ファイルのみを対象にする (例: origin/develop)",
)
args = parser.parse_args()

CPP_EXTENSIONS = {".cpp", ".cc", ".c", ".cxx"}

if args.diff_base:
    target_files = [
        file
        for file in get_diff_files(args.diff_base, TARGET_DIR)
        if file.suffix.lower() in CPP_EXTENSIONS and "lib" in file.parts
    ]

    if not target_files:
        print("No changed lib/ source files to check.")
        sys.exit(0)

    file_filters = [re.escape(str(file)) for file in target_files]
else:
    file_filters = [r"^(?!.*/_deps/).*/(lib)/.*\.(cpp|cc|c|cxx)$"]

cmd = [
    "run-clang-tidy",
    f"-j{os.cpu_count() or 1}",
    "-p", BUILD_DIR,
    "-header-filter=.*/lib/(inc|src)/.*",
    *(["-fix"] if args.fix else []),
    *file_filters,
]

print(" ".join(cmd))
result = subprocess.run(cmd)
sys.exit(result.returncode)
