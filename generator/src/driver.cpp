#include <cassert>
#include <fstream>
#include <numeric>

#include "driver.hpp"

Grammar::Driver::Driver()
    : arguments{ ArgumentComparator{} }, maxArgLength{ 0 }, maxParamLength{ 0 } {
	addArg(std::make_shared<FlagArgument>("help", std::nullopt, "shows this help message"));
	addArg(std::make_shared<FlagArgument>("version", std::nullopt,
	                                      "shows the version of this software"));
	addArg(std::make_shared<FlagArgument>("license", std::nullopt,
	                                      "shows the license of this software"));
}

Grammar::Driver::~Driver() {
	delete (scanner);
	delete (parser);
}

void Grammar::Driver::parse(const char* const filename) {
	assert(filename != nullptr);
	std::ifstream iss{ filename };

	if (!iss)
		exit(EXIT_FAILURE);

	parse_helper(iss);
}

void Grammar::Driver::parse(std::istream& iss) {
	if (!iss.good() && iss.eof())
		return;

	parse_helper(iss);
}

void Grammar::Driver::parse_helper(std::istream& iss) {
	delete scanner;
	try {
		scanner = new Grammar::Scanner(&iss);
		// scanner->set_debug(1);
	} catch (const std::bad_alloc& ba) {
		std::cerr << "Failed to allocate scanner: \"" << ba.what() << "\". Exiting!\n";
		exit(EXIT_FAILURE);
	}

	delete parser;
	try {
		parser = new Grammar::Parser(*scanner, *this);
		// parser->set_debug_level(1);
	} catch (const std::bad_alloc& ba) {
		std::cerr << "Failed to allocate parser: \"" << ba.what() << "\". Exiting!\n";
		exit(EXIT_FAILURE);
	}

	if (parser->parse() != 0) {
		std::cerr << "Parsing failure!\n";
	}
	return;
}

void Grammar::Driver::setProgramName(std::string programName) {
	this->programName = programName;
}

void Grammar::Driver::setVersion(std::string version) {
	this->version = version;
}

void Grammar::Driver::setLicense(std::string license) {
	this->license = license;
}

void Grammar::Driver::setHelpAddendum(std::string addendum) {
	this->helpAddendum = addendum;
}

void Grammar::Driver::addArg(std::shared_ptr<Argument> argument) {
	maxArgLength = std::max(maxArgLength, argument->argStrLength());
	maxParamLength = std::max(maxParamLength, argument->paramStrLength());
	const auto prevVal = arguments.find(argument);

	if (prevVal != arguments.end()) {
		arguments.erase(prevVal);
	}

	arguments.insert(std::move(argument));
}

void Grammar::Driver::addUsage(Usage usage) {
	usages.push_back(usage);
}

void Grammar::Driver::addRule(std::string ruleName, std::vector<std::string> options) {
	rules.insert({ ruleName, options });
}

mstch::map Grammar::Driver::getContext() const {
	return mstch::map{ { "argspec", getSafeName() },
		                 { "any_parameters", usesAnyParameters() },
		                 { "argument_tokens", generateArgumentTokens() },
		                 { "argument_explanations", generateArgumentExplanation() },
		                 { "usage", generateUsageList() },
		                 { "positional_arguments", generatePositionalList() },
		                 { "usage_rules", generateUsageRuleList() },
		                 { "version", version },
		                 { "license", license },
		                 { "help_addendum", getHelpAddendum() } };
}

std::string Grammar::Driver::getSafeName() const {
	std::string safeName{ programName };
	safeName.erase(std::remove_if(safeName.begin(), safeName.end(),
	                              [](char letter) { return !isalpha(letter); }),
	               safeName.end());

	return safeName;
}

bool Grammar::Driver::usesAnyParameters() const {
	for (auto& argument : arguments) {
		if (argument->hasParameters())
			return true;
	}

	return false;
}

