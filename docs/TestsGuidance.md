# Complete Guide to Building, Running, and Debugging the Tests

This guide explains how to build, run, and debug all three test programs in this repository:

1. `test_OthelloBoard.cpp`
2. `test_BoardUtils.cpp`
3. `test_OthelloAI.cpp`

The order is intentional. `OthelloBoard` is the core board implementation. `BoardUtils` uses `OthelloBoard`, and `OthelloAI` uses the board engine while testing the AI evaluation and search logic.

## Repository layout

Run commands from the repository root:

```text
Othello_MinimaxUsingAlphaBetaPruning/
|-- include/
|   |-- BoardUtils.hpp
|   |-- OthelloAI.hpp
|   `-- OthelloBoard.hpp
|-- src/
|   |-- BoardUtils.cpp
|   |-- OthelloAI.cpp
|   `-- OthelloBoard.cpp
|-- tests/
|   |-- test_BoardUtils/
|   |   `-- test_BoardUtils.cpp
|   |-- test_OthelloAI/
|   |   `-- test_OthelloAI.cpp
|   `-- test_OthelloBoard/
|       `-- test_OthelloBoard.cpp
|-- docs/
|   `-- TestsGuidance.md
`-- .vscode/
    |-- launch.json
    `-- tasks.json
```

## Prerequisites

Install a C++17 compiler and GDB. The VS Code tasks in this repository use the MSYS2 UCRT64 tools:

```text
C:\msys64\ucrt64\bin\g++.exe
C:\msys64\ucrt64\bin\gdb.exe
```

The commands below use `g++` for readability. If `g++` is not on your `PATH`, replace it with the full path to `g++.exe`, or run the commands from an MSYS2 UCRT64 terminal.

Open PowerShell at the repository root before using the root commands. In VS Code, open the integrated terminal with the repository folder as the workspace; it normally starts in the repository root. If you are starting from the repository's parent directory, use the relative project path:

```powershell
cd .\Othello_MinimaxUsingAlphaBetaPruning
```

The test programs use the standard C++ `assert` macro. If an assertion fails, the program stops at the failed assertion and returns a nonzero exit code. A successful program prints its corresponding `All ... tests passed.` message.

## Recommended test order

Run the tests in this order:

1. `test_OthelloBoard.cpp` verifies the core board state, board serialization, legal moves, and disc flipping.
2. `test_BoardUtils.cpp` verifies board formatting, stream output, and row/column/index conversion. It links against both `BoardUtils.cpp` and `OthelloBoard.cpp`.
3. `test_OthelloAI.cpp` verifies AI construction, search depth limits, heuristic calculations, stability checks, move selection, and visited-node counting.

This order makes failures easier to interpret: a board failure should be addressed before investigating a utility or AI failure.

---

## 1. OthelloBoard tests

### What this test covers

`tests/test_OthelloBoard/test_OthelloBoard.cpp` checks:

- The initial four-disc board position.
- Conversion between the board and its 64-character string representation.
- Generation of valid moves for Black on the initial board.
- Applying a move and flipping the captured disc.

### Compile and run from the repository root

From the repository root, compile the implementation and the test file together:

```powershell
g++ -std=c++17 -g -Iinclude src/OthelloBoard.cpp tests/test_OthelloBoard/test_OthelloBoard.cpp -o tests/test_OthelloBoard/test_OthelloBoard.exe
```

Explanation:

- `-std=c++17` enables C++17.
- `-g` adds debug information for GDB.
- `-Iinclude` makes headers in `include/` available.
- `src/OthelloBoard.cpp` supplies the production implementation.
- `tests/test_OthelloBoard/test_OthelloBoard.cpp` supplies the test program and its `main` function.
- `-o` chooses the output executable path.

Run the executable:

```powershell
.\tests\test_OthelloBoard\test_OthelloBoard.exe
```

Expected output:

```text
All OthelloBoard tests passed.
```

### Compile and run from the exact test directory

Change to the directory containing the test source:

```powershell
cd .\tests\test_OthelloBoard
```

Compile using paths relative to that directory:

```powershell
g++ -std=c++17 -g -I..\..\include ..\..\src\OthelloBoard.cpp test_OthelloBoard.cpp -o test_OthelloBoard.exe
```

Run it from the same directory:

```powershell
.\test_OthelloBoard.exe
```

Return to the repository root before following another root-based section:

```powershell
cd ..\..
```

### Debug in VS Code

The repository already contains a matching build task and launch configuration:

- Build task: `build OthelloBoard test`
- Debug configuration: `Debug OthelloBoard test`

Use this workflow:

1. Open the repository root as the VS Code workspace.
2. Open the Run and Debug view panel (or press Ctrl+Shift+D).
3. Select `Debug OthelloBoard test`.
4. Press `F5`.
5. VS Code runs the `build OthelloBoard test` pre-launch task.
6. The task creates `tests/test_OthelloBoard/test_OthelloBoard.exe` with debug symbols.
7. GDB launches the executable with the working directory set to the repository root.
8. Set breakpoints in `tests/test_OthelloBoard/test_OthelloBoard.cpp` or `src/OthelloBoard.cpp` before pressing `F5`.

You can build without starting the debugger by opening `Terminal > Run Task...` and selecting `build OthelloBoard test`.

### Debug directly with GDB

Build the program with `-g`, then start GDB from the repository root:

```powershell
gdb .\tests\test_OthelloBoard\test_OthelloBoard.exe
```

Useful GDB commands:

```text
break main
break testApplyMoveFlipsDiscs
run
next
continue
print boardString
bt
quit
```

`break` sets a breakpoint, `run` starts the program, `next` executes the next source line, `continue` runs to the next breakpoint, `print` displays a value, and `bt` prints the call stack.

---

## 2. BoardUtils tests

### What this test covers

`tests/test_BoardUtils/test_BoardUtils.cpp` checks:

- Formatting an empty board.
- Formatting a custom board state.
- Printing to a caller-provided `std::ostream`.
- Conversion of the first, middle, and last linear indexes to row and column coordinates.
- Conversion of boundary coordinates to linear indexes.
- Round-trip conversion for every board cell.

### Compile and run from the repository root

`BoardUtils.cpp` uses `OthelloBoard`, so both implementation files must be linked:

```powershell
g++ -std=c++17 -g -Iinclude src/BoardUtils.cpp src/OthelloBoard.cpp tests/test_BoardUtils/test_BoardUtils.cpp -o tests/test_BoardUtils/test_BoardUtils.exe
```

Run the executable:

```powershell
.\tests\test_BoardUtils\test_BoardUtils.exe
```

Expected output:

```text
All BoardUtils tests passed.
```

Compiling only `test_BoardUtils.cpp` will normally produce linker errors because the definitions in `BoardUtils.cpp` and `OthelloBoard.cpp` are missing.

### Compile and run from the exact test directory

Change to the directory containing the test source:

```powershell
cd .\tests\test_BoardUtils
```

Compile both required production files:

```powershell
g++ -std=c++17 -g -I..\..\include ..\..\src\BoardUtils.cpp ..\..\src\OthelloBoard.cpp test_BoardUtils.cpp -o test_BoardUtils.exe
```

Run it:

```powershell
.\test_BoardUtils.exe
```

Return to the repository root when finished:

```powershell
cd ..\..
```

### Debug in VS Code

The current `.vscode/launch.json` contains debugger configurations for `OthelloBoard` and `OthelloAI`, but not for `BoardUtils`. To debug `BoardUtils`, first build it with the command above. Then add this configuration inside the `configurations` array in `.vscode/launch.json`:

```jsonc
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
  "miDebuggerPath": "C:\\msys64\\ucrt64\\bin\\gdb.exe"
}
```

Use this workflow:

1. Compile the test from the repository root using the BoardUtils command above.
2. Set breakpoints in `tests/test_BoardUtils/test_BoardUtils.cpp`, `src/BoardUtils.cpp`, or `src/OthelloBoard.cpp`.
3. Open Run and Debug.
4. Select `Debug BoardUtils test`.
5. Press `F5`.
6. If the executable is missing or stale, rebuild it before starting the debugger.

For a complete one-click workflow, add a task named `build BoardUtils test` using the root compilation command, then add this property to the launch configuration:

```json
"preLaunchTask": "build BoardUtils test"
```

### Debug directly with GDB

From the repository root, build the executable and start GDB:

```powershell
gdb .\tests\test_BoardUtils\test_BoardUtils.exe
```

Useful breakpoints include:

```text
break main
break testRoundTripIndexConversion
break BoardUtils::formatBoard
run
next
continue
bt
quit
```

If GDB cannot resolve a function name, set the breakpoint in the source file using a line number or set it after the executable has loaded.

---

## 3. OthelloAI tests

### What this test covers

`tests/test_OthelloAI/test_OthelloAI.cpp` checks:

- Default construction and valid exploration-depth limits.
- Corner-control evaluation.
- Potential-corner evaluation.
- Axis and disc stability calculations.
- Mobility and coin-parity calculations.
- Combined board evaluation.
- Move selection at multiple exploration depths and for both players.
- The visited-node counter used to inspect minimax search work.

The test temporarily defines `private` as `public` before including `OthelloAI.hpp`. This is test-only access to private helper functions and fields; it does not change the production header. The test also includes `src/OthelloAI.cpp` and `src/OthelloBoard.cpp` directly because the private inline helper definitions are in the implementation file. Do not add `src/OthelloAI.cpp` or `src/OthelloBoard.cpp` a second time to this test's compile command, or duplicate definitions may occur.

### Compile and run from the repository root

Use the repository's existing command:

```powershell
g++ -std=c++17 -fpermissive -g -Iinclude tests/test_OthelloAI/test_OthelloAI.cpp -o tests/test_OthelloAI/test_OthelloAI.exe
```

The `-fpermissive` option is required by the current test setup because it includes implementation details directly for test access. The test source itself brings in the required AI and board implementations.

Run the executable:

```powershell
.\tests\test_OthelloAI\test_OthelloAI.exe
```

Expected output:

```text
All OthelloAI tests passed.
```

### Compile and run from the exact test directory

Change to the directory containing the test source:

```powershell
cd .\tests\test_OthelloAI
```

Compile using paths relative to that directory:

```powershell
g++ -std=c++17 -fpermissive -g -I..\..\include test_OthelloAI.cpp -o test_OthelloAI.exe
```

Run it:

```powershell
.\test_OthelloAI.exe
```

Return to the repository root when finished:

```powershell
cd ..\..
```

### Debug in VS Code

The repository already contains a matching build task and launch configuration:

- Build task: `build OthelloAI test`
- Debug configuration: `Debug OthelloAI test`

Use this workflow:

1. Open the repository root as the VS Code workspace.
2. Open Run and Debug.
3. Select `Debug OthelloAI test`.
4. Press `F5`.
5. VS Code runs `build OthelloAI test` first.
6. The task compiles the test with C++17, debug symbols, and `-fpermissive`.
7. GDB launches `tests/test_OthelloAI/test_OthelloAI.exe` from the repository root.
8. Set breakpoints in the test file or in the included AI and board implementation files.

You can build without debugging from `Terminal > Run Task...` by selecting `build OthelloAI test`.

The test at exploration depth 7 may take longer than the depth 0 and depth 1 cases because minimax explores more positions. Allow it to finish before treating a delay as a failure.

### Debug directly with GDB

Build the executable and start GDB from the repository root:

```powershell
gdb .\tests\test_OthelloAI\test_OthelloAI.exe
```

Useful breakpoints include:

```text
break main
break testChooseMoveAndVisitedNodeCount
break othello::OthelloAI::chooseMove
break othello::OthelloAI::minimaxUsingAlphaBetaPruning
run
next
continue
print move.row
print move.col
print ai.visitedNodesCount_
bt
quit
```

Because the test exposes private members only in the test translation unit, GDB can inspect those members while stopped in the test. If a breakpoint is not resolved before execution starts, use `rbreak` or set it after the program has loaded.

---

## VS Code debugging details

### Build tasks and launch configurations

The `.vscode/tasks.json` file currently provides build tasks for `OthelloBoard` and `OthelloAI`. Each task invokes the MSYS2 UCRT64 compiler, adds the `include/` directory, writes the executable beside its test source, and uses `-g` for debugging.

The `.vscode/launch.json` file currently provides launch configurations for those same two tests. Each configuration:

- Uses the `cppdbg` debugger type.
- Starts the generated `.exe` file.
- Sets the repository root as the working directory.
- Uses GDB at `C:\msys64\ucrt64\bin\gdb.exe`.
- Runs its matching build task through `preLaunchTask`.

`BoardUtils` currently has no configured task or launch entry, so use the manual command and launch configuration shown in Section 2, or add a matching task.

### Breakpoints and stepping

Click in the gutter next to a source line to set a breakpoint. When execution stops:

- `Continue` resumes until the next breakpoint.
- `Step Over` executes the current line without entering called functions.
- `Step Into` enters the called function.
- `Step Out` finishes the current function and returns to its caller.
- The Variables and Watch panels show current values.
- The Debug Console evaluates expressions while stopped.

Useful expressions include `move.row`, `move.col`, `board.convertBoardFormat()`, and `ai.getVisitedNodesCount()` where those variables are in scope.

### Common debugger problems

- **Executable not found:** Build the selected test and verify the `.exe` path in `launch.json`.
- **GDB not found:** Install the MSYS2 UCRT64 toolchain or change `miDebuggerPath` to the actual `gdb.exe` path.
- **Breakpoints are hollow or skipped:** Confirm the program was compiled with `-g` and that the source path matches the workspace copy.
- **Undefined references:** Include every required implementation file. `BoardUtils` needs both `BoardUtils.cpp` and `OthelloBoard.cpp`; `OthelloBoard` needs `OthelloBoard.cpp`; `OthelloAI` uses the implementation files included by its test source.
- **AI build duplicate definitions:** Do not pass `src/OthelloAI.cpp` or `src/OthelloBoard.cpp` separately when compiling `test_OthelloAI.cpp`, because the test already includes them.
- **AI test appears slow:** Higher exploration depth visits substantially more minimax positions. Start with depth 0 or 1 when investigating a breakpoint.

## Troubleshooting test failures

If a test fails:

1. Read the assertion location printed by the debugger or terminal.
2. Confirm that the test was rebuilt after source changes.
3. Run the affected test alone using its section above.
4. Inspect the relevant board, move, score, or node-count value.
5. Run the tests again in the recommended order after fixing the cause.

Do not ignore a failed assertion just because another test passes. Each test program exercises a different part of the project.

## Quick command summary

From the repository root:

```powershell
# OthelloBoard
g++ -std=c++17 -g -Iinclude src/OthelloBoard.cpp tests/test_OthelloBoard/test_OthelloBoard.cpp -o tests/test_OthelloBoard/test_OthelloBoard.exe
.\tests\test_OthelloBoard\test_OthelloBoard.exe

# BoardUtils
g++ -std=c++17 -g -Iinclude src/BoardUtils.cpp src/OthelloBoard.cpp tests/test_BoardUtils/test_BoardUtils.cpp -o tests/test_BoardUtils/test_BoardUtils.exe
.\tests\test_BoardUtils\test_BoardUtils.exe

# OthelloAI
g++ -std=c++17 -fpermissive -g -Iinclude tests/test_OthelloAI/test_OthelloAI.cpp -o tests/test_OthelloAI/test_OthelloAI.exe
.\tests\test_OthelloAI\test_OthelloAI.exe
```

Successful output is respectively:

```text
All OthelloBoard tests passed.
All BoardUtils tests passed.
All OthelloAI tests passed.
```
