#include "driver.hpp"
#include "templates.hpp"
#include <fstream>
#include <iostream>
#include <mstch/mstch.hpp>

std::string symbolNameToOutputFile(const std::string &symbolName);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename-to-parse>\n";
    return 1;
  }

  Grammar::Driver driver{};
  std::ifstream specFile{argv[1]};
  driver.parse(specFile);
  const auto context = driver.getContext();

  for (const auto &templateFile : templateFiles) {
    std::string strTemplate{templateFile.contents};
    std::ofstream outputFile{driver.getSafeName() +
                             symbolNameToOutputFile(templateFile.symbolName)};

    outputFile << mstch::render(strTemplate, context);
    outputFile.close();
  }

  return 0;
}

std::string symbolNameToOutputFile(const std::string &symbolName) {
  std::string outputFileName(symbolName.size(), static_cast<char>(NULL));
  std::transform(symbolName.begin(), symbolName.end(), outputFileName.begin(),
                 [](char letter) {
                   if (letter == '_')
                     return '.';
                   return letter;
                 });

  return outputFileName;
}
