/* Simply lexes arguments */
%{
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <vector>
#include "parser.tab.hh"
#include "scanner.hpp"
#include "argument.hpp"

#undef YY_DECL
#define YY_DECL int Grammar::Scanner::yylex(Grammar::Parser::semantic_type* const lval, Grammar::Parser::location_type* lloc)
#define YY_USER_ACTION lloc->step(); lloc->columns(yyleng);

using token = Grammar::Parser::token;

#define LOG std::clog
%}

%option debug
%option nodefault
%option yyclass="Grammar::Scanner"
%option noyywrap
%option c++

%x PROGRAM_SECTION PROGRAM_NAME PROGRAM_VALUE PROGRAM_QUOTED_VALUE PROGRAM_QUOTED_VALUE_END USAGE_SECTION USAGE_DETAILS_BEGIN USAGE_DETAILS USAGE_RULE_BEGIN USAGE_RULE ARGUMENTS_SECTION ARGUMENTS_LONGOPT ARGUMENTS_SHORTOPT ARGUMENTS_PARAMETERS ARGUMENTS_DESCRIPTION_BEGIN ARGUMENTS_DESCRIPTION ARGUMENTS_DESCRIPTION_END

NAME [^[:space:]]+
SL_SPACE [[:space:]]{-}[\n]
LONGOPT [[:alpha:]][[:alnum:]\-]*
SHORTOPT [^[:space:]]
PARAM [[:alpha:]][[:alnum:]_]*
RULE_TOKEN [[:alpha:]][[:alpha:]_-]*

%%

%{	/** Code executed at the beginning of yylex **/
	yyval = lval;
	static std::string quotedVal{};
	static std::string programName{};
	static std::vector<std::string> arguments{};
%}

<INITIAL>"Program:" {
	BEGIN(PROGRAM_SECTION);
}

<PROGRAM_SECTION>"Usage:" {
	BEGIN(USAGE_SECTION);
}

<USAGE_SECTION>"Arguments:" {
	BEGIN(ARGUMENTS_SECTION);
}

<PROGRAM_SECTION>{SL_SPACE}+ {
	/* ignored */
}

<PROGRAM_SECTION>\n {
	lloc->lines();
}

<PROGRAM_SECTION>program {
	BEGIN(PROGRAM_NAME);
	return token::PROGRAM;
}

<PROGRAM_NAME>{SL_SPACE}+ {
	/* ignored */
}

<PROGRAM_NAME>{NAME} {
	programName = yytext;
	yyval->build<std::string>(programName);
	return token::VALUE;
}

<PROGRAM_NAME>\n {
	BEGIN(PROGRAM_SECTION);
	lloc->lines();
}

<PROGRAM_SECTION>version {
	BEGIN(PROGRAM_VALUE);
	return token::VERSION;
}

<PROGRAM_VALUE>{SL_SPACE}+ {
	/* ignored */
}

<PROGRAM_VALUE>\n {
	BEGIN(PROGRAM_SECTION);
	throw std::runtime_error{ "Error: no value set on line " + std::to_string(lloc->begin.line) };
}

<PROGRAM_VALUE>\" {
	BEGIN(PROGRAM_QUOTED_VALUE);
	quotedVal = "";
}

<PROGRAM_QUOTED_VALUE>[^\"\\]+ {
	quotedVal += yytext;
}

<PROGRAM_QUOTED_VALUE>\\. {
	quotedVal += yytext;
}

<PROGRAM_QUOTED_VALUE>\" {
	BEGIN(PROGRAM_QUOTED_VALUE_END);
	yyval->build<std::string>(quotedVal);
	quotedVal = "";
	return token::VALUE;
}

<PROGRAM_QUOTED_VALUE_END>[^[:space:]]+$ {
	/* received more text at end of line, after quoted value */
	throw std::runtime_error{ "Unknown text after value: \'" + std::string{ yytext } + "\'" };
}

<PROGRAM_QUOTED_VALUE_END>\n {
	BEGIN(PROGRAM_SECTION);
	lloc->lines();
}

<PROGRAM_SECTION>license {
	BEGIN(PROGRAM_VALUE);
	return token::LICENSE;
}

<PROGRAM_SECTION>help {
	BEGIN(PROGRAM_VALUE);
	return token::HELP;
}

<USAGE_SECTION>{SL_SPACE}+ {
	/* ignored */
}

<USAGE_SECTION>\n {
	/* ignored */
	lloc->lines();
}

