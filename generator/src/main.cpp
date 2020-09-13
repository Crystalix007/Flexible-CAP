#include <filesystem>
#include <fstream>
#include <iostream>
#include <mstch/mstch.hpp>

#include "driver.hpp"
#include "pretty-printer.hpp"
#include "templates.hpp"
#include "semantic-checking.hpp"

#ifdef COMMANDLINE_INTERFACE
# include "debugJSON.hpp"
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
	fcapArgGrammar::Driver cliDriver{ argc, argv };
	auto res = cliDriver.parse();

	if (res != fcapArgGrammar::Driver::Result::success) {
		if (res == fcapArgGrammar::Driver::Result::completedAction)
			return 0;

		return -1;
	}

	const auto fileArg = cliDriver.getArg(fcapArgGrammar::PositionalArg::file);

	if (!fileArg) {
		std::cerr << "You must specify a file parameter." << std::endl;

		return 1;
	}

	std::ifstream specFile{ fileArg.value() };
#endif

	driver.parse(specFile);

#ifdef COMMANDLINE_INTERFACE
	/*
	 * Pretty print.
	 */


	if (cliDriver.getArg(fcapArgGrammar::FlagArg::pretty_print)) {
		Grammar::PrettyPrinter pp{ driver.getParseTree() };
		std::cout << pp.print() << std::endl;
	} else {
#endif

	/*
	 * Generate the output files
	 *
	 */

	auto context = driver.getContext();
	std::filesystem::path outputPath{ std::filesystem::current_path() };

#ifdef COMMANDLINE_INTERFACE
	if (const auto outputFolder = cliDriver.getArg(fcapArgGrammar::ParamArg::generated_folder)) {
		std::filesystem::path p{ outputFolder.value()[0] };

		if (!std::filesystem::is_directory(p)) {
			std::cerr << "Cannot output to '" << p.generic_string() << "' as this is not a valid directory" << std::endl;
			return 3;
		}

		outputPath = p;
	}

	if (cliDriver.getArg(fcapArgGrammar::FlagArg::debug)) {
		std::clog << DebugPrinter::printJSON(context) << std::endl;
	}
#endif

	if (!checkSemantics(driver.getParseTree())) {
		return 4;
	}

	for (const auto& templateFile : templateFiles) {
		std::string strTemplate{ templateFile.contents };
		const auto symbolFileName = symbolNameToOutputFile(templateFile.symbolName);
		const std::filesystem::path templPath = outputPath / (driver.getSafeName() + symbolFileName);
		std::ofstream outputFile{ templPath };

		outputFile << mstch::render(strTemplate, context);
		outputFile.close();
	}

#ifdef COMMANDLINE_INTERFACE
	}
#endif

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
