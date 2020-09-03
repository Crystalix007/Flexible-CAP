#include "debugJSON.hpp"
#include <mstch/mstch.hpp>
#include <sstream>

namespace DebugPrinter {
	class JSONVisitor : public boost::static_visitor<std::string> {
	public:
		std::string operator()(const std::nullptr_t&) const;
		std::string operator()(const mstch::array& arr) const;
		std::string operator()(const mstch::map& map) const;
		std::string operator()(const int& value) const;
		std::string operator()(const double& value) const;
		std::string operator()(const bool& value) const;
		std::string operator()(const std::string& str) const;
		std::string operator()(const std::shared_ptr<mstch::object>& ptr) const;
		std::string operator()(const mstch::lambda& ptr) const;
	};

	std::string printJSON(const mstch::map& context) {
		JSONVisitor visitor{};

		return visitor(context);
	}

	std::string JSONVisitor::operator()(const std::nullptr_t&) const {
		return "null";
	}

	std::string JSONVisitor::operator()(const mstch::map& map) const {
		std::string json{ "{" };

		for (const auto& [key, val] : map) {
			json += "\"" + key + "\":";
			json += val.apply_visitor(*this) + ",";
		}

		json += "}";

		return json;
	}

	std::string JSONVisitor::operator()(const mstch::array& arr) const {
		std::string json{ '[' };

		for (const auto& elem : arr) {
			json += elem.apply_visitor(*this) + ',';
		}

		json += ']';

		return json;
	}

	std::string JSONVisitor::operator()(const int& value) const {
    return std::to_string(value);
  }

  std::string JSONVisitor::operator()(const double& value) const {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }

  std::string JSONVisitor::operator()(const bool& value) const {
    return value ? "true" : "false";
  }

	std::string JSONVisitor::operator()(const std::string& str) const {
		return "\"" + str + "\"";
	}

	std::string JSONVisitor::operator()(const std::shared_ptr<mstch::object>& ptr) const {
		return "[object]";
	}

	std::string JSONVisitor::operator()(const mstch::lambda& ptr) const {
		return "[func]";
	}
};
