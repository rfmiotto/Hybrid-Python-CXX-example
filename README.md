# Modern hybrid Python + C++ project

You have a simple Python project, but you want part of the code implemented in C++ for performance. How to do it?

Here we show how you can configure the project such that it uses UV to manage the environment and pybind11 for the C++/Python bridge.


| Component             | Role                                                                               |
| --------------------- | ---------------------------------------------------------------------------------- |
| **UV**                | Environment & dependency manager — replaces `venv`, `pip`, and `poetry`.           |
| **scikit-build-core** | The modern PEP 517 build backend that lets Python build CMake-based projects.      |
| **CMake**             | The actual build system for compiling the C++ code.                                |
| **pybind11**          | A lightweight C++ library that exposes C++ functions/classes to Python seamlessly. |


So the workflow is:

```
[Your C++ code] --(CMake + pybind11)--> [shared library .so]
[scikit-build-core] --(packaging)--> [Python wheel with extension]
[uv] --(pip install)--> [site-packages/myproject/core.so]
```

Python can now do:
```
from myproject import core  # core is the name of our C++ module
```


## Build and run using UV:

Install dependencies and build the extension:

```
uv sync --dev
uv run pip install .
```

Now, let's verify if it is working:
```
uv run python -c "from myproject import core; print(core.fast_sum([1,2,3]))"
```

Then, run:
```
uv run python src/main.py
```

Or test:
```
uv run pytest
```

-----------

## What is going on?


When you run `uv sync --dev`, it installs those packages (in an isolated environment) for building and testing.

When you run `uv run pip install .`, this is a regular install (there is also an editable install). Here is what happens:

1. `pip` reads your `pyproject.toml`.
2. It installs build requirements (`scikit-build-core`, `pybind11`, etc.) in a temporary build environment.
3. `scikit-build-core` launches `CMake`, pointing it to `CMakeLists.txt`.
4. `CMake` compiles your C++ into a shared library.
  scikit-build-core executes:
  1. `cmake -S . -B build`
  2. `cmake --build build`
  3. `cmake --install build --prefix <temporary wheel dir>`
  That third step copies the compiled shared library into your Python wheel, according to the `install()` rule in `CMakeLists.txt`
5. The `install()` directive copies the `.so` into `myproject/` inside a wheel.
6. `pip` installs that wheel into your environment.



You can list the installed package inside the environment running `uv run python -m site`.
To see the real `.so` that Python imports -- distinct from the one in `build/`, run:

```
ls $(uv run python -c "import site; print(site.getsitepackages()[0])")/myproject
```

----------

**What is the difference between regular and editable installs?**

When we build the C++ code using the regular install (`uv run pip install .`), the `.so` will be in the venv under `site-packages/myproject`. You will see something like:
```
.venv/lib/python3.10/site-packages/myproject/core.cpython-310-x86_64-linux-gnu.so
```

The command asks your backend (`scikit-build-core.build`) to:
- Configure and invoke CMake,
- Compile your C++ extensions,
- Produce installable artifacts:
  - `dist/myproject-0.1.0-cp310-cp310-linux_x86_64.whl`, which is a wheel
  - `dist/myproject-0.1.0.tar.gz`, which is a sdist
- Copy files into your environment’s `site-packages` directory (installs the wheel).
- The result:
  - You can `import myproject`
  - But changes to your source (C++ or Python) require reinstalling.


This is what happens when you install your project as a normal package (like installing numpy from PyPI):
- Your project is built into a wheel (`.whl`) — a self-contained binary distribution.
- The wheel is installed into your environment’s `site-packages` directory.
- All compiled artifacts (`.so` files, etc.) are copied into the `site-packages` version of your package.
- The original `src/` or `build/` directories no longer matter; Python uses the installed copy.
- Performance: ✅ Maximum — everything is compiled, optimized, and isolated.
- Useful for:
  - Distributing your package to others.
  - Production deployments.
  - Reproducible builds.


--------

If you’re doing active development and don’t want to reinstall every time, you can use an editable build (aka editable install or development install):
```
uv run pip install -e .
```
Here’s what happens:

- `pip` runs your build backend (`scikit-build-core`) to compile any C/C++ extensions (like your `.so` file);
- It then installs a pointer (a `.pth` file) in your environment that points to your local `src/` directory;
- The result is: Python imports the local source directly, and not a copied version.
- This means:
  - Any changes to Python files (`src/myproject/*.py`) take effect immediately;
  - When you modify C++ code (`core.cpp`), you just need to recompile (`uv build` or `cmake --build build`) — no reinstall needed, since the `.so` file lives in your `src/myproject/`.


So, an editable install works differently:
- Instead of copying your package into `site-packages`, it places a link (a `.pth` file or symlink) pointing to your source tree (`src/myproject`).
- This means `import myproject` actually loads code from your local folder, not from a copied installation.
- Any edits to `.py` files (and recompiled `.so` files) are immediately visible without reinstalling.
- Performance: ⚙️ Identical at runtime — once imported, it runs just as fast, because Python still executes the same bytecode and C extensions.
- Useful for:
  - Active development.
  - Debugging or testing changes without reinstalling.
  - Collaborative projects.


