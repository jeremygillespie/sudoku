# Solving Sudoku puzzles
### with the DPLL algorithm in C++

## How big is this problem?
A Sudoku puzzle of size N has N^2 spaces. Each space contains one of N different digits.

`N^3 literals`

Each space must contain a digit = `N^2 clauses`

No space can contain two digits = `(1 / 2) * N * (N + 1) * N^2 clauses`

No row, column, or box can contain the same digit twice = `3 * (1 / 2) * N * (N + 1) * N^2 clauses`

Total = `N^2 + 4 * (1 / 2) * N * (N + 1) * N^2`

4x4 puzzle = 64 literals, 656 clauses
9x9 puzzle = 729 literals, 14661 clauses
16x16 puzzle = 4096 literals, 139520 clauses