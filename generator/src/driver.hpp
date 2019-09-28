#pragma once

#include "argument.hpp"
#include "parser.tab.hh"
#include "scanner.hpp"
#include "usage.hpp"
#include <iostream>
#include <memory>
#include <set>
#include <string>

namespace Grammar {
class Driver {
public:
  Driver();
  virtual ~Driver();

  void parse(const char *const filename);
  void parse(std::istream &iss);

  /*
   * functions to store data retrieved by grammar / parser
   */

  void setProgramName(std::string programName);
  void setVersion(std::string version);
  void setLicense(std::string license);
  void setHelpAddendum(std::string addendum);
  void addUsage(Usage usage);
  void addRule(std::string ruleName, std::vector<std::string> options);
  void addArg(std::unique_ptr<Argument> argument);

  mstch::map getContext() const;
  std::string getSafeName() const;

private:
  void parse_helper(std::istream &iss);

  Grammar::Parser *parser = nullptr;
  Grammar::Scanner *scanner = nullptr;

  bool usesAnyParameters() const;
  mstch::array generateArgumentTokens() const;
  mstch::map generateHelpToken() const;
  mstch::map generateVersionToken() const;
  mstch::map generateLicenseToken() const;
  mstch::array generateArgumentExplanation() const;
  mstch::array generateUsageList() const;
  mstch::array generateUsageRuleList() const;
  mstch::node getHelpAddendum() const;

  static std::string explainRule(const std::string &ruleName,
                                 const std::vector<std::string> &ruleOptions);
  static std::string spaceN(size_t spaceCount);
  mstch::map alignArg(Argument &arg) const;

  struct ArgumentComparator {
    bool operator()(const std::unique_ptr<Argument> &left,
                    const std::unique_ptr<Argument> &right) const;
  };

  /*
   * specialised data store
   */

  friend Argument;
  std::string programName, version, license;
  std::set<std::unique_ptr<Argument>, ArgumentComparator> arguments;
  std::optional<std::string> helpAddendum;
  std::vector<Usage> usages;
  std::vector<std::pair<std::string, std::vector<std::string>>> rules;
  size_t maxArgLength, maxParamLength;
};
} // namespace Grammar
