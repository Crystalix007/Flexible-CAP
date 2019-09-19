#pragma once

#include "trueArgGrammarParser.tab.hh"
#include <iostream>
#include <map>
#include <optional>
#include <string>

namespace trueArgGrammar {
	enum class SingleArg { help, version };

	class Scanner;

	class Driver {
	public:
		enum class Result { success, completedAction, wrongArgument };

		Driver(int argc, char* argv[]);
		virtual ~Driver();

		Result parse();

		void addArg(const SingleArg flag, const std::string value);
		std::optional<std::string> getArg(const SingleArg flag) const;
		void setResult(Result result);

		const std::map<SingleArg, std::string>& getSingleArgs() const;

		std::ostream& print(std::ostream& oss);

		char** getArgv();

	private:
		int argc;
		char** argv;

		Result result = Result::success;

		void parseHelper(std::istream& iss);

		trueArgGrammar::Parser* parser = nullptr;
		trueArgGrammar::Scanner* scanner = nullptr;

		/*
		 * specialised data store
		 */

		std::map<SingleArg, std::string> singleArguments;
	};
} // namespace trueArgGrammar
