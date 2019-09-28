{{=@@ @@=}}
#pragma once

#if !defined(yyFlexLexerOnce)
#	include <FlexLexer.h>
#endif

#include "@@{argspec}@@ArgGrammarDriver.hpp"
#include <cstring>
#include <optional>
#include <sstream>

namespace @@{argspec}@@ArgGrammar {
	class Scanner : public yyFlexLexer {
	public:
		using Result = Driver::Result;

		Scanner(int argc, char* argv[], Driver& driver);
		virtual ~Scanner() = default;

		using FlexLexer::yylex;

		virtual int yylex(Parser::semantic_type* const lval);
		void resetOnWrap();
		void setResult(Result result);

	private:
		int argc, argi;
		char** argv;
		Driver& driver;
		std::stringstream streamInput;
		std::optional<int> resetVal;
		Parser::semantic_type* yyval = nullptr;

	protected:
		int yywrap() override;
	};
} // namespace trueArgGrammar
