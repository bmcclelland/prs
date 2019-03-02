#pragma once

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

enum class Op { opAdd, opSub, opMul, opDiv, opAnd, opOr, opNot, opNeg };

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
        return "(" + type + " " + name + " " + value->to_string() + ")";
    }
};
