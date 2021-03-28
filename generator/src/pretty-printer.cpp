#include "pretty-printer.hpp"
#include <iterator>
#include <numeric>

namespace Grammar {
	PrettyPrinter::PrettyPrinter(const ParseTree& parseTree)
	    : parseTree{ parseTree } {}

	std::string PrettyPrinter::print() const {
		std::vector<std::vector<std::string>> grid{
			{ "program ", "" + parseTree.getProgramName() + "" },
			{ "version ", "\"" + parseTree.getVersion() + "\"" },
			{ "license ", "\"" + parseTree.getLicense() + "\"" },
			{ "help ", "\"" + parseTree.getHelp() + "\"" },
		};

		return "Program:\n"
		       "\n" +
		       align(grid) +
		       "\n"
		       "\n"
		       "Usage:\n"
		       "\n" +
		       getUsages() + "\n" + getRules() +
		       "\n"
		       "Arguments:\n"
		       "\n" +
		       getArguments();
	}

	std::string PrettyPrinter::getUsages() const {
		std::vector<std::vector<std::string>> grid{};

		for (const auto& usage : parseTree.getUsages()) {
			std::string arguments{};

			for (const auto& argument : usage.arguments) {
				if (const auto npu =
				        std::dynamic_pointer_cast<NonPositionalUsageArgument>(
				            argument)) {
					arguments += " " + npu->toStr();
				} else if (const auto pu =
				               std::dynamic_pointer_cast<PositionalUsageArgument>(
				                   argument)) {
					arguments += " " + pu->toStr();
				} else if (const auto du =
				               std::dynamic_pointer_cast<DerivedUsageArgument>(
				                   argument)) {
					arguments += " " + du->toStr();
				} else {
					/* Undefined Argument Handling: ignore. */
				}
			}

			grid.push_back({ parseTree.getProgramName(), arguments });
		}

		return align(grid) + "\n";
	}

	std::string PrettyPrinter::getRules() const {
		std::vector<std::vector<std::string>> grid{};

		for (const auto& [rule, definition] : parseTree.getRules()) {
			std::vector<std::string> ruleStrings{};
			std::transform(std::begin(definition), std::end(definition),
			               std::back_inserter(ruleStrings),
			               [](const auto& alt) { return alt->toStr(); });

			std::string defStr =
			    std::accumulate(std::begin(ruleStrings), std::end(ruleStrings),
			                    std::string{},
			                    [](const std::string& s1, const std::string& s2) {
				                    return s1.empty() ? s2 : s1 + " | " + s2;
			                    }) +
			    "\n";

			grid.push_back({ rule, " = ", defStr });
		}

		return align(grid);
	}

	std::string PrettyPrinter::getArguments() const {
		std::vector<std::vector<std::string>> grid{};

		for (const auto& argument : parseTree.getArguments()) {
			const std::string longopt{ "--" + argument->argument + ", " };
			std::string shortOpt{};
			std::string parameterStr{};

			if (argument->shortArgument.has_value()) {
				shortOpt = std::string{ "-" } + argument->shortArgument.value() + ", ";
			}

			if (const auto* paramArg =
			        dynamic_cast<ParameterArgument*>(argument.get())) {
				const auto& params = paramArg->parameters;
				parameterStr =
				    std::accumulate(std::begin(params), std::end(params), std::string{},
				                    [](const std::string& s1, const std::string& s2) {
					                    return s1.empty() ? s2 : s1 + " " + s2;
				                    }) +
				    ", ";
			}

			const std::string help{ "\"" + argument->usage + "\"" };

			grid.push_back({ longopt, shortOpt, parameterStr, help });
		}

		return align(grid);
	}

	std::string PrettyPrinter::align(std::vector<std::vector<std::string>> grid) {
		if (grid.size() == 0) {
			return "";
		}

		size_t rowSize = grid[0].size();

		// Check grid is square
		for (size_t i = 1; i < grid.size(); i++) {
			if (grid[i].size() != rowSize) {
				throw std::runtime_error{ "Row " + std::to_string(i) + " has size " +
					                        std::to_string(grid[i].size()) +
					                        " which is not the expected " +
					                        std::to_string(rowSize) + " elements" };
			}
		}

		for (size_t i = 0; i < rowSize - 1; i++) {
			size_t colWidth = std::accumulate(
			    std::begin(grid), std::end(grid), 0,
			    [i](const size_t maxWidth, const std::vector<std::string>& row) {
				    return std::max(maxWidth, row[i].size());
			    });

			for (size_t j = 0; j < grid.size(); j++) {
				grid[j][i] = fill(colWidth, grid[j][i], ' ');
			}
		}

		return combine(grid);
	}

	std::string
	PrettyPrinter::combine(const std::vector<std::vector<std::string>>& grid) {
		return std::accumulate(
		    std::begin(grid), std::end(grid), std::string{ "" },
		    [](const std::string s1, const std::vector<std::string>& row) {
			    std::string s2 =
			        std::accumulate(std::begin(row), std::end(row), std::string{ "" },
			                        [](const std::string s1, const std::string s2) {
				                        return s1.size() == 0 ? s2 : s1 + s2;
			                        });
			    return s1.size() == 0 ? s2 : s1 + "\n" + s2;
		    });
	}

	std::string PrettyPrinter::fill(size_t size, std::string base,
	                                char fillCharacter) {
		base.reserve(size);

		for (size_t i = base.size(); i < size; i++) {
			base += ' ';
		}

		return base;
	}
} // namespace Grammar
