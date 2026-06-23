#!/usr/bin/env python3

from pathlib import Path
import sys

from linter_config import TARGET_DIR, run_for_all_files

failed = run_for_all_files(
    TARGET_DIR,
    lambda file: [
        "clang-format",
        "-i",
        str(file),
    ],
)

sys.exit(1 if failed else 0)