bool Grammar::Driver::usesAnyPositionalArguments() const {
	for (const auto& usage : usages) {
		for (const auto& argument : usage.arguments) {
			if (std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
				return true;
			}
		}
	}

	return false;
}

mstch::array Grammar::Driver::generateArgumentTokens() const {
	mstch::array tokens{};

	for (auto& argument : arguments) {
		tokens.push_back(alignArg(*argument));
	}

	return tokens;
}

mstch::array Grammar::Driver::generateArgumentExplanation() const {
	mstch::array argumentExplanations{};

	for (const auto& rule : rules) {
		argumentExplanations.push_back(explainRule(rule.first, rule.second));
	}

	return argumentExplanations;
}

mstch::array Grammar::Driver::generateUsageList() const {
	mstch::array usageList{};

	for (const auto& usage : usages) {
		mstch::array flags{};
		mstch::array positional{};

		for (const auto& argument : usage.arguments) {
			if (std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
				flags.push_back(argument->toStr());
			} else {
				positional.push_back(argument->toStr());
			}
		}

		usageList.push_back(mstch::map{ { "flags", flags }, { "positional", positional } });
	}

	return usageList;
}

mstch::array Grammar::Driver::generatePositionalList() const {
	std::set<std::string> positionalArgumentsSet{};

	for (const auto& usage : usages) {
		for (const auto& argument : usage.arguments) {
			if (std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
				positionalArgumentsSet.insert(argument->cleanToken());
			}
		}
	}

	return mstch::array{ positionalArgumentsSet.begin(), positionalArgumentsSet.end() };
}

mstch::array Grammar::Driver::generateUsageRuleList() const {
	mstch::array usageRuleList{};

	for (const auto& rulePair : rules) {
		mstch::map usageRule{ { "rule_name", Argument::cleanToken(rulePair.first) } };

		mstch::array ruleOptions{};

		for (size_t i = 0; i < rulePair.second.size(); i++) {
			mstch::map ruleOption{ { "option", Argument::cleanToken(rulePair.second[i]) } };

			if (i + 1 < rulePair.second.size())
				ruleOption.insert({ "has_next", true });

			ruleOptions.push_back(ruleOption);
		}

		usageRule.insert({ "options", ruleOptions });
		usageRuleList.push_back(usageRule);
	}

	return usageRuleList;
}

mstch::node Grammar::Driver::getHelpAddendum() const {
	return helpAddendum ? mstch::node{ *helpAddendum } : mstch::node{ false };
}

std::string Grammar::Driver::explainRule(const std::string& ruleName,
                                         const std::vector<std::string>& ruleOptions) {
	return ruleName + " can be " +
	       std::accumulate(std::next(ruleOptions.begin()), ruleOptions.end(), "--" + ruleOptions[0],
	                       [](const std::string& left, const std::string& right) {
		                       return left + " or --" + right;
	                       });
}

std::string Grammar::Driver::spaceN(size_t spaceCount) {
	std::string spaces(spaceCount, ' ');
	return spaces;
}

mstch::map Grammar::Driver::alignArg(Argument& arg) const {
	auto result = arg.render();
	result.insert({ "parameter_align_spacing", spaceN(maxArgLength - arg.argStrLength()) });
	result.insert({ "explain_align_spacing", spaceN(maxParamLength - arg.paramStrLength()) });
	return result;
}

std::string Grammar::Driver::getProgramName() const {
	return programName;
}

std::string Grammar::Driver::getVersion() const {
	return version;
}

std::string Grammar::Driver::getLicense() const {
	return license;
}

std::string Grammar::Driver::getHelp() const {
	return helpAddendum.value_or("");
}

std::vector<Usage> Grammar::Driver::getUsages() const {
	return usages;
}

std::map<std::string, std::vector<std::string>> Grammar::Driver::getRules() const {
	return rules;
}

std::set<std::shared_ptr<Argument>, ArgumentComparator> Grammar::Driver::getArguments() const {
	return arguments;
}
