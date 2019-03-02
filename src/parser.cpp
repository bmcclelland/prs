#include "parser.hpp"
#include "tokens.hpp"
#include "parsestate.hpp"
#include "parsercombi.hpp"

using namespace std;


//// HELPERS ////

template <typename T>
auto make_ast = [](auto&&... args) -> Parsed<ASTPtr>
{
    return make_shared<T>(std::move(args)...);
};

template <typename T>
auto construct = [](auto&&... args) -> Parsed<T>
{
    return T(std::move(args)...);
};

template <typename T>
Parser<Op> parse_op(Op t)
{
    return match<T>() >> value(t);
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
            return make_shared<ASTNum>(t.value);
        };
}

Parser<ASTPtr> parse_var()
{
    return TRACE
        /= parse_token<VarTok>
        >> [](VarTok t) -> Parsed<ASTPtr>
        {
            return make_shared<ASTVar>(t.value);
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

Parser<string> parse_name()
{
    return TRACE
        /= parse_token<VarTok>
        >> [](VarTok t) -> Parsed<string>
        {
            return t.value;
        };
}

Parser<Op> parse_unary_op()
{
    return TRACE
        /= MEMO
        /= parse_op<NotTok>(Op::opNot)
         | parse_op<SubTok>(Op::opNeg);
}

Parser<Op> parse_multive_op()
{
    return TRACE
        /= MEMO
        /= parse_op<MulTok>(Op::opMul) 
         | parse_op<DivTok>(Op::opDiv);
}

Parser<Op> parse_additive_op() 
{
    return TRACE
        /= MEMO
        /= parse_op<AddTok>(Op::opAdd) 
         | parse_op<SubTok>(Op::opSub);
}

Parser<Op> parse_logical_op() 
{
    return TRACE
        /= MEMO
        /= parse_op<AndTok>(Op::opAnd)
         | parse_op<OrTok>(Op::opOr);
}

Parser<ASTPtr> parse_unary()
{
    return TRACE
        /= MEMO
        /= parse_unary_op()
        >> lazy(parse_primary) 
        >> make_ast<ASTUnop>;
}

Parser<ASTPtr> parse_parens_exp()
{
    return TRACE
        /= MEMO
        /= match<LParTok>() 
        >> lazy(parse_exp) 
        >> match<RParTok>();
}

Parser<ASTPtr> parse_primary()
{
    return TRACE
        /= MEMO
        /= parse_num()
         | parse_parens_exp()
         | parse_var()
         | parse_unary();
}

Parser<ASTPtr> parse_multive()
{
    return TRACE
        /= MEMO
        /= parse_primary()
        >> parse_multive_op()
        >> lazy(parse_multive) 
        >> make_ast<ASTBinop>
         | parse_primary();
}

Parser<ASTPtr> parse_additive()
{
    return TRACE
        /= MEMO
        /= parse_multive() 
        >> parse_additive_op()
        >> lazy(parse_additive)
        >> make_ast<ASTBinop>
         | parse_multive();
}

Parser<ASTPtr> parse_logical()
{
    return TRACE
        /= MEMO
        /= parse_additive()
        >> parse_logical_op()
        >> lazy(parse_logical) 
        >> make_ast<ASTBinop>
         | parse_additive();
}

Parser<ASTPtr> parse_exp()
{
    return TRACE
        /= MEMO
        /= parse_logical();
}

Parser<ASTPtr> parse_var_decl()
{
    return TRACE
        /= MEMO
        /= parse_name()
        >> parse_name()
        >> match<AssignTok>() 
        >> parse_exp() 
        >> make_ast<ASTVarDecl>;
}

Parser<ASTPtr> parse_assign()
{
    return TRACE
        /= MEMO
        /= parse_name()
        >> match<AssignTok>()
        >> parse_exp() 
        >> make_ast<ASTAssign>;
}

Parser<ASTPtr> parse_stmt()
{
    static auto const stmt = parse_assign()
                           | parse_var_decl();

    return TRACE
        /= stmt
        >> match<SemiTok>();
}

Parser<ASTPtr> parse_block()
{
    return TRACE
        /= match<LBraceTok>()
        >> zero_or_more(parse_stmt())
        >> match<RBraceTok>()
        >> make_ast<ASTBlock>;
}

Parser<Arg> parse_arg()
{
    return TRACE
        /= parse_name()
        >> parse_name()
        >> construct<Arg>;
}

Parser<vector<Arg>> parse_formal_args()
{
    return TRACE
        /= match<LParTok>()
        >> parse_list(parse_arg())
        >> match<RParTok>();
}

Parser<ASTPtr> parse_func()
{
    return TRACE
        /= parse_name()
        >> parse_name()
        >> parse_formal_args()
        >> parse_block()
        >> make_ast<ASTFunc>;
}

Parser<ASTPtr> parse_program()
{
    return TRACE
        /= zero_or_more(parse_func())
        >> parse_end()
        >> make_ast<ASTProgram>;
}

Parsed<ASTPtr> parse(vector<TokPtr>const& tokens)
{
    ParseState state(tokens);
    Parser<ASTPtr> parser = parse_program();
    Parsed<ASTPtr> result = parser(state);
    state.print_trace();
    return result;
}
