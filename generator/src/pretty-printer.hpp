#pragma once

#include "driver.hpp"

namespace Grammar {
	class PrettyPrinter {
	public:
		PrettyPrinter(const Driver& driver);
		virtual ~PrettyPrinter() = default;

		std::string print() const;

		std::string getUsages() const;
		std::string getRules() const;
		std::string getArguments() const;

	protected:
		const std::string programName;
		const std::string version;
		const std::string license;
		const std::string help;

		std::vector<Usage> usages;
		std::map<std::string, std::vector<std::string>> rules;
		std::set<std::shared_ptr<Argument>, ArgumentComparator> arguments;
	};
} // namespace Grammarrules
