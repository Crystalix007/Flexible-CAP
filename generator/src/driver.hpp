#pragma once

#include "argument.hpp"
#include "parser.tab.hh"
#include "scanner.hpp"
#include "usage.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>

namespace Grammar {
	class PrettyPrinter;

	class Driver {
	public:
		Driver();
		virtual ~Driver();

		void parse(const char* const filename);
		void parse(std::istream& iss);

		/*
		 * functions to store data retrieved by grammar / parser
		 */

		void setProgramName(std::string programName);
		void setVersion(std::string version);
		void setLicense(std::string license);
		void setHelpAddendum(std::string addendum);
		void addUsage(Usage usage);
		void addRule(std::string ruleName, std::vector<std::string> options);
		void addArg(std::shared_ptr<Argument> argument);

		mstch::map getContext() const;
		std::string getSafeName() const;

	protected:
		std::string getProgramName() const;
		std::string getVersion() const;
		std::string getLicense() const;
		std::string getHelp() const;

		std::vector<Usage> getUsages() const;
		std::map<std::string, std::vector<std::string>> getRules() const;
		std::set<std::shared_ptr<Argument>, ArgumentComparator>
		getArguments() const;

	private:
		void parse_helper(std::istream& iss);

		Grammar::Parser* parser = nullptr;
		Grammar::Scanner* scanner = nullptr;

		bool usesAnyParameters() const;
		bool usesAnyPositionalArguments() const;
		mstch::array generateArgumentTokens() const;
		mstch::map generateHelpToken() const;
		mstch::map generateVersionToken() const;
		mstch::map generateLicenseToken() const;
		mstch::array generateArgumentExplanation() const;
		mstch::array generateUsageList() const;
		mstch::array generateUsageRuleList() const;
		mstch::array generatePositionalList() const;
		mstch::node getHelpAddendum() const;

		static std::string explainRule(const std::string& ruleName,
		                               const std::vector<std::string>& ruleOptions);
		static std::string spaceN(size_t spaceCount);
		mstch::map alignArg(Argument& arg) const;

		/*
		 * specialised data store
		 */

		std::string programName, version, license;
		std::set<std::shared_ptr<Argument>, ArgumentComparator> arguments;
		std::optional<std::string> helpAddendum;
		std::vector<Usage> usages;
		std::map<std::string, std::vector<std::string>> rules;
		size_t maxArgLength, maxParamLength;

		friend class Argument;
		friend class PrettyPrinter;
	};
} // namespace Grammar
