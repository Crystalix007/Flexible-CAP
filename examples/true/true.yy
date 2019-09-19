%skeleton "lalr1.cc"
%require "3.2"
%debug
%defines
%define api.namespace {trueArgGrammar}
%define api.parser.class {Parser}

%code requires{
	namespace trueArgGrammar {
		class Driver;
		class Scanner;
	}

#ifndef YY_NULLPTR
#	if defined __cplusplus && 201103L <= __cplusplus
#		define YY_NULLPTR nullptr
#	else
#		define YY_NULLPTR 0
#	endif
#endif
}

%parse-param { Scanner& scanner }
%parse-param { Driver& driver }

%code{
#include <string>
#include <map>
#include <utility>

#include "trueArgGrammarDriver.hpp"
#include "trueArgGrammarScanner.hpp"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token ARGUMENT_HELP_LONG
%token ARGUMENT_VERSION_LONG

%start ARGUMENTS

%%

ARGUMENTS
	: %empty
	| ARGUMENT_HELP {
		std::cout
			<< "Usage:\n"
			<< "\n"
			<< driver.getArgv()[0] << "\n"
			<< driver.getArgv()[0] << " <option>" << "\n"
			<< "\n"
			<< "Arguments:\n"
			<< "\n"
			<< "option = --help | --version" << "\n"
			<< "\n"
			<< "--help    display this help and exit\n"
			<< "--version output version information and exit\n"
			<< "\n"
			<< "NOTE: your shell may have its own version of true, which usually supersedes the version described here."
			<< std::endl;
		driver.addArg(trueArgGrammar::SingleArg::help, {});
		driver.setResult(Driver::Result::completedAction);
	}
	| ARGUMENT_VERSION {
		std::cout
			<< "true (GNU coreutils) 8.3.1"
			<< std::endl;
		driver.addArg(trueArgGrammar::SingleArg::version, {});
		driver.setResult(Driver::Result::completedAction);
	}
	;

ARGUMENT_HELP
	: ARGUMENT_HELP_LONG
	;

ARGUMENT_VERSION
	: ARGUMENT_VERSION_LONG
	;

%%

void trueArgGrammar::Parser::error(const std::string& err_message)
{
	std::cerr << "Error: " << err_message << '\n';
}
