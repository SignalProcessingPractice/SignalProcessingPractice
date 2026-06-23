#!/usr/bin/env python3

from pathlib import Path
import sys

from linter_config import TARGET_DIR, BUILD_DIR, run_for_all_files

failed = run_for_all_files(
    TARGET_DIR,
    lambda file: [
        "clang-tidy",
        "-p",
        BUILD_DIR,
        str(file),
    ],
)

sys.exit(1 if failed else 0)