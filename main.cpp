#include <iostream>
#include <string>
#include <forward_list>
#include <vector>

#include "dpll.hpp"

using namespace std;



int box_width, grid_width;

int literal(int row, int column, int digit, bool value = true)
{
    return (grid_width * grid_width * row + grid_width * column + digit) * value ? 1 : -1;
}

int parse_literal(string s)
{
    int row, column, digit; 
    bool value;
    if(s[0] == '~')
    {
        row = stoi(s.substr(1, 2), nullptr, 16);
        column = stoi(s.substr(2, 3), nullptr, 16);
        digit  = stoi(s.substr(3, 4), nullptr, 16);
        value = false;
    }
    else
    {
        row = stoi(s.substr(0, 1), nullptr, 16);
        column = stoi(s.substr(1, 2), nullptr, 16);
        digit  = stoi(s.substr(2, 3), nullptr, 16);
        value = false;
    }
    return literal(row, column, digit, value);
}

// read clauses from stdin
forward_list<forward_list<int>> clauses_input()
{
    forward_list<forward_list<int>> clauses{};
    
    string line;
    while(getline(cin, line))
    {
        forward_list<int> clause{};

        size_t pos = 0;
        string token;
        while ((pos = line.find(' ')) != string::npos) {
            token = line.substr(0, pos);
            clause.push_front(parse_literal(token));
            line.erase(0, pos + 1);
        }
        clause.push_front(parse_literal(line));

        clauses.push_front(clause);
    }

    return clauses;
}

forward_list<forward_list<int>> clauses_sudoku()
{
    forward_list<forward_list<int>> clauses{};

    // Each space must contain a digit
    for(int row = 0; row < grid_width; ++row)
    {
        for(int column = 0; column < grid_width; ++column)
        {
            forward_list<int> clause{};
            for(int digit = 0; digit < grid_width; ++digit)
            {
                clause.push_front(literal(row, column, digit, true));
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

// TODO
void propagate(forward_list<forward_list<int>> &clauses, int lit);

// TODO
void assign(forward_list<forward_list<int>> &clauses, vector<bool> &literals_assigned, vector<bool> &literal_values, int lit);

bool dpll(forward_list<forward_list<int>> &clauses, vector<bool> &literals_assigned, vector<bool> &literal_values)
{
    // success
    if(clauses.empty())
        return true;

    // failure
    for(auto c = clauses.begin(); c != clauses.end(); ++c)
    {
        if(c->empty())
            return false;
    }

    // unit clause
    for(auto c = clauses.begin(); c != clauses.end(); ++c)
    {
        if(++c->begin() == c->end())
            propagate(clauses, *c->begin());
    }

    // pure literal
    // TODO
    int lit; // while clauses contains pure literals
    {
        assign(clauses, literals_assigned, literal_values, lit);
        literals_assigned.push_back(lit);
    }

    // arbitrary assign true
    // TODO
    int lit; // first unassigned literal

    forward_list<forward_list<int>> old_clauses{clauses};
    vector<bool> old_literals_assigned{literals_assigned};
    vector<bool> old_literal_values{literal_values};

    assign(clauses, literals_assigned, literal_values, lit);
    if(dpll(clauses, literals_assigned, literal_values))
        return true;

    // arbitrary assign false
    lit *= -1;
    clauses = old_clauses;
    literals_assigned = old_literals_assigned;
    literal_values = old_literal_values;
    assign(clauses, literals_assigned, literal_values, lit);
    return dpll(clauses, literals_assigned, literal_values);
}

void print_sudoku(vector<bool> literals_assigned, vector<bool> literal_values)
{
    for(int row = 0; row < grid_width; ++row)
    {
        if(row % box_width + 1 == box_width)
        {
            for(int column = 0; column < grid_width + box_width - 3; ++column)
                cout << "-";
            cout << "\n";
        }

        for(int column = 0; column < grid_width; ++column)
        {
            if(column % box_width + 1 == box_width)
                cout << "| ";

            int value = 0;
            for(int digit = 0; digit < grid_width; ++digit)
            {
                int lit = literal(row, column, digit);
                if(literals_assigned[lit] && literal_values[lit])
                    value = digit + 1;
            }
            if(value != 0)
                cout << value << " ";
            else
                cout << "  ";
        }
        cout << "\n";
    }
}

int main()
{
    cin >> box_width;
    grid_width = box_width * box_width;
    int num_literals = grid_width * grid_width * grid_width;

    auto clauses = clauses_input();
    clauses.splice_after(clauses.before_begin(), clauses_sudoku());
    vector<bool> literals_assigned(num_literals, false);
    vector<bool> literal_values(num_literals, false);
    
    if(dpll(clauses, literals_assigned, literal_values))
    {
        print_sudoku(literals_assigned, literal_values);
    }
    else
    {
        cout << "no solution\n";
    }
}