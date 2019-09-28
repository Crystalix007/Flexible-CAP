{{=@@ @@=}}
#include "@@{argspec}@@ArgGrammarScanner.hpp"
#include "@@{argspec}@@ArgGrammarDriver.hpp"

namespace @@{argspec}@@ArgGrammar {
	Scanner::Scanner(int argc, char* argv[], Driver& driver)
	    : yyFlexLexer{}, argc{ argc }, argv{ argv }, argi{ 1 }, streamInput{}, resetVal{}, driver{
		      driver
	      } {
		if (argi < argc)
			streamInput << argv[argi];

		switch_streams(&streamInput);
	}

	int Scanner::yywrap() {
		++argi;
		bool more = (argi < argc);

		if (more) {
			streamInput << argv[argi];
			resetOnWrap();
		}

		return more ? 0 : 1;
	}

	void Scanner::setResult(Scanner::Result result) {
		driver.setResult(result);
	}
} // namespace @@{argspec}@@ArgGrammar
