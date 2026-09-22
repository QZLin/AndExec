# And Executer (&Exec)

Use `&Exec` you can run windows console application background without popup cmd window.

## Usage

** Different image name will lead to different behaviour

```bash
andexec <CommandLine> # normal behaviour
andexec_d <CommandLine> <WorkDir> # specify workdir
andexec_w <CommandLine> # wait subprocess exit
andexec_wd <CommandLine> <WorkDir> # specify workdir and wait subprocess exit

```

## Build Requirements

* MSVC
* Windows SDK
* (Optional) cmake

(Everything required can install via visual studio)
