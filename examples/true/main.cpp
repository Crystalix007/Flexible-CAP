#include "trueArgGrammarDriver.hpp"
#include <sstream>

using Result = trueArgGrammar::Driver::Result;

int main(int argc, char* argv[]) {
	trueArgGrammar::Driver driver{ argc, argv };
	auto res = driver.parse();

	if (res != Result::success) {
		if (res == Result::completedAction)
			return 0;

		return -1;
	}

	std::cout << "true";

	if (auto hasHelp = driver.getArg(trueArgGrammar::SingleArg::help)) {
		std::cout << " --help";
	} else if (auto hasVersion = driver.getArg(trueArgGrammar::SingleArg::version)) {
		std::cout << " --version";
	}

	std::cout << std::endl;

	return 0;
}
