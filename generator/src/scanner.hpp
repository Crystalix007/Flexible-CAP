#pragma once

#if !defined(yyFlexLexerOnce)
# define yyFlexLexer __yyFlexLexer
#	include <FlexLexer.h>
# undef yyFlexLexer
#endif

#include "parser.tab.hh"

namespace Grammar {
	class Scanner : public __yyFlexLexer {
	public:
		Scanner(std::istream* in) : __yyFlexLexer{ in } {};

		virtual ~Scanner(){};

		using FlexLexer::yylex;

		virtual int yylex(Grammar::Parser::semantic_type* const lval,
		                  Grammar::Parser::location_type* lloc);

	private:
		Grammar::Parser::semantic_type* yyval = nullptr;
		Grammar::Parser::location_type* loc = nullptr;
	};
} // namespace Grammar
