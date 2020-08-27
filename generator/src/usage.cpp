#include "usage.hpp"
#include "argument.hpp"

std::string UsageArgument::cleanToken() const {
	return Argument::cleanToken(this->toStr());
}

NonPositionalUsageArgument::NonPositionalUsageArgument(const std::string& argument)
    : arg{ argument } {}

std::string NonPositionalUsageArgument::toStr() const {
	return this->arg;
}

PositionalUsageArgument::PositionalUsageArgument(const std::string& argument) : arg{ argument } {}

std::string PositionalUsageArgument::toStr() const {
	return this->arg;
}

DerivedUsageArgument::DerivedUsageArgument(const std::string& rule) : rule{ rule } {}

std::string DerivedUsageArgument::toStr() const {
	return this->rule;
}
