#pragma once
#include <cstdint>
#include <string>

#define PREFIX(filename) _binary_templates_ArgGrammar##filename
#define DECLARE_TEMPLATE_FILE(filename)                                        \
  extern const char PREFIX(filename##_start);                                  \
  extern const char PREFIX(filename##_end);

DECLARE_TEMPLATE_FILE(Driver_cpp)
DECLARE_TEMPLATE_FILE(Driver_hpp)
DECLARE_TEMPLATE_FILE(Scanner_ll)
DECLARE_TEMPLATE_FILE(Parser_yy)
DECLARE_TEMPLATE_FILE(Scanner_cpp)
DECLARE_TEMPLATE_FILE(Scanner_hpp)

struct TemplateFile {
  std::string symbolName;
  std::string_view contents;
};

#define STRING_TEMPLATE(filename)                                              \
  std::string_view(&PREFIX(filename##_start),                                  \
                   &PREFIX(filename##_end) - &PREFIX(filename##_start))

#define FILE_TEMPLATE(filename)                                                \
  TemplateFile { "ArgGrammar" #filename, STRING_TEMPLATE(filename) }

const TemplateFile templateFiles[] = {
    FILE_TEMPLATE(Driver_cpp),  FILE_TEMPLATE(Driver_hpp),
    FILE_TEMPLATE(Scanner_ll),  FILE_TEMPLATE(Parser_yy),
    FILE_TEMPLATE(Scanner_cpp), FILE_TEMPLATE(Scanner_hpp)};
