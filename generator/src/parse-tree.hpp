#pragma once
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include <mstch/mstch.hpp>

namespace Grammar {
	class UsageArgument {
	public:
		virtual ~UsageArgument() = default;

		virtual std::string toStr() const = 0;
		virtual std::string cleanToken() const;

		virtual bool operator<(const UsageArgument& other) const;
		virtual bool operator==(const UsageArgument& other) const;
		virtual bool operator>(const UsageArgument& other) const;
	};

	class NonPositionalUsageArgument : public UsageArgument {
	public:
		NonPositionalUsageArgument(const std::string& argument);
		virtual ~NonPositionalUsageArgument() = default;

		virtual std::string toStr() const override;
		virtual std::string cleanToken() const override;

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

	class DerivedUsageArgument : public UsageArgument {
	public:
		DerivedUsageArgument(const std::string& rule);
		virtual ~DerivedUsageArgument() = default;

		virtual std::string toStr() const override;

	protected:
		std::string rule;
	};

	struct Usage {
		std::vector<std::shared_ptr<UsageArgument>> arguments;
	};

	struct UsageComparator {
		bool operator()(const std::shared_ptr<UsageArgument>& left,
		                const std::shared_ptr<UsageArgument>& right) const;
	};

	class Argument {
	public:
		Argument(std::string argument, std::optional<char> shortArgument,
		         std::string usage);
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
		FlagArgument(std::string argument, std::optional<char> shortArgument,
		             std::string usage);
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

	class RuleAlternation {
	public:
		RuleAlternation() = default;
		virtual ~RuleAlternation() = default;

		virtual std::string toStr() const = 0;
		virtual std::string cleanToken() const;
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
		virtual std::string cleanToken() const override;

	protected:
		std::string argument;
	};

	class Driver;

	class ParseTree {
	public:
		ParseTree();
		virtual ~ParseTree() = default;

		std::string getProgramName() const;
		std::string getVersion() const;
		std::string getLicense() const;
		std::string getHelp() const;

		std::vector<Usage> getUsages() const;
		std::map<std::string, std::vector<std::shared_ptr<RuleAlternation>>>
		getRules() const;
		std::set<std::shared_ptr<Argument>, ArgumentComparator>
		getArguments() const;

	protected:
		std::string programName, version, license;
		std::set<std::shared_ptr<Argument>, ArgumentComparator> arguments;
		std::optional<std::string> helpAddendum;
		std::vector<Usage> usages;
		std::map<std::string, std::vector<std::shared_ptr<RuleAlternation>>> rules;
		size_t maxArgLength, maxParamLength;

		friend class Driver;
	};
}; // namespace Grammar
