#pragma once

#include <string>

class RuleAlternation {
public:
	RuleAlternation() = default;
	virtual ~RuleAlternation() = default;

	virtual std::string toStr() const = 0;
};

class DerivedRuleAlternation : public RuleAlternation {
public:
	DerivedRuleAlternation(const std::string& rule);
	virtual ~DerivedRuleAlternation() = default;

	virtual std::string toStr() const override;

protected:
	std::string rule;
};

class ArgumentRuleAlternation : public RuleAlternation {
public:
	ArgumentRuleAlternation(const std::string& argument);
	virtual ~ArgumentRuleAlternation() = default;

	virtual std::string toStr() const override;

protected:
	std::string argument;
};
