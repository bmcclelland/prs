#pragma once

#include "tokens.hpp"
#include "tracer.hpp"
#include <vector>

using std::vector;

struct State
{
    vector<TokPtr> const& tokens;
    Tracer tracer;
    unsigned pos = 0;

    State(vector<TokPtr> const& _tokens)
        : tokens(_tokens)
        , tracer("", 3)
    {}
};

inline 
Tok const* cur_unchecked(State const& s)
{
    return s.tokens[s.pos].get();
}

inline 
bool at_end(State const& s)
{
    return s.pos >= s.tokens.size();
}

inline
Tok const* cur(State const& s)
{
    if (at_end(s))
    {
        return nullptr;
    }
    else
    {
        return cur_unchecked(s);
    }
}

template <typename T>
T const* match_cur(State& s)
{
    if (at_end(s))
    {
        return nullptr;
    }


    T const* t = dynamic_cast<T const*>(cur_unchecked(s));

    if (!t)
    {
        return nullptr;
    }

    ++s.pos;
    return t;
}
