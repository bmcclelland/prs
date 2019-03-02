#pragma once

#include <string>
#include <memory>
#include <variant>
#include <vector>

using std::variant;
using std::string;
using std::to_string;
using std::vector;

struct NumTok
{
    int value;
};

struct VarTok
{
    string value;
};

struct EndTok {};
struct LParTok {};
struct RParTok {};
struct LBraceTok {};
struct RBraceTok {};
struct CommaTok {};
struct SemiTok {};
struct AddTok {};
struct SubTok {};
struct MulTok {};
struct DivTok {};
struct AndTok {};
struct OrTok {};
struct NegTok {};
struct NotTok {};
struct AssignTok {};

using Tok = variant<
    EndTok,
    NumTok,
    VarTok,
    LParTok, RParTok,
    LBraceTok, RBraceTok,
    CommaTok,
    SemiTok,
    AddTok, SubTok, MulTok, DivTok,
    AndTok, OrTok,
    NegTok, NotTok,
    AssignTok
    >;

string to_string(Tok const&);
string to_string(vector<Tok> const&);
