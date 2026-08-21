# OthelloBoard Test Guide

This guide explains how to compile, run, and debug the `test_OthelloBoard.cpp` test file for the `OthelloBoard` class.

## Repository layout

The relevant layout is:

- `include/OthelloBoard.hpp`
- `src/OthelloBoard.cpp`
- `tests/test_OthelloBoard/test_OthelloBoard.cpp`

The test file uses the header and the implementation file. That means the compiler must compile and link both files together.

---

## 1. Compile and run from the project root

Open a terminal at the repository root:

`\Othello_MinimaxWithAlpha-Beta-Pruning`

Run this command:

```powershell
g++ -std=c++17 -g -Iinclude src/OthelloBoard.cpp tests/test_OthelloBoard/test_OthelloBoard.cpp -o tests/test_OthelloBoard/test_OthelloBoard.exe
```

Explanation:

- `-std=c++17` enables C++17 language support.
- `-g` includes debug symbols, which are needed for debugging.
- `-Iinclude` tells the compiler where to find header files.
- `src/OthelloBoard.cpp` is the implementation file for the `OthelloBoard` class.
- `tests/test_OthelloBoard/test_OthelloBoard.cpp` is the test file.
- `-o tests/test_OthelloBoard/test_OthelloBoard.exe` writes the executable into the test folder.

Then run the executable:

```powershell
tests\test_OthelloBoard\test_OthelloBoard.exe
```

If the build succeeds, the program should print:

```text
All OthelloBoard tests passed.
```

---

## 2. Compile and run from the test folder

Open a terminal in the test folder:

`\Othello_MinimaxWithAlpha-Beta-Pruning\tests\test_OthelloBoard`

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
    "${workspaceFolder}/tests/test_OthelloBoard/test_OthelloBoard.cpp",
    "-o",
    "${workspaceFolder}/tests/test_OthelloBoard/test_OthelloBoard.exe"
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
  "program": "${workspaceFolder}/tests/test_OthelloBoard/test_OthelloBoard.exe",
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
4. Set breakpoints inside `tests/test_OthelloBoard/test_OthelloBoard.cpp` or `src/OthelloBoard.cpp`.
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
g++ -std=c++17 -g -Iinclude src/OthelloBoard.cpp tests/test_OthelloBoard/test_OthelloBoard.cpp -o tests/test_OthelloBoard/test_OthelloBoard.exe
```

### Build from test folder

```powershell
g++ -std=c++17 -g -I../../include ../../src/OthelloBoard.cpp test_OthelloBoard.cpp -o test_OthelloBoard.exe
```

### Run the test

```powershell
tests\test_OthelloBoard\test_OthelloBoard.exe
```

### Debug in VS Code

- Use a task that compiles both source files
- Use a launch config pointing to the compiled executable
- Enable `-g` in the compile command

---

## 9. Running and debugging `test_BoardUtils.cpp`

The `BoardUtils` test file is located at `tests/test_BoardUtils/test_BoardUtils.cpp` and exercises the `BoardUtils` methods for formatting, printing, and coordinate conversion.

### 9.1. Compile and run from the project root

Open a terminal in the repository root. If you are already inside the repository, you can use:

```powershell
cd .\${workspaceFolder}
```

If you are starting from a parent folder, navigate to the repository folder first:

```powershell
cd .\Othello_MinimaxWithAlpha-BetaPruning
```

Run:

```powershell
g++ -std=c++17 -g -Iinclude src/BoardUtils.cpp src/OthelloBoard.cpp tests/test_BoardUtils/test_BoardUtils.cpp -o tests/test_BoardUtils/test_BoardUtils.exe
```

Then execute:

```powershell
tests\test_BoardUtils\test_BoardUtils.exe
```

Expected output:

```text
All BoardUtils tests passed.
```

### 9.2. Compile and run from the test folder

Open a terminal in the test folder. From the repository root, use:

```powershell
cd .\tests\test_BoardUtils
```

Run:

```powershell
g++ -std=c++17 -g -I../../include ../../src/BoardUtils.cpp ../../src/OthelloBoard.cpp test_BoardUtils.cpp -o test_BoardUtils.exe
```

Then run:

```powershell
.\test_BoardUtils.exe
```

### 9.3. Why both implementation files are needed

The `BoardUtils` test uses both `BoardUtils` and `OthelloBoard` behavior. The test file creates `OthelloBoard` objects and also calls methods from `BoardUtils`, so the compilation step must include:

- `src/BoardUtils.cpp`
- `src/OthelloBoard.cpp`

If you compile only the test file, the build will fail with linker errors such as `undefined reference`.

### 9.4. Debugging `test_BoardUtils.cpp` in VS Code

To debug this file in Visual Studio Code, use a task that includes both implementation files.

Example build task command:

```powershell
g++ -std=c++17 -g -I${workspaceFolder}/include ${workspaceFolder}/src/BoardUtils.cpp ${workspaceFolder}/src/OthelloBoard.cpp ${workspaceFolder}/tests/test_BoardUtils/test_BoardUtils.cpp -o ${workspaceFolder}/tests/test_BoardUtils/test_BoardUtils.exe
```

Then use a launch configuration that points to:

Example `launch.json` entry:
```json
{
  "name": "Debug BoardUtils test",
  "type": "cppdbg",
  "request": "launch",
  "program": "${workspaceFolder}/tests/test_BoardUtils/test_BoardUtils.exe",
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

Recommended debugging steps:

1. Set breakpoints inside `tests/test_BoardUtils/test_BoardUtils.cpp`.
2. Build the test with the command above.
3. Start the debugger from the Run and Debug panel.
4. Step through the test functions to inspect the values returned by `BoardUtils::formatBoard`, `BoardUtils::printBoard`, `BoardUtils::toCoordinates`, and `BoardUtils::toIndex`.

### 9.5. Troubleshooting the BoardUtils test

- If you see `undefined reference`, ensure both `src/BoardUtils.cpp` and `src/OthelloBoard.cpp` are included in the build command.
- If the debugger cannot launch the executable, confirm that `tests/test_BoardUtils/test_BoardUtils.exe` exists after the build.
- If the test fails, check the assertion line in `tests/test_BoardUtils/test_BoardUtils.cpp` to see which case is failing.
