#pragma once

#include "ast.hpp"
#include "parsercombi.hpp"

Parsed<ASTPtr> parse(vector<Tok> const&);