As mentioned earlier, when you modify a C++ code, we need to rebuild it ([see here how to do it](#how-to-rebuild-cpp)). In editable mode, Python immediately sees the new `.so` file without reinstalling. That is, the `.so` is not in `site-packages` - it stays in your `build/` directory, and Python is told to look in your `src/` folder directly. So, imports work like this:

```
src/myproject/__init__.py     ✅
build/core.cpython-310-...so  ✅ (loaded via the editable link)
```
Even though the `.so` lives in `build/`, Python still loads it thanks to the build backend’s path hooks.


<a name="how-to-rebuild-cpp"></a>
**How to rebuild C++ code?**

If you modify your C++ code (e.g., `core.cpp`), you must rebuild the extension so that the `.so` file in your local directory updates. Python will still import it directly, so the rebuild is immediately picked up. How can we rebuild?

If you edit `core.cpp`, simply rerun:
```
uv run pip install -e .
```
or, alternatively:
```
uv build
```

There is also another way of rebuilding the project:
```
cmake --build build
```
However, you can only use if:
- You’ve already done `uv run pip install -e .` once (so the package is linked),
- And your `CMakeLists.txt` has the `POST_BUILD` copy step that moves the `.so` into `src/myproject`

This approach is actually how many developers do “hot rebuilds” without reinstalling the package every time.

When you run `uv build`, it internally:
1. Configures CMake (runs `cmake -S . -B build/ ...`)
2. Builds (runs `cmake --build build/`)
3. Copies artifacts into wheel structure
4. Produces the `.whl`

But, when you call `uv run cmake --build build` directly, you’re manually triggering step 2 only — the compile step.



| Command                      | Meaning                                                                                        | Editable? | Produces Wheel?             | Installs? |
| ---------------------------- | ---------------------------------------------------------------------------------------------- | --------- | --------------------------- | --------- |
| 🧱 `uv build`                | Builds your project (via `scikit-build-core`) into a wheel (`.whl`) or source dist (`.tar.gz`) | ❌ No      | ✅ Yes (`dist/*.whl`)        | ❌ No      |
| 🧩 `uv run pip install .`    | Installs the package normally from source (non-editable)                                       | ❌ No      | (builds internally)         | ✅ Yes     |
| 🧩 `uv run pip install -e .` | Installs in **editable (development)** mode                                                    | ✅ Yes     | (builds extension in-place) | ✅ Yes     |
 


Notice that when you install the project via `uv run pip install -e .`, `scikit-build-core` builds your C++ extension inside the virtual environment managed by `uv`. It automatically tells `CMake` where to find `pybind11`, because it resolves it via the Python package metadata. Hence, the build works during installation. However, if we need to rebuild the code, running `uv run cmake --build build` will skip that automatic integration layer, calling `CMake` directly. It will only work if you configured you `CMake` properly. Otherwise, it doesn't know where the `pybind11Config.cmake` file is, because it's installed inside your virtualenv, not in a system path. `scikit-build-core` automatically sets that up internally — that’s why the build works during installation, but we are skipping it when calling `Cmake` directly.


So, during active development it is convenient to avoid reinstalling every time and use the `.so`directly from the build directory using the editable build (`uv run pip install -e .`). Then, to rebuild the binary (if you modify you C++ code):
```
uv run cmake --build build
```
and immediately `uv run python src/main.py` will work, as Python will find the updated `.so` (via the editable link, without reinstalling).


| Task                                         | Recommended command                              | Why                                                 |
| -------------------------------------------- | ------------------------------------------------ | --------------------------------------------------- |
| 🧩 Edit C++ in development                   | `uv run cmake --build build`                     | Fast incremental rebuild using existing build cache |
| 🧩 Edit Python code only                     | Nothing — editable install updates automatically | No rebuild needed                                   |
| 🧩 Edit both and want to reconfigure cleanly | `uv build`                                       | Re-runs scikit-build-core and CMake configure step  |
| 🧱 Full reinstall (clean wheel + install)    | `uv run pip install .`                           | For testing non-editable installs                   |
| 🧱 Install in editable mode for dev          | `uv run pip install -e .`                        | Sets up dev workflow (so `.so` in src/ works)       |


A typical development workflow would be:

```
# 1. One-time setup
uv run pip install -e .

# 2. Edit your C++ file (e.g. src/myproject/core.cpp)
# 3. Rebuild just the native extension
uv run cmake --build build

# 4. Test it immediately
uv run pytest
```

While a typical production workflow would be:

```
#1️⃣1. Build the wheel
uv build

# 2. Test the built artifact
uv run pip install dist/myproject-0.1.0-cp310-cp310-linux_x86_64.whl

# 3. Or publish it
uv publish
```

-------


**How to check which `.so` Python is actually using?**
Run this:
```
uv run python -c "import myproject.core as c; print(c.__file__)"
```

So notice that when we run `uv run pip install .`, scikit-build-core executes:
1. `cmake -S . -B build`
2. `cmake --build build`
3. `cmake --install build --prefix <temporary wheel dir>`

That third step copies the compiled shared library (`.so`) into your Python wheel, according to the `install()` rule in `CMakeLists.txt`:
```
install(TARGETS core DESTINATION myproject)
```
So the installed `.so` ends up at:
```
.venv/lib/python3.10/site-packages/myproject/core.cpython-310-x86_64-linux-gnu.so
```
That’s what Python actually imports.

The version under `build/` is a build-time artifact, not the installed one.


