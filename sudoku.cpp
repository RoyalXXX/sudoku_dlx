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

#include "sudoku.h"
#include <iostream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <algorithm>

DLXNode::DLXNode() 
	: left(nullptr), right(nullptr), up(nullptr), down(nullptr),
	  column(nullptr), columnSize(0), rowData(3, 0) 
{
}

SudokuDLXSolver::SudokuDLXSolver(int size)
	: gridSize(size)
	, blockSize(static_cast<int>(std::sqrt(size)))
	, cellCount(size * size)
	, exactCoverRows(size * cellCount)
	, exactCoverCols(4 * cellCount)
	, rootHeader(nullptr)
	, solution(cellCount, nullptr)
	, fixedClues(cellCount, nullptr)
	, solutionCount(0)
{
	if (blockSize * blockSize != size)
		throw std::invalid_argument("Grid size must be a perfect square (e.g., 4, 9, 16, 25), but got: " + std::to_string(size));
}

void SudokuDLXSolver::coverColumn(DLXNode* col)
{
	col->left->right = col->right;
	col->right->left = col->left;
	for (DLXNode* node = col->down; node != col; node = node->down)
	{
		for (DLXNode* temp = node->right; temp != node; temp = temp->right)
		{
			temp->down->up = temp->up;
			temp->up->down = temp->down;
			temp->column->columnSize--;
		}
	}
}

void SudokuDLXSolver::uncoverColumn(DLXNode* col)
{
	for (DLXNode* node = col->up; node != col; node = node->up)
	{
		for (DLXNode* temp = node->left; temp != node; temp = temp->left)
		{
			temp->column->columnSize++;
			temp->down->up = temp;
			temp->up->down = temp;
		}
	}
	col->left->right = col;
	col->right->left = col;
}

void SudokuDLXSolver::searchDLX(int k, int searchLimit, std::vector<std::vector<std::vector<int>>>& solutions)
{
	if (solutionCount >= searchLimit)
		return;

	if (rootHeader->right == rootHeader)
	{
		std::vector<std::vector<int>> sudokuGrid(gridSize, std::vector<int>(gridSize, 0));
		mapSolutionToGrid(sudokuGrid);
		solutions.push_back(sudokuGrid);
		solutionCount++;
		return;
	}

	// Select column with minimum size (heuristic)
	DLXNode* col = rootHeader->right;
	for (DLXNode* temp = col->right; temp != rootHeader; temp = temp->right)
		if (temp->columnSize < col->columnSize)
			col = temp;

	coverColumn(col);

	for (DLXNode* temp = col->down; temp != col; temp = temp->down)
	{
		solution[k] = temp;
		for (DLXNode* node = temp->right; node != temp; node = node->right)
			coverColumn(node->column);

		searchDLX(k + 1, searchLimit, solutions);

		temp = solution[k];
		solution[k] = nullptr;
		col = temp->column;
		for (DLXNode* node = temp->left; node != temp; node = node->left)
			uncoverColumn(node->column);
	}

	uncoverColumn(col);
}

void SudokuDLXSolver::buildExactCoverMatrix(std::vector<std::vector<bool>>& exactCoverMatrix)
{
	// Constraint 1: Each cell can only be filled with one number
	int j = 0, counter = 0;
	for (int i = 0; i < exactCoverRows; ++i)
	{
		exactCoverMatrix[i][j] = true;
		counter++;
		if (counter >= gridSize)
		{
			j++;
			counter = 0;
		}
	}

	// Constraint 2: Each number appears exactly once in each row
	int x = 0;
	counter = 1;
	for (j = cellCount; j < 2 * cellCount; ++j)
	{
		for (int i = x; i < counter * cellCount; i += gridSize)
			exactCoverMatrix[i][j] = true;

		if ((j + 1) % gridSize == 0)
		{
			x = counter * cellCount;
			counter++;
		}
		else
			x++;
	}

	// Constraint 3: Each number appears exactly once in each column
	j = 2 * cellCount;
	for (int i = 0; i < exactCoverRows; ++i)
	{
		exactCoverMatrix[i][j] = true;
		j++;
		if (j >= 3 * cellCount)
			j = 2 * cellCount;
	}

	// Constraint 4: Each number appears exactly once in each block
	x = 0;
	for (j = 3 * cellCount; j < exactCoverCols; ++j)
	{
		for (int l = 0; l < blockSize; ++l)
			for (int k = 0; k < blockSize; ++k)
				exactCoverMatrix[x + l * gridSize + k * cellCount][j] = true;

		int temp = j + 1 - 3 * cellCount;

		if (temp % (blockSize * gridSize) == 0)
			x += (blockSize - 1) * cellCount + (blockSize - 1) * gridSize + 1;
		else if (temp % gridSize == 0)
			x += gridSize * (blockSize - 1) + 1;
		else
			x++;
	}
}

