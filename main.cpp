#include <iostream>

#include "dpll.hpp"
#include "sudoku.hpp"

int main()
{
    using namespace sudoku;
    auto clauses = clauses_input();
    clauses.splice(clauses.begin(), clauses_sudoku());
    vector<bool> lit_assigned(num_literals, false);
    vector<bool> lit_value(num_literals, false);
    
    if(dpll(clauses, lit_assigned, lit_value))
    {
        print_sudoku(lit_assigned, lit_value);
    }
    else
    {
        cout << "no solution\n";
    }
}