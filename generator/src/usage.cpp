#include "usage.hpp"
#include "argument.hpp"

std::string UsageArgument::cleanToken() const {
	return Argument::cleanToken(this->toStr());
}

bool UsageArgument::operator<(const UsageArgument& other) const {
	return this->cleanToken() < other.cleanToken();
}

bool UsageArgument::operator==(const UsageArgument& other) const {
	return this->cleanToken() == other.cleanToken();
}

bool UsageArgument::operator>(const UsageArgument& other) const {
	return this->cleanToken() > other.cleanToken();
}

NonPositionalUsageArgument::NonPositionalUsageArgument(const std::string& argument)
    : arg{ argument } {}

std::string NonPositionalUsageArgument::toStr() const {
	return "--" + this->arg;
}

std::string NonPositionalUsageArgument::cleanToken() const {
	return Argument::cleanToken(this->arg);
}

PositionalUsageArgument::PositionalUsageArgument(const std::string& argument) : arg{ argument } {}

std::string PositionalUsageArgument::toStr() const {
	return this->arg;
}

DerivedUsageArgument::DerivedUsageArgument(const std::string& rule) : rule{ rule } {}

std::string DerivedUsageArgument::toStr() const {
	return this->rule;
}

bool UsageComparator::operator()(const std::shared_ptr<UsageArgument>& left,
	                const std::shared_ptr<UsageArgument>& right) const {
	return left->cleanToken() < right->cleanToken();
}
