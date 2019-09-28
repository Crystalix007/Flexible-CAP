/* Parses an argument specification */
%skeleton "lalr1.cc"
%require "3.2"
%debug
%defines
%define api.namespace {Grammar}
%define api.parser.class {Parser}

%code requires{
#	include <optional>

	namespace Grammar {
		class Driver;
		class Scanner;
	}

#	ifndef YY_NULLPTR
#		if defined __cplusplus && 201103L <= __cplusplus
#			define YY_NULLPTR nullptr
#		else
#			define YY_NULLPTR 0
#		endif
#	endif
}

%parse-param { Scanner& scanner  }
%parse-param { Driver& driver }

%code{
#	include "driver.hpp"
#	include <string>
#	include <vector>
#	include <utility>
#	include <memory>


#	undef yylex
#	define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert
%locations
%define api.location.file none

%token                            PROGRAM
%token                            VERSION
%token                            LICENSE
%token                            HELP
%token <std::string>              VALUE
%token <std::string>              USAGE
%token <std::string>              LONGOPT
%token <char>                     SHORTOPT
%token <std::vector<std::string>> PARAMETERS
%token <std::string>              DESCRIPTION
%token <std::string>              RULE_NAME
%token                            RULE_EQUALS
%token                            RULE_OR
%token <std::string>              RULE_TOKEN

%type <std::optional<char>>                     OPTIONAL_SHORTOPT
%type <std::optional<std::vector<std::string>>> OPTIONAL_PARAMETERS
%type <std::vector<std::string>>                RULE_OPTIONS

%start ARGSPEC

%%

ARGSPEC
	: PROGRAM_DETAILS
	  USAGE_DETAILS
	  ARGUMENTS_DETAILS
	;

PROGRAM_DETAILS
	: PROGRAM VALUE
	  VERSION VALUE
	  LICENSE VALUE
	  OPTIONAL_HELP {
	  	driver.setProgramName($2);
	  	driver.setVersion($4);
	  	driver.setLicense($6);
	}
	;

OPTIONAL_HELP
	: %empty
	| HELP VALUE {
		driver.setHelpAddendum($2);
	}
	;

USAGE_DETAILS
	: USAGE_DETAIL
	| USAGE_DETAILS USAGE_DETAIL
	;

USAGE_DETAIL
	: USAGE {
		if ($1.empty())
			driver.addUsage(Usage{ {}, {} });
		else
			driver.addUsage(Usage{ { $1 }, {} });
	}
	| RULE_NAME RULE_EQUALS RULE_OPTIONS {
		driver.addRule($1, $3);
	}
	;

RULE_OPTIONS
	: RULE_TOKEN {
		$$ = std::vector{ $1 };
	}
	| RULE_OPTIONS RULE_OR RULE_TOKEN {
		$$ = $1;
		$$.push_back($3);
	}
	;

ARGUMENTS_DETAILS
	: %empty
	| ARGUMENTS_DETAILS ARGUMENT_DETAILS
	;

ARGUMENT_DETAILS
	: LONGOPT
	  OPTIONAL_SHORTOPT OPTIONAL_PARAMETERS
	  DESCRIPTION {
	  	if ($1 == "help" && $3) {
			std::cerr << "--help cannot take parameters\n";
			throw std::runtime_error{ "Invalid rule for --help" };
		} else if ($1 == "version" && $3) {
			std::cerr << "--version cannot take parameters\n";
			throw std::runtime_error{ "Invalid rule for --version" };
		} else if ($1 == "license" && $3) {
			std::cerr << "--license cannot take parameters\n";
			throw std::runtime_error{ "Invalid rule for --license" };
		}

		if ($3) /* if parameters exist */
			driver.addArg(std::make_unique<ParameterArgument>($1, $2, *$3, $4));
		else
			driver.addArg(std::make_unique<FlagArgument>($1, $2, $4));
	  }
	;

OPTIONAL_SHORTOPT
	: %empty {
		$$ = std::nullopt;
	}
	| SHORTOPT {
		$$ = $1;
	}
	;

OPTIONAL_PARAMETERS
	: %empty {
		$$ = std::nullopt;
	}
	| PARAMETERS {
		$$ = $1;
	}
	;

%%

void Grammar::Parser::error(const location_type& loc, const std::string& err_message)
{
	std::cerr << "Error: \'" << err_message << "\' at " << loc << '\n';
}
