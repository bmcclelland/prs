#include "tokens.hpp"
#include "parser.hpp"
#include "types.hpp"
#include <iostream>

using namespace std;

int main()
{
    auto name   = [](char const* s) { return VarTok{s}; };
    auto num    = [](int i)         { return NumTok{i}; };
    auto lpar   = LParTok{};
    auto rpar   = RParTok{};
    auto lbrace = LBraceTok{};
    auto rbrace = RBraceTok{};
    auto comma  = CommaTok{};
    auto semi   = SemiTok{};
    auto assign = AssignTok{};

    vector<Tok> tokens
    {
        name("ret"),
        name("fun"),
        lpar,
            name("type1"), name("arg1"),
            comma,
            name("type2"), name("arg2"),
        rpar,
        lbrace,
            name("int"), name("x"), assign, num(80), semi,
            name("flt"), name("y"), assign, num(90), semi,
        rbrace,
    };

    auto r = parse(tokens);
    
    if (r)
    {
        auto& [x] = *r;
        cout << x->to_string() << "\n";
    }
    else
    {
        cout << "failure\n";
    }
}
