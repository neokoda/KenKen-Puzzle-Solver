#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

using namespace std;

class Cell {
    private:
        int value;
        int target;
        char operation;
        vector<tuple<int, int>> cluster;

    public:
        Cell() {}

        Cell(int val, int tgt, char op, const std::vector<std::tuple<int, int>>& clust)
        : value(val), target(tgt), operation(op), cluster(clust) {}

        int getValue() const {
            return value;
        }

        void setValue(int val) {
            value = val;
        }

        int getTarget() const {
            return target;
        }

        void setTarget(int tgt) {
            target = tgt;
        }

        char getOperation() const {
            return operation;
        }

        void setOperation(char op) {
            operation = op;
        }

        const std::vector<std::tuple<int, int>>& getCluster() const {
            return cluster;
        }
};

class KenKen {
    private:
        int gridSize;
        vector<vector<Cell>> grid;
        bool solutionFound;

    public:
        KenKen(const string& path) {
            ifstream file(path);
            if (!file.is_open()) {
                cerr << "Error: Unable to open file " << path << endl;
            }

            string line;
            getline(file, line);

            istringstream iss(line);
            iss >> gridSize;

            grid.resize(gridSize, vector<Cell>(gridSize, Cell(0, 0, ' ', {})));

            while (getline(file, line)) {
                istringstream iss(line);
                char operation;
                int row, target;
                vector<tuple<int, int>> cluster;

                iss >> target >> operation;

                char columnChar;
                while (iss >> columnChar >> row) {
                    cluster.push_back(make_tuple(row - 1, columnChar - 'A'));
                }

                for (const auto& entry : cluster) {
                    grid[get<0>(entry)][get<1>(entry)] = Cell(0, target, operation, cluster);
                }
            }

            file.close();

            solutionFound = false;
        }

