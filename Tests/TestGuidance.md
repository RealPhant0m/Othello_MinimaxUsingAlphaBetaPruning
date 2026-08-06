# OthelloBoard Test Guide

This guide explains how to compile, run, and debug the `test_OthelloBoard.cpp` test file for the `OthelloBoard` class.

## Repository layout

The relevant layout is:

- `include/OthelloBoard.hpp`
- `src/OthelloBoard.cpp`
- `Tests/test_OthelloBoard/test_OthelloBoard.cpp`

The test file uses the header and the implementation file. That means the compiler must compile and link both files together.

---

## 1. Compile and run from the project root

Open a terminal at the repository root:

`\Othello_MinimaxWithAlpha-Beta-Pruning`

Run this command:

```powershell
g++ -std=c++17 -g -Iinclude src/OthelloBoard.cpp Tests/test_OthelloBoard/test_OthelloBoard.cpp -o Tests/test_OthelloBoard/test_OthelloBoard.exe
```

Explanation:

- `-std=c++17` enables C++17 language support.
- `-g` includes debug symbols, which are needed for debugging.
- `-Iinclude` tells the compiler where to find header files.
- `src/OthelloBoard.cpp` is the implementation file for the `OthelloBoard` class.
- `Tests/test_OthelloBoard/test_OthelloBoard.cpp` is the test file.
- `-o Tests/test_OthelloBoard/test_OthelloBoard.exe` writes the executable into the test folder.

Then run the executable:

```powershell
Tests\test_OthelloBoard\test_OthelloBoard.exe
```

If the build succeeds, the program should print:

```text
All OthelloBoard tests passed.
```

---

## 2. Compile and run from the test folder

Open a terminal in the test folder:

`\Othello_MinimaxWithAlpha-Beta-Pruning\Tests\test_OthelloBoard`

Run:

```powershell
g++ -std=c++17 -g -I../../include ../../src/OthelloBoard.cpp test_OthelloBoard.cpp -o test_OthelloBoard.exe
```

Then run:

```powershell
.\test_OthelloBoard.exe
```

---

## 3. Why the default VS Code "build active file" task fails

The current default build task only compiles the active `.cpp` file. It does not compile or link `src/OthelloBoard.cpp`, so the linker cannot find the `OthelloBoard` method implementations.

That is why you see errors like:

```text
undefined reference to `othello::OthelloBoard::OthelloBoard()`
```

The fix is to build both files together.

---

## 4. Recommended VS Code task for this test

If you want to run the test in VS Code, add a custom task that compiles both the implementation and the test.

Example `tasks.json` entry:

```json
{
  "label": "build OthelloBoard test",
  "type": "shell",
  "command": "C:\\msys64\\ucrt64\\bin\\g++.exe",
  "args": [
    "-std=c++17",
    "-g",
    "-I${workspaceFolder}/include",
    "${workspaceFolder}/src/OthelloBoard.cpp",
    "${workspaceFolder}/Tests/test_OthelloBoard/test_OthelloBoard.cpp",
    "-o",
    "${workspaceFolder}/Tests/test_OthelloBoard/test_OthelloBoard.exe"
  ],
  "options": {
    "cwd": "${workspaceFolder}"
  },
  "problemMatcher": ["$gcc"],
  "group": {
    "kind": "build",
    "isDefault": true
  }
}
```

After adding this task, use `Terminal > Run Task...` or press the VS Code build shortcut.

---

## 5. Recommended VS Code debug configuration

Use this `launch.json` configuration to debug the generated executable.

Example `launch.json` entry:

```json
{
  "name": "Debug OthelloBoard test",
  "type": "cppdbg",
  "request": "launch",
  "program": "${workspaceFolder}/Tests/test_OthelloBoard/test_OthelloBoard.exe",
  "args": [],
  "stopAtEntry": false,
  "cwd": "${workspaceFolder}",
  "environment": [],
  "externalConsole": false,
  "MIMode": "gdb",
  "miDebuggerPath": "C:\\msys64\\ucrt64\\bin\\gdb.exe",
  "preLaunchTask": "build OthelloBoard test"
}
```

Important points:

- `program` must point to the compiled executable.
- `preLaunchTask` must match the build task label.
- `miDebuggerPath` must point to your installed `gdb.exe`.

---

## 6. Full debug workflow

1. Make sure the custom build task is saved in `.vscode/tasks.json`.
2. Make sure the debug configuration is saved in `.vscode/launch.json`.
3. Build once with the task or use `Ctrl+Shift+B`.
4. Set breakpoints inside `Tests/test_OthelloBoard/test_OthelloBoard.cpp` or `src/OthelloBoard.cpp`.
5. Start the debug configuration.

If the executable is not built, the `preLaunchTask` will build it first.

---

## 7. Troubleshooting

- If you see `undefined reference`, verify that `src/OthelloBoard.cpp` is included in the compile command.
- If the debug session fails because `gdb.exe` is missing, install the MSYS2 UCRT toolchain or update `miDebuggerPath` to a valid GDB path.
- If the executable is not found, confirm the `-o` path in the build command and the `program` path in `launch.json`.

---

## 8. Quick summary

### Build from root

```powershell
g++ -std=c++17 -g -Iinclude src/OthelloBoard.cpp Tests/test_OthelloBoard/test_OthelloBoard.cpp -o Tests/test_OthelloBoard/test_OthelloBoard.exe
```

### Build from test folder

```powershell
g++ -std=c++17 -g -I../../include ../../src/OthelloBoard.cpp test_OthelloBoard.cpp -o test_OthelloBoard.exe
```

### Run the test

```powershell
Tests\test_OthelloBoard\test_OthelloBoard.exe
```

### Debug in VS Code

- Use a task that compiles both source files
- Use a launch config pointing to the compiled executable
- Enable `-g` in the compile command
