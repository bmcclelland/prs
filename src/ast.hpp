#pragma once

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

enum class Op { opAdd, opSub, opMul, opDiv, opAnd, opOr, opNot, opNeg };

struct Arg
{
    string type;
    string name;

    Arg() = default;
    Arg(string _type, string _name)
        : type(std::move(_type))
        , name(std::move(_name))
    {}
};

inline
string to_string(Arg const& arg)
{
    return "[" + arg.type + " " + arg.name + "]";
}

inline
string to_string(Op op)
{
    switch (op)
    {
        case Op::opAdd: return "+";
        case Op::opSub: return "-";
        case Op::opMul: return "*";
        case Op::opDiv: return "/";
        case Op::opAnd: return "&&";
        case Op::opOr:  return "||";
        case Op::opNot: return "!";
        case Op::opNeg: return "-";
    }

    std::abort();
}

struct AST
{
    virtual ~AST() {}
    virtual string to_string() const = 0;
};

using ASTPtr = shared_ptr<AST>;

struct ASTNum : AST
{
    int value;
    string to_string() const { return std::to_string(value); }
    ASTNum(int _value) : value(_value) {}
};

struct ASTVar : AST
{
    string value;
    string to_string() const { return value; }
    ASTVar(string _value) : value(_value) {}
};

struct ASTBinop : AST
{
    ASTPtr left;
    Op op;
    ASTPtr right;
    ASTBinop(ASTPtr _left, Op _op, ASTPtr _right) 
        : left(std::move(_left))
        , op(_op)
        , right(std::move(_right)) 
    {}
    string to_string() const 
    { 
        return "(" + left->to_string() 
            + ::to_string(op) + right->to_string() + ")"; 
    }
};

struct ASTUnop : AST
{
    Op op;
    ASTPtr right;
    ASTUnop(Op _op, ASTPtr _right) 
        : op(_op)
        , right(std::move(_right)) 
    {}
    string to_string() const 
    { 
        return "(" + ::to_string(op) + right->to_string() + ")"; 
    }
};

struct ASTBlock :  AST
{
    vector<ASTPtr> stmts;
    ASTBlock(vector<ASTPtr> _stmts)
        : stmts(std::move(_stmts))
    {}
    string to_string() const
    {
        string s = "(";
        for (auto&& stmt : stmts)
        {
            s += stmt->to_string();
        }
        return s + ")";
    }
};

struct ASTVarDecl : AST
{
    string type;
    string name;
    ASTPtr value;
    ASTVarDecl(string _type, string _name, ASTPtr _value)
        : type(_type)
        , name(_name)
        , value(std::move(_value))
    {}
    string to_string() const
    {
        return "(" + type + " " + name + " = " + value->to_string() + ")";
    }
};  

struct ASTAssign : AST
{
    string name;
    ASTPtr value;
    ASTAssign(string _name, ASTPtr _value)
        : name(_name)
        , value(std::move(_value))
    {}
    string to_string() const
    {
        return "(" + name + " = " + value->to_string() + ")";
    }
};  

struct ASTFunc : AST
{
    string ret_type;
    string name;
    vector<Arg> args;
    ASTPtr body;
    ASTFunc(string _ret_type, string _name, vector<Arg> _args, ASTPtr _body)
        : ret_type(std::move(_ret_type))
        , name(std::move(_name))
        , args(std::move(_args))
        , body(std::move(_body))
    {}
    string to_string() const
    {
        string s;
        for (auto&& a : args)
        {
            s += ::to_string(a);
        }
        return ret_type + " " + name + "(" + s + ")" + body->to_string();
    }
};

struct ASTProgram : AST
{
    vector<ASTPtr> decls;
    ASTProgram(vector<ASTPtr> _decls)
        : decls(std::move(_decls))
    {}
    string to_string() const
    {
        string s;
        for (auto&& d : decls)
        {
            s += d->to_string();
            s += "\n";
        }
        return s;
    }
};
