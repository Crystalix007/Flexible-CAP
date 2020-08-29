#pragma once

#include "driver.hpp"

namespace Grammar {
	class PrettyPrinter {
	public:
		PrettyPrinter(const Driver& driver);
		virtual ~PrettyPrinter() = default;

		std::string print() const;

	protected:
		std::string getUsages() const;
		std::string getRules() const;
		std::string getArguments() const;

	protected:
		static std::string align(std::vector<std::vector<std::string>> grid);

	private:
		static std::string combine(const std::vector<std::vector<std::string>> &grid);
		static std::string fill(size_t size, std::string base, char fillCharacter);

	protected:
		const std::string programName;
		const std::string version;
		const std::string license;
		const std::string help;

		std::vector<Usage> usages;
		std::map<std::string, std::vector<std::shared_ptr<RuleAlternation>>> rules;
		std::set<std::shared_ptr<Argument>, ArgumentComparator> arguments;
	};
} // namespace Grammarrules
