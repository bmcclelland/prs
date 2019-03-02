#include "parser.hpp"
#include "tokens.hpp"
#include "state.hpp"
#include "parsercombi.hpp"

using namespace std;


//// TRACING ////

#define TRACE string(__func__)

template <typename R>
Parser<R> operator /=(string const& func, Parser<R> const& p)
{
    return [=](State& s) -> Parsed<R>
    {
        string tok = cur(s) ? cur(s)->to_string() : "END";
        s.tracer.push(func + " " + tok);
    
        auto r = p(s);

        if (r)
        {
            s.tracer.pop(TraceResult::success);
        }
        else
        {
            s.tracer.pop(TraceResult::failure);
        }

        return r;
    };
}


//// HELPERS ////

template <typename T>
auto construct = [](auto&&... args) -> Parsed<ASTPtr>
{
    return make_unique<T>(std::move(args)...);
};

template <typename T>
Parser<Op> match_op(Op t)
{
    return match<T> >> value(t);
}


//// PARSERS ////

Parser<ASTPtr> parse_exp();
Parser<ASTPtr> parse_primary();

Parser<ASTPtr> parse_num()
{
    return TRACE
        /= parse_token<NumTok> 
        >> [](NumTok t) -> Parsed<ASTPtr> 
        { 
            return make_unique<ASTNum>(t.value);
        };
}

Parser<ASTPtr> parse_var()
{
    return TRACE
        /= parse_token<VarTok>
        >> [](VarTok t) -> Parsed<ASTPtr>
        {
            return make_unique<ASTVar>(t.value);
        };
}

Parser<int> parse_num_value()
{
    return TRACE
        /= parse_token<NumTok>
        >> [](NumTok t) -> Parsed<int> 
        { 
            return t.value; 
        };
}

Parser<string> parse_var_value()
{
    return TRACE
        /= parse_token<VarTok>
        >> [](VarTok t) -> Parsed<string>
        {
            return t.value;
        };
}

Parser<> parse_end = [](State& s) -> Parsed<>
{
    if (at_end(s))
    {
        return tuple<>();
    }
    else
    {
        return nullopt;
    }
};

Parser<Op> parse_unary_op()
{
    return TRACE
        /= match_op<NotTok>(Op::opNot)
         | match_op<SubTok>(Op::opNeg);
}

Parser<Op> parse_multive_op()
{
    return TRACE
        /= match_op<MulTok>(Op::opMul) 
         | match_op<DivTok>(Op::opDiv);
}

Parser<Op> parse_additive_op() 
{
    return TRACE
        /= match_op<AddTok>(Op::opAdd) 
         | match_op<SubTok>(Op::opSub);
}

Parser<Op> parse_logical_op() 
{
    return TRACE
        /= match_op<AndTok>(Op::opAnd)
         | match_op<OrTok>(Op::opOr);
}

Parser<ASTPtr> parse_unary()
{
    return TRACE
        /= parse_unary_op()
        >> thunk(parse_primary) 
        >> construct<ASTUnop>;
}

Parser<ASTPtr> parse_parens_exp()
{
    return TRACE
        /= match<LParTok> 
        >> thunk(parse_exp) 
        >> match<RParTok>;
}

Parser<ASTPtr> parse_primary()
{
    return TRACE
        /= parse_num()
         | parse_parens_exp()
         | parse_var()
         | parse_unary();
}

Parser<ASTPtr> parse_multive()
{
    return TRACE
        /= parse_primary()
        >> parse_multive_op()
        >> thunk(parse_multive) 
        >> construct<ASTBinop>
         | parse_primary();
}

Parser<ASTPtr> parse_additive()
{
    return TRACE
        /= parse_multive() 
        >> parse_additive_op()
        >> thunk(parse_additive)
        >> construct<ASTBinop>
         | parse_multive();
}

Parser<ASTPtr> parse_logical()
{
    return TRACE
        /= parse_additive()
        >> parse_logical_op()
        >> thunk(parse_logical) 
        >> construct<ASTBinop>
         | parse_additive();
}

Parser<ASTPtr> parse_exp()
{
    return TRACE
        /= parse_logical();
}

Parser<ASTPtr> parse_var_decl()
{
    return TRACE
        /= parse_var_value()
        >> parse_var_value()
        >> match<AssignTok> 
        >> parse_exp() 
        >> construct<ASTVarDecl>;
}

Parser<ASTPtr> parse_program()
{
    return TRACE
        /= parse_logical();
}

Parsed<ASTPtr> parse(vector<TokPtr>const& tokens)
{
    State state(tokens);
    Parser<ASTPtr> parser = parse_program();
    Parsed<ASTPtr> result = parser(state);
    state.tracer.finalize();
    state.tracer.print();
    return result;
}