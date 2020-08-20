#pragma once
#include <memory>
#include <string>
#include <vector>

class UsageArgument {
public:
	virtual ~UsageArgument() = default;

	virtual std::string toStr() const = 0;
	virtual std::string cleanToken() const;
};

class NonPositionalUsageArgument : public UsageArgument {
public:
	NonPositionalUsageArgument(const std::string& argument);
	virtual ~NonPositionalUsageArgument() = default;

	virtual std::string toStr() const override;

protected:
	std::string arg;
};

class PositionalUsageArgument : public UsageArgument {
public:
	PositionalUsageArgument(const std::string& argument);
	virtual ~PositionalUsageArgument() = default;

	virtual std::string toStr() const override;

protected:
	std::string arg;
};

struct Usage {
  std::vector<std::shared_ptr<UsageArgument>> arguments;
};
