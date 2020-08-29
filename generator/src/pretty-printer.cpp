#include "pretty-printer.hpp"
#include <iterator>
#include <numeric>

namespace Grammar {
	PrettyPrinter::PrettyPrinter(const Driver& driver)
	    : programName{ driver.getProgramName() }, version{ driver.getVersion() },
	      license{ driver.getLicense() }, help{ driver.getHelp() },
	      usages{ driver.getUsages() }, rules{ driver.getRules() }, arguments{
		      driver.getArguments()
	      } {}

	std::string PrettyPrinter::print() const {
		return "Program:\n"
		       "\n"
		       "program " + programName + "\n"
		       "version \"" + version + "\"\n"
		       "license \"" + license + "\"\n"
		       "help \"" + help + "\"\n"
		       "\n"
		       "Usage:\n"
		       "\n" +
		       getUsages() +
		       "\n" +
		       getRules() +
					 "\n"
					 "Arguments:\n"
					 "\n" +
					 getArguments();
	}

	std::string PrettyPrinter::getUsages() const {
		std::string res{};

		for (const auto& usage : usages) {
			res += programName;

			for (const auto& argument : usage.arguments) {
				res += " ";

				if (const auto npu =
				        std::dynamic_pointer_cast<NonPositionalUsageArgument>(
				            argument)) {
					res += "--" + npu->toStr();
				} else if (const auto pu =
					    std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
					res += pu->toStr();
				} else if (const auto du = std::dynamic_pointer_cast<DerivedUsageArgument>(argument)) {
					res += du->toStr();
				} else {
					/* Undefined Argument Handling: ignore. */
				}
			}

			res += "\n";
		}

		return res;
	}

	std::string PrettyPrinter::getRules() const {
		std::string res{};

		for (const auto& [rule, definition] : rules) {
			res += rule + " = ";

			std::vector<std::string> ruleStrings{};
			std::transform(std::begin(definition), std::end(definition), std::back_inserter(ruleStrings), [](const auto& alt){
					return alt->toStr();
			});

			res += std::accumulate(
			    std::begin(ruleStrings), std::end(ruleStrings), std::string{},
			    [](const std::string& s1, const std::string& s2) {
			      return s1.empty() ? s2 : s1 + " | " + s2;
			    }) +
			"\n";
		}

		return res;
	}

	std::string PrettyPrinter::getArguments() const {
		std::string res{};

		for (const auto& argument : arguments) {
			res += "--" + argument->argument;

			if (argument->shortArgument.has_value()) {
				res += std::string{ ", -" } + argument->shortArgument.value();
			}

			if (const auto* paramArg =
			        dynamic_cast<ParameterArgument*>(argument.get())) {
				res += ", ";
				const auto& params = paramArg->parameters;
				res +=
				    std::accumulate(std::begin(params), std::end(params), std::string{},
				                    [](const std::string& s1, const std::string& s2) {
					                    return s1.empty() ? s2 : s1 + " " + s2;
				                    });
			}

			res += ", \"" + argument->usage + "\"\n";
		}

		return res;
	}
} // namespace Grammar