        void clearGrid() {
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                    grid[i][j].setValue(0);
                }
            }
        }

        vector<vector<Cell>> copyGrid(const vector<vector<Cell>>& gr) {
            vector<vector<Cell>> gridCopy(gridSize, vector<Cell>(gridSize));

            for (int i = 0; i < gridSize; ++i) {
                for (int j = 0; j < gridSize; ++j) {
                    gridCopy[i][j] = gr[i][j];
                }
            }

            return gridCopy;
        }

        bool isRowValid(const vector<vector<Cell>>& gr, int row) {
            vector<bool> seen(gridSize, false);

            for (int j = 0; j < gridSize; j++) {
                int value = gr[row][j].getValue();
                if (value != 0) {
                    if (seen[value - 1]) {
                        return false;
                    } else {
                        seen[value - 1] = true;
                    }
                }
            }

            return true;
        }

        bool isColValid(const vector<vector<Cell>>& gr, int col) {
            vector<bool> seen(gridSize, false);

            for (int i = 0; i < gridSize; i++) {
                int value = gr[i][col].getValue();
                if (value != 0) {
                    if (seen[value - 1]) {
                        return false;
                    } else {
                        seen[value - 1] = true;
                    }
                }
            }

            return true;
        }

        bool isClusterValid(const vector<vector<Cell>>& grid, int row, int col, bool finalCheck) {
            char operation = grid[row][col].getOperation();
            int target = grid[row][col].getTarget();
            vector<tuple<int, int>> cluster = grid[row][col].getCluster();

            int total, a, b;
            switch (operation) {
                case '+':
                    total = 0;
                    for (const auto& entry : cluster) {
                        total += grid[get<0>(entry)][get<1>(entry)].getValue();
                    }

                    if (total > target) {
                        return false;
                    }
                    if (finalCheck && total != target) {
                        return false;
                    }
                    break;
                case '-':
                    a = grid[get<0>(cluster[0])][get<1>(cluster[0])].getValue();
                    b = grid[get<0>(cluster[1])][get<1>(cluster[1])].getValue();

                    if (a == 0 || b == 0) {
                        return true;
                    }

                    if (a > b) {
                        if (a - b != target) {
                            return false;
                        }
                    } else {
                        if (b - a != target) {
                            return false;
                        }
                    }
                    break;
                case '*':
                    total = 1;
                    for (const auto& entry : cluster) {
                        total *= grid[get<0>(entry)][get<1>(entry)].getValue();
                    }

                    if (total > target) {
                        return false;
                    }
                    if (finalCheck && total != target) {
                        return false;
                    }
                    break;
                case '/':
                    a = grid[get<0>(cluster[0])][get<1>(cluster[0])].getValue();
                    b = grid[get<0>(cluster[1])][get<1>(cluster[1])].getValue();

                    if (a == 0 || b == 0) {
                        return true;
                    }

                    if (a > b) {
                        if (a / b != target) {
                            return false;
                        }
                    } else {
                        if (b / a != target) {
                            return false;
                        }
                    }
                    break;
            }

            return true;
        }

        bool placeable(vector<vector<Cell>>& gr, int row, int col, int val) {
            vector<vector<Cell>> gridCopy = copyGrid(gr);
            gridCopy[row][col].setValue(val);

            if (!isRowValid(gridCopy, row)) {
                return false;
            }

            if (!isColValid(gridCopy, col)) {
                return false;
            }

            if (!isClusterValid(gridCopy, row, col, false)) {
                return false;
            }

            return true;
        }

        bool isSolution(vector<vector<Cell>>& gr) {
            for (int row = 0; row < gridSize; row++) {
               if (!isRowValid(gr, row)) {
                    return false;
               }
                if (!isColValid(gr, row)) {
                    return false;
                }
                for (int col = 0; col < gridSize; col++) {
                    if (!isClusterValid(gr, row, col, true)) {
                        return false;
                    }

                }
            }

            return true;
        }

        void findSolutionBacktrack(vector<vector<Cell>>& gr, int k) {
            int i = k / gridSize;
            int j = k % gridSize;
            for (int val = 1; val <= gridSize && !solutionFound; val++) {
                if (placeable(gr, i, j, val)) {
                    vector<vector<Cell>> gridCopy = copyGrid(gr);
                    gridCopy[i][j].setValue(val);

                    if (k == (gridSize * gridSize - 1) && isSolution(gridCopy)) {
                        grid = copyGrid(gridCopy);
                        solutionFound = true;
                    }

                    if (k < (gridSize * gridSize  - 1)) {
                        findSolutionBacktrack(gridCopy, k + 1);
                    }
                }
            }
        }

        vector<vector<int>> permuteNumbers(vector<int>& numbers) {
            vector<vector<int>> permutations;
            do {
                permutations.push_back(numbers);
            } while (next_permutation(numbers.begin(), numbers.end()));
            return permutations;
        }

        void findSolutionBruteForce(vector<vector<Cell>>& gr, vector<vector<int>>& permutations, int i) {
            if (solutionFound) {
                return;
            }
            
            if (i == gridSize) {
                if (isSolution(gr)) {
                    grid = copyGrid(gr);
                    solutionFound = true;
                }
                return;
            }

            for (vector<int> permutation : permutations) {
                if (solutionFound) {
                    return;
                }

                vector<int> temp;
                for (int j = 0; j < gridSize; j++) {
                    temp.push_back(gr[i][j].getValue());
                    gr[i][j].setValue(permutation[j]);
                }

                findSolutionBruteForce(gr, permutations, i + 1);

                for (int j = 0; j < gridSize; j++) {
                    gr[i][j].setValue(temp[j]);
                }
            }
        }

        void printLayout() {
            cout << "Puzzle layout: " << endl;
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                   cout << grid[i][j].getTarget() << grid[i][j].getOperation() << " ";
                }
                cout << endl;
            }
            cout << endl;
        }

        void printGrid() {
            for (int i = 0; i < gridSize; i++) {
                for (int j = 0; j < gridSize; j++) {
                   cout << grid[i][j].getValue() << " ";
                }
                cout << endl;
            }
            cout << endl;
        }

        void getSolution() {
            vector<vector<Cell>> gridCopy;
            solutionFound = false;
            clearGrid();
            gridCopy = copyGrid(grid);

            auto startBacktrack = chrono::high_resolution_clock::now();

            findSolutionBacktrack(gridCopy, 0);

            auto stopBacktrack = chrono::high_resolution_clock::now();
            auto durationBacktrack = chrono::duration_cast<chrono::microseconds>(stopBacktrack - startBacktrack); 
            cout << "Solution using backtracking: " << endl;
            printGrid();
            cout << "Time taken: " << durationBacktrack.count() << " microseconds." << endl << endl;

            solutionFound = false;
            clearGrid();
            gridCopy = copyGrid(grid);

            auto startBruteForce = chrono::high_resolution_clock::now();

            vector<int> numberList;
            for (int i = 1; i <= gridSize; i++) {
                numberList.push_back(i);
            }
            vector<vector<int>> permutations = permuteNumbers(numberList);
            findSolutionBruteForce(gridCopy, permutations, 0);
            
            auto stopBruteForce = chrono::high_resolution_clock::now();
            auto durationBruteForce = chrono::duration_cast<chrono::microseconds>(stopBruteForce - startBruteForce); 
            cout << "Solution using brute force: " << endl;
            printGrid();
            cout << "Time taken: " << durationBruteForce.count() << " microseconds." << endl << endl;
        }
};

int main() {
    string filePath;
    cout << "Enter the file path: ";
    getline(cin, filePath);
    cout << endl;

    KenKen kenKen(filePath);

    kenKen.printLayout();
    kenKen.getSolution();

    return 0;
}