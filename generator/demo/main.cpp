{{=@@ @@=}}
#include "@@argspec@@ArgGrammarDriver.hpp"
#include <sstream>

using Result = @@argspec@@ArgGrammar::Driver::Result;

int main(int argc, char* argv[]) {
	@@argspec@@ArgGrammar::Driver driver{ argc, argv };
	auto res = driver.parse();

	if (res != Result::success) {
		if (res == Result::completedAction)
			return 0;

		return -1;
	}

	return 0;
}
