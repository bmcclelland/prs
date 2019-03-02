#pragma once

#include "parsestate.hpp"
#include "tokens.hpp"
#include <optional>
#include <tuple>
#include <functional>
#include <unordered_map>
#include <boost/core/demangle.hpp>

using std::optional;
using std::nullopt;
using std::tuple;
using std::function;
using std::unordered_map;

template <typename... Ts>
using Parsed = optional<tuple<Ts...>>;

template <typename R>
using RParser = function<R(ParseState&)>;

template <typename... Ts>
using Parser = RParser<Parsed<Ts...>>;

template <typename R, typename... Ts>
using Finisher = function<Parsed<R>(Ts...)>;

struct TraceTag { string func; };
struct MemoTag  { string func; };
struct NullTag {};

#define TRACE TraceTag{__func__}
#define MEMO  MemoTag{__func__}

template <typename... R>
Parser<R...> operator /=(NullTag const&, Parser<R...> const& p)
{
    return p;
}

template <typename... R>
Parser<R...> operator /=(TraceTag const& trace, Parser<R...> const& p)
{
    return [=](ParseState& s) -> Parsed<R...>
    {
        s.push_trace(trace.func + " " + to_string(s.cur()));

        auto r = p(s);

        if (r)
        {
            s.pop_trace_success();
        }
        else
        {
            s.pop_trace_failure();
        }

        return r;
    };
}

template <typename... R>
Parser<R...> operator /=(MemoTag const& memo, Parser<R...> const& p)
{
    return [=](ParseState& s) -> Parsed<R...>
    {
        struct Memo
        {
            Parsed<R...> value;
            unsigned end_pos;
        };

        using MemoMap = unordered_map<unsigned, Memo>;
        
        static unordered_map<string, MemoMap> maps; 
       
        auto& map = maps[memo.func];
        unsigned const start_pos = s.pos();

        auto const it = map.find(start_pos);

        if (it != map.end())
        {
            s.set_pos(it->second.end_pos);
            return it->second.value;
        }

        auto r = p(s);
        unsigned const end_pos = s.pos();
        map.insert({start_pos, Memo{r, end_pos}});
        return r;
    };
}

template <typename T>
Parser<> match()
{
    Parser<> p = [](ParseState& s) -> Parsed<>
    {
        T const* t = s.match<T>();
        
        if (!t)
            return nullopt;
        
        return tuple<>();
    };

    string Tname = boost::core::demangle(typeid(T).name());
    return TraceTag{"match<" + Tname + ">"}
        /= p;
}

inline
Parser<> parse_end()
{
    Parser<> p = [](ParseState& s) -> Parsed<>
    {
        if (s.at_end())
        {
            return tuple<>();
        }
        else
        {
            return nullopt;
        }
    };

    return TRACE
        /= p;
}

template <typename T>
Finisher<T> value(T t)
{
    return [=]() -> Parsed<T>
    {
        return t;
    };
}

template <typename P>
Parser<P> lazy(Parser<P>(&f)())
{
    return [&f](ParseState& s) -> Parsed<P>
    {
        static Parser<P> const p = f();
        return p(s);
    };
}

template <typename F,
    typename G = std::invoke_result_t<F>,
    typename R = std::invoke_result_t<G,ParseState&>>
RParser<R> lazy(F const& f)
{
    return [f](ParseState& s) -> R
    {
        static auto const p = f();
        return p(s);
    };
}

template <typename T>
Parser<T> parse_token = [](ParseState& s) -> Parsed<T>
{
    T const* t = s.match<T>();
    
    if (!t)
        return nullopt;
    
    return *t;
};

template <typename... P1, typename... P2>
Parser<P1..., P2...> operator>>(Parser<P1...> const& p1, Parser<P2...> const& p2)
{
    return [=](ParseState& s) -> Parsed<P1..., P2...>
    {
        unsigned const start_pos = s.pos();

        Parsed<P1...> r1 = p1(s);

        if (!r1)
        {
            s.set_pos(start_pos);
            return nullopt;
        }

        Parsed<P2...> r2 = p2(s);

        if (!r2)
        {
            s.set_pos(start_pos);
            return nullopt;
        }

        return std::tuple_cat(std::move(*r1), std::move(*r2));
    };
}

template <typename... P1, typename... P2>
Parser<P1...> operator|(Parser<P1...> const& p1, Parser<P2...> const& p2)
{
    return [=](ParseState& s) -> Parsed<P1...>
    {
        unsigned const start_pos = s.pos();
    
        if (Parsed<P1...> r1 = p1(s))
        {
            return r1;
        }
        else
        {
            s.set_pos(start_pos);
            return p2(s);
        }
    };
}

template <typename F, typename... P,
    typename R = std::invoke_result_t<F,P...>>
RParser<R> operator>>(Parser<P...> const& p, F const& f)
{
    return [=](ParseState& s) -> R
    {
        Parsed<P...> r = p(s);

        if (!r) 
            return nullopt;

        return std::apply(f, *r);
    };
}

template <typename P>
Parser<vector<P>> _parse_many(Parser<P> const& p, Parser<P> const& q)
{
    return [=](ParseState& s) -> Parsed<vector<P>>
    {
        vector<P> rs;

        unsigned start_pos = s.pos();
        Parsed<P> r = p(s);

        while (true)
        {
            if (!r)
            {
                s.set_pos(start_pos);
                return rs;
            }
            
            rs.push_back(std::get<0>(*r));
            start_pos = s.pos();
            r = q(s); // Note the q.
        }
    };
}

template <typename P>
Parser<vector<P>> zero_or_more(Parser<P> const& p)
{
    return _parse_many(p, p);
}

template <typename T, typename P>
Parser<vector<P>> parse_listT(Parser<P> const& p)
{
    Parser<P> q = match<T>() >> p;
    return _parse_many(p, q);
}

template <typename P>
Parser<vector<P>> parse_list(Parser<P> const& p)
{
    return parse_listT<CommaTok>(p);
}

