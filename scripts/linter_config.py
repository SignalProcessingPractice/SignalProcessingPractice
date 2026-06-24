from pathlib import Path
import subprocess

BUILD_DIR = "../build/app"

TARGET_DIR = Path("..")

EXCLUDE_DIRS = {
    "build",
    ".git",
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


def run_for_all_files(root: Path, command_builder):
    failed = False

    for file in sorted(find_source_files(root)):
        command = command_builder(file)

        print(" ".join(command))

        result = subprocess.run(command)

        if result.returncode != 0:
            failed = True

    return failed
