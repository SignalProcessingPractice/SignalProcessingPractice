#!/usr/bin/env python3

import argparse
import sys

from linter_config import TARGET_DIR, find_source_files, get_diff_files, run_for_files

parser = argparse.ArgumentParser()
parser.add_argument(
    "--diff-base",
    help="指定した git ref との差分に含まれるファイルのみを対象にする (例: origin/develop)",
)
parser.add_argument(
    "--check",
    action="store_true",
    help="ファイルを書き換えず, フォーマット違反があれば失敗させる (CI 向け)",
)
args = parser.parse_args()

files = (
    get_diff_files(args.diff_base, TARGET_DIR)
    if args.diff_base
    else find_source_files(TARGET_DIR)
)


def build_command(file):
    if args.check:
        return ["clang-format", "--dry-run", "--Werror", str(file)]
    return ["clang-format", "-i", str(file)]


failed = run_for_files(files, build_command)

sys.exit(1 if failed else 0)
