#pragma once

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

enum class TraceResult { undefined, success, failure, subfailure };

inline
string to_string(TraceResult t)
{
    switch (t)
    {
        case TraceResult::undefined:    return "undefined";
        case TraceResult::success:      return "success";
        case TraceResult::failure:      return "failure";
        case TraceResult::subfailure:   return "subfailure";
    }

    std::abort();
}

struct Trace
{
    string name;
    size_t depth;
    Trace* parent;
    vector<Trace> children;
    TraceResult result = TraceResult::undefined;

    Trace(string _name, Trace* _parent, size_t _depth)
        : name(_name)
        , depth(_depth)
        , parent(_parent)
    {}

};

class Tracer
{
    Trace _root;
    Trace* _cur;
    size_t _indent;
      
    void finalize(Trace& t, bool flag)
    {
        if (flag)
        {
            if (t.result == TraceResult::success)
            {
                t.result = TraceResult::subfailure;
            }
        }
        else
        {
            if (t.result == TraceResult::failure)
            {
                flag = true;
            }
        }

        for (Trace& c : t.children)
        {
            finalize(c, flag);
        }
    }

    void print(Trace const& t) const
    {
        int color = [&]()
        {
            switch (t.result)
            {
                case TraceResult::undefined:    return 0;
                case TraceResult::success:      return 32;
                case TraceResult::failure:      return 31;
                case TraceResult::subfailure:   return 33;
            }

            std::abort();
        }();

		auto printColor = [](int c)
		{
			std::cout << "\033[" << c << "m";
		};
        
        printColor(color);
        std::cout << string(t.depth * _indent, ' ') << t.name << "\n";
        printColor(0);

        for (Trace const& c : t.children)
        {
            print(c);
        }
    }

    public:
        Tracer(string root_name, size_t indent)
            : _root(root_name, nullptr, 0)
            , _cur(&_root)
            , _indent(indent)
        {}

        void push(string name)
        {
            _cur->children.push_back(
                Trace(name, _cur, _cur->depth + 1)
                );
            _cur = &_cur->children.back();
        }

        void pop(TraceResult result)
        {
            if (_cur)
            {
                _cur->result = result;
                _cur = _cur->parent;
            }
        }

        void finalize()
        {
            return finalize(_root, false);
        }
    
        void print() const
        {
            return print(_root);
        }

};
