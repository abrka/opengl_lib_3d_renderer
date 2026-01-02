#pragma once

#include <filesystem>

namespace Engine {
	struct MeshInfoComponent {
		std::filesystem::path filepath{};
		bool requires_reload{ true };
	};
}