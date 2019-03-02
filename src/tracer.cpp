#include "tracer.hpp"
#include <iostream>

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

Tracer::Trace::Trace(string const& _name, Trace* _parent, unsigned _depth)
    : name(_name)
    , depth(_depth)
    , parent(_parent)
{}
        
string Tracer::indent(unsigned depth) const
{
    return string(depth * _indent, ' ');
}

void Tracer::finalize(Trace& t, bool flag)
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

int Tracer::print(Trace const& t) const
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
    std::cout << indent(t.depth) << t.name << "\n";
    printColor(0);

    int count = 1;

    for (Trace const& c : t.children)
    {
        count += print(c);
    }

    return count;
}

Tracer::Tracer(string const& root_name, unsigned indent)
    : _root(root_name, nullptr, 0)
    , _cur(&_root)
    , _indent(indent)
{}

void Tracer::push(string const& name)
{
    _cur->children.push_back(
        Trace(name, _cur, _cur->depth + 1)
        );
    _cur = &_cur->children.back();
}

void Tracer::pop(TraceResult result)
{
    if (_cur)
    {
        _cur->result = result;
        _cur = _cur->parent;
    }
}

void Tracer::finalize()
{
    return finalize(_root, false);
}

void Tracer::print() const
{
    int count = print(_root);
    std::cout << indent(1) << count << " steps\n";
}
