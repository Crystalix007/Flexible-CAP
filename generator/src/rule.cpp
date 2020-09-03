#include "rule.hpp"
#include "argument.hpp"

std::string RuleAlternation::cleanToken() const {
	return Argument::cleanToken(this->toStr());
}

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

std::string ArgumentRuleAlternation::cleanToken() const {
	return Argument::cleanToken(argument);
}
