# And Executer (&Exec)

Use `&Exec` you can run windows console application background without popup cmd window.

## Usage

```bash
AndExec.exe [-w] [-d] <Command Line>
	-w wait process exit
	-d specify working directory
Example:
AndExec -d C:\User\Tester\Desktop python -m http.server
AndExec -w -d C:\User\Tester\Desktop "node test.js -a -b -c"
AndExec cmd /k "ping example.com"
```

## Build Requirements

* MSVC
* Windows SDK

(Everything required can install via visual studio)