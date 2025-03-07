# Linux Terminal Simulator

## Overview
This project is a simple Unix shell implementation named `mysh`. It provides basic shell functionalities, including:
- Command parsing and execution
- Handling aliases and command history
- Input and output redirection
- Background execution
- Pipeline support

The code is well-commented, making it easier to understand the implementation details.

---

## Installation & Compilation
### Compile the Project
```bash
make
```

### Run the Shell
```bash
./mysh
```

---

## Project Structure
The repository contains the following files:

- **`mysh.cpp`**  
  Contains the `main` function and signal handling implementations.

- **`mysh.h`**  
  Includes necessary headers, macros, and custom structures used in the project.

- **`functions.cpp`**  
  Implements various helper functions utilized by `main`.

- **`Makefile`**  
  Automates the compilation process.

---

## Functionality
### Command Processing
- The shell reads user input and processes it accordingly.
- It checks if the command exists in the alias table, history, or is directly entered by the user.
- The command is parsed into arguments and files (if any redirections are present).

### Alias & History Management
- Supports creating and destroying command aliases.
- Stores up to the last 20 commands in history.
- `myHistory <n>` prints the last `n` commands, with the most recent command indexed as `1`.

### Command Parsing Details
- The command is split into separate arguments and file redirections (`<`, `>`).
- Example:
  - `ls -a -l` → All parts stored as arguments.
  - `sort < file1 > file2` → `sort` is an argument, while `file1` and `file2` are identified as input and output files, respectively.
- Commands are stored in a structured format for execution.

### Pipe & Background Execution
- Commands containing `|` are split into separate processes for pipeline execution.
- Background commands (`&`) require a space before the `&`, e.g., `sort file1 &; ls &;`.

### Wildcards
- If a wildcard pattern (e.g., `ls file*.t?t`) does not match any files, it is treated as a direct argument and may result in an error.

---

## Notes & Considerations
- When using `createalias`, ensure to use **double quotes (`" "`)** instead of **curly quotes (`“ ”`)**.
- The implementation follows the test cases as specified, but minor variations may occur.
- The code has been tested extensively, and all required test cases pass.

---

## Acknowledgment
The project has been implemented with careful consideration of shell behavior and Unix system programming principles.

---

## License
This project is open-source. Feel free to use, modify, and distribute it as needed.

---

Thank you for checking out this project! 😊
