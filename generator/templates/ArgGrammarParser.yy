{{=@@ @@=}}
%skeleton "lalr1.cc"
%require "3.2"
%debug
%defines
%define api.namespace {@@{argspec}@@ArgGrammar}
%define api.parser.class {Parser}
%define api.prefix {__yy}
%file-prefix "@@{argspec}@@ArgGrammarParser"

%code requires{
	namespace @@{argspec}@@ArgGrammar {
		class Driver;
		class Scanner;
	}

#ifndef __YY_NULLPTR
#	if defined __cplusplus && 201103L <= __cplusplus
#		define __YY_NULLPTR nullptr
#	else
#		define __YY_NULLPTR 0
#	endif
#endif
}

%parse-param { Scanner& scanner }
%parse-param { Driver& driver }

%code{
#include <string>
#include <map>
#include <utility>

#include "@@{argspec}@@ArgGrammarDriver.hpp"
#include "@@{argspec}@@ArgGrammarScanner.hpp"

#undef __yylex
#define __yylex scanner.__yylex
}

%define api.value.type variant
%define parse.assert

%token <std::string> POSITIONAL_ARGUMENT@@#argument_tokens@@
%token ARGUMENT_@@{clean_token}@@@@/argument_tokens@@

%start ARGUMENTS

%%

ARGUMENTS
	: ARGUMENT_help {
		std::cout
			<< "Usage:\n"
			<< "\n"@@#usage@@
			<< driver.getArgv()[0] << "@@#arguments@@@@#flag@@ @@{.}@@@@/flag@@@@/arguments@@@@#arguments@@@@#positional@@ <@@{.}@@>@@/positional@@@@/arguments@@\n"@@/usage@@
			<< driver.getArgv()[0] << " --help\n"
			<< driver.getArgv()[0] << " --version\n"
			<< driver.getArgv()[0] << " --license\n"
			<< "\n"
			<< "Arguments:\n"
			<< "\n"@@#argument_explanations@@
			<< "@@{.}@@\n"
			<< "\n"@@/argument_explanations@@@@#argument_tokens@@
			<< " @@#short_argument@@-@@{short_argument}@@,@@/short_argument@@@@^short_argument@@   @@/short_argument@@ --@@{argument}@@@@{parameter_align_spacing}@@@@#parameters@@ <@@{name}@@>@@/parameters@@ @@{explain_align_spacing}@@@@{usage}@@\n"@@/argument_tokens@@
			<< "\n"
			<< "@@{help_addendum}@@"@@/help_addendum@@
			<< std::endl;
		driver.addArg(@@{argspec}@@ArgGrammar::FlagArg::help);
		driver.setResult(Driver::Result::completedAction);
	}
	| ARGUMENT_version {
		std::cout << "@@{version}@@" << std::endl;
		driver.addArg(@@{argspec}@@ArgGrammar::FlagArg::version);
		driver.setResult(Driver::Result::completedAction);
	}
	| ARGUMENT_license {
		std::cout << "@@{license}@@" << std::endl;
		driver.addArg(@@{argspec}@@ArgGrammar::FlagArg::license);
		driver.setResult(Driver::Result::completedAction);
	}@@#usage@@
	|@@#arguments@@ @@#positional@@POSITIONAL@@/positional@@@@^positional@@ARGUMENT@@/positional@@_@@{clean_token}@@@@/arguments@@ {
		driver.setResult(Driver::Result::success);
	}@@/usage@@
	;@@#positional_arguments@@

POSITIONAL_@@{clean_token}@@
	: POSITIONAL_ARGUMENT {
		driver.addArg(@@{argspec}@@ArgGrammar::PositionalArg::@@{clean_token}@@, $1);
	}
	;@@/positional_arguments@@@@#usage_rules@@

ARGUMENT_@@{rule_name}@@
	:@@#options@@ ARGUMENT_@@{option}@@@@#has_next@@
	|@@/has_next@@@@/options@@
	;@@/usage_rules@@

%%

void @@{argspec}@@ArgGrammar::Parser::error(const std::string& err_message)
{
	std::cerr << "Error: " << err_message << '\n';
	driver.setResult(Driver::Result::failure);
}
