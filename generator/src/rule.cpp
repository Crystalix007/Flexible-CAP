#include "rule.hpp"

DerivedRuleAlternation::DerivedRuleAlternation(const std::string& rule)
    : rule{ rule } {}

std::string DerivedRuleAlternation::toStr() const {
	return rule;
}

ArgumentRuleAlternation::ArgumentRuleAlternation(const std::string& argument)
    : argument{ argument } {}

std::string ArgumentRuleAlternation::toStr() const {
	return "--" + argument;
}