void SudokuDLXSolver::buildDLXLinkedList(const std::vector<std::vector<bool>>& exactCoverMatrix)
{
	allNodes.reserve(1 + exactCoverCols + 4 * exactCoverRows);

	allNodes.push_back(std::make_unique<DLXNode>());
	DLXNode* header = allNodes.back().get();

	header->left = header;
	header->right = header;
	header->down = header;
	header->up = header;
	header->columnSize = -1;
	header->column = header;
	DLXNode* temp = header;

	// Create all Column Nodes
	for (int i = 0; i < exactCoverCols; ++i)
	{
		allNodes.push_back(std::make_unique<DLXNode>());
		DLXNode* newNode = allNodes.back().get();

		newNode->columnSize = 0;
		newNode->up = newNode;
		newNode->down = newNode;
		newNode->column = newNode;
		newNode->right = header;
		newNode->left = temp;
		temp->right = newNode;
		temp = newNode;
	}

	std::vector<int> id = { 0, 1, 1 };

	// Add a Node for each true value in the matrix
	for (int i = 0; i < exactCoverRows; ++i)
	{
		DLXNode* top = header->right;
		DLXNode* prev = nullptr;

		if (i != 0 && i % cellCount == 0)
		{
			id[0] -= gridSize - 1;
			id[1]++;
			id[2] -= gridSize - 1;
		}
		else if (i != 0 && i % gridSize == 0)
		{
			id[0] -= gridSize - 1;
			id[2]++;
		}
		else
		{
			id[0]++;
		}

		for (int j = 0; j < exactCoverCols; j++, top = top->right)
		{
			if (exactCoverMatrix[i][j])
			{
				allNodes.push_back(std::make_unique<DLXNode>());
				DLXNode* newNode = allNodes.back().get();

				newNode->rowData[0] = id[0];
				newNode->rowData[1] = id[1];
				newNode->rowData[2] = id[2];

				if (prev == nullptr)
				{
					prev = newNode;
					prev->right = newNode;
				}
				newNode->left = prev;
				newNode->right = prev->right;
				newNode->right->left = newNode;
				prev->right = newNode;
				newNode->column = top;
				newNode->down = top;
				newNode->up = top->up;
				top->up->down = newNode;
				top->columnSize++;
				top->up = newNode;
				if (top->down == top)
					top->down = newNode;
				prev = newNode;
			}
		}
	}

	rootHeader = header;
}

DLXNode* SudokuDLXSolver::findNodeForClue(int value, int row, int col)
{
	for (DLXNode* colHeader = rootHeader->right; colHeader != rootHeader; colHeader = colHeader->right)
	{
		for (DLXNode* node = colHeader->down; node != colHeader; node = node->down)
		{
			if (node->rowData[0] == value &&
				(node->rowData[1] - 1) == row &&
				(node->rowData[2] - 1) == col)
			{
				return node;
			}
		}
	}
	return nullptr;
}

void SudokuDLXSolver::applyInitialConstraints(const std::vector<std::vector<int>>& puzzle)
{
	int index = 0;
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
		{
			if (puzzle[i][j] > 0)
			{
				DLXNode* temp = findNodeForClue(puzzle[i][j], i, j);

				if (temp != nullptr)
				{
					DLXNode* col = temp->column;
					coverColumn(col);
					fixedClues[index] = temp;
					index++;

					for (DLXNode* node = temp->right; node != temp; node = node->right)
						coverColumn(node->column);
				}
			}
		}
	}
}

