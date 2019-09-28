#include "src/dcArgGrammarDriver.hpp"
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

	return 0;
}
