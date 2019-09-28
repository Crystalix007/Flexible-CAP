{{=@@ @@=}}
#pragma once

#include "@@{argspec}@@ArgGrammarParser.tab.hh"
#include <iostream>
#include <set>@@#any_parameters@@
#include <map>
#include <optional>@@/any_parameters@@
#include <string>

namespace @@{argspec}@@ArgGrammar {
	enum class FlagArg { @@#argument_tokens@@@@^has_parameters@@@@{clean_token}@@, @@/has_parameters@@@@/argument_tokens@@ };@@#any_parameters@@
	enum class ParamArg { @@#argument_tokens@@@@#has_parameters@@@@{clean_token}@@, @@/has_parameters@@@@/argument_tokens@@ };@@/any_parameters@@

	class Scanner;

	class Driver {
	public:
		enum class Result { success, completedAction, wrongArgument };

		Driver(int argc, char* argv[]);
		virtual ~Driver();

		Result parse();

		void addArg(const FlagArg flag);@@#any_parameters@@
		void addArg(const ParamArg flag, const std::vector<std::string> arguments);@@/any_parameters@@

		bool getArg(const FlagArg flag) const;@@#any_parameters@@
		std::optional<std::vector<std::string>> getArg(const ParamArg flag) const;@@/any_parameters@@
		void setResult(Result result);

		const std::set<FlagArg>& getFlagArgs() const;@@#any_parameters@@
		const std::map<ParamArg, std::vector<std::string>>& getParamArgs() const;@@/any_parameters@@

		std::ostream& print(std::ostream& oss);

		char** getArgv();

	private:
		int argc;
		char** argv;

		Result result = Result::success;

		void parseHelper(std::istream& iss);

		@@{argspec}@@ArgGrammar::Parser* parser = nullptr;
		@@{argspec}@@ArgGrammar::Scanner* scanner = nullptr;

		/*
		 * specialised data store
		 */

		std::set<FlagArg> flagArguments;@@#any_parameters@@
		std::map<ParamArg, std::vector<std::string>> paramArguments;@@/any_parameters@@
	};
} // namespace trueArgGrammar
