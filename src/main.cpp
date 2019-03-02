#include "tokens.hpp"
#include "parser.hpp"
#include "types.hpp"
#include <iostream>

using namespace std;

template <typename T>
unique_ptr<T> unique_from(T&& t)
{
    return make_unique<T>(std::move(t));
}

int main()
{
    vector<TokPtr> tokens;
    
    auto p = [&](auto&& t)
    {
       tokens.push_back(unique_from(std::move(t))); 
    };

    auto name = [&](auto&& n) { p(VarTok(n)); };
    auto num = [&](auto&& n) { p(NumTok(n)); };

    auto lpar = [&]() { p(LParTok()); };
    auto rpar = [&]() { p(RParTok()); };
    
    name("ret");
    name("fun");
    lpar();
    name("type1");
    name("arg1");
    p(CommaTok());
    name("type2");
    name("arg2");
    rpar();
    p(LBraceTok());
    name("int");
    name("x");
    p(AssignTok());
    num(80);
    p(SemiTok());
    name("float");
    name("y");
    p(AssignTok());
    num(90);
    p(SemiTok());
    p(RBraceTok());

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
