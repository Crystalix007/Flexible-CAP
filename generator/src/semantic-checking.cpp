#include "semantic-checking.hpp"
#include <iostream>
#include <optional>

namespace Grammar {
	bool checkSemantics(const ParseTree& parseTree) {
		return checkAllRulesDefined(parseTree) && checkNoCircularRules(parseTree);
	}

	bool checkAllRulesDefined(const ParseTree& parseTree) {
		const auto& rules = parseTree.getRules();
		std::set<std::string> definedRules{};

		for ([[maybe_unused]] const auto& [key, _] : rules) {
			definedRules.insert(key);
		}

		for ([[maybe_unused]] const auto& [name, rule] : rules) {
			for (const auto& alternation : rule) {
				if (const auto derivedAlternation =
				        std::dynamic_pointer_cast<DerivedRuleAlternation>(
				            alternation)) {
					/*
					 * Check if this derived rule doesn't have a definition.
					 */
					if (definedRules.find(derivedAlternation->toStr()) ==
					    definedRules.end()) {
						std::cerr << "Definition of " << name << " includes derived rule "
						          << derivedAlternation->toStr()
						          << " which has not been defined.\n";
						return false;
					}
				} else if (const auto argumentAlternation =
				               std::dynamic_pointer_cast<ArgumentRuleAlternation>(
				                   alternation)) {
					const auto& arguments = parseTree.getArguments();

					/*
					 * Check if this rule doesn't have a specification.
					 */
					if (std::find_if(arguments.begin(), arguments.end(),
					                 [argumentAlternation](const auto& argument) {
						                 return argument->cleanToken() ==
						                        argumentAlternation->cleanToken();
					                 }) == arguments.end()) {
						std::cerr << "Definition of " << name << " includes argument "
						          << argumentAlternation->toStr()
						          << " which has not been defined\n";
						return false;
					}
				}
			}
		}

		return true;
	}

	using RuleMap = std::map<std::string, std::set<std::string>>;
	std::optional<std::string> containsLoop(const std::string& baseRule,
	                                        RuleMap rules,
	                                        const std::string currentRule);

	bool checkNoCircularRules(const ParseTree& parseTree) {
		const auto& rules = parseTree.getRules();
		RuleMap childRules{};

		for (const auto& [name, rule] : rules) {
			childRules.insert({ name, {} });

			for (const auto& alternation : rule) {
				if (const auto derivedAlternation =
				        std::dynamic_pointer_cast<DerivedRuleAlternation>(
				            alternation)) {
					/*
					 * Add this derived rule to the set included in this rule's child
					 * rules.
					 */
					childRules[name].insert(derivedAlternation->toStr());
				}
			}
		}

		for (const auto& [key, _] : childRules) {
			if (const auto offendingChildRule = containsLoop(key, childRules, key)) {
				std::cerr << "Definition for rule " << key
				          << " has cyclical dependency on "
				          << offendingChildRule.value() << "\n";
				return false;
			}
		}

		return true;
	}

	std::optional<std::string> containsLoop(const std::string& baseRule,
	                                        RuleMap rules,
	                                        const std::string currentRule) {
		/*
		 * If we've removed this rule.
		 */
		if (rules.find(currentRule) == rules.end()) {
			return std::nullopt;
		}

		const auto children = rules[currentRule];

		/*
		 * If a child rule depends upon the base rule, we have a loop.
		 */
		if (children.find(baseRule) != children.end()) {
			return currentRule;
		}

		/*
		 * Don't search this rule's children again.
		 */
		rules.erase(currentRule);

		for (const auto& child : children) {
			if (const auto& offendingChild = containsLoop(baseRule, rules, child)) {
				return baseRule == currentRule ? offendingChild : currentRule;
			}
		}

		return std::nullopt;
	}
}; // namespace Grammar
