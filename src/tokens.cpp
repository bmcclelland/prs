#include "tokens.hpp"

string to_string(Tok const& tok)
{
    static struct
    {
        string operator()(EndTok)       { return "END"; }
        string operator()(LParTok)	    { return "("; }
        string operator()(RParTok)	    { return ")"; }
        string operator()(LBraceTok)	{ return "{"; }
        string operator()(RBraceTok)	{ return "}"; }
        string operator()(CommaTok)	    { return ","; }
        string operator()(SemiTok)	    { return ";"; }
        string operator()(AddTok)	    { return "+"; }
        string operator()(SubTok)	    { return "-"; }
        string operator()(MulTok)	    { return "*"; }
        string operator()(DivTok)	    { return "/"; }
        string operator()(AndTok)	    { return "&&"; }
        string operator()(OrTok)	    { return "||"; }
        string operator()(NegTok)	    { return "-"; }
        string operator()(NotTok)	    { return "!"; }
        string operator()(AssignTok)	{ return "="; }
        
        string operator()(NumTok const& t)
        {
            return to_string(t.value);
        }
        
        string operator()(VarTok const& t)
        {
            return t.value;
        }
    } 
    visitor;

    return std::visit(visitor, tok);
}

string to_string(vector<Tok> const& toks)
{
    string s;
    bool first = true;

    for (Tok const& tok : toks)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s += " ";
        }

        s += to_string(tok);
    }

    return s;
}
