#pragma once

#include "tokens.hpp"
#include "tracer.hpp"
#include <vector>

using std::vector;

class ParseState
{
    private:
        vector<Tok> const& _tokens;
        Tracer _tracer;
        unsigned _pos = 0;
        
        Tok const& cur_unchecked() const;

    public:
        ParseState(vector<Tok> const&);
       
        bool at_end() const;
        unsigned pos() const;
        Tok const& cur() const;
        
        void set_pos(unsigned);
        template <typename T> T const* match();
        void push_trace(string const&);
        void pop_trace_success();
        void pop_trace_failure();
        void print_trace();
};

template <typename T>
T const* ParseState::match()
{
    if (at_end())
    {
        return nullptr;
    }

    T const* t = std::get_if<T>(&cur_unchecked());

    if (!t)
    {
        return nullptr;
    }

    ++_pos;
    return t;
}
