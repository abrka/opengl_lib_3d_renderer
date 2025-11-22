#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>

namespace GLUtils {
	std::optional<std::string> read_string_from_filepath(std::filesystem::path filepath) {
		std::ifstream file{ filepath };
		if (!file.is_open()) {
			return std::nullopt;
		}
		std::stringstream stream;
		stream << file.rdbuf();
		return stream.str();
	}
}