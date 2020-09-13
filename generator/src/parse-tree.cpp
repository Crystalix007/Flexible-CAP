#include "parse-tree.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>

namespace Grammar {
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

	NonPositionalUsageArgument::NonPositionalUsageArgument(
	    const std::string& argument)
	    : arg{ argument } {}

	std::string NonPositionalUsageArgument::toStr() const {
		return "--" + this->arg;
	}

	std::string NonPositionalUsageArgument::cleanToken() const {
		return Argument::cleanToken(this->arg);
	}

	PositionalUsageArgument::PositionalUsageArgument(const std::string& argument)
	    : arg{ argument } {}

	std::string PositionalUsageArgument::toStr() const {
		return this->arg;
	}

	DerivedUsageArgument::DerivedUsageArgument(const std::string& rule)
	    : rule{ rule } {}

	std::string DerivedUsageArgument::toStr() const {
		return this->rule;
	}

	bool UsageComparator::operator()(
	    const std::shared_ptr<UsageArgument>& left,
	    const std::shared_ptr<UsageArgument>& right) const {
		return left->cleanToken() < right->cleanToken();
	}

	Argument::Argument(std::string argument, std::optional<char> shortArgument,
	                   std::string usage)
	    : argument{ argument }, usage{ usage }, shortArgument{ shortArgument } {}

	Argument::~Argument() {}

	mstch::map Argument::render() const {
		mstch::map result{
			{ std::string{ "argument" }, argument },
			{ "usage", usage },
			{ "short_argument", shortArgument
			                        ? mstch::node{ std::string{ *shortArgument } }
			                        : mstch::node{ false } },
			{ "clean_token", cleanToken() },
			{ "has_parameters", hasParameters() },
		};

		return result;
	}

	std::string Argument::cleanToken(const std::string& token) {
		std::string result(token.size(), static_cast<char>(NULL));
		std::transform(token.begin(), token.end(), result.begin(),
		               [](char character) {
			               if (isalpha(character) || isdigit(character))
				               return character;
			               else
				               return '_';
		               });

		return result;
	}

	std::string Argument::cleanToken() const {
		return cleanToken(argument);
	}

	bool Argument::operator==(const Argument& other) const {
		return this->argument == other.argument;
	}

	bool Argument::operator<(const Argument& other) const {
		return this->argument < other.argument;
	}

	bool Argument::operator>(const Argument& other) const {
		return this->argument > other.argument;
	}

	size_t Argument::argStrLength() const {
		return argument.size();
	}

	FlagArgument::FlagArgument(std::string argument,
	                           std::optional<char> shortArgument,
	                           std::string usage)
	    : Argument{ argument, shortArgument, usage } {}

	mstch::map FlagArgument::render() const {
		auto result = Argument::render();
		result.insert({ "parameters", mstch::array{} });
		return result;
	}

	bool FlagArgument::hasParameters() const {
		return false;
	}

	size_t FlagArgument::paramStrLength() const {
		return 0;
	}

	ParameterArgument::ParameterArgument(std::string argument,
	                                     std::optional<char> shortArgument,
	                                     std::vector<std::string> parameters,
	                                     std::string usage)
	    : Argument{ argument, shortArgument, usage }, parameters{ parameters } {}

	mstch::map ParameterArgument::render() const {
		auto result = Argument::render();
		mstch::array resultParameters{};

		for (int i = 0; i < parameters.size(); i++) {
			mstch::map resultParameter{ { "index", i + 1 },
				                          { "name", parameters[i] } };

			if (i + 1 < parameters.size())
				resultParameter.insert({ "next", i + 2 });

			resultParameters.push_back(resultParameter);
		}

		result.insert({ "parameters", resultParameters });
		return result;
	}

	bool ParameterArgument::hasParameters() const {
		return true;
	}

	size_t ParameterArgument::paramStrLength() const {
		size_t totalSize =
		    std::accumulate(parameters.begin(), parameters.end(), 0,
		                    [](size_t count, const std::string& str) {
			                    return count + str.size();
		                    });

		return totalSize + (parameters.size() - 1);
	}

	bool
	ArgumentComparator::operator()(const std::shared_ptr<Argument>& left,
	                               const std::shared_ptr<Argument>& right) const {
		return *left < *right;
	}

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

	ParseTree::ParseTree()
	    : arguments{ ArgumentComparator{} }, maxArgLength{ 0 }, maxParamLength{
		      0
	      } {}

	std::string ParseTree::getProgramName() const {
		return programName;
	}

	std::string ParseTree::getVersion() const {
		return version;
	}

	std::string ParseTree::getLicense() const {
		return license;
	}

	std::string ParseTree::getHelp() const {
		return helpAddendum.value_or("");
	}

	std::vector<Usage> ParseTree::getUsages() const {
		return usages;
	}

	std::map<std::string, std::vector<std::shared_ptr<RuleAlternation>>> ParseTree::getRules() const {
		return rules;
	}

	std::set<std::shared_ptr<Argument>, ArgumentComparator> ParseTree::getArguments() const {
		return arguments;
	}
} // namespace Grammar
