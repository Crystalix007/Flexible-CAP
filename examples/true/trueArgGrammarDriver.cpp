#include <cassert>
#include <fstream>

#include "trueArgGrammarDriver.hpp"
#include "trueArgGrammarScanner.hpp"

namespace trueArgGrammar {
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

	void Driver::addArg(const SingleArg arg, std::string value) {
		singleArguments.insert_or_assign(arg, value);
	}

	std::optional<std::string> Driver::getArg(const SingleArg arg) const {
		try {
			return std::make_optional(singleArguments.at(arg));
		} catch (const std::out_of_range& ignored) {
			return std::nullopt;
		}
	}

	void Driver::setResult(Driver::Result result) {
		this->result = result;
	}

	const std::map<SingleArg, std::string>& Driver::getSingleArgs() const {
		return singleArguments;
	}

	char** Driver::getArgv() {
		return argv;
	}
} // namespace trueArgGrammar
