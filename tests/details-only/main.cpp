#include "programnameArgGrammarDriver.hpp"
#include <sstream>

using Result = programnameArgGrammar::Driver::Result;

int main(int argc, char* argv[]) {
	programnameArgGrammar::Driver driver{ argc, argv };
	auto res = driver.parse();

	if (res != Result::success) {
		if (res == Result::completedAction)
			return 0;

		return -1;
	}

	std::cout << "program";

	if (auto hasHelp = driver.getArg(programnameArgGrammar::FlagArg::help)) {
		std::cout << " --help";
	} else if (auto hasVersion = driver.getArg(programnameArgGrammar::FlagArg::version)) {
		std::cout << " --version";
	}

	std::cout << std::endl;

	return 0;
}
