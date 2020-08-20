#include "argument.hpp"
#include <numeric>

Argument::Argument(std::string argument, std::optional<char> shortArgument, std::string usage)
    : argument{ argument }, usage{ usage }, shortArgument{ shortArgument } {}

Argument::~Argument() {}

mstch::map Argument::render() const {
	mstch::map result{
		{ std::string{ "argument" }, argument },
		{ "usage", usage },
		{ "short_argument",
		  shortArgument ? mstch::node{ std::string{ *shortArgument } } : mstch::node{ false } },
		{ "clean_token", cleanToken() },
		{ "has_parameters", hasParameters() },
	};

	return result;
}

std::string Argument::cleanToken(const std::string& token) {
	std::string result(token.size(), static_cast<char>(NULL));
	std::transform(token.begin(), token.end(), result.begin(), [](char character) {
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

#include <iostream>

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

FlagArgument::FlagArgument(std::string argument, std::optional<char> shortArgument,
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

ParameterArgument::ParameterArgument(std::string argument, std::optional<char> shortArgument,
                                     std::vector<std::string> parameters, std::string usage)
    : Argument{ argument, shortArgument, usage }, parameters{ parameters } {}

mstch::map ParameterArgument::render() const {
	auto result = Argument::render();
	mstch::array resultParameters{};

	for (int i = 0; i < parameters.size(); i++) {
		mstch::map resultParameter{ { "index", i + 1 }, { "name", parameters[i] } };

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
	                    [](size_t count, const std::string& str) { return count + str.size(); });

	return totalSize + (parameters.size() - 1);
}
