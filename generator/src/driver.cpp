#include <cassert>
#include <fstream>
#include <numeric>

#include "driver.hpp"

namespace Grammar {
	Driver::Driver() {
		addArg(std::make_shared<FlagArgument>("help", std::nullopt,
		                                      "shows this help message"));
		addArg(std::make_shared<FlagArgument>(
		    "version", std::nullopt, "shows the version of this software"));
		addArg(std::make_shared<FlagArgument>(
		    "license", std::nullopt, "shows the license of this software"));
	}

	Driver::~Driver() {
		delete (scanner);
		delete (parser);
	}

	void Driver::parse(const char* const filename) {
		assert(filename != nullptr);
		std::ifstream iss{ filename };

		if (!iss)
			exit(EXIT_FAILURE);

		parse_helper(iss);
	}

	void Driver::parse(std::istream& iss) {
		if (!iss.good() && iss.eof())
			return;

		parse_helper(iss);
	}

	void Driver::parse_helper(std::istream& iss) {
		delete scanner;
		try {
			scanner = new Scanner(&iss);
			// scanner->set_debug(1);
		} catch (const std::bad_alloc& ba) {
			std::cerr << "Failed to allocate scanner: \"" << ba.what()
			          << "\". Exiting!\n";
			exit(EXIT_FAILURE);
		}

		delete parser;
		try {
			parser = new Parser(*scanner, *this);
			// parser->set_debug_level(1);
		} catch (const std::bad_alloc& ba) {
			std::cerr << "Failed to allocate parser: \"" << ba.what()
			          << "\". Exiting!\n";
			exit(EXIT_FAILURE);
		}

		if (parser->parse() != 0) {
			std::cerr << "Parsing failure!\n";
		}
		return;
	}

	void Driver::setProgramName(std::string programName) {
		parseTree.programName = programName;
	}

	void Driver::setVersion(std::string version) {
		parseTree.version = version;
	}

	void Driver::setLicense(std::string license) {
		parseTree.license = license;
	}

	void Driver::setHelpAddendum(std::string addendum) {
		parseTree.helpAddendum = addendum;
	}

	void Driver::addArg(std::shared_ptr<Argument> argument) {
		parseTree.maxArgLength =
		    std::max(parseTree.maxArgLength, argument->argStrLength());
		parseTree.maxParamLength =
		    std::max(parseTree.maxParamLength, argument->paramStrLength());
		const auto prevVal = parseTree.arguments.find(argument);

		if (prevVal != parseTree.arguments.end()) {
			parseTree.arguments.erase(prevVal);
		}

		parseTree.arguments.insert(std::move(argument));
	}

	void Driver::addUsage(Usage usage) {
		parseTree.usages.push_back(usage);
	}

	void Driver::addRule(std::string ruleName,
	                     std::vector<std::shared_ptr<RuleAlternation>> options) {
		parseTree.rules.insert({ ruleName, options });
	}

	mstch::map Driver::getContext() const {
		return mstch::map{
			{ "argspec", getSafeName() },
			{ "any_parameters", usesAnyParameters() },
			{ "any_positional_arguments", usesAnyPositionalArguments() },
			{ "argument_tokens", generateArgumentTokens() },
			{ "argument_explanations", generateArgumentExplanation() },
			{ "usage", generateUsageList() },
			{ "positional_arguments", generatePositionalList() },
			{ "usage_rules", generateUsageRuleList() },
			{ "version", parseTree.getVersion() },
			{ "license", parseTree.getLicense() },
			{ "help_addendum", getHelpAddendum() }
		};
	}

	std::string Driver::getSafeName() const {
		std::string safeName{ parseTree.getProgramName() };
		safeName.erase(std::remove_if(safeName.begin(), safeName.end(),
		                              [](char letter) { return !isalpha(letter); }),
		               safeName.end());

		return safeName;
	}

	bool Driver::usesAnyParameters() const {
		for (auto& argument : parseTree.getArguments()) {
			if (argument->hasParameters())
				return true;
		}

		return false;
	}

