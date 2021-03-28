{{=@@ @@=}}
%{
#include <stdexcept>
#include "@@{argspec}@@ArgGrammarParser.tab.hh"

#include "@@{argspec}@@ArgGrammarScanner.hpp"
#undef YY_DECL
#define YY_DECL int @@{argspec}@@ArgGrammar::Scanner::yylex(@@{argspec}@@ArgGrammar::Parser::semantic_type* const lval)

using token = @@{argspec}@@ArgGrammar::Parser::token;
%}

%option debug
%option nodefault
%option yyclass="@@{argspec}@@ArgGrammar::Scanner"
%option noyywrap
%option c++
%option outfile="@@{argspec}@@ArgGrammarScanner.ll.cc"

%x SHORT_ARGUMENTS ARGUMENT_VALUE POSITIONAL_ARGUMENTS@@#argument_tokens@@@@#parameters@@ @@{clean_token}@@_PARAMETER_@@{index}@@@@/parameters@@@@/argument_tokens@@

ANYTHING .|\n
LONGARG [[:alnum:]][[:alnum:]\-]*

%%

%{	/** Code executed at the beginning of yylex **/
	yyval = lval;
	static std::vector<std::string> parameters;
%}@@#argument_tokens@@@@#has_parameters@@

<@@#parameters@@@@{clean_token}@@_PARAMETER_@@{index}@@@@#next_state@@,@@/next_state@@@@/parameters@@>-- { /** invalid usage of --@@{argument}@@ flag **/
	BEGIN(POSITIONAL_ARGUMENTS);
	std::cerr << "--@@{argument}@@ has not been given sufficient parameters:\n";

	for (const auto& parameter : parameters)
		std::cerr << "\t- \'" << parameter << "\'\n";

	setResult(Result::failure);
}@@/has_parameters@@@@/argument_tokens@@

<*>-- { /* end of arguments */
	BEGIN(POSITIONAL_ARGUMENTS);
}

	/** Arguments **/
@@#argument_tokens@@

<INITIAL>--@@{argument}@@ {@@#has_parameters@@
	BEGIN(@@{clean_token}@@_PARAMETER_1);
	parameters = {};@@/has_parameters@@@@^has_parameters@@
	driver.addArg(@@{argspec}@@ArgGrammar::FlagArg::@@{clean_token}@@);
	return token::ARGUMENT_@@{clean_token}@@;@@/has_parameters@@
}@@#short_argument@@

<SHORT_ARGUMENTS>@@{short_argument}@@ {@@#has_parameters@@
	BEGIN(@@{clean_token}@@_PARAMETER_1);
	parameters = {};@@/has_parameters@@@@^has_parameters@@
	driver.addArg(@@{argspec}@@ArgGrammar::FlagArg::@@{clean_token}@@);
	return token::ARGUMENT_@@{clean_token}@@;@@/has_parameters@@
}@@/short_argument@@@@#parameters@@

<@@{clean_token}@@_PARAMETER_@@{index}@@>{ANYTHING}+ {@@#next_state@@
	BEGIN(@@{clean_token}@@_PARAMETER_@@{.}@@);@@/next_state@@@@^next_state@@
	BEGIN(INITIAL);@@/next_state@@
	parameters.push_back(yytext);@@^next_state@@
	driver.addArg(@@{argspec}@@ArgGrammar::ParamArg::@@{clean_token}@@, parameters);
	return token::ARGUMENT_@@{clean_token}@@;@@/next_state@@
}@@/parameters@@
@@/argument_tokens@@

	/** Default (error) arguments */

<INITIAL>--{LONGARG} {
	std::cerr << "Unknown argument: \'" << yytext << "\' found\n";
	setResult(Result::failure);
}

<INITIAL>--{ANYTHING}+ { /* show common error for invalid option */
	std::cerr << "Invalid argument: \'" << yytext << "\' found\n";
	setResult(Result::failure);
}

<SHORT_ARGUMENTS>{ANYTHING} {
	std::cerr << "Unknown argument \'-" << yytext << "\' found\n";
	setResult(Result::failure);
}

	/** Check for short argument **/

<INITIAL>-/{ANYTHING}+ {
	BEGIN(SHORT_ARGUMENTS);
}

	/** Check for possible unknown arguments **/

<POSITIONAL_ARGUMENTS>{ANYTHING}+ {
	/* std::cerr << "Unknown positional argument \'" << yytext << "\' found\n";
	setResult(Result::failure); */
	yyval->build<std::string>(yytext);
	return token::POSITIONAL_ARGUMENT;
}

<INITIAL>{ANYTHING}* { /* unknown positional argument */
	BEGIN(POSITIONAL_ARGUMENTS);
	/* std::cerr << "Unknown positional argument \'" << yytext << "\' found\n";
	setResult(Result::failure); */
	yyval->build<std::string>(yytext);
	return token::POSITIONAL_ARGUMENT;
}

%%

namespace @@{argspec}@@ArgGrammar {
	void Scanner::resetOnWrap() {
		switch (YYSTATE) {
			case SHORT_ARGUMENTS:
				BEGIN(INITIAL);
				break;
		}
	}
}
