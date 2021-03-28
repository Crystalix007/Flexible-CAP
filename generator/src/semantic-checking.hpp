#pragma once
#include "parse-tree.hpp"

namespace Grammar {
	/*
	 * Checks the semantics of the definition.
	 *
	 * Returns whether the semantics were valid.
	 */
	bool checkSemantics(const ParseTree& parseTree);

	/*
	 * Individual semantic checks.
	 *
	 * Not a public interface.
	 * May change at any time.
	 */

	bool checkAllRulesDefined(const ParseTree& parseTree);
	bool checkNoCircularRules(const ParseTree& parseTree);
}; // namespace Grammar
