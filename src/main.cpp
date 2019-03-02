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
    
    p(NumTok(1));
    p(MulTok());
    p(NumTok(2));
    p(AddTok());
    p(NumTok(3));
    p(MulTok());
    p(NumTok(4));

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
