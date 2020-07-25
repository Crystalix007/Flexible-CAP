#include "dcArgGrammarDriver.hpp"
#include <sstream>

using Result = dcArgGrammar::Driver::Result;

int main(int argc, char* argv[]) {
	dcArgGrammar::Driver driver{ argc, argv };
	auto res = driver.parse();

	if (res != Result::success) {
		if (res == Result::completedAction)
			return 0;

		return -1;
	}

	std::cout << "dc";

	if (auto hasHelp = driver.getArg(dcArgGrammar::FlagArg::help)) {
		std::cout << " --help";
	} else if (auto hasVersion = driver.getArg(dcArgGrammar::FlagArg::version)) {
		std::cout << " --version";
	}

	std::cout << std::endl;

	return 0;
}
