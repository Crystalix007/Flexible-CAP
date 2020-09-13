#pragma once

#include "driver.hpp"
#include "parse-tree.hpp"

namespace Grammar {
	class PrettyPrinter {
	public:
		PrettyPrinter(const ParseTree& parseTree);
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
		const ParseTree parseTree;
	};
} // namespace Grammarrules
