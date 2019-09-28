#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.tab.hh"

namespace Grammar {
class Scanner : public yyFlexLexer {
public:
  Scanner(std::istream *in) : yyFlexLexer{in} {};

  virtual ~Scanner(){};

  using FlexLexer::yylex;

  virtual int yylex(Grammar::Parser::semantic_type *const lval,
                    Grammar::Parser::location_type *lloc);

private:
  Grammar::Parser::semantic_type *yyval = nullptr;
  Grammar::Parser::location_type *loc = nullptr;
};
} // namespace Grammar
