{{=@@ @@=}}
#include <cassert>
#include <fstream>

#include "@@{argspec}@@ArgGrammarDriver.hpp"
#include "@@{argspec}@@ArgGrammarScanner.hpp"

namespace @@{argspec}@@ArgGrammar {
	Driver::Driver(int argc, char* argv[]) : argc{ argc }, argv{ argv }, scanner{}, parser{} {
		assert(argc > 0 && "Arguments must include program invocation");
	}

	Driver::~Driver() {
		delete scanner;
		delete parser;
	}

	Driver::Result Driver::parse() {
		if (scanner)
			delete scanner;

		try {
			scanner = new Scanner(argc, argv, *this);
		} catch (const std::bad_alloc& ba) {
			std::cerr << "Failed to allocate scanner: \"" << ba.what() << "\". Exiting!\n";
			exit(EXIT_FAILURE);
		}

		if (parser)
			delete parser;
		try {
			parser = new Parser(*scanner, *this);
		} catch (const std::bad_alloc& ba) {
			std::cerr << "Failed to allocate parser: \"" << ba.what() << "\". Exiting!\n";
			exit(EXIT_FAILURE);
		}

		if (parser->parse() != 0) {
			std::cerr << "Parsing failure!\n";
		}

		return result;
	}

	void Driver::addArg(const FlagArg flag) {
		flagArguments.insert(flag);
	}@@#any_parameters@@

	void Driver::addArg(const ParamArg flag, const std::vector<std::string>& parameters) {
		paramArguments.insert_or_assign(flag, parameters);
	}@@/any_parameters@@@@#any_positional_arguments@@

	void Driver::addArg(const PositionalArg flag, const std::string& value) {
		positionalArguments.insert_or_assign(flag, value);
	}@@/any_positional_arguments@@

	bool Driver::getArg(const FlagArg flag) const {
		return (flagArguments.find(flag) != flagArguments.end());
	}@@#any_parameters@@

	std::optional<std::vector<std::string>> Driver::getArg(const ParamArg flag) const {
		try {
			return std::make_optional(paramArguments.at(flag));
		} catch (const std::out_of_range& ignored) {
			return std::nullopt;
		}
	}@@/any_parameters@@@@#any_positional_arguments@@

	std::optional<std::string> Driver::getArg(const PositionalArg flag) const {
		try {
			return std::make_optional(positionalArguments.at(flag));
		} catch (const std::out_of_range& ignored) {
			return std::nullopt;
		}
	}@@/any_positional_arguments@@

	void Driver::setResult(Driver::Result result) {
		this->result = std::max(result, this->result);
	}

	const std::set<FlagArg>& Driver::getFlagArgs() const {
		return flagArguments;
	}@@#any_parameters@@

	const std::map<ParamArg, std::vector<std::string>>& Driver::getParamArgs() const {
		return paramArguments;
	}@@/any_parameters@@

	char** Driver::getArgv() {
		return argv;
	}
} // namespace trueArgGrammar