<USAGE_SECTION>{NAME} {
	if (yytext != programName) {
		BEGIN(USAGE_RULE_BEGIN);
		yyless(0);
	} else {
		BEGIN(USAGE_DETAILS_BEGIN);
	}
}

<USAGE_DETAILS_BEGIN>{SL_SPACE}+ {
	/* ignore */
	BEGIN(USAGE_DETAILS);
}

<USAGE_DETAILS_BEGIN>\n {
	BEGIN(USAGE_SECTION);
	lloc->lines();

	return token::USAGE_END;
}

<USAGE_DETAILS>[^[:space:]]+ {
	yyval->build<std::string>(yytext);

	return token::ARGUMENT;
}

<USAGE_DETAILS>{SL_SPACE}+ {
	/* ignore */
}

<USAGE_DETAILS>\n {
	BEGIN(USAGE_SECTION);

	lloc->lines();

	return token::USAGE_END;
}

<USAGE_RULE_BEGIN>{RULE_TOKEN} {
	BEGIN(USAGE_RULE);
	yyval->build<std::string>(yytext);
	return token::RULE_NAME;
}

<USAGE_RULE_BEGIN>{NAME} {
	throw std::runtime_error{ "Invalid rule name: " + std::string{ yytext } };
}

<USAGE_RULE>{SL_SPACE} {
	/* ignored */
}

<USAGE_RULE>= {
	return token::RULE_EQUALS;
}

<USAGE_RULE>--{LONGOPT} {
	yyval->build<std::string>(yytext + 2);
	return token::RULE_TOKEN;
}

<USAGE_RULE>{RULE_TOKEN} {
	yyval->build<std::string>(yytext);
	return token::RULE_TOKEN;
}

<USAGE_RULE>"|" {
	return token::RULE_OR;
}

<USAGE_RULE>\n {
	BEGIN(USAGE_SECTION);
	lloc->lines();
}


<ARGUMENTS_SECTION>{SL_SPACE}+ {
	/* ignored */
}

<ARGUMENTS_SECTION>\n {
	lloc->lines();
}

<ARGUMENTS_SECTION>--{LONGOPT} {
	yyval->build<std::string>(yytext + 2);
	return token::LONGOPT;
}

<ARGUMENTS_SECTION>, {
	BEGIN(ARGUMENTS_LONGOPT);
}

<ARGUMENTS_LONGOPT>-{SHORTOPT} {
	yyval->build<char>(*(yytext + 1));
	arguments = {};
	return token::SHORTOPT;
}

<ARGUMENTS_LONGOPT>, {
	BEGIN(ARGUMENTS_SHORTOPT);
}

<ARGUMENTS_LONGOPT,ARGUMENTS_SHORTOPT,ARGUMENTS_PARAMETERS,ARGUMENTS_DESCRIPTION_BEGIN>{SL_SPACE}+ {
	/* ignored */
}

<ARGUMENTS_SHORTOPT,ARGUMENTS_PARAMETERS>{PARAM} {
	BEGIN(ARGUMENTS_PARAMETERS);
	arguments.push_back({ yytext });
}

<ARGUMENTS_SHORTOPT,ARGUMENTS_PARAMETERS>, {
	BEGIN(ARGUMENTS_DESCRIPTION_BEGIN);
	if (!arguments.empty()) {
		yyval->build<std::vector<std::string>>(arguments);
		arguments = {};
		return token::PARAMETERS;
	}
}

<ARGUMENTS_DESCRIPTION_BEGIN>\" {
	BEGIN(ARGUMENTS_DESCRIPTION);
	quotedVal = "";
}

<ARGUMENTS_DESCRIPTION>[^\\\"]+ {
	quotedVal += yytext;
}

<ARGUMENTS_DESCRIPTION>\\. {
	quotedVal += yytext;
}

<ARGUMENTS_DESCRIPTION>\" {
	BEGIN(ARGUMENTS_DESCRIPTION_END);
	yyval->build<std::string>(quotedVal);
	quotedVal = "";
	return token::DESCRIPTION;
}

<ARGUMENTS_DESCRIPTION_END>\n {
	BEGIN(ARGUMENTS_SECTION);
	lloc->lines();
}

<*>. {
	std::cerr << "Scanning error in state: " << YY_START << std::endl;
	throw std::runtime_error{ "Invalid input: \'" + std::string{ yytext } + "\' on line " + std::to_string(lloc->begin.line) };
}

%%
