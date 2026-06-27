#!/usr/bin/env python3

import argparse
import sys

from linter_config import TARGET_DIR, BUILD_DIR, run_for_all_files

parser = argparse.ArgumentParser()
parser.add_argument("--fix", action="store_true", help="Apply fixes automatically")
args = parser.parse_args()

failed = run_for_all_files(
    TARGET_DIR,
    lambda file: [
        "clang-tidy",
        *(["-fix"] if args.fix else []),
        "-p",
        BUILD_DIR,
        str(file),
    ],
)

sys.exit(1 if failed else 0)