#ifndef SUDOKU_DPLL_HPP
#define SUDOKU_DPLL_HPP

#include <list>
#include <vector>
#include <unordered_set>

namespace sudoku
{

using namespace std;

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
    auto c = clauses.begin();
    while (c != clauses.end())
    {
        if(c->find(lit) != c->end())
        {
            c = clauses.erase(c);
        }
        else
        {
            c->erase(-lit - 1);
            ++c;
        }
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

} // namespace sudoku

#endif