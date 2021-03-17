#include <iostream>
#include <string>

#include "dpll.hpp"

constexpr int box_width = 3;
constexpr int grid_width = box_width * box_width;
constexpr int num_literals = grid_width * grid_width * grid_width;

using namespace std;

int literal(int row, int column, int digit, bool value = true)
{
    int id = grid_width * grid_width * row + grid_width * column + digit;
    if(value)
        return id;
    return ~id;
}

int parse_literal(string s)
{
    int row, column, digit; 
    bool value;
    if(s[0] == '~')
    {
        row = stoi(s.substr(1, 1), nullptr, 16) - 1;
        column = stoi(s.substr(2, 1), nullptr, 16) - 1;
        digit  = stoi(s.substr(3, 1), nullptr, 16) - 1;
        value = false;
    }
    else
    {
        row = stoi(s.substr(0, 1), nullptr, 16) - 1;
        column = stoi(s.substr(1, 1), nullptr, 16) - 1;
        digit  = stoi(s.substr(2, 1), nullptr, 16) - 1;
        value = true;
    }
    return literal(row, column, digit, value);
}

// read clauses from stdin
dpll::clauses_t clauses_input()
{
    dpll::clauses_t clauses{};
    
    string line;
    while(getline(cin, line))
    {
        unordered_set<int> clause{};

        size_t pos = 0;
        string token;
        while ((pos = line.find(' ')) != string::npos) {
            token = line.substr(0, pos);
            clause.insert(parse_literal(token));
            line.erase(0, pos + 1);
        }
        clause.insert(parse_literal(line));

        clauses.push_front(clause);
    }

    return clauses;
}

dpll::clauses_t clauses_sudoku()
{
    dpll::clauses_t clauses{};

    // Each space must contain a digit
    for(int row = 0; row < grid_width; ++row)
    {
        for(int column = 0; column < grid_width; ++column)
        {
            unordered_set<int> clause{};
            for(int digit = 0; digit < grid_width; ++digit)
            {
                clause.insert(literal(row, column, digit, true));
            }
            clauses.push_front(clause);
        }
    }

    // No space can contain two digits
    for(int row = 0; row < grid_width; ++row)
    {
        for(int column = 0; column < grid_width; ++column)
        {
            for(int digit1 = 0; digit1 < grid_width; ++digit1)
            {
                for(int digit2 = digit1 + 1; digit2 < grid_width; ++digit2)
                {
                    clauses.push_front({
                        literal(row, column, digit1, false),
                        literal(row, column, digit2, false)
                    });
                }
            }
        }
    }

    // No row can contain the same digit twice
    for(int row = 0; row < grid_width; ++row)
    {
        for(int column1 = 0; column1 < grid_width; ++column1)
        {
            for(int column2 = column1 + 1; column2 < grid_width; ++column2)
            {
                for(int digit = 0; digit < grid_width; ++digit)
                {
                    clauses.push_front({
                        literal(row, column1, digit, false),
                        literal(row, column2, digit, false)
                    });
                }
            }
        }
    }

    // No column can contain the same digit twice
    for(int row1 = 0; row1 < grid_width; ++row1)
    {
        for(int row2 = row1 + 1; row2 < grid_width; ++row2)
        {
            for(int column = 0; column < grid_width; ++column)
            {
                for(int digit = 0; digit < grid_width; ++digit)
                {
                    clauses.push_front({
                        literal(row1, column, digit, false),
                        literal(row2, column, digit, false)
                    });
                }
            }
        }
    }

    // No box can contain the same digit twice
    for(int box = 0; box < grid_width; ++box)
    {
        for(int space1 = 0; space1 < grid_width; ++space1)
        {
            for(int space2 = space1 + 1; space2 < grid_width; ++space2)
            {
                int row1 = (box / box_width) * box_width + space1 / box_width;
                int col1 = (box % box_width) * box_width + space1 % box_width;
                int row2 = (box / box_width) * box_width + space2 / box_width;
                int col2 = (box % box_width) * box_width + space2 % box_width;
                if(row1 != row2 && col1 != col2)
                {
                    for(int digit = 0; digit < grid_width; ++digit)
                        {
                        clauses.push_front({
                            literal(row1, col1, digit, false),
                            literal(row2, col2, digit, false)
                        });
                    }
                }
            }
        }
    }

    return clauses;
}

void print_sudoku(vector<bool> literals)
{
    for(int row = grid_width - 1; row >= 0; --row)
    {
        if(row != grid_width - 1 && row % box_width == box_width - 1)
        {
            for(int column = 0; column < (grid_width + box_width) * 2 - 3; ++column)
                cout << "-";
            cout << "\n";
        }

        for(int column = 0; column < grid_width; ++column)
        {
            if(column != 0 && column % box_width == 0)
                cout << "| ";

            int value = 0;
            for(int digit = 0; digit < grid_width; ++digit)
            {
                int lit = literal(row, column, digit);
                if(literals[lit])
                {
                    value = digit + 1;
                    break;
                }
            }
            cout << value << " ";
        }
        cout << "\n";
    }
}

int main()
{
    auto clauses = clauses_input();
    clauses.splice(clauses.begin(), clauses_sudoku());
    std::vector<bool> literals(num_literals, false);
    
    if(dpll::solve(clauses, literals))
    {
        print_sudoku(literals);
    }
    else
    {
        std::cout << "no solution\n";
    }
}