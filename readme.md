# And Executer (&Exec)

Use `&Exec` you can run windows console application background without popup cmd window.

## Usage

```bash
AndExec.exe <CommandLine> [WorkDir] [Flags]
AndExec.exe <CommandLine>|<WorkDir>
AndExec.exe [Flags]|<CommandLine>|<WorkDir>
#Example:
AndExec cmd /k "ping example.com"
AndExec python -m http.server|C:\User\Tester\Desktop
AndExec w|node test.js -a -b -c|C:\User\Tester\Desktop
# \|
AndExec w|cmd /c ls | grep a|C:\User\Tester\Desktop
```

## Build Requirements

* MSVC
* Windows SDK
* (Optional) cmake

(Everything required can install via visual studio)