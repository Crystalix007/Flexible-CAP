#pragma once

#include "driver.hpp"

namespace Grammar {
class PrettyPrinter {
	public:
		PrettyPrinter(const Driver& driver);
		virtual ~PrettyPrinter() = default;
};
} // namespace Grammar
