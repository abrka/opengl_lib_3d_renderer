#pragma once

#include <filesystem>

namespace Engine {
	struct ShaderInfoComponent {
		std::filesystem::path vertex_filepath{};
		std::filesystem::path fragment_filepath{};
	};
}