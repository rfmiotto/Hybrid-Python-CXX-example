#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

def main():
    # The module to generate stubs for
    module = "myproject.core"

    # Output directory: must match Python package source layout
    output_dir = Path(__file__).parent.parent / "src"

    # Ensure output dir exists
    output_dir.mkdir(parents=True, exist_ok=True)

    # Run pybind11-stubgen
    subprocess.run(
        [sys.executable, "-m", "pybind11_stubgen", module, "-o", str(output_dir)],
        check=True
    )

    print(f"Stubs generated for {module} in {output_dir}")

if __name__ == "__main__":
    main()
