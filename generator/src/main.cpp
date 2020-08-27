#include <fstream>
#include <iostream>
#include <mstch/mstch.hpp>

#include "driver.hpp"
#include "pretty-printer.hpp"
#include "templates.hpp"

#ifdef COMMANDLINE_INTERFACE
#	include "fcapArgGrammarDriver.hpp"
#endif

std::string symbolNameToOutputFile(const std::string& symbolName);

int main(int argc, char* argv[]) {
#ifndef COMMANDLINE_INTERFACE
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <filename-to-parse>\n";
		return 1;
	}
#endif

	Grammar::Driver driver{};

#ifndef COMMANDLINE_INTERFACE
	std::ifstream specFile{ argv[1] };

	if (!specFile) {
		std::cerr << "Failed to open file '" << argv[1]
		          << "' for writing.\n Make sure this file exists and is readable.\n";
		return 2;
	}
#else
	fcapArgGrammar::Driver driver{ argc, argv };
	auto res = driver.parse();

	if (res != Result::success) {
		if (res == Result::completedAction)
			return 0;

		return -1;
	}
#endif

	driver.parse(specFile);

	/*
	 * Generate the output files
	 *

	const auto context = driver.getContext();

	for (const auto& templateFile : templateFiles) {
		std::string strTemplate{ templateFile.contents };
		std::ofstream outputFile{ driver.getSafeName() +
			                        symbolNameToOutputFile(templateFile.symbolName) };

		outputFile << mstch::render(strTemplate, context);
		outputFile.close();
	}
	*/

	/*
	 * Pretty print.
	 *
	 */
	Grammar::PrettyPrinter pp{ driver };
	std::cout << pp.print() << std::endl;

	return 0;
}

std::string symbolNameToOutputFile(const std::string& symbolName) {
	std::string outputFileName(symbolName.size(), static_cast<char>(NULL));
	std::transform(symbolName.begin(), symbolName.end(), outputFileName.begin(), [](char letter) {
		if (letter == '_')
			return '.';
		return letter;
	});

	return outputFileName;
}
