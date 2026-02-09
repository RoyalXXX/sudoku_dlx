# Sudoku DLX Solver

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Static Badge](https://img.shields.io/badge/C%2B%2B-11-blue?)](https://isocpp.org/)

A high-performance C++ Sudoku solver implementing Knuth's **Dancing Links (DLX)** algorithm — one of the fastest exact cover algorithms in the world.

## 🚀 Key Features

### Performance & Algorithm
- **Dancing Links (DLX) Algorithm**: Implements Donald Knuth's Algorithm X with Dancing Links, renowned as one of the most efficient algorithms for solving exact cover problems
- **Blazing Fast**: Solves even the hardest Sudoku puzzles in microseconds
- **Multiple Solutions**: Can find all possible solutions or limit the search to a specified number
- **Optimized Search**: Uses column selection heuristic (minimum size) for improved performance

### Flexibility & Scalability
- **Any Grid Size**: Supports puzzles of any size — 4×4, 9×9, 16×16, 25×25, and beyond (size must be a perfect square)
- **Runtime Configuration**: Grid size is configurable at runtime, not compile-time
- **Any Difficulty**: Solves puzzles of any complexity, from simple to world's hardest

### Modern C++ Design
- **C++11 Standard**: Written in modern C++ with smart pointers and RAII principles
- **Memory Safe**: Uses `std::unique_ptr` for automatic memory management, eliminating memory leaks
- **Exception Safe**: Proper input validation with descriptive error messages
- **Const Correctness**: Immutable members and const-qualified methods where appropriate
- **Move Semantics**: Supports move construction and assignment for efficiency
- **Rule of Five**: Properly implements special member functions (copy operations deleted, move operations defaulted)

### Safety & Robustness
- **Input Validation**: Validates puzzle dimensions and grid size constraints
- **Type Safety**: Strong typing with explicit constructors
- **Bounds Checking**: Comprehensive checks for valid puzzle configurations
- **No Raw Pointers Leaking**: All allocations managed through smart pointers

## 📋 How It Works

The solver transforms the Sudoku puzzle into an **exact cover problem** with four constraints:
1. Each cell contains exactly one number
2. Each row contains each number exactly once
3. Each column contains each number exactly once
4. Each block contains each number exactly once

The DLX algorithm efficiently explores the solution space by:
- Building a sparse matrix representation as a toroidal doubly-linked list
- Using the "covering" and "uncovering" operations to navigate the search space
- Backtracking when constraints cannot be satisfied
- Applying heuristics to minimize search time

## 🛠️ Usage Example

```cpp
#include "sudoku.h"
#include <iostream>

int main()
{
    try
    {
        // Create a 9×9 Sudoku solver
        // Note: You can also write just SudokuDLXSolver solver; 
        // since the default size is 9
        SudokuDLXSolver solver(9);
        
        // Define a puzzle (0 represents empty cells)
        std::vector<std::vector<int>> puzzle = {
            { 0,0,0,  0,0,0,  0,0,0 },
            { 0,0,0,  0,0,3,  0,8,5 },
            { 0,0,1,  0,2,0,  0,0,0 },

            { 0,0,0,  5,0,7,  0,0,0 },
            { 0,0,4,  0,0,0,  1,0,0 },
            { 0,9,0,  0,0,0,  0,0,0 },

            { 5,0,0,  0,0,0,  0,7,3 },
            { 0,0,2,  0,1,0,  0,0,0 },
            { 0,0,0,  0,4,0,  0,0,9 }
        };
        
        // Display the original puzzle
        std::cout << "Original Puzzle:" << std::endl;
        printGrid(puzzle);
        
        // Solve the puzzle
        // Note: searchLimit has a default value of 10, so you can also write:
        // auto solutions = solver.solve(puzzle);
        auto solutions = solver.solve(puzzle, 10);
        
        // Print all found solutions
        printSolutions(solutions);
        
        std::cout << "Solutions found: " << solutions.size() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
```

## 🎯 API Reference

### Constructor
```cpp
SudokuDLXSolver(int size = 9)
```
Creates a solver for puzzles of the specified size. Size must be a perfect square (4, 9, 16, 25, etc.).

### Main Solving Method
```cpp
std::vector<std::vector<std::vector<int>>> solve(
    const std::vector<std::vector<int>>& puzzle, 
    int searchLimit = 10
)
```
- **Parameters**:
  - `puzzle`: 2D vector representing the Sudoku grid (0 for empty cells)
  - `searchLimit`: Maximum number of solutions to find (default: 10)
- **Returns**: Vector of all found solutions, each as a 2D grid
- **Throws**: `std::invalid_argument` if puzzle dimensions are invalid

### Utility Functions
```cpp
void printGrid(const std::vector<std::vector<int>>& grid);
void printSolutions(const std::vector<std::vector<std::vector<int>>>& solutions, 
                   int printLimit = 10);
```

## 🧠 About Dancing Links (DLX)

Dancing Links is an ingenious technique invented by Donald Knuth for efficiently implementing his Algorithm X. The key insights are:

- **Doubly-Linked Lists**: The sparse matrix is represented as a toroidal doubly-linked list structure
- **O(1) Deletions and Restorations**: Nodes can be removed and restored in constant time by manipulating pointers
- **Backtracking**: The algorithm explores the solution space with efficient backtracking
- **Exact Cover**: Perfectly suited for constraint satisfaction problems like Sudoku

The name "Dancing Links" comes from the way pointers appear to "dance" as they're updated during the covering and uncovering operations.

## ⚡ Performance Characteristics

- **Time Complexity**: Exponential in worst case, but extremely fast in practice due to aggressive pruning
- **Space Complexity**: O(n²) for storing the sparse matrix representation
- **Memory Management**: Near stack-level performance despite dynamic allocation, thanks to modern allocators and smart pointers
- **Cache Efficiency**: Linked list structure maintains good locality of reference

## 🏆 Advantages

1. **World-Class Algorithm**: DLX is recognized as one of the fastest algorithms for exact cover problems
2. **Production-Ready Code**: Robust error handling, modern C++ practices, and comprehensive safety checks
3. **Scalable**: Handles any puzzle size limited only by available memory
4. **Maintainable**: Clean separation of concerns, well-documented code structure
5. **Portable**: Standard C++11, no external dependencies

## 📝 License

MIT License

Copyright (c) 2026 Royal_X

https://github.com/RoyalXXX
https://royalxxx.itch.io
https://sourceforge.net/u/royal-x

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## 📚 References

- Knuth, Donald E. "Dancing Links" (2000) - [arXiv:cs/0011047](https://arxiv.org/abs/cs/0011047)
- Knuth's Algorithm X for exact cover problems

---

> [!NOTE] 
> This implementation demonstrates that modern C++ can achieve excellent performance with dynamic memory allocation while maintaining safety and expressiveness. The use of smart pointers and RAII ensures memory safety without sacrificing speed.
