# And Executer (&Exec)

Use `&Exec` you can run windows console application background without popup cmd window.

## Usage

** Different image name will lead to different behaviour

```bash
andexec <CommandLine>                  # normal behaviour
andexec_d <CommandLine> <WorkDir>      # specify workdir
andexec_w <CommandLine>                # wait subprocess exit
andexec_wd <CommandLine> <WorkDir>     # specify workdir and wait subprocess exit
```

## Variants

All four executables are built from the same `and_exec.c`, differing only by
compile definitions:

| Executable   | `FLAG_ENABLE_WORKDIR` | `FLAG_ENABLE_WAIT` | Behaviour                            |
|--------------|-----------------------|--------------------|--------------------------------------|
| `andexec`    |                       |                    | run command line, return immediately |
| `andexec_d`  | ✓                    |                    | run in given working directory       |
| `andexec_w`  |                       | ✓                 | wait for subprocess to exit          |
| `andexec_wd` | ✓                    | ✓                 | working directory + wait             |

## Build

### CMake (recommended)

Any generator works, e.g. Ninja or Visual Studio:

```bash
# MinGW + Ninja
cmake -S . -B build -G Ninja
cmake --build build -j

# MSVC + Visual Studio generator
cmake -S . -B build
cmake --build build --config Release -j
```

The four executables land in the build output directory (with the
Visual Studio generator under `build/<Config>/`).

### Visual Studio

Open `AndExec.sln` and build — this produces the single MSVC variant only.

## Build Requirements

* MSVC or MinGW-w64
* Windows SDK
* (Optional) CMake, Ninja

(Everything required can install via Visual Studio or use mingw+cmake+ninja)
