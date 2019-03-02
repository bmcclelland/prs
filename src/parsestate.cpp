#include "parsestate.hpp"

ParseState::ParseState(vector<Tok> const& tokens)
    : _tokens(tokens)
    , _tracer("", 2)
{}

Tok const& ParseState::cur_unchecked() const
{
    return _tokens[_pos];
}

bool ParseState::at_end() const
{
    return _pos >= _tokens.size();
}

unsigned ParseState::pos() const
{
    return static_cast<unsigned>(_pos);
}

Tok const& ParseState::cur() const
{
    static Tok const end = EndTok{};

    if (at_end())
    {
        return end;
    }
    else
    {
        return cur_unchecked();
    }
}

void ParseState::set_pos(unsigned pos)
{
    if (pos > _tokens.size())
    {
        _pos = static_cast<unsigned>(_tokens.size());
    }
    else
    {
        _pos = pos;
    }
}

void ParseState::push_trace(string const& label)
{
    _tracer.push(label);
}

void ParseState::pop_trace_success()
{
    _tracer.pop(TraceResult::success);
}

void ParseState::pop_trace_failure()
{
    _tracer.pop(TraceResult::failure);
}

void ParseState::print_trace()
{
    _tracer.finalize();
    _tracer.print();
}