void SudokuDLXSolver::mapSolutionToGrid(std::vector<std::vector<int>>& sudoku)
{
	for (int i = 0; solution[i] != nullptr; ++i)
		sudoku[solution[i]->rowData[1] - 1][solution[i]->rowData[2] - 1] = solution[i]->rowData[0];
	for (int i = 0; fixedClues[i] != nullptr; ++i)
		sudoku[fixedClues[i]->rowData[1] - 1][fixedClues[i]->rowData[2] - 1] = fixedClues[i]->rowData[0];
}

std::vector<std::vector<std::vector<int>>> SudokuDLXSolver::solve(const std::vector<std::vector<int>>& puzzle, 
																   int searchLimit)
{
	// Validate puzzle dimensions
	if (puzzle.size() != gridSize ||
		(puzzle.size() > 0 && puzzle[0].size() != gridSize))
		throw std::invalid_argument("Expected puzzle dimensions " +
			std::to_string(gridSize) + "x" + std::to_string(gridSize) +
			", but got " + std::to_string(puzzle.size()) + "x" +
			std::to_string(puzzle.size() > 0 ? puzzle[0].size() : 0));

	std::vector<std::vector<std::vector<int>>> solutions;
	solutions.reserve(searchLimit);
	solutionCount = 0;
	allNodes.clear();
	std::fill(solution.begin(), solution.end(), nullptr);
	std::fill(fixedClues.begin(), fixedClues.end(), nullptr);

	std::vector<std::vector<bool>> exactCoverMatrix(exactCoverRows,
		std::vector<bool>(exactCoverCols, false));

	buildExactCoverMatrix(exactCoverMatrix);
	buildDLXLinkedList(exactCoverMatrix);
	applyInitialConstraints(puzzle);
	searchDLX(0, searchLimit, solutions);

	return solutions;
}

// Utility function implementations
void printGrid(const std::vector<std::vector<int>>& grid)
{
	if (grid.empty()) return;

	const int gridSize = grid.size();
	const int blockSize = static_cast<int>(std::sqrt(gridSize));

	if (blockSize * blockSize != gridSize) return;

	const int extraSpacing = (gridSize > 9) ? gridSize : 0;
	const int borderLength = (gridSize + blockSize - 1) * 2 + extraSpacing + 1;

	std::string outerBorder = "+";
	std::string innerBorder = "|";
	int blockCounter = 1;

	for (int i = 0; i < borderLength; i++)
	{
		outerBorder += '-';
		const int separatorPosition = ((blockSize * 2 + blockSize * (gridSize > 9) + 1) * blockCounter + blockCounter - 1);
		if (i > 0 && i % separatorPosition == 0) {
			innerBorder += '+';
			blockCounter++;
		}
		else {
			innerBorder += '-';
		}
	}
	outerBorder += '+';
	innerBorder += '|';

	std::cout << outerBorder << '\n';

	for (int i = 0; i < gridSize; ++i)
	{
		std::cout << "| ";
		for (int j = 0; j < gridSize; ++j)
		{
			if (grid[i][j] == 0)
				std::cout << '.';
			else
				std::cout << grid[i][j];

			std::cout << ' ';

			if (extraSpacing > 0 && grid[i][j] < 10)
				std::cout << ' ';

			if ((j + 1) % blockSize == 0)
				std::cout << "| ";
		}
		std::cout << '\n';

		if ((i + 1) % blockSize == 0 && (i + 1) < gridSize)
			std::cout << innerBorder << '\n';
	}
	std::cout << outerBorder << "\n\n";
}

void printSolutions(const std::vector<std::vector<std::vector<int>>>& solutions, int printLimit)
{
	for (size_t i = 0; i < std::min(static_cast<size_t>(printLimit), solutions.size()); ++i)
	{
		std::cout << "Solution " << i + 1 << ":" << std::endl;
		printGrid(solutions[i]);
	}
}