	bool Driver::usesAnyPositionalArguments() const {
		for (const auto& usage : parseTree.getUsages()) {
			for (const auto& argument : usage.arguments) {
				if (std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
					return true;
				}
			}
		}

		return false;
	}

	mstch::array Driver::generateArgumentTokens() const {
		mstch::array tokens{};

		for (auto& argument : parseTree.getArguments()) {
			tokens.push_back(alignArg(*argument));
		}

		return tokens;
	}

	mstch::array Driver::generateArgumentExplanation() const {
		mstch::array argumentExplanations{};

		for (const auto& rule : parseTree.getRules()) {
			argumentExplanations.push_back(explainRule(rule.first, rule.second));
		}

		return argumentExplanations;
	}

	mstch::array Driver::generateUsageList() const {
		mstch::array usageList{};

		for (const auto& usage : parseTree.getUsages()) {
			mstch::array arguments{};

			for (const auto& argument : usage.arguments) {
				if (std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
					arguments.push_back(mstch::map{
					    { "clean_token", argument->cleanToken() },
					    { "positional", argument->toStr() },
					    { "flag", false },
					});
				} else {
					arguments.push_back(mstch::map{
					    { "clean_token", argument->cleanToken() },
					    { "positional", false },
					    { "flag", argument->toStr() },
					});
				}
			}

			usageList.push_back(mstch::map{ { "arguments", arguments } });
		}

		return usageList;
	}

	mstch::array Driver::generatePositionalList() const {
		std::set<std::shared_ptr<PositionalUsageArgument>, UsageComparator>
		    positionalArgumentsSet{};

		for (const auto& usage : parseTree.getUsages()) {
			for (const auto& argument : usage.arguments) {
				if (const auto posArg =
				        std::dynamic_pointer_cast<PositionalUsageArgument>(argument)) {
					positionalArgumentsSet.insert(posArg);
				}
			}
		}

		mstch::array positionalArgumentsArray{};

		for (const auto& posArg : positionalArgumentsSet) {
			positionalArgumentsArray.push_back(
			    mstch::map{ { "clean_token", posArg->cleanToken() } });
		}

		return positionalArgumentsArray;
	}

	mstch::array Driver::generateUsageRuleList() const {
		mstch::array usageRuleList{};

		for (const auto& rulePair : parseTree.getRules()) {
			mstch::map usageRule{ { "rule_name",
				                      Argument::cleanToken(rulePair.first) } };

			mstch::array ruleOptions{};

			for (size_t i = 0; i < rulePair.second.size(); i++) {
				mstch::map ruleOption{ { "option", rulePair.second[i]->cleanToken() } };

				if (i + 1 < rulePair.second.size())
					ruleOption.insert({ "has_next", true });

				ruleOptions.push_back(ruleOption);
			}

			usageRule.insert({ "options", ruleOptions });
			usageRuleList.push_back(usageRule);
		}

		return usageRuleList;
	}

	mstch::node Driver::getHelpAddendum() const {
		const auto& helpAddendum = parseTree.helpAddendum;
		return helpAddendum ? mstch::node{ *helpAddendum } : mstch::node{ false };
	}

	std::string Driver::explainRule(
	    const std::string& ruleName,
	    const std::vector<std::shared_ptr<RuleAlternation>>& ruleOptions) {
		return ruleName + " can be " +
		       std::accumulate(std::next(ruleOptions.begin()), ruleOptions.end(),
		                       ruleOptions[0]->toStr(),
		                       [](const std::string& left,
		                          const std::shared_ptr<RuleAlternation>& right) {
			                       return left + " or " + right->toStr();
		                       });
	}

	std::string Driver::spaceN(size_t spaceCount) {
		std::string spaces(spaceCount, ' ');
		return spaces;
	}

	mstch::map Driver::alignArg(Argument& arg) const {
		auto result = arg.render();
		result.insert({ "parameter_align_spacing",
		                spaceN(parseTree.maxArgLength - arg.argStrLength()) });
		result.insert({ "explain_align_spacing",
		                spaceN(parseTree.maxParamLength - arg.paramStrLength()) });
		return result;
	}

	ParseTree Driver::getParseTree() const {
		return this->parseTree;
	}
} // namespace Grammar
