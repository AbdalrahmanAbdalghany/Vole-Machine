# 🖥️ Vole Machine Simulator

An educational **Vole Machine Simulator** developed in C++ as part of the CS213 – Programming II (Object-Oriented Programming) course at Cairo University.

The project simulates a simplified computer architecture, allowing users to load and execute machine-language instructions while observing how the CPU, memory, and registers interact.

## 🚀 Download & Run

**Want to try the application without compiling the source code?**

📥 [**Download Vole Machine GUI (Windows EXE)**](https://github.com/AbdalrahmanAbdalghany/Vole-Machine/raw/refs/heads/master/VoleMachineGUI%20(1).zip)

### How to Run

1. Download the ZIP file using the link above.
2. Extract the ZIP file.
3. Locate the `.exe` application.
4. Double-click the executable to launch the simulator.

No need to compile the source code to use the provided executable.

## ✨ Features

- **CPU Simulation:** Implements the Fetch–Decode–Execute instruction cycle.
- **Memory Management:** Simulates 256 memory cells (00–FF).
- **Register Management:** Supports 16 general-purpose registers (R0–RF).
- **Instruction Execution:** Supports arithmetic, logical, data movement, rotation, and jump operations.
- **Floating-Point Arithmetic:** Implements 8-bit floating-point addition using Excess-4 notation.
- **Program Loading:** Loads machine-language programs from text files.
- **Step-by-Step Execution:** Execute instructions individually and observe the machine state after every step.
- **Run All:** Execute an entire program automatically.
- **Memory Editing:** Allows users to modify memory cells.
- **Register Editing:** Allows users to modify register values.
- **Screen Output:** Supports memory-mapped screen output through address `FF`.
- **Instruction Validation:** Detects invalid instructions and operands.

## 🧠 Supported Instruction Set

| Opcode | Operation      | Description                      |
| ------ | -------------- | -------------------------------- |
| 1      | LOAD           | Load a value from memory         |
| 2      | LOAD Immediate | Load an immediate value          |
| 3      | STORE          | Store a register value in memory |
| 4      | MOVE           | Move data between registers      |
| 5      | ADD Integer    | Two's complement addition        |
| 6      | ADD Float      | Floating-point addition          |
| 7      | OR             | Bitwise OR                       |
| 8      | AND            | Bitwise AND                      |
| 9      | XOR            | Bitwise XOR                      |
| A      | ROTATE         | Circular right rotation          |
| B      | JUMP           | Conditional jump                 |
| C      | HALT           | Stop execution                   |

## 🛠️ Technologies Used

- C++
- Object-Oriented Programming (OOP)
- Git & GitHub
- UML Class Diagrams
- PlantUML
- QT

## 📂 Project Structure

```text
Vole-Machine/
│
├── ConsoleApplication1/
│   └── Console simulator source files
│
├── GUI/
│   └── Graphical User Interface source files
│
├── VoleMachineGUI.zip
│   └── Downloadable application package
│
└── README.md
```

## 📖 About the Vole Machine

The Vole Machine is a simplified computer architecture designed for educational purposes.

It demonstrates fundamental computer organization concepts, including:

- CPU instruction processing.
- Memory addressing.
- Register operations.
- Arithmetic and logical operations.
- Conditional branching.
- Machine-language programming.

## 👨‍💻 Team Members

Developed by a team of four students at the Faculty of Computers and Artificial Intelligence, Cairo University.

**Course:** CS213 – Programming II
**Assignment:** Assignment 2 – Part 2
**Academic Year:** 2026–2027
**Section:** S23/S24

## 🔗 Repository

[View Source Code on GitHub](https://github.com/AbdalrahmanAbdalghany/Vole-Machine)

---

⭐ If you find this project useful, feel free to explore the source code and try the simulator!
