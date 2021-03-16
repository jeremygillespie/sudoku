#include <iostream>
#include <forward_list>
#include <vector>
#include <string>

using namespace std;

struct literal
{
    int id;
    bool value;
};

literal parse_literal(string s)
{
    if(s[0] == '~')
    {
        int id = stoi(s.substr(1));
        return literal{id, false};
    }
    else
    {
        int id = stoi(s);
        return literal{id, true};
    }
}

vector<literal> dpll(forward_list<vector<literal>> clauses, vector<literal> assigned)
{
}

int main()
{
    forward_list<vector<literal>> clauses = {};
    
    string line;
    while(getline(cin, line))
    {
        vector<literal> clause{};

        size_t pos = 0;
        string token;
        while ((pos = line.find(' ')) != std::string::npos) {
            token = line.substr(0, pos);
            clause.push_back(parse_literal(token));
            line.erase(0, pos + 1);
        }
        clause.push_back(parse_literal(line));

        clauses.push_front(clause);
    }

    vector<literal> solution = dpll(clauses, {});
    for(auto it = solution.begin(); it != solution.end(); ++it)
    {
        if(it->value)
            cout << it->id << " ";
    }
    cout << "\n";
}

literal parse_literal(int puzzle_size, string s)
{
    int subgrid, location, number; 
    bool value;
    if(s[0] == '~')
    {
        subgrid = s[1] - '0' - 1;
        location = s[2] - '0' - 1;
        number  = s[3] - '0' - 1;
        value = false;
    }
    else
    {
        subgrid = s[0] - '0' - 1;
        location = s[1] - '0' - 1;
        number  = s[2] - '0' - 1;
        value = false;
    }
    int id = puzzle_size * puzzle_size * subgrid + puzzle_size * location + number;
    return literal{id, value};
}