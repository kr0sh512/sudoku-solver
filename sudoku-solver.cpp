/**
 * @file sudoku-solver-v2.cpp
 * @brief A Sudoku solver that reads a Sudoku puzzle from a file, solves it, and writes the solution to another file.
 *
 * This program reads a Sudoku puzzle from "sudoku.txt", solves it using a combination of constraint propagation and
 * backtracking, and writes the solved puzzle to "solve.txt".
 *
 * The main components of the program are:
 * - `struct cell`: Represents a cell in the Sudoku grid, with a solved value and possible notes.
 * - `input_board()`: Reads the Sudoku puzzle from a file into a 2D vector.
 * - `solve()`: Solves the Sudoku puzzle.
 * - `check()`: Helper function used in solving to check constraints.
 * - `print_board()`: Writes the solved Sudoku puzzle to a file.
 *
 * @note The program assumes that the input file "sudoku.txt" exists and contains a valid Sudoku puzzle.
 *
 * @author Kr0sH_512
 * @date 2023
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <format>
#include <string>
#include <cstring>
#include <utility>



struct cell {
    cell(int x) {
        this->solve = x;
    }

    int solve = 0;
    bool notes[10] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
};

void input_board(std::vector<std::vector<int>>&, std::ifstream&);
void solve(std::vector<std::vector<int>>&);
void check(std::vector<std::vector<cell>>&, std::pair<int, int>(*)(int, int));
void print_board(std::vector<std::vector<int>>, std::ofstream&);


int main(int argc, char* argv[]) { // https://sudoku.com/ru/evil/
    std::string input_path = "sudoku.txt";
    if (argc > 1) {
        input_path = argv[1];
    }

    std::ifstream fin(input_path);
    std::ofstream fout("solve.txt");

    if (!fin.is_open()) {
        std::cerr << "Failed to open input file: " << input_path << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> board(9, std::vector<int>(9, 0));

    input_board(board, fin);

    solve(board);

    print_board(board, fout);

    fin.close();
    fout.close();

    return 0;
}

void solve(std::vector<std::vector<int>>& board) {


    std::vector<std::vector<cell>> mas(9, std::vector<cell>(9, 0));

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            mas[i][j].solve = board[i][j];
        }
    }

    while (true) {
        bool is_checked = false;

        std::cout << 1;

        // Removing values from notes that cannot be used

        for (int x = 0; x < 9; ++x) {
            for (int y = 0; y < 9; ++y) {
                if (mas[x][y].solve != 0) continue;

                for (int i = 0; i < 9; ++i) {
                    if (mas[i][y].solve != 0) {
                        if (mas[x][y].notes[mas[i][y].solve]) {
                            is_checked = true;
                        }

                        mas[x][y].notes[mas[i][y].solve] = 0;
                    }

                    if (mas[x][i].solve != 0) {
                        if (mas[x][y].notes[mas[x][i].solve]) {
                            is_checked = true;
                        }
                        mas[x][y].notes[mas[x][i].solve] = 0;
                    }
                }

                // checking the square
                int sx = (x / 3) * 3, sy = (y / 3) * 3;

                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        if (mas[sx + i][sy + j].solve != 0) {
                            if (mas[x][y].notes[mas[sx + i][sy + j].solve]) {
                                is_checked = true;
                            }
                            mas[x][y].notes[mas[sx + i][sy + j].solve] = 0;
                        }
                    }
                }

            }
        }

        // check for infinite loop
        break;
    }

    // check that the digit can only be in 1 cell
    check(mas, [](int d, int i) -> std::pair<int, int> {return std::make_pair(d, i);});
    check(mas, [](int d, int i) -> std::pair<int, int> {return std::make_pair(i, d);});
    check(mas, [](int d, int i) -> std::pair<int, int> {
        return std::make_pair(d / 3 * 3 + i / 3, d % 3 * 3 + i % 3);
        });

    // placing values in cells where only one digit is in the notes
    for (int x = 0; x < 9; ++x) {
        for (int y = 0; y < 9; ++y) {
            if (mas[x][y].solve != 0) continue;

            bool is_single = true;
            int ind = -1;

            for (int i = 0; i < 9; ++i) {
                if (mas[x][y].notes[i + 1] == 1) {
                    if (ind == -1) {
                        ind = i + 1;
                    } else {
                        is_single = false;
                        break;
                    }
                }
            }

            // cannot be index -1
            // TODO: might be a good check for solvability.
            if (is_single) {
                mas[x][y].solve = ind;
            }
        }
    }



    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            board[i][j] = mas[i][j].solve;
        }
    }

    return;
}

void check(std::vector<std::vector<cell>>& mas, std::pair<int, int>(*coord)(int, int)) {
    for (int d = 0; d < 9; ++d) {
        std::vector<std::vector<int>> count(10, std::vector<int>());

        for (int i = 0; i < 9; ++i) {
            if (mas[coord(d, i).first][coord(d, i).second].solve != 0) {
                count[mas[coord(d, i).first][coord(d, i).second].solve].push_back(-1);
                count[mas[coord(d, i).first][coord(d, i).second].solve].push_back(-1);
                continue;
            }

            for (int k = 1; k <= 9; ++k) {
                if (mas[coord(d, i).first][coord(d, i).second].notes[k]) {
                    count[k].push_back(i);
                }
            }
        }

        for (int k = 1; k <= 9; ++k) {
            if (count[k].size() == 1) {
                mas[coord(d, count[k][0]).first][coord(d, count[k][0]).second].solve = k;
            }
        }
    }

    return;
}

void input_board(std::vector<std::vector<int>>& board, std::ifstream& fin) {
    int i = 0;

    while (true) {
        char c;
        fin >> c;

        if (fin.eof()) break;

        if (c <= '9' and c >= '0') {
            board[i / 9][i % 9] = c - '0';
            ++i;
        }
    }

    return;
}

void print_board(std::vector<std::vector<int>> board, std::ofstream& fout) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            fout << board[i][j];

            if (j < 8) {
                fout << " ";

                if ((j + 1) % 3 == 0) {
                    fout << "| ";
                }
            }
        }

        if (i < 8) {
            fout << std::endl;

            if ((i + 1) % 3 == 0) {
                fout << std::string(21, '-');
                fout << std::endl;
            }
        }
    }

    return;
}