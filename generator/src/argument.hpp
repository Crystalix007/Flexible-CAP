#pragma once
#include <mstch/mstch.hpp>
#include <optional>
#include <string>

class Argument {
public:
	Argument(std::string argument, std::optional<char> shortArgument, std::string usage);
	virtual ~Argument() = 0;

	virtual mstch::map render() const = 0;
	static std::string cleanToken(const std::string& longToken);
	std::string cleanToken() const;
	virtual bool hasParameters() const = 0;
	virtual size_t argStrLength() const;
	virtual size_t paramStrLength() const = 0;

	bool operator==(const Argument& other) const;
	bool operator<(const Argument& other) const;
	bool operator>(const Argument& other) const;

public:
	std::string argument, usage;
	std::optional<char> shortArgument;
};

class FlagArgument : public Argument {
public:
	FlagArgument(std::string argument, std::optional<char> shortArgument, std::string usage);
	~FlagArgument() = default;

	mstch::map render() const override;
	bool hasParameters() const override;
	size_t paramStrLength() const override;
};

class ParameterArgument : public Argument {
public:
	ParameterArgument(std::string argument, std::optional<char> shortArgument,
	                  std::vector<std::string> parameters, std::string usage);

	mstch::map render() const override;
	bool hasParameters() const override;
	size_t paramStrLength() const override;

public:
	std::vector<std::string> parameters;
};

struct ArgumentComparator {
	bool operator()(const std::shared_ptr<Argument>& left,
	                const std::shared_ptr<Argument>& right) const;
};

