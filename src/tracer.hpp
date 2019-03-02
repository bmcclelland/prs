#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

enum class TraceResult 
{ 
    undefined, 
    success, 
    failure, 
    subfailure 
};

class Tracer
{
    public:
        Tracer(string const& root_name, unsigned indent);
        void push(string const& label);
        void pop(TraceResult);
        void finalize();
        void print() const;
    
    private:
        struct Trace
        {
            string        name;
            unsigned      depth;
            Trace*        parent;
            vector<Trace> children;
            TraceResult   result = TraceResult::undefined;

            Trace(string const& name, Trace* parent, unsigned depth);
        };

    private:
        Trace    _root;
        Trace*   _cur;
        unsigned _indent;

        string indent(unsigned) const;
        void finalize(Trace&, bool);
        int print(Trace const&) const;
};

string to_string(TraceResult);
