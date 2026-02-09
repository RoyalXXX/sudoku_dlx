/*
	Sudoku DLX Solver
	
	Copyright (c) 2026 Royal_X (MIT License)
 
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
*/

#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
#include <memory>

struct DLXNode
{
	DLXNode* left;
	DLXNode* right;
	DLXNode* up;
	DLXNode* down;
	DLXNode* column;
	int columnSize;
	std::vector<int> rowData; // [value, row, col]

	DLXNode();
};

class SudokuDLXSolver
{
private:
	const int gridSize;
	const int blockSize;
	const int cellCount;
	const int exactCoverRows;
	const int exactCoverCols;

	DLXNode* rootHeader;
	std::vector<DLXNode*> solution;
	std::vector<DLXNode*> fixedClues;
	std::vector<std::unique_ptr<DLXNode>> allNodes;
	int solutionCount;

	void coverColumn(DLXNode* col);
	void uncoverColumn(DLXNode* col);
	void searchDLX(int k, int searchLimit, std::vector<std::vector<std::vector<int>>>& solutions);
	void buildExactCoverMatrix(std::vector<std::vector<bool>>& exactCoverMatrix);
	void buildDLXLinkedList(const std::vector<std::vector<bool>>& exactCoverMatrix);
	DLXNode* findNodeForClue(int value, int row, int col);
	void applyInitialConstraints(const std::vector<std::vector<int>>& puzzle);
	void mapSolutionToGrid(std::vector<std::vector<int>>& sudoku);

public:
	explicit SudokuDLXSolver(int size = 9);
	~SudokuDLXSolver() = default;

	SudokuDLXSolver(const SudokuDLXSolver&) = delete;
	SudokuDLXSolver& operator=(const SudokuDLXSolver&) = delete;
	SudokuDLXSolver(SudokuDLXSolver&&) = default;
	SudokuDLXSolver& operator=(SudokuDLXSolver&&) = default;

	std::vector<std::vector<std::vector<int>>> solve(const std::vector<std::vector<int>>& puzzle, 
													  int searchLimit = 10);

	int getGridSize() const { return gridSize; }
	int getBlockSize() const { return blockSize; }
};

// Utility functions
void printGrid(const std::vector<std::vector<int>>& grid);
void printSolutions(const std::vector<std::vector<std::vector<int>>>& solutions, int printLimit = 10);

#endif // SUDOKU_H