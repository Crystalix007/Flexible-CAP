#pragma once

#if !defined(yyFlexLexerOnce)
#	include <FlexLexer.h>
#endif

#include "trueArgGrammarDriver.hpp"
#include <cstring>
#include <optional>
#include <sstream>

namespace trueArgGrammar {
	class Scanner : public yyFlexLexer {
	public:
		using Result = Driver::Result;

		Scanner(int argc, char* argv[], Driver& driver);
		virtual ~Scanner() = default;

		using FlexLexer::yylex;

		virtual int yylex(trueArgGrammar::Parser::semantic_type* const lval);
		void resetOnWrap();
		void setResult(Result result);

	private:
		int argc, argi;
		char** argv;
		Driver& driver;
		std::stringstream streamInput;
		std::optional<int> resetVal;
		trueArgGrammar::Parser::semantic_type* yyval = nullptr;

	protected:
		int yywrap() override;
	};
} // namespace trueArgGrammar
