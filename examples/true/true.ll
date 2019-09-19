%{
#include <stdexcept>
#include "trueArgGrammarParser.tab.hh"

#include "trueArgGrammarScanner.hpp"
#undef YY_DECL
#define YY_DECL int trueArgGrammar::Scanner::yylex(trueArgGrammar::Parser::semantic_type* const lval)

using token = trueArgGrammar::Parser::token;
%}

%option debug
%option nodefault
%option yyclass="trueArgGrammar::Scanner"
%option noyywrap
%option c++
%option outfile="trueArgGrammarScannerDef.cpp"

%x SHORT_ARGUMENTS ARGUMENT_VALUE POSITIONAL_ARGUMENTS

%%

%{	/** Code executed at the beginning of yylex **/
	yyval = lval;
%}

<INITIAL>-- { /* end of arguments */
	BEGIN(POSITIONAL_ARGUMENTS);
}

<INITIAL>-/. {
	BEGIN(SHORT_ARGUMENTS);
	resetVal = INITIAL;
}

	/** Default arguments implemented for all parsers **/

<INITIAL>--help {
	return token::ARGUMENT_HELP_LONG;
}

<INITIAL>--version {
	return token::ARGUMENT_VERSION_LONG;
}

<INITIAL>--[[:alnum:]][[:alnum:]\-]* {
	std::cerr << "Unknown argument: \'" << yytext << "\' found\n";
	setResult(Result::wrongArgument);
}

<INITIAL>--.+ { /* show common error for invalid option */
	std::cerr << "Invalid argument: \'" << yytext << "\' found\n";
	setResult(Result::wrongArgument);
}

<SHORT_ARGUMENTS>. {
	std::cerr << "Unknown argument \'-" << yytext << "\' found\n";
	setResult(Result::wrongArgument);
}

<POSITIONAL_ARGUMENTS>.+ {
	std::cerr << "Unknown positional argument \'" << yytext << "\' found\n";
	setResult(Result::wrongArgument);
}

<INITIAL>[^\-].* { /* unknown positional argument */
	std::cerr << "Unknown positional argument \'" << yytext << "\' found\n";
	setResult(Result::wrongArgument);
}

<INITIAL>. {
	std::cerr << "Invalid character \'" << yytext << "\' found\n";
	setResult(Result::wrongArgument);
}

%%

namespace trueArgGrammar {
void Scanner::resetOnWrap() {
	if (resetVal)
		BEGIN(*resetVal);

	resetVal = INITIAL;
}
}
