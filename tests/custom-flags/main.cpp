#include "customflagsArgGrammarDriver.hpp"
#include <sstream>

using Result = customflagsArgGrammar::Driver::Result;

int main(int argc, char* argv[]) {
	customflagsArgGrammar::Driver driver{ argc, argv };
	auto res = driver.parse();

	if (res != Result::success) {
		if (res == Result::completedAction) {
			return 0;
		}

		return -1;
	}

	std::cout << "program";

	if (auto hasHelp = driver.getArg(customflagsArgGrammar::FlagArg::help)) {
		std::cout << " --help";
	}

	if (auto hasVersion = driver.getArg(customflagsArgGrammar::FlagArg::version)) {
		std::cout << " --version";
	}

	if (auto hasFlag1 = driver.getArg(customflagsArgGrammar::FlagArg::flag1)) {
		std::cout << " --flag1";
	}

	if (auto hasFlag2 = driver.getArg(customflagsArgGrammar::ParamArg::flag2)) {
		std::cout << " --flag2 (" << hasFlag2.value()[0] << ")";
	}

	if (auto hasFlag3 = driver.getArg(customflagsArgGrammar::ParamArg::flag3)) {
		std::cout << " --flag3 (" << hasFlag3.value()[0] << ", " << hasFlag3.value()[1] << ")";
	}

	if (auto hasFlag4 = driver.getArg(customflagsArgGrammar::ParamArg::flag4)) {
		std::cout << " --flag4 (" << hasFlag4.value()[0] << ", " << hasFlag4.value()[1] << ")";
	}

	if (auto hasFlag5 = driver.getArg(customflagsArgGrammar::ParamArg::flag5)) {
		std::cout << " --flag5 (" << hasFlag5.value()[0] << ", " << hasFlag5.value()[1] << ", " << hasFlag5.value()[2] << ")";
	}

	std::cout << std::endl;

	return 0;
}
