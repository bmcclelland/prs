#pragma once

#include "state.hpp"
#include "tokens.hpp"
#include <optional>
#include <tuple>
#include <functional>
#include <iostream>
#include <set>

using std::optional;
using std::nullopt;
using std::tuple;
using std::function;
using std::set;

template <typename... Ts>
using Parsed = optional<tuple<Ts...>>;

template <typename R>
using RParser = function<R(State&)>;

template <typename... Ts>
using Parser = RParser<Parsed<Ts...>>;

template <typename R, typename... Ts>
using Finisher = function<Parsed<R>(Ts...)>;

template <typename T>
Finisher<T> value(T t)
{
    return [=]() -> Parsed<T>
    {
        return t;
    };
}

template <typename P>
Parser<P> thunk(Parser<P>(&f)())
{
    return [&f](State& s) -> Parsed<P>
    {
        static set<unsigned> fails;
        unsigned pos = s.pos;

        if (fails.count(pos))
        {
            return nullopt;
        }

        auto r = f()(s);

        if (!r)
        {
            fails.insert(pos);
        }

        return r;
    };
}

template <typename F,
    typename G = std::invoke_result_t<F>,
    typename R = std::invoke_result_t<G,State&>>
RParser<R> thunk(F const& f)
{
    return [f](State& s) -> R
    {
        static set<unsigned> fails;
        unsigned pos = s.pos;

        if (fails.count(pos))
        {
            return nullopt;
        }

        auto r = f()(s);

        if (!r)
        {
            fails.insert(pos);
        }

        return r;
    };
}

template <typename T>
Parser<T> parse_token = [](State& s) -> Parsed<T>
{
    T const* t = match_cur<T>(s);
    
    if (!t)
        return nullopt;
    
    return *t;
};

template <typename T>
Parser<> match = [](State& s) -> Parsed<>
{
    T const* t = match_cur<T>(s);
    
    if (!t)
        return nullopt;
    
    return tuple<>();
};


template <typename... P1, typename... P2>
Parser<P1..., P2...> operator>>(Parser<P1...> const& p1, Parser<P2...> const& p2)
{
    return [=](State& s) -> Parsed<P1..., P2...>
    {
        unsigned pos = s.pos;

        Parsed<P1...> r1 = p1(s);

        if (!r1)
        {
            s.pos = pos;
            return nullopt;
        }

        Parsed<P2...> r2 = p2(s);

        if (!r2)
        {
            s.pos = pos;
            return nullopt;
        }

        return std::tuple_cat(std::move(*r1), std::move(*r2));
    };
}

template <typename... P1, typename... P2>
Parser<P1...> operator|(Parser<P1...> const& p1, Parser<P2...> const& p2)
{
    return [=](State& s) -> Parsed<P1...>
    {
        unsigned pos = s.pos;
    
        if (Parsed<P1...> r1 = p1(s))
        {
            return r1;
        }
        else
        {
            s.pos = pos;
            return p2(s);
        }
    };
}

template <typename F, typename... P,
    typename R = std::invoke_result_t<F,P...>>
RParser<R> operator>>(Parser<P...> const& p, F const& f)
{
    return [=](State& s) -> R
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
    return [=](State& s) -> Parsed<vector<P>>
    {
        vector<P> rs;

        unsigned pos = s.pos;
        Parsed<P> r = p(s);

        while (true)
        {
            if (!r)
            {
                s.pos = pos;
                return rs;
            }
            
            rs.push_back(std::get<0>(*r));
            pos = s.pos;
            r = q(s); // Note the q.
        }
    };
}

template <typename P, typename Q>
Parser<vector<P>> parse_many(Parser<P> const& p)
{
    return _parse_many(p, p);
}

template <typename T, typename P>
Parser<vector<P>> parse_listT(Parser<P> const& p)
{
    Parser<P> q = match<T> >> p;
    return _parse_many(p, q);
}

template <typename P>
Parser<vector<P>> parse_list(Parser<P> const& p)
{
    return parse_listT<CommaTok>(p);
}

