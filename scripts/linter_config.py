from pathlib import Path
import subprocess
import sys

SCRIPT_DIR = Path(__file__).resolve().parent

if Path.cwd().resolve() != SCRIPT_DIR:
    sys.exit(
        f"Error: このスクリプトは scripts/ ディレクトリから実行してください。\n"
        f"  cd {SCRIPT_DIR} && python3 {Path(sys.argv[0]).name}"
    )

BUILD_DIR = "../build/gtest-clang"

TARGET_DIR = Path("..")

EXCLUDE_DIRS = {
    "build",
    ".git",
    # CI (jurplel/install-qt-action) がリポジトリ直下にインストールする Qt SDK.
    # サードパーティコードのため解析対象から除外する.
    "qt",
}

SOURCE_EXTENSIONS = {
    ".c",
    ".cc",
    ".cpp",
    ".cxx",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
}


def find_source_files(root: Path):
    for path in root.rglob("*"):
        if (
            path.is_file()
            and path.suffix.lower() in SOURCE_EXTENSIONS
            and not any(part in EXCLUDE_DIRS for part in path.parts)
        ):
            yield path


def get_diff_files(base_ref: str, root: Path):
    """base_ref との差分でマージベース以降に追加・変更されたソースファイルを取得する."""

    result = subprocess.run(
        [
            "git",
            "diff",
            "--name-only",
            "--diff-filter=ACMR",
            f"{base_ref}...HEAD",
        ],
        cwd=root,
        capture_output=True,
        text=True,
        check=True,
    )

    for line in result.stdout.splitlines():
        path = (root / line).resolve()
        if (
            path.is_file()
            and path.suffix.lower() in SOURCE_EXTENSIONS
            and not any(part in EXCLUDE_DIRS for part in path.parts)
        ):
            yield path


def run_for_files(files, command_builder):
    failed = False

    for file in sorted(files):
        command = command_builder(file)

        print(" ".join(command))

        result = subprocess.run(command)

        if result.returncode != 0:
            failed = True

    return failed


def run_for_all_files(root: Path, command_builder):
    return run_for_files(find_source_files(root), command_builder)
