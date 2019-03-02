#pragma once

#include <string>
#include <memory>

using std::string;
using std::to_string;
using std::unique_ptr;

struct Tok
{
    virtual ~Tok() {}
    virtual string to_string() const = 0;
};

using TokPtr = unique_ptr<Tok>;

struct NumTok : Tok
{
    int value;
    string to_string() const { return std::to_string(value); }
    NumTok(int x) : value(x) {}
};

struct VarTok : Tok
{
    string value;
    string to_string() const { return value; }
    VarTok(string x) : value(x) {}
};

#define OPTOK(T, s) struct T : Tok { string to_string() const { return s; }}

OPTOK(LParTok,   "(");
OPTOK(RParTok,   ")");
OPTOK(LBraceTok, "{");
OPTOK(RBraceTok, "}");
OPTOK(CommaTok,  ",");
OPTOK(SemiTok,   ";");
OPTOK(AddTok,    "+");
OPTOK(SubTok,    "-");
OPTOK(MulTok,    "*");
OPTOK(DivTok,    "/");
OPTOK(AndTok,    "&&");
OPTOK(OrTok,     "||");
OPTOK(NotTok,    "!");
OPTOK(AssignTok, "=");
