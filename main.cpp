#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <unordered_set>

using namespace std;

int box_width, grid_width;

int literal(int row, int column, int digit, bool value = true)
{
    int id = grid_width * grid_width * row + grid_width * column + digit;
    if(value)
        return id;
    return -id - 1;
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
list<unordered_set<int>> clauses_input()
{
    list<unordered_set<int>> clauses{};
    
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

list<unordered_set<int>> clauses_sudoku()
{
    list<unordered_set<int>> clauses{};

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

void assign(list<unordered_set<int>> &clauses, vector<bool> &lit_assigned, vector<bool> &lit_value, int lit)
{
    if(lit >= 0)
    {
        lit_assigned[lit] = true;
        lit_value[lit] = true;
    }
    else
    {
        lit_assigned[-lit - 1] = true;
        lit_value[-lit - 1] = false;
    }
    for(auto c = clauses.begin(); c != clauses.end(); ++c)
    {
        if(c->find(lit) != c->end())
            c = clauses.erase(c);
        else
            c->erase(-lit - 1);
    }
}

bool dpll(list<unordered_set<int>> &clauses, vector<bool> &lit_assigned, vector<bool> &lit_value)
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

    // unit clauses
    for(auto c = clauses.begin(); c != clauses.end(); ++c)
    {
        if(c->size() == 1)
        {
            assign(clauses, lit_assigned, lit_value, *c->begin());
            c = clauses.begin();
        }
    }

    // pure literals
    for(int lit = 0; lit < lit_assigned.size(); ++lit)
    {
        if(!lit_assigned[lit])
        {
            bool lit_found = false, negation_found = false;
            for(auto c = clauses.begin(); c != clauses.end(); ++c)
            {
                if(c->find(lit) != c->end())
                {
                    lit_found = true;
                    if(negation_found)
                        break;
                }
                if(c->find(-lit - 1) != c->end())
                {
                    negation_found = true;
                    if(lit_found)
                        break;
                }
            }

            if(lit_found && negation_found)
                continue;
            assign(clauses, lit_assigned, lit_value, lit);
        }
    }

    // arbitrary assign true
    int lit = 0;
    while(lit_assigned[lit])
        ++lit;
    list<unordered_set<int>> old_clauses{clauses};
    vector<bool> old_lit_assigned{lit_assigned};
    vector<bool> old_lit_value{lit_value};
    assign(clauses, lit_assigned, lit_value, lit);
    if(dpll(clauses, lit_assigned, lit_value))
        return true;

    // arbitrary assign false
    lit *= -1;
    clauses = old_clauses;
    lit_assigned = old_lit_assigned;
    lit_value = old_lit_value;
    assign(clauses, lit_assigned, lit_value, lit);
    return dpll(clauses, lit_assigned, lit_value);
}

void print_sudoku(vector<bool> lit_assigned, vector<bool> lit_value)
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
                if(lit_assigned[lit] && lit_value[lit])
